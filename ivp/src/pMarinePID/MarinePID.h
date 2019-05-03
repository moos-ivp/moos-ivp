/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MarinePID.h                                          */
/*    DATE: April 10 2006                                        */
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

#ifndef MARINE_PID_HEADER
#define MARINE_PID_HEADER

#include <string>
#include "MOOS/libMOOS/MOOSLib.h"
#include "PIDEngine.h"

class MarinePID : public CMOOSApp
{
public:
  MarinePID();
  virtual ~MarinePID() {}
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void postCharStatus();
  void postAllStop();
  void registerVariables();
  bool handleYawSettings();
  bool handleSpeedSettings();
  bool handleDepthSettings();

protected:
  bool     m_has_control;
  bool     m_allow_overide;
  bool     m_allstop_posted;
  double   m_speed_factor;

  double   m_current_heading;
  double   m_current_speed;
  double   m_current_depth;
  double   m_current_pitch;

  double   m_rudder_bias_duration;
  double   m_rudder_bias_limit;
  double   m_rudder_bias_side;
  double   m_rudder_bias_timestamp;

  double   m_desired_heading;
  double   m_desired_speed;
  double   m_desired_depth;
  double   m_current_thrust;

  double   m_max_pitch;
  double   m_max_rudder;
  double   m_max_thrust;
  double   m_max_elevator;

  PIDEngine    m_pengine;
  std::string  m_verbose;

  int     m_iteration;
  double  m_start_time;
  bool    m_depth_control;
  bool    m_paused;

  double  m_ok_skew;

  double  m_time_of_last_helm_msg;
  double  m_time_of_last_nav_msg;

  double  m_tardy_helm_thresh;
  double  m_tardy_nav_thresh;

  bool    m_max_sat_hdg_debug;
  bool    m_max_sat_spd_debug;
  bool    m_max_sat_dep_debug;

  bool    m_ignore_nav_yaw;
};
#endif 









