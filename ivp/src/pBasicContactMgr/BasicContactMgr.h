/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BasicContactMgr.h                                    */
/*    DATE: Feb 24th 2010                                        */
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

#ifndef BASIC_CONTACT_MANAGER_HEADER
#define BASIC_CONTACT_MANAGER_HEADER

#include <vector>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "NodeRecord.h"
#include "PlatformAlertRecord.h"

class BasicContactMgr : public AppCastingMOOSApp
{
 public:
  BasicContactMgr();
  virtual ~BasicContactMgr() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 public:
  bool buildReport();

 protected:
  void registerVariables();
  bool handleConfigAlert(const std::string&);
  void handleMailNodeReport(const std::string&);
  void handleMailDisplayRadii(const std::string&);
  void handleMailResolved(const std::string&);
  void updateRanges();
  void postSummaries();
  bool postAlerts();
  void postRadii(bool=true);

  double      getAlertRange(const std::string& alert_id) const;
  double      getAlertRangeCPA(const std::string& alert_id) const;
  std::string getAlertRangeColor(const std::string& alert_id) const;
  std::string getAlertRangeCPAColor(const std::string& alert_id) const;

 protected: // Configuration parameters

  // Main Record #1: The Alerts
  std::map<std::string, std::string> m_map_alert_varname;
  std::map<std::string, std::string> m_map_alert_pattern;
  std::map<std::string, double>      m_map_alert_rng;
  std::map<std::string, double>      m_map_alert_rng_cpa;
  std::map<std::string, std::string> m_map_alert_rng_color;
  std::map<std::string, std::string> m_map_alert_rng_cpa_color;

  // Default values for various alert parameters
  double       m_default_alert_rng;
  double       m_default_alert_rng_cpa;
  std::string  m_default_alert_rng_color;
  std::string  m_default_alert_rng_cpa_color;

  // Other configuration parameters
  std::string  m_ownship;
  bool         m_display_radii;
  double       m_contact_max_age;

 protected: // State variables

  // Main Record #2: The Vehicles (contacts) and position info
  std::map<std::string, NodeRecord>   m_map_node_records;
  std::map<std::string, double>       m_map_node_ranges;
  std::map<std::string, unsigned int> m_map_node_alerts_total;
  std::map<std::string, unsigned int> m_map_node_alerts_active;
  std::map<std::string, unsigned int> m_map_node_alerts_resolved;

  // memory of previous status postings: A posting to the MOOS var
  // is only made when a change is detected between curr and prev.
  std::string m_prev_contacts_list;
  std::string m_prev_contacts_retired;
  std::string m_prev_contacts_alerted;
  std::string m_prev_contacts_unalerted;
  std::string m_prev_contacts_recap;

  // A matrix: vehicle_name X alert_id. Cell val is Boolean indicating
  // if the alert is active or resolved, for the given vehicle.
  PlatformAlertRecord  m_par;

  double m_nav_x;
  double m_nav_y;
  double m_nav_hdg;
  double m_nav_spd;
};

#endif 

