/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PIDEngine.cpp                                        */
/*    DATE: Jul 31st, 2005 Sunday in Montreal                    */
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
#endif

#include <iostream>
#include "PIDEngine.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "MOOS/libMOOS/MOOSLib.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

PIDEngine::PIDEngine()
{
  // If spid_active is zero speed is controlled via PID. 
  // If not, thrust is set to multiple of desired speed.
  m_speed_factor = 20.0;
  m_current_time = 0;

  m_max_sat_hdg = false;
  m_max_sat_spd = false;
  m_max_sat_dep = false;

  m_debug_hdg = false;
  m_debug_spd = false;
  m_debug_dep = false;
}

//------------------------------------------------------------
// Procedure: setDebugHdg()

void PIDEngine::setDebugHdg()
{
  m_debug_hdg = true;
  m_heading_pid.setDebug(true);
}

//------------------------------------------------------------
// Procedure: setDebugSpd

void PIDEngine::setDebugSpd()
{
  m_debug_spd = true;
  m_speed_pid.setDebug(true);
}

//------------------------------------------------------------
// Procedure: setDebugDep

void PIDEngine::setDebugDep()
{
  m_debug_dep = true;
  m_z_to_pitch_pid.setDebug(true);
  m_pitch_pid.setDebug(true);
}

//------------------------------------------------------------
// Procedure: getDesiredRudder
// Rudder angles are processed in degrees

double PIDEngine::getDesiredRudder(double desired_heading,
				   double current_heading,
				   double max_rudder)
{
  desired_heading = angle180(desired_heading);
  double heading_error = current_heading - desired_heading;
  heading_error = angle180(heading_error);
  double desired_rudder = 0;
  m_heading_pid.Run(heading_error, m_current_time, desired_rudder);
  desired_rudder *= -1.0;

  // Added 4/19 mikerb: monitor and report max saturation events
  m_max_sat_hdg_str.clear();
  m_max_sat_hdg = m_heading_pid.getMaxSat();
  if(m_max_sat_hdg)
    m_max_sat_hdg_str = m_heading_pid.getDebugStr();
  
  // Enforce limit on desired rudder
  MOOSAbsLimit(desired_rudder,max_rudder);

  string rpt = "PID_COURSE: ";
  rpt += " (Want):" + doubleToString(desired_heading);
  rpt += " (Curr):" + doubleToString(current_heading);
  rpt += " (Diff):" + doubleToString(heading_error);
  rpt += " RUDDER:" + doubleToString(desired_rudder);
  m_pid_report.push_back(rpt);
  return(desired_rudder);
}

//------------------------------------------------------------
// Procedure: getDesiredThrust

double PIDEngine::getDesiredThrust(double desired_speed, 
				   double current_speed,
				   double current_thrust,
				   double max_thrust)
{
  double speed_error  = desired_speed - current_speed;
  double delta_thrust = 0;
  double desired_thrust = current_thrust;

  // If NOT using PID control, just apply multiple to des_speed
  if(m_speed_factor != 0) {
    desired_thrust = desired_speed * m_speed_factor;
  }
  // ELSE apply the PID contoller to the problem.
  else {
    m_speed_pid.Run(speed_error,  m_current_time, delta_thrust);
    desired_thrust += delta_thrust;

    // Added 4/19 mikerb: monitor and report max saturation events
    m_max_sat_spd_str.clear();
    m_max_sat_spd = m_speed_pid.getMaxSat();
    if(m_max_sat_spd)
      m_max_sat_spd_str = m_speed_pid.getDebugStr();
  }
  
  if(desired_thrust < 0)
    desired_thrust = 0;

  if(m_speed_factor != 0) {
    string rpt = "PID_SPEED: ";
    rpt += " (Want):" + doubleToString(desired_speed);
    rpt += " (Curr):" + doubleToString(current_speed);
    rpt += " (Diff):" + doubleToString(speed_error);
    rpt += " (Fctr):" + doubleToString(m_speed_factor);
    rpt += " THRUST:" + doubleToString(desired_thrust);
    m_pid_report.push_back(rpt);
  }    
  else {
    string rpt = "PID_SPEED: ";
    rpt += " (Want):" + doubleToString(desired_speed);
    rpt += " (Curr):" + doubleToString(current_speed);
    rpt += " (Diff):" + doubleToString(speed_error);
    rpt += " (Delt):" + doubleToString(delta_thrust);
    rpt += " THRUST:" + doubleToString(desired_thrust);
    m_pid_report.push_back(rpt);
  }

  // Enforce limit on desired thrust
  MOOSAbsLimit(desired_thrust,max_thrust);

  return(desired_thrust);
}

//------------------------------------------------------------
// Procedure: getDesiredElevator
// Elevator angles and pitch are processed in radians

double PIDEngine::getDesiredElevator(double desired_depth,
				     double current_depth,
				     double current_pitch,
				     double max_pitch,
				     double max_elevator)
{
  double desired_elevator = 0;
  double desired_pitch = 0;
  double depth_error = current_depth - desired_depth;
  m_z_to_pitch_pid.Run(depth_error, m_current_time, desired_pitch);
  m_max_sat_dep = m_z_to_pitch_pid.getMaxSat();

  // Enforce limits on desired pitch
  MOOSAbsLimit(desired_pitch,max_pitch);

  double pitch_error = current_pitch - desired_pitch;
  m_pitch_pid.Run(pitch_error, m_current_time, desired_elevator);
  m_max_sat_dep = m_max_sat_dep || m_pitch_pid.getMaxSat();


  // Added 4/19 mikerb: monitor and report max saturation events
  m_max_sat_dep_str.clear();
  if(m_z_to_pitch_pid.getMaxSat()) {
    m_max_sat_dep = true;
    m_max_sat_dep_str = "Z:" + m_z_to_pitch_pid.getDebugStr();
  }
  if(m_pitch_pid.getMaxSat()) {
    m_max_sat_dep = true;
    m_max_sat_dep_str += "P:" + m_z_to_pitch_pid.getDebugStr();
  }
  
  // Convert desired elevator to degrees
  desired_elevator=MOOSRad2Deg(desired_elevator);

  // Enforce elevator limit
  MOOSAbsLimit(desired_elevator,max_elevator);
  
  string rpt = "PID_DEPTH: ";
  rpt += " (Want):" + doubleToString(desired_depth);
  rpt += " (Curr):" + doubleToString(current_depth);
  rpt += " (Diff):" + doubleToString(depth_error);
  rpt += " ELEVATOR:" + doubleToString(desired_elevator);
  m_pid_report.push_back(rpt);

  return(desired_elevator);
}

//-----------------------------------------------------------
// Procedure: setPID

void PIDEngine::setPID(int ix, ScalarPID g_pid)
{
  if(ix==0) 
    m_heading_pid = g_pid;
  else if(ix==1) 
    m_speed_pid = g_pid;
  else if(ix==2) 
    m_z_to_pitch_pid = g_pid;
  else if(ix==3) 
    m_pitch_pid = g_pid;
}



















