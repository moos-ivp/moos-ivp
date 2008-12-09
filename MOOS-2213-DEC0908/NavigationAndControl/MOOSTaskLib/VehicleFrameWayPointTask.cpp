/*
 *  VehicleFrameWayPointTask.cpp
 *  MOOS
 *
 *  Created by pnewman on 27/03/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "VehicleFrameWayPointTask.h"

#define DEFAULT_VICINITY_RADIUS 0.1
#define POSE_HISTORY_SIZE 10.0


/*
 TaskConfiguration:
 
 PoseSource - Name of message containing pose data (which should be of form "Pose = [3x1]{x,y,a},time = 12.5")
 Radius - precision of waypoint homing
 WayPointSource - Nmae of Message which contains waypoint data (which should be of form "Pose = [3x1]{x,y,a},time = 12.5" where Pose in in vehicle frame at time=XYZ)
 ThrustSource = Name of Message which contains thrust data
 Thrust - if present specifies constant (default thrust)
 
 */



CVehicleFrameWayPointTask::CVehicleFrameWayPointTask()
{
    m_sTargetWayPoint = "";
    m_sPoseSource = "";
    m_sDesiredThrustSource = "";
    m_dfVicinityRadius = DEFAULT_VICINITY_RADIUS;
    m_bThrustSet = false;
    m_bWayPointSet = false;
    
}
CVehicleFrameWayPointTask::~CVehicleFrameWayPointTask()
{
    
}

bool CVehicleFrameWayPointTask::GetRegistrations(STRING_LIST &List)
{
    if(!m_sPoseSource.empty())
    {
        List.push_back(m_sPoseSource);
    }
    if(!m_sTargetWayPoint.empty())
    {
    	List.push_back(m_sTargetWayPoint);    
    }
    return true;
}

bool CVehicleFrameWayPointTask::OnNewMail(MOOSMSG_LIST &NewMail)
{   
    CMOOSMsg Msg;
    
    //are we being infomred of a new pose?
    if(PeekMail(NewMail,m_sPoseSource,Msg))
    {
        std::vector<double> P;
        int nRows,nCols;
        if(MOOSValFromString(P, nRows,nCols,Msg.GetString(),"Pose"))
        {
            double dfTime;
            if(MOOSValFromString(dfTime, Msg.GetString(), "time"))
            {
                CSE2Pose Pose;
                Pose.X = P[0];
                Pose.Y = P[1];
                Pose.A = P[2];
                
                m_PoseHistory[dfTime] = Pose;
                
                m_PoseHistory.MakeSpanTime(POSE_HISTORY_SIZE);
                              
                m_XDOF.SetCurrent(Pose.X,dfTime);
                m_YDOF.SetCurrent(Pose.Y,dfTime);
                m_YawDOF.SetCurrent(Pose.A,dfTime);
            }
           
        }
    }
    
    //are we being told about a new thrust?
    if(!m_sDesiredThrustSource.empty() && PeekMail(NewMail, m_sDesiredThrustSource, Msg))
    {
    	m_dfDesiredThrust = Msg.GetDouble();   
        m_bThrustSet = true;
    }
    
    //have we been sent a new waypoint?
    if(PeekMail(NewMail,m_sTargetWayPoint,Msg))
    {
        OnNewTargetWayPoint(Msg.GetString(),Msg.GetTime());
    }
    
      
    //always call base class version
    return CMOOSBehaviour::OnNewMail(NewMail);
    
    
}

bool CVehicleFrameWayPointTask::OnNewTargetWayPoint(const std::string & sVal,double dfMsgTime )
{
    //check it isn't old...
    if(fabs(dfMsgTime-MOOSTime())>1.0)
        return false;
    
    
 	//this sets a waupoint specified  in the coordinate frame of the vehicle
    //at a specified time
    std::vector<double> P;
    int nRows,nCols;
    if(MOOSValFromString(P, nRows,nCols,sVal,"Waypoint"))
    {
        double dfTime;
        if(!MOOSValFromString(dfTime, sVal, "time"))
        {
            dfTime = dfMsgTime;
        }
        
        CSE2Pose WPBody;
        WPBody.X = P[0];
        WPBody.Y = P[1];
        WPBody.A = P[2];
        
        //get gloabl position at this time...
        CSE2Pose PoseVehicle = m_PoseHistory(dfTime);

		double dfMostRecentTime = m_PoseHistory.MaxKey();
		CSE2Pose PoseNow;
		m_PoseHistory.MaxData(PoseNow);
		MOOSTrace("retrieved %.3f %.3f @ %.3f",PoseVehicle.X,PoseVehicle.Y, dfTime);
		MOOSTrace("current %.3f %.3f @ %.3f",PoseNow.X,PoseNow.Y, dfMostRecentTime);



        //figure out gloabl position of way point
        CSE2Pose WPGlobal = Compose(PoseVehicle, WPBody);
        
        //use this gloabl waypoint to set goals;
        m_XDOF.SetDesired(WPGlobal.X);
        m_YDOF.SetDesired(WPGlobal.Y);
        m_YawDOF.SetDesired(WPGlobal.A);
        
        m_bWayPointSet = true;
        
        MOOSTrace("Formed New GlobalWayPoint: %.2f %.2f %.2f\n",WPGlobal.X,WPGlobal.Y,WPGlobal.A);
        
        return true;
        
        
    }
    
	return false;    
}

double CVehicleFrameWayPointTask::GetDistanceToGo()
{
    return sqrt(pow(m_YDOF.GetError(),2)+pow(m_XDOF.GetError(),2));    
}

bool CVehicleFrameWayPointTask::ValidData()
{
    if(m_sPoseSource.empty())
        return false;
    
    if(m_sTargetWayPoint.empty())
        return false;
    
    if(m_PoseHistory.empty())
        return false;
    
    if(!m_XDOF.IsValid())
        return false;
    
    if(!m_YDOF.IsValid())
        return false;
    
    if(!m_YawDOF.IsValid())
        return false;
    
    if(!m_bWayPointSet)
        return false;
    
    return true;
}

bool CVehicleFrameWayPointTask::Run(CPathAction &DesiredAction)
{
       
    if(ShouldRun())
    {
        
        if(ValidData())
        {
            
            double dfDistanceToGo = GetDistanceToGo();
            
            if(dfDistanceToGo<m_dfVicinityRadius)
            {            
                //nothing to do....
                return true;
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
                    
                    DesiredAction.Set(  ACTUATOR_RUDDER,
                                      -dfCmd,
                                      m_nPriority,
                                      GetName().c_str());
                    
                    //MOOSTrace("Rudder Cmd = %f\n",-dfCmd);
                    
                    
                    // we culd also be asked to control thrust
                    if(m_bThrustSet)
                    {
                        DesiredAction.Set(  ACTUATOR_THRUST,
                                          m_dfDesiredThrust,
                                          m_nPriority,
                                          GetName().c_str());
                        
                        //MOOSTrace("Thrust Cmd = %f\n",m_dfDesiredThrust);
                        
                    }
                }
                
                
            }
        }
    }
    return true;
    
}

bool CVehicleFrameWayPointTask::RegularMailDelivery(double dfTimeNow)
{
    if(m_YDOF.IsStale(dfTimeNow, GetStartTime(), 100.0))
        return false;
    
    return true;
}

bool CVehicleFrameWayPointTask::SetParam(string sParam, string sVal)
{
    if(!CMOOSBehaviour::SetParam(sParam,sVal))
    {
        //this is for us...
        if(MOOSStrCmp(sParam,"PoseSource"))
        {
            m_sPoseSource = sVal;
        }
        else if(MOOSStrCmp(sParam,"Radius"))
        {
            m_dfVicinityRadius=atof(sVal.c_str());
        }
        else if(MOOSStrCmp(sParam,"WaypointSource"))
        {
            m_sTargetWayPoint = sVal;
        }
        else if(MOOSStrCmp(sParam, "ThrustSource"))
        {
            m_sDesiredThrustSource = sVal;
        }
        else if(MOOSStrCmp(sParam, "Thrust"))
        {
        	m_dfDesiredThrust = atof(sVal.c_str());    
            m_bThrustSet = true;
        }
		else
        {
        	MOOSTrace("Unknown Parameter - %s\n",sParam.c_str());    
        	return false;
        }
    }            
    return true;
}



