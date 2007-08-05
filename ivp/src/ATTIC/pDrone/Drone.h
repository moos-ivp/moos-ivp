/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Drone.h                                              */
/*    DATE: March 2005                                           */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef DRONE_HEADER
#define DRONE_HEADER

#include "MOOSLib.h"

class Drone : public CMOOSApp
{
 public:
  Drone();
  virtual ~Drone() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

protected:
  int    counter;
  double last_time;
  bool   first_notify;
};

#endif




