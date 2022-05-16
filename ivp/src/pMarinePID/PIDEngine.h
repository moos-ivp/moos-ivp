/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
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

  void setDebugHdg();
  void setDebugSpd();
  void setDebugDep();

  void setHdgIntegralReset(bool zero_error, bool new_desired) 
        {m_reset_hdg_i_zero_error = zero_error; m_reset_hdg_i_new_desired = new_desired;}
  void setSpdIntegralReset(bool zero_error, bool new_desired)
        {m_reset_spd_i_zero_error = zero_error; m_reset_spd_i_new_desired = new_desired;}
  void setDepIntegralReset(bool zero_error, bool new_desired)
        {m_reset_dep_i_zero_error = zero_error; m_reset_dep_i_new_desired = new_desired;}
  void setPchIntegralReset(bool zero_error, bool new_desired)
        {m_reset_pch_i_zero_error = zero_error; m_reset_pch_i_new_desired = new_desired;}
  
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

  bool getMaxSatHdg() {return(m_max_sat_hdg);}
  bool getMaxSatSpd() {return(m_max_sat_spd);}
  bool getMaxSatDep() {return(m_max_sat_dep);}
  
  std::string getMaxSatHdgStr() {return(m_max_sat_hdg_str);}
  std::string getMaxSatSpdStr() {return(m_max_sat_spd_str);}
  std::string getMaxSatDepStr() {return(m_max_sat_dep_str);}
  
protected:
  ScalarPID m_heading_pid;
  ScalarPID m_speed_pid;
  ScalarPID m_z_to_pitch_pid;
  ScalarPID m_pitch_pid;

  double  m_current_time;
  double  m_speed_factor;

  std::vector<std::string> m_pid_report;

  // Added April 2019 by mikerb
  // Regardless of debug settings, take note if a max saturation
  // condition was encountered for any pid controllers on any iter.
  bool m_max_sat_hdg;
  bool m_max_sat_spd;
  bool m_max_sat_dep;

  // Added April 2019 by mikerb
  // If debug settings on, ScalarPIDs will build debug output on
  // every iteration. Likely only want to do this if seeing max_sat.
  bool m_debug_hdg;
  bool m_debug_spd;
  bool m_debug_dep;

  // Added April 2019 by mikerb
  // If a max saturation was encountered, debugging output may be
  // in these message strings if debugging had been turned on.
  std::string m_max_sat_hdg_str;
  std::string m_max_sat_spd_str;
  std::string m_max_sat_dep_str;  

  // Added May 2022 to reset integral when crossing desired command
  // and/or when desired command changes.
  double m_desired_headingOld;
  double m_desired_speedOld;
  double m_desired_depthOld;
  double m_desired_pitchOld;
  double m_heading_errorOld;
  double m_speed_errorOld;
  double m_depth_errorOld;
  double m_pitch_errorOld;

  bool    m_reset_hdg_i_zero_error;
  bool    m_reset_spd_i_zero_error;
  bool    m_reset_dep_i_zero_error;
  bool    m_reset_pch_i_zero_error;
  bool    m_reset_hdg_i_new_desired;
  bool    m_reset_spd_i_new_desired;
  bool    m_reset_dep_i_new_desired;
  bool    m_reset_pch_i_new_desired;
};
#endif



















