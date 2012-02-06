/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ULV_MOOSApp.h                                        */
/*    DATE:                                                      */
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

#ifndef ULV_MOOS_APP_HEADER
#define ULV_MOOS_APP_HEADER

#include <string>
#include "MOOSLib.h"
#include "ULV_GUI.h"
#include "Threadsafe_pipe.h"
#include "VarDataPair.h"
#include "MOOS_event.h"

class ULV_MOOSApp : public CMOOSApp  
{
 public:
  ULV_MOOSApp();
  virtual ~ULV_MOOSApp() {};

  void setGUI(ULV_GUI* g_gui) {m_gui=g_gui;};
  
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
  void handlePendingGUI();
  void registerVariables();
  void postConnectionPairs();

 protected:
  ULV_GUI* m_gui;
  Threadsafe_pipe<MOOS_event> * m_pending_moos_events;

  double m_start_time;
  double m_lastredraw_time;
  bool   m_verbose;
  bool   m_pending_pairs;
  int    m_counter;

  std::vector<VarDataPair> m_connection_pairs; 
};

#endif 
