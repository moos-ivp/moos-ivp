/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AppCastMonitor.h                                     */
/*    DATE: June 4th 2012                                        */
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

#ifndef APPCAST_MONITOR_HEADER
#define APPCAST_MONITOR_HEADER

#include <string>
#include <map>
#include "MOOS/libMOOS/MOOSLib.h"
#include "AppCastRepo.h"

class AppCastMonitor : public CMOOSApp
{
 public:
  AppCastMonitor();
  ~AppCastMonitor() {}
  
  void handleCommand(char);
  void setTerseMode(bool v) {m_terse_mode=v;}

  void setInitialProc(std::string s) {m_initial_proc=s;}
  void setInitialNode(std::string s) {m_initial_node=s;}
  void setRefreshPaused() {m_refresh_mode = "paused";}
  
 protected:
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void RegisterVariables();

 protected:
  bool handleMailAppCast(const std::string&);
  bool handleMailMissionHash(const std::string&);
  void handleSelectNode(std::string);
  void handleSelectChannel(std::string);
  void postAppCastRequest(std::string node, std::string app,
			  std::string key,  std::string thresh,
			  double duration);

  bool setCurrentNode(std::string s)   {return(m_repo.setCurrentNode(s));}
  bool setCurrentProc(std::string s)   {return(m_repo.setCurrentProc(s));}
  bool switchContentMode(const std::string& s="revert");

  void printHelp();
  void printReportNodes();
  void printReportProcs();
  void printReportAppCast();
  void printHeader();

  std::string currentNode() const;
  std::string currentProc() const;

  void trySetInitialNodeProc();
  
 private: // Configuration variables

  std::string  m_refresh_mode;      // paused,events,streaming
  std::string  m_content_mode;      // help,nodes,procs,appcast 
  std::string  m_content_mode_prev;

  bool         m_terse_mode;

  std::string  m_initial_node;
  std::string  m_initial_proc;
  
 private: // State variables
  unsigned int m_term_reports;
  unsigned int m_iteration;
  double       m_timewarp;  
  double       m_curr_time;
  double       m_last_report_time;
  double       m_term_report_interval;
  bool         m_update_pending;
  
  AppCastRepo  m_repo;

  std::string  m_mission_hash;
};

#endif 

