/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MarinePID.cpp                                        */
/*    DATE: Apr 10 2006                                          */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iterator>
#include <iostream>
#include <cmath>
#include <cstring>
#include "MarinePID.h"
#include "MBUtils.h"
#include "AngleUtils.h"

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
  m_current_pitch   = 0;

  m_rudder_bias_duration  = 10;
  m_rudder_bias_limit     = 0;
  m_rudder_bias_side      = 1;
  m_rudder_bias_timestamp = 0;

  m_max_thrust   = 100;
  m_max_rudder   = 100;
  m_max_pitch    = 15;
  m_max_elevator = 13;
  m_iteration    = 0;
  m_start_time   = 0;

  m_paused       = false;
  m_ok_skew      = 360;

  m_tardy_helm_thresh = 2.0;
  m_tardy_nav_thresh  = 2.0;

  m_time_of_last_helm_msg = 0;
  m_time_of_last_nav_msg  = 0;

  m_max_sat_hdg_debug = false;
  m_max_sat_spd_debug = false;
  m_max_sat_dep_debug = false;

  m_ignore_nav_yaw = false;
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
      else if(!m_ignore_nav_yaw && (key == "NAV_YAW"))
	m_current_heading = angle360(-MOOSRad2Deg(msg.m_dfVal));
      else if(key == "NAV_HEADING")
	m_current_heading = angle360(msg.m_dfVal);
      else if(key == "NAV_SPEED")
	m_current_speed = msg.m_dfVal;
      else if(key == "NAV_DEPTH")
	m_current_depth = msg.m_dfVal;
      else if(key == "NAV_PITCH")
	m_current_pitch = msg.m_dfVal;
      
      if(!strncmp(key.c_str(), "NAV_", 4))
	m_time_of_last_nav_msg = curr_time;

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
    Notify("DESIRED_THRUST", 0.0);
    m_current_thrust = 0;
    return(true);
  }
  
  if((current_time - m_time_of_last_nav_msg) > m_tardy_nav_thresh) {
    if(!m_paused)
      MOOSDebugWrite("Paused Due To Tardy NAV Input: THRUST=0");
    cout << "Paused Due To Tardy NAV Input: THRUST=0" << endl;
    m_paused = true;
    Notify("DESIRED_THRUST", 0.0);
    m_current_thrust = 0;
    return(true);
  }

  double rudder = 0;
  double thrust = 0;
  double elevator = 0;

  m_pengine.updateTime(current_time);

  rudder = m_pengine.getDesiredRudder(m_desired_heading, m_current_heading, 
				    m_max_rudder);
  
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
  

  thrust = m_pengine.getDesiredThrust(m_desired_speed, m_current_speed, 
				    m_current_thrust, m_max_thrust);


  if(m_depth_control)
    elevator = m_pengine.getDesiredElevator(m_desired_depth, m_current_depth,
					  m_current_pitch, m_max_pitch, 
					  m_max_elevator);
  
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
  Notify("DESIRED_RUDDER", rudder);
  Notify("DESIRED_THRUST", thrust);
  m_current_thrust = thrust;
  if(m_depth_control)
    Notify("DESIRED_ELEVATOR", elevator);

  m_allstop_posted = false;

  // Added April 2019 by mikerb
  // Simple quick check if max saturation event occurred
  // If so, and debug was turned on, report debug info for the event
  if(m_pengine.getMaxSatHdg()) {
    Notify("PID_MAX_SAT_HDG", "true");
    string debug_info = m_pengine.getMaxSatHdgStr();
    if(debug_info != "")
      Notify("PID_MAX_SAT_HDG_DEBUG", debug_info);
  }
  if(m_pengine.getMaxSatSpd()) {
    Notify("PID_MAX_SAT_SPD", "true");
    string debug_info = m_pengine.getMaxSatSpdStr();
    if(debug_info != "")
      Notify("PID_MAX_SAT_SPD_DEBUG", debug_info);
  }
  if(m_pengine.getMaxSatDep()) {
    Notify("PID_MAX_SAT_DEp", "true");
    string debug_info = m_pengine.getMaxSatDepStr();
    if(debug_info != "")
      Notify("PID_MAX_SAT_DEP_DEBUG", debug_info);
  }
  
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

  Notify("DESIRED_RUDDER", 0.0);
  Notify("DESIRED_THRUST", 0.0);
  if(m_depth_control)
    Notify("DESIRED_ELEVATOR", 0.0);

  m_allstop_posted = true;
}

//------------------------------------------------------------
// Procedure: OnConnectToServer()

bool MarinePID::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void MarinePID::registerVariables()
{
  Register("NAV_HEADING", 0);
  Register("NAV_SPEED", 0);
  Register("NAV_DEPTH", 0);
  Register("NAV_PITCH", 0);
  Register("DESIRED_HEADING", 0);
  Register("DESIRED_SPEED", 0);
  Register("DESIRED_DEPTH", 0);
  Register("PID_VERBOSE", 0);
  Register("SPEED_FACTOR", 0);
  Register("MOOS_MANUAL_OVERIDE", 0);
  Register("MOOS_MANUAL_OVERRIDE", 0);

  if(m_ignore_nav_yaw)
    UnRegister("NAV_YAW");
  else
    Register("NAV_YAW", 0);
}

//--------------------------------------------------------
// Procedure: onStartUp()

bool MarinePID::OnStartUp()
{
  cout << "pMarinePID starting...." << endl;
  
  m_start_time = MOOSTime();
  // m_speed_factor=0 means speed PID will be used.
  m_speed_factor = 0;
  
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  
  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string sLine = *p;
    string param = toupper(biteStringX(sLine, '='));
    string value = sLine;
    double dval  = atof(value.c_str());
    
    if(param == "SPEED_FACTOR")
      m_speed_factor = vclip(dval, 0, 100);
    else if(param == "SIM_INSTABILITY")
      m_rudder_bias_limit = vclip(dval, 0, 100);
    else if(param == "TARDY_HELM_THRESHOLD") 
      m_tardy_helm_thresh = vclip_min(dval, 0);
    else if(param == "TARDY_NAV_THRESHOLD")
      m_tardy_nav_thresh = vclip_min(dval, 0);
    else if(param == "ACTIVE_START") 
      setBooleanOnString(m_has_control, value);
    else if(param == "MAX_SAT_HDG_DEBUG") 
      setBooleanOnString(m_max_sat_hdg_debug, value);
    else if(param == "MAX_SAT_SPD_DEBUG") 
      setBooleanOnString(m_max_sat_spd_debug, value);
    else if(param == "MAX_SAT_DEP_DEBUG") 
      setBooleanOnString(m_max_sat_dep_debug, value);
    else if(param == "IGNORE_NAV_YAW") 
      setBooleanOnString(m_ignore_nav_yaw, value);
    //else if(param == "OK_SKEW") 
    //  handled = handleConfigSkewAny(value);
    else if(param == "VERBOSE") {
      if((sLine == "true") || (sLine == "verbose"))
	m_verbose = "verbose";
      if(sLine == "terse") 
	m_verbose = "terse";
      if(sLine == "quiet")
	m_verbose = "quiet";
    }
  }

  bool ok_yaw = handleYawSettings();
  bool ok_spd = handleSpeedSettings();
  bool ok_dep = handleDepthSettings();

  if(!ok_yaw || !ok_spd || !ok_dep) {
    cout << "Improper PID Settings" << endl;
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
  if(!m_MissionReader.GetConfigurationParam("MAXRUDDER",m_max_rudder)) {
    MOOSDebugWrite("MAXRUDDER not found in Mission File");
    ok = false;
  }
  
  ScalarPID crsPID;
  crsPID.SetGains(yaw_pid_Kp, yaw_pid_Kd, yaw_pid_Ki);
  crsPID.SetLimits(yaw_pid_ilim, 100);
  m_pengine.setPID(0, crsPID);
  if(m_max_sat_hdg_debug)
    m_pengine.setDebugHdg();
  
  MOOSDebugWrite(MOOSFormat("** NEW CONTROLLER GAINS ARE **"));
  MOOSDebugWrite(MOOSFormat("YAW_PID_KP             = %.3f",yaw_pid_Kp));
  MOOSDebugWrite(MOOSFormat("YAW_PID_KD             = %.3f",yaw_pid_Kd));
  MOOSDebugWrite(MOOSFormat("YAW_PID_KI             = %.3f",yaw_pid_Ki));
  MOOSDebugWrite(MOOSFormat("YAW_PID_INTEGRAL_LIMIT = %.3f",yaw_pid_ilim));
  MOOSDebugWrite(MOOSFormat("MAXRUDDER              = %.3f",m_max_rudder));
  
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
  if(!m_MissionReader.GetConfigurationParam("MAXTHRUST",m_max_thrust))
  {
    MOOSDebugWrite("MAXTHRUST not found in Mission File");
    ok = false;
  }

  ScalarPID spdPID;
  spdPID.SetGains(spd_pid_Kp, spd_pid_Kd, spd_pid_Ki);
  spdPID.SetLimits(spd_pid_ilim, 100);
  m_pengine.setPID(1, spdPID);
  if(m_max_sat_spd_debug)
    m_pengine.setDebugSpd();
  
  MOOSDebugWrite(MOOSFormat("SPEED_PID_KP           = %.3f",spd_pid_Kp));
  MOOSDebugWrite(MOOSFormat("SPEED_PID_KD           = %.3f",spd_pid_Kd));
  MOOSDebugWrite(MOOSFormat("SPEED_PID_KI           = %.3f",spd_pid_Ki));
  MOOSDebugWrite(MOOSFormat("SPEED_PID_INTEGRAL_LIMIT = %.3f",spd_pid_ilim));
  MOOSDebugWrite(MOOSFormat("MAXTHRUST              = %.3f",m_max_thrust));

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
  m_depth_control = ((depth_control_str == "true") ||
		   (depth_control_str == "1") ||
		   (depth_control_str == "yes"));
#endif

#if 0
  m_MissionReader.GetConfigurationParam("DEPTH_CONTROL", m_depth_control);
#endif
#if 0
  double dc;
  m_MissionReader.GetConfigurationParam("DEPTH_CONTROL", dc);
  m_depth_control = (int)dc;
#endif
  
  if(!m_depth_control)
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
  if(!m_MissionReader.GetConfigurationParam("MAXPITCH",m_max_pitch))
  {
    MOOSDebugWrite("MAXPITCH not found in Mission File");
    ok = false;
  }
  // Convert pitch limit to radians
  m_max_pitch=MOOSDeg2Rad(m_max_pitch);

  ScalarPID ztopPID;
  ztopPID.SetGains(z_top_pid_Kp, z_top_pid_Kd, z_top_pid_Ki);
  ztopPID.SetLimits(z_top_pid_ilim, 100);
  m_pengine.setPID(2, ztopPID);
  
  MOOSDebugWrite(MOOSFormat("Z_TO_PITCH_PID_KP      = %.3f",z_top_pid_Kp));
  MOOSDebugWrite(MOOSFormat("Z_TO_PITCH_PID_KD      = %.3f",z_top_pid_Kd));
  MOOSDebugWrite(MOOSFormat("Z_TO_PITCH_PID_KI      = %.3f",z_top_pid_Ki));
  MOOSDebugWrite(MOOSFormat("Z_TO_PITCH_PID_KI_LIMIT= %.3f",z_top_pid_ilim));
  MOOSDebugWrite(MOOSFormat("MAXELEVATOR            = %.3f",m_max_elevator));
  
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
  if(!m_MissionReader.GetConfigurationParam("MAXELEVATOR",m_max_elevator))
  {
    MOOSDebugWrite("MAXELEVATOR not found in Mission File");
    ok = false;
  }

  ScalarPID pitchPID;
  pitchPID.SetGains(pitch_pid_Kp, pitch_pid_Kd, pitch_pid_Ki);
  pitchPID.SetLimits(pitch_pid_ilim, 100);
  m_pengine.setPID(3, pitchPID);
  if(m_max_sat_dep_debug)
    m_pengine.setDebugDep();

  MOOSDebugWrite(MOOSFormat("PITCH_PID_KP           = %.3f",pitch_pid_Kp));
  MOOSDebugWrite(MOOSFormat("PITCH_PID_KD           = %.3f",pitch_pid_Kd));
  MOOSDebugWrite(MOOSFormat("PITCH_PID_KI           = %.3f",pitch_pid_Ki));
  MOOSDebugWrite(MOOSFormat("PITCH_PID_KI_LIMIT     = %.3f",pitch_pid_ilim));
  MOOSDebugWrite(MOOSFormat("MAXPITCH               = %.3f",m_max_pitch));

  return(ok);
}









