/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TS_MOOSApp.h                                         */
/*    DATE: May 21st 2009                                        */
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

#ifndef TIMER_SCRIPT_MOOS_APP_HEADER
#define TIMER_SCRIPT_MOOS_APP_HEADER

#include <vector>
#include <set>
#include <string>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "VarDataPair.h"
#include "LogicCondition.h"
#include "InfoBuffer.h"
#include "RandomVariableSet.h"
#include "RandomPairSet.h"

class TS_MOOSApp : public AppCastingMOOSApp
{
 public:
  TS_MOOSApp();
  virtual ~TS_MOOSApp() {}

 public: // Standard MOOSApp functions to overload
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload
  bool   buildReport();

 public: // Public configuration functions
  void setVerbose(bool val) {m_verbose = val;}
  void setShuffle(bool val) {m_shuffle = val;}

 protected: // Locally defined and locally used functions
  void   registerVariables();
  void   addNewEvent(const std::string&);
  void   scheduleEvents(bool shuffle=true);
  void   printRawScript();
  bool   checkForReadyPostings();
  void   executePosting(VarDataPair);
  void   jumpToNextPostingTime();
  void   handleReset();
  void   handleRestart();
  void   postStatus();
  void   seedRandom();
  bool   handleMathExpr(std::string&);
  void   addLogEvent(std::string, std::string, double);
  bool   addBlockApps(std::string);
  void   checkBlockApps(std::string);

 protected: // Functions in support of logic conditions
  bool updateInfoBuffer(CMOOSMsg&);
  bool checkConditions();

 protected: // Functions in support of random variables
  std::string randomNumString(unsigned int);

 protected: // Configuration parameters
  std::vector<VarDataPair>  m_pairs;
  std::vector<double>       m_ptime;
  std::vector<double>       m_ptime_min;
  std::vector<double>       m_ptime_max;
  std::vector<bool>         m_poked;

  std::string               m_script_name;
  bool                      m_time_zero_connect;
  bool                      m_verbose;
  bool                      m_shuffle;
  bool                      m_atomic;
  std::string               m_upon_awake;
  RandVarUniform            m_uts_time_warp;
  RandVarUniform            m_delay_start;   
  RandVarUniform            m_delay_reset;  
  RandomVariableSet         m_rand_vars;
  RandomPairSet             m_rand_pairs;

  // Set of logic conditions pertaining to entire script
  std::vector<LogicCondition> m_logic_conditions;
  
  // MOOS variables configurable by the user
  std::string m_var_forward;
  std::string m_var_pause;
  std::string m_var_status;
  std::string m_var_reset;

  bool          m_reset_forever;
  unsigned int  m_reset_max;
  double        m_reset_time;  // -1:none, 0:after-last, NUM:atNUM

 protected: // State variables
  double   m_previous_time;
  double   m_elapsed_time;
  double   m_script_start_time;
  double   m_connect_tstamp;
  double   m_status_tstamp;
  bool     m_status_needed;
  double   m_skip_time;
  double   m_pause_time;
  bool     m_paused;
  bool     m_conditions_ok;
  bool     m_exit_ready;
  bool     m_exit_confirmed;

  unsigned int  m_posted_count_local;
  unsigned int  m_posted_count_total;
  unsigned int  m_reset_count;

  InfoBuffer *m_info_buffer;

  double m_nav_x;
  double m_nav_x_set;
  double m_nav_y;
  double m_nav_y_set;

  std::list<std::string>   m_event_log;
  std::set<std::string>    m_block_apps;
};

#endif 










