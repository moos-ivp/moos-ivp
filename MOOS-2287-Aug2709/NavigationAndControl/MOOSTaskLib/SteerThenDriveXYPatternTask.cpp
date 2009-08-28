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
// SteerThenDriveXYPatternTask.cpp: implementation of the CSteerThenDriveXYPatternTask class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

#include "SteerThenDriveXYPatternTask.h"
#include <math.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define DEFAULT_OUTER_YAW_TOLERANCE  MOOSDeg2Rad(40)
#define DEFAULT_INNER_YAW_TOLERANCE  MOOSDeg2Rad(10)

CSteerThenDriveXYPatternTask::CSteerThenDriveXYPatternTask()
{
    m_dfOuterYawTolerance= DEFAULT_OUTER_YAW_TOLERANCE;
    m_dfInnerYawTolerance = DEFAULT_INNER_YAW_TOLERANCE;
    m_dfLastThrust = 0;

    //if alpha  = 0 then no damping
    m_dfAlpha = 0.1;
}

CSteerThenDriveXYPatternTask::~CSteerThenDriveXYPatternTask()
{

}

bool CSteerThenDriveXYPatternTask::GetRegistrations(STRING_LIST &List)
{
    List.push_front("XYPATTERN_LOCATIONS");

    //always call base class version
    CXYPatternTask::GetRegistrations(List);

    return true;
}


bool CSteerThenDriveXYPatternTask::OnNewMail(MOOSMSG_LIST & NewMail)
{
    CMOOSMsg Msg;
      if(PeekMail(NewMail,"XYPATTERN_LOCATIONS",Msg))
    {
        DebugNotify("CSteerThenDriveXYPatternTask::reloading locations\n");
        //start afresh...
        m_Positions.clear();
        m_XYPoints.clear();
        m_nTotalPositions = 0;
        m_nRepCounter = 0;
        m_bInitialised = false;
        m_nCurrentPosition = 0;

        string sData = Msg.m_sVal;
        while(!sData.empty())
        {
            string sChunk = MOOSChomp(sData,"|");
            string sTok = MOOSChomp(sChunk,"=");
            string sVal = sChunk;
            SetParam(sTok,sVal);
        }

    }

    //now call base class default;
    return CXYPatternTask::OnNewMail(NewMail);

}

bool CSteerThenDriveXYPatternTask::SetParam(string sParam, string sVal)
{
    MOOSToUpper(sParam);
    MOOSToUpper(sVal);

    if(MOOSStrCmp(sParam,"OUTERYAWLIMIT"))
    {
        m_dfOuterYawTolerance=MOOSDeg2Rad(atof(sVal.c_str()));
    }
    else if(MOOSStrCmp(sParam,"INNERYAWLIMIT"))
    {
        m_dfInnerYawTolerance=MOOSDeg2Rad(atof(sVal.c_str()));
    }
    else if(MOOSStrCmp(sParam,"THRUSTDAMPING"))
    {
        m_dfAlpha=atof(sVal.c_str());
    }
    else
    {
        if(!CXYPatternTask::SetParam(sParam,sVal))
        {
            //hmmm - it wasn't for us at all: base class didn't understand either
            MOOSTrace("Param \"%s\" not understood!\n",sParam.c_str());
            return false;
        }
    }

    return true;
}
bool CSteerThenDriveXYPatternTask::SetControl(CPathAction &DesiredAction,double dfRudder,double dfThrust)
{
    //we always set rudder
   DesiredAction.Set(  ACTUATOR_RUDDER,
                        dfRudder,
                        m_nPriority,
                        GetName().c_str());

    //but not always thrust

    double dfFE = fabs(m_YawDOF.GetError());
    if(dfFE>m_dfOuterYawTolerance)
    {
        //too much error! stop we might hit somethging steer first
        SetThrust(DesiredAction,0);
    }
    else if(dfFE<=m_dfInnerYawTolerance)
    {
        SetThrust(DesiredAction,dfThrust);
    }
    else
    {

        if(m_dfInnerYawTolerance>m_dfOuterYawTolerance)
        {
            MOOSTrace("bad tolerance values - resetting to defaults\n");
            m_dfOuterYawTolerance= DEFAULT_OUTER_YAW_TOLERANCE;
            m_dfInnerYawTolerance = DEFAULT_INNER_YAW_TOLERANCE;    
        }
        //some middle ground here...
        double dfNewThrust = dfThrust+(dfFE-m_dfInnerYawTolerance)*(-dfThrust)/(m_dfOuterYawTolerance-m_dfInnerYawTolerance);

        SetThrust(DesiredAction,dfNewThrust);
    }
    

    return true;

}

bool CSteerThenDriveXYPatternTask::SetThrust(CPathAction &DesiredAction, double dfThrust)
{
    double dfOut = (1.0-m_dfAlpha)*dfThrust+m_dfAlpha*m_dfLastThrust;

    if(dfOut<1)
    {
        dfOut = 0;
    }

    DesiredAction.Set(ACTUATOR_THRUST,dfOut,m_nPriority,
                                    GetName().c_str());

    m_dfLastThrust = dfOut;

    return true;

}

bool CSteerThenDriveXYPatternTask::OnComplete()
{
    //this special task never quite bevause of its spatial
    //location only because of a timeout

    //overloading it stops the base class version running
    return true;
}
