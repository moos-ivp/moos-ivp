/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: MissionHash_MOOSApp.                            */
/*    DATE: July 15th, 2023                                 */
/************************************************************/

#ifndef MISSION_HASH_MOOSAPP_HEADER
#define MISSION_HASH_MOOSAPP_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class MissionHash_MOOSApp : public AppCastingMOOSApp
{
public:
  MissionHash_MOOSApp();
  ~MissionHash_MOOSApp() {};

protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();

protected:
  bool setMissionHash();
  void registerVariables();

private: // Configuration variables

  std::string m_mission_hash_var;
  
private: // State variables

  double      m_last_mhash_post;
  std::string m_mission_hash;
  std::string m_mission_hash_short;



};

#endif 
