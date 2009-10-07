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
// ConstantHeadingTask.h: interface for the CConstantHeadingTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONSTANTHEADINGTASK_H__EE1A21FC_E1F3_4124_A420_6439DF0C71AF__INCLUDED_)
#define AFX_CONSTANTHEADINGTASK_H__EE1A21FC_E1F3_4124_A420_6439DF0C71AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MOOSBehaviour.h"

class CConstantHeadingTask : public CMOOSBehaviour  
{
public:
    CConstantHeadingTask();
    virtual ~CConstantHeadingTask();

    virtual bool SetParam(string sParam, string sVal);
            bool OnNewMail(MOOSMSG_LIST &NewMail);
            bool Run(CPathAction &DesiredAction);
    virtual bool GetRegistrations(STRING_LIST &List);
    virtual bool RegularMailDelivery(double dfTimeNow);

protected:
    
    bool m_bInitialised;
    bool Initialise();
    bool m_bThrustSet;
    double m_dfThrust;

    ControlledDOF   m_YawDOF;



};

#endif // !defined(AFX_CONSTANTHEADINGTASK_H__EE1A21FC_E1F3_4124_A420_6439DF0C71AF__INCLUDED_)
