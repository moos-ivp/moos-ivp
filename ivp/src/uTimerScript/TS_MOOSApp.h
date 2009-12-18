/************************************************************/
/*    NAME: Michael Benjamin, H.Schmidt, J.Leonard          */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: TS_MOOSApp.h                                    */
/*    DATE: May 21st 2009                                   */
/************************************************************/

#ifndef TIMER_SCRIPT_MOOS_APP_HEADER
#define TIMER_SCRIPT_MOOS_APP_HEADER

#include <vector>
#include <string>
#include "MOOSLib.h"
#include "VarDataPair.h"
#include "LogicCondition.h"
#include "InfoBuffer.h"
#include "RandomVariable.h"

class TS_MOOSApp : public CMOOSApp
{
 public:
  TS_MOOSApp();
  virtual ~TS_MOOSApp() {};

 public: // Standard MOOSApp functions to overload
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Locally defined and locally used functions
  void RegisterVariables();
  bool addNewEvent(std::string);
  void sortEvents();
  void printScript();
  bool checkForReadyPostings();
  void jumpToNextPostingTime();
  void handleReset();
  void postStatus();
  void seedRandom();
  void expandIndexPairs();

 protected: // Functions in support of logic conditions
  bool updateInfoBuffer(CMOOSMsg&);
  bool checkConditions();

 protected: // Functions in support of random variables
  std::string addRandomVariable(std::string spec);
  void resetVariables(std::string key, double time);

 protected: // Configuration parameters
  std::vector<VarDataPair>    m_pairs;
  std::vector<double>         m_ptime;
  std::vector<bool>           m_poked;

  // Set of logic conditions pertaining to entire script
  std::vector<LogicCondition> m_logic_conditions;
  
  std::string m_var_next_event;
  std::string m_var_forward;
  std::string m_var_pause;
  std::string m_var_status;
  std::string m_var_reset;

  int      m_reset_max;
  double   m_reset_time;  // -1:none, 0:after-last, NUM:atNUM

 protected: // State variables
  double   m_previous_time;
  double   m_elapsed_time;
  double   m_start_time;
  double   m_skip_time;
  double   m_pause_time;
  double   m_db_uptime;
  double   m_utc_time;
  bool     m_paused;
  bool     m_conditions_ok;
  int      m_posted_count;
  int      m_posted_tcount;
  int      m_reset_count;
  int      m_iteration;

  InfoBuffer *m_info_buffer;

  std::vector<RandomVariable> m_rand_vars;
};

#endif 
