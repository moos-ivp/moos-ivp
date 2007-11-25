/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MetaCompass.cpp                                      */
/*    DATE: June 14th 2005                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef META_COMPASS_HEADER
#define META_COMPASS_HEADER

#include "MOOSLib.h"

class MetaCompass : public CMOOSApp
{
public:
  MetaCompass();
  virtual ~MetaCompass() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

protected:
  double curr_gps_speed;
  double curr_gps_heading;
  double curr_compass_heading;
  double curr_thrust;
  double curr_delta;
  double curr_total_delta;
  double curr_total_compares;
  double speed_thresh;

};

#endif





