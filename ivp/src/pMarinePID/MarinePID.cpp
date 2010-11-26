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
  m_has_control    = false;
  m_allow_overide  = true;
  m_allstop_posted = false;
  m_depth_control  = true;
  m_verbose        = "terse";
  m_speed_factor   = 0;

  m_desired_heading = 0;
  m_desired_speed   = 0;
  m_desired_depth   = 0;

  m_current_heading = 0;
  m_current_speed   = 0;
  m_current_depth   = 0;

  m_rudder_bias_duration  = 10;
  m_rudder_bias_limit     = 0;
  m_rudder_bias_side      = 1;
  m_rudder_bias_timestamp = 0;

  m_iteration    = 0;
  m_start_time   = 0;

  m_paused       = false;

  m_tardy_helm_thresh = 2.0;
  m_tardy_nav_thresh  = 2.0;

  m_time_of_last_helm_msg = 0;
  m_time_of_last_nav_msg  = 0;
}

//--------------------------------------------------------------------
// Procedure: OnNewMail()

bool MarinePID::OnNewMail(MOOSMSG_LIST &NewMail)
{
  double curr_time = MOOSTime();

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    double dfT;

    msg.IsSkewed(curr_time, &dfT);

    string key = toupper(stripBlankEnds(msg.m_sKey));
    if(key == "SPEED_FACTOR") {
      m_speed_factor = msg.m_dfVal;
      m_pengine.setSpeedFactor(m_speed_factor);
    }

    #define ACCEPTABLE_SKEW_FIX_ME 360.0
    if(fabs(dfT)<ACCEPTABLE_SKEW_FIX_ME) {
      if((key == "MOOS_MANUAL_OVERIDE") ||
	 (key == "MOOS_MANUAL_OVERRIDE")) {
	if(MOOSStrCmp(msg.m_sVal, "FALSE")) {
	  m_has_control = true;
	  MOOSTrace("\n");
	  MOOSDebugWrite("pMarinePID Control Is On");
	}
	else if(MOOSStrCmp(msg.m_sVal, "TRUE")) {
	  if(m_allow_overide) {
	    m_has_control = false;
	    MOOSTrace("\n");
	    MOOSDebugWrite("pMarinePID Control Is Off");
	  }
	}
      }
      else if(key == "PID_VERBOSE") {
	if(msg.m_sVal == "verbose")
	  m_verbose = "verbose";
	else if(msg.m_sVal == "quiet")
	  m_verbose = "quiet";
	else
	  m_verbose = "terse";
      }
      else if(key == "NAV_YAW")
	m_current_heading = -MOOSRad2Deg(msg.m_dfVal);
      else if(key == "NAV_HEADING")
	m_current_heading = msg.m_dfVal;
      else if(key == "NAV_SPEED")
	m_current_speed = msg.m_dfVal;
      else if(key == "NAV_DEPTH")
	m_current_depth = msg.m_dfVal;
      
      if(!strncmp(key.c_str(), "NAV_", 4))
	m_time_of_last_nav_msg = curr_time;

      else if(key == "DESIRED_THRUST")
	m_current_thrust = msg.m_dfVal;
      else if(key == "DESIRED_HEADING") {
	m_desired_heading = msg.m_dfVal;
	m_time_of_last_helm_msg = curr_time;
      }
      else if(key == "DESIRED_SPEED") {
	m_desired_speed = msg.m_dfVal;
	m_time_of_last_helm_msg = curr_time;
      }
      else if(key == "DESIRED_DEPTH")
	m_desired_depth = msg.m_dfVal;
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
  m_iteration++;
  postCharStatus();

  if(!m_has_control) {
    postAllStop();
    return(false);
  }

  double current_time = MOOSTime();

  if(m_verbose == "verbose") {
    double hz = m_iteration / (MOOSTime() - m_start_time);
    cout << endl << endl << endl;
    cout << "PID REPORT: (" << m_iteration << ")";
    cout << "(" << hz << "/sec)" << endl;
  }


  if((current_time - m_time_of_last_helm_msg) > m_tardy_helm_thresh) {
    if(!m_paused)
      MOOSDebugWrite("Paused Due To Tardy HELM Input: THRUST=0");
    cout << "Paused Due To Tardy HELM Input: THRUST=0" << endl;
    m_paused = true;
    m_Comms.Notify("DESIRED_THRUST", 0);
    return(true);
  }
  
  if((current_time - m_time_of_last_nav_msg) > m_tardy_nav_thresh) {
    if(!m_paused)
      MOOSDebugWrite("Paused Due To Tardy NAV Input: THRUST=0");
    cout << "Paused Due To Tardy NAV Input: THRUST=0" << endl;
    m_paused = true;
    m_Comms.Notify("DESIRED_THRUST", 0);
    return(true);
  }

  double rudder = 0;
  double thrust = 0;
  double elevator = 0;

  m_pengine.updateTime(current_time);

  rudder = m_pengine.getDesiredRudder(m_desired_heading, 
				      m_current_heading);
  
  //--------------------
  double rbias_duration = current_time - m_rudder_bias_timestamp;
  if(rbias_duration > m_rudder_bias_duration) {
    rbias_duration = 0;
    m_rudder_bias_timestamp = current_time;
    m_rudder_bias_side *= -1;
  }
  double pct = rbias_duration / m_rudder_bias_duration;
  double bias = m_rudder_bias_side * (pct * m_rudder_bias_limit);
  rudder += bias;
  //--------------------
  

  thrust = m_pengine.getDesiredThrust(m_desired_speed, 
				      m_current_speed, 
				      m_current_thrust);

  if(m_depth_control)
    elevator = m_pengine.getDesiredElevator(m_desired_depth, 
					    m_current_depth);
  
  if((m_desired_speed <= 0.001) && (m_desired_speed >= -0.001))
    thrust = 0;

  vector<string> pid_report;
  if(m_verbose == "verbose") {
    pid_report = m_pengine.getPIDReport();
    for(unsigned int i=0; i<pid_report.size(); i++)
      cout << pid_report[i] << endl;
  }
  m_pengine.clearReport();

  m_paused = false;

  if(thrust ==0)
    rudder = 0;
  m_Comms.Notify("DESIRED_RUDDER", rudder);
  m_Comms.Notify("DESIRED_THRUST", thrust);
  if(m_depth_control)
    m_Comms.Notify("DESIRED_ELEVATOR", elevator);

  m_allstop_posted = false;
  
  return(true);
}
  

//------------------------------------------------------------
// Procedure: postCharStatus()

void MarinePID::postCharStatus()
{
  if(m_has_control) {
    if(m_verbose == "terse")
      MOOSTrace("$");
  }
  else
    MOOSTrace("*");
}

//------------------------------------------------------------
// Procedure: postAllStop()

void MarinePID::postAllStop()
{
  if(m_allstop_posted)
    return;

  m_Comms.Notify("DESIRED_RUDDER", 0);
  m_Comms.Notify("DESIRED_THRUST", 0);
  if(m_depth_control)
    m_Comms.Notify("DESIRED_ELEVATOR", 0);

  m_allstop_posted = true;
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
  m_Comms.Register("DESIRED_HEADING", 0);
  m_Comms.Register("DESIRED_SPEED", 0);
  m_Comms.Register("DESIRED_THRUST", 0);
  m_Comms.Register("DESIRED_DEPTH", 0);
  m_Comms.Register("PID_VERBOSE", 0);
  m_Comms.Register("SPEED_FACTOR", 0);
  m_Comms.Register("MOOS_MANUAL_OVERIDE", 0);
  m_Comms.Register("MOOS_MANUAL_OVERRIDE", 0);
}

//--------------------------------------------------------
// Procedure: onStartUp()

bool MarinePID::OnStartUp()
{
  MOOSTrace("pMarinePID starting....\n");
  
  m_start_time = MOOSTime();
  // m_speed_factor=0 means speed PID will be used.
  m_speed_factor = 0;
  
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
      m_speed_factor = atof(sLine.c_str());
    
    if(MOOSStrCmp(sVarName, "SIM_INSTABILITY"))
      m_rudder_bias_limit = atof(sLine.c_str());

    if(MOOSStrCmp(sVarName, "TARDY_HELM_THRESHOLD")) {
      double dval = atof(sLine.c_str());
      if(dval >= 0)
	m_tardy_helm_thresh = dval;
    }
    if(MOOSStrCmp(sVarName, "TARDY_NAV_THRESHOLD")) {
      double dval = atof(sLine.c_str());
      if(dval >= 0)
	m_tardy_nav_thresh = dval;
    }
    
    if(MOOSStrCmp(sVarName, "ACTIVE_START")) {
      sLine = tolower(sLine);
      if(sLine == "true") {
	m_has_control = true;
	//m_allow_overide = false;
      }
    }
    
    if(MOOSStrCmp(sVarName, "VERBOSE")) {
      if((sLine == "true") || (sLine == "verbose"))
	m_verbose = "verbose";
      if(sLine == "terse") 
	m_verbose = "terse";
      if(sLine == "quiet")
	m_verbose = "quiet";
    }
    
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

  if(m_depth_control)
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
  
  double max_rudder = 100;
  m_MissionReader.GetConfigurationParam("MAXRUDDER", max_rudder);
  
  ScalarPID crsPID;
  crsPID.SetGains(yaw_pid_Kp, yaw_pid_Kd, yaw_pid_Ki);
  crsPID.SetLimits(yaw_pid_ilim, 100);
  m_pengine.setPID(0, crsPID);

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
  m_pengine.setSpeedFactor(m_speed_factor);

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
  
  double max_thrust = 100;
  m_MissionReader.GetConfigurationParam("MAXTHRUST", max_thrust);
  
  ScalarPID spdPID;
  spdPID.SetGains(spd_pid_Kp, spd_pid_Kd, spd_pid_Ki);
  spdPID.SetLimits(spd_pid_ilim, max_thrust);
  m_pengine.setPID(1, spdPID);

  MOOSDebugWrite(MOOSFormat("SPEED_PID_KP       = %.3f", spd_pid_Kp));
  MOOSDebugWrite(MOOSFormat("SPEED_PID_KD       = %.3f", spd_pid_Kd));
  MOOSDebugWrite(MOOSFormat("SPEED_PID_KI       = %.3f", spd_pid_Ki));
  MOOSDebugWrite(MOOSFormat("SPEED_PID_KI_LIMIT = %.3f", spd_pid_ilim));
  MOOSDebugWrite(MOOSFormat("MAXTHRUST          = %.3f", max_thrust));

  return(ok);
}

//--------------------------------------------------------------------
// Procedure: handleDepthSettings()

bool MarinePID::handleDepthSettings()
{
  int ok = true;

  string depth_control_str = "false";
  m_MissionReader.GetConfigurationParam("DEPTH_CONTROL", depth_control_str);
  depth_control_str = tolower(depth_control_str);
  m_depth_control = ((depth_control_str == "true") ||
		   (depth_control_str == "1") ||
		   (depth_control_str == "yes"));
  
  if(!m_depth_control)
    return(true);
  
  double dep_pid_Kp, dep_pid_Kd, dep_pid_Ki, dep_pid_ilim;
  if(!m_MissionReader.GetConfigurationParam("DEPTH_PID_KP", dep_pid_Kp)) {
    MOOSDebugWrite("DEPTH_PID_KP not found in Mission File");
    ok = false;
  }
  if(!m_MissionReader.GetConfigurationParam("DEPTH_PID_KD", dep_pid_Kd)) {
    MOOSDebugWrite("DEPTH_PID_KD not found in Mission File");
    ok = false;
  }
  if(!m_MissionReader.GetConfigurationParam("DEPTH_PID_KI", dep_pid_Ki)) {
    MOOSDebugWrite("DEPTH_PID_KI not found in Mission File");
    ok = false;
  }
  if(!m_MissionReader.GetConfigurationParam("DEPTH_PID_INTEGRAL_LIMIT", dep_pid_ilim)) {
    MOOSDebugWrite("DEPTH_PID_INTEGRAL_LIMIT not found in Mission File");
    ok = false;
  }
  
  double max_elevator = 100;
  m_MissionReader.GetConfigurationParam("MAXELEVATOR", max_elevator);


  ScalarPID depPID;
  depPID.SetGains(dep_pid_Kp, dep_pid_Kd, dep_pid_Ki);
  depPID.SetLimits(dep_pid_ilim, 100);
  m_pengine.setPID(2, depPID);

  MOOSDebugWrite(MOOSFormat("DEPTH_PID_KP       = %.3f",dep_pid_Kp));
  MOOSDebugWrite(MOOSFormat("DEPTH_PID_KD       = %.3f",dep_pid_Kd));
  MOOSDebugWrite(MOOSFormat("DEPTH_PID_KI       = %.3f",dep_pid_Ki));
  MOOSDebugWrite(MOOSFormat("DEPTH_PID_KI_LIMIT = %.3f",dep_pid_ilim));
  MOOSDebugWrite(MOOSFormat("MAXELEVATOR        = %.3f",max_elevator));

  return(ok);
}









