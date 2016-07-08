/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: UCMD_MOOSApp.h                                       */
/*    DATE: July 1st, 2016                                       */
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

#ifndef UCMD_MOOS_APP_HEADER
#define UCMD_MOOS_APP_HEADER

#include <string>
#include <list>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "UCMD_GUI.h"
#include "Threadsafe_pipe.h"
#include "VarDataPair.h"
#include "CommandFolio.h"
#include "MOOS_event.h"

class UCMD_MOOSApp : public AppCastingMOOSApp  
{
 public:
  UCMD_MOOSApp();
  virtual ~UCMD_MOOSApp() {}

  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  bool OnNewMail(MOOSMSG_LIST &NewMail);

  bool buildReport();

  void setGUI(UCMD_GUI* g_gui)           {m_gui=g_gui;}

  void setPendingEventsPipe(Threadsafe_pipe<MOOS_event>*); 
  

  // Only call these methods in the main FLTK l thread, for thread
  // safety w.r.t. that library...
  void handleNewMail(const MOOS_event & e);
  void handleIterate(const MOOS_event & e);
  void handleStartUp(const MOOS_event & e);

 protected:
  void handlePendingGUI();
  void handlePendingHistory();
  void registerVariables();
  bool handleConfigCmd(std::string);

  void postAppCastRequest(std::string node, std::string app,
			  std::string key,  std::string thresh,
			  double duration);
  
 protected:
  Threadsafe_pipe<MOOS_event>* m_pending_moos_events;

  CommandFolio  m_cmd_folio;
  
  UCMD_GUI*     m_gui;

  std::list<std::string>      m_post_vars;
  std::list<std::string>      m_post_vals;
  std::list<std::string>      m_post_pids;
  std::map<std::string, bool> m_post_acks;

  unsigned int m_posting_count;

  bool         m_gui_history_pending;
};

#endif 




