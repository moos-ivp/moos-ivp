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
// EndMission.cpp: implementation of the CEndMission class.
//
//////////////////////////////////////////////////////////////////////

#include "EndMission.h"
#include <iostream>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEndMission::CEndMission()
{
    


    m_dfEndRudder = 0;
    m_dfEndElevator = 0;
    m_dfEndThrust = 0;
    m_nPriority = 0;

}

CEndMission::~CEndMission()
{

}

//returns false if we haven't received data in a while..bad news!
bool CEndMission::RegularMailDelivery(double dfTimeNow)
{
    //always return true (never stop!)
    return true;
}


bool CEndMission::Run(CPathAction &DesiredAction)
{
    if(1/*m_DepthDOF.IsValid()*/)
    {
//        if(m_DepthDOF.GetError()>0)
        {
            DesiredAction.Set(  ACTUATOR_ELEVATOR,
                                m_dfEndElevator,
                                m_nPriority,
                                "End Mission");

            DesiredAction.Set(  ACTUATOR_RUDDER,
                                m_dfEndRudder,
                                m_nPriority,
                                "End Mission");

            DesiredAction.Set(  ACTUATOR_THRUST,
                                m_dfEndThrust,
                                m_nPriority,
                                "End Mission");

        }
    }
    else
    {

        DesiredAction.Set(  ACTUATOR_ELEVATOR,
                            0,
                            m_nPriority,
                            "End Mission");

        DesiredAction.Set(  ACTUATOR_RUDDER,
                            0,
                            m_nPriority,
                            "End Mission");

        DesiredAction.Set(  ACTUATOR_THRUST,
                            0,
                            m_nPriority,
                            "End Mission");

    }

    return true;
}

bool CEndMission::OnNewMail(MOOSMSG_LIST &NewMail)
{

    //always call base class version
    CMOOSBehaviour::OnNewMail(NewMail);


    return true;
}

bool CEndMission::GetRegistrations(STRING_LIST &List)
{

    //always call base class version
    CMOOSBehaviour::GetRegistrations(List);


    return true;
}

bool CEndMission::SetParam(string sParam, string sVal)
{
    MOOSToUpper(sParam);
    MOOSToUpper(sVal);


    if(!CMOOSBehaviour::SetParam(sParam,sVal))
    {
        //this is for us...
        if(sParam=="ENDELEVATOR")
        {
            m_dfEndElevator=MOOSDeg2Rad(atof(sVal.c_str()));
        }
        else if(sParam=="ENDRUDDER")
        {
            m_dfEndRudder =MOOSDeg2Rad(atof(sVal.c_str()));
        }
        else if(sParam=="ENDTHRUST")
        {
            m_dfEndThrust =atof(sVal.c_str());
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
