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
// GoToWayPoint.h: interface for the CGoToWayPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GOTOWAYPOINT_H__62F59468_EF1B_4542_A53C_92618ABF7607__INCLUDED_)
#define AFX_GOTOWAYPOINT_H__62F59468_EF1B_4542_A53C_92618ABF7607__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <MOOSGenLib/MOOSGenLib.h>
#include "MOOSBehaviour.h"

class CGoToWayPoint : public CMOOSBehaviour  
{
public:
    virtual bool SetParam(string sParam, string sVal);

    CGoToWayPoint();
    virtual ~CGoToWayPoint();

    virtual bool GetRegistrations(STRING_LIST &List);
            bool OnNewMail(MOOSMSG_LIST &NewMail);
            bool Run(CPathAction &DesiredAction);
    virtual bool RegularMailDelivery(double dfTimeNow);


    bool m_bInitialised;
    bool m_bPositionSet;
    bool m_bThrustSet;
    
    ControlledDOF m_YawDOF;
    ControlledDOF m_XDOF;
    ControlledDOF m_YDOF;
  
    double m_dfVicinityRadius;
    double m_dfThrust;
    string m_sLocation;
    

protected:
    virtual double GetDistanceToGo();
    bool ValidData();
    bool Initialise();
};

#endif // !defined(AFX_GOTOWAYPOINT_H__62F59468_EF1B_4542_A53C_92618ABF7607__INCLUDED_)
