/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PIDEngine.h                                          */
/*    DATE: Jul 31st, 2005 Sunday in Montreal                    */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef MARINE_PIDENGINE_HEADER
#define MARINE_PIDENGINE_HEADER

#include <vector>
#include <string>
#include "IB_HelmIvP.h"
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






