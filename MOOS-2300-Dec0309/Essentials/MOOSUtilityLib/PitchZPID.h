///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite 
//  
//   A suit of Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and 
//   Oxford University. 
//    
//   This software was written by Paul Newman at MIT 2001-2002 and Oxford 
//   University 2003-2005. email: pnewman@robots.ox.ac.uk. 
//      
//   This file is part of a  MOOS Core Component. 
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
// PitchZPID.h: interface for the CPitchZPID class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PITCHZPID_H__26C86831_23CB_4388_A28C_D11BC351C13C__INCLUDED_)
#define AFX_PITCHZPID_H__26C86831_23CB_4388_A28C_D11BC351C13C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ScalarPID.h"

class CPitchZPID : public CScalarPID  
{
public:
    virtual bool SetGoal(double dfGoal);
    virtual bool SetLogPath(std::string &sPath);
    bool SetLimits(double dfMaxPitch,double dfMaxElevator,double dfPitchIntegralLimit,double dfElevatorIntegralLimit);
    virtual bool SetGains(double dfZToPitchKp,
                          double dfZToPitchKd,
                          double dfZToPitchKi,
                          double dfPitchKp,
                          double dfPitchKd,
                          double dfPitchKi);
    bool SetReversing(bool bReverse);
    bool Run(double dfError,
            double dfErrorTime,
            double &dfOut,
            double dfPitch,
            double dfPitchTime);
    CPitchZPID();
    virtual ~CPitchZPID();

    /// call this function if we are controlling depth with this controller
    bool    SetAsDepthController(bool bIsDepth);

    double GetPitchDesired(){return m_dfPitchDesired;};
protected:
    CScalarPID m_PitchPID;
    /// true if we are actually controlling depth
    bool    m_bIsDepth;
    bool    m_bReversing;
    double  m_dfPitchDesired;

};

#endif // !defined(AFX_PITCHZPID_H__26C86831_23CB_4388_A28C_D11BC351C13C__INCLUDED_)
