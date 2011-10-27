/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ProcessWatch.h                                       */
/*    DATE: May 27th 2007 (MINUS-07)                             */
/*          Aug 7th  2011 (overhaul, Waterloo)                   */
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

#ifndef PROCESS_WATCH_HEADER
#define PROCESS_WATCH_HEADER

#include <string>
#include <vector>
#include <map>
#include "MOOSLib.h"

class ProcessWatch : public CMOOSApp
{
public:
  ProcessWatch();
  virtual ~ProcessWatch() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

protected:
  void handleNewDBClients();

  void augmentIncludeList(std::string);
  void addToIncludeList(std::string);

  void augmentExcludeList(std::string);
  void addToExcludeList(std::string);

  void addToWatchList(std::string);

  void checkForIndividualUpdates();

  void postFullSummary();

  bool isAlive(std::string);
  void procNotedHere(std::string);
  void procNotedGone(std::string);

  void handlePostMapping(std::string);
  std::string postVar(std::string);

 protected: // State Variables
  bool         m_proc_watch_summary_changed;
  double       m_last_posting_time;

  // Clients and Summary stored across iterations
  std::string  m_db_clients;
  std::string  m_proc_watch_summary;

  std::vector<std::string>  m_watch_list;

  std::map<std::string, bool> m_map_alive;
  std::map<std::string, bool> m_map_alive_prev;
  std::map<std::string, unsigned int> m_map_noted_gone;
  std::map<std::string, unsigned int> m_map_noted_here;

 protected: // Configurations Variables
  bool         m_watch_all;
  double       m_min_wait; // -1 means only post on change

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
