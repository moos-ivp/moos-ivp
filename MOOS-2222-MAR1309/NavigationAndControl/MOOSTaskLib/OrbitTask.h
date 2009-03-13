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
// OrbitTask.h: interface for the COrbitTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ORBITTASK_H__A4BB872D_3EF5_422A_83C6_078DF279763B__INCLUDED_)
#define AFX_ORBITTASK_H__A4BB872D_3EF5_422A_83C6_078DF279763B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <MOOSGenLib/MOOSGenLib.h>
#include "MOOSBehaviour.h"
#include <vector>

#define ORBIT_MAX_TOTAL_REPETITION 50
#define ORBIT_DEFAULT_TOTAL_REPETITION 25

#define ORBIT_MAX_TOTAL_POSITIONS 30
#define ORBIT_DEFAULT_TOTAL_POSITIONS 6
#define CW      1
#define CCW -1

class COrbitTask : public CMOOSBehaviour
{
public:
    typedef vector<CXYPoint> POSITION_DATA;
    POSITION_DATA m_XYPoints;

    double m_dfPositionRadius;

    int m_nRepCounter;
    int m_nTotalRepetitions;
    int m_nTotalPositions;
    int m_nCurrentPosition;
    int m_nOrbitDirection;

    bool SetParam(string sParam, string sVal);
    COrbitTask();
    virtual ~COrbitTask();
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

    ControlledDOF m_XOrbitCenter;
    ControlledDOF m_YOrbitCenter;

    double m_dfVicinityRadius;
    double m_dfThrust;
    string m_sLocation;


protected:


    bool ValidData();
    bool Initialise();

private:
    void SetNextPointInOrbit();
};

#endif // !defined(AFX_ORBITTASK_H__A4BB872D_3EF5_422A_83C6_078DF279763B__INCLUDED_)
