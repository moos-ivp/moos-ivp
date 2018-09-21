/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XMS.h                                                */
/*    DATE: Mar 23rd 2006                                        */
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

#ifndef XMS_HEADER
#define XMS_HEADER

#include <vector>
#include <string>
#include <set>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "ScopeEntry.h"

class XMS : public AppCastingMOOSApp
{
 public:
  XMS();
  virtual ~XMS() {}
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
  void handleCommand(char);
  bool addVariables(std::string);
  bool addVariable(std::string, bool histvar=false);
  bool addSources(std::string);
  bool addSource(std::string);
  void setFilter(std::string);
  bool setContentMode(std::string, bool=false);
  bool setRefreshMode(std::string);
  void setDisplayColumns(std::string);
  void setTruncData(std::string);

  void ignoreFileVars(bool v)           {m_ignore_file_vars = v;}
  void setDispVirgins(bool v)           {m_display_virgins = v;}
  void setDispTime(bool v)              {m_display_time = v;}
  void setDispSource(bool v)            {m_display_source = v;}
  void setDispAuxSource(bool v)         {m_display_aux_source = v;}
  void setDispCommunity(bool v)         {m_display_community = v;}
  void setDispOwnCommunity(bool v)      {m_display_own_community = v;}
  void setDispAll(bool v)               {m_display_all = v;}
  void setDispAllReally(bool v)         {m_display_all_really = v;}
  void setTermServerHost(std::string s) {m_term_server_host = s;}
  void setTermServerPort(std::string s) {m_term_server_port = s;}
  void setAppNameNoIndex(std::string s) {m_app_name_noindex = s;}
  void setTermReportInterval(std::string);
  
  bool setColorMapping(std::string var, std::string color="any");
  bool setColorMappingsAny(std::string);
  bool setColorMappingsPairs(std::string);

 protected:
  void registerVariables();

  bool buildReport();
  bool printHelp();
  bool printReport();
  bool printHistoryReport();
  bool printProcsReport();
  
  void updateDBClients(std::string);
  void updateVariable(CMOOSMsg& msg);
  bool updateVarEntry(std::string, const ScopeEntry&);
  void updateHistory(std::string, std::string, std::string, double);
  void updateSourceInfo(std::string);
 
  void handleSelectMaskSubSources();
  void handleSelectMaskAddSources();
  
  void refreshProcVarsList();
  void cacheColorMap();
  bool colorTaken(std::string);

  std::string getColorMapping(std::string);

  // An overloading of the CMOOSApp ConfigureComms function
  bool ConfigureComms();

 protected:
  // Map from MOOS var name to last-message components
  std::map<std::string, ScopeEntry>  m_map_var_entries;
  std::map<std::string, std::string> m_map_var_entries_color;

  // A shadow list to hold when otherwise scoping on all variables
  std::map<std::string, ScopeEntry>  m_map_orig_var_entries;
  std::map<std::string, std::string> m_map_orig_var_entries_color;

  // Map from source to alive indicators (msg rcvd and db_clients)
  std::map<std::string, double>      m_map_src_update;
  std::map<std::string, double>      m_map_src_dbclient;

  // Map from source to id and id to source
  std::map<std::string, std::string> m_map_src_id;
  std::map<std::string, std::string> m_map_id_src;
  unsigned int     m_max_proc_name_len;
  unsigned int     m_variable_id_srcs;
  unsigned int     m_proc_watch_count;
  std::string      m_proc_watch_summary;

  std::set<std::string> m_mask_sub_sources;
  std::set<std::string> m_mask_add_sources;

  // Map from source to source _STATUS string
  std::map<std::string, std::string> m_map_src_status;

  bool   m_update_requested;

  bool   m_history_event;
  bool   m_display_source;
  bool   m_display_aux_source;
  bool   m_display_time;
  bool   m_display_community;
  bool   m_display_own_community;
  bool   m_display_hist_var;
  std::string m_refresh_mode;
  std::string m_content_mode;
  std::string m_content_mode_prev;
  std::string m_community;

  double m_trunc_data;
  double m_trunc_data_start;
  bool   m_display_virgins;
  
  bool   m_ignore_file_vars;
  double m_db_start_time;
  
  std::string m_filter;
  std::string m_term_server_host;
  std::string m_term_server_port;

  std::string             m_history_var;
  std::list<std::string>  m_history_list;
  std::list<std::string>  m_history_sources;
  std::list<std::string>  m_history_sources_aux;
  std::list<int>          m_history_counts;
  std::list<double>       m_history_times;
  unsigned int            m_history_length;

  bool   m_display_all;
  bool   m_display_all_requested;
  bool   m_display_all_really;
  double m_last_all_refresh;   
  bool   m_suppress_appcasts;
  bool   m_help_is_showing;

  std::map<std::string, std::string> m_color_map;

  std::string m_app_name_noindex;
};
#endif 








