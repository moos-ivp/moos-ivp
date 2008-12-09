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
// OrbitTask.cpp: implementation of the COrbitTask class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

#include "MOOSTaskDefaults.h"
#include "OrbitTask.h"
#include "math.h"

#include <vector>
#include <sstream>
#include <iostream>
using namespace std;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COrbitTask::COrbitTask()
{

    m_nPriority = 3;

    m_bInitialised = false;

    m_dfVicinityRadius = 10;

    m_bPositionSet = true;

    m_bThrustSet = false;

    m_dfThrust = 0;

    m_nTotalPositions = ORBIT_DEFAULT_TOTAL_POSITIONS;

    m_nOrbitDirection = CCW;

    m_nTotalRepetitions = 0;

    m_nRepCounter = 0;

    m_dfPositionRadius = 2.0;
}

COrbitTask::~COrbitTask()
{
    m_XYPoints.clear();
}


bool COrbitTask::OnNewMail(MOOSMSG_LIST &NewMail)
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
        //    MOOSTrace("%s gets yaw = %f@ %f\n",m_sName.c_str(),dfVal,Msg.m_dfTime);
            m_YawDOF.SetCurrent(dfVal,Msg.m_dfTime);
        }

    }

    //always call base class version
    CMOOSBehaviour::OnNewMail(NewMail);

    return true;
}

bool COrbitTask::GetRegistrations(STRING_LIST &List)
{
    List.push_front("NAV_X");
    List.push_front("NAV_Y");
    List.push_front("NAV_YAW");

    //always call base class version
    CMOOSBehaviour::GetRegistrations(List);

    return true;
}



//returns false if we haven't received data in a while..bad news!
bool COrbitTask::RegularMailDelivery(double dfTimeNow)
{
    if(m_YawDOF.IsStale(dfTimeNow,GetStartTime()))
        return false;

    if(m_XDOF.IsStale(dfTimeNow,GetStartTime()))
        return false;

    if(m_YDOF.IsStale(dfTimeNow,GetStartTime()))
        return false;

    return true;
}



bool COrbitTask::Run(CPathAction &DesiredAction)
{
    if(!m_bPositionSet)
    {
        MOOSTrace("Orbit position not set\n");;
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

            double dfDistanceToGo = sqrt(pow(m_YDOF.GetError(),2)+pow(m_XDOF.GetError(),2));


            if(dfDistanceToGo<m_dfPositionRadius)
            {
                if(m_nRepCounter < m_nTotalRepetitions)
                {
                    SetNextPointInOrbit();
                }
                else
                {
                    OnComplete();
                }
            }
            else
            {

                //calculate vector heading angle to goal
                double dfDesiredYaw = -atan2(m_XDOF.GetError(),m_YDOF.GetError());

                m_YawDOF.SetDesired(dfDesiredYaw);

                double dfError = m_YawDOF.GetError();

                if(dfError<-PI)
                {
                    dfError+=2*PI;
                }
                else if(dfError>PI)
                {
                    dfError-=2*PI;
                }

                double dfCmd = 0;

                //this is for logging purposes only
                m_YawPID.SetGoal(m_YawDOF.GetDesired());

                if(m_YawPID.Run(dfError,m_YawDOF.GetErrorTime(),dfCmd))
                {
                        //OK we need to change something

                        DesiredAction.Set(  ACTUATOR_RUDDER,
                                            -dfCmd,
                                            m_nPriority,
                                            GetName().c_str());


                        //set the thrust
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

bool COrbitTask::Initialise()
{

    m_YawDOF.SetDesired(0);
    m_YawDOF.SetTolerance(0.00);

    //begin the loop-d-loop of points for this orbit
    SetNextPointInOrbit();

    m_bInitialised = true;

    return m_bInitialised;
}

bool COrbitTask::ValidData()
{
    return  m_XDOF.IsValid() &&
            m_YDOF.IsValid() &&
            m_YawDOF.IsValid();
}



bool COrbitTask::SetParam(string sParam, string sVal)
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
        else if(sParam=="POSRADIUS")
        {
            m_dfPositionRadius=atof(sVal.c_str());
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

            if(sTmpX.empty()||sTmpY.empty()||sTmpZ.empty())
            {
                MOOSTrace("error in reading orbit location from file\n");
                return false;
            }

            //instead of setting this as the X,Y point to go to,
            //this information is actually the center of the circle
            //that we want to orbit
            double dfX =   atof(sTmpX.c_str());
            m_XOrbitCenter.SetDesired(dfX);

            double dfY =   atof(sTmpY.c_str());
            m_YOrbitCenter.SetDesired(dfY);

        }
        else if((sParam=="DIRECTION"))
        {
            //default is CCW, which is +1 for the multiplier
            if(sVal == "CW")
            {
                m_nOrbitDirection = CW;
            }
            else if(sVal == "CCW")
            {
                m_nOrbitDirection = CCW;
            }

        }
        else if((sParam == "REPEAT"))
        {
            int nTotal = atoi(sVal.c_str());
            //only accept positive values
            if((nTotal > 0) && (nTotal < ORBIT_MAX_TOTAL_REPETITION))
            {
                m_nTotalRepetitions = nTotal;
            }
            else if((nTotal > 0) && (nTotal > ORBIT_MAX_TOTAL_REPETITION))
            {
                m_nTotalRepetitions = ORBIT_MAX_TOTAL_REPETITION;
            }
            else
            {
                //default
                m_nTotalRepetitions = ORBIT_DEFAULT_TOTAL_REPETITION;
            }

        }
        else if((sParam == "TOTALPOS") || (sParam == "SIDES"))
        {
            int nTotal = atoi(sVal.c_str());
            //only accept positive values
            if((nTotal > 0) && (nTotal < ORBIT_MAX_TOTAL_POSITIONS))
            {
                m_nTotalPositions = nTotal;
            }
            else if((nTotal > 0) && (nTotal > ORBIT_MAX_TOTAL_POSITIONS))
            {
                m_nTotalPositions = ORBIT_MAX_TOTAL_POSITIONS;
            }
            else
            {
                //default
                m_nTotalPositions = ORBIT_DEFAULT_TOTAL_POSITIONS;
            }

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


void COrbitTask::SetNextPointInOrbit()
{
    //first time through, we need to make some calculations
    if(!m_bInitialised)
    {
        //always start by heading to the first position around the center
        m_nCurrentPosition = 0;
        //note the direction
        int nD = m_nOrbitDirection;
        //how many degrees are the points offset by
        int nDegSeparation = 360 / m_nTotalPositions;
        //note the radius here is the Vicinity, as this is how close to the
        //orbit center we will rotate around
        //m_dfPositionRadius defines how close we need to get to positions
        //to qualify our having been at a particular position
        double dfR        = m_dfVicinityRadius;
        double dfXCenter= m_XOrbitCenter.GetDesired();
        double dfYCenter= m_YOrbitCenter.GetDesired();

        //guarantee enough space
        m_XYPoints.clear();
        m_XYPoints.resize(m_nTotalPositions + 1);

        for(int i = 0; i < m_nTotalPositions; i++)
        {
            //CCW is default to rotate, represented by nD = 1
            int nTheta = 360 - (nD * (nDegSeparation * i));
            double  dfRad   = nTheta * PI/180;

            CXYPoint Position;

            Position.SetX((dfXCenter + dfR*cos(dfRad)));
            Position.SetY((dfYCenter + dfR*sin(dfRad)));

            m_XYPoints[i] = Position;

        }

    }
    else
    {
        //augment our current point we are heading to
        m_nCurrentPosition++;
        //make sure we go in a circle
        m_nCurrentPosition %= m_nTotalPositions;

        //augment the reps
        if(m_nCurrentPosition == 0)
        {
            m_nRepCounter++;
        }

    }

    //announce where we are going to
    ostringstream os;

    os<<"ORBIT - transiting to: pos["<<m_nCurrentPosition<<"]"<<
        " -> "<<m_XYPoints[m_nCurrentPosition].GetX()<<","<<
        m_XYPoints[m_nCurrentPosition].GetY()<<" - [REP : "<<
        m_nRepCounter<<"]"<<endl<<ends;

    DebugNotify(os.str());

    //now set the point we are heading to
    m_XDOF.SetDesired(m_XYPoints[m_nCurrentPosition].GetX());
    m_YDOF.SetDesired(m_XYPoints[m_nCurrentPosition].GetY());

    return;
}
