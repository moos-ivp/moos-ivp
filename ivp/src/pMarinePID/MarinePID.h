/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MarinePID.h                                          */
/*    DATE: April 10 2006                                        */
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

#ifndef MARINE_PID_HEADER
#define MARINE_PID_HEADER

#include <string>
#include "MOOSLib.h"
#include "PIDEngine.h"

class MarinePID : public CMOOSApp
{
public:
  MarinePID();
  virtual ~MarinePID() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void postCharStatus();
  void registerVariables();
  bool handleYawSettings();
  bool handleSpeedSettings();
  bool handleDepthSettings();

protected:
  bool     has_control;
  bool     allow_overide;
  bool     ylog_flag;
  double   speed_factor;

  double   current_heading;
  double   current_speed;
  double   current_depth;
  double   current_pitch;

  double   desired_heading;
  double   desired_speed;
  double   desired_depth;
  double   current_thrust;

  double   max_pitch;
  double   max_rudder;
  double   max_thrust;
  double   max_elevator;

  PIDEngine     pengine;
  std::string   ownship;
  std::string   verbose;

  int     iteration;
  double  start_time;
  bool    depth_control;
  bool    paused;

  double  time_of_last_helm_msg;
  double  time_of_last_nav_msg;
};

#endif 









