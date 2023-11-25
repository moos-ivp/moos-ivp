/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Realm.h                                              */
/*    DATE: December 2nd, 2020                                   */
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

#ifndef REALM_APP_HEADER
#define REALM_APP_HEADER

#include <map>
#include <list>
#include <set>
#include <string>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "ACTable.h"
#include "PipeWay.h"

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
  bool buildRealmCastChannel(PipeWay);
  bool buildWatchCast(PipeWay);
  void buildRealmCastSummary();
  
 protected:
  bool handleConfigHistVar(std::string);
  bool handleConfigScopeSet(std::string);
  void handleMailDBRWSummary(std::string);
  void handleMailRealmCastReq(std::string);

  void handleGeneralMail(const CMOOSMsg&);

  std::vector<std::string> getAppsVector() const;

  bool isIgnoreVar(std::string) const;

 protected: // convenience functions for ACTable operations
  void resetACTable(PipeWay);
  void addRowACTab(PipeWay, std::string var, std::string src,
		   std::string time, std::string comm, std::string val);

  void addLatestOutCast(std::string);

  std::string getHistoryVars() const;
  
 private: // Configuration variables

  unsigned int m_msg_max_hist;

  double m_relcast_interval;
  double m_summary_interval;

  unsigned int m_wrap_length;
  unsigned int m_trunc_length;

  std::set<std::string> m_set_hist_vars;

  std::map<std::string, std::set<std::string> > m_map_scope_sets;
  
 private: // State variables (DB Info)

  // A Map from var name to recently received values;
  std::map<std::string, std::list<CMOOSMsg> > m_map_data;

  // Maps from app name to list of pub/sub variables
  std::map<std::string, std::set<std::string> > m_map_subs;
  std::map<std::string, std::set<std::string> > m_map_pubs;

  // Convenient summaries of overall pubs,subs,apps,vars
  std::set<std::string> m_set_apps;
  std::set<std::string> m_set_realm_subs;
  std::set<std::string> m_set_realm_pubs;
  std::set<std::string> m_set_unique_vars;

  // Info about local MOOSDB
  std::string m_moosdb_name;
  double      m_time_warp;
  bool        m_new_app_noticed; 

  // Info about client interactions on requests and postings
  unsigned int m_summaries_posted;

  // Key is the client, e.g., pmv, umview 
  std::map<std::string, PipeWay> m_map_pipeways;

  std::map<std::string, double> m_map_var_last_wcast;

  std::string m_last_post_summary_info;
  double m_last_post_summary;  
  double m_last_post_relcast;

  unsigned int m_total_realmcasts;
  unsigned int m_total_watchcasts;
  
  std::list<std::string> m_recent_outcasts;
  
  // Keep an ACTable scope available  
  ACTable m_actab;
};

#endif 

