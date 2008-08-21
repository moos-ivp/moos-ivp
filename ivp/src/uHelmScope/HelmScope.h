/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HelmScope.h                                          */
/*    DATE: Apr 12th 2008                                        */
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

#ifndef HELM_SCOPE_HEADER
#define HELM_SCOPE_HEADER

#include <vector>
#include <string>
#include <map>
#include <list>

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "IterBlockHelm.h"
#include "IterBlockPosts.h"
#include "IterBlockXMS.h"

class HelmScope : public CMOOSApp  
{
 public:
  HelmScope();
  virtual ~HelmScope() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
  void handleCommand(char);
  void setPaused(bool v)         {m_paused = v;};
  void setDisplayXMS(bool v)     {m_display_xms = v;};
  void setDisplayPosts(bool v)   {m_display_posts = v;};
  void setIgnoreFileVars(bool v) {m_ignore_filevars = v;};
  void setDisplayTrunc(bool v)   {m_display_truncate = v;};
  void setDisplayVirgins(bool v) {m_display_virgins = v;};

#if 0 // modification in progress 
  bool configureComms(std::string, std::string, double, std::string);
  void setAppTick(double v)      {SetAppFreq(v);};
  void setServer(const char *s, long v) {SetServer(s,v);};
#endif

 public:  // Public Functions for basic DB Scoping
  void addVariables(const std::string&, const std::string&);
  void addVariable(const std::string&, const std::string&);

 protected:
  void registerVariables();
  void handleNewIvPDomain(const std::string&);
  void handleNewHelmSummary(const std::string&);
  void handleNewHelmPostings(const std::string&);
  void handleNewStateVars(const std::string&);
  void handleNewIterXMS();
  void pruneHistory();
  
 protected: // Protected Functions for basic DB Scoping
  void updateVariable(CMOOSMsg& msg);
  void updateVarVal(const std::string&, const std::string&);
  void updateVarType(const std::string&, const std::string&);
  void updateVarSource(const std::string&, const std::string&);
  void updateVarTime(const std::string&, const std::string&);
  void updateVarCommunity(const std::string&, const std::string&);

  void printHelp();
  void printReport();
  void printHelmReport(int ix);
  void printDBReport(int);
  void printPostingReport(int ix);
  
 protected:   
  std::map<int, IterBlockHelm>  m_blocks_helm;
  std::map<int, IterBlockPosts> m_blocks_posts;
  std::map<int, IterBlockXMS>   m_blocks_xms;

  bool   m_display_help;
  bool   m_paused;
  bool   m_display_truncate;
  bool   m_concise_bhv_list;

  // Settings for the Postings Report
  bool   m_display_posts;
  bool   m_display_msgs_pc;    // PC_BHV_FOOBAR posts
  bool   m_display_msgs_view;  // VIEW_FOOBAR posts
  bool   m_display_msgs_pwt;   // PWT_BHV_FOOBAR posts
  bool   m_display_msgs_upd;   // UH_BHV_FOOBAR posts
  bool   m_display_msgs_state; // STATE_BHV_FOOBAR posts

  // Settings for the XMS Report
  bool   m_display_xms;
  bool   m_display_virgins;
  bool   m_display_statevars;
  bool   m_ignore_filevars;

  bool   m_update_requested;

  int    m_moosapp_iter;
  int    m_warning_count;

  // The last helm index received
  int    m_iteration_helm;      
  int    m_iter_last_post;
  int    m_iter_next_post;

  int    m_history_size_max;
  int    m_history_last_cut;

  double m_db_uptime;
  double m_max_time_loop;
  double m_max_time_create;
  double m_max_time_solve;

  int    m_last_iter_recd;
  double m_last_iter_time;
  std::list<double> m_helm_intervals;
  int    m_interval_samples_a;
  int    m_interval_samples_b;
  
  std::string m_ivpdomain;

 protected:  // Structures for basic DB Scoping 
  std::vector<std::string> m_var_names;
  std::vector<std::string> m_var_vals;
  std::vector<std::string> m_var_type;
  std::vector<std::string> m_var_source;
  std::vector<std::string> m_var_time;
  std::vector<std::string> m_var_community;
  std::vector<std::string> m_var_category;
};
#endif 






