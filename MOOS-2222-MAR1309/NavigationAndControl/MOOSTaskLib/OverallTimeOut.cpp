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
// OverallTimeOut.cpp: implementation of the COverallTimeOut class.
//
//////////////////////////////////////////////////////////////////////

#include "OverallTimeOut.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COverallTimeOut::COverallTimeOut()
{
    m_bInitialised = false;
    m_dfTimeRemaining = -1;
}

COverallTimeOut::~COverallTimeOut()
{

}

bool COverallTimeOut::Initialise()
{   
    bool bFoundEndMission = false;

    //need to check on the FinishFlag = EndMission being present
    STRING_LIST::iterator p;
    for(p = m_CompleteFlags.begin(); p != m_CompleteFlags.end(); p++)
    {
        string sFlag = *p;
        MOOSToUpper(sFlag);

        if(sFlag == "ENDMISSION")
            bFoundEndMission = true;
    }

    if(!bFoundEndMission)
        DebugNotify("OverallTimeOut does not fire EndMission, but it should\n");
    
    m_bInitialised = true;

    return m_bInitialised;
}

//this task does not require input
bool COverallTimeOut::RegularMailDelivery(double dfTimeNow)
{
   
    return true;
    
}

bool COverallTimeOut::OnTimeOut()
{
    OnEvent("OverallTimeOut Complete");
    return CMOOSBehaviour::OnTimeOut();    
}

bool COverallTimeOut::Run(CPathAction &DesiredAction)
{
    
    if(!m_bInitialised)
    {
        Initialise();
    }

    SetTime(MOOSTime());
    
    double dfTimeElapsed = GetTimeNow() - m_dfStartTime;

    m_dfTimeRemaining = m_dfTimeOut - dfTimeElapsed;
    
    return true;
}

bool COverallTimeOut::GetNotifications(MOOSMSG_LIST & List)
{
    if(m_bActive)
    {
        CMOOSMsg Msg(MOOS_NOTIFY,
         "MISSION_TIME_REMAINING",
         m_dfTimeRemaining);

        List.push_back(Msg);
    }

    return CMOOSBehaviour::GetNotifications(List);
}

bool COverallTimeOut::SetParam(string sParam, string sVal)
{

    MOOSToUpper(sParam);
    MOOSToUpper(sVal);


    if(!CMOOSBehaviour::SetParam(sParam,sVal))
    {
    }


    return true;

}
