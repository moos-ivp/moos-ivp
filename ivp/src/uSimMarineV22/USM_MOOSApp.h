/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: USM_MOOSApp.h                                        */
/*    DATE: Oct 25th 2004                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#ifndef USM_MOOSAPP_HEADER
#define USM_MOOSAPP_HEADER

#include <string>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "USM_Model.h"
#include "PIDEngine.h"
#include "WormHoleSet.h"

class USM_MOOSApp : public AppCastingMOOSApp
{
public:
  USM_MOOSApp();
  virtual ~USM_MOOSApp() {}

 public: // Standard MOOSApp functions to overload
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool OnStartUp();
  bool Iterate();
  bool OnConnectToServer();
  bool buildReport();
  void registerVariables();

 protected: // Simulator functions
  void postNodeRecordUpdate(std::string, const NodeRecord&);
  void postWormHolePolys();
  void postWindModelVisuals();
  void handleBuoyancyAndTrim(NodeRecord);
  void handleMailBuoyancyControl(std::string);
  void handleMailTrimControl(std::string);

 protected: // PID Controller functions
  void postPenginePostings();
  void postPengineResults();

protected: // Utility function(s)
  std::list<std::string> removeStr(std::string);
  void applyWormHoles();
  
 protected:
  // SIM variables
  std::string  m_sim_prefix;
  USM_Model    m_model;
  bool         m_enabled;
  
  bool    m_buoyancy_requested;      // Buoyancy/Trim HS 2012-07-22
  bool    m_trim_requested;
  double  m_buoyancy_request_time;
  double  m_trim_request_time;
  double  m_buoyancy_delay;
  double  m_max_trim_delay;
  double  m_last_report;
  double  m_report_interval;
  double  m_pitch_tolerance;

  WormHoleSet  m_wormset;

  // PID variables
  PIDEngine m_pengine;
  bool      m_pid_ignore_nav_yaw;
  bool      m_pid_allstop_posted;
  bool      m_pid_verbose;
  double    m_pid_ok_skew;

  // Variables unique to SIM/PID coupling
  bool         m_pid_coupled;
  unsigned int m_nav_modulo;

};

#endif





