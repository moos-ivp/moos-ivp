/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HeadingMonitor.cpp                                   */
/*    DATE: June 25th 2005                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef HEADING_MONITOR_HEADER
#define HEADING_MONITOR_HEADER

#include <string>
#include "MOOSLib.h"

class HeadingMonitor : public CMOOSInstrument
{
public:
  HeadingMonitor();
  virtual ~HeadingMonitor() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

protected:
  double heading_curr;
  double heading_delta;
  double heading_rate;
  bool   heading_fresh;
  double heading_new;

  double curr_rudder;
  double time_thresh;
  double time_reset;
  double time_first;
  double curr_gps_speed;

  bool   track_fresh;

  std::string heading_source;

};

#endif





