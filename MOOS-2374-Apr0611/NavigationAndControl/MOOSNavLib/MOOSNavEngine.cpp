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
// MOOSNavEngine.cpp: implementation of the CMOOSNavEngine class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include <MOOSGenLib/MOOSGenLib.h>
#include "MOOSNavBeacon.h"
#include "MOOSNavVehicle.h"
#include "MOOSNavSensor.h"
#include "MOOSNavObsStore.h"    
#include "MOOSNavEngine.h"
#include "MOOSNavLibDefs.h"
#include <sstream>
#include <iomanip>
#include "math.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//noises
#define LBL_2WR_STD (2*0.0015)
#define BODY_VEL_STD (0.02)

#define YAW_STD 0.001
#define DEPTH_STD 0.1
#define XY_STD 1.0


CMOOSNavEngine::CMOOSNavEngine()
{
    m_bInitialOnline = false;


    m_dfSV = 1500.0;

    m_nNextID = 0;
    m_dfLastUpdate = 0;

    //by defualt update as often as posisble
    m_dfUpdatePeriod = 0;

    m_nIterations = 0;

    //by default we make static vehciles
     m_eVehicleType =  CMOOSNavEntity::POSE_ONLY;

     m_bOnline = true;

     m_bEnabled = true;

     //by default we will not map thrust to velocity measurements
     m_bThrust2Vel = false;
     m_dfThrust2VelGain = 1.0;

     //by default no heading bias state!
     m_bEstimateHeadingBias = false;


     //here we set the base class pointer to results mesage list to be our own 
     //results list (lets CMOOSNavBase::addToOutput work

     CMOOSNavBase::SetOutputList(&m_ResultsList);
}

CMOOSNavEngine::~CMOOSNavEngine()
{
    if(m_pStore!=NULL)
        delete m_pStore;
}

bool CMOOSNavEngine::AddData(const CMOOSMsg &Msg)
{
    //don't add data if not enabled
    if(!IsEnabled())
        return true;

    
    // 1) convert to observation
    // 2) store it
    // 3) look to processes
    
    OBSLIST NewObs;
                
    if(DataToObservations(Msg,NewObs))
    {
        return m_pStore->Add(NewObs);            
    }
    return false;
}

bool CMOOSNavEngine::DataToObservations(const CMOOSMsg &Msg, OBSLIST &ObsList)
{
    CMOOSNavSensor * pRelevantSensor = GetSensorBySource(Msg.m_sSrc,Msg.m_sKey);

    if(pRelevantSensor==NULL)
    {
        //no idea what to do with it...
        return false;
    }

    bool bSuccess = false;
    switch(pRelevantSensor->m_eType)
    {
    case CMOOSNavSensor::LBL:
        bSuccess =  MakeLBLObservations(Msg,ObsList);        
        break;

    case CMOOSNavSensor::DEPTH:
        bSuccess =  MakeDepthObservations(Msg,ObsList);
        break;

    case CMOOSNavSensor::XY:
        bSuccess =  MakeXYObservations(Msg,ObsList);
        break;

    case CMOOSNavSensor::BODY_VEL:
        bSuccess =  MakeBodyVelObservations(Msg,ObsList);
        break;

    case CMOOSNavSensor::CONTROL:
        bSuccess =  MakeControlObservations(Msg,ObsList);
        break;
        

    case CMOOSNavSensor::ORIENTATION:
        //for now we only deal with yaw
        if(Msg.m_sKey.find("YAW")!=string::npos)
        {
            bSuccess = MakeYawObservations(Msg,ObsList);        
        }
        break;
    
    default:
        break;
    }
    


    OBSLIST::iterator p;

    for(p = ObsList.begin();p!=ObsList.end();p++)
    {
        //observations may  need to know if they are
        // using heading bias estimation
        p->UsingHeadingBias(m_bEstimateHeadingBias);
    }

    return bSuccess;
}


CMOOSSensorChannel * CMOOSNavEngine::GetSensorChannel(const string & sKey)
{
    SOURCE_SENSORCHANNEL_MAP::iterator q = m_SensorChannelMap.find(sKey);
    
    if(q==m_SensorChannelMap.end() || m_SensorChannelMap.empty())
    {
        //must be made from mission file
        return NULL;
    }
    
    return &(q->second);
}



bool CMOOSNavEngine::MakeDepthObservations(const CMOOSMsg &Msg, OBSLIST &ObsList)
{
    CMOOSObservation NewObs;
    NewObs.m_dfTime = Msg.m_dfTime;
    NewObs.m_dfData = Msg.m_dfVal;
    NewObs.m_dfDataStd = DEPTH_STD;

    NewObs.m_eType = CMOOSObservation::DEPTH;
    NewObs.m_nID = GetNextID();
    
    //use the message source to figure out the sensor this
    //corresponds to
    NewObs.m_pInterrogateSensor = GetSensorBySource(Msg.m_sSrc,Msg.m_sKey);

    if(NewObs.m_pInterrogateSensor!=NULL)
    {
        //overload noise
        if(NewObs.m_pInterrogateSensor->GetNoise()>=0)
        {
            NewObs.m_dfDataStd = NewObs.m_pInterrogateSensor->GetNoise();
        }

        ObsList.push_front(NewObs);
    }

    return true;
}

bool CMOOSNavEngine::MakeXYObservations(const CMOOSMsg &Msg, OBSLIST &ObsList)
{
    CMOOSObservation NewObs;
    NewObs.m_dfTime = Msg.m_dfTime;
    NewObs.m_dfData = Msg.m_dfVal;
    NewObs.m_dfDataStd = XY_STD;

    if(Msg.m_sKey.find("_X")!=string::npos)
    {
        NewObs.m_eType = CMOOSObservation::X;
    }
    if(Msg.m_sKey.find("_Y")!=string::npos)
    {
        NewObs.m_eType = CMOOSObservation::Y;
    }

    NewObs.m_nID = GetNextID();

    //use the message source to figure out the sensor this
    //corresponds to
    NewObs.m_pInterrogateSensor = GetSensorBySource(Msg.m_sSrc,Msg.m_sKey);

    if(NewObs.m_pInterrogateSensor!=NULL)
    {   
        //overload noise
        if(NewObs.m_pInterrogateSensor->GetNoise()>=0)
        {
            NewObs.m_dfDataStd = NewObs.m_pInterrogateSensor->GetNoise();
        }

        ObsList.push_front(NewObs);
    }
    return true;

}

bool CMOOSNavEngine::MakeControlObservations(const CMOOSMsg &Msg, OBSLIST &ObsList)
{
   if(Msg.m_sKey.find("_THRUST")!=string::npos)
   {
       //do we wnat to use thrust measurements?
        if(!m_bThrust2Vel)
            return true;

        CMOOSMsg MappedMsg = Msg;

        //dilute the precision of this..
        double dfObsDilation = 4;
        //make a message for the y (forward velocity)
        MappedMsg.m_dfVal*= m_dfThrust2VelGain;
        MappedMsg.m_sKey="THRUST_DERIVED_BODY_VEL_Y";
        if(!MakeBodyVelObservations(MappedMsg,ObsList,dfObsDilation))
            return false;

        //make a message for the x (Lateral velocity)
        MappedMsg.m_dfVal= 0;
        MappedMsg.m_sKey="THRUST_DERIVED_BODY_VEL_X";
        if(!MakeBodyVelObservations(MappedMsg,ObsList,dfObsDilation))
            return false;
   }
 
   return true;

}


bool CMOOSNavEngine::MakeBodyVelObservations(const CMOOSMsg &Msg, OBSLIST &ObsList,double dfSF)
{
    CMOOSObservation NewObs;
    NewObs.m_dfTime = Msg.m_dfTime;
    NewObs.m_dfData = Msg.m_dfVal;
    NewObs.m_dfDataStd = BODY_VEL_STD*dfSF;

    if(Msg.m_sKey.find("_Y")!=string::npos)
    {
        NewObs.m_eType = CMOOSObservation::BODY_VEL_Y;
    }
    else if(Msg.m_sKey.find("_X")!=string::npos)
    {
        NewObs.m_eType = CMOOSObservation::BODY_VEL_X;
    }

    NewObs.m_nID = GetNextID();

    //use the message source to figure out the sensor this
    //corresponds to
    NewObs.m_pInterrogateSensor = GetSensorBySource(Msg.m_sSrc,Msg.m_sKey);

    if(NewObs.m_pInterrogateSensor!=NULL)
    {
        //overload noise
        if(NewObs.m_pInterrogateSensor->GetNoise()>=0)
        {
            NewObs.m_dfDataStd = NewObs.m_pInterrogateSensor->GetNoise();
        }

        ObsList.push_front(NewObs);
    }
    return true;

}


bool CMOOSNavEngine::MakeYawObservations(const CMOOSMsg &Msg, OBSLIST &ObsList)
{
    CMOOSObservation NewObs;
    NewObs.m_dfTime = Msg.m_dfTime;
    NewObs.m_dfData = Msg.m_dfVal;
    NewObs.m_dfDataStd = YAW_STD;
    NewObs.m_eType = CMOOSObservation::YAW;
    NewObs.m_nID = GetNextID();



    //use the message source to figure out the sensor this
    //corresponds to
    NewObs.m_pInterrogateSensor = GetSensorBySource(Msg.m_sSrc,Msg.m_sKey);

    if(NewObs.m_pInterrogateSensor!=NULL)
    {

        //overload noise
        if(NewObs.m_pInterrogateSensor->GetNoise()>=0)
        {
            NewObs.m_dfDataStd = NewObs.m_pInterrogateSensor->GetNoise();
        }

        ObsList.push_front(NewObs);
    }

    ObsList.push_front(NewObs);

    return true;
}


bool CMOOSNavEngine::MakeLBLObservations(const CMOOSMsg &Msg, OBSLIST &ObsList)
{
    //this is LBL data    
    string sData = Msg.m_sVal;
    
    //what time was interrogation sent?
    MOOSChomp(sData,"Tx=");
    double dfTxTime = atof(MOOSChomp(sData,",").c_str());
    
    if(dfTxTime<=0)
    {
        MOOSTrace("Tx time of LBL is not positive\n");
        return false;
    }
    
    while(!sData.empty())
    {
        //for each reply...
        CMOOSObservation NewObs;
        string sChunk = MOOSChomp(sData,",");

        MOOSChomp(sChunk,"Ch[");

        int nChan = atoi(sChunk.c_str());
        
        if(nChan<=0 || nChan > 14)
        {
            MOOSTrace("Channel of LBL obs is not valid\n");
            continue;
        }


        NewObs.m_nChan = nChan;
        
        //figure out the beacon from the channel....
        CMOOSNavBeacon * pBeacon = GetBeaconByChannel(nChan);
        
        if(pBeacon==NULL)
        {
            MOOSTrace("Warning: No known Beacon with Channel[%d]\n",nChan);
        }

        

        //so what was the observation - the TOF?
        MOOSChomp(sChunk,"=");
        double dfTOF = atof(sChunk.c_str());
        if(dfTOF<=0)
        {
            MOOSTrace("TOF of LBL obs is not valid\n");
            return false;
        }
        NewObs.m_dfData = dfTOF;
        

        //set obs time Tx Time plus in water time
        NewObs.m_dfTime = dfTxTime+dfTOF;

//        MOOSTrace("Making LBL Obs.dfTime = %f and now = %f\n",NewObs.m_dfTime,MOOSTime());
        
        //set obs type 
        NewObs.m_eType = CMOOSObservation::LBL_BEACON_2WR;
        
        //give it a unique id
        NewObs.m_nID = GetNextID();


        //use the message source to figure out the sensor this
        //corresponds to
        NewObs.m_pInterrogateSensor = GetSensorBySource(Msg.m_sSrc,Msg.m_sKey);

        if(NewObs.m_pInterrogateSensor==NULL)
        {
            return false;
        }
        
        //set observation noise
        NewObs.m_dfDataStd = LBL_2WR_STD;
        if(NewObs.m_pInterrogateSensor->GetNoise()>=0)
        {
            NewObs.m_dfDataStd = NewObs.m_pInterrogateSensor->GetNoise();
        }
        
        //what other responding sensor was involved?
        if(pBeacon!=NULL)
        {
            NewObs.m_pRespondingSensor = pBeacon->GetSensorByType(CMOOSNavSensor::LBL);
        }
        else
        {
            NewObs.m_pRespondingSensor=NULL;
        }
        

        //this is an acoustic obs so set the sound velocity
        NewObs.m_dfSV = m_dfSV;
        //finally add the obs to the list
        if(NewObs.m_pRespondingSensor!=NULL)
        {
            ObsList.push_back(NewObs);
        }

/*        MOOSTrace("NewLBL: MOOSTime = %.3f,msgtime= %.3f,datatime = %.3f,TxTime = %.3f, cTOF= %.3f\n",
            MOOSTime(),
            Msg.m_dfTime,
            NewObs.m_dfTime,
            dfTxTime,
            NewObs.m_dfTime-dfTxTime);*/

    }
    
    return true;
}

CMOOSNavBeacon * CMOOSNavEngine::GetBeaconByName(const string & sName)
{
    BEACONLIST::iterator p;
    
    for(p = m_Beacons.begin();p!=m_Beacons.end();p++)
    {
        
        CMOOSNavBeacon * pBcn =  *p;

        if(pBcn->m_sName==sName)
        {
            return pBcn;
        }
    }
    return NULL;
}

CMOOSNavBeacon * CMOOSNavEngine::GetBeaconByChannel(int nChannel)
{
    BEACONLIST::iterator p;
    
    for(p = m_Beacons.begin();p!=m_Beacons.end();p++)
    {
        
        CMOOSNavBeacon * pBcn =  *p;

        if(pBcn->m_nChan==nChannel)
        {
            return pBcn;
        }
    }
    return NULL;
}


bool CMOOSNavEngine::Iterate(double dfTimeNow)
{
    return false;
}


bool CMOOSNavEngine::Initialise(STRING_LIST  sParams)
{

    //make a store for observations..
    m_pStore = new CMOOSNavObsStore;

    //we may be being asked to map thrust to velocity
    string sVal;
    if(MOOSGetValueFromToken(sParams,"THRUST2VELOCITY",sVal))
    {
        if(MOOSStrCmp(sVal,"TRUE"))
        {
            if(MOOSGetValueFromToken(sParams,"THRUST2VELOCITY_GAIN",sVal))
            {
                m_bThrust2Vel = true;
                m_dfThrust2VelGain = atof(sVal.c_str());
            }
        }
    }



    if(MOOSGetValueFromToken(sParams,"SV",sVal))
    {
        double dfNewSV = atof(sVal.c_str());         
        if(dfNewSV !=0)
        {
            m_dfSV=dfNewSV;
        }
    }

    //set up navigation filter logging....
    bool bLog = false;
    if(MOOSGetValueFromToken(sParams,"NAV_LOG",sVal))
    {
        bLog = MOOSStrCmp(sVal,"TRUE");
    }
    if(bLog)
    {
        string sPath="";
        
        //do we have a globally defined path?
        CProcessConfigReader Reader;
        Reader.SetFile(m_sMissionFileName);

        if(!Reader.GetValue("GLOBALLOGPATH",sPath))
        {
            //no..get it locally
            MOOSGetValueFromToken(sParams,"NAV_LOG_PATH",sPath);
        }
        bool bTimeStamp=false;
        if(MOOSGetValueFromToken(sParams,"NAV_LOG_TIMESTAMP",sVal))
        {
            bTimeStamp = MOOSStrCmp(sVal,"TRUE");
        }

        
        m_Logger.Initialise(m_sName,sPath,bTimeStamp);

             
    }



    //Allocate Global states..
    SetUpGlobalStates();

    AddTheVehicle(sParams);

    //fixed observations are attached to the COG...
    AddSensor("FIXED","COG", CMOOSNavSensor::FIXED,0,0,0);

    //set our default start up state...
    //set in constructor of derived classes
    SetOnline(m_bInitialOnline);
    return true;

}


bool CMOOSNavEngine::AddEntity(CMOOSNavEntity *pEntity,
                               Matrix * pCovariance,
                               Matrix * pCrossCovariance)
{
    Matrix XNew;

    //we always give entities a pinter to the global estimate
    //matrices

    pEntity->m_pXhat = & m_Xhat;
    pEntity->m_pPhat = & m_Phat;


    //get the entity to format a state vector representing
    //its state
    if(pEntity->GetFullState(XNew,NULL,false))
    {
        //XNew will be 8x1 but what are the estimated states?
        int nSize = pEntity->GetStateSize();

        if(nSize==0)
        {
            //its not in the state vector (eg a beaon)
            return true;
        }

        
        pEntity->m_nStart = m_Xhat.Nrows()+1;
        pEntity->m_nEnd = pEntity->m_nStart+nSize-1;

        if(m_Xhat.Nrows()==0 || m_Xhat.Ncols() ==0)
        {
            m_Xhat = XNew.SubMatrix(1,nSize,1,1);
            m_Phat.ReSize(nSize,nSize);
            m_XhatTmp = m_Xhat;
            m_PhatTmp = m_Phat;

        }
        else
        {
            m_Xhat = m_Xhat & XNew.SubMatrix(1,nSize,1,1);

            //now resize P..
            Matrix OldP = m_Phat;
            m_Phat.ReSize(m_Xhat.Nrows(),m_Xhat.Nrows());
            m_Phat=0;
            m_Phat.SubMatrix(1,OldP.Nrows(),1,OldP.Ncols()) = OldP;


        }

    }


    return true;
}

bool CMOOSNavEngine::IndexObservations(int &nObsDim)
{

    nObsDim = 0;

    OBSLIST::iterator p;

    for(p = m_Observations.begin();p!=m_Observations.end();p++)
    {
        CMOOSObservation  & rObs = *p;
        
        if(rObs.m_bIgnore || rObs.m_bGoodDA==false)
            continue;

        rObs.m_nRow = nObsDim+1;

        nObsDim+=rObs.GetDimension();

    }

    return true;
}

bool CMOOSNavEngine::TraceObservationSet()
{
    OBSLIST::iterator p;
    int nIgnored=0;
    for(p = m_Observations.begin();p!=m_Observations.end();p++)
    {
        if(p->m_bIgnore)
            nIgnored++;
    }


    MOOSTrace("\n %s Working with %d observations (%d ignored):\n",
        m_sName.c_str(),
        m_Observations.size()-nIgnored,nIgnored);


    int i = 1;
    for(p = m_Observations.begin();p!=m_Observations.end();p++)
    {
        if(p->m_bIgnore)
            continue;

        MOOSTrace("%d)\t",i++);
        p->Trace();
    }

    return true;
}

bool CMOOSNavEngine::AddSensor(const string & sSource,
                               const string &sName,
                               CMOOSNavSensor::Type eType,
                               double dfX,
                               double dfY,
                               double dfZ,
                               double dfNoise)
{
    //////////////////////////////////////////
    //            add an new sensor
    CMOOSNavSensor * pNewSensor = new CMOOSNavSensor;
    pNewSensor->m_nID = GetNextID();
    pNewSensor->m_sName=sName;
    pNewSensor->m_eType = eType;
    pNewSensor->m_sMOOSSource = sSource;
    pNewSensor->SetNoise(dfNoise);
    if(m_pTracked!=NULL)
    {
        m_pTracked->AddSensor(pNewSensor);

        string sKey = sSource+":"+sName;
        if(m_SourceToSensorMap.find(sName)==m_SourceToSensorMap.end())
        {
            m_SourceToSensorMap[sKey] = pNewSensor;
            MOOSTrace("Added new sensor:\n\t\"%s\" @ %f %f %f\n",sKey.c_str(),dfX,dfY,dfZ);

        }
        else
        {
            MOOSTrace("Error Sensor \"%s\" already exists \n",sKey.c_str());
            return false;
        }
    }
    else
    {
        return false;
    }
    
    return true;
}

bool CMOOSNavEngine::AddAcousticBeacon(const string & sName,
                                       int nChan,
                                       double dfTAT,
                                       double dfX,
                                       double dfY,
                                       double dfZ)
{

    //////////////////////////////////////////
    //            add a beacon
    CMOOSNavBeacon* pBeacon = new CMOOSNavBeacon;

    pBeacon->m_sName = sName;
    pBeacon->m_nID = GetNextID();
    
    pBeacon->m_State.m_dfX = dfX;
    pBeacon->m_State.m_dfY = dfY;
    pBeacon->m_State.m_dfZ = dfZ;
    
    pBeacon->m_dfTAT = dfTAT;
    pBeacon->m_nChan = nChan;    

    if(AddEntity(pBeacon))
    {
//        MOOSTrace("Added Beacon :\n\t");
//        pBeacon->Trace();
        m_Beacons.push_front(pBeacon);
    }

    return true;
    
}


bool CMOOSNavEngine::AddFixedObservation(CMOOSObservation::Type eType, double dfVal, double dfStd)
{
    CMOOSObservation NewFixedObs;

    NewFixedObs.m_dfData = dfVal;
    NewFixedObs.m_dfDataStd = dfStd;
    NewFixedObs.m_eType = eType;
    NewFixedObs.m_nID = GetNextID();
    NewFixedObs.m_dfTime = -1;
    NewFixedObs.SetFixed(true);
    NewFixedObs.UsingHeadingBias(m_bEstimateHeadingBias);

    NewFixedObs.m_pInterrogateSensor = GetSensorByName("COG");

    if(NewFixedObs.m_pInterrogateSensor==NULL)
    {
        return false;
    }

    m_FixedObservations.push_back(NewFixedObs);

    return true;

}

CMOOSNavSensor* CMOOSNavEngine::GetSensorByName(const string &sSensorName)
{
    SENSOR_MAP::iterator p;
    for(p = m_SourceToSensorMap.begin();p!=m_SourceToSensorMap.end();p++)
    {
        CMOOSNavSensor * pSensor = p->second;
        if(pSensor->GetName()==sSensorName)
        {
            return pSensor;
        }
        
    }
    return NULL;

}


CMOOSNavSensor* CMOOSNavEngine::GetSensorBySource(const string &sMOOSSource,const string & sDataName)
{
    SENSOR_MAP::iterator p;

    CMOOSNavSensor::Type eType = SensorTypeFromDataName(sDataName);

    if(eType==CMOOSNavSensor::INVALID)
        return NULL;

    for(p = m_SourceToSensorMap.begin();p!=m_SourceToSensorMap.end();p++)
    {
        CMOOSNavSensor * pSensor = p->second;
        if(pSensor->m_eType==eType && pSensor->m_sMOOSSource==sMOOSSource)
        {
            return pSensor;
        }        
    }
    return NULL;

}

// here we set aside space for global states such
//as tide and heading bias estimation etc..
bool CMOOSNavEngine::SetUpGlobalStates()
{
    int nGlobalStates = 1; //tide...

    if(m_bEstimateHeadingBias)
        nGlobalStates = 2; //tide and heading bias

    m_Xhat.ReSize(nGlobalStates,1);
    m_Xhat=0;
    m_Phat.ReSize(nGlobalStates,nGlobalStates);
    m_Phat = 0;

    return true;
}

//client calls this to fetch results...
bool CMOOSNavEngine::GetNextResult(CMOOSMsg &ResultMsg)
{
    if(m_ResultsList.empty())
    {
        return false;
    }

    ResultMsg = m_ResultsList.front();
    m_ResultsList.pop_front();

    return true;
}

bool CMOOSNavEngine::LimitObservations(CMOOSObservation::Type eType, int nNumber)
{
    OBSLIST::iterator p;

    int nFound = 0;
    for(p = m_Observations.begin();p!=m_Observations.end();p++)
    {
        if(p->m_eType==eType)
        {
            nFound++;
            if(nFound>nNumber)
            {
                p->m_bIgnore = true;
            }
            else
            {
                p->m_bIgnore = false;
            }
        }
    }

    return true;
}

bool CMOOSNavEngine::WrapAngleStates()
{
    //wrap the state itself!
    int nStateNdx = m_pTracked->m_nStart+iiH;
    m_Xhat(nStateNdx,1) = MOOS_ANGLE_WRAP(m_Xhat(nStateNdx,1));
    
    if(m_pTracked->GetEntityType()==CMOOSNavEntity::POSE_AND_RATE)
    {
        nStateNdx = m_pTracked->m_nStart+iiHdot;
        m_Xhat(nStateNdx,1) = MOOS_ANGLE_WRAP(m_Xhat(nStateNdx,1));
    }


    return true;
}

bool CMOOSNavEngine::GuessVehicleLocation()
{
    BEACONLIST::iterator q;
    double dfX=0;
    double dfY=0;
    double dfZ = 0;
    for(q=m_Beacons.begin();q!=m_Beacons.end();q++)
    {
        dfX+=(*q)->m_State.m_dfX;
        dfY+=(*q)->m_State.m_dfY;
        dfZ+=(*q)->m_State.m_dfZ;

    }

    dfX/=m_Beacons.size();
    dfY/=m_Beacons.size();
    dfZ/=m_Beacons.size();

    m_pTracked->m_State.m_dfX = dfX;
    m_pTracked->m_State.m_dfY = dfY;
    m_pTracked->m_State.m_dfZ = dfZ+10;

    m_pTracked->RefreshStateVector();
    

    MOOSTrace("Set Vehicle to [%7.3f %7.3f %7.3f]\n",
        m_pTracked->m_State.m_dfX, 
        m_pTracked->m_State.m_dfY,
        m_pTracked->m_State.m_dfZ);


    return true;

}




bool CMOOSNavEngine::GetTATByChannel(int nChannel, double &dfTAT)
{
    CMOOSNavBeacon * pBcn = GetBeaconByChannel(nChannel);

    if(pBcn)
    {
        dfTAT =  pBcn->m_dfTAT;
        return true;
    }
    else
    {
        return false;
    }
}

bool CMOOSNavEngine::AddTheVehicle(STRING_LIST &sParams)
{
    //////////////////////////////////////////
    //        add the vehicle itself!
    // overide this function to make more complex vehicles
    m_pTracked = new CMOOSNavVehicle;
    m_pTracked->m_nID = GetNextID();
    m_pTracked->m_sName="TheAUV";
    m_pTracked->SetEntityType(m_eVehicleType);
    m_pTracked->m_State.m_dfZ = 0;
    return AddEntity(m_pTracked);
}


bool CMOOSNavEngine::MakeObsMatrices()
{
    int nObsSize;
    
    if(!IndexObservations(nObsSize))
        return false;
    
    int nStateSize = m_Xhat.Nrows();
    
    if(m_jH.Nrows()!=nObsSize || m_jH.Ncols() !=  nStateSize)
    {
        m_jH.ReSize(nObsSize,nStateSize);
    }
    
    if(m_R.Nrows()!=nObsSize || m_R.Ncols() !=  nObsSize)
    {
        m_R.ReSize(nObsSize,nObsSize);
    }
    
    if(m_Innov.Nrows()!=nObsSize || m_Innov.Ncols() !=  1)
    {
        m_Innov.ReSize(nObsSize,1);
    }
    
    //zero all..
    m_jH=0;
    m_R=0;
    m_Innov = 0;
    

    OBSLIST::iterator p;
    
    for(p = m_Observations.begin();p!=m_Observations.end();p++)
    {
        CMOOSObservation  & rObs = *p;

        if(rObs.m_bIgnore || rObs.m_bGoodDA==false)
            continue;
        
        rObs.MakeMatrices(m_Innov,m_jH,m_R,m_Xhat);
        
    }
        
    
    return true;
}

bool CMOOSNavEngine::GetTrackedPosition(double &dfX, double &dfY, double &dfZ,double & dfH,double & dfLastUpdate)
{
    m_pTracked->RefreshState();
    dfX = m_pTracked->m_State.m_dfX;
    dfY = m_pTracked->m_State.m_dfY;
    dfZ = m_pTracked->m_State.m_dfZ;
    dfH = m_pTracked->m_State.m_dfH;

    dfLastUpdate = m_dfLastUpdate;

    return true;
}

bool CMOOSNavEngine::GetTrackedUncertainty(double &dfPX,
                                           double &dfPY,
                                           double &dfPZ,
                                           double &dfPH)                                           
{
    m_pTracked->RefreshState();
    dfPX = m_pTracked->m_State.m_dfPX;
    dfPY = m_pTracked->m_State.m_dfPY;
    dfPZ = m_pTracked->m_State.m_dfPZ;
    dfPH = m_pTracked->m_State.m_dfPH;

    return true;
}


bool CMOOSNavEngine::ForceTrackedPosition(double dfX, double dfY, double dfZ, double dfH)
{
    m_pTracked->m_State.m_dfX = dfX;
    m_pTracked->m_State.m_dfY = dfY;
    m_pTracked->m_State.m_dfZ = dfZ;
    m_pTracked->m_State.m_dfH = dfH;

    m_pTracked->RefreshStateVector();

    AddToOutput("Forcing %s to [%.1f,%.1f,%.1f,%.1f]",
        m_sName.c_str(),
        dfX,
        dfY,
        dfZ,
        dfH);

    


    return true;
 
}

bool CMOOSNavEngine::LimitObservationTypes()
{

    if(m_pTracked->GetEntityType()==CMOOSNavEntity::POSE_ONLY)
    {
        set<CMOOSObservation::Type> Allowed;

        Allowed.insert(CMOOSObservation::X);
        Allowed.insert(CMOOSObservation::Y);
        Allowed.insert(CMOOSObservation::YAW);
        Allowed.insert(CMOOSObservation::LBL_BEACON_2WR);
        Allowed.insert(CMOOSObservation::DEPTH);
        Allowed.insert(CMOOSObservation::TIDE);

        if(m_bEstimateHeadingBias)
        {
            Allowed.insert(CMOOSObservation::HEADING_BIAS);
        }


        OBSLIST::iterator p;

        for(p=m_Observations.begin();p!=m_Observations.end();p++)
        {
            CMOOSObservation & rObs = *p;
            if(Allowed.find(rObs.m_eType)==Allowed.end())
            {
                rObs.Ignore(true);
            }
        }
    }
    
    return true;
}

bool CMOOSNavEngine::Reset()
{
    return true;
}

bool CMOOSNavEngine::IsOnline()
{
    return m_bOnline;
}

bool CMOOSNavEngine::SetOnline(bool bOnline)
{
    if(m_bOnline==false && bOnline==true)
    {
        //reset cousnters when starting up
        m_nIterations = 0;
    }

    m_bOnline = bOnline;

    AddToOutput("%s is %s!",m_sName.c_str(),m_bOnline?"ONLINE":"OFFLINE");

    return true;
}

int CMOOSNavEngine::GetIterations()
{
    return m_nIterations;
}

bool CMOOSNavEngine::Enable(bool bEnable)
{
    m_bEnabled = bEnable;
    return true;
}

bool CMOOSNavEngine::IsEnabled()
{
    return m_bEnabled;
}

double CMOOSNavEngine::GetYoungestDataTime()
{
   return m_pStore->GetNewestObsTime(); 
}

bool CMOOSNavEngine::SetMissionFileName(const string &sFileName)
{
    m_sMissionFileName = sFileName;
    return true;
}

bool CMOOSNavEngine::MarkObservationsDA(bool bGoodDA)
{
    OBSLIST::iterator p;
    for(p = m_Observations.begin();p!=m_Observations.end();p++)
    {
        CMOOSObservation & rObs = *p;

        if(!rObs.m_bIgnore)
        {
            rObs.SetGoodDA(bGoodDA);
        }
    }

    return true;
}
bool CMOOSNavEngine::LogObservationSet(double dfTimeNow,                                   
                                       int nthUpdate)
{
    OBSLIST::iterator p;
    for(p=m_Observations.begin();p!=m_Observations.end();p++)
    {
        CMOOSObservation & rObs = *p;

        if(!rObs.m_bIgnore)
        {
            m_Logger.LogObservation(dfTimeNow,rObs,nthUpdate);
        }
    }

    return true;
}

bool CMOOSNavEngine::RecordObsStatistics(Matrix *pInnov, Matrix *pS)
{
    if(pInnov==NULL)
        return true;

    int nObs = pInnov->Nrows();
    for(int i = 1;i<=nObs;i++)
    {
        OBSLIST::iterator t;
        for(t = m_Observations.begin();t!=m_Observations.end();t++)
        {
            if(t->m_bIgnore==false && t->m_nRow==i)
            {
                t->m_dfInnov = (*pInnov)(i,1);
                if(pS!=NULL)
                {
                    t->m_dfInnovStd = sqrt((*pS)(i,i));
                }
            }
        }
    }
    return true;
}

bool CMOOSNavEngine::SetTimeStarted(double dfTime)
{
    m_dfTimeStarted = dfTime;
    return true;
}


double CMOOSNavEngine::GetTimeStarted()
{
    return m_dfTimeStarted;
}

CMOOSNavSensor::Type CMOOSNavEngine::SensorTypeFromDataName(const string &sDataName)
{
    if(sDataName.find("_YAW")!=string ::npos)
        return CMOOSNavSensor::ORIENTATION;

    if(sDataName.find("_DEPTH")!=string ::npos)
        return CMOOSNavSensor::DEPTH;
    
    if(sDataName.find("_BODY_VEL")!=string ::npos )
        return CMOOSNavSensor::BODY_VEL;

    if(sDataName.find("_X")!=string ::npos)
        return CMOOSNavSensor::XY;

    if(sDataName.find("_Y")!=string ::npos)
        return CMOOSNavSensor::XY;

    if(sDataName.find("_TOF")!=string ::npos)
        return CMOOSNavSensor::LBL;


    return CMOOSNavSensor::INVALID;
}

bool CMOOSNavEngine::TraceDiagPhat()
{

    ostringstream os;

    os.setf(ios::left);

    os<<endl<<"********* COVARIANCE DUMP *************"<<endl;

    os<<setw(15)<<"Tide Std = "<<sqrt(m_Phat(TIDE_STATE_INDEX,TIDE_STATE_INDEX))<<endl;

    if(m_bEstimateHeadingBias)
    {
        os<<"YawBias   Std= "<<sqrt(m_Phat(HEADING_BIAS_STATE_INDEX,HEADING_BIAS_STATE_INDEX))<<endl;
    }

    int nOffset = m_pTracked->m_nStart;

    os<<setw(15)<<"X   Std= "<<sqrt(m_Phat(nOffset+iiX,nOffset+iiX))<<endl;
    os<<setw(15)<<"Y   Std= "<<sqrt(m_Phat(nOffset+iiY,nOffset+iiY))<<endl;
    os<<setw(15)<<"Z   Std= "<<sqrt(m_Phat(nOffset+iiZ,nOffset+iiZ))<<endl;
    os<<setw(15)<<"Yaw Std= "<<sqrt(m_Phat(nOffset+iiH,nOffset+iiH))<<endl;

    

    if(m_pTracked->GetEntityType()==CMOOSNavEntity::POSE_AND_RATE)
    {

        os<<setw(15)<<"XVel   Std= "<<sqrt(m_Phat(nOffset+iiXdot,nOffset+iiXdot))<<endl;
        os<<setw(15)<<"YVel   Std= "<<sqrt(m_Phat(nOffset+iiYdot,nOffset+iiYdot))<<endl;
        os<<setw(15)<<"ZVel   Std= "<<sqrt(m_Phat(nOffset+iiZdot,nOffset+iiZdot))<<endl;
        os<<setw(15)<<"YawVel Std= "<<sqrt(m_Phat(nOffset+iiHdot,nOffset+iiHdot))<<endl;
    }

    os<<ends;
    string sOut = os.str();
    //os.rdbuf()->freeze(0);

    MOOSTrace(sOut);
    return true;

}
bool CMOOSNavEngine::SetUpSensorChannels(STRING_LIST  sParams,string sToken)
{

       
    //ok we need to load up rejection settings
    STRING_LIST::iterator p;
    
    for(p = sParams.begin();p!=sParams.end();p++)
    {
        string sLine = *p;
        if(sLine.find(sToken)!=string::npos)
        {
            MOOSRemoveChars(sLine," \t");
            MOOSChomp(sLine,"=");
            //LSQ_REJECTION = TheAvtrak : Reject = 3, History = 5,Fail = 0.001           
            CMOOSSensorChannel NewChannel;
            
            string sSensor =    MOOSChomp(sLine,":");
            string sHistory =    MOOSChomp(sLine,",");
            string sFail =        MOOSChomp(sLine,",");

            MOOSChomp(sHistory,"=");
            MOOSChomp(sFail,"=");

            
            if(sFail.empty() ||sHistory.empty())
            {
                MOOSTrace("error in %s line!\n",sToken.c_str());
                return false;
            }
            
            int nDepth = atoi(sHistory.c_str());
            if(nDepth>0)
            {
                NewChannel.SetHistoryDepth(nDepth);
            }
            

               double dfFail = atof(sFail.c_str());
            if(dfFail>0)
            {
                NewChannel.SetNoiseLimit(dfFail);
            }

            NewChannel.SetName(sSensor);

            m_SensorChannelMap[sSensor]=NewChannel;
            
        }       
    }

    return true;
}

bool CMOOSNavEngine::AddToHistory(CMOOSObservation & rObs)
{
    
    string sKey = rObs.GetName();
    
    //we now have a list of observations on this channel
    //with the most recent at the fromt of the list
    CMOOSSensorChannel * pChannel = GetSensorChannel(sKey);
    
    pChannel->Add(rObs);
    

    return true;
}


bool CMOOSNavEngine::AgreesWithHistory(CMOOSObservation &rObs)
{
    //figure out what acoustic trajectory we are talking about
    string sKey = rObs.GetName();
    
    CMOOSSensorChannel * pChannel = GetSensorChannel(sKey);
    
    if(pChannel==NULL)
    {
        //we are not running trajectory filter on this sensor
        //so we suppose its OK - definately don't want to reject it
        //otherwise we are enforcing use of a sensor channel rejection
        //scheme!
        return true; 
    }
    else
    {
        //ask the channel
        return pChannel->Agrees(rObs);
    }
        
    
}

bool CMOOSNavEngine::PreFilterData()
{
    OBSLIST::iterator p;

//    TraceObservationSet();

    //firstly see if we are accpeting this kind of data
    LimitObservationTypes();


    bool bSentProgress = false;
    for(p = m_Observations.begin();p!=m_Observations.end();p++)
    {
        
        //from most recent to oldest..
        CMOOSObservation & rObs = *p;

        //if we have already been told not to use then we had better
        //not overide this decision
        if(rObs.m_bIgnore==true)
        {
            continue;
        }
        
        //do we have filtering set up for this kind of data?
        CMOOSSensorChannel * pChannel =GetSensorChannel(rObs.GetName());
        if(pChannel==NULL)
        {
            //no then we must use it!
            rObs.Ignore(false);
            continue;
        }

        //if the observation is fixed or already marked for ignore
        //then don't use it
        if(!rObs.IsFixed() && rObs.m_bIgnore==false)
        {
            //add it to our history
            AddToHistory(rObs);
            
            //does this observation line up with its own history
            if(!AgreesWithHistory(rObs))
            {
                //maybe we are just starting up
                if(pChannel->IsOnline())
                {
                    //no we are online ....
                    MOOSTrace("SensorChannel rejecting obs[%d] %s data = %f\n",
                        rObs.m_nID,
                        rObs.GetName().c_str(),
                        rObs.m_dfData);

                    //pChannel->Trace();
                }
                else
                {
                    if(!bSentProgress && !pChannel->IsBuilt())
                    {
                        SOURCE_SENSORCHANNEL_MAP::iterator q;
                        for(q = m_SensorChannelMap.begin();q!=m_SensorChannelMap.end();q++)
                        {
                            CMOOSSensorChannel & rLocalChannel = q->second;

                            if(rLocalChannel.GetPercentFull()>0)
                            {
                                #ifdef VERBOSE
                                    AddToOutput("%s building history for %s, %f percent done",
                                        GetName().c_str(),
                                        rLocalChannel.GetName().c_str(),
                                        rLocalChannel.GetPercentFull());
                                #endif
                            }
                            
                        }
                        bSentProgress = true;
                    }
                }
                //didn't agree with history or history not yet built
                //set the ignore flag

                rObs.m_bGoodDA = false;

                //log our rejection here
                m_Logger.LogObservation(m_dfTimeNow,
                                        rObs,
                                        m_nIterations,
                                        !pChannel->IsOnline());

                rObs.m_bIgnore = true;
            }            
            else
            {
                rObs.m_bIgnore = false;
                rObs.m_bGoodDA = true;
            }
            //pChannel->Trace();
        }
    }
    
    
    return true;
}

CMOOSNavEngine::CObservationStatistic::CObservationStatistic()
{
    m_dfRejectionRate = 0;
    m_nTotal = 0;
}
bool CMOOSNavEngine::CObservationStatistic::AddPoint(bool bAccepted)
{
    double dfAlpha = 0.95;
    double dfRejected = bAccepted?0.0:1.0;
    m_nTotal++;
    m_dfRejectionRate = (100.0)*(dfAlpha*m_dfRejectionRate/100.0 + (1.0-dfAlpha)*dfRejected);

    return true;
}


bool CMOOSNavEngine::DoObservationStatistics()
{
    
    OBSLIST::iterator p;
    for(p=m_Observations.begin();p!=m_Observations.end();p++)
    {
        CMOOSObservation & rObs = *p;
        
        OBSSTATISTIC_MAP::iterator q= m_ObsStatisticsMap.find(rObs.GetName());

        if(q==m_ObsStatisticsMap.end())
        {
            CObservationStatistic NewStatistic;
            NewStatistic.m_sType = rObs.GetName();
            m_ObsStatisticsMap[rObs.GetName()] = NewStatistic;
            q= m_ObsStatisticsMap.find(rObs.GetName());
        }

        CObservationStatistic & rStat = q->second;

        rStat.AddPoint(rObs.m_bGoodDA);

    }


    return true;
}

bool CMOOSNavEngine::DoObservationSummary()
{
    OBSSTATISTIC_MAP::iterator q;
    for(q = m_ObsStatisticsMap.begin();q!=m_ObsStatisticsMap.end();q++)
    {
        CObservationStatistic & rStat = q->second;
        string sText = MOOSFormat("%s Rejecting %d percent of %s [%d]",
            GetName().c_str(),
            (int)rStat.m_dfRejectionRate,
            rStat.m_sType.c_str(),
            rStat.m_nTotal);

        AddToOutput(sText);
    }
    return true;
}
