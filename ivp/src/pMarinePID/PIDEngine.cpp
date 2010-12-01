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

#include "PIDEngine.h"
#include "AngleUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

PIDEngine::PIDEngine()
{
  // If spid_active is zero speed is controlled via PID. 
  // If not, thrust is set to multiple of desired speed.
  m_speed_factor = 20.0;
  m_current_time = 0;

  m_elevator_step = 0;
  m_rudder_step   = 0;
  m_thruster_step = 0;

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
    m_depth_pid = g_pid;
}


//-----------------------------------------------------------
// Procedure: setRudderStep

void PIDEngine::setRudderStep(double step_value)
{
  m_rudder_step = vclip(step_value, 0, 10);
}

//-----------------------------------------------------------
// Procedure: setThrusterStep

void PIDEngine::setThrusterStep(double step_value)
{
  m_thruster_step = vclip(step_value, 0, 10);
}

//-----------------------------------------------------------
// Procedure: setElevatorStep

void PIDEngine::setElevatorStep(double step_value)
{
  m_elevator_step = vclip(step_value, 0, 10);
}


//------------------------------------------------------------
// Procedure: getDesiredRudder
// Rudder angles are processed in degrees

double PIDEngine::getDesiredRudder(double desired_heading,
				   double current_heading)
{
  desired_heading = angle180(desired_heading);
  double heading_error = current_heading - desired_heading;
  heading_error = angle180(heading_error);
  double desired_rudder = 0;
  m_heading_pid.Run(heading_error, m_current_time, desired_rudder);
  desired_rudder *= -1.0;
    
  if(m_rudder_step > 0)
    desired_rudder = snapToStep(desired_rudder, m_rudder_step);

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
				   double current_thrust)
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
  }
  
  if(desired_thrust < 0)
    desired_thrust = 0;

  if(m_thruster_step > 0)
    desired_thrust = snapToStep(desired_thrust, m_thruster_step);

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

  return(desired_thrust);
}

//------------------------------------------------------------
// Procedure: getDesiredElevator

double PIDEngine::getDesiredElevator(double desired_depth,
				     double current_depth)
{
  double depth_error      = (desired_depth - current_depth);
  double desired_elevator = 0;

  m_depth_pid.Run(depth_error, m_current_time, desired_elevator);

  if(m_elevator_step > 0)
    desired_elevator = snapToStep(desired_elevator, m_elevator_step);

  string rpt = "PID_DEPTH: ";
  rpt += " (Want):" + doubleToString(desired_depth);
  rpt += " (Curr):" + doubleToString(current_depth);
  rpt += " (Diff):" + doubleToString(depth_error);
  rpt += " ELEVATOR:" + doubleToString(desired_elevator);
  m_pid_report.push_back(rpt);

  return(desired_elevator);
}

