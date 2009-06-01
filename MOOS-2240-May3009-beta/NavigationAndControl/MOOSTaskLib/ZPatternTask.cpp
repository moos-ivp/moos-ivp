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
// ZPatternTask.cpp: implementation of the CZPatternTask class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif


#include "MOOSTaskDefaults.h"
#include "math.h"
#include "ZPatternTask.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CZPatternTask::CZPatternTask()
{
    m_ePatternType = MOOS_Z_PATTERN_ERROR;
    m_eGoal = MOOS_Z_PATTERN_GOAL_MIN_DEPTH;
    m_dfTolerance = 0.3;
    m_dfMaxDepth = 1.0;
    m_dfMinDepth = 1.0;

    m_dfLevelStartTime = -1;
    m_dfLevelDuration = 20.0;


    m_bInitialised = false;
    m_DepthDOF.SetDesired(0.0);


}

CZPatternTask::~CZPatternTask()
{

}




bool CZPatternTask::SetParam(string sParam, string sVal)
{

    MOOSToUpper(sParam);
    MOOSToUpper(sVal);


    if(!CMOOSBehaviour::SetParam(sParam,sVal))
    {
        if(MOOSStrCmp(sParam,"THRUST"))
        {
            m_Thrust.SetCurrent(atof(sVal.c_str()),MOOSTime());
        }
        else if(MOOSStrCmp(sParam,"TOLERANCE"))
    {
        m_dfTolerance = atof(sVal.c_str());
    }
        else if(MOOSStrCmp(sParam,"PATTERN"))
        {
            if(MOOSStrCmp(sVal,"YOYO"))
            {
                m_ePatternType = MOOS_Z_PATTERN_YOYO;
            }
        else if(MOOSStrCmp(sVal,"SQUARE"))
            {
                m_ePatternType = MOOS_Z_PATTERN_SQUARE;
            }
        }
        else if(MOOSStrCmp(sParam,"LEVELTIME"))
    {
        m_dfLevelDuration = atof(sVal.c_str());
    }

        else if(MOOSStrCmp(sParam,"MAXDEPTH"))
    {
        m_dfMaxDepth = atof(sVal.c_str());
    }
        else if(MOOSStrCmp(sParam,"MINDEPTH"))
    {
        m_dfMinDepth = atof(sVal.c_str());
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


//returns false if we haven't received data in a while..bad news!
bool CZPatternTask::RegularMailDelivery(double dfTimeNow)
{
    return !(
    m_DepthDOF.IsStale(dfTimeNow,GetStartTime()) ||
    m_PitchDOF.IsStale(dfTimeNow,GetStartTime())
    );
}



bool CZPatternTask::Initialise()
{
    //set a pitch driven depth controller
    m_ZPID.SetAsDepthController(true);

    m_DepthDOF.SetDesired(m_dfMinDepth);

    m_bInitialised = true;
    return true;
}

bool CZPatternTask::Run(CPathAction &DesiredAction)
{

    if(!m_bInitialised)
    {
        Initialise();
    }
    
    if(m_DepthDOF.IsValid() && m_PitchDOF.IsValid())
    {

    

        double dfError = m_DepthDOF.GetError();
    
    //modfied by David Battle Dec 05
    if (fabs(dfError)<=m_dfTolerance)
    {
        SetNextSetPoint();
    }
       


        double dfCmd= 0;

        //this is for loggin purposes only
        m_ZPID.SetGoal(m_DepthDOF.GetDesired());


        if(m_ZPID.Run(    dfError,
            m_DepthDOF.GetErrorTime(),
            dfCmd,
            m_PitchDOF.GetCurrent(),
            m_PitchDOF.GetErrorTime()))
    {


    
        //OK we need to change something
/*            DesiredAction.Set(  ACTUATOR_THRUST,
            m_Thrust.GetCurrent(),
            m_nPriority,
            "Z Pattern");
*/

            DesiredAction.Set(  ACTUATOR_ELEVATOR,
                                dfCmd,
                                m_nPriority,
                                "Z Pattern");


    }
    }
    return true;
}

bool CZPatternTask::OnNewMail(MOOSMSG_LIST &NewMail)
{
    CMOOSMsg Msg;
    
    if(PeekMail(NewMail,"NAV_DEPTH",Msg))
    {
        if(!Msg.IsSkewed(GetTimeNow()))
        {
        m_DepthDOF.SetCurrent(Msg.m_dfVal,Msg.m_dfTime);
        }
    }

    if(PeekMail(NewMail,"NAV_PITCH",Msg))
    {
        if(!Msg.IsSkewed(GetTimeNow()))
        {
        m_PitchDOF.SetCurrent(Msg.m_dfVal,Msg.m_dfTime);
        }
    }


    //always call base class version
    CMOOSBehaviour::OnNewMail(NewMail);

    return true;
}

bool CZPatternTask::GetRegistrations(STRING_LIST &List)
{

    List.push_front("NAV_DEPTH");
    List.push_front("NAV_PITCH");

    //always call base class version
    CMOOSBehaviour::GetRegistrations(List);

    return true;
}



bool CZPatternTask::SetNextSetPoint()
{
    

    if(m_DepthDOF.GetError()>m_dfTolerance)
    return true;


    
    switch(m_ePatternType)
    {

    case MOOS_Z_PATTERN_SQUARE:

    switch(m_eGoal)
    {

    case MOOS_Z_PATTERN_GOAL_WAIT_MIN:
    case MOOS_Z_PATTERN_GOAL_WAIT_MAX:
        if(MOOSTime()-m_dfLevelStartTime>=m_dfLevelDuration)
        {
        if(m_eGoal==MOOS_Z_PATTERN_GOAL_WAIT_MAX)
        {
            m_eGoal = MOOS_Z_PATTERN_GOAL_MIN_DEPTH;                    
            m_DepthDOF.SetDesired(m_dfMinDepth);
        }
        else if(m_eGoal==MOOS_Z_PATTERN_GOAL_WAIT_MIN)
        {
            m_eGoal = MOOS_Z_PATTERN_GOAL_MAX_DEPTH;
            m_DepthDOF.SetDesired(m_dfMaxDepth);
        }
        }
        break;

    case MOOS_Z_PATTERN_GOAL_MIN_DEPTH:
        m_dfLevelStartTime = MOOSTime();
        m_eGoal = MOOS_Z_PATTERN_GOAL_WAIT_MIN;
        break;

    case MOOS_Z_PATTERN_GOAL_MAX_DEPTH:
        m_dfLevelStartTime = MOOSTime();
        m_eGoal = MOOS_Z_PATTERN_GOAL_WAIT_MAX;
        break;
    }

    break;

    case MOOS_Z_PATTERN_YOYO:
    switch(m_eGoal)
    {
    case MOOS_Z_PATTERN_GOAL_MIN_DEPTH:
        m_DepthDOF.SetDesired(m_dfMaxDepth);
        m_eGoal = MOOS_Z_PATTERN_GOAL_MAX_DEPTH;
        break;
    case MOOS_Z_PATTERN_GOAL_MAX_DEPTH:
        m_DepthDOF.SetDesired(m_dfMinDepth);
        m_eGoal = MOOS_Z_PATTERN_GOAL_MIN_DEPTH;
        break;
    }
    break;

    default:
    return false;
    break;
    }

    return true;
}
