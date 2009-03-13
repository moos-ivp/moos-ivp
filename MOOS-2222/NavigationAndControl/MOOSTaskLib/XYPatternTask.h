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
// XYPatternTask.h: interface for the CXYPatternTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XYPATTERNTASK_H__57B8999A_6EEB_4B50_BE1C_8B302AE33657__INCLUDED_)
#define AFX_XYPATTERNTASK_H__57B8999A_6EEB_4B50_BE1C_8B302AE33657__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <list>
#include <vector>
#include <MOOSGenLib/MOOSGenLib.h>
#include "MOOSBehaviour.h"

#define XYPATTERN_X_VAL 0
#define XYPATTERN_Y_VAL 1

#define XYPATTERN_DEFAULT_TOTAL_REPETITION 25

#define XYPATTERN_MAX_TOTAL_REPETITION 50
#define XYPATTERN_MAX_TOTAL_POSITIONS 10

class CXYPatternTask : public CMOOSBehaviour  
{
public:
    typedef vector<CXYPoint> POSITION_DATA;
    POSITION_DATA m_XYPoints;

    int m_nRepCounter;
    
    CXYPatternTask();
    virtual ~CXYPatternTask();

    int m_nTotalPositions;
    int m_nCurrentPosition;
    int m_nTotalRepetitions;

    double *m_dfOrbit[XYPATTERN_MAX_TOTAL_POSITIONS];
    bool SetParam(string sParam, string sVal);

    
    virtual bool GetRegistrations(STRING_LIST &List);
    virtual bool OnNewMail(MOOSMSG_LIST &NewMail);
            bool Run(CPathAction &DesiredAction);
    virtual bool RegularMailDelivery(double dfTimeNow);
    virtual bool SetControl(CPathAction &DesiredAction,double dfRudder,double dfThrust);


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
typedef list<string> POSITION_LIST;
    POSITION_LIST m_Positions;    
    bool ValidData();
    bool Initialise();
    void SetNextPoint();

};

#endif // !defined(AFX_XYPATTERNTASK_H__57B8999A_6EEB_4B50_BE1C_8B302AE33657__INCLUDED_)
