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
// PathAction.cpp: implementation of the CPathAction class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>


#include "PathAction.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPathAction::CPathAction()
{
  
}

CPathAction::~CPathAction()
{

}

bool CPathAction::Set(ActuatorType eType, double dfVal, unsigned int nPriority, const char * sTag)
{

    if(eType<sizeof(m_Actuators)/sizeof(m_Actuators[0]))
    {

        if(m_Actuators[eType].m_nPriority>nPriority)
        {
            if(m_Actuators[eType].m_nPriority==nPriority)
            {
                printf("CPathAction::Set() Squabbling twins!\n");
            }

            m_Actuators[eType].m_dfVal = dfVal;
            m_Actuators[eType].m_nPriority = nPriority;
            m_Actuators[eType].m_sTag = sTag;

           

            return true;
        }
        else
        {
            return false;
        }
        
    }
    else
    {
        printf("CPathAction::Set() Hey! Subscript out of range! what kind of compiler is this?\n");
        return false;
    }
}

double CPathAction::Get(ActuatorType eType)
{
    if(eType<sizeof(m_Actuators)/sizeof(m_Actuators[0]))
    {
        return m_Actuators[eType].m_dfVal;
    }
    else
    {
        printf("CPathAction::Set() Hey! Subscript out of range! what kind of compiler is this?\n");
        return 0;
    }
}

string CPathAction::GetTag(ActuatorType eType)
{
    if(eType<sizeof(m_Actuators)/sizeof(m_Actuators[0]))
    {
        return m_Actuators[eType].m_sTag;
    }
    else
    {
        printf("CPathAction::Set() Hey! Subscript out of range! what kind of compiler is this?\n");
        return "error";
    }
}

unsigned int CPathAction::GetPriority(ActuatorType eType)
{
    if(eType<sizeof(m_Actuators)/sizeof(m_Actuators[0]))
    {
        return m_Actuators[eType].m_nPriority;
    }
    else
    {
        printf("CPathAction::Set() Hey! Subscript out of range! what kind of compiler is this?\n");
        return VERY_LARGE_NUMBER;
    }
}

void CPathAction::Trace()
{
    printf("El=%7.3f Task=%s Priority=%d\n",
        m_Actuators[ACTUATOR_ELEVATOR].m_dfVal,
        m_Actuators[ACTUATOR_ELEVATOR].m_sTag.c_str(),
        m_Actuators[ACTUATOR_ELEVATOR].m_nPriority);

    printf("Rd=%7.3f Task=%s Priority=%d\n",
        m_Actuators[ACTUATOR_RUDDER].m_dfVal,
        m_Actuators[ACTUATOR_RUDDER].m_sTag.c_str(),
        m_Actuators[ACTUATOR_RUDDER].m_nPriority);

    printf("Th=%7.3f Task=%s Priority=%d\n",
        m_Actuators[ACTUATOR_THRUST].m_dfVal,
        m_Actuators[ACTUATOR_THRUST].m_sTag.c_str(),
        m_Actuators[ACTUATOR_THRUST].m_nPriority);




}
