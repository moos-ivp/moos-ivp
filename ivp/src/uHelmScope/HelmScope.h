/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmScope.h                                          */
/*    DATE: Apr 12th 2008                                        */
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

#ifndef U_HELM_SCOPE_HEADER
#define U_HELM_SCOPE_HEADER

#include <vector>
#include <string>
#include <map>
#include <list>
#include <set>

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "HelmReport.h"
#include "StringTree.h"
#include "LifeEventHistory.h"
#include "ScopeEntry.h"

class HelmScope : public AppCastingMOOSApp
{
 public:
  HelmScope();
  virtual ~HelmScope() {}
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
  bool buildReport();

  void handleCommand(char);
  void setPaused(bool v)                {m_paused = v;}
  void setDisplayXMS(bool v)            {m_display_xms = v;}
  void setDisplayPosts(bool v)          {m_display_posts = v;}
  void setIgnoreFileVars(bool v)        {m_ignore_filevars = v;}
  void setDisplayTrunc(bool v)          {m_display_truncate = v;}
  void setDisplayVirgins(bool v)        {m_display_virgins = v;}
  void setTermServerHost(std::string s) {m_term_server_host = s;}
  void setTermServerPort(std::string s) {m_term_server_port = s;}

 public:  // Public Functions for basic DB Scoping
  void addScopeVariables(const std::string& varname_list);
  void addScopeVariable(const std::string& varname);

 protected:
  void registerVariables();
  void handleNewIvPDomain(const std::string&);
  void handleNewHelmSummary(const std::string&);
  void handleNewHelmModeSet(const std::string&);
  void handleNewStateVars(const std::string&);
  void updateScopeEntries(const CMOOSMsg&);
  void handleHelmStatusVar(const std::string&);
  
 protected: // Protected Functions for basic DB Scoping
  void updateEngaged(const std::string&);

  void printHelp();
  void printModeSet();
  void printReport();
  void printLifeEventHistory();
  void printDBReport();
  void printPostingReport();
  void printWarnings();
 
  // An overloading of the CMOOSApp ConfigureComms function
  bool ConfigureComms();
 
 protected: 
  // Set of variables named by user or involved in behavior conditions
  std::set<std::string> m_set_scopevars; 
  
  // Set of variables known to be published by the helm
  std::set<std::string>   m_set_helmvars; 
  
  
  std::set<std::string>   m_observed_behaviors;
  std::list<ScopeEntry>   m_bhv_warnings;
  std::vector<StringTree> m_mode_trees;
  
  std::map<std::string, ScopeEntry> m_map_posts;

  HelmReport       m_helm_report;
  LifeEventHistory m_life_event_history;

  std::string  m_helm_engaged_primary;
  std::string  m_helm_engaged_standby;
  std::string  m_display_mode;
  std::string  m_community;

  std::string  m_term_server_host;
  std::string  m_term_server_port;

  bool         m_paused;
  bool         m_display_truncate;
  bool         m_concise_bhv_list;
  bool         m_display_posts;

   // Settings for the XMS Report
  bool         m_display_xms;
  bool         m_display_virgins;
  bool         m_display_statevars;
  bool         m_ignore_filevars;

  bool         m_update_pending;
  bool         m_updates_throttled;

  unsigned int m_total_warning_cnt;
  unsigned int m_iteration_helm;      

  double       m_db_start_time;

  std::string  m_ivpdomain;
};
#endif 








