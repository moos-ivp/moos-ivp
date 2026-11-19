/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: MissionWrap.h                                   */
/*    DATE: November 12th, 2026                             */
/************************************************************/

#ifndef MISSION_MWRAP_HEADER
#define MISSION_MWRAP_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "MWrap.h"

class MissionWrap : public AppCastingMOOSApp
{
 public:
   MissionWrap();
  ~MissionWrap() {};

 protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected:
   void registerVariables();

 protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();

 protected:
  void handleMailMissionHash(std::string);
  bool handleMailMWrap(std::string);
  void updateResults();

  void postResultsToFile(std::vector<std::string>, bool finish);
  void postResultsToMOOS();

  std::string expandMacros(std::string) const;
    
 private: // Configuration variables

  unsigned int m_required_nodes;

  std::vector<std::string> m_prereport_columns;
  std::vector<std::string> m_report_columns;
  std::string m_report_file;
  std::string m_colsep;

  std::string m_mission;
  std::string m_patch;
  
 private: // State variables
  std::map<std::string, MWrap> m_map_wraps;

  bool   m_results_posted;
  bool   m_all_pass;
  double m_pct_pass;
  double m_avg_score;
  double m_min_score;
  double m_max_score;

  std::string m_mission_hash;
  std::string m_mhash_short;
  std::string m_mhash_lshort;
  double      m_mhash_utc;  
};

#endif 
