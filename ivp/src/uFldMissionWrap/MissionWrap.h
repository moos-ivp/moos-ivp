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

 protected: // Standard AppCastingMOOSApp function to overload 
  bool handleMailMWrap(std::string);
  void updateResults();
  void postResults();
  bool buildReport();

  
 protected:
   void registerVariables();

 private: // Configuration variables

  unsigned int m_required_nodes;
  
 private: // State variables
  std::map<std::string, MWrap> m_map_wraps;

  bool   m_results_posted;
  bool   m_all_pass;
  double m_pct_pass;
  double m_avg_score;
  double m_min_score;
  double m_max_score;

};

#endif 
