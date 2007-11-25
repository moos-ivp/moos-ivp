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
  void setSpeedFactor(double v)     {speed_factor = v;};
  void updateTime(double ctime)     {current_time = ctime;};

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

  void clearReport() {pid_report.clear();};
  std::vector<std::string> getPIDReport() {return(pid_report);};

protected:
  ScalarPID heading_pid;
  ScalarPID speed_pid;
  ScalarPID z_to_pitch_pid;
  ScalarPID pitch_pid;

  double  current_time;
  double  speed_factor;

  std::vector<std::string> pid_report;
};
#endif










