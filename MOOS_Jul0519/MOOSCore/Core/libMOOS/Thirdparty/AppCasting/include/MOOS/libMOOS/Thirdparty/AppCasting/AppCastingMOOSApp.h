///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of
//   Applications and Libraries for Mobile Robotics Research
//   Copyright (C) Paul Newman
//
//   This software was written by Paul Newman at MIT 2001-2002 and
//   the University of Oxford 2003-2013
//
//   email: pnewman@robots.ox.ac.uk.
//
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Lesser Public License v2.1
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/lgpl.txt
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AppCastingMOOSApp.h                                  */
/*    DATE: June 5th 2012                                        */
/*                                                               */

#ifndef APPCASTING_MOOS_APP_HEADER
#define APPCASTING_MOOS_APP_HEADER

#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "MOOS/libMOOS/App/MOOSApp.h"
#include "AppCast.h"

class AppCastingMOOSApp : public CMOOSApp
{
public:
  AppCastingMOOSApp();
  virtual ~AppCastingMOOSApp() {};

  virtual bool Iterate();
  virtual bool OnNewMail(MOOSMSG_LIST&);
  virtual bool OnStartUp();
  virtual bool buildReport() {return(false);};
  virtual bool deprecated()  {return(false);}
  
 protected:
  void         RegisterVariables();
  void         PostReport(const std::string& directive="");
  void         reportEvent(const std::string&);
  void         reportConfigWarning(const std::string&);
  void         reportUnhandledConfigWarning(const std::string&);
  bool         reportRunWarning(const std::string&);
  void         retractRunWarning(const std::string&);
  unsigned int getWarningCount(const std::string&) const;
  void 	       preOnStartUp();
  bool 	       OnStartUpDirectives(std::string directives="");
  void         setAppLoggingInfo(std::string s) {m_app_logging_info=s;}
  std::string  commsPolicy() const {return(m_comms_policy);}
  
 private:
  void         handleMailAppCastRequest(const std::string&);
  bool         handleMailCommsPolicy(const std::string&);
  bool         appcastRequested();

protected:
  unsigned int m_iteration;
  double       m_curr_time;
  double       m_start_time;
  double       m_time_warp;
  double       m_last_iterate_time;
  double       m_iterate_start_time;
  double       m_last_report_time;
  double       m_term_report_interval;
  bool         m_term_reporting;
  bool         m_deprecated_ok;
  std::string  m_deprecated_alt;
  
  std::stringstream m_msgs;
  std::stringstream m_cout;
  
  AppCast      m_ac;
  std::string  m_host_community;

 private:
  double       m_last_report_time_appcast;
  bool         m_new_run_warning;
  bool         m_new_cfg_warning;

  std::ofstream m_outfile;
  std::string  m_app_logging;
  std::string  m_app_logging_info;

  std::string  m_comms_policy;
  std::string  m_comms_policy_config;
  
  // Map from KEY (AC requestor) to config param.
  std::map<std::string, double>       m_map_bcast_duration;
  std::map<std::string, double>       m_map_bcast_tstart;
  std::map<std::string, std::string>  m_map_bcast_thresh;  
};
#endif
