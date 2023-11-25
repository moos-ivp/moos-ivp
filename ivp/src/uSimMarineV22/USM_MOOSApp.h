/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: USM_MOOSApp.h                                        */
/*    DATE: Oct 25th 2004                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
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

  // Optional post of DES_* in pid_coupled mode
  std::string  m_post_des_thrust;
  std::string  m_post_des_rudder;
};

#endif






