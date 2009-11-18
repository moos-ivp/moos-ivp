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
// ThirdpartyTask.cpp: implementation of the CThirdPartyTask class.
//
//////////////////////////////////////////////////////////////////////

#include "ThirdPartyTask.h"

#define THIRDPARTY_STALE 10.0
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThirdPartyTask::CThirdPartyTask()
{
    m_bInitialised = false;
    m_bEnabled = false;
}

CThirdPartyTask::~CThirdPartyTask()
{

}

//returns false if we haven't received data in a while..bad news!
bool CThirdPartyTask::RegularMailDelivery(double dfTimeNow)
{
    return true;

    
}
bool CThirdPartyTask::SetParam(string sParam, string sVal)
{

    if(!CMOOSBehaviour::SetParam(sParam,sVal))
    {
        //this is for us...
        if(MOOSStrCmp(sParam,"CLIENT"))
        {
            //format is what@who            
            m_sClient = sVal;
        }
        else if(MOOSStrCmp(sParam,"JOB"))
        {
            m_sJob = sVal;
        }
    }
    return true;
}

bool CThirdPartyTask::OnNewMail(MOOSMSG_LIST &NewMail)
{
    CMOOSMsg Msg;

    if(PeekMail(NewMail,m_sJob,Msg) && Msg.m_sSrc==m_sClient)
    {
        if(!Msg.IsSkewed(GetTimeNow()))
        {
            return OnNewInstruction(Msg.m_sVal,Msg.m_dfTime);
        }
    }

    return true;
}

bool CThirdPartyTask::GetNotifications(MOOSMSG_LIST & List)
{
    if(m_bActive)
    {
    }

    return CMOOSBehaviour::GetNotifications(List);
}



bool CThirdPartyTask::GetRegistrations(STRING_LIST &List)
{

    List.push_front(m_sJob);

    //always call base class version
    CMOOSBehaviour::GetRegistrations(List);

    return true;
}


bool CThirdPartyTask::Initialise()
{
    Enable(true);
    m_bInitialised = true;
    return true;
}


bool CThirdPartyTask::Run(CPathAction &DesiredAction)
{

    if(!m_bInitialised)
    {
        Initialise();
    }

    if(!m_bEnabled)
        return true;


    if(m_Rudder.GetAge(GetTimeNow())<THIRDPARTY_STALE)
    {
        DesiredAction.Set(  ACTUATOR_RUDDER,
                            m_Rudder.GetDoubleVal(),
                            m_nPriority,
                            m_sJob.c_str());
    }

    if(m_Elevator.GetAge(GetTimeNow())<THIRDPARTY_STALE)
    {
        DesiredAction.Set(  ACTUATOR_ELEVATOR,
                            m_Elevator.GetDoubleVal(),
                            m_nPriority,
                            m_sJob.c_str());
    }

    if(m_Thrust.GetAge(GetTimeNow())<THIRDPARTY_STALE)
    {
        DesiredAction.Set(  ACTUATOR_THRUST,
                            m_Thrust.GetDoubleVal(),
                            m_nPriority,
                            m_sJob.c_str());
    }

    return true;
}

bool CThirdPartyTask::OnNewInstruction(string sInstruction,double dfTimeNow)
{
    MOOSToUpper(sInstruction);
    MOOSRemoveChars(sInstruction," \t");

    if(sInstruction.find("ACTUATION:")!=string::npos)
    {
        return OnActuationInstruction(sInstruction,dfTimeNow);
    }
    else if(sInstruction.find("STATUS:")!=string::npos)
    {
        //eg "STATUS:ENABLE=TRUE"
        return OnStatusInstruction(sInstruction,dfTimeNow);
    }
    else
    {
        MOOSTrace("Unknown thirdparty command! : %s",sInstruction.c_str());
        return false;
    }

 
}

bool CThirdPartyTask::OnActuationInstruction(string sInstruction, double dfTimeNow)
{
   //instruction is of form
    //"RUDDER=9,ELEVATOR = 0,THRUST = 90"
    string sCopy = sInstruction;

    MOOSChomp(sCopy,"RUDDER=");
    if(!sCopy.empty())
    {
        string sRudder = MOOSChomp(sCopy,",");
        if(!sRudder.empty())
        {
            double dfRudder = atof(sRudder.c_str());
            m_Rudder.Set(dfRudder,dfTimeNow);        
        }
    }

    sCopy = sInstruction;
    MOOSChomp(sCopy,"ELEVATOR=");
    if(!sCopy.empty())
    {
        string sElevator = MOOSChomp(sCopy,",");
        if(!sElevator.empty())
        {
            double dfElevator = atof(sElevator.c_str());
            m_Elevator.Set(dfElevator,dfTimeNow);        
        }
    }

    sCopy = sInstruction;
    MOOSChomp(sCopy,"THRUST=");
    if(!sCopy.empty())
    {
        string sThrust = MOOSChomp(sCopy,",");
        if(!sThrust.empty())
        {
            double dfThrust = atof(sThrust.c_str());
            m_Thrust.Set(dfThrust,dfTimeNow);        
        }
    }


    return true;
}

bool CThirdPartyTask::OnStatusInstruction(string sInstruction, double dfTimeNow)
{
    if(sInstruction.find("ENABLE=TRUE")!=string::npos)
    {
      Enable(true);
    }
    if(sInstruction.find("ENABLE=FALSE")!=string::npos)
    {
      Enable(false);
    }

    return true;
}

bool CThirdPartyTask::Enable(bool bEnable)
{
    string sMsg = MOOSFormat("%s Thridparty Task %s job %s\n",
                            bEnable?"Enabling":"Disabling",
                            GetName().c_str(),
                            m_sJob.c_str());
        
     DebugNotify(sMsg);
        
     m_bEnabled = bEnable;

     return true;
}
