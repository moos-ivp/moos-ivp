///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite 
//  
//   A suit of Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and 
//   Oxford University. 
//    
//   This software was written by Paul Newman at MIT 2001-2002 and Oxford 
//   University 2003-2005. email: pnewman@robots.ox.ac.uk. 
//      
//   This file is part of a  MOOS Utility Component. 
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

// SimpleAUVSim.cpp: implementation of the CMVSim class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include <MOOSGenLib/MOOSGenLib.h>
#include "AcousticResponder.h"
#include "MVSim.h"
#include "math.h"
#include <iostream>
#include <iomanip>
using namespace std;


#include "AcousticIntersection.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////




CMVSim::CMVSim()
{
    m_dfSimulatorTime = 0.0;
    m_dfLastMailed = 0.0;
    m_dfUpdateRate = 5.0;
    m_dfOldSimulatorTime = 0;

    m_dfStartTime = MOOSTime();

    m_bInitialised = false;

    m_bRealTime = true;

    SetAppFreq(10);
    SetCommsFreq(20);
}

CMVSim::~CMVSim()
{
    
}

//this will be called for us automatically
bool CMVSim::OnConnectToServer()
{
    return DoRegistrations();
}


bool CMVSim::DoRegistrations()
{
    SIM_ENTITY_LIST::iterator p;

    for(p = m_Entities.begin();p!=m_Entities.end();p++)
    {
        CSimEntity* pEntity = *p;

        CSixDOFAUV* pAUV = dynamic_cast<CSixDOFAUV*> (pEntity);

        if(pAUV)
        {
            m_Comms.Register(pAUV->m_sInputPrefix+"DESIRED_THRUST",0.0);
            m_Comms.Register(pAUV->m_sInputPrefix+"DESIRED_ELEVATOR",0.02);
            m_Comms.Register(pAUV->m_sInputPrefix+"DESIRED_RUDDER",0.02);
        }
    }


    m_Comms.Register("SIM_RESET",0.02);
    m_Comms.Notify("SIMULATION_MODE","TRUE");

    return true;
}

bool CMVSim::OnNewMail(MOOSMSG_LIST &NewMail)
{
    //someone has changed one of the variable we were intersted in!
    MOOSMSG_LIST::iterator p;

 

    for(p=NewMail.begin();p!=NewMail.end();p++)
    {
       
        CMOOSMsg & rMsg = *p;

        CSixDOFAUV * pAUV = GetSubscriber(rMsg.m_sKey);

        if(pAUV)
        {
            //we now know which AUV wants this control data..
            if(rMsg.m_sKey.find("DESIRED_ELEVATOR")!=string::npos)
            {
                pAUV->m_dfElevator = MOOSDeg2Rad(rMsg.m_dfVal);
                continue;
            }
            else if(rMsg.m_sKey.find("DESIRED_RUDDER")!=string::npos)
            {
                pAUV->m_dfRudder = MOOSDeg2Rad(rMsg.m_dfVal);
                continue;
            }
            else if(rMsg.m_sKey.find("DESIRED_THRUST")!=string::npos)
            {
                pAUV->m_dfThrust = rMsg.m_dfVal;
                continue;
            }
        }
    
        if(MOOSStrCmp(rMsg.m_sKey,"SIM_RESET"))
        {
            Initialise();
        }
    }

    return true;
}


// who of teh AUVs want to rx this data (look at prefixes)
CSixDOFAUV * CMVSim::GetSubscriber(const std::string & sName)
{
    //index to entities...
    SIM_ENTITY_LIST::iterator p;

    for(p = m_Entities.begin();p!=m_Entities.end();p++)
    {
        CSimEntity* pEntity = *p;

        CSixDOFAUV* pAUV = dynamic_cast<CSixDOFAUV*> (pEntity);

        if(pAUV)
        {
            std::string sPrefix = pAUV->m_sInputPrefix;

            if(MOOSStrCmp(sName.substr(0,sPrefix.length()),sPrefix))
            {
                return pAUV;
            }
        }
    }

    return NULL;
}

bool CMVSim::Iterate()
{
    
  
    //have we initialised?
    if(!m_bInitialised)
    {

        Initialise();
    }


    ///////////////////////////////////////
    //          sort timing out
    ///////////////////////////////////////
    
    double dfDT  = 0;

    //here we decide what the increment in time is...
    if(m_bRealTime)
    {
        //we will go at real world speed.
        dfDT     = MOOSTime()-m_dfSimulatorTime;
        

        if(dfDT==0)
        {
            return true;
        }
    }
    else
    {
        //coarse DT is 0.1 seconds..good as any...
        dfDT = 0.1;
        SetAppFreq(int(1/dfDT));
    }
    

    //the epoch ends at:
    double dfEndTime    =   m_dfSimulatorTime +dfDT;    


    //but we want a fine granularity within this epoch of
    //better than 10 ms.
    //and within this we shall perform linear interpolation.
    double dfSmallDT    =   dfDT;
    int nDiv            =   1;

    do
    {

        dfSmallDT = dfDT/(nDiv++);

    }while(dfSmallDT>0.01);



    /////////////////////////////////////////////////
    //          now propagate models.....
    /////////////////////////////////////////////////
    
    while(m_dfSimulatorTime<=dfEndTime)
    {

        //index to entities...
        SIM_ENTITY_LIST::iterator p;


        //move all entities - Newton(1:3) still rule even in the
        //new millenium 
        for(p = m_Entities.begin();p!=m_Entities.end();p++)
        {
            CSimEntity* pEntity = *p;
            pEntity->Iterate(m_dfSimulatorTime,dfSmallDT);
        }

        //MOOSTrace("\n\nAbout To Solve Acoustics\n");
        //look for intersection with signals
        for(p = m_Entities.begin();p!=m_Entities.end();p++)
        {
            CSimEntity* pEntity = *p;

            pEntity->SolveAcoustics(m_dfSimulatorTime,dfSmallDT);

        }

        //move just a wee bit forward in time...
        m_dfSimulatorTime+=dfSmallDT;
    
    }//completed epoch...



    // housekeeping  - remove old pings in the water column..
    m_Environment.RemoveOldSignals(m_dfSimulatorTime);



    //////////////////////////////////////
    // Tell the world what has happened //
    //////////////////////////////////////
    
    //MOOSTrace("Posting @ %f\n",MOOSTime());
    PostResults();



    return true;
}





bool CMVSim::NeedToMail()
{
    return m_dfSimulatorTime-m_dfLastMailed>1.0/m_dfUpdateRate;

}


bool CMVSim::OnStartUp()
{
    Initialise();

    DoRegistrations();

    return true;
}

bool CMVSim::Initialise()
{



    Clean();

    SetAppFreq(50);

    m_dfLastMailed = 0.0;

 
    if(m_bRealTime)
    {
        m_dfStartTime = MOOSTime();
        m_dfSimulatorTime = m_dfStartTime;
    }
    else
    {
        m_dfAppStartTime=0.0;
        m_dfStartTime = 0.0;
        m_dfSimulatorTime = 0.0;
    }
    m_dfOldSimulatorTime = m_dfSimulatorTime;

    m_Environment.SetStartTime(m_dfSimulatorTime);
    
    //now load in terrain data
    string sTerrainFile= "terrain.dat";
    m_MissionReader.GetValue("TerrainFile",sTerrainFile);
    
    if(!m_Environment.Initialise(sTerrainFile.c_str()))
    {
        MOOSTrace("warning environment failed to build\n");
    }

    // load general parameters from process config block
    STRING_LIST sParams;
    m_MissionReader.GetConfiguration(GetAppName(),sParams);
    m_Params.Load(sParams);

    //load some others in
    m_MissionReader.GetConfigurationParam("TideHeight",m_Environment.m_dfTideHeight);
    m_MissionReader.GetValue("MagneticOffset",m_Environment.m_dfMagneticOffset);



    //populate with vehicles and beacons
    STRING_LIST::iterator p;
    for(p = sParams.begin();p!=sParams.end();p++)
    {
        std::string sTmp = *p;
        std::string sTok,sVal;
        CMOOSFileReader::GetTokenValPair(sTmp,sTok,sVal);
        if(MOOSStrCmp(sTok,"ADD_AUV"))
        {
            MakeAUV(sVal);
        }
        else if(MOOSStrCmp(sTok,"ADD_TRANSPONDER"))
        {
            MakeBeacon(sVal);
        }
    }


    //open log file
    OpenLogFile();

    //log all entities' initial state
    LogStartConditions();


    m_bInitialised = true;

    return true;
}




void CMVSim::PostResults()
{
    if(NeedToMail())
    {
        //index to entities...
        SIM_ENTITY_LIST::iterator p;
        
        for(p = m_Entities.begin();p!=m_Entities.end();p++)
        {
            CSimEntity* pEntity = *p;
            
            CSixDOFAUV* pAUV = dynamic_cast<CSixDOFAUV*> (pEntity);
            
            if(pAUV==NULL)
                continue;
            
            double dfX = pAUV->GetX();
            if(m_Params.m_bAddNoise)
                dfX+=MOOSWhiteNoise(m_Params.m_dfXYStd);
            
            double dfY = pAUV->GetY();
            if(m_Params.m_bAddNoise)
                dfY+=MOOSWhiteNoise(m_Params.m_dfXYStd);
            
            double dfZ = pAUV->GetZ();
            if(m_Params.m_bAddNoise)
                dfZ+=MOOSWhiteNoise(m_Params.m_dfZStd);
            
            double dfYaw = pAUV->GetYaw();
            if(m_Params.m_bAddNoise)
            {
                dfYaw+=MOOSWhiteNoise(m_Params.m_dfYawStd);
                dfYaw+=m_Params.m_dfYawBias;
            }
            
            double dfDepth = pAUV->GetDepth();
            if(m_Params.m_bAddNoise)
                dfDepth+=MOOSWhiteNoise(m_Params.m_dfZStd);
            
            double dfBodyVelY = pAUV->GetBodyVelY();
            if(m_Params.m_bAddNoise)
                dfBodyVelY+=MOOSWhiteNoise(m_Params.m_dfXYVelStd);
            
            double dfBodyVelX = pAUV->GetBodyVelX();
            if(m_Params.m_bAddNoise)
                dfBodyVelX+=MOOSWhiteNoise(m_Params.m_dfXYVelStd);
            
            double dfSpeed = pAUV->GetSpeed();
            if(m_Params.m_bAddNoise)
                dfSpeed=hypot(dfBodyVelY,dfBodyVelX);
            
            double dfHeading = pAUV->GetHeading();
            if(m_Params.m_bAddNoise)
                dfHeading = -MOOSRad2Deg(dfYaw);
            
            std::string sP = pAUV->m_sOutputPrefix;

            m_Comms.Notify(sP+"X",dfX);
            m_Comms.Notify(sP+"Y",dfY);
            m_Comms.Notify(sP+"Z",dfZ);
            m_Comms.Notify(sP+"YAW",dfYaw);
            m_Comms.Notify(sP+"DEPTH",dfDepth);
            m_Comms.Notify(sP+"SPEED",dfSpeed);
            m_Comms.Notify(sP+"HEADING",dfHeading);
            m_Comms.Notify(sP+"BODY_VEL_Y",dfBodyVelY);
            m_Comms.Notify(sP+"BODY_VEL_X",dfBodyVelX);
            m_Comms.Notify(sP+"PITCH",pAUV->GetPitch());
                        
            
            if(pAUV->GetAltitude()>=0)
            {
                m_Comms.Notify(sP+"ALTITUDE",pAUV->GetAltitude());
            }        
            
        }    

        m_Comms.Notify("TIDE_HEIGHT",m_Environment.GetTideHeight());
        m_dfLastMailed = m_dfSimulatorTime;

    }

        //finally get all Msg that have appeared in Environments
        //out box, this allows objects from anywhere to send notifications
        MOOSMSG_LIST::iterator q;
        for(q = m_Environment.m_MailOut.begin();q!=m_Environment.m_MailOut.end();q++)
        {
            //MOOSTrace("Sending Env :%s\n",q->GetString().c_str());
            m_Comms.Post(*q);
        }

        if(!m_Environment.m_MailOut.empty())
        {
//            MOOSTrace("Sent %d Mesg from Environment\n",m_Environment.m_MailOut.size());
            m_Environment.m_MailOut.clear();
        }


        //log all entities
        SIM_ENTITY_LIST::iterator p;
        for(p = m_Entities.begin();p!=m_Entities.end();p++)
        {
            (*p)->LogState(m_dfSimulatorTime);
        }




}


bool CMVSim::Clean()
{
    SIM_ENTITY_LIST::iterator p;


    //move all entities
    for(p = m_Entities.begin();p!=m_Entities.end();p++)
    {
        CSimEntity* pEntity = *p;
        delete pEntity;
    }
    m_Entities.clear();
    
    m_Environment.Clean();

    return true;
}

/// Simply make an AUV object
bool CMVSim::MakeAUV(std::string sConfig)
{

    //ADD_AUV=  pose=[3x1]{7,3,4,5},name = AUV1,InputPrefix=AUV1,OutputPrefix=AUV1
    CSixDOFAUV * pAUV = new CSixDOFAUV; 
    try
    {
        //give it a parameter block
        pAUV->SetParams(&m_Params);
        
        pAUV->Initialise();
        
        //where is it?
        std::vector<double> T; int nR,nC;
        if(MOOSValFromString(T,nR,nC,sConfig,"pose"))
        {
            if(nR !=4)
            {
                throw CMOOSException(MOOSFormat("pose must be a 4 vector %s\n",MOOSHERE));
            }
            pAUV->SetX(T[0]);
            pAUV->SetY(T[1]);
            pAUV->SetZ(T[2]);
            pAUV->SetYaw(T[3]);
            
        }
        
        //whats it called
        std::string sName;
        if(!MOOSValFromString(sName,sConfig,"name"))
            throw CMOOSException(MOOSFormat("AUV has no name! %s\n",MOOSHERE));
        
        
        pAUV->SetName(sName);
        
        
        //how will control be sent to it and positions be emmited?
        std::string sInputPrefix="SIM"; //this would mean it Rxs SIM_DESIRED_RUDDER
        std::string sOutputPrefix="SIM";//and publishes SIM_X etc...
        
        if(!MOOSValFromString(sInputPrefix,sConfig,"InputPrefix"))
        {
            throw CMOOSException(MOOSFormat("No Input prefix defined - assuming  %s\n",sInputPrefix.c_str()));
        }
        if(!MOOSValFromString(sOutputPrefix,sConfig,"OutputPrefix"))
        {
            throw CMOOSException(MOOSFormat("No OutputPrefix  defined - assuming  %s\n",sOutputPrefix.c_str()));
        }
        
        pAUV->SetInputPrefix(sInputPrefix);
        pAUV->SetOutputPrefix(sOutputPrefix);
        
        //tell it about teh world
        pAUV->SetEnvironment(&m_Environment);

        //what channel does it reply to interrogations on?
        std::string  sChannelResponder = "Ch2";
        if(!MOOSValFromString(sChannelResponder,sConfig,"ResponderChannel"))
        {
            MOOSTrace("No ResponderChannel  defined - assuming no Responder required \n");

            //turn it off - other parameters don't matter
            pAUV->ConfigureResponder(false,ACOUSTIC_CHAN_CIF,ACOUSTIC_CHAN_CIF,-1);

        }
        else
        {
            AcousticChannel eCh = CAcousticSignal::ChannelFromString(sChannelResponder);
                        
            //what is its TAT on interrogations?
            double dfTAT = 0.125;
            if(!MOOSValFromString(dfTAT,sConfig,"TAT"))
            {
                throw CMOOSException(MOOSFormat("No TAT defined - assuming  %f\n",dfTAT));
            }
                        
            //turn it on..
            pAUV->ConfigureResponder(true,ACOUSTIC_CHAN_CIF,eCh,dfTAT);
        }
        //store it
        m_Entities.push_front(pAUV);
    }
    catch(CMOOSException e)
    {
        delete pAUV;
        return MOOSFail(e.m_sReason);
    }

    return true;


}



bool CMVSim::MakeBeacon(std::string sConfig)
{
    //"ADD_TRANSPONDER = Name = B1,pose=[3x1]{23, 45, 0},TAT = 0.125,RX = CH8 | CH9 | C10,TX = CIF

    CAcousticBeacon * pBeacon = new CAcousticBeacon;
    try
    {

        std::vector<double> T; int nR,nC;
        if(MOOSValFromString(T,nR,nC,sConfig,"pose"))
        {
            if(nR !=3)
            {
                return MOOSFail("pose must be a 3 vector %s\n",MOOSHERE);
            }
            pBeacon->m_Pos_e<<T[0]
                <<T[1]
                <<T[2]
                <<0
                <<0
                <<0;
        }
        
        //whats it called
        std::string sName;
        if(!MOOSValFromString(sName,sConfig,"name"))
            throw CMOOSException(MOOSFormat("Beacon has no name! %s %s\n",sConfig.c_str(),MOOSHERE));
        pBeacon->SetName(sName);
        
        std::string sTx;
        if(!MOOSValFromString(sTx,sConfig,"Tx"))
            throw CMOOSException(MOOSFormat("Beacon has no Tx field %s %s\n",sConfig.c_str(),MOOSHERE));
        
        
        ACOUSTIC_NODE_LIST AcousticNodes;
        pBeacon->GetAcousticNodes(AcousticNodes);
        
        AcousticChannel eChan = CAcousticSignal::ChannelFromString(sTx);
        
        if( eChan==ACOUSTIC_CHAN_ERROR ||
            !AcousticNodes.front()->SetTxChan(eChan))
        {
            throw CMOOSException(MOOSFormat("error setting Tx Channel on beacon %s\n",sName.c_str()));
        }
        
        std::string sRx;
        if(!MOOSValFromString(sTx,sConfig,"Rx"))
            throw CMOOSException(MOOSFormat("Beacon has no Rx field %s %s\n",sConfig.c_str(),MOOSHERE));
        
        
        
        while(!sRx.empty())
        {
            string sChan = MOOSChomp(sRx,"|");
            
            AcousticChannel eChan = CAcousticSignal::ChannelFromString(sChan);
            
            if( eChan==ACOUSTIC_CHAN_ERROR ||
                !AcousticNodes.front()->SetRxChan(eChan,true))
            { 
                MOOSTrace("error setting Rx Channel on beacon\n");
            }
        }//for all channels
        
        
        ////////////////////////////
        //      set up TAT
        ////////////////////////////
        
        double dfTAT;
        if(!MOOSValFromString(dfTAT,sConfig,"TAT"))
            throw CMOOSException(MOOSFormat("Beacon has no TAT field %s %s\n",sConfig.c_str(),MOOSHERE));
        
        
        CAcousticResponder * pResponder = dynamic_cast<CAcousticResponder*>(AcousticNodes.front());
        if(pResponder!=NULL)
        {
            pResponder->SetTAT(dfTAT);
        }
        
        //final set up of beacon...
        pBeacon->SetEnvironment(&m_Environment);
        
        pBeacon->SetParams(&m_Params);
        
        m_Entities.push_front(pBeacon);
    }
    catch (CMOOSException e)
    {
        delete pBeacon;
        return MOOSFail(e.m_sReason);
    }

    return true;
}            
    


bool CMVSim::OpenLogFile()
{
    //set up log file...
    if(!m_Params.m_sLogFileName.empty())
    {
        //open and close ..truncates file
        ofstream os(m_Params.m_sLogFileName.c_str());
        
        os.close();
    }
    return true;
}

bool CMVSim::LogStartConditions()
{
    SIM_ENTITY_LIST::iterator p;

    if(!m_Params.m_sLogFileName.empty())
    {
        //open and close ..truncates file
        ofstream os(m_Params.m_sLogFileName.c_str(),ios::app);

        //start time...
        os.setf(ios::fixed,ios::floatfield);
        os<<setprecision(3);
        os<<"StartTime,T="<<setprecision(3)<<m_dfSimulatorTime<<endl;

        //entity states
        for(p = m_Entities.begin();p!=m_Entities.end();p++)
        {
            (*p)->CSimEntity::LogState(m_dfSimulatorTime);
        }


    }

    return true;
}
