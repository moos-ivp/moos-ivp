/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PMV_MOOSApp.h                                        */
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

#ifndef MOOS_VEHICLE_STATE_HEADER
#define MOOS_VEHICLE_STATE_HEADER

#include <string>
#include "MOOSLib.h"
#include "PMV_GUI.h"

class PMV_MOOSApp : public CMOOSApp  
{
 public:
  PMV_MOOSApp();
  virtual ~PMV_MOOSApp() {};

  void setGUI(PMV_GUI* g_gui) {m_gui=g_gui;};

  bool Iterate();

  // virtual overide of base class CMOOSApp member.
  // Here we register for data we wish be informed about
  bool OnConnectToServer();

  bool OnStartUp();
  bool OnNewMail(MOOSMSG_LIST &NewMail);

 protected:
  void receiveVehicleState(CMOOSMsg &Msg);
  bool receivePK_SOL(CMOOSMsg &Msg);
  void registerVariables();

 protected:
  PMV_GUI* m_gui;

  double m_start_time;
  bool   m_verbose;
  int    m_counter;

  std::string  m_left_click_str;
  std::string  m_right_click_str;
};

#endif 








