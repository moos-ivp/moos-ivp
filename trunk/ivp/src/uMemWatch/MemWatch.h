/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT                                             */
/*    FILE: MemWatch.h                                      */
/*    DATE: May 4th, 2019                                   */
/************************************************************/

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
