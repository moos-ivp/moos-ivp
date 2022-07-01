/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BasicContactMgr.h                                    */
/*    DATE: Feb 24th 2010                                        */
/*    DATE: Sep 26th 2017 Added Alert Region Support / mikerb    */
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

#ifndef BASIC_CONTACT_MANAGER_HEADER
#define BASIC_CONTACT_MANAGER_HEADER

#include <vector>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"
#include "NodeRecord.h"
#include "XYPolygon.h"
#include "PlatformAlertRecord.h"
#include "CMAlert.h"

class BasicContactMgr : public AppCastingMOOSApp
{
 public:
  BasicContactMgr();
  virtual ~BasicContactMgr() {}

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 public:
  bool buildReport();
  bool deprecated() {return(true);}
  
 protected:
  void registerVariables();
  bool handleConfigAlert(std::string);
  bool handleConfigIgnoreGroup(std::string);

  void handleMailNodeReport(std::string);
  void handleMailReportRequest(std::string);
  void handleMailDisplayRadii(std::string);
  void handleMailAlertRequest(std::string);
  void handleMailResolved(std::string);

  void updateRanges();
  void postSummaries();
  void checkForAlerts();
  void checkForCloseInReports();

  void postRadii(bool=true);
  void postOnAlerts(NodeRecord, std::string id);
  void postOffAlerts(NodeRecord, std::string id);
  void postAlert(NodeRecord, VarDataPair);

  bool checkAlertApplies(std::string contact, std::string id);
  bool knownAlert(std::string id) const;

  void checkForNewRetiredContacts();
  void postRangeReports();

 protected: // Alert Getters
  double      getAlertRange(std::string id) const;
  double      getAlertRangeCPA(std::string id) const;

  bool        hasAlertRegion(std::string id) const;
  XYPolygon   getAlertRegion(std::string id) const;

  std::string getAlertRangeColor(std::string id) const;
  std::string getAlertRangeCPAColor(std::string id) const;

  bool        hasAlertOnFlag(std::string id) const;
  bool        hasAlertOffFlag(std::string id) const;

  std::vector<VarDataPair> getAlertOnFlags(std::string id) const;
  std::vector<VarDataPair> getAlertOffFlags(std::string id) const;

  std::vector<std::string> getAlertMatchTypes(std::string id) const;
  std::vector<std::string> getAlertIgnoreTypes(std::string id) const;

  std::vector<std::string> getAlertMatchGroups(std::string id) const;
  std::vector<std::string> getAlertIgnoreGroups(std::string id) const;

 private: // main record of alerts, each keyed on the alert_id
  std::map<std::string, CMAlert> m_map_alerts;
  
 protected: // Configuration parameters

  // Default values for various alert parameters
  double       m_default_alert_rng;
  double       m_default_alert_rng_cpa;
  std::string  m_default_alert_rng_color;
  std::string  m_default_alert_rng_cpa_color;

  double       m_reject_range;

  std::vector<std::string> m_ignore_groups;
  std::vector<std::string> m_ignore_types;
  
  // Other configuration parameters
  std::string  m_ownship;
  bool         m_display_radii;
  bool         m_post_closest_range;
  double       m_contact_max_age;
  double       m_contacts_recap_interval;
  double       m_range_report_timeout;
  
  std::string  m_contact_local_coords;
  bool         m_alert_verbose;
  double       m_decay_start;
  double       m_decay_end;

  double       m_closest_contact_rng_one;
  double       m_closest_contact_rng_two;

  double       m_eval_range_far;
  double       m_eval_range_near;

  unsigned int m_max_retired_hist;
  
 protected: // State variables

  double m_nav_x;
  double m_nav_y;
  double m_nav_hdg;
  double m_nav_spd;

  double m_contacts_recap_posted;

  // Optional requested range reports
  std::map<std::string, double>      m_map_rep_range;
  std::map<std::string, double>      m_map_rep_reqtime;
  std::map<std::string, std::string> m_map_rep_group;
  std::map<std::string, std::string> m_map_rep_vtype;
  std::map<std::string, std::string> m_map_rep_contacts;
  
  // Main Record #2: The Vehicles (contacts) and position info
  std::map<std::string, NodeRecord>   m_map_node_records;
  std::map<std::string, unsigned int> m_map_node_alerts_total;
  std::map<std::string, unsigned int> m_map_node_alerts_active;
  std::map<std::string, unsigned int> m_map_node_alerts_resolved;
  std::map<std::string, double>       m_map_node_ranges_actual;
  std::map<std::string, double>       m_map_node_ranges_extrap;
  std::map<std::string, double>       m_map_node_ranges_cpa;

  std::string m_closest_name;

  // memory of previous status postings: A posting to the MOOS
  // var is only made when a change is detected between
  // curr and prev.
  std::string m_prev_contacts_list;
  std::string m_prev_contacts_retired;
  std::string m_prev_contacts_alerted;
  std::string m_prev_contacts_unalerted;
  std::string m_prev_contacts_recap;
  std::string m_prev_contact_closest;
  unsigned int m_prev_contacts_count;

  double  m_prev_closest_range;
  double  m_prev_closest_contact_val;

  // A matrix: vehicle_name X alert_id. Cell val is Boolean
  // indicating if the alert is active or resolved, for
  // the given vehicle.
  PlatformAlertRecord  m_par;

  std::list<std::string> m_contacts_retired;
 
private:
  bool         m_use_geodesy;
  CMOOSGeodesy m_geodesy;

  unsigned int m_alert_requests_received;
};

#endif 
