/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ProcessWatch.h                                       */
/*    DATE: May 27th 2007 (MINUS-07)                             */
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

#ifndef PROCESS_WATCH_HEADER
#define PROCESS_WATCH_HEADER

#include <string>
#include <vector>
#include <map>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class ProcessWatch : public AppCastingMOOSApp
{
public:
  ProcessWatch();
  virtual ~ProcessWatch() {}

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

  void registerVariables();
  bool buildReport();

protected:
  void handleMailNewDBClients();
  void handleMailStatusUpdate(std::string);

  bool handleConfigWatchList(std::string);
  bool handleConfigWatchItem(std::string);

  bool handleConfigExcludeList(std::string);
  bool handleConfigExcludeItem(std::string);

 protected:
  bool addToWatchList(std::string);

  void checkForIndividualUpdates();
  void postFullSummary();

  bool isAlive(std::string);
  void procNotedHere(std::string);
  void procNotedGone(std::string);
  void procNotedExcused(std::string);

  void handlePostMapping(std::string);
  std::string postVar(std::string);

  void populateAntlerList();

  bool processIncluded(const std::string& procname);
  bool processExcluded(const std::string& procname);

 protected: // State Variables
  bool         m_proc_watch_summary_changed;
  double       m_last_posting_time;

  // Clients and Summary stored across iterations
  std::string  m_db_clients;
  std::string  m_proc_watch_summary;

  std::vector<std::string>  m_watch_list;
  std::vector<std::string>  m_excused_list;

  // Mapping from proc name to data
  std::map<std::string, bool> m_map_alive;
  std::map<std::string, bool> m_map_alive_prev;
  std::map<std::string, unsigned int> m_map_noted_gone;
  std::map<std::string, unsigned int> m_map_noted_here;
  std::map<std::string, double>       m_map_now_cpuload;
  std::map<std::string, double>       m_map_max_cpuload;

  std::set<std::string> m_set_db_clients;
  std::set<std::string> m_set_watch_clients;
  std::set<std::string> m_set_antler_clients;

 protected: // Configurations Variables
  bool         m_watch_all_db;
  bool         m_watch_all_antler;
  double       m_allow_retractions;
  double       m_min_wait;             
  double       m_noted_gone_wait;

  // Include List
  std::vector<std::string>  m_include_list;
  std::vector<bool>         m_include_list_prefix;

  // Exclude List
  std::vector<std::string>  m_exclude_list;
  std::vector<bool>         m_exclude_list_prefix;

  // A dedicated MOOS var for posting when a proc chgs status
  std::map<std::string, std::string> m_map_proc_post;

  // A map for changing the MOOS variable posted.
  std::map<std::string, std::string> m_map_chgpost;
};

#endif 








