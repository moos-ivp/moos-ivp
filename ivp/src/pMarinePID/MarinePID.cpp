/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MarinePID.cpp                                        */
/*    DATE: Apr 10 2006                                          */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iterator>
#include <iostream>
#include <math.h>
#include <string.h>
#include "MarinePID.h"
#include "MBUtils.h"

using namespace std;

#define VERBOSE

//--------------------------------------------------------------------
// Procedure: Constructor

MarinePID::MarinePID()
{
  has_control   = false;
  allow_overide = true;
  depth_control = true;
  ylog_flag     = true;
  verbose       = "terse";
  speed_factor  = 0;

  desired_heading = 0;
  desired_speed   = 0;
  desired_depth   = 0;

  current_heading = 0;
  current_speed   = 0;
  current_depth   = 0;
  current_pitch   = 0;

  max_thrust   = 100;
  max_rudder   = 100;
  max_pitch    = 15;
  max_elevator = 13;
  iteration    = 0;
  start_time   = 0;

  paused       = false;

  time_of_last_helm_msg = 0;
  time_of_last_nav_msg  = 0;
}

//--------------------------------------------------------------------
// Procedure: OnNewMail()

bool MarinePID::OnNewMail(MOOSMSG_LIST &NewMail)
{
  // Iterate through the list in reverse since the most recent
  // information is at the beginning and we dont want older
  // actuation commands overwriting newer ones.
  MOOSMSG_LIST::reverse_iterator p;
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;
    double dfT;

    msg.IsSkewed(MOOSTime(),&dfT);


    string key = toupper(stripBlankEnds(msg.m_sKey));
    if(key == "SPEED_FACTOR") {
      speed_factor = msg.m_dfVal;
      pengine.setSpeedFactor(speed_factor);
    }

    #define ACCEPTABLE_SKEW_FIX_ME 360.0
    if(fabs(dfT)<ACCEPTABLE_SKEW_FIX_ME) {
      if(key == "MOOS_MANUAL_OVERIDE") {
	if(MOOSStrCmp(msg.m_sVal, "FALSE")) {
	  has_control = true;
	  MOOSTrace("\n");
	  MOOSDebugWrite("pMarinePID Control Is On");
	}
	else {
	  if(allow_overide) {
	    has_control = false;
	    MOOSTrace("\n");
	    MOOSDebugWrite("pMarinePID Control Is Off");
	  }
	}
      }
      else if(key == "PID_VERBOSE") {
	if(msg.m_sVal == "verbose")
	  verbose = "verbose";
	else if(msg.m_sVal == "quiet")
	  verbose = "quiet";
	else
	  verbose = "terse";
      }
      else if(key == "NAV_YAW")
	current_heading = -MOOSRad2Deg(msg.m_dfVal);
      else if(key == "NAV_HEADING")
	current_heading = msg.m_dfVal;
      else if(key == "NAV_SPEED")
	current_speed = msg.m_dfVal;
      else if(key == "NAV_DEPTH")
	current_depth = msg.m_dfVal;
      else if(key == "NAV_PITCH")
	current_pitch = msg.m_dfVal;
      
      if(!strncmp(key.c_str(), "NAV_", 4))
	time_of_last_nav_msg = MOOSTime();

      else if(key == "DESIRED_THRUST")
	current_thrust = msg.m_dfVal;
      else if(key == "DESIRED_HEADING") {
	desired_heading = msg.m_dfVal;
	time_of_last_helm_msg = MOOSTime();
      }
      else if(key == "DESIRED_SPEED") {
	desired_speed = msg.m_dfVal;
	time_of_last_helm_msg = MOOSTime();
      }
      else if(key == "DESIRED_DEPTH")
	desired_depth = msg.m_dfVal;

      }
  }
  return(true);
}

//--------------------------------------------------------------------
// Procedure: Iterate()
//      Note: happens AppTick times per second
//            

bool MarinePID::Iterate()
{
  iteration++;
  postCharStatus();

  if(!has_control)
    return(false);

  double current_time = MOOSTime();

  if(verbose == "verbose") {
    double hz = iteration / (MOOSTime() - start_time);
    cout << endl << endl << endl;
    cout << "PID REPORT: (" << iteration << ")";
    cout << "(" << hz << "/sec)" << endl;
  }


  if((current_time - time_of_last_helm_msg) > 2) {
    if(!paused)
      MOOSDebugWrite("Paused Due To Tardy HELM Input: THRUST=0");
    cout << "Paused Due To Tardy HELM Input: THRUST=0" << endl;
    paused = true;
    m_Comms.Notify("DESIRED_THRUST", 0);
    return(true);
  }
  
  if((current_time - time_of_last_nav_msg) > 2) {
    if(!paused)
      MOOSDebugWrite("Paused Due To Tardy NAV Input: THRUST=0");
    cout << "Paused Due To Tardy NAV Input: THRUST=0" << endl;
    paused = true;
    m_Comms.Notify("DESIRED_THRUST", 0);
    return(true);
  }

  double rudder = 0;
  double thrust = 0;
  double elevator = 0;

  pengine.updateTime(current_time);

  rudder = pengine.getDesiredRudder(desired_heading, current_heading, 
				    max_rudder);
  thrust = pengine.getDesiredThrust(desired_speed, current_speed, 
				    current_thrust, max_thrust);
  if(depth_control)
    elevator = pengine.getDesiredElevator(desired_depth, current_depth,
					  current_pitch, max_pitch, 
					  max_elevator);
  
  if((desired_speed <= 0.001) && (desired_speed >= -0.001))
    thrust = 0;

  vector<string> pid_report;
  if(verbose == "verbose") {
    pid_report = pengine.getPIDReport();
    pengine.clearReport();
    for(unsigned int i=0; i<pid_report.size(); i++)
      cout << pid_report[i] << endl;
  }

  paused = false;

  if(thrust ==0)
    rudder = 0;
  m_Comms.Notify("DESIRED_RUDDER", rudder);
  m_Comms.Notify("DESIRED_THRUST", thrust);
  if(depth_control)
    m_Comms.Notify("DESIRED_ELEVATOR", elevator);
  
  return(true);
}
  

//------------------------------------------------------------
// Procedure: postCharStatus()

void MarinePID::postCharStatus()
{
  if(has_control) {
    if(verbose == "terse")
      MOOSTrace("$");
  }
  else
    MOOSTrace("*");
}

//------------------------------------------------------------
// Procedure: OnConnectToServer()

bool MarinePID::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));
  
  registerVariables();

  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void MarinePID::registerVariables()
{
  m_Comms.Register("NAV_HEADING", 0);
  m_Comms.Register("NAV_YAW", 0);
  m_Comms.Register("NAV_SPEED", 0);
  m_Comms.Register("NAV_DEPTH", 0);
  m_Comms.Register("NAV_PITCH", 0);
  m_Comms.Register("DESIRED_HEADING", 0);
  m_Comms.Register("DESIRED_SPEED", 0);
  m_Comms.Register("DESIRED_THRUST", 0);
  m_Comms.Register("DESIRED_DEPTH", 0);
  m_Comms.Register("PID_VERBOSE", 0);
  m_Comms.Register("SPEED_FACTOR", 0);
  m_Comms.Register("MOOS_MANUAL_OVERIDE", 0);
}

//--------------------------------------------------------
// Procedure: onStartUp()

bool MarinePID::OnStartUp()
{
  MOOSTrace("pMarinePID starting....\n");
  
  start_time = MOOSTime();
  // speed_factor=0 means speed PID will be used.
  speed_factor = 0;
  
  string pid_logpath  = "";
  
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine    = *p;
    string sVarName = MOOSChomp(sLine, "=");
    sVarName = toupper(sVarName);
    sLine    = stripBlankEnds(sLine);
    
    if(MOOSStrCmp(sVarName, "SPEED_FACTOR"))
      speed_factor = atof(sLine.c_str());
    
    if(MOOSStrCmp(sVarName, "ACTIVE_START")) {
      sLine = tolower(sLine);
      if(sLine == "true") {
	has_control   = true;
	//allow_overide = false;
      }
    }
    
    if(MOOSStrCmp(sVarName, "VERBOSE")) {
      if((sLine == "true") || (sLine == "verbose"))
	verbose = "verbose";
      if(sLine == "terse") 
	verbose = "terse";
      if(sLine == "quiet")
	verbose = "quiet";
    }
    
    if(MOOSStrCmp(sVarName, "YLOG"))
      ylog_flag = (sLine == "true");
    
    if(MOOSStrCmp(sVarName, "LOGPATH"))
      pid_logpath = sLine;
  }

  bool ok_yaw = handleYawSettings();
  bool ok_spd = handleSpeedSettings();
  bool ok_dep = handleDepthSettings();

  if(!ok_yaw || !ok_spd || !ok_dep) {
    MOOSTrace("Improper PID Settings \n");
    return(false);
  }

  if(depth_control)
    cout << "Depth Control is ON" << endl;
  else
    cout << "Depth Control is OFF" << endl;

  registerVariables();
  
  return(true);
}

//--------------------------------------------------------------------
// Procedure: handleYawSettings

bool MarinePID::handleYawSettings()
{
  int ok = true;

  double yaw_pid_Kp, yaw_pid_Kd, yaw_pid_Ki, yaw_pid_ilim;
  if(!m_MissionReader.GetConfigurationParam("YAW_PID_KP", yaw_pid_Kp)) {
    MOOSDebugWrite("YAW_PID_KP not found in Mission File");
    ok = false;
  }
  if(!m_MissionReader.GetConfigurationParam("YAW_PID_KD", yaw_pid_Kd)) {
    MOOSDebugWrite("YAW_PID_KD not found in Mission File");
    ok = false;
  }
  if(!m_MissionReader.GetConfigurationParam("YAW_PID_KI", yaw_pid_Ki)) {
    MOOSDebugWrite("YAW_PID_KI not found in Mission File");
    ok = false;
  }
  if((!m_MissionReader.GetConfigurationParam("YAW_PID_INTEGRAL_LIMIT", yaw_pid_ilim)) &&
     (!m_MissionReader.GetConfigurationParam("YAW_PID_KI_LIMIT", yaw_pid_ilim))) {
    MOOSDebugWrite("YAW_PID_INTEGRAL_LIMIT not found in Mission File");
    ok = false;
  }
  if(!m_MissionReader.GetConfigurationParam("MAXRUDDER",max_rudder)) {
    MOOSDebugWrite("MAXRUDDER not found in Mission File");
    ok = false;
  }
  
  ScalarPID crsPID;
  crsPID.SetGains(yaw_pid_Kp, yaw_pid_Kd, yaw_pid_Ki);
  crsPID.SetLimits(yaw_pid_ilim, 100);
  pengine.setPID(0, crsPID);

  MOOSDebugWrite(MOOSFormat("** NEW CONTROLLER GAINS ARE **"));
  MOOSDebugWrite(MOOSFormat("YAW_PID_KP             = %.3f",yaw_pid_Kp));
  MOOSDebugWrite(MOOSFormat("YAW_PID_KD             = %.3f",yaw_pid_Kd));
  MOOSDebugWrite(MOOSFormat("YAW_PID_KI             = %.3f",yaw_pid_Ki));
  MOOSDebugWrite(MOOSFormat("YAW_PID_INTEGRAL_LIMIT = %.3f",yaw_pid_ilim));
  MOOSDebugWrite(MOOSFormat("MAXRUDDER              = %.3f",max_rudder));
  
  return(ok);
}
  

//--------------------------------------------------------------------
// Procedure: handleSpeedSettings

bool MarinePID::handleSpeedSettings()
{
  int ok = true;
  pengine.setSpeedFactor(speed_factor);

  double spd_pid_Kp, spd_pid_Kd, spd_pid_Ki, spd_pid_ilim;
  if(!m_MissionReader.GetConfigurationParam("SPEED_PID_KP", spd_pid_Kp)) {
    MOOSDebugWrite("SPEED_PID_KP not found in Mission File");
    ok = false;
  }
  if(!m_MissionReader.GetConfigurationParam("SPEED_PID_KD", spd_pid_Kd)) {
    MOOSDebugWrite("SPEED_PID_KD not found in Mission File");
    ok = false;
  }
  if(!m_MissionReader.GetConfigurationParam("SPEED_PID_KI", spd_pid_Ki)) {
    MOOSDebugWrite("SPEED_PID_KI not found in Mission File");
    ok = false;
  }
  if(!m_MissionReader.GetConfigurationParam("SPEED_PID_INTEGRAL_LIMIT", spd_pid_ilim)) {
    MOOSDebugWrite("SPEED_PID_INTEGRAL_LIMIT not found in Mission File");
    ok = false;
  }
  if(!m_MissionReader.GetConfigurationParam("MAXTHRUST",max_thrust))
  {
    MOOSDebugWrite("MAXTHRUST not found in Mission File");
    ok = false;
  }

  ScalarPID spdPID;
  spdPID.SetGains(spd_pid_Kp, spd_pid_Kd, spd_pid_Ki);
  spdPID.SetLimits(spd_pid_ilim, 100);
  pengine.setPID(1, spdPID);

  MOOSDebugWrite(MOOSFormat("SPEED_PID_KP           = %.3f",spd_pid_Kp));
  MOOSDebugWrite(MOOSFormat("SPEED_PID_KD           = %.3f",spd_pid_Kd));
  MOOSDebugWrite(MOOSFormat("SPEED_PID_KI           = %.3f",spd_pid_Ki));
  MOOSDebugWrite(MOOSFormat("SPEED_PID_KI_LIMIT     = %.3f",spd_pid_ilim));
  MOOSDebugWrite(MOOSFormat("MAXTHRUST              = %.3f",max_thrust));

  return(ok);
}

//--------------------------------------------------------------------
// Procedure: handleDepthSettings()

bool MarinePID::handleDepthSettings()
{
  int ok = true;


#if 1
  string depth_control_str = "false";
  m_MissionReader.GetConfigurationParam("DEPTH_CONTROL", depth_control_str);
  depth_control_str = tolower(depth_control_str);
  depth_control = ((depth_control_str == "true") ||
		   (depth_control_str == "1") ||
		   (depth_control_str == "yes"));
#endif

#if 0
  m_MissionReader.GetConfigurationParam("DEPTH_CONTROL", depth_control);
#endif
#if 0
  double dc;
  m_MissionReader.GetConfigurationParam("DEPTH_CONTROL", dc);
  depth_control = (int)dc;
#endif
  
  if(!depth_control)
    return(true);
  

  double z_top_pid_Kp, z_top_pid_Kd, z_top_pid_Ki, z_top_pid_ilim;
  if(!m_MissionReader.GetConfigurationParam("Z_TO_PITCH_PID_KP", z_top_pid_Kp)) {
    MOOSDebugWrite("Z_TO_PITCH_PID_KP not found in Mission File");
    ok = false;
  }
  if(!m_MissionReader.GetConfigurationParam("Z_TO_PITCH_PID_KD", z_top_pid_Kd)) {
    MOOSDebugWrite("Z_TO_PITCH_PID_KD not found in Mission File");
    ok = false;
  }
  if(!m_MissionReader.GetConfigurationParam("Z_TO_PITCH_PID_KI", z_top_pid_Ki)) {
    MOOSDebugWrite("Z_TO_PITCH_PID_KI not found in Mission File");
    ok = false;
  }
  if(!m_MissionReader.GetConfigurationParam("Z_TO_PITCH_PID_INTEGRAL_LIMIT", z_top_pid_ilim)) {
    MOOSDebugWrite("Z_TO_PITCH_PID_INTEGRAL_LIMIT not found in Mission File");
    ok = false;
  }
  if(!m_MissionReader.GetConfigurationParam("MAXPITCH",max_pitch))
  {
    MOOSDebugWrite("MAXPITCH not found in Mission File");
    ok = false;
  }
  // Convert pitch limit to radians
  max_pitch=MOOSDeg2Rad(max_pitch);

  ScalarPID ztopPID;
  ztopPID.SetGains(z_top_pid_Kp, z_top_pid_Kd, z_top_pid_Ki);
  ztopPID.SetLimits(z_top_pid_ilim, 100);
  pengine.setPID(2, ztopPID);

  MOOSDebugWrite(MOOSFormat("Z_TO_PITCH_PID_KP      = %.3f",z_top_pid_Kp));
  MOOSDebugWrite(MOOSFormat("Z_TO_PITCH_PID_KD      = %.3f",z_top_pid_Kd));
  MOOSDebugWrite(MOOSFormat("Z_TO_PITCH_PID_KI      = %.3f",z_top_pid_Ki));
  MOOSDebugWrite(MOOSFormat("Z_TO_PITCH_PID_KI_LIMIT= %.3f",z_top_pid_ilim));
  MOOSDebugWrite(MOOSFormat("MAXELEVATOR            = %.3f",max_elevator));
  
  double pitch_pid_Kp, pitch_pid_Kd, pitch_pid_Ki, pitch_pid_ilim;
  if(!m_MissionReader.GetConfigurationParam("PITCH_PID_KP", pitch_pid_Kp)) {
    MOOSDebugWrite("PITCH_PID_KP not found in Mission File");
    ok = false;
  }
  if(!m_MissionReader.GetConfigurationParam("PITCH_PID_KD", pitch_pid_Kd)) {
    MOOSDebugWrite("PITCH_PID_KD not found in Mission File");
    ok = false;
  }
  if(!m_MissionReader.GetConfigurationParam("PITCH_PID_KI", pitch_pid_Ki)) {
    MOOSDebugWrite("PITCH_PID_KI not found in Mission File");
    ok = false;
  }
  if(!m_MissionReader.GetConfigurationParam("PITCH_PID_INTEGRAL_LIMIT", pitch_pid_ilim)) {
    MOOSDebugWrite("PITCH_PID_INTEGRAL_LIMIT not found in Mission File");
    ok = false;
  }
  if(!m_MissionReader.GetConfigurationParam("MAXELEVATOR",max_elevator))
  {
    MOOSDebugWrite("MAXELEVATOR not found in Mission File");
    ok = false;
  }

  ScalarPID pitchPID;
  pitchPID.SetGains(pitch_pid_Kp, pitch_pid_Kd, pitch_pid_Ki);
  pitchPID.SetLimits(pitch_pid_ilim, 100);
  pengine.setPID(3, pitchPID);

  MOOSDebugWrite(MOOSFormat("PITCH_PID_KP           = %.3f",pitch_pid_Kp));
  MOOSDebugWrite(MOOSFormat("PITCH_PID_KD           = %.3f",pitch_pid_Kd));
  MOOSDebugWrite(MOOSFormat("PITCH_PID_KI           = %.3f",pitch_pid_Ki));
  MOOSDebugWrite(MOOSFormat("PITCH_PID_KI_LIMIT     = %.3f",pitch_pid_ilim));
  MOOSDebugWrite(MOOSFormat("MAXPITCH               = %.3f",max_pitch));


  return(ok);
}









