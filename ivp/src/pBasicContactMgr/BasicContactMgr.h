/************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J.Leonard                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: BasicContactMgr.h                               */
/*    DATE: Feb 24th 2010                                   */
/************************************************************/

#ifndef BASIC_CONTACT_MANAGER_HEADER
#define BASIC_CONTACT_MANAGER_HEADER

#include <vector>
#include "MOOSLib.h"
#include "ContactRecord.h"
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
  std::vector<ContactRecord> m_records;
  std::vector<double>        m_ranges;

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
