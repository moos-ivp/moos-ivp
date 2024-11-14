/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: NodeReporter.h                                       */
/*    DATE: Feb 9th 2006 (TransponderAIS)                        */
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

#ifndef NODE_REPORTER_HEADER
#define NODE_REPORTER_HEADER

#include <string>
#include <vector>
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "NodeRecord.h"
#include "NodeRiderSet.h"
#include "Odometer.h"
#include "LinearExtrapolator.h"

class NodeReporter : public AppCastingMOOSApp
{
public:
  NodeReporter();
  virtual ~NodeReporter() {}
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

  void registerVariables();
  bool buildReport();

 protected:
  void handleLocalHelmSummary(const std::string&);
  std::string assembleNodeReport(NodeRecord);
  std::string assemblePlatformReport();
  
  void updatePlatformVar(std::string, std::string);
  bool addPlatformVar(std::string);
  bool setCrossFillPolicy(std::string);
  void crossFillCoords(NodeRecord&, double, double);
  void crossFillLocalToGlobal(NodeRecord&);
  void crossFillGlobalToLocal(NodeRecord&);
  void handleHelmSwitch();

  void updateNavWarning(bool ok_nav) const;
  void updateMHashOdo();
  
  bool handleMailRiderVars(std::string, std::string, double);
  
 protected: // Configuration Variables (Node Reports)
  std::string  m_vessel_name;
  std::string  m_crossfill_policy;
  std::string  m_node_report_var;
  double       m_nohelm_thresh;
  std::string  m_group_name;
  bool         m_terse_reports;
  bool         m_allow_color_change;

  // Sep 01, 2022
  bool         m_extrap_enabled;
  double       m_extrap_pos_thresh;
  double       m_extrap_hdg_thresh;
  double       m_extrap_max_gap;
  
 protected: // State Variables (Node Reports)
  CMOOSGeodesy m_geodesy;
  std::string  m_helm_mode;
  std::string  m_helm_mode_aux;
  std::string  m_helm_allstop_mode;
  std::string  m_alt_nav_prefix;
  std::string  m_alt_nav_name;
  std::string  m_alt_nav_group;
  std::string  m_helm_status_primary;
  std::string  m_helm_status_standby;
  double       m_helm_lastmsg;

  // Oct 18, 2021
  double m_nav_grace_period;
  bool   m_nav_warning_posted;
  
  NodeRecord   m_record;
  NodeRecord   m_record_gt;
  double       m_record_gt_updated;

  double       m_nav_xy_updated;
  double       m_nav_latlon_updated;
  double       m_nav_xy_updated_gt;
  double       m_nav_latlon_updated_gt;

 protected: // State Variables (General)
  bool         m_helm_switch_noted;
  unsigned int m_reports_posted;
  unsigned int m_reports_posted_alt_nav;
  bool         m_thrust_mode_reverse;
  bool         m_paused;
  bool         m_node_info_posted;

  // Post 24.8: Allow node reports to contain just LAT/LON
  bool    m_coord_policy_global; 
  
 protected: // Config and State Vars (Blackout Interval)
  double  m_blackout_interval;
  double  m_blackout_baseval;
  double  m_blackout_variance;
  double  m_last_post_time;

 protected: // Config and State Vars (Platform Reports)
  std::string              m_plat_report_var;
  std::vector<std::string> m_plat_vars;
  std::vector<std::string> m_plat_vals;
  std::vector<std::string> m_plat_alias;
  std::vector<double>      m_plat_post_gap;
  std::vector<double>      m_plat_post_tstamp;
  std::vector<double>      m_plat_recv_tstamp;

 protected: 
  LinearExtrapolator m_extrapolator;
  NodeRecord         m_record_last_posted;

protected: // MissionHash support
  Odometer     m_odometer;
  std::string  m_curr_mhash;
  double       m_max_extent;
  double       m_max_extent_prev;
  
 protected: // NodeRider support
  NodeRiderSet m_riderset;
};

#endif

