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
//   This file is part of a  MOOS Instrument. 
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
// DepthInstrument.cpp: implementation of the CDepthInstrument class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>

#include "DepthInstrument.h"
#include "MOOSConsiDepthSensor.h"
#include "MOOSParaSciDepthSensor.h"

#include <iostream>
#include <sstream>
#include <math.h>

#include <algorithm>

#define MAX_SANE_DEPTH 4000.0
#define MAX_SANE_DELTA_DEPTH 2.0
#define DEPTH_HISTORY_LENGTH 10
using namespace std;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDepthInstrument::CDepthInstrument()
{
    //some sensible defaults (missionfile can overwrite this)
    SetAppFreq(2);
    SetCommsFreq(8);    
    m_nHistoryLength = DEPTH_HISTORY_LENGTH;
    m_bFilter = true;
}

CDepthInstrument::~CDepthInstrument() 
{
    
}

bool CDepthInstrument::OnNewMail(MOOSMSG_LIST &NewMail)
{
    
    if(IsSimulateMode())
    {
        return UpdateMOOSVariables(NewMail);
    }
    else
    {
        CMOOSMsg Msg;
        if(m_Comms.PeekMail(NewMail,"ZERO_DEPTH",Msg))
        {
            m_pDepthSensor->Zero();
        }
    }
    
    
    return true;
}


/////////////////////////////////////////////
///this is where it all happens..
bool CDepthInstrument::Iterate()
{
    
    if(GetDepth())
    {
        PublishDepth();
    }
    return true;
}


bool CDepthInstrument::OnStartUp()
{
    CMOOSInstrument::OnStartUp();
    
    m_MissionReader.GetConfigurationParam("FILTER",m_bFilter);

    //what is our MOOS variable?
    AddMOOSVariable("Depth",    //locally called depth
        "SIM_DEPTH",//subscribed to as SIM_DEPTH (iProcesses subscribed via the simulator!
        "DEPTH_DEPTH",//
                    0.5);        //can update @10Hz                
    
    AddMOOSVariable("DEPTH_RAW","","DEPTH_RAW",0.1);
    
    if(IsSimulateMode())
    {
        RegisterMOOSVariables();
        SetAppFreq(5);
        SetCommsFreq(10);    
    }
    
    else
    {
        //try to open 
        if(!SetupPort())
        {
            return false;
        }
        
        string sType;
        m_MissionReader.GetConfigurationParam("TYPE",sType);
        
        if(MOOSStrCmp(sType,"CONSI"))
        {
            m_pDepthSensor = new CMOOSConsiDepthSensor;
        }
        else if (MOOSStrCmp(sType,"PARASCI"))
        {
            m_pDepthSensor = new CMOOSParaSciDepthSensor;
        }
        else
        {
            MOOSTrace("Depth sensor type must be one of PARASCI or CONSI \n");
            MOOSTrace("Assuming ParaSci...! \n");
            m_pDepthSensor = new CMOOSParaSciDepthSensor;
        }
        
        
        if(dynamic_cast<CMOOSParaSciDepthSensor *> (m_pDepthSensor) )
        {
            double dfResolution;
            
            if(m_MissionReader.GetConfigurationParam("RESOLUTION",dfResolution))
            {
                
                ((CMOOSParaSciDepthSensor *)m_pDepthSensor)->SetResolution(dfResolution);
            }
        }
        
        //tell teh driver what port to use..
        m_pDepthSensor->SetSerialPort(&m_Port);
        
        //try 10 times to initialise sensor
        if(!InitialiseSensorN(10,"Depth"))
        {
            return false;
        }          
        
    }
    
    return true;
}





bool CDepthInstrument::OnConnectToServer()
{
    
    if(IsSimulateMode())
    {
        //we only subscribe to things if we are in simulator mode
        RegisterMOOSVariables();
    }
    else
    {
        m_Comms.Register("ZERO_DEPTH",0.5);
    }
    
    return true;
}

bool CDepthInstrument::GetDepth()
{
    
    //we only do this if there is no simulator connected and
    if(!IsSimulateMode())
    {
        if(m_pDepthSensor->GetDepth())
        {
            double dfDepth = m_pDepthSensor->GetDepthValue();
            if(fabs(dfDepth)<MAX_SANE_DEPTH)
            {
                if(Filter(dfDepth))
                {
                    SetMOOSVar("Depth",dfDepth,MOOSTime());
                }
            }
            else
            {
                string sCrazy = MOOSFormat("Crazy Depth: %fm",dfDepth);
                MOOSDebugWrite(sCrazy);
            }
        }
    }
    else
    {
        /*testing filtering offline
        CMOOSVariable * pVar = GetMOOSVar("Depth");
        if(pVar->IsFresh())
        {
            double dfDepth = pVar->GetDoubleVal();
            Filter(dfDepth);
        }*/
    }
    
    
    return true;
}

////////////////////////////////////////////////////////////
// tell the world
bool CDepthInstrument::PublishDepth()
{
    return PublishFreshMOOSVariables();
    
}

///////////////////////////////////////////////////////////////////////////
// here we initialise the sensor, giving it start up values
bool CDepthInstrument::InitialiseSensor()
{
    return m_pDepthSensor->Initialise();      
    
}



bool CDepthInstrument::Filter(double dfDepth)
{
    //no filtering -> accept
    if(!m_bFilter)
        return true;


    DEPTH_HISTORY::iterator p;

    m_DepthHistory.push_front(dfDepth);

    //not enough history -> reject
    if(m_DepthHistory.size()<m_nHistoryLength)
    {
        return false;
    }

    //don't grow for too long
    while(m_DepthHistory.size()>m_nHistoryLength)
    {
        m_DepthHistory.pop_back();
    }

    //copy list to vector
    vector<double> LocalVec;
    LocalVec.resize(m_DepthHistory.size());



    copy(m_DepthHistory.begin(),m_DepthHistory.end(),LocalVec.begin());

    sort(LocalVec.begin(),LocalVec.end());

    
    double dfMedian = LocalVec[m_DepthHistory.size()/2];

    bool bAccept = fabs(dfDepth-dfMedian)<MAX_SANE_DELTA_DEPTH;
    
    if(!bAccept)
    {
        string sText = MOOSFormat("Depth Filter rejects %f m",dfDepth);
        MOOSDebugWrite(sText);
    }
    return bAccept;
    
}
