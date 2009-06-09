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
// GoToDepth.cpp: implementation of the CGoToDepth class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include <math.h>
#include <iostream>
using namespace std;

#include "MOOSTaskDefaults.h"
#include "ConstantDepthTask.h"

#include "GoToDepth.h"
#define DEFAULT_GO_TO_DEPTH_TOLERANCE 3.14

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGoToDepth::CGoToDepth()
{
    m_bSetRudder = false;
    m_dfRudder = 0;
    m_dfTolerance = DEFAULT_GO_TO_DEPTH_TOLERANCE;
    m_bSetThrust = false;
    m_dfThrust = 0;
}

CGoToDepth::~CGoToDepth()
{

}

bool CGoToDepth::SetParam(string sParam, string sVal)
{
    MOOSToUpper(sParam);
    MOOSToUpper(sVal);

    if(!CConstantDepthTask::SetParam(sParam,sVal))
    {
        //this is for us...
        if(MOOSStrCmp(sParam,"RUDDER"))
        {
            m_bSetRudder = true;
            m_dfRudder=atof(sVal.c_str());
        }
        else if(MOOSStrCmp(sParam,"TOLERANCE"))
        {
            m_dfTolerance=atof(sVal.c_str());
        }
        else if(MOOSStrCmp(sParam,"THRUST"))
        {
            m_dfThrust=atof(sVal.c_str());
            m_bSetThrust = true;
        }

    }    
    return true;
}


bool CGoToDepth::Run(CPathAction &DesiredAction)
{

    if(CConstantDepthTask::Run(DesiredAction))
    {
        if(fabs(m_DepthDOF.GetError())<m_dfTolerance)
        {
            Stop("Target Depth Reached");         
        }
        else
        {
            if(m_bSetRudder)
            {
                 DesiredAction.Set(  ACTUATOR_RUDDER,
                                     m_dfRudder,
                                     m_nPriority,
                                    m_sName.c_str());
            }
            if(m_bSetThrust)
            {
                DesiredAction.Set(  ACTUATOR_THRUST,
                                     m_dfThrust,
                                     m_nPriority,
                                     m_sName.c_str());
            }
        }
        return true;
    }
    return false;
}


