/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: UMV_MOOSApp.h                                        */
/*    DATE: Aug 11th, 2012                                       */
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

#ifndef UMV_MOOS_APP_HEADER
#define UMV_MOOS_APP_HEADER

#include <string>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "UMV_GUI.h"
#include "Threadsafe_pipe.h"
#include "VarDataPair.h"
#include "AppCastRepo.h"
#include "RealmRepo.h"
#include "MOOS_event.h"

class UMV_MOOSApp : public AppCastingMOOSApp  
{
 public:
  UMV_MOOSApp();
  virtual ~UMV_MOOSApp() {}

  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  bool OnNewMail(MOOSMSG_LIST &NewMail);

  bool buildReport();

  void setGUI(UMV_GUI* g_gui)            {m_gui=g_gui;}
  void setAppCastRepo(AppCastRepo* repo) {m_appcast_repo=repo;}
  void setRealmRepo(RealmRepo* repo)     {m_realm_repo=repo;}
  void setPendingEventsPipe(Threadsafe_pipe<MOOS_event>*); 
  
  // Only call these methods in the main FLTK l thread, for thread
  // safety w.r.t. that library...
  void handleNewMail(const MOOS_event & e);
  void handleIterate(const MOOS_event & e);
  void handleStartUp(const MOOS_event & e);
  void handleRealmCastRequesting();
  bool handleConfigWatchCluster(std::string);
  
 protected:
  void registerVariables();
  void postAppCastRequest(std::string node, std::string app,
			  std::string key,  std::string thresh,
			  double duration);
  
 protected:
  Threadsafe_pipe<MOOS_event>* m_pending_moos_events;

  UMV_GUI*     m_gui;

  AppCastRepo *m_appcast_repo;
  double       m_appcast_last_req_time;
  double       m_appcast_request_interval;

  RealmRepo   *m_realm_repo;
  double       m_relcast_last_req_time;
  double       m_relcast_request_interval;
  std::string  m_relcast_client_name;
  
};

#endif 









