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
// PathAction.h: interface for the CPathAction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATHACTION_H__5ED641C5_EBE1_4606_9758_087B8E19F48A__INCLUDED_)
#define AFX_PATHACTION_H__5ED641C5_EBE1_4606_9758_087B8E19F48A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
using namespace std;

#define VERY_LARGE_NUMBER 1000

enum ActuatorType
{
    ACTUATOR_ELEVATOR=0,
    ACTUATOR_RUDDER,
    ACTUATOR_THRUST,
};

class CPathAction  
{
public:
    void Trace();
    unsigned int GetPriority(ActuatorType eType);
    string GetTag(ActuatorType eType);
    double Get(ActuatorType eType);
    bool Set(ActuatorType eType,double dfVal,unsigned int nPriority,const char * sTag="");
  
    CPathAction();

    virtual ~CPathAction();



protected:
    struct Actuation
    {
        Actuation()
        {
            m_nPriority=VERY_LARGE_NUMBER;
            m_dfVal = 0.0;
        }
        unsigned int m_nPriority;
        double m_dfVal;
        string m_sTag;
    };

    Actuation m_Actuators[3];

};

#endif // !defined(AFX_PATHACTION_H__5ED641C5_EBE1_4606_9758_087B8E19F48A__INCLUDED_)
