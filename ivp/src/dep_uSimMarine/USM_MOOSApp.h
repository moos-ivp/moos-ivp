/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: USM_MOOSApp.h                                        */
/*    DATE: Oct 25th 2004                                        */
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

#ifndef USM_MOOSAPP_HEADER
#define USM_MOOSAPP_HEADER

#include <string>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"
#include "USM_Model.h"

class USM_MOOSApp : public AppCastingMOOSApp
{
public:
  USM_MOOSApp();
  virtual ~USM_MOOSApp() {}

 public: // Standard MOOSApp functions to overload
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool OnStartUp();
  bool Iterate();
  bool OnConnectToServer();

 protected: // Standard AppCastingMOOSApp function to overload
  bool buildReport();
  bool deprecated() {return(true);}

 protected:
  void registerVariables();
  void postNodeRecordUpdate(std::string, const NodeRecord&);
  bool handleThrustMapping(std::string);
  void cacheStartingInfo();
  
  std::string handleConfigDeprecations(std::string);

protected:
  std::string  m_sim_prefix;
  USM_Model    m_model;
  unsigned int m_reset_count;
  bool         m_enabled;
  
  CMOOSGeodesy m_geodesy;
  bool         m_geo_ok;
  bool         m_thrust_mode_reverse;
  bool         m_thrust_mode_differential;

  // A cache of starting info to facilitate generation of reports.
  std::string m_start_nav_x;
  std::string m_start_nav_y;
  std::string m_start_nav_hdg;
  std::string m_start_nav_spd;
  std::string m_start_nav_dep;
  std::string m_start_nav_alt;
  std::string m_start_nav_lat;
  std::string m_start_nav_lon;
  std::string m_start_buoyrate;
  std::string m_start_drift_x;
  std::string m_start_drift_y;
  std::string m_start_drift_mag;
  std::string m_start_drift_ang;
  std::string m_start_rotate_spd;

  std::set<std::string> m_srcs_buoyrate;
  std::set<std::string> m_srcs_drift;

  bool   buoyancy_requested;
  bool   trim_requested;
  double buoyancy_request_time;
  double trim_request_time;
  double buoyancy_delay;
  double max_trim_delay;
  double last_report;
  double report_interval;
  double pitch_tolerance;
  
  bool   m_obstacle_hit;
};

#endif




