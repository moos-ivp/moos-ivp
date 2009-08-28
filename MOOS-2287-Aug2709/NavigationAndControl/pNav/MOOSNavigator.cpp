///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite
//
//   A suit of Applications and Libraries for Mobile Robotics Research
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and
//   Oxford University.
//
//   This software was written by Paul Newman and others
//   at MIT 2001-2002 and Oxford University 2003-2005.
//   email: pnewman@robots.ox.ac.uk.
//
//   This file is part of a  MOOS Basic (Common) Application.
//
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the GNU General Public License as
//   published by the Free Software Foundation; either version 2 of the
//   License, or (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
//   General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//   02111-1307, USA.
//
//////////////////////////    END_GPL    //////////////////////////////////
///////////////////////////////////////////////////////////////////
//                                                               //
//          This file is part of MOOS Suite                      //
//            Copyright Paul Newman, September 2000                //
//                                                               //
///////////////////////////////////////////////////////////////////
//
//MOOSNavigator.cpp: implementation of the CMOOSNavigator class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include "../MOOSNavLib/MOOSNavLib.h"

#include "MOOSNavigator.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>

#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//no variables will be update at more than 10HZ
//however we don;t want to miss stuff like two consecuative
//LBL pings!!
#define DEFAULT_NAV_UPDATE_PERIOD 0.0

/*
ProcessConfig = pNav
{

  X        = SIM @ 5.0 GPS @ 2.0 , DVL @ 3.0 , LBL @ 5.0
  Y        = GPS then DVL then LBL
  Z        = GPS then DVL then LBL
  Depth    = PARA then DVL then LBL
  Yaw        = CROSS_BOW then DVL

}*/
#define DEFAULT_LSQ_TIMEOUT 30
#define DEFAULT_MAX_EKF_LSQ_DEVIATION 20
#define DEFAULT_MAX_POSITION_UNCERTAINTY 30
#define DEFAULT_MAX_EKFLSQ_DISAGREEMENTS 6
#define DEFAULT_LSQ_SAMPLE_SIZE 8
#define DEFAULT_MAX_MEDIAN_LSQ_SHIFT 20

CMOOSNavigator::CFilterSafety::CFilterSafety()
{
    Initialise();
}

bool CMOOSNavigator::CFilterSafety::Initialise()
{
    m_nLastEKFDisagreeIteration = 0;
    m_nLastLSQDisagreeIteration = 0;

    m_DeltaLSQHistory.clear();

    m_dfLSQTimeOut = DEFAULT_LSQ_TIMEOUT;
    m_dfMaxEKFLSQDeviation = DEFAULT_MAX_EKF_LSQ_DEVIATION;
    m_dfMaxEKFPositionUncertainty = DEFAULT_MAX_POSITION_UNCERTAINTY;

    m_nLastEKFDisagreeIteration = 0;
    m_nLastLSQDisagreeIteration = 0;

    m_nEKFLSQDisagreements = 0;

    m_nForceEKFAfterNDisagreements = DEFAULT_MAX_EKFLSQ_DISAGREEMENTS;

    m_dfLastLSQX=0;
    m_dfLastLSQY=0;
    m_dfLastLSQZ=0;
    m_dfLastLSQH=0;

    m_nLSQSampleSize = DEFAULT_LSQ_SAMPLE_SIZE;
    m_dfMaxMedianLSQShift = DEFAULT_MAX_MEDIAN_LSQ_SHIFT;

    return true;
}


CMOOSNavigator::CMOOSNavigator()
{
    SetAppFreq(10);
    SetCommsFreq(20);

    m_pEKF=NULL;
    m_pLSQ=NULL;

}

CMOOSNavigator::~CMOOSNavigator()
{

}



bool CMOOSNavigator::OnNewMail(MOOSMSG_LIST &NewMail)
{

    //look for personal mail first.
    HandlePersonalMail(NewMail);

    CMOOSPriorityInput * Inputs[] = {&m_XInput,
        &m_YInput,
        &m_ZInput,
        &m_DepthInput,
        &m_YawInput,
        &m_AltitudeInput,
        &m_SpeedInput,
        &m_PitchInput,
        &m_PoseInput,
        &m_OdometryInput

    } ;


    MOOSMSG_LIST::iterator p;
    NAVENGINE_LIST::iterator q;
    for(p = NewMail.begin();p!=NewMail.end();p++)
    {
        CMOOSMsg &  rMsgIn = *p;

        //if we are in playback mode then don't worry about skew
        //this happens in the IsSkewed call
        if(rMsgIn.IsSkewed(MOOSTime()))
            continue;

        for(q=m_NavEngines.begin();q!=m_NavEngines.end();q++)
        {
            if((*q)->IsEnabled())
            {
                (*q)->AddData(rMsgIn);
            }
        }
        double dfTimeNow = GetTimeNow();
        for(unsigned int i = 0;i<sizeof(Inputs)/sizeof(Inputs[0]);i++)
        {

            Inputs[i]->SetInput(rMsgIn,dfTimeNow);
        }
    }




    return true;
}


/////////////////////////////////////////////
///this is where it all happens..
bool CMOOSNavigator::Iterate()
{
    NAVENGINE_LIST::iterator q;

    for(q=m_NavEngines.begin();q!=m_NavEngines.end();q++)
    {
        CMOOSNavEngine * pEngine = *q;

        if(pEngine->IsEnabled())
        {
            //figure out upto what time we want to
            //progress
            double dfTimeToIterateTo = GetTimeNow();

            pEngine->Iterate(dfTimeToIterateTo);

            CMOOSMsg ResultMsg;

            while(pEngine->GetNextResult(ResultMsg))
            {
                PRIORITYINPUT_LIST::iterator q;

                for(q = m_InputsList.begin();q!=m_InputsList.end();q++)
                {
                    //post to imput stacks.
                    //this makes it look as though data came from outside world
                    (*q)->SetInput(ResultMsg,ResultMsg.GetTime());

                }
                //also publish results of the filter.
                //code in MangeInputs prevents us from
                //subscribing to what we publish
                m_Comms.Post(ResultMsg);
            }
        }
    }

    MonitorFilters();

    PublishData();
    return true;
}


bool CMOOSNavigator::OnStartUp()
{
    Clean();
    return Initialise();
}

bool CMOOSNavigator::OnConnectToServer()
{
    return MakeSubscriptions();
}



bool CMOOSNavigator::ManageInputs()
{
    string sVal;
    STRING_LIST TmpList;


    struct InputSpec{
        CMOOSPriorityInput * m_pPriorityInput;
        const char * m_sConfigName;
        const char * m_sPublishName;
        const char* m_sStem;
    };

    InputSpec Array[] = {
        {&m_XInput,         "X",            "NAV_X",      		"X"},
        {&m_YInput,         "Y",            "NAV_Y",      		"Y"},
        {&m_ZInput,         "Z",            "NAV_Z",      		"Z"},
        {&m_YawInput,       "Yaw",          "NAV_YAW",    		"YAW"},
        {&m_SpeedInput,     "Speed",        "NAV_SPEED",    	"SPEED"},
        {&m_DepthInput,     "Depth",        "NAV_DEPTH",    	"DEPTH"},
        {&m_PitchInput,     "Pitch",        "NAV_PITCH",    	"PITCH"},
        {&m_AltitudeInput,  "Altitude",     "NAV_ALTITUDE",  	"ALTITUDE"},
        {&m_OdometryInput,  "Odometry",     "NAV_ODOMETRY",  	"ODOMETRY"},
        {&m_PoseInput,  	"Pose",     	"NAV_POSE",  		"POSE"},
        
    };

    for(unsigned int i = 0; i<sizeof(Array)/sizeof(Array[0]);i++)
    {
        InputSpec is = Array[i];

        MOOSTrace("setting up data routing table and priorities for %s\n",is.m_sPublishName);

        if(m_MissionReader.GetConfigurationParam(is.m_sConfigName,sVal))
        {
            is.m_pPriorityInput->Initialise(is.m_sPublishName,
                sVal,
                is.m_sStem,
                TmpList);

            m_SubScriptions.splice(m_SubScriptions.begin(),TmpList);
        }
    }

    //sometimes sensors supply data that we want that does not map to
    //state coordinates - eg LBL TOF...
    //this field is used for such occasions and to force the navigator
    //to subscribe todata thgat is not decalred in the priority queues
    string sAlways;
    if(m_MissionReader.GetConfigurationParam("ALWAYS_READ",sAlways))
    {
        while(!sAlways.empty())
        {
            m_SubScriptions.push_back(MOOSChomp(sAlways,","));
        }
    }


    //so we have build a long list of all the variable we want to subscribe to
    //now we tell the server to tell us when they change.
    STRING_LIST::iterator p;

    //but don;t subscribe to things with the stems
    //that we will be publishing
    set<string> DoNotSubscribe;
    DoNotSubscribe.insert("LSQ");
    DoNotSubscribe.insert("EKF");

    for(p = m_SubScriptions.begin();p!=m_SubScriptions.end();p++)
    {
        set<string>::iterator q;
        bool bOmit = false;
        for(q = DoNotSubscribe.begin();q!=DoNotSubscribe.end();q++)
        {
            //if we can find a banned stem in the SubScriptions
            //we shall skip this potential subscription
            string sBanned = *q;
            string sSubs = *p;
            if(p->find(*q)!=string::npos)
            {
                bOmit = true;
                break;
            }
        }

        if(!bOmit)
        {
            m_Comms.Register(*p,DEFAULT_NAV_UPDATE_PERIOD);
        }
    }





    //keep a STL list of our inputs for future use...
    m_InputsList.clear();
    m_InputsList.push_front(&m_XInput);
    m_InputsList.push_front(&m_YInput);
    m_InputsList.push_front(&m_ZInput);
    m_InputsList.push_front(&m_DepthInput);
    m_InputsList.push_front(&m_YawInput);
    m_InputsList.push_front(&m_AltitudeInput);
    m_InputsList.push_front(&m_SpeedInput);
    m_InputsList.push_front(&m_PitchInput);
    m_InputsList.push_front(&m_OdometryInput);
    m_InputsList.push_front(&m_PoseInput);


    //some additional susbcriptions
    m_Comms.Register("TOP_DOWN_CAL_CONTROL",0);

    return true;
}




bool CMOOSNavigator::PublishData()
{


    double dfTimeNow = GetTimeNow();

    PRIORITYINPUT_LIST::iterator q;

    ostringstream os;

    for(q = m_InputsList.begin();q!=m_InputsList.end();q++)
    {

        CMOOSMsg MsgOut;

        if((*q)->GetOutput(MsgOut,dfTimeNow))
        {
            m_Comms.Post(MsgOut);
        }

        double dfTime,dfVal;
        if((*q)->GetLastValue(dfTime,dfVal))
        {
            os<<(*q)->m_sName.c_str()<<"=";
            os<<dfVal;
            os<<"@"<<dfTime-GetAppStartTime();
            os<<",";
        }

    }

    os<<ends;

    //finally send a nav summary
    string sSummary = os.str();
    //   os.rdbuf()->freeze(0);
    if(!sSummary.empty())
    {
        m_Comms.Notify("NAV_SUMMARY",sSummary);
    }

    return true;
}


bool CMOOSNavigator::SetUpNavEngines()
{
    //firstly make the nav engines....

    if(!MakeNavEngines())
        return false;

    if(!AddSensorsToEngines())
        return false;

    if(!AddAcousticsToEngines())
        return false;

    if(!AddFixedObservations())
        return false;

    return true;
}

bool CMOOSNavigator::AddSensorsToEngines()
{

    //now Add sensors
    NAVENGINE_LIST::iterator p;

    for(p=m_NavEngines.begin();p!=m_NavEngines.end();p++)
    {
        CMOOSNavEngine * pEngine = *p;

        STRING_LIST sList;
        m_MissionReader.GetConfiguration(GetAppName(),sList);

        STRING_LIST::iterator q;

        for(q = sList.begin();q!=sList.end();q++)
        {
            string sLine = *q;
            string sTok,sVal;

            if(m_MissionReader.GetTokenValPair(sLine,sTok,sVal))
            {
                CMOOSNavSensor::Type eType=CMOOSNavSensor::INVALID;

                if(MOOSStrCmp(sTok,"SENSOR_XY"))
                {
                    eType = CMOOSNavSensor::XY;
                }
                else if(MOOSStrCmp(sTok,"SENSOR_ORIENTATION"))
                {
                    eType = CMOOSNavSensor::ORIENTATION;
                }
                else if(MOOSStrCmp(sTok,"SENSOR_DEPTH"))
                {
                    eType = CMOOSNavSensor::DEPTH;
                }
                else if(MOOSStrCmp(sTok,"SENSOR_LBL"))
                {
                    eType = CMOOSNavSensor::LBL;
                }
                else if(MOOSStrCmp(sTok,"SENSOR_ALTITUDE"))
                {
                    eType = CMOOSNavSensor::ALTITUDE;
                }
                else if(MOOSStrCmp(sTok,"SENSOR_BODY_VEL"))
                {
                    eType = CMOOSNavSensor::BODY_VEL;
                }
                else if(MOOSStrCmp(sTok,"SENSOR_CONTROL"))
                {
                    eType = CMOOSNavSensor::CONTROL;
                }

                if(eType!=CMOOSNavSensor::INVALID)
                {
                    string sSource = MOOSChomp(sVal,"->");

                    string sName = MOOSChomp(sVal,"@");

                    double dfX = atof(MOOSChomp(sVal,",").c_str());
                    double dfY = atof(MOOSChomp(sVal,",").c_str());
                    double dfZ = atof(sVal.c_str());

                    //now look for noise figure
                    MOOSChomp(sVal,"~");
                    double dfNoise=-1;
                    if(!sVal.empty())
                    {
                        dfNoise = atof(sVal.c_str());
                    }

                    if(!pEngine->AddSensor(sSource,sName,eType,dfX,dfY,dfZ,dfNoise))
                    {
                        MOOSTrace("Sensor Not added!\n");
                    }
                }
                else
                {
                    if(sTok.find("SENSOR_")!=string::npos)
                    {
                        MOOSTrace("Cannot add sensor of type %s - not supported\n",sTok.c_str());
                    }
                }
            }
        }
    }

    return true;
}

bool CMOOSNavigator::MakeNavEngines()
{
    m_pEKF = NULL;
    m_pLSQ = NULL;

    string sBool;
    if(m_MissionReader.GetConfigurationParam("USELSQ",sBool))
    {
        if(MOOSStrCmp("TRUE",sBool))
        {
            m_pLSQ = new CMOOSNavLSQEngine;
            m_NavEngines.push_back(m_pLSQ);
        }
    }
    if(m_MissionReader.GetConfigurationParam("USETOPDOWN",sBool))
    {
        if(MOOSStrCmp("TRUE",sBool))
        {
            m_NavEngines.push_back(new CMOOSNavTopDownCalEngine);
        }
    }
    if(m_MissionReader.GetConfigurationParam("USEEKF",sBool))
    {
        if(MOOSStrCmp("TRUE",sBool))
        {
            m_pEKF = new CMOOSNavEKFEngine;
            m_NavEngines.push_back(m_pEKF);
        }
    }

    //now initialise all the engines giving themchance to
    //pick out their own parameters
    NAVENGINE_LIST::iterator p;

    STRING_LIST sParams;
    m_MissionReader.GetConfiguration(GetAppName(),sParams);


    for(p=m_NavEngines.begin();p!=m_NavEngines.end();p++)
    {
        CMOOSNavEngine * pEngine = *p;

        pEngine->SetMissionFileName(this->m_sMissionFile);

        pEngine->Initialise(sParams);


    }

    if(m_pLSQ!=NULL)
    {
        //the LSQ starts imediately
        m_pLSQ->SetTimeStarted(MOOSTime());
    }

    //now we may want to run the EKF WITHOUT LSQ
    if(m_pEKF!=NULL && m_pLSQ==NULL)
    {
        //we turn the EKF on!
        m_pEKF->SetOnline(true);
        m_pEKF->Enable(true);
    }

    return true;
}

bool CMOOSNavigator::AddAcousticsToEngines()
{

    STRING_LIST sParams;

    if(!m_MissionReader.GetConfiguration("Acoustics",sParams))
        return true;

    sParams.reverse();

    STRING_LIST::iterator q;

    string sTok,sVal;

    for(q = sParams.begin();q!=sParams.end();q++)
    {
        string sLine = *q;
        MOOSToUpper(sLine);
        if(sLine.find("BEACON")!=string::npos)
        {

            //this is what we want to know
            double dfX;
            double dfY;
            double dfZ;
            int nChan;
            double dfTAT;
            string sName;


            //lets find them out...
            CMOOSFileReader::GetTokenValPair(sLine,sTok,sVal);

            CMOOSNavBeacon Beacon;

            sName=sVal;

            if(*(++q) == "[")
            {
                while(++q!=sParams.end() && *q!="]")
                {
                    sLine = *q;
                    MOOSToUpper(sLine);

                    CMOOSFileReader::GetTokenValPair(sLine,sTok,sVal);


                    ////////////////////////////
                    //      set up location
                    ////////////////////////////

                    if(sLine.find("POS")!=string::npos)
                    {
                        //we are being told location
                        dfX = atof(MOOSChomp(sVal,",").c_str());
                        dfY = atof(MOOSChomp(sVal,",").c_str());
                        dfZ = atof(MOOSChomp(sVal,",").c_str());

                        Beacon.m_State.m_dfX=dfX;
                        Beacon.m_State.m_dfY=dfY;
                        Beacon.m_State.m_dfZ=dfZ;

                    }

                    ////////////////////////////
                    //      set up transmit
                    ////////////////////////////

                    if(sLine.find("TX")!=string::npos)
                    {
                        MOOSChomp(sVal,"CH");

                        nChan = atoi(sVal.c_str());

                        if(nChan>0)
                        {
                            Beacon.m_nChan = nChan;
                        }
                        else
                        {
                            MOOSTrace("Illegal Beacon Reply Channel!\n");
                            return false;
                        }

                    }


                    ////////////////////////////
                    //      set up TAT
                    ////////////////////////////

                    if(sLine.find("TAT")!=string::npos)
                    {
                        dfTAT = atof(sVal.c_str());

                        if(dfTAT>0)
                        {
                            Beacon.m_dfTAT=dfTAT;
                        }
                        else
                        {
                            MOOSTrace("Illegal TAT must be >0\n");
                            return false;
                        }
                    }
                }
            }//end of Beacon [.....] block

            //now tell all engines about this beacon
            NAVENGINE_LIST::iterator p;
            for(p=m_NavEngines.begin();p!=m_NavEngines.end();p++)
            {
                CMOOSNavEngine * pEngine = *p;
                if(!pEngine->AddAcousticBeacon(sName,nChan,dfTAT,dfX,dfY,dfZ))
                {
                    MOOSTrace("failed to add acoustic beacon\n");
                    return false;
                }
            }
        }
    }

    return true;
}





bool CMOOSNavigator::AddFixedObservations()
{

    typedef map<string,CMOOSObservation::Type> NAME_2_OBS_TYPE_MAP;

    NAME_2_OBS_TYPE_MAP    AllowedObs;

    //add new fixed observatin types here.....
    AllowedObs["FIXEDDEPTH"]=CMOOSObservation::DEPTH;
    AllowedObs["FIXEDHEADING"]=CMOOSObservation::YAW;
    AllowedObs["FIXEDTIDE"]=CMOOSObservation::TIDE;



    NAME_2_OBS_TYPE_MAP::iterator s;
    for(s = AllowedObs.begin();s!=AllowedObs.end();s++)
    {
        string sName = s->first;
        string sVal;

        if(m_MissionReader.GetConfigurationParam(sName,sVal))
        {
            //we know type of observation this maps to..
            CMOOSObservation::Type eType = s->second;

            //look for value
            double dfVal = atof(sVal.c_str());

            if(eType==CMOOSObservation::YAW)
            {
                dfVal = MOOS_ANGLE_WRAP(-MOOSDeg2Rad(dfVal));
            }

            //now look for uncertainty
            //chomp on @ symbol
            MOOSChomp(sVal,"@");

            double dfStd = -1;
            if(!sVal.empty())
            {
                dfStd = atof(sVal.c_str());
            }


            NAVENGINE_LIST::iterator p;

            for(p=m_NavEngines.begin();p!=m_NavEngines.end();p++)
            {
                CMOOSNavEngine * pEngine = *p;
                if(!pEngine->AddFixedObservation(eType,dfVal,dfStd))
                    return false;
            }

        }
    }





    return true;

}


bool CMOOSNavigator::HandlePersonalMail(MOOSMSG_LIST &NewMail)
{


    CMOOSMsg Msg;

    if(m_Comms.PeekMail(NewMail,"RESTART_NAV",Msg,true))
    {
        if(!Msg.IsSkewed(MOOSTime()))
        {
            OnNavRestart();
        }
    }

    if(m_Comms.PeekMail(NewMail,"NAV_SUMMARY_REQUEST",Msg,true))
    {
        if(!Msg.IsSkewed(MOOSTime()))
        {
            if(m_pEKF!=NULL)
            {
                m_pEKF->DoObservationSummary();
            }
        }
    }


    if(m_Comms.PeekMail(NewMail,"LSQ_ENABLE",Msg,true))
    {
        if(!Msg.IsSkewed(MOOSTime()))
        {
            bool bEnable = MOOSStrCmp("TRUE",Msg.m_sVal);
            if(m_pLSQ!=NULL)
            {
                m_pLSQ->Enable(bEnable);
            }
        }
    }

    if(m_Comms.PeekMail(NewMail,"EKF_ENABLE",Msg,true))
    {
        if(!Msg.IsSkewed(MOOSTime()))
        {
            bool bEnable = MOOSStrCmp("TRUE",Msg.m_sVal);
            if(m_pEKF!=NULL)
            {
                m_pEKF->Enable(bEnable);
                m_pEKF->SetOnline(bEnable);
            }
        }
    }


    return true;
}

bool CMOOSNavigator::OnNavRestart()
{
    MOOSDebugWrite("Restarting Navigator");

    Clean();

    return Initialise();

}

bool CMOOSNavigator::Clean()
{
    NAVENGINE_LIST::iterator p;

    for(p=m_NavEngines.begin();p!=m_NavEngines.end();p++)
    {
        CMOOSNavEngine * pEngine = *p;
        delete pEngine;
    }
    m_NavEngines.clear();
    m_pEKF = NULL;
    m_pLSQ = NULL;

    //clean down inputs list..

    PRIORITYINPUT_LIST::iterator q;

    for(q = m_InputsList.begin();q!=m_InputsList.end();q++)
    {
        CMOOSPriorityInput*  pInput = *q;
        pInput->Clear();
    }


    return true;

}

bool CMOOSNavigator::OnNavFailure(const string &sReason)
{
    if(m_pLSQ!=NULL)
    {
        m_pLSQ->SetOnline(false);
        m_pLSQ->Enable(false);
    }
    if(m_pEKF!=NULL)
    {
        m_pEKF->SetOnline(false);
        m_pEKF->Enable(false);
    }


    string sBadNews = MOOSFormat("NAVIGATION FAILURE : \"%s\" ALL FILTERS GOING DOWN. Are all the required sensors working?",sReason.c_str());

    MOOSDebugWrite(sBadNews);

    m_Comms.Notify("NAVIGATION_FAILURE",sBadNews,MOOSTime());

    m_Comms.Notify("EndMission","Nav Failure");

    return true;
}

bool CMOOSNavigator::MonitorFilters()
{
    //note this is a complicated bit of logic
    //to show the whole piture it is written in one
    //oversized function. The idea is to
    // 1)spot divergence in the EKF
    // 2)spot failuer in LSQ (no updates)
    // 3) spot differeing solutions between EKF and LSQ
    // 4) if possible reset the EKF from the LSQ if the LSQ
    //    has been disagreeing with LSQ for a while
    //
    // note that 4) will reset the EKF continually if the LSQ is all
    // over the shop..
    double dfXEKF;
    double dfYEKF;
    double dfZEKF;
    double dfHEKF;

    double dfXLSQ;
    double dfYLSQ;
    double dfZLSQ;
    double dfHLSQ;

    double dfLastLSQUpdate;
    double dfLastEKFUpdate;
    double dfTimeSinceLSQUpdate=1e6;


    //is LSQ healthy?
    if(m_pLSQ!=NULL && m_pLSQ->IsOnline() )
    {
        //so the LSQ is meant to be working (we set it up)
        //and it has booted.
        m_pLSQ->GetTrackedPosition(dfXLSQ,dfYLSQ,dfZLSQ,dfHLSQ,dfLastLSQUpdate);


        //////////////////////////////////////////////////////
        //    are we getting regular updates?
        dfTimeSinceLSQUpdate = GetTimeNow()-dfLastLSQUpdate;

        if(dfTimeSinceLSQUpdate>m_FilterSafety.m_dfLSQTimeOut && m_pLSQ->GetIterations()!=0)
        {
            //this is really bad news...
            string sReason = MOOSFormat("No updates from LSQ Filter in %f seconds",m_FilterSafety.m_dfLSQTimeOut);
            return OnNavFailure(sReason);
        }

        //////////////////////////////////////////////////////
        //        have we spent too long waiting to boot?
        double dfTimeSinceStart = GetTimeNow()-m_pLSQ->GetTimeStarted();
        if(m_pLSQ->GetIterations()==0 && dfTimeSinceStart>m_FilterSafety.m_dfLSQTimeOut)
        {
            string sReason = MOOSFormat("LSQ Filter seems not to have booted in %f seconds",m_FilterSafety.m_dfLSQTimeOut);
            return OnNavFailure(sReason);
        }

        //////////////////////////////////////////////////////
        //        Are the LSQ fixes really noisy?
        m_FilterSafety.SetLSQSolution(dfXLSQ,dfYLSQ,dfZLSQ,dfHLSQ,dfLastLSQUpdate);
        if(m_FilterSafety.IsLSQNoisey())
        {
            string sReason = MOOSFormat("LSQ Filter is \"all over the shop\" median shift of %f m over %d solutions!",
                m_FilterSafety.GetMedianLSQShift(),
                m_FilterSafety.m_nLSQSampleSize);

            return OnNavFailure(sReason);
        }


        if(m_pEKF!=NULL && m_pEKF->IsEnabled() )
        {
            //so the EKF exists and we want to use it( Enabled)
            // howver it may not be booted (online)
            if(!m_pEKF->IsOnline())
            {
                if(dfTimeSinceLSQUpdate<2.0)
                {
                    //to do add boot code here!!!
                    MOOSDebugWrite("Booting EKF from LSQ Estimate");
                    m_pEKF->SetOnline(true);
                    m_pEKF->SetTimeStarted(MOOSTime());

                    //we boot with our LSQ solution...
                    m_pEKF->ForceTrackedPosition(dfXLSQ,dfYLSQ,dfZLSQ,dfHLSQ);
                }
            }
            else
            {
                //well the EKF and the LSQ are both in use and both
                //online - we should compare then

                m_pEKF->GetTrackedPosition(dfXEKF,dfYEKF,dfZEKF,dfHEKF,dfLastEKFUpdate);

                double dfDistance = sqrt(
                    pow(dfXLSQ-dfXEKF,2)+
                    pow(dfYLSQ-dfYEKF,2)+
                    pow(dfZLSQ-dfZEKF,2));

                //are we behaving?....(in agreement (roughly))
                if(dfDistance>m_FilterSafety.m_dfMaxEKFLSQDeviation )
                {
                    //no... they seem to be diverging...

                    //We may want to see if they disgree over a period of time
                    //ie not just an outlier in the LSQ!
                    int nEKFDisagree = m_pEKF->GetIterations();
                    int nLSQDisagree = m_pLSQ->GetIterations();

                    if(nEKFDisagree!=m_FilterSafety.m_nLastEKFDisagreeIteration
                        && nLSQDisagree!=m_FilterSafety.m_nLastLSQDisagreeIteration)
                    {
                        //both filters have advanced..
                        m_FilterSafety.m_nEKFLSQDisagreements++;

                        m_FilterSafety.m_nLastEKFDisagreeIteration = nEKFDisagree;
                        m_FilterSafety.m_nLastLSQDisagreeIteration = nLSQDisagree;

                    }

                    if(m_FilterSafety.m_nEKFLSQDisagreements>m_FilterSafety.m_nForceEKFAfterNDisagreements)
                    {
                        //we had better force the EKF to the LSQ solution...
                        //this has been going on for a while!
                        string sReason = MOOSFormat("Forcing EKF to LSQ Pose disageed for %d iterations deviation = %f ( >%f )",
                            m_FilterSafety.m_nEKFLSQDisagreements,
                            dfDistance,
                            m_FilterSafety.m_dfMaxEKFLSQDeviation);

                        MOOSDebugWrite(sReason);

                        //here we reset the EKF filter
                        m_pEKF->ForceTrackedPosition(dfXLSQ,dfYLSQ,dfZLSQ,dfHLSQ);


                        //reset our counts..
                        m_FilterSafety.m_nEKFLSQDisagreements=0;
                    }
                }
                else
                {
                        //there is NO disagreement..
                        //reset our counts..
                        m_FilterSafety.m_nEKFLSQDisagreements=0;
                }
            }
        }
    }

    //look after EKF explicit things...
    if(m_pEKF!=NULL && m_pEKF->IsOnline() && m_pEKF->GetIterations()>10)
    {
        //are we too uncertain?
        double dfPX,dfPY,dfPZ,dfPH;
        m_pEKF->GetTrackedUncertainty(dfPX,dfPY,dfPZ,dfPH);

        if(sqrt(dfPX)>m_FilterSafety.m_dfMaxEKFPositionUncertainty || sqrt(dfPY)>m_FilterSafety.m_dfMaxEKFPositionUncertainty)
        {
            //we are lost :-(
            m_pEKF->Reset();
            m_pEKF->SetOnline(false);

            string sReason = MOOSFormat("EKF is lost!, CEP = [%.2f,%.2f]m which is greater than %.2f limit specified in mission file\n",
            sqrt(dfPX),
            sqrt(dfPY),
            m_FilterSafety.m_dfMaxEKFPositionUncertainty);

            MOOSDebugWrite(sReason);
        }
    }
    return true;
}

double CMOOSNavigator::GetTimeNow()
{
    if(IsMOOSPlayBack())
    {
        //we are in playback - time can be warped.
        //best we can do is figure out what the last time
        //that the engines know about is....
        NAVENGINE_LIST::iterator p;

        double dfYoungest = -1.0;
        for(p=m_NavEngines.begin();p!=m_NavEngines.end();p++)
        {
            CMOOSNavEngine * pEngine = *p;

            double dfTmp = pEngine->GetYoungestDataTime();
            dfYoungest = dfTmp>dfYoungest?dfTmp:dfYoungest;
        }

        return dfYoungest;
    }
    else
    {
        //we are live so we use NOW time
        return MOOSTime();
    }
}

bool CMOOSNavigator::MakeSubscriptions()
{
    m_Comms.Register("RESTART_NAV",0.1);
    m_Comms.Register("EKF_ENABLE",0.1);
    m_Comms.Register("LSQ_ENABLE",0.1);
    m_Comms.Register("NAV_SUMMARY_REQUEST",0.1);


    return ManageInputs();
}

bool CMOOSNavigator::Initialise()
{
    m_FilterSafety.Initialise();

    m_MissionReader.GetConfigurationParam("MAXLSQEKFDEVIATION",m_FilterSafety.m_dfMaxEKFLSQDeviation);
    m_MissionReader.GetConfigurationParam("LSQTIMEOUT",m_FilterSafety.m_dfLSQTimeOut);
    m_MissionReader.GetConfigurationParam("MAXEKFPOSITIONUNCERTAINTY",m_FilterSafety.m_dfMaxEKFPositionUncertainty);
    m_MissionReader.GetConfigurationParam("MAXMEDIANLSQSHIFT",m_FilterSafety.m_dfMaxMedianLSQShift);


    if(!SetUpNavEngines())
        return false;

    //this sets up subscriptions etc
    if(!MakeSubscriptions())
        return false;


    return true;
}


bool CMOOSNavigator::CFilterSafety::SetLSQSolution(double dfX, double dfY, double dfZ, double dfH, double dfTime)
{
    if(dfTime==m_dfLastLSQUpdate)
        return true;

    double dfR =    sqrt(pow(m_dfLastLSQX-dfX,2)+
                    pow(m_dfLastLSQY-dfY,2)+
                    pow(m_dfLastLSQZ-dfZ,2));

    m_DeltaLSQHistory.push_front(dfR);

    while(static_cast<int> (m_DeltaLSQHistory.size())>m_nLSQSampleSize)
    {
        m_DeltaLSQHistory.pop_back();
    }

    m_dfLastLSQX=dfX;
    m_dfLastLSQY=dfY;
    m_dfLastLSQZ=dfZ;
    m_dfLastLSQH=dfH;
    m_dfLastLSQUpdate = dfTime;

    return true;
}

bool CMOOSNavigator::CFilterSafety::IsLSQNoisey()
{
    return GetMedianLSQShift()>m_dfMaxMedianLSQShift;
}

double CMOOSNavigator::CFilterSafety::GetMedianLSQShift()
{

    if(static_cast<int> (m_DeltaLSQHistory.size())<m_nLSQSampleSize)
        return 0;

    vector<double> LocalVec;
    LocalVec.resize(m_DeltaLSQHistory.size());

    copy(m_DeltaLSQHistory.begin(),m_DeltaLSQHistory.end(),LocalVec.begin());
    sort(LocalVec.begin(),LocalVec.end());

    double dfMedian = LocalVec[LocalVec.size()/2];

    return dfMedian;
}

