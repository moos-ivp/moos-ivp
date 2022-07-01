/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: MarinePID.h                                          */
/*    DATE: April 10 2006                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#ifndef MARINE_PID_HEADER
#define MARINE_PID_HEADER

#include <string>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "PIDEngine.h"

class MarinePID : public AppCastingMOOSApp
{
public:
  MarinePID();
  virtual ~MarinePID() {}
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void registerVariables();
  bool buildReport();

  void postCharStatus();
  void postPenginePostings();
  void postPengineResults();

protected:
  PIDEngine m_pengine;
  bool      m_pid_ignore_nav_yaw;
  bool      m_pid_allstop_posted;
  bool      m_pid_verbose;
  double    m_pid_ok_skew;
};
#endif 





