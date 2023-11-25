/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ContactMgrV20.h                                      */
/*    DATE: Feb 24th 2010                                        */
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

#ifndef CONTACT_MANAGER_V20_HEADER
#define CONTACT_MANAGER_V20_HEADER

#include <vector>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"
#include "NodeRecord.h"
#include "XYPolygon.h"
#include "PlatformAlertRecord.h"
#include "CMAlert.h"
#include "ExFilterSet.h"

class ContactMgrV20 : public AppCastingMOOSApp
{
 public:
  ContactMgrV20();
  virtual ~ContactMgrV20() {}

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 public:
  bool buildReport();

 protected:
  void registerVariables();

  bool handleConfigAlert(std::string, std::string);
  bool handleConfigDecay(std::string);

  bool handleConfigMaxRetHist(std::string);
  bool handleConfigCoords(std::string);
  bool handleConfigRecapInterval(std::string);
  bool handleConfigRejectRange(std::string);

  std::string handleConfigDeprecations(std::string);

  void handleMailNodeReport(std::string);
  void handleMailReportRequest(std::string, std::string);
  void handleMailAlertRequest(std::string, std::string);
  void handleMailDisplayRadii(std::string);
  void handleMailHelmState(std::string);

  void updateRanges();
  void postSummaries();
  void checkForAlerts();

  void postRadii(bool=true);
  void postOnAlerts(NodeRecord, std::string id);
  void postOffAlerts(NodeRecord, std::string id);
  void postAlert(NodeRecord, VarDataPair);

  bool checkAlertApplies(std::string contact, std::string id);
  bool knownAlert(std::string id) const;

  void checkForNewRetiredContacts();
  void postRangeReports();
  
  std::list<std::string> getRangeOrderedContacts() const;
  
  void pruneRangeReports();
  
  
 protected: // Alert Getters
  double      getAlertRange(std::string id) const;
  double      getAlertRangeCPA(std::string id) const;

  std::vector<VarDataPair> getAlertOnFlags(std::string id) const;
  std::vector<VarDataPair> getAlertOffFlags(std::string id) const;

 private: // main record of alerts, each keyed on the alert_id
  std::map<std::string, CMAlert> m_map_alerts;
  
 protected: // Configuration parameters

  // Global Exclusion filters
  double       m_reject_range;
  ExFilterSet  m_filter_set;
  unsigned int m_max_contacts;
  
  // Other configuration parameters
  std::string  m_ownship;
  bool         m_display_radii;
  std::string  m_display_radii_id;
  std::string  m_alert_rng_color;
  std::string  m_alert_rng_cpa_color;

  bool         m_post_closest_range;
  bool         m_post_closest_relbng;
  bool         m_post_all_ranges;
  double       m_contact_max_age;
  double       m_contacts_recap_interval;
  double       m_range_report_timeout;
  unsigned int m_range_report_maxsize;
  
  std::string  m_contact_local_coords;
  bool         m_alert_verbose;
  double       m_decay_start;
  double       m_decay_end;

  double       m_closest_contact_rng_one;
  double       m_closest_contact_rng_two;

  unsigned int m_max_retired_hist;
  
 protected: // State variables

  double m_osx;
  double m_osy;
  double m_osh;
  double m_osv;

  std::string m_os_group;
  std::string m_os_type;

  double m_contacts_recap_posted;

  std::set<std::string> m_filtered_vnames;

  // Optional requested range reports all keyed on varname
  std::map<std::string, double>      m_map_rep_range;
  std::map<std::string, double>      m_map_rep_reqtime;
  std::map<std::string, std::string> m_map_rep_group;
  std::map<std::string, std::string> m_map_rep_vtype;
  std::map<std::string, std::string> m_map_rep_contacts;
  std::map<std::string, bool>        m_map_rep_refresh;
  
  // Main Record #2: The Vehicles (contacts) and position info
  std::map<std::string, NodeRecord>   m_map_node_records;
  std::map<std::string, double>       m_map_node_ranges_actual;
  std::map<std::string, double>       m_map_node_ranges_extrap;
  std::map<std::string, double>       m_map_node_ranges_cpa;

  std::string m_closest_name;

  // memory of previous status postings: A posting to the MOOS var is
  // only made when a change is detected between curr and prev.
  std::string m_prev_contacts_list;
  std::string m_prev_contacts_retired;
  std::string m_prev_contacts_alerted;
  std::string m_prev_contacts_recap;
  std::string m_prev_contact_closest;
  unsigned int m_prev_contacts_count;

  double  m_prev_closest_range;
  double  m_prev_closest_relbng;  // relbng of closest contact
  double  m_prev_closest_contact_val;

  // A matrix: vehicle_name X alert_id. Cell val is Boolean
  // indicating if the alert is active or resolved, for
  // the given vehicle.
  PlatformAlertRecord  m_par;

  std::list<std::string> m_contacts_retired;

  bool  m_hold_alerts_for_helm;
  bool  m_helm_in_drive_noted;
  
private:
  bool         m_use_geodesy;
  CMOOSGeodesy m_geodesy;

  unsigned int m_alert_requests_received;
};

#endif 

