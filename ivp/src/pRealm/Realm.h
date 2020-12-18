/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: Realm.h                                         */
/*    DATE: December 2nd, 2020                              */
/************************************************************/

#ifndef REALM_APP_HEADER
#define REALM_APP_HEADER

#include <map>
#include <list>
#include <set>
#include <string>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "ACTable.h"

class Realm : public AppCastingMOOSApp
{
 public:
  Realm();
  ~Realm() {};

 protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Standard protected MOOS and Appcast functions
  bool buildReport();
  void registerVariables();
 
 protected: // Auxilliary functions

  void buildRealmCast();
  void buildRealmCast(std::string channel);

  void buildRealmCastSummary();
  
 protected:
  void handleMailDBRWSummary(std::string);
  void handleMailRealmCastReq(std::string, std::string, std::string);

  void handleGeneralMail(const CMOOSMsg&);

  std::vector<std::string> getAppsVector() const;

  void resetACTable();
  void addRowACTab(std::string var, std::string src, std::string time,
		   std::string community, std::string value);

  bool isIgnoreVar(std::string) const;

  
 private: // Configuration variables

  std::string m_channel;

  unsigned int m_msg_max_hist;

  double m_relcast_interval;
  double m_summary_interval;

  unsigned int m_wrap_length;
  unsigned int m_trunc_length;
  
 private: // State variables

  // A Map from var name to recently received values;
  std::map<std::string, std::list<CMOOSMsg> > m_map_data;

  // Maps from app name to list of pub/sub variables
  std::map<std::string, std::set<std::string> > m_map_subs;
  std::map<std::string, std::set<std::string> > m_map_pubs;
  std::map<std::string, unsigned int> m_map_post_count;

  std::set<std::string> m_set_apps;
  std::set<std::string> m_set_realm_subs;
  std::set<std::string> m_set_realm_pubs;
  std::set<std::string> m_set_unique_vars;

  unsigned int m_summaries_posted;

  std::map<std::string, double> m_map_key_expire;
  std::map<std::string, std::string> m_map_key_channel;
  
  double m_expire_time;
  double m_time_warp;
  double m_last_post_summary;
  double m_last_post_relcast;

  bool m_new_app_noticed; 
  
  bool m_show_source;
  bool m_show_community;
  bool m_wrap_content;

  bool m_show_subscriptions;
  bool m_show_masked;
  bool m_trunc_content;
  
  std::string m_moosdb_name;

  ACTable m_actab;

};

#endif 
