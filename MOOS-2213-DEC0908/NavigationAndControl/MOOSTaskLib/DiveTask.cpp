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
// DiveTask.cpp: implementation of the CDiveTask class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

#include "MOOSTaskDefaults.h"

#include "DiveTask.h"


#define CHUCK_DIVE_MAX 3.0

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDiveTask::CDiveTask()
{
    m_bReverseDive = false;
    m_bChuckDive = false;
    m_bInitialised = false;
    m_ZPID.SetReversing(m_bReverseDive);
    m_dfTimeOut = 60;

    m_Elevator.SetCurrent(2,MOOSTime());
    m_Rudder.SetCurrent(0,MOOSTime());
    m_Thrust.SetCurrent(20,MOOSTime());
    m_DepthDOF.SetDesired(CHUCK_DIVE_MAX);
}

CDiveTask::~CDiveTask()
{

}


bool CDiveTask::SetParam(string sParam, string sVal)
{

    MOOSToUpper(sParam);
    MOOSToUpper(sVal);


    if(!CMOOSBehaviour::SetParam(sParam,sVal))
    {
        //this is for us...
        if(MOOSStrCmp(sParam,"RUDDER"))
        {
            m_Rudder.SetCurrent(atof(sVal.c_str()),MOOSTime());
        }
        
        //this is for us...
        else if(MOOSStrCmp(sParam,"ELEVATOR"))
        {
            m_Elevator.SetCurrent(atof(sVal.c_str()),MOOSTime());
        }

        
        //this is for us...
        else if(MOOSStrCmp(sParam,"DEPTH"))
        {
            m_DepthDOF.SetDesired(atof(sVal.c_str()));
        }


        else if(MOOSStrCmp(sParam,"THRUST"))
        {
            m_Thrust.SetCurrent(atof(sVal.c_str()),MOOSTime());
        }

        else if(MOOSStrCmp(sParam,"MODE"))
        {
            if(MOOSStrCmp(sVal,"REVERSE"))
            {
                m_bReverseDive = true;
            }
            else if(MOOSStrCmp(sVal,"CHUCK"))
            {
                m_bChuckDive = true;
                m_bReverseDive = true;
            }
        }

        else
        {
            //hmmm - it wasn't for us at all: base class didn't understand either
            MOOSTrace("Param \"%s\" not understood!\n",sParam.c_str());
            return false;
        }
    }

    return true;

}

bool CDiveTask::GetNotifications(MOOSMSG_LIST & List)
{
    if(m_bActive)
    {
        CMOOSMsg Msg(MOOS_NOTIFY,
         "DESIRED_PITCH",
         m_ZPID.GetPitchDesired());

        List.push_back(Msg);
    }

    return CMOOSBehaviour::GetNotifications(List);
}



//returns false if we haven't received data in a while..bad news!
bool CDiveTask::RegularMailDelivery(double dfTimeNow)
{
    return !(
        m_DepthDOF.IsStale(dfTimeNow,GetStartTime()) ||
        m_PitchDOF.IsStale(dfTimeNow,GetStartTime())
        );
}



bool CDiveTask::Initialise()
{

    //set a pitch driven depth controller
    m_ZPID.SetAsDepthController(true);
    
    m_bInitialised = true;

    return true;
}

bool CDiveTask::Run(CPathAction &DesiredAction)
{

    if(!m_bInitialised)
    {
        Initialise();
    }
    
    if(!m_DepthDOF.IsValid() || !m_PitchDOF.IsValid())
    {
        return false;
    }
    double dfCmd= 0;

    if(m_bChuckDive)
    {
        //an open loop dive
        if(m_DepthDOF.GetCurrent()>CHUCK_DIVE_MAX)
        {
            Stop("Chuck Dive Went Too Deep");
            return true;
        }


        if(m_DepthDOF.GetError()>0 && MOOSTime()-m_dfStartTime<m_dfTimeOut*0.7)
        {
            //we haven't reached our depth yet and still have more than
            //30% of dive time left..
            dfCmd = m_Elevator.GetCurrent();
        }
        else
        {
            //we are in levelling stage..

            //look for roughly horizontal (nose down) pose
            if(m_PitchDOF.GetCurrent()<0)
            {
                Stop("Chuck Dive Complete");        
                return true;
            }

            dfCmd = -m_Elevator.GetCurrent();

        }
    }
    else
    {
        //use PID controler to figure stuff out    
        double dfError = m_DepthDOF.GetError();
        
        if(dfError<0)
        {
            //we have reached our desired depth..
            //break...
            Stop("Dive Complete");
            return true;
        }

    
        if(!m_ZPID.Run(    dfError,
                            m_DepthDOF.GetErrorTime(),
                            dfCmd,
                            m_PitchDOF.GetCurrent(),
                            m_PitchDOF.GetErrorTime()))
        {
            return false;
        }
    }
    
    double dfSign = m_bReverseDive ? -1 : 1;


    //OK we need to change something
    DesiredAction.Set(  ACTUATOR_THRUST,
                        dfSign*m_Thrust.GetCurrent(),
                        m_nPriority,
                        "Dive Depth");

    DesiredAction.Set(  ACTUATOR_RUDDER,
                        m_Rudder.GetCurrent(),
                        m_nPriority,
                        "Dive Depth");

    DesiredAction.Set(  ACTUATOR_ELEVATOR,
                        dfCmd,
                        m_nPriority,
                        "Dive Depth");
    
    return true;
}

bool CDiveTask::OnNewMail(MOOSMSG_LIST &NewMail)
{
    CMOOSMsg Msg;
    
    if(PeekMail(NewMail,"NAV_DEPTH",Msg))
    {
        if(!Msg.IsSkewed(GetTimeNow()))
        {
            m_DepthDOF.SetCurrent(Msg.m_dfVal,Msg.m_dfTime);
        }
    }

    if(PeekMail(NewMail,"NAV_PITCH",Msg))
    {
        if(!Msg.IsSkewed(GetTimeNow()))
        {
            m_PitchDOF.SetCurrent(Msg.m_dfVal,Msg.m_dfTime);
        }
    }


    //always call base class version
    CMOOSBehaviour::OnNewMail(NewMail);

    return true;
}

bool CDiveTask::GetRegistrations(STRING_LIST &List)
{

    List.push_front("NAV_DEPTH");
    List.push_front("NAV_PITCH");

    //always call base class version
    CMOOSBehaviour::GetRegistrations(List);

    return true;
}



