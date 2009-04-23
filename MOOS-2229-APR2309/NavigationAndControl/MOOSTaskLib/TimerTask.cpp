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
// TimerTask.cpp: implementation of the CTimerTask class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

#include "TimerTask.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimerTask::CTimerTask()
{

}

CTimerTask::~CTimerTask()
{

}

//returns false if we haven't received data in a while..bad news!
bool CTimerTask::RegularMailDelivery(double dfTimeNow)
{
   
    return true;
    
}

bool CTimerTask::Run(CPathAction &DesiredAction)
{
    return true;
}

bool CTimerTask::OnNewMail(MOOSMSG_LIST &NewMail)
{

    //always call base class version
    CMOOSBehaviour::OnNewMail(NewMail);


    return true;
}

bool CTimerTask::GetRegistrations(STRING_LIST &List)
{

    //always call base class version
    CMOOSBehaviour::GetRegistrations(List);

    return true;
}

bool CTimerTask::SetParam(string sParam, string sVal)
{

    MOOSToUpper(sParam);
    MOOSToUpper(sVal);


    if(!CMOOSBehaviour::SetParam(sParam,sVal))
    {
    }

    return true;

}

bool CTimerTask::OnTimeOut()
{
    OnEvent("Timer Complete");
    return CMOOSBehaviour::OnTimeOut();    
}
