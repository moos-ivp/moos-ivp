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
// MOOSBluefinDriver.h: interface for the CMOOSBluefinDriver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSBLUEFINDRIVER_H__FF6A236A_0820_4EAF_8FA6_F1FAF3137606__INCLUDED_)
#define AFX_MOOSBLUEFINDRIVER_H__FF6A236A_0820_4EAF_8FA6_F1FAF3137606__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <MOOSGenLib/MOOSGenLib.h>
#include <MOOSUtilityLib/ScalarPID.h>
#include "MOOSActuationDriver.h"

class CMOOSBluefinDriver : public CMOOSActuationDriver  
{
public:
    CMOOSBluefinDriver();
    virtual ~CMOOSBluefinDriver();


    virtual bool Initialise();
    virtual bool SetThrust(double dfPercent);
    virtual bool SetRudder(double dfAng){return SetRudder(dfAng,false);};
    virtual bool SetElevator(double dfAng){return SetElevator(dfAng,false);};

    virtual bool SetRudder(double dfAng,bool bAnalog = false);
    virtual bool SetElevator(double dfAng,bool bAnalog = false);
    virtual bool SetZeroRudder();
    virtual bool SetZeroElevator();

    
    
protected:
    double Volts2RPM(double dfVolts);
    double GetRPM();
    double AnalogFromAngle(int nActuation, double dfAng,double & dfStepPosition);
    int     m_nSelectedActuation;
    bool MoveTo(int nActuation , double dfAnalog,bool bAnalog = false);
    bool SelectBoard(int nActuator);
    bool GetAnalogReading(double & dfReading);
    
    string GetBoardSelectString(int nActuator);


    bool HomeActuators();


    CScalarPID m_RPMPID;
};

#endif // !defined(AFX_MOOSBLUEFINDRIVER_H__FF6A236A_0820_4EAF_8FA6_F1FAF3137606__INCLUDED_)
