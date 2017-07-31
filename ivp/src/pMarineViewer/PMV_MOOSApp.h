/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PMV_MOOSApp.h                                        */
/*    DATE: September 2003                                       */
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

#ifndef MOOS_VEHICLE_STATE_HEADER
#define MOOS_VEHICLE_STATE_HEADER

#include <string>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "PMV_GUI.h"
#include "Threadsafe_pipe.h"
#include "MOOS_event.h"

class PMV_MOOSApp : public AppCastingMOOSApp  
{
 public:
  PMV_MOOSApp();
  virtual ~PMV_MOOSApp() {}

  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  bool OnNewMail(MOOSMSG_LIST &NewMail);

  void setGUI(PMV_GUI* g_gui)            {m_gui=g_gui;}  
  void setAppCastRepo(AppCastRepo* repo) {m_appcast_repo=repo;}
  void setPendingEventsPipe(Threadsafe_pipe<MOOS_event>*); 

  // Only call these methods in the main FLTK l thread, for thread
  // safety w.r.t. that  library...
  void handleNewMail(const MOOS_event & e);
  void handleIterate(const MOOS_event & e);
  void handleStartUp(const MOOS_event & e);
  void handleAppCastRequesting(bool force=false);

 protected:
  bool buildReport();
  void handlePendingGUI();
  void registerVariables();
  void postConnectionPairs();
  void postAppCastRequest(std::string node, std::string app,
			  std::string key,  std::string thresh,
			  double duration);

  std::string getContextKey(std::string);
  bool handleMailClear(std::string);
  bool handleConfigCmd(std::string);

  void handlePendingPostsFromGUI();
  void handlePendingCommandSummary();
  
 protected:
  Threadsafe_pipe<MOOS_event> *m_pending_moos_events;

  PMV_GUI     *m_gui;
  double       m_lastredraw_time;
  bool         m_verbose;
  bool         m_pending_pairs;

  std::vector<std::string>  m_node_report_vars;
  std::vector<std::string>  m_scope_vars;
  std::vector<VarDataPair>  m_connection_pairs; 

  AppCastRepo *m_appcast_repo;
  double       m_appcast_last_req_time;
  double       m_appcast_request_interval;

  unsigned int m_node_reports_received;
  unsigned int m_clear_geoshapes_received;
  int          m_node_report_index;
  bool         m_log_the_image;

  CommandFolio   m_cmd_folio;
  CommandSummary m_cmd_summary;
};

#endif 









