/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PIDEngine.cpp                                        */
/*    DATE: Jul 31st, 2005 Sunday in Montreal                    */
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
#endif

#include <iostream>
#include "PIDEngine.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "MOOSGenLib.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

PIDEngine::PIDEngine()
{
  // If spid_active is zero speed is controlled via PID. 
  // If not, thrust is set to multiple of desired speed.
  speed_factor = 20.0;
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
  heading_pid.Run(heading_error, current_time, desired_rudder);
  desired_rudder *= -1.0;
    
  // Enforce limit on desired rudder
  MOOSAbsLimit(desired_rudder,max_rudder);

  string rpt = "PID_COURS: ";
  rpt += " (Want):" + doubleToString(desired_heading);
  rpt += " (Curr):" + doubleToString(current_heading);
  rpt += " (Diff):" + doubleToString(heading_error);
  rpt += " RUDDER:" + doubleToString(desired_rudder);
  pid_report.push_back(rpt);
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
  if(speed_factor != 0) {
    desired_thrust = desired_speed * speed_factor;
  }
  // ELSE apply the PID contoller to the problem.
  else {
    speed_pid.Run(speed_error, current_time, delta_thrust);
    desired_thrust += delta_thrust;
  }
  
  if(desired_thrust < 0)
    desired_thrust = 0;

  if(speed_factor != 0) {
    string rpt = "PID_SPEED: ";
    rpt += " (Want):" + doubleToString(desired_speed);
    rpt += " (Curr):" + doubleToString(current_speed);
    rpt += " (Diff):" + doubleToString(speed_error);
    rpt += " (Fctr):" + doubleToString(speed_factor);
    rpt += " THRUST:" + doubleToString(desired_thrust);
    pid_report.push_back(rpt);
  }    
  else {
    string rpt = "PID_SPEED: ";
    rpt += " (Want):" + doubleToString(desired_speed);
    rpt += " (Curr):" + doubleToString(current_speed);
    rpt += " (Diff):" + doubleToString(speed_error);
    rpt += " (Delt):" + doubleToString(delta_thrust);
    rpt += " THRUST:" + doubleToString(desired_thrust);
    pid_report.push_back(rpt);
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
  z_to_pitch_pid.Run(depth_error, current_time, desired_pitch);

  // Enforce limits on desired pitch
  MOOSAbsLimit(desired_pitch,max_pitch);

  double pitch_error = current_pitch - desired_pitch;
  pitch_pid.Run(pitch_error, current_time, desired_elevator);

  // Convert desired elevator to degrees
  desired_elevator=MOOSRad2Deg(desired_elevator);

  // Enforce elevator limit
  MOOSAbsLimit(desired_elevator,max_elevator);
  
  string rpt = "PID_DEPTH: ";
  rpt += " (Want):" + doubleToString(desired_depth);
  rpt += " (Curr):" + doubleToString(current_depth);
  rpt += " (Diff):" + doubleToString(depth_error);
  rpt += " ELEVATOR:" + doubleToString(desired_elevator);
  pid_report.push_back(rpt);

  return(desired_elevator);
}

//-----------------------------------------------------------
// Procedure: setPID

void PIDEngine::setPID(int ix, ScalarPID g_pid)
{
  if(ix==0) heading_pid     = g_pid;
  if(ix==1) speed_pid       = g_pid;
  if(ix==2) z_to_pitch_pid  = g_pid;
  if(ix==3) pitch_pid       = g_pid;
}










