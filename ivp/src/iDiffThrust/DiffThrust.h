/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: DiffThrust.h                                         */
/*   DATE: Sep 14th 2014                                        */
/****************************************************************/

#ifndef DIFF_THRUST_HEADER
#define DIFF_THRUST_HEADER

#include "MOOS/libMOOS/App/MOOSApp.h"

class DiffThrust : public CMOOSApp
{
 public:
  DiffThrust();
  virtual ~DiffThrust() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void RegisterVariables();

  bool handleCharInput(char);
  
 protected:
  
  double m_desired_thrust_l;
  double m_desired_thrust_r;
  
  double m_desired_thrust_l_prev;
  double m_desired_thrust_r_prev;

  double m_desired_thrust_l_observed;
  double m_desired_thrust_r_observed;

  double m_last_input_tstamp;

public:
  CMOOSLock  m_tc_mutex;

};

#endif
