/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: SSV_MOOSApp.h                                        */
/*    DATE: September 11th, 2007                                 */
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

#ifndef MOOS_VEHICLE_STATE_HEADER
#define MOOS_VEHICLE_STATE_HEADER

#include <string>
#include "MOOSLib.h"
#include "SSV_GUI.h"

class SSV_MOOSApp : public CMOOSApp  
{
 public:
  SSV_MOOSApp();
  virtual ~SSV_MOOSApp() {};

  void setGUI(SSV_GUI* g_gui) {m_gui=g_gui;};

  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  bool OnNewMail(MOOSMSG_LIST &NewMail);

 protected:
  void handlePendingGUI();
  void registerVariables();

 protected:
  SSV_GUI* m_gui;

  double m_start_time;
  
  // We hold a local copy of last click-string to repeatedly compare
  // against the currently generated one and only post when different
  std::string  m_left_click_str;
  std::string  m_right_click_str;
};

#endif 







