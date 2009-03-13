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
// PitchZPID.cpp: implementation of the CPitchZPID class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSGenLib/MOOSGenLibGlobalHelper.h>
#include <math.h>
#include "PitchZPID.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPitchZPID::CPitchZPID()
{

    m_dfPitchDesired = 0.0;

    m_PitchPID.SetGains(MOOSDeg2Rad(10),0,0);
    m_PitchPID.SetLimits(MOOSDeg2Rad(10),MOOSDeg2Rad(15.0));

    //by default we control in +ve X direction
    m_bIsDepth = false;

    m_bReversing = false;
}

CPitchZPID::~CPitchZPID()
{

}


bool CPitchZPID::Run(double dfZError, double dfErrorTime, double &dfOut, double dfPitch,double dfPitchTime)
{
    //dfError is Z error
    //calculate pitch change...
    //+ve pitch and forward makes us go up in Z direction...
    if(m_bIsDepth)
    {
        //if we are actually controlling depth then this varies
        //with -z
        //m_dfPitchDesired = -m_dfPitchDesired;
        dfZError = -dfZError;
    }

    if(m_bReversing)
    {
        dfZError = -dfZError;
    }

    //we are controlling on depth via pitch
    //to get around logging issues we need
    //to keep a copy of our Z goal
    double dfGoalCopy = m_dfGoal;

    //call overloaded version that may spot change in direction
    //due to really controlling depth (-Z)
    m_PitchPID.SetGoal(m_dfGoal);

    //figure out a desired pitch...
    if(!m_PitchPID.Run(dfZError,dfErrorTime,m_dfPitchDesired))
        return false;

    //get pitch error signal...
    double dfPitchError = m_dfPitchDesired-dfPitch;

    //reset our goal to pitch temporarily
    //not I'm not using the class method as
    //this may negate goal if depth controlling!
    CScalarPID::SetGoal(m_dfPitchDesired);

    //use this as input into the base type controller (controlling pitch)    
    if(CScalarPID::Run(dfPitchError,dfPitchTime,dfOut))
    {
        if(m_bReversing)
        {
           dfOut = dfOut;
        }
        else
        {
            //finally positive elevator makes vehicle dive...
            //so dfOut is requiring more positive pitch we need more NEGATIVE
            //elevator
            dfOut =  -dfOut;
            //MOOSTrace("Desired = %f, Error = %f, Cmd = %f\n",dfPitchDesired,dfPitchError,dfOut);
        }
        CScalarPID::SetGoal(dfGoalCopy);
        return true;
    }
    else
    {
        CScalarPID::SetGoal(dfGoalCopy);
        return false;
    }
}


bool CPitchZPID::SetAsDepthController(bool bDepth)
{
    m_bIsDepth = bDepth;
    return true;
}

bool CPitchZPID::SetReversing(bool bReverse)
{
    m_bReversing = bReverse;
    return true;
}

bool CPitchZPID::SetLogPath(std::string &sPath)
{
    m_sLogPath = sPath;
    m_PitchPID.SetLogPath(sPath);
    return true;
}


bool CPitchZPID::SetGains(double dfZToPitchKp,
                          double dfZToPitchKd,
                          double dfZToPitchKi,
                          double dfPitchKp,
                          double dfPitchKd,
                          double dfPitchKi)
{


    //we have another 'owned controller' that maps z error to pitch
    m_PitchPID.SetGains(dfZToPitchKp,dfZToPitchKd,dfZToPitchKi);

    std::string sPitch = MOOSFormat("%sZToPitch",m_sName.c_str());
    m_PitchPID.SetName(sPitch);
    m_PitchPID.SetLog(m_bLog);


    //the pitch controller is our fundamental concern
    CScalarPID::SetGains(dfPitchKp,dfPitchKd,dfPitchKi);
    std::string sMe = MOOSFormat("%sPitchToElevator",m_sName.c_str());
    SetName(sMe);


    return true;
}

bool CPitchZPID::SetLimits(double dfMaxPitch, double dfMaxElevator, double dfPitchIntegralLimit, double dfElevatorIntegralLimit)
{
    //set limits on depth to pitch control
    m_PitchPID.SetLimits(dfPitchIntegralLimit,dfMaxPitch);

    //set limit on elevator control
    CScalarPID::SetLimits(dfElevatorIntegralLimit,dfMaxElevator);

    return true;
}

bool CPitchZPID::SetGoal(double dfGoal)
{
    if(m_bIsDepth)
    {
        m_dfGoal = -dfGoal;
    }
    return true;
}
