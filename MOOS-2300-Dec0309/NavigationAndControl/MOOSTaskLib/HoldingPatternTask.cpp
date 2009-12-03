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
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif


// HoldingPatternTask.cpp: implementation of the CHoldingPatternTask class.
//
//////////////////////////////////////////////////////////////////////

#include "HoldingPatternTask.h"
#include <iostream>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHoldingPatternTask::CHoldingPatternTask()
{
    m_dfThrust=0.0;
    m_dfRudder=0.0;
    m_dfElevator=0.0;
    
    //a good default value
    m_nPriority = 3;

    m_bElevatorSet = false;
    m_bRudderSet = false;
    m_bThrustSet = false;

}

CHoldingPatternTask::~CHoldingPatternTask()
{

}

//returns false if we haven't received data in a while..bad news!
bool CHoldingPatternTask::RegularMailDelivery(double dfTimeNow)
{
    //doesn't respond to mail...
    return true;
}

bool CHoldingPatternTask::Run(CPathAction &DesiredAction)
{

    if(m_bRudderSet)
    {

        DesiredAction.Set(  ACTUATOR_RUDDER,
                            m_dfRudder,
                            m_nPriority,
                            GetName().c_str());
    }
    if(m_bThrustSet)
    {
        DesiredAction.Set(  ACTUATOR_THRUST,
                            m_dfThrust,
                            m_nPriority,
                            GetName().c_str());
    }

       if(m_bElevatorSet)
    {
        DesiredAction.Set(  ACTUATOR_ELEVATOR,
                            m_dfElevator,
                            m_nPriority,
                            GetName().c_str());
    }

    
    return true;
}

bool CHoldingPatternTask::OnNewMail(MOOSMSG_LIST &NewMail)
{

    //always call base class version
    CMOOSBehaviour::OnNewMail(NewMail);


    return true;
}

bool CHoldingPatternTask::GetRegistrations(STRING_LIST &List)
{

    //always call base class version
    CMOOSBehaviour::GetRegistrations(List);

    return true;
}

bool CHoldingPatternTask::SetParam(string sParam, string sVal)
{

    MOOSToUpper(sParam);
    MOOSToUpper(sVal);


    if(!CMOOSBehaviour::SetParam(sParam,sVal))
    {
        //this is for us...
        if(MOOSStrCmp(sParam,"RUDDER"))
        {
            m_dfRudder =MOOSDeg2Rad(atof(sVal.c_str()));
            m_bRudderSet = true;
        }
        
        //this is for us...
        else if(MOOSStrCmp(sParam,"ELEVATOR"))
        {
            m_dfElevator =MOOSDeg2Rad(atof(sVal.c_str()));
            m_bElevatorSet = true;
        }

        
        else if(MOOSStrCmp(sParam,"THRUST"))
        {
            m_dfThrust =atof(sVal.c_str());
            m_bThrustSet = true;
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
