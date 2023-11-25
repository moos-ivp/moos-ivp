/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PIDEngine.cpp                                        */
/*    DATE: Jul 31st, 2005 Sunday in Montreal                    */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include <iostream>
#include "PIDEngine.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "MOOS/libMOOS/MOOSLib.h"

using namespace std;

//-----------------------------------------------------------
// Constructor()

PIDEngine::PIDEngine()
{
  // If speed_factor is zero, speed is controlled via PID. 
  // If not, thrust is set to multiple of desired speed.
  m_speed_factor = 0;
  m_curr_time    = 0;
  m_curr_thrust  = 0;
  m_start_time   = 0;
  m_iterations   = 0;

  m_curr_heading = 0;
  m_curr_speed   = 0;
  m_curr_depth   = 0;
  m_curr_pitch   = 0;
  m_curr_heading_tstamp = 0;
  m_curr_speed_tstamp   = 0;
  m_curr_depth_tstamp   = 0;
  m_curr_pitch_tstamp   = 0;
  
  m_desired_hdg = 0;
  m_desired_spd = 0;
  m_desired_dep = 0;
  m_desired_hdg_tstamp = 0;
  m_desired_spd_tstamp = 0;
  m_desired_dep_tstamp = 0;

  
  m_desired_rudder   = 0;
  m_desired_thrust   = 0;
  m_desired_elevator = 0;
  
  m_max_rudder   = 100;
  m_max_thrust   = 100;
  m_max_pitch    = 15;
  m_max_elevator = 13;
  
  m_rudder_bias_duration  = 10;
  m_rudder_bias_limit     = 0;
  m_rudder_bias_side      = 1;
  m_rudder_bias_timestamp = 0;

  m_depth_control = true;
  m_pid_override = true;
  m_pid_stale = true;
  
  m_tardy_helm_thresh = 2.0;
  m_tardy_nav_thresh  = 2.0;

  m_debug_hdg = false;
  m_debug_spd = false;
  m_debug_dep = false;

  m_simulation = false;
}

//------------------------------------------------------------
// Procedure: setConfigParams()

list<string> PIDEngine::setConfigParams(list<string> param_lines)
{
  list<string> unhandled_params;
  list<string>::iterator p;
  for(p=param_lines.begin(); p!=param_lines.end(); p++) {
    string orig  = *p;
    string line  = tolower(orig);
    string param = biteStringX(line, '=');
    if(param == "speed_factor")
      m_config_params.push_front(orig);
    else if(param == "simulation")
      m_config_params.push_front(orig);
    else if(param == "sim_instability")
      m_config_params.push_front(orig);
    else if(param == "tardy_helm_threshold")
      m_config_params.push_front(orig);
    else if(param == "tardy_nav_threshold")
      m_config_params.push_front(orig);
    else if(param == "yaw_pid_kp")
      m_config_params.push_front(orig);
    else if(param == "yaw_pid_kd")
      m_config_params.push_front(orig);
    else if(param == "yaw_pid_ki")
      m_config_params.push_front(orig);
    else if(param == "yaw_pid_integral_limit")
      m_config_params.push_front(orig);
    else if(param == "yaw_pid_ki_limit")
      m_config_params.push_front(orig);
    else if(param == "maxrudder")
      m_config_params.push_front(orig);
    else if(param == "heading_debug")
      m_config_params.push_front(orig);

    else if(param == "speed_pid_kp")
      m_config_params.push_front(orig);
    else if(param == "speed_pid_kd")
      m_config_params.push_front(orig);
    else if(param == "speed_pid_ki")
      m_config_params.push_front(orig);
    else if(param == "speed_pid_integral_limit")
      m_config_params.push_front(orig);
    else if(param == "maxthrust")
      m_config_params.push_front(orig);
    else if(param == "speed_debug")
      m_config_params.push_front(orig);

    else if(param == "depth_control")
      m_config_params.push_front(orig);
    else if(param == "z_to_pitch_pid_kp")
      m_config_params.push_front(orig);
    else if(param == "z_to_pitch_pid_kd")
      m_config_params.push_front(orig);
    else if(param == "z_to_pitch_pid_ki")
      m_config_params.push_front(orig);
    else if(param == "z_to_pitch_pid_integral_limit")
      m_config_params.push_front(orig);
    else if(param == "maxpitch")
      m_config_params.push_front(orig);
    else if(param == "depth_debug")
      m_config_params.push_front(orig);

    else if(param == "pitch_pid_kp")
      m_config_params.push_front(orig);
    else if(param == "pitch_pid_kd")
      m_config_params.push_front(orig);
    else if(param == "pitch_pid_ki")
      m_config_params.push_front(orig);
    else if(param == "pitch_pid_integral_limit")
      m_config_params.push_front(orig);
    else if(param == "maxelevator")
      m_config_params.push_front(orig);

    else
      unhandled_params.push_front(orig);    
  }

  return(unhandled_params);
}

  
//------------------------------------------------------------
// Procedure: setDesHeading()

void PIDEngine::setDesHeading(double dval)
{
  m_desired_hdg = dval;
  m_desired_hdg_tstamp = m_curr_time;
}

//------------------------------------------------------------
// Procedure: setDesSpeed()

void PIDEngine::setDesSpeed(double dval)
{
  m_desired_spd = dval;
  m_desired_spd_tstamp = m_curr_time;
}

//------------------------------------------------------------
// Procedure: setDesDepth()

void PIDEngine::setDesDepth(double dval)
{
  m_desired_dep = dval;
  m_desired_dep_tstamp = m_curr_time;
}

//------------------------------------------------------------
// Procedure: setCurrHeading()

void PIDEngine::setCurrHeading(double dval)
{
  m_curr_heading = dval;
  m_curr_heading_tstamp = m_curr_time;
}

//------------------------------------------------------------
// Procedure: setCurrSpeed()

void PIDEngine::setCurrSpeed(double dval)
{
  m_curr_speed = dval;
  m_curr_speed_tstamp = m_curr_time;
}

//------------------------------------------------------------
// Procedure: setCurrDepth()

void PIDEngine::setCurrDepth(double dval)
{
  m_curr_depth = dval;
  m_curr_depth_tstamp = m_curr_time;
}

//------------------------------------------------------------
// Procedure: setCurrPitch()

void PIDEngine::setCurrPitch(double dval)
{
  m_curr_pitch = dval;
  m_curr_pitch_tstamp = m_curr_time;
}

//------------------------------------------------------------
// Procedure: setPIDOverride()

void PIDEngine::setPIDOverride(string sval)
{
  if(tolower(sval) == "true") {
    if(m_pid_override == false) 
      addPosting("PID_HAS_CONTROL", "false");    
    m_pid_override = true;
  }
      
  else if(tolower(sval) == "false") {

    // Upon lifting an override, the timestamps are reset. New values
    // for all will need to be received before desired_* outputs will
    // be produced.  If we do not reset, it's possible we may
    // interpret older pubs as being stale but they may have been
    // paused also during an override.
    if(m_pid_override == true) {
      m_curr_heading_tstamp = 0;
      m_curr_speed_tstamp   = 0;
      m_curr_depth_tstamp   = 0;
      m_curr_pitch_tstamp   = 0;
      m_desired_hdg_tstamp = 0;
      m_desired_spd_tstamp = 0;
      m_desired_dep_tstamp = 0;
    }
    m_pid_override = false;
    addPosting("PID_HAS_CONTROL", "true");
  }

}

//------------------------------------------------------------
// Procedure: setDesiredValues()

void PIDEngine::setDesiredValues()
{
  m_iterations++;

  m_desired_thrust = 0;
  m_desired_rudder = 0;
  m_desired_elevator = 0;

  //=============================================================
  // Part 1: Ensure all nav and helm messages have been received 
  // for first time. If not, we simply wait, without declaring an
  // override. After all messages have been received at least
  // once, staleness is checked below.
  //=============================================================
  if((m_desired_hdg_tstamp == 0) || (m_desired_spd_tstamp == 0))
    return;
  if((m_curr_heading_tstamp == 0) || (m_curr_speed_tstamp == 0))
    return;
  if(m_depth_control) {
    if(m_desired_dep_tstamp == 0)
      return;
    if((m_curr_depth_tstamp == 0) || (m_curr_pitch_tstamp == 0))
      return;
  }
  
  //=============================================================
  // Part 2: Critical info staleness (if not in simulation mode)
  //=============================================================
  if(!m_simulation) {
    //bool is_stale = checkForStaleness();
    checkForStaleness();
    if(m_pid_stale) {
      m_pid_override = true;
      return;
    }
  }
  
  //=============================================================
  // Part 3: Set the actuator values. Note: If desired thrust is 
  // zero others desired values will automatically be zero too.
  //=============================================================
  m_desired_thrust = setDesiredThrust();
  
  if(m_desired_thrust > 0)
    m_desired_rudder = setDesiredRudder();
  
  if(m_desired_thrust > 0 && m_depth_control) 
    m_desired_elevator = setDesiredElevator();
}

//------------------------------------------------------------
// Procedure: setDesiredValues()

void PIDEngine::checkForStaleness()
{
  bool is_stale = false;

  // =========================================================
  // Part 1: Check for Helm staleness
  // =========================================================
  double hdg_delta = (m_curr_time - m_desired_hdg_tstamp);
  if(hdg_delta > m_tardy_helm_thresh) {
    string staleness = doubleToStringX(hdg_delta,3);
    addPosting("PID_STALE", "Stale DesHdg:" + staleness);
    is_stale = true;
  }
  double spd_delta = (m_curr_time - m_desired_spd_tstamp);
  if(spd_delta > m_tardy_helm_thresh) {
    string staleness = doubleToStringX(spd_delta,3);
    addPosting("PID_STALE", "Stale DesSpd:" + staleness);
    is_stale = true;
  }

  // =========================================================
  // Part 2B: Check for Nav staleness
  // =========================================================
  double nav_hdg_delta = (m_curr_time - m_curr_heading_tstamp);
  if(nav_hdg_delta > m_tardy_nav_thresh) {
    string staleness = doubleToStringX(nav_hdg_delta,3);
    addPosting("PID_STALE", "Stale NavHdg:" + staleness);
    is_stale = true;
  }
  double nav_spd_delta = (m_curr_time - m_curr_speed_tstamp);
  if(nav_spd_delta > m_tardy_nav_thresh) {
    string staleness = doubleToStringX(nav_spd_delta,3);
    addPosting("PID_STALE", "Stale NavSpd:" + staleness);
    is_stale = true;
  }

  // =========================================================
  // Part 2C: If depth control, check for Helm Depth staleness 
  // =========================================================
  if(m_depth_control) {
    double dep_delta = (m_curr_time - m_desired_dep_tstamp);
    if(dep_delta > m_tardy_helm_thresh) {
      string staleness = doubleToStringX(dep_delta,3);
      addPosting("PID_STALE", "Stale DesDepth:" + staleness);
      is_stale = true;
    }
  }
  
  // =========================================================
  // Part 2D: If depth control, check for Nav Depth staleness 
  // =========================================================
  if(m_depth_control) {    
    double nav_dep_delta = (m_curr_time - m_curr_depth_tstamp);
    if(nav_dep_delta > m_tardy_nav_thresh) {
      string staleness = doubleToStringX(nav_dep_delta,3);
      addPosting("PID_STALE", "Stale NavDep:" + staleness);
      is_stale = true;
    }
    double nav_pit_delta = (m_curr_time - m_curr_pitch_tstamp);
    if(nav_pit_delta > m_tardy_nav_thresh) {
      string staleness = doubleToStringX(nav_pit_delta,3);
      addPosting("PID_STALE", "Stale NavPitch:" + staleness);
      is_stale = true;
    }
  }

  m_pid_stale = is_stale;
  //return(is_stale);
}
  

//------------------------------------------------------------
// Procedure: setDesiredRudder()
//      Note: Rudder angles are processed in degrees

double PIDEngine::setDesiredRudder()
{
  m_desired_hdg = angle180(m_desired_hdg);

  double heading_error = m_curr_heading - m_desired_hdg;
  heading_error = angle180(heading_error);
  double desired_rudder = 0;
  m_heading_pid.Run(heading_error, m_curr_time, desired_rudder);
  desired_rudder *= -1.0;

  //--------------------
  if(m_rudder_bias_duration > 0) {
    double rbias_duration = m_curr_time - m_rudder_bias_timestamp;
    if(rbias_duration > m_rudder_bias_duration) {
      rbias_duration = 0;
      m_rudder_bias_timestamp = m_curr_time;
      m_rudder_bias_side *= -1;
    }
    double pct = rbias_duration / m_rudder_bias_duration;
    double bias = m_rudder_bias_side * (pct * m_rudder_bias_limit);
    desired_rudder += bias;
  }
  //--------------------

  // Enforce limit on desired rudder
  MOOSAbsLimit(desired_rudder, m_max_rudder);
  if(desired_rudder > m_max_rudder)
    desired_rudder = m_max_rudder;
  if(desired_rudder < -m_max_rudder)
    desired_rudder = -m_max_rudder;

  // Added 4/19 mikerb: monitor and report max saturation events
  if(m_debug_hdg) {
    bool max_sat_hdg = m_heading_pid.getMaxSat();
    if(max_sat_hdg)
      addPosting("PID_MAX_SAT_HDG_DEBUG", m_heading_pid.getDebugStr());
  
    string rpt = "PID_COURSE: ";
    rpt += " (Want):" + doubleToString(m_desired_hdg);
    rpt += " (Curr):" + doubleToString(m_curr_heading);
    rpt += " (Diff):" + doubleToString(heading_error);
    rpt += " RUDDER:" + doubleToString(desired_rudder);
    addPosting("PID_HDG_DEBUG", rpt);
  }
  
  return(desired_rudder);
}

//------------------------------------------------------------
// Procedure: setDesiredThrust()

double PIDEngine::setDesiredThrust()
{
  double speed_error  = m_desired_spd - m_curr_speed;
  double delta_thrust = 0;
  double desired_thrust = m_curr_thrust;

  // If NOT using PID control, just apply multiple to des_speed
  if(m_speed_factor != 0) {
    desired_thrust = m_desired_spd * m_speed_factor;
  }
  // ELSE apply the PID contoller to the problem.
  else {
    m_speed_pid.Run(speed_error,  m_curr_time, delta_thrust);
    desired_thrust += delta_thrust;
  }
  
  if(desired_thrust < 0.01)
    desired_thrust = 0;

  // Enforce limit on desired thrust
  if(desired_thrust > m_max_thrust)
    desired_thrust = m_max_thrust;
  else if(desired_thrust < -m_max_thrust)
    desired_thrust = -m_max_thrust;

  // We assume the desired thrust will be the current thrust on the
  // next iteration.
  m_curr_thrust = desired_thrust;

  // Added 4/19 mikerb: monitor and report max saturation events
  if(m_debug_spd) {
    bool max_sat_spd = m_speed_pid.getMaxSat();
    if(max_sat_spd)
      addPosting("PID_MAX_SAT_SPD_DEBUG", m_heading_pid.getDebugStr());

    if(m_speed_factor != 0) {
      string rpt = "PID_SPEED: ";
      rpt += " (Want):" + doubleToString(m_desired_spd);
      rpt += " (Curr):" + doubleToString(m_curr_speed);
      rpt += " (Diff):" + doubleToString(speed_error);
      rpt += " (Fctr):" + doubleToString(m_speed_factor);
      rpt += " THRUST:" + doubleToString(desired_thrust);
      addPosting("PID_SPD_DEBUG", rpt);
    }    
    else {
      string rpt = "PID_SPEED: ";
      rpt += " (Want):" + doubleToString(m_desired_spd);
      rpt += " (Curr):" + doubleToString(m_curr_speed);
      rpt += " (Diff):" + doubleToString(speed_error);
      rpt += " (Delt):" + doubleToString(delta_thrust);
      rpt += " THRUST:" + doubleToString(desired_thrust);
      addPosting("PID_SPD_DEBUG", rpt);
    }
  }

  return(desired_thrust);
}

//------------------------------------------------------------
// Procedure: setDesiredElevator()
// Elevator angles and pitch are processed in radians

double PIDEngine::setDesiredElevator()
{
  double desired_elevator = 0;
  double desired_pitch = 0;
  double depth_error = m_curr_depth - m_desired_dep;
  m_z_to_pitch_pid.Run(depth_error, m_curr_time, desired_pitch);

  // Enforce limits on desired pitch
  if(desired_pitch > m_max_pitch)
    desired_pitch = m_max_pitch;
  else if(desired_pitch < -m_max_pitch)
    desired_pitch = -m_max_pitch;
  
  double pitch_error = m_curr_pitch - desired_pitch;
  m_pitch_pid.Run(pitch_error, m_curr_time, desired_elevator);

  // Convert desired elevator to degrees
  desired_elevator=MOOSRad2Deg(desired_elevator);

  // Enforce elevator limit
  if(desired_elevator > m_max_elevator)
    desired_elevator = m_max_elevator;
  else if(desired_elevator < -m_max_elevator)
    desired_elevator = -m_max_elevator;
  
  if(m_debug_dep) {
    if(m_z_to_pitch_pid.getMaxSat()) {
      string debug_str = "Z:" + m_z_to_pitch_pid.getDebugStr();
      addPosting("PID_MAX_SAT_DEP_DEBUG", debug_str);
    }
    if(m_pitch_pid.getMaxSat()) {
      string debug_str = "P:" + m_z_to_pitch_pid.getDebugStr();
      addPosting("PID_MAX_SAT_DEP_DEBUG", debug_str);
    }

    string rpt = "PID_DEPTH: ";
    rpt += " (Want):" + doubleToString(m_desired_dep);
    rpt += " (Curr):" + doubleToString(m_curr_depth);
    rpt += " (Diff):" + doubleToString(depth_error);
    rpt += " ELEVATOR:" + doubleToString(desired_elevator);
    addPosting("PID_DEP_DEBUG", rpt);
  }


  return(desired_elevator);
}

//--------------------------------------------------------------------
// Procedure: handleYawSettings()

bool PIDEngine::handleYawSettings()
{
  double yaw_pid_kp = 0, yaw_pid_kd = 0, yaw_pid_ki = 0, yaw_pid_ilim = 0;
  
  bool yaw_pid_kp_found = false,  yaw_pid_kd_found   = false;
  bool yaw_pid_ki_found = false,  yaw_pid_ilim_found = false;
  bool max_rudder_found = false;

  list<string>::iterator p;
  for(p=m_config_params.begin(); p!=m_config_params.end(); p++) {
    string line = *p;
    string param = tolower(biteStringX(line, '='));
    string sval = line;
    double dval = atof(sval.c_str());
    
    if(param == "simulation")
      setBooleanOnString(m_simulation, sval);
    if(param == "sim_instability")
      m_rudder_bias_limit = vclip(dval, 0, 100);
    else if(param == "tardy_helm_threshold") 
      m_tardy_helm_thresh = vclip_min(dval, 0);
    else if(param == "tardy_nav_threshold")
      m_tardy_nav_thresh = vclip_min(dval, 0);

    else if(param == "yaw_pid_kp")
      yaw_pid_kp_found = setDoubleOnString(yaw_pid_kp, sval);
    else if(param == "yaw_pid_kd")
      yaw_pid_kd_found = setDoubleOnString(yaw_pid_kd, sval);
    else if(param == "yaw_pid_ki")
      yaw_pid_ki_found = setDoubleOnString(yaw_pid_ki, sval);
    else if(param == "yaw_pid_integral_limit")
      yaw_pid_ilim_found = setDoubleOnString(yaw_pid_ilim, sval);
    else if(param == "yaw_pid_ki_limit")
      yaw_pid_ilim_found = setDoubleOnString(yaw_pid_ilim, sval);
    else if(param == "maxrudder")
      max_rudder_found = setDoubleOnString(m_max_rudder, sval);
    else if((param == "heading_debug") && (tolower(sval)=="true")) {
      m_debug_hdg = true;
      m_heading_pid.setDebug(true);
    }
  }
  
  if(!yaw_pid_kp_found) 
    m_config_errors.push_back("YAW_PID_KP not found in Mission File");
  if(!yaw_pid_kd_found) 
    m_config_errors.push_back("YAW_PID_KD not found in Mission File");
  if(!yaw_pid_ki_found) 
    m_config_errors.push_back("YAW_PID_KI not found in Mission File");
  if(!yaw_pid_ilim_found) 
    m_config_errors.push_back("YAW_PID_INTEGRAL_LIMIT not found in Mission File");
  if(!max_rudder_found)
    m_config_errors.push_back("MAXRUDDER not found in Mission File");

  
  m_heading_pid.SetGains(yaw_pid_kp, yaw_pid_kd, yaw_pid_ki);
  m_heading_pid.SetLimits(yaw_pid_ilim, m_max_rudder);

  m_config_info.push_back("** NEW CONTROLLER GAINS ARE **");
  m_config_info.push_back("YAW_PID_KP     " + doubleToStringX(yaw_pid_kp,3));
  m_config_info.push_back("YAW_PID_KD     " + doubleToStringX(yaw_pid_kd,3));
  m_config_info.push_back("YAW_PID_KI     " + doubleToStringX(yaw_pid_ki,3));
  m_config_info.push_back("YAW_PID_KI_LIM " + doubleToStringX(yaw_pid_ilim,3));
  m_config_info.push_back("MAXRUDDER      " + doubleToStringX(m_max_rudder,3));
  
  return(true);
}
  

//--------------------------------------------------------------------
// Procedure: handleSpeedSettings()

bool PIDEngine::handleSpeedSettings()
{
  double spd_pid_kp   = 0, spd_pid_kd = 0,    spd_pid_ki= 0;
  double spd_pid_ilim = 0;

  bool spd_pid_kp_found = false, spd_pid_kd_found   = false;
  bool spd_pid_ki_found = false, spd_pid_ilim_found = false;
  bool max_thrust_found = false;
  
  list<string>::iterator p;
  for(p=m_config_params.begin(); p!=m_config_params.end(); p++) {
    string line = *p;
    string param = tolower(biteStringX(line, '='));
    string sval = line;
    
    if(param == "speed_pid_kp")
      spd_pid_kp_found = setDoubleOnString(spd_pid_kp, sval);
    else if(param == "speed_pid_kd")
      spd_pid_kd_found = setDoubleOnString(spd_pid_kd, sval);
    else if(param == "speed_pid_ki")
      spd_pid_ki_found = setDoubleOnString(spd_pid_ki, sval);
    else if(param == "speed_pid_integral_limit")
      spd_pid_ilim_found = setDoubleOnString(spd_pid_ilim, sval);
    else if(param == "maxthrust")
      max_thrust_found = setDoubleOnString(m_max_thrust, sval);
    else if(param == "speed_factor")
      setNonNegDoubleOnString(m_speed_factor, sval);
    else if((param == "speed_debug") && (tolower(sval)=="true")) {
      m_debug_spd = true;
      m_speed_pid.setDebug(true);
    }
  }

  if(!spd_pid_kp_found) 
    m_config_errors.push_back("SPEED_PID_KP not in Mission File");
  if(!spd_pid_kd_found) 
    m_config_errors.push_back("SPEED_PID_KD not in Mission File");
  if(!spd_pid_ki_found) 
    m_config_errors.push_back("SPEED_PID_KI not in Mission File");
  if(!spd_pid_ilim_found) 
    m_config_errors.push_back("SPEED_PID_INTEGRAL_LIMIT not in Mission File");
  if(!max_thrust_found)
    m_config_errors.push_back("MAXTHRUST not found in Mission File");

  m_speed_pid.SetGains(spd_pid_kp, spd_pid_kd, spd_pid_ki);
  m_speed_pid.SetLimits(spd_pid_ilim, m_max_thrust);
  
  m_config_info.push_back("SPEED_PID_KP   = " + doubleToStringX(spd_pid_kp,3));
  m_config_info.push_back("SPEED_PID_KD   = " + doubleToStringX(spd_pid_kd,3));
  m_config_info.push_back("SPEED_PID_KI   = " + doubleToStringX(spd_pid_ki,3));
  m_config_info.push_back("SPEED_PID_KI_LIM = " + doubleToStringX(spd_pid_ilim,3));
  m_config_info.push_back("MAXTHRUST      = " + doubleToStringX(m_max_thrust,3)); 

  return(true);
}


//--------------------------------------------------------------------
// Procedure: handleDepthSettings()

bool PIDEngine::handleDepthSettings()
{
  double z_top_pid_kp   = 0, z_top_pid_kd =0, z_top_pid_ki = 0;
  double z_top_pid_ilim = 0;

  bool z_top_pid_kp_found = 0,     z_top_pid_kd_found   = 0;
  bool z_top_pid_ki_found = 0,     z_top_pid_ilim_found = 0;
  bool max_pitch_found    = false;

  list<string>::iterator p;
  for(p=m_config_params.begin(); p!=m_config_params.end(); p++) {
    string line = *p;
    string param = tolower(biteStringX(line, '='));
    string sval = line;
    
    if(param == "depth_control") {
      bool ok = setBooleanOnString(m_depth_control, sval); 
      if(!ok) {
	m_config_errors.push_back("depth_control has improper value");
	return(false);
      }
    }
    else if(param == "z_to_pitch_pid_kp")
      z_top_pid_kp_found = setDoubleOnString(z_top_pid_kp, sval);
    else if(param == "z_to_pitch_pid_kd")
      z_top_pid_kd_found = setDoubleOnString(z_top_pid_kd, sval);
    else if(param == "z_to_pitch_pid_ki")
      z_top_pid_ki_found = setDoubleOnString(z_top_pid_ki, sval);
    else if(param == "z_to_pitch_pid_integral_limit")
      z_top_pid_ilim_found = setDoubleOnString(z_top_pid_ilim, sval);
    else if(param == "maxpitch")
      max_pitch_found = setDoubleOnString(m_max_pitch, sval);  
    else if((param == "depth_debug") && (tolower(sval)=="true")) {
      m_debug_dep = true;
      m_z_to_pitch_pid.setDebug(true);
      m_pitch_pid.setDebug(true);
    }
  }
  if(!m_depth_control)
    return(true);

  if(max_pitch_found)
    m_max_pitch = degToRadians(m_max_pitch);

  if(!z_top_pid_kp_found) 
    m_config_errors.push_back("Z_TO_PITCH_PID_KP not in Mission File");
  if(!z_top_pid_kd_found) 
    m_config_errors.push_back("Z_TO_PITCH_PID_KD not in Mission File");
  if(!z_top_pid_ki_found) 
    m_config_errors.push_back("Z_TO_PITCH_PID_KI not in Mission File");
  if(!z_top_pid_ilim_found) 
    m_config_errors.push_back("Z_TO_PITCH_PID_INTEGRAL_LIMIT not in Mission File");
  if(!max_pitch_found)
    m_config_errors.push_back("MAXTPITCH not found in Mission File");

  m_z_to_pitch_pid.SetGains(z_top_pid_kp, z_top_pid_kd, z_top_pid_ki);
  m_z_to_pitch_pid.SetLimits(z_top_pid_ilim, m_max_pitch);

  m_config_info.push_back("Z_TO_PITCH_PID_KP   = " + doubleToStringX(z_top_pid_kp,3));
  m_config_info.push_back("Z_TO_PITCH_PID_KD   = " + doubleToStringX(z_top_pid_kd,3));
  m_config_info.push_back("Z_TO_PITCH_PID_KI   = " + doubleToStringX(z_top_pid_ki,3));
  m_config_info.push_back("Z_TO_PITCH_PID_KI_LIM = " + doubleToStringX(z_top_pid_ilim,3));
  m_config_info.push_back("MAXPITCH            = " + doubleToStringX(m_max_pitch,3)); 


  //----------------------------------------------------------------


  double pitch_pid_kp   = 0, pitch_pid_kd = 0,   pitch_pid_ki = 0;
  double pitch_pid_ilim = 0;

  bool pitch_pid_kp_found   = 0, pitch_pid_kd_found = 0,   pitch_pid_ki_found = 0;
  bool pitch_pid_ilim_found = 0, max_elevator_found = false;

  list<string>::iterator q;
  for(q=m_config_params.begin(); q!=m_config_params.end(); q++) {
    string line = *q;
    string param = tolower(biteStringX(line, '='));
    string sval = line;
    
    if(param == "pitch_pid_kp")
      pitch_pid_kp_found = setDoubleOnString(pitch_pid_kp, sval);
    else if(param == "pitch_pid_kd")
      pitch_pid_kd_found = setDoubleOnString(pitch_pid_kd, sval);
    else if(param == "pitch_pid_ki")
      pitch_pid_ki_found = setDoubleOnString(pitch_pid_ki, sval);
    else if(param == "pitch_pid_integral_limit")
      pitch_pid_ilim_found = setDoubleOnString(pitch_pid_ilim, sval);
    else if(param == "maxelevator")
      max_elevator_found = setDoubleOnString(m_max_elevator, sval);  
  }

  if(!pitch_pid_kp_found) 
    m_config_errors.push_back("PITCH_PID_KP not in Mission File");
  if(!pitch_pid_kd_found) 
    m_config_errors.push_back("PITCH_PID_KD not in Mission File");
  if(!pitch_pid_ki_found) 
    m_config_errors.push_back("PITCH_PID_KI not in Mission File");
  if(!pitch_pid_ilim_found) 
    m_config_errors.push_back("PITCH_PID_INTEGRAL_LIMIT not in Mission File");
  if(!max_elevator_found)
    m_config_errors.push_back("MAXELEVATOR not found in Mission File");

  m_pitch_pid.SetGains(pitch_pid_kp, pitch_pid_kd, pitch_pid_ki);
  m_pitch_pid.SetLimits(pitch_pid_ilim, m_max_elevator);

  m_config_info.push_back("PITCH_PID_KP   = " + doubleToStringX(pitch_pid_kp,3));
  m_config_info.push_back("PITCH_PID_KD   = " + doubleToStringX(pitch_pid_kd,3));
  m_config_info.push_back("PITCH_PID_KI   = " + doubleToStringX(pitch_pid_ki,3));
  m_config_info.push_back("PITCH_PID_KI_LIM = " + doubleToStringX(pitch_pid_ilim,3));
  m_config_info.push_back("MAXELEVATOR    = " + doubleToStringX(m_max_elevator,3)); 

  return(true);
}


//--------------------------------------------------------------------
// Procedure: hasConfigSettingsForPID()
//   Purpose: A quick heuristic check to see if the set of config params
//            has pid related params. Intended to be used by apps where
//            the PID engine is a secondary optional embedded capability 

bool PIDEngine::hasConfigSettingsForPID() const
{
  if(m_config_params.size() == 0)
    return(false);

  list<string>::const_iterator p;
  for(p=m_config_params.begin(); p!=m_config_params.end(); p++) {
    string line = *p;
    string param = tolower(biteStringX(line, '='));
    
    if(param == "yaw_pid_kp")
      return(true);
    else if(param == "yaw_pid_kd")
      return(true);
    else if(param == "yaw_pid_ki")
      return(true);
    else if(param == "yaw_pid_integral_limit")
      return(true);
    else if(param == "yaw_pid_ki_limit")
      return(true);
  }
  
  return(false);
}

//--------------------------------------------------------------------
// Procedure: addPosting()

void PIDEngine::addPosting(string var, string sval)
{
  VarDataPair pair(var, sval);
  m_postings.push_back(pair);
}


//--------------------------------------------------------------------
// Procedure: addPosting()

void PIDEngine::addPosting(string var, double dval)
{
  VarDataPair pair(var, dval);
  m_postings.push_back(pair);
}

//--------------------------------------------------------------------
// Procedure: getFrequency()

double PIDEngine::getFrequency() const
{
  double elapsed = m_curr_time - m_start_time;

  double frequency = 0;
  if(elapsed > 0) 
    frequency = ((double)(m_iterations)) / elapsed;

  return(frequency);
}


//--------------------------------------------------------------------
// Procedure: getConfigSummaryAll()

vector<string> PIDEngine::getConfigSummaryAll() const
{
  vector<string> svec = getConfigSummary("yaw");

  svec = mergeVectors(svec, getConfigSummary("speed"));
  svec = mergeVectors(svec, getConfigSummary("pitch"));
  svec = mergeVectors(svec, getConfigSummary("zpitch"));
  
  return(svec);
}

//--------------------------------------------------------------------
// Procedure: getConfigSummary()

vector<string> PIDEngine::getConfigSummary(string ptype) const
{
  vector<string> svec;
  
  if(ptype == "yaw") {
    string str_yaw_kp = doubleToStringX(m_heading_pid.getKP(),3);
    string str_yaw_ki = doubleToStringX(m_heading_pid.getKI(),3);
    string str_yaw_kd = doubleToStringX(m_heading_pid.getKD(),3);
    string str_yaw_il = doubleToStringX(m_heading_pid.getIL(),3);
    svec.push_back("YAW_PID_KP             = " + str_yaw_kp);
    svec.push_back("YAW_PID_KD             = " + str_yaw_kd);
    svec.push_back("YAW_PID_KI             = " + str_yaw_ki);
    svec.push_back("YAW_PID_INTEGRAL_LIMIT = " + str_yaw_il);
  }
  else if(ptype == "speed") {
    string str_spd_kp = doubleToStringX(m_speed_pid.getKP(),3);
    string str_spd_ki = doubleToStringX(m_speed_pid.getKI(),3);
    string str_spd_kd = doubleToStringX(m_speed_pid.getKD(),3);
    string str_spd_il = doubleToStringX(m_speed_pid.getIL(),3);
    svec.push_back("SPEED_PID_KP             = " + str_spd_kp);
    svec.push_back("SPEED_PID_KD             = " + str_spd_kd);
    svec.push_back("SPEED_PID_KI             = " + str_spd_ki);
    svec.push_back("SPEED_PID_INTEGRAL_LIMIT = " + str_spd_il);
  }
  else if(ptype == "pitch") {
    string str_pitch_kp = doubleToStringX(m_pitch_pid.getKP(),3);
    string str_pitch_ki = doubleToStringX(m_pitch_pid.getKI(),3);
    string str_pitch_kd = doubleToStringX(m_pitch_pid.getKD(),3);
    string str_pitch_il = doubleToStringX(m_pitch_pid.getIL(),3);
    svec.push_back("PITCH_PID_KP             = " + str_pitch_kp);
    svec.push_back("PITCH_PID_KD             = " + str_pitch_kd);
    svec.push_back("PITCH_PID_KI             = " + str_pitch_ki);
    svec.push_back("PITCH_PID_INTEGRAL_LIMIT = " + str_pitch_il);
  }
  else if(ptype == "zpitch") {
    string str_zpitch_kp = doubleToStringX(m_z_to_pitch_pid.getKP(),3);
    string str_zpitch_ki = doubleToStringX(m_z_to_pitch_pid.getKI(),3);
    string str_zpitch_kd = doubleToStringX(m_z_to_pitch_pid.getKD(),3);
    string str_zpitch_il = doubleToStringX(m_z_to_pitch_pid.getIL(),3);
    svec.push_back("Z_TO_PITCH_PID_KP             = " + str_zpitch_kp);
    svec.push_back("Z_TO_PITCH_PID_KD             = " + str_zpitch_kd);
    svec.push_back("Z_TO_PITCH_PID_KI             = " + str_zpitch_ki);
    svec.push_back("Z_TO_PITCH_PID_INTEGRAL_LIMIT = " + str_zpitch_il);
  }

  return(svec);
}

