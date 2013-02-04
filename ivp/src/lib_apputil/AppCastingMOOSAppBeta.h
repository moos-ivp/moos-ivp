/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AppCastingMOOSApp.h                                  */
/*    DATE: June 5th 2012                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifndef APPCASTING_MOOS_APP_HEADER
#define APPCASTING_MOOS_APP_HEADER

#include <sstream>
#include <iostream>
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
  
 protected:
  bool         OnStartUpDirectives(std::string directives="");
  void         RegisterVariables();
  void         PostReport(const std::string& directive="");
  void         reportEvent(const std::string&);
  void         reportConfigWarning(const std::string&);
  void         reportUnhandledConfigWarning(const std::string&);
  void         reportRunWarning(const std::string&);
  void         retractRunWarning(const std::string&);
  unsigned int getWarningCount(const std::string&) const;

  void         Notify(const std::string&, const std::string&, double time=-1);
  void         Notify(const std::string&, const std::string&, const std::string&, double time=-1);
  void         Notify(const std::string&, double, double time=-1);
  void         Notify(const std::string&, double, const std::string&, double time=-1);

 private:
  void         handleMailAppCastRequest(const std::string&);
  bool         appcastRequested();

  //-------------------------------------------------------------------
  void         pushPub(const std::string& var, const std::string& val);
  void         pushMail(const CMOOSMsg& msg);
  void         buildScopeReport();
  //-------------------------------------------------------------------

protected:
  unsigned int m_iteration;
  double       m_curr_time;
  double       m_start_time;
  double       m_time_warp;
  double       m_last_report_time;
  double       m_term_report_interval;
  bool         m_term_reporting;

  std::stringstream m_msgs;

  AppCast      m_ac;
  std::string  m_host_community;

 private:
  double       m_last_report_time_appcast;
  bool         m_new_run_warning;
  bool         m_new_cfg_warning;

  // Map from KEY (AC requestor) to config param.
  std::map<std::string, double>       m_map_bcast_duration;
  std::map<std::string, double>       m_map_bcast_tstart;
  std::map<std::string, std::string>  m_map_bcast_thresh;  

  // ----------------------------------------------------
  std::list<std::string> m_pub_vars;
  std::list<std::string> m_pub_vals;

  std::list<std::string> m_mail_vars;
  std::list<std::string> m_mail_vals;
  std::list<std::string> m_mail_srcs;

  unsigned int m_mail_ctr;
  bool m_scoping;

};
#endif
