/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PIDEngine.h                                          */
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

#ifndef MARINE_PIDENGINE_HEADER
#define MARINE_PIDENGINE_HEADER

#include <vector>
#include <string>
#include "ScalarPID.h"

class PIDEngine {
public:
  PIDEngine();
  ~PIDEngine() {}

  void setPID(int, ScalarPID);
  void setSpeedFactor(double v)     {m_speed_factor = v;}
  void updateTime(double ctime)     {m_current_time = ctime;}

  double getDesiredRudder(double desired_heading, 
			  double current_heading,
			  double max_rudder);
  double getDesiredThrust(double desired_speed, 
			  double current_speed,
			  double current_thrust,
			  double max_thrust);
  double getDesiredElevator(double desired_depth, 
			    double current_depth,
			    double current_pitch,
			    double max_pitch,
			    double max_elevator);

  void clearReport() {m_pid_report.clear();}
  std::vector<std::string> getPIDReport() {return(m_pid_report);}

protected:
  ScalarPID m_heading_pid;
  ScalarPID m_speed_pid;
  ScalarPID m_z_to_pitch_pid;
  ScalarPID m_pitch_pid;

  double  m_current_time;
  double  m_speed_factor;

  std::vector<std::string> m_pid_report;
};
#endif















