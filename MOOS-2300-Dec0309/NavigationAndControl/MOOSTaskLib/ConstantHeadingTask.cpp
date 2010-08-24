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
// ConstantHeadingTask.cpp: implementation of the CConstantHeadingTask class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

#include <math.h>
#include <iostream>
using namespace std;

#include "MOOSTaskDefaults.h"

#include "ConstantHeadingTask.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CConstantHeadingTask::CConstantHeadingTask()
{

    m_bInitialised = false;
    m_YawDOF.SetDesired(0);
    m_YawDOF.SetTolerance(0.00);
    m_bThrustSet = false;
    m_dfThrust = 0;

}

CConstantHeadingTask::~CConstantHeadingTask()
{

}
//returns false if we haven't received data in a while..bad news!
bool CConstantHeadingTask::RegularMailDelivery(double dfTimeNow)
{
 //     return true;
    return !m_YawDOF.IsStale(dfTimeNow,GetStartTime());
}


bool CConstantHeadingTask::Run(CPathAction &DesiredAction)
{

    if(!m_bInitialised)
    {
        Initialise();
    }
    
    if(m_YawDOF.IsValid())
    {

        double dfError = m_YawDOF.GetError();

        if(dfError<-PI)
        {
            dfError+=2*PI;
        }
        else if(dfError>PI)
        {
            dfError-=2*PI;
        }


        double dfCmd= 0;

        //this is for loggin purposes only
        m_YawPID.SetGoal(m_YawDOF.GetDesired());

        if(m_YawPID.Run(dfError,m_YawDOF.GetErrorTime(),dfCmd))
        {
            //OK we need to change something
            DesiredAction.Set(  ACTUATOR_RUDDER,
                                -(dfCmd),
                                m_nPriority,
                                "Constant Heading");

        }

        if(m_bThrustSet)
        {
            //OK we need to change something
            DesiredAction.Set(  ACTUATOR_THRUST,
                                m_dfThrust,
                                m_nPriority,
                                "Constant Heading");

        }
    }
    return true;
}

bool CConstantHeadingTask::OnNewMail(MOOSMSG_LIST &NewMail)
{
 
    CMOOSMsg Msg;
    
    if(PeekMail(NewMail,"NAV_YAW",Msg))
    { 
        if(!Msg.IsSkewed(GetTimeNow()))
        {
            //yaw is in radians
            double dfYaw = Msg.m_dfVal;
           
            m_YawDOF.SetCurrent(dfYaw,Msg.m_dfTime);
        }
    }

    //always call base class version
    CMOOSBehaviour::OnNewMail(NewMail);

    return true;
}

bool CConstantHeadingTask::GetRegistrations(STRING_LIST &List)
{

    List.push_front("NAV_YAW");

    //always call base class version
    CMOOSBehaviour::GetRegistrations(List);

    return true;
}


bool CConstantHeadingTask::SetParam(string sParam, string sVal)
{
    MOOSToUpper(sParam);
    MOOSToUpper(sVal);


    if(!CMOOSBehaviour::SetParam(sParam,sVal))
    {
        //this is for us...
        if(MOOSStrCmp(sParam,"THRUST"))
        {
            m_dfThrust = atof(sVal.c_str());
            m_bThrustSet = true;
        }        
        //this is for us...
        else if(MOOSStrCmp(sParam,"HEADING"))
        {

            //now user specifies heading w.r.t to true north indegrees , the compassm or otehr magnetic device
            //has already made the corrections...
            double dfDesired = atof(sVal.c_str());

            double dfMagneticOffset = 0.0;
            
            if(m_pMissionFileReader)
            {
                if(!m_pMissionFileReader->GetValue("MAGNETICOFFSET",dfMagneticOffset))
                {
                    MOOSTrace("WARNING: No magnetic offset specified  in Mission file (Field name = \"MagneticOffset\")\n");
                }
            }
            
            //we control on *_YAW variables....
            dfDesired*=PI/180.0;

            //yaw is in opposite direction to heading...
            dfDesired = MOOS_ANGLE_WRAP(-dfDesired);

            m_YawDOF.SetDesired(dfDesired);
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

bool CConstantHeadingTask::Initialise()
{



    m_bInitialised = true;

    return true;
}


