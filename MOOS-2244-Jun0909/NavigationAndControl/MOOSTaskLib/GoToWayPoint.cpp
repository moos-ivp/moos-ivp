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
// GoToWayPoint.cpp: implementation of the CGoToWayPoint class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include "MOOSTaskDefaults.h"
#include "GoToWayPoint.h"
#include "math.h"
#include <iostream>
using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////




CGoToWayPoint::CGoToWayPoint()
{
    m_nPriority = 3;
    
    m_bInitialised = false;
    
    m_dfVicinityRadius = 0;
    
    m_bPositionSet = true;
    
    m_bThrustSet = false;
    
    m_dfThrust = 0;
}

CGoToWayPoint::~CGoToWayPoint()
{
    
}

bool CGoToWayPoint::OnNewMail(MOOSMSG_LIST &NewMail)
{
    CMOOSMsg Msg;
    if(PeekMail(NewMail,"NAV_X",Msg))
    {
        
        if(!Msg.IsSkewed(GetTimeNow()))
        {
            m_XDOF.SetCurrent(Msg.m_dfVal,Msg.m_dfTime);
        }
    }
    if(PeekMail(NewMail,"NAV_Y",Msg))
    {
        if(!Msg.IsSkewed(GetTimeNow()))
        {   
            m_YDOF.SetCurrent(Msg.m_dfVal,Msg.m_dfTime);
        }
    }
    
    if(PeekMail(NewMail,"NAV_YAW",Msg))
    {
        if(!Msg.IsSkewed(GetTimeNow()))
        {
            
            double dfVal = Msg.m_dfVal;
            
            if(dfVal>PI)
            {
                dfVal-=2*PI;
            }
            m_YawDOF.SetCurrent(dfVal,Msg.m_dfTime);
        }
        
    }
    
    //always call base class version
    CMOOSBehaviour::OnNewMail(NewMail);
    
    return true;
}

bool CGoToWayPoint::GetRegistrations(STRING_LIST &List)
{
    List.push_front("NAV_X");
    List.push_front("NAV_Y");
    List.push_front("NAV_YAW");
    
    //always call base class version
    return CMOOSBehaviour::GetRegistrations(List);
    
}




//returns false if we haven't received data in a while..bad news!
bool CGoToWayPoint::RegularMailDelivery(double dfTimeNow)
{
    if(m_YawDOF.IsStale(dfTimeNow,GetStartTime()))
        return false;
    
    if(m_XDOF.IsStale(dfTimeNow,GetStartTime()))
        return false;
    
    if(m_YDOF.IsStale(dfTimeNow,GetStartTime()))
        return false;
    
    return true;
}



bool CGoToWayPoint::Run(CPathAction &DesiredAction)
{
    if(!m_bPositionSet)
    {
        MOOSTrace("Waypoint position not set\n");;
        return false;
    }
    
    if(!m_bInitialised)
    {
        Initialise();
    }
    
    if(ShouldRun())
    {
        
        if(ValidData())
        {
            
            double dfDistanceToGo = GetDistanceToGo();
            
            
            if(dfDistanceToGo<m_dfVicinityRadius)
            {            
                OnComplete();
            }
            else
            {
                
                //calculate vector heading angle to goal
                double dfDesiredYaw = -atan2(m_XDOF.GetError(),m_YDOF.GetError());
                
                m_YawDOF.SetDesired(dfDesiredYaw);
                
                double dfError = m_YawDOF.GetError();
                
                dfError = MOOS_ANGLE_WRAP(dfError);
                
                double dfCmd = 0;
                
                //this is for logging purposes only
                m_YawPID.SetGoal(m_YawDOF.GetDesired());
                
                
                if(m_YawPID.Run(dfError,m_YawDOF.GetErrorTime(),dfCmd))
                {
                    //OK we need to change something
                    
                    //convert to degrees!
                    DesiredAction.Set(  ACTUATOR_RUDDER,
                        -dfCmd,
                        m_nPriority,
                        GetName().c_str());
                    
                    
                    //convert to degrees!
                    if(m_bThrustSet)
                    {
                        DesiredAction.Set(  ACTUATOR_THRUST,
                            m_dfThrust,
                            m_nPriority,
                            GetName().c_str());
                    }
                    
                    
                }
            }
        }
    }
    return true;
}

bool CGoToWayPoint::Initialise()
{
    
    m_YawDOF.SetDesired(0);
    m_YawDOF.SetTolerance(0.00);
    
    m_bInitialised = true;
    
    return m_bInitialised;
}

bool CGoToWayPoint::ValidData()
{   
    return  m_XDOF.IsValid() &&
        m_YDOF.IsValid() &&
        m_YawDOF.IsValid();
}



bool CGoToWayPoint::SetParam(string sParam, string sVal)
{
    MOOSToUpper(sParam);
    MOOSToUpper(sVal);
    
    
    if(!CMOOSBehaviour::SetParam(sParam,sVal))
    {
        //this is for us...
        if(sParam=="RADIUS")
        {
            m_dfVicinityRadius=atof(sVal.c_str());
        }
        else if(sParam=="THRUST")
        {
            m_dfThrust=atof(sVal.c_str());
            m_bThrustSet = true;
        }
        else if(sParam=="LOCATION")
        {
            //useful for later...
            m_sLocation = sVal;
            
            string sTmpX = MOOSChomp(sVal,",");
            string sTmpY = MOOSChomp(sVal,",");
            string sTmpZ = MOOSChomp(sVal,",");
            
            if(sTmpX.empty()||sTmpY.empty())
            {
                MOOSTrace("error in parsing waypoint location from %s\n",m_sLocation.c_str());
                return false;
            }
            
            double dfX =   atof(sTmpX.c_str());
            m_XDOF.SetDesired(dfX);
            
            double dfY =   atof(sTmpY.c_str());
            m_YDOF.SetDesired(dfY);
            
        }
        else
        {
            //hmmm - it wasn't for us at all: base class didn't understand either
            MOOSTrace("Param \"%s\" not understood!\n",sParam.c_str());
            return false;
        }
    }
    
    return true;
}

double CGoToWayPoint::GetDistanceToGo()
{
    return sqrt(pow(m_YDOF.GetError(),2)+pow(m_XDOF.GetError(),2));
}
