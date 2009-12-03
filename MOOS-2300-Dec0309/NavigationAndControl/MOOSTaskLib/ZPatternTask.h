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
// ZPatternTask.h: interface for the CZPatternTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZPATTERNTASK_H__B8DED353_FFC6_4C3C_86A3_CC2AF27716A1__INCLUDED_)
#define AFX_ZPATTERNTASK_H__B8DED353_FFC6_4C3C_86A3_CC2AF27716A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <MOOSGenLib/MOOSGenLib.h>
#include <string>
using namespace std;
#include "MOOSBehaviour.h"


#include "MOOSBehaviour.h"




class CZPatternTask : public CMOOSBehaviour  
{
public:
    CZPatternTask();
    virtual ~CZPatternTask();


    virtual bool SetParam(string sParam, string sVal);
            bool OnNewMail(MOOSMSG_LIST &NewMail);
            bool Run(CPathAction &DesiredAction);
    virtual bool GetRegistrations(STRING_LIST &List);
    virtual bool RegularMailDelivery(double dfTimeNow);

    enum MOOS_Z_PATTERN
    {
        MOOS_Z_PATTERN_ERROR=-1,
        MOOS_Z_PATTERN_SQUARE,
        MOOS_Z_PATTERN_YOYO,
        
    };

    enum MOOS_Z_PATTERN_GOAL
    {
        MOOS_Z_PATTERN_GOAL_WAIT_MIN,
        MOOS_Z_PATTERN_GOAL_MAX_DEPTH,
        MOOS_Z_PATTERN_GOAL_WAIT_MAX,
        MOOS_Z_PATTERN_GOAL_MIN_DEPTH,
    };


protected:

    bool Initialise();
    bool m_bInitialised;

    ControlledDOF m_Thrust;

    ControlledDOF m_DepthDOF;
    ControlledDOF m_PitchDOF;


    MOOS_Z_PATTERN m_ePatternType;
    MOOS_Z_PATTERN_GOAL m_eGoal;


    double m_dfTolerance;

    double m_dfMaxDepth;
    double m_dfMinDepth;

    double m_dfLevelStartTime;
    double m_dfLevelDuration;



    bool SetNextSetPoint();
};

#endif // !defined(AFX_ZPATTERNTASK_H__B8DED353_FFC6_4C3C_86A3_CC2AF27716A1__INCLUDED_)
