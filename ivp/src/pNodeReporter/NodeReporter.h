/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: NodeReporter.h                                       */
/*    DATE: Feb 9th 2006 (TransponderAIS)                        */
/*    DATE: Jun 8th 2009 (NodeReporter)                          */
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

#ifndef NODE_REPORTER_HEADER
#define NODE_REPORTER_HEADER

#include <iostream>
#include <string>
#include <vector>
#include "MOOSLib.h"
#include "MOOSGeodesy.h"

class NodeReporter : public CMOOSApp
{
public:
  NodeReporter();
  virtual ~NodeReporter() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected:
  void handleLocalHelmSummary(const std::string&);
  std::string assembleNodeReport();
  std::string assemblePlatformReport();
  
  void updatePlatformVar(std::string, std::string);
  bool addPlatformVar(std::string);
  void registerVariables();

protected:
  std::string m_vessel_name;
  std::string m_vessel_type;
  std::string m_vessel_len;
  std::string m_vessel_type_src;
  std::string m_vessel_len_src;
  std::string m_helm_mode;
  std::string m_helm_allstop_mode;
  double      m_db_uptime;
  double      m_moos_utc_time;
  bool        m_time_updated;
  double      m_nav_x;
  double      m_nav_y;
  double      m_nav_lat;
  double      m_nav_lon;
  double      m_nav_speed;
  double      m_nav_heading;
  double      m_nav_depth;
  double      m_nav_yaw;

  bool        m_helm_engaged;
  double      m_helm_lastmsg;

  std::string m_position_source;

  // moos variable name for the contact report
  // default name is "NODE_REPORT"
  std::string m_node_report_var;
  
  // for lat long conversion
  CMOOSGeodesy m_geodesy;

  double  m_blackout_interval;
  double  m_blackout_baseval;
  double  m_blackout_variance;
  double  m_last_post_time;
  double  m_nohelm_thresh;

  // moos variable name for the contact report
  // default name is "PLATFORM_REPORT"
  std::string m_plat_report_var;

  std::vector<std::string> m_plat_vars;
  std::vector<std::string> m_plat_vals;
  std::vector<std::string> m_plat_alias;
  std::vector<double>      m_plat_post_gap;
  std::vector<double>      m_plat_post_tstamp;
  std::vector<double>      m_plat_recv_tstamp;
};

#endif








