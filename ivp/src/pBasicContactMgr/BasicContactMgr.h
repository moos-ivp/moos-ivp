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
#include "MOOSLib.h"
#include "NodeRecord.h"
#include "PlatformAlertRecord.h"

class BasicContactMgr : public CMOOSApp
{
 public:
  BasicContactMgr();
  virtual ~BasicContactMgr() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void RegisterVariables();

 public:
  void setVerbose(bool val) {m_verbose=val;};

 protected:
  bool addNewAlert(const std::string&);
  bool handleNodeReport(const std::string&);
  bool handleResolved(std::string);
  void postErrorMsg(const std::string&);
  void postSummaries();
  bool postAlerts();
  void updateRanges();
  void postRadii();
  void postConsoleSummary();
  void augmentAlertHistory(const std::string& alert);

 protected: // Configuration parameters

  // Main Record #1: The Alerts
  std::vector<std::string>   m_alert_var;
  std::vector<std::string>   m_alert_val;
  std::vector<std::string>   m_alert_id;

  PlatformAlertRecord  m_par;

  std::string  m_ownship;
  bool   m_verbose;
  bool   m_display_radii;
  double m_contact_max_age;
  double m_alert_range;
  double m_alert_cpa_range;
  double m_alert_cpa_time;

 protected: // State variables

  // Main Record #1: The Vehicles and position info
  std::vector<NodeRecord> m_node_records;
  std::vector<double>     m_ranges;

  // memory of previous status postings
  std::string m_prev_contacts_list;
  std::string m_prev_contacts_retired;
  std::string m_prev_contacts_alerted;
  std::string m_prev_contacts_unalerted;
  std::string m_prev_contacts_recap;

  std::list<std::string> m_alert_history;

  double m_nav_x;
  double m_nav_y;
  double m_nav_lat;
  double m_nav_lon;
  double m_nav_hdg;
  double m_nav_spd;
  double m_nav_dep;
  
  double m_pos_timestamp;
  double m_curr_time;
  double m_start_time;

  unsigned int m_iteration;
};

#endif 

