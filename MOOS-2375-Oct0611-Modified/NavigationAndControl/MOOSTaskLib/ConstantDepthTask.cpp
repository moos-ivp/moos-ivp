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
// ConstantDepthTask.cpp: implementation of the CConstantDepthTask class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include <math.h>
#include <iostream>
using namespace std;

#include "MOOSTaskDefaults.h"
#include "ConstantDepthTask.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConstantDepthTask::CConstantDepthTask()
{
  
    m_bInitialised = false;
    m_DepthDOF.SetDesired(0.2);
}

CConstantDepthTask::~CConstantDepthTask()
{

}

//returns false if we haven't received data in a while..bad news!
bool CConstantDepthTask::RegularMailDelivery(double dfTimeNow)
{
    return !m_DepthDOF.IsStale(dfTimeNow,GetStartTime());
}



bool CConstantDepthTask::Run(CPathAction &DesiredAction)
{

    if(!m_bInitialised)
    {
        Initialise();
    }
    
    if(m_DepthDOF.IsValid() && m_PitchDOF.IsValid())
    {
        double dfError = m_DepthDOF.GetError();

        double dfCmd= 0;

        //this is for logging purposes only
        m_ZPID.SetGoal(m_DepthDOF.GetDesired());


        if(m_ZPID.Run(dfError,
                        m_DepthDOF.GetErrorTime(),
                        dfCmd,m_PitchDOF.GetCurrent(),
                        m_PitchDOF.GetErrorTime()))
    {
        //OK we need to change something
            DesiredAction.Set(  ACTUATOR_ELEVATOR,
                                dfCmd,
                                m_nPriority,
                                m_sName.c_str());


        
        
    }
    }
    return true;
}

bool CConstantDepthTask::OnNewMail(MOOSMSG_LIST &NewMail)
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

bool CConstantDepthTask::GetNotifications(MOOSMSG_LIST & List)
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

bool CConstantDepthTask::GetRegistrations(STRING_LIST &List)
{

    List.push_front("NAV_DEPTH");
    List.push_front("NAV_PITCH");

    //always call base class version
    CMOOSBehaviour::GetRegistrations(List);

    return true;
}


bool CConstantDepthTask::SetParam(string sParam, string sVal)
{
    MOOSToUpper(sParam);
    MOOSToUpper(sVal);


    if(!CMOOSBehaviour::SetParam(sParam,sVal))
    {
        //this is for us...
        if(MOOSStrCmp(sParam,"DEPTH"))
        {
            m_DepthDOF.SetDesired(atof(sVal.c_str()));
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

bool CConstantDepthTask::Initialise()
{

    //set a pitch driven depth controller
    m_ZPID.SetAsDepthController(true);


    m_bInitialised = true;

    return true;
}
