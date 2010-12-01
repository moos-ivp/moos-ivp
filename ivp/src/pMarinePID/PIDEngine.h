/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PIDEngine.h                                          */
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

#ifndef MARINE_PIDENGINE_HEADER
#define MARINE_PIDENGINE_HEADER

#include <vector>
#include <string>
#include "ScalarPID.h"

class PIDEngine {
public:
  PIDEngine();
  ~PIDEngine() {};

  void setPID(int, ScalarPID);
  void setSpeedFactor(double v)     {m_speed_factor = v;};
  void updateTime(double ctime)     {m_current_time = ctime;};

  void setRudderStep(double);
  void setThrusterStep(double);
  void setElevatorStep(double);

  double getDesiredRudder(double desired_heading, 
			  double current_heading);
  double getDesiredThrust(double desired_speed, 
			  double current_speed,
			  double current_thrust);
  double getDesiredElevator(double desired_depth, 
			    double current_depth);

  void clearReport() {m_pid_report.clear();};
  std::vector<std::string> getPIDReport() {return(m_pid_report);};

protected:
  ScalarPID m_heading_pid;
  ScalarPID m_speed_pid;
  ScalarPID m_depth_pid;

  double  m_rudder_step;
  double  m_thruster_step;
  double  m_elevator_step;

  double  m_current_time;
  double  m_speed_factor;

  std::vector<std::string> m_pid_report;
};
#endif
