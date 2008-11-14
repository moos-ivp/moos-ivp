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
// XYPatternTask.cpp: implementation of the CXYPatternTask class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

#include "MOOSTaskDefaults.h"
#include "XYPatternTask.h"
#include "math.h"
#include <sstream>
#include <iostream>
using namespace std;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXYPatternTask::CXYPatternTask()
{
    m_nPriority = 2;


    m_bInitialised = false;

    m_dfVicinityRadius = 0;

    m_bPositionSet = true;

    m_bThrustSet = false;

    m_dfThrust = 0;

    m_nTotalPositions = 0;

    m_nTotalRepetitions = 0;

    m_nCurrentPosition = 0;

    m_nRepCounter = 0;

}

CXYPatternTask::~CXYPatternTask()
{
    m_XYPoints.clear();
}

bool CXYPatternTask::OnNewMail(MOOSMSG_LIST &NewMail)
{

    CMOOSMsg Msg;

	if(PeekMail(NewMail,"NAV_POSE",Msg))
	{
        if(!Msg.IsSkewed(GetTimeNow()))
        {
			std::vector<double> Pose;
			int nRows,nCols;
			if(MOOSValFromString(Pose,nRows,nCols,Msg.GetString(),"Pose",true))
			{
				if(Pose.size() == 3)
				{
					m_XDOF.SetCurrent(Pose[0],Msg.GetTime());
					m_YDOF.SetCurrent(Pose[1],Msg.GetTime());
					m_YawDOF.SetCurrent(Pose[2],Msg.GetTime());

                    // MOOSTrace("we are at %f %f %f\n",Pose[0],Pose[1],Pose[2]);
				}
			}

        }
    }

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

        //    MOOSTrace("%s gets yaw = %f @ %f\n",m_sName.c_str(),dfVal,Msg.m_dfTime);
        }

    }

    //always call base class version
    CMOOSBehaviour::OnNewMail(NewMail);

    return true;
}

bool CXYPatternTask::GetRegistrations(STRING_LIST &List)
{
    List.push_front("NAV_X");
    List.push_front("NAV_Y");
    List.push_front("NAV_YAW");
    List.push_front("NAV_POSE");

    //always call base class version
    CMOOSBehaviour::GetRegistrations(List);

    return true;
}



//returns false if we haven't received data in a while..bad news!
bool CXYPatternTask::RegularMailDelivery(double dfTimeNow)
{
    if(m_YawDOF.IsStale(dfTimeNow,GetStartTime()))
        return false;
    
    if(m_XDOF.IsStale(dfTimeNow,GetStartTime()))
        return false;

    if(m_YDOF.IsStale(dfTimeNow,GetStartTime()))
        return false;

    return true;
}



bool CXYPatternTask::Run(CPathAction &DesiredAction)
{
    if(!m_bPositionSet)
    {
        MOOSTrace("position not set\n");;
        return false;
    }

    if(!m_bInitialised)
    {
        Initialise();
    }

    if(ShouldRun())
    {
     
        if(ValidData() && m_nTotalPositions!=0)
        {
                   
            double dfDistanceToGo = sqrt(pow(m_YDOF.GetError(),2)+pow(m_XDOF.GetError(),2));


            if(dfDistanceToGo<m_dfVicinityRadius)
            {            
                
                if(m_nRepCounter < m_nTotalRepetitions)
                {
                    SetNextPoint();
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

//				MOOSTrace("x_err = %f y_err = %f current_yaw = %f desired_yaw = %f\n",
                //				m_XDOF.GetError(),m_YDOF.GetError(),m_YawDOF.GetCurrent(),dfDesiredYaw);


                dfError = MOOS_ANGLE_WRAP(dfError);
        
                double dfCmd = 0;

                //this is for loggin purposes only
                m_YawPID.SetGoal(m_YawDOF.GetDesired());
                if(m_YawPID.Run(dfError,m_YawDOF.GetErrorTime(),dfCmd))
                {
                    //OK we need to change something
                    SetControl(DesiredAction,-dfCmd,m_dfThrust);                        
                }
            }
        }
    }
    return true;
}


bool CXYPatternTask::SetControl(CPathAction &DesiredAction,double dfRudder,double dfThrust)
{
    DesiredAction.Set(  ACTUATOR_RUDDER,
                        dfRudder,
                        m_nPriority,
                        GetName().c_str());

    //set the thrust
    if(m_bThrustSet)
    {
        DesiredAction.Set(  ACTUATOR_THRUST,
                            dfThrust,
                            m_nPriority,
                            GetName().c_str());
    }

    return true;
}



bool CXYPatternTask::Initialise()
{
    
    m_YawDOF.SetDesired(0);
    m_YawDOF.SetTolerance(0.00);

    //begin the loop-d-loop of points 
    SetNextPoint();

    m_bInitialised = true;

    return m_bInitialised;
}

bool CXYPatternTask::ValidData()
{   
    return  m_XDOF.IsValid() &&
            m_YDOF.IsValid() &&
            m_YawDOF.IsValid();
}



bool CXYPatternTask::SetParam(string sParam, string sVal)
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
            string sCopyOfLocation = sVal;

            string sTmpX = MOOSChomp(sVal,",");
            string sTmpY = MOOSChomp(sVal,",");
            string sTmpZ = MOOSChomp(sVal,",");
            
            if(sTmpX.empty()||sTmpY.empty()||sTmpZ.empty())
            {
                MOOSTrace("error in reading  location from file\n");
                return false;
            }

            //instead of setting this as the X,Y point to go to,
            //store the points in a list that will be parsed later
            m_Positions.push_back(sCopyOfLocation);
            m_nTotalPositions++;

        }
        else if((sParam == "REPEAT"))
        {
            int nTotal = atoi(sVal.c_str());
            //only accept positive values
            if((nTotal > 0) && (nTotal < XYPATTERN_MAX_TOTAL_REPETITION))
            {
                m_nTotalRepetitions = nTotal;
            }
            else if((nTotal > 0) && (nTotal > XYPATTERN_MAX_TOTAL_REPETITION))
            {
                m_nTotalRepetitions = XYPATTERN_MAX_TOTAL_REPETITION;
            }
            else
            {
                //default
                m_nTotalRepetitions = XYPATTERN_DEFAULT_TOTAL_REPETITION;
            }

        }
        else
        {
            //hmmm - it wasn't for us at all: base class didn't understand either
            MOOSTrace("Param \"%s\" not understood by %s!\n",sParam.c_str(),m_sName.c_str());
            return false;
        }
    }

    return true;
}


void CXYPatternTask::SetNextPoint()
{
    //first time through, we need to make some calculations
    if(!m_bInitialised)
    {
        //always start by heading to the first position around the center
        m_nCurrentPosition = 0;
        
        //guarantee enough space
        m_XYPoints.clear();
        m_XYPoints.resize(m_nTotalPositions + 1);

        for(int i = 0; i < m_nTotalPositions; i++)
        {
            string sVal = m_Positions.front();
            m_Positions.pop_front();
            double dfX  = atof(MOOSChomp(sVal,",").c_str());
            double dfY  = atof(MOOSChomp(sVal,",").c_str());

            CXYPoint Position;
            
            Position.SetX(dfX);
            Position.SetY(dfY);

            m_XYPoints[i] = Position;
            
        }
    }
    else
    {
        //otherwise just augment our current point we are heading to
        //and have the new Desired location input for the controller
        m_nCurrentPosition++;
        //make sure we go in a circle
        m_nCurrentPosition %= m_nTotalPositions;
        //augment our repetitions
        if(m_nCurrentPosition==0)
        {
            m_nRepCounter++;
        }
    }

    //announce where we are going to
    ostringstream os;

    os<<"XYPATTERN - transiting to: pos["<<m_nCurrentPosition<<"]"<<
        " -> "<<m_XYPoints[m_nCurrentPosition].GetX()<<","<<
        m_XYPoints[m_nCurrentPosition].GetY()<<" - [REP : "<<
        m_nRepCounter<<"]"<<endl<<ends;

    DebugNotify(os.str());

    //now set the point we are heading to
    m_XDOF.SetDesired(m_XYPoints[m_nCurrentPosition].GetX());
    m_YDOF.SetDesired(m_XYPoints[m_nCurrentPosition].GetY());

    return;
}
