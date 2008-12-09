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
// PilotTask.cpp: implementation of the CPilotTask class.
//
//////////////////////////////////////////////////////////////////////

#include "PilotTask.h"
#define SET_POINT_TIMEOUT 20.0

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define DEFAULT_SPEED_TO_THRUST_FACTOR 30.0
CPilotTask::CPilotTask()
{
    m_bInitialised = false;
    m_dfSpeedToThrustFactor = DEFAULT_SPEED_TO_THRUST_FACTOR;
}

CPilotTask::~CPilotTask()
{

}

//returns false if we haven't received data in a while..bad news!
bool CPilotTask::RegularMailDelivery(double dfTimeNow)
{
      if(m_YawDOF.IsStale(dfTimeNow,GetStartTime()))
      {
          DebugNotify(MOOSFormat("%s Not Rx'ing NAV_YAW\n",GetName().c_str()));
          return false;
      }
      if(m_DepthDOF.IsStale(dfTimeNow,GetStartTime()))
      {
          DebugNotify(MOOSFormat("%s Not Rx'ing NAV_DEPTH\n",GetName().c_str()));
          return false;
      }
      if(m_YawSetPoint.IsStale(dfTimeNow,GetStartTime(),SET_POINT_TIMEOUT))
      {
          DebugNotify(MOOSFormat("%s Not Rx'ing PILOT_DESIRED_YAW for more than %f s\n",GetName().c_str(),SET_POINT_TIMEOUT));
          return false;
      }
      if(m_SpeedSetPoint.IsStale(dfTimeNow,GetStartTime(),SET_POINT_TIMEOUT))
      {
          DebugNotify(MOOSFormat("%s Not Rx'ing PILOT_DESIRED_SPEED for more than %f s\n",GetName().c_str(),SET_POINT_TIMEOUT));
          return false;
      }
      if(m_DepthSetPoint.IsStale(dfTimeNow,GetStartTime(),SET_POINT_TIMEOUT))
      {
          DebugNotify(MOOSFormat("%s Not Rx'ing PILOT_DESIRED_DEPTH for more than %f s\n",GetName().c_str(),SET_POINT_TIMEOUT));
          return false;
      }



      return true;

}

bool CPilotTask::GetNotifications(MOOSMSG_LIST & List)
{
   if(m_bActive)
   {
   }
   
   return CMOOSBehaviour::GetNotifications(List);
}


bool CPilotTask::Run(CPathAction &DesiredAction)
{

    if(!m_bInitialised)
    {
        Initialise();
    }
    
    if(m_YawDOF.IsValid())
    {

        double dfError = MOOS_ANGLE_WRAP(m_YawDOF.GetError());

        double dfCmd= 0;


        // 1 RUDDER CONTROL
        //this is for loggin purposes only
        m_YawPID.SetGoal(m_YawDOF.GetDesired());

        if(m_YawPID.Run(dfError,m_YawDOF.GetErrorTime(),dfCmd))
        {

            //OK we need to change something
            DesiredAction.Set(  ACTUATOR_RUDDER,
                                -(dfCmd),
                                m_nPriority,
                                m_sName.c_str());            
            
            //MOOSTrace("just wrote rudder = %f\n",dfCmd);

        }

        //2 THRUST CONTROL
        if(m_SpeedSetPoint.IsValid())
        {
            //OK we need to change something
            DesiredAction.Set(  ACTUATOR_THRUST,
                                m_SpeedSetPoint.GetCurrent(),
                                m_nPriority,
                                m_sName.c_str());                

            //MOOSTrace("just wrote thrust = %f\n",m_ThrustSetPoint.GetCurrent());

        }

        //3 ELEVATOR CONTROL
        if(m_DepthDOF.IsValid() && m_PitchDOF.IsValid())
        {
            double dfError = m_DepthDOF.GetError();
            
            double dfCmd= 0;
            
            //this is for logging purposes only
            m_ZPID.SetGoal(m_DepthDOF.GetDesired());
            
            
            if(m_ZPID.Run(dfError,
                m_DepthDOF.GetErrorTime(),
                dfCmd,m_PitchDOF.GetCurrent(),
                m_PitchDOF.GetErrorTime()))
            {
                //OK we need to change something
                DesiredAction.Set(  ACTUATOR_ELEVATOR,
                    dfCmd,
                    m_nPriority,
                    m_sName.c_str());                

                //MOOSTrace("just wrote elevator = %f\n",dfCmd);

            }
        }

    }
    return true;
}

bool CPilotTask::OnNewMail(MOOSMSG_LIST &NewMail)
{
    CMOOSMsg Msg;
    
    //get yaw
    if(PeekMail(NewMail,"NAV_YAW",Msg))
    { 
        if(!Msg.IsSkewed(GetTimeNow()))
        {           
            m_YawDOF.SetCurrent(Msg.GetDouble(),Msg.m_dfTime);
        }
    }

    //depth
    if(PeekMail(NewMail,"NAV_DEPTH",Msg))
    {
        if(!Msg.IsSkewed(GetTimeNow()))
        {
            m_DepthDOF.SetCurrent(Msg.GetDouble(),Msg.m_dfTime);
        }
    }

    //pitch
    if(PeekMail(NewMail,"NAV_PITCH",Msg))
    {
        if(!Msg.IsSkewed(GetTimeNow()))
        {
            m_PitchDOF.SetCurrent(Msg.GetDouble(),Msg.m_dfTime);
        }
    }

    if(PeekMail(NewMail,"PILOT_DESIRED_YAW",Msg))
    {
        if(!Msg.IsSkewed(GetTimeNow()))
        {
            if(MOOSStrCmp(Msg.GetSource(),m_sYawSetPointSourceProcess))
            {
                m_YawDOF.SetDesired(Msg.GetDouble());            
                m_YawSetPoint.SetCurrent(Msg.GetDouble(),Msg.GetTime());
            }
            else
            {
                static double dfT =MOOSTime();
                if(MOOSTime()-dfT>2.0)
                {
                    DebugNotify(MOOSFormat("Process %s : no Pilot task permission",Msg.GetSource().c_str()));
                    dfT = MOOSTime();
                }
            }
        }
    }

    if(PeekMail(NewMail,"PILOT_DESIRED_DEPTH",Msg))
    {
        if(!Msg.IsSkewed(GetTimeNow()))
        {
            if(MOOSStrCmp(Msg.GetSource(),m_sDepthSetPointSourceProcess))
            {
                m_DepthDOF.SetDesired(Msg.GetDouble());            
                m_DepthSetPoint.SetCurrent(Msg.GetDouble(),Msg.GetTime());
            }
            else
            {
                static double dfT =MOOSTime();
                if(MOOSTime()-dfT>2.0)
                {
                    DebugNotify(MOOSFormat("Process %s : no Pilot task permission",Msg.GetSource().c_str()));
                    dfT = MOOSTime();
                }
            }
        }
    }

    if(PeekMail(NewMail,"PILOT_DESIRED_SPEED",Msg))
    {
        if(!Msg.IsSkewed(GetTimeNow()))
        {
            if(MOOSStrCmp(Msg.GetSource(),m_sSpeedSetPointSourceProcess))
            {
                double dfT = MOOSClamp<double>(m_dfSpeedToThrustFactor*Msg.GetDouble(),0,100);
                m_SpeedSetPoint.SetCurrent(dfT,Msg.GetTime());
//                MOOSTrace("Just Set thrust SP to %f\n",Msg.GetDouble());
            }
            else
            {
                static double dfT =MOOSTime();
                if(MOOSTime()-dfT>2.0)
                {

                    DebugNotify(MOOSFormat("Process %s : no Pilot task permission",Msg.GetSource().c_str()));
                    dfT = MOOSTime();
                }
            }
        }
    }

    //always call base class version
    CMOOSBehaviour::OnNewMail(NewMail);

    return true;
}

bool CPilotTask::GetRegistrations(STRING_LIST &List)
{

    List.push_front("NAV_YAW");
    List.push_front("NAV_PITCH");
    List.push_front("NAV_DEPTH");
    List.push_front("PILOT_DESIRED_YAW");
    List.push_front("PILOT_DESIRED_DEPTH");
    List.push_front("PILOT_DESIRED_SPEED");

    //always call base class version
    CMOOSBehaviour::GetRegistrations(List);

    return true;
}


bool CPilotTask::SetParam(string sParam, string sVal)
{
    MOOSToUpper(sParam);
    MOOSToUpper(sVal);


    if(!CMOOSBehaviour::SetParam(sParam,sVal))
    {
        //this is for us...
        if(MOOSStrCmp(sParam,"YawControllingProcess"))
        {
            m_sYawSetPointSourceProcess = sVal;
        }        
        //this is for us...
        else if(MOOSStrCmp(sParam,"SpeedControllingProcess"))
        {
            m_sSpeedSetPointSourceProcess = sVal;
        }        
        //this is for us...
        else if(MOOSStrCmp(sParam,"DepthControllingProcess"))
        {
            m_sDepthSetPointSourceProcess = sVal;
        }            
        else if(MOOSStrCmp(sParam,"SpeedToThrustFactor"))
        {
            m_dfSpeedToThrustFactor = atof(sVal.c_str());
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

bool CPilotTask::Initialise()
{
    if(m_sYawSetPointSourceProcess=="")
    {
        return MOOSFail("%s : \"YawControllingProcess\" not set\n",GetName().c_str());
    }
    
    if(m_sDepthSetPointSourceProcess=="")
    {
        return MOOSFail("%s : \"DepthControllingProcess\" not set\n",GetName().c_str());
    }
    if(m_sSpeedSetPointSourceProcess=="")
    {
        return MOOSFail("%s : \"SpeedControllingProcess\" not set\n",GetName().c_str());
    }



    //set a pitch driven depth controller
    m_ZPID.SetAsDepthController(true);


    m_bInitialised = true;
    return true;
}
