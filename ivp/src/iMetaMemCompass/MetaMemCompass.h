/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MetaMemCompass.cpp                                   */
/*    DATE: Oct 3rd 2007                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef META_MEM_COMPASS_HEADER
#define META_MEM_COMPASS_HEADER

#include "MOOSLib.h"

class MetaMemCompass : public CMOOSApp
{
public:
  MetaMemCompass();
  virtual ~MetaMemCompass() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

protected:

  double gpsAge();
  double compassAge();

  void   addHeading(double, double);
  bool   getHeadingAvg(double&);

  std::string determineSource();

protected:
  double curr_gps_speed;
  double curr_gps_heading;
  double curr_compass_heading;
  double curr_thrust;
  double curr_delta;
  double curr_total_delta;
  double curr_total_compares;
  double speed_thresh;

  double time_stamp_gps;
  double time_stamp_compass;

  std::list<double> m_heading_val;
  std::list<double> m_heading_time;
  double m_memory_time;

  double m_age_stale;
};

#endif




