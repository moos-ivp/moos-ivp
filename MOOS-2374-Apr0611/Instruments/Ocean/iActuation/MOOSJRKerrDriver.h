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
//   This file is part of a  MOOS Instrument.
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
// MOOSJRKerrDriver.h: interface for the CMOOSJRKerrDriver class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <MOOSGenLib/MOOSGenLib.h>
#include "MOOSActuationDriver.h"

class CMOOSJRKerrDriver : public CMOOSActuationDriver
{
public:
    CMOOSJRKerrDriver();
    virtual ~CMOOSJRKerrDriver();


    virtual bool Initialise();
    virtual bool SetThrust(double dfPercent);
    virtual bool SetRudder(double dfAng);
    virtual bool SetElevator(double dfAng);
    virtual bool SetZeroRudder();
    virtual bool SetZeroElevator();



protected:
    bool m_bMoveDone;
    string m_sLogFileName;
    double m_dfRudder;
    double m_dfKerrRudderOffset;
    double m_dfElevator;
    double m_dfKerrElevatorOffset;

    bool SendCmd(int addr, int cmd, const char *data);
    bool DoPositionMove(int position, int velocity, int acceleration, int address);
    void TraceKerrMessage(const char * pMsg,int nLen);
    bool LogPosition();
    bool SyncLog();
    double GetRPM();
};
