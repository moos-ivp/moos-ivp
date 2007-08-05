/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TC_MOOSApp.h                                         */
/*    DATE: July 17th 2007                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef TRAFFIC_CONTROLLER_HEADER
#define TRAFFIC_CONTROLLER_HEADER

#include "MOOSLib.h"
#include "TrajectorySet.h"

class TC_MOOSApp : public CMOOSApp
{
public:
  TC_MOOSApp();
  virtual ~TC_MOOSApp() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

protected:

  void handleConfig(const std::string &, const std::string&);
  void makeTSet();
  void postTSet();

protected:
  double  m_last_reset_time;
  double  m_wait_period;
  bool    m_reset_request;
  bool    m_paused;
  double  m_paused_time_total;
  double  m_paused_time_start;

  std::vector<std::string> m_vehicles;
  TrajectorySet            m_trajectory_set;

  double  m_center_x;
  double  m_center_y;
  double  m_radius;
  double  m_variance;
  double  m_speed;
};

#endif




