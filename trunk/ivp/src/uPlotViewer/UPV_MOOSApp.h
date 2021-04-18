/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: UPV_MOOSApp.h                                        */
/*    DATE: May 18th, 2012                                       */
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

#ifndef UPV_MOOS_APP_HEADER
#define UPV_MOOS_APP_HEADER

#include <string>
#include "MOOS/libMOOS/MOOSLib.h"
#include "UPV_GUI.h"
#include "Threadsafe_pipe.h"
#include "VarDataPair.h"
#include "MOOS_event.h"

class UPV_MOOSApp : public CMOOSApp  
{
 public:
  UPV_MOOSApp();
  virtual ~UPV_MOOSApp() {}

  void setGUI(UPV_GUI* g_gui) {m_gui=g_gui;}
  
  void setPendingEventsPipe(Threadsafe_pipe<MOOS_event> 
			    *pending_moos_events) 
  {
    m_pending_moos_events = pending_moos_events;
  }
  
  bool Iterate();
  
  // virtual overide of base class CMOOSApp member.
  // Here we register for data we wish be informed about
  bool OnConnectToServer();
  
  bool OnStartUp();
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  
  // Only call these methods in the main FLTK l thread, for thread
  // safety w.r.t. that  library...
  void handleNewMail(const MOOS_event & e);
  void handleIterate(const MOOS_event & e);
  void handleStartUp(const MOOS_event & e);
  
 protected:
  void registerVariables();
  
 protected:
  UPV_GUI* m_gui;
  Threadsafe_pipe<MOOS_event> * m_pending_moos_events;

  std::vector<std::string> m_plot_vars;
};

#endif 








