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
  SSV_MOOSApp() {m_left_click_ix=-1; m_right_click_ix=-1;};
  virtual ~SSV_MOOSApp() {};

  void setGUI(SSV_GUI* g_gui) {m_gui=g_gui;};

  bool Iterate();

  // virtual overide of base class CMOOSApp member.
  // Here we register for data we wish be informed about
  bool OnConnectToServer();

  bool OnStartUp();
  bool OnNewMail(MOOSMSG_LIST &NewMail);

 protected:
  void receiveVehicleState(CMOOSMsg &Msg);
  bool receiveAIS_REPORT(CMOOSMsg &Msg);
  bool receiveGRID_CONFIG(CMOOSMsg &Msg);
  void receiveGRID_DELTA(CMOOSMsg &Msg);
  bool receivePolygon(CMOOSMsg &Msg);
  bool receiveSegList(CMOOSMsg &Msg);
  bool receivePoint(CMOOSMsg &Msg);
  bool receiveStationCircle(CMOOSMsg &Msg);

  bool handleContactList(std::string);
  bool handleMarker(std::string, std::string);

  void handlePendingGUI();

 protected:
  SSV_GUI* m_gui;

  double m_start_time;
  
  int m_left_click_ix;
  int m_right_click_ix;

};

#endif 







