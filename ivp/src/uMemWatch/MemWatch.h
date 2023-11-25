/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: MemWatch.h                                           */
/*    DATE: May 4th, 2019                                        */
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

#ifndef MEM_WATCH_HEADER
#define MEM_WATCH_HEADER

#include <map>
#include <string>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class MemWatch : public AppCastingMOOSApp
{
public:
  MemWatch();
  ~MemWatch() {};
  
protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();
  
protected:
  void measureMemory();
  void measureMemoryApp(std::string app);
  void handleMailDBClients(std::string);
  void handleMailInfoPID(std::string, double);

  bool handleConfigTimeGap(std::string);
  bool handleConfigWatchOnly(std::string);
  bool handleConfigIgnore(std::string);
  void registerVariables();
  
private: // Configuration variables

  double m_absolute_time_gap;

private: // State variables

  std::vector<std::string> m_apps_known_to_db;
  std::vector<std::string> m_apps_watch_only;
  std::vector<std::string> m_apps_ignore;

  // All in this group indexed together
  std::vector<std::string> m_app;
  std::vector<std::string> m_pid;
  std::vector<double>      m_mem;
  std::vector<double>      m_delta;

  unsigned int m_last_measure_ix;
  unsigned int m_total_measures;

  std::string m_last_app;
  std::string m_last_mem;
  std::string m_last_result_raw;

  double m_last_check_tstamp;  
};

#endif 

