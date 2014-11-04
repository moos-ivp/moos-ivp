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
  
  void setPostThrustModeDif(bool v) {m_post_thrust_mode_dif=v;};
  void setPostThrustModeRev(bool v) {m_post_thrust_mode_rev=v;};

 protected:
  void postHelp();

 protected:
  
  double m_desired_thrust_l;
  double m_desired_thrust_r;
  
  double m_desired_thrust_l_prev;
  double m_desired_thrust_r_prev;

  double m_desired_thrust_l_observed;
  double m_desired_thrust_r_observed;

  double m_desired_thrust_l_observed_prev;
  double m_desired_thrust_r_observed_prev;
  
  std::string m_thrust_mode_rev_observed;
  std::string m_thrust_mode_dif_observed;


  double m_last_input_tstamp;

  bool   m_allstop_prev;
  bool   m_warning_prev;
  bool   m_allstop_commanded;
  bool   m_continued;
  bool   m_thrust_mode_mail;

  bool   m_first_iteration; 

  bool   m_post_thrust_mode_dif;
  bool   m_post_thrust_mode_rev;

public:
  CMOOSLock  m_tc_mutex;

};

#endif