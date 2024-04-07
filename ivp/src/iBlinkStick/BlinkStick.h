/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: BlinkStick.h                                    */
/*    DATE: April 7th, 2024                                 */
/************************************************************/

#ifndef BLINK_STICK_HEADER
#define BLINK_STICK_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class BlinkStick : public AppCastingMOOSApp
{
 public:
  BlinkStick();
  ~BlinkStick() {};
  
protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();

  bool handleMailQBlink(std::string);
  
 protected:
  void registerVariables();

  void setPathEnv();
  bool validColor(std::string);
  
 private: // Configuration variables

  std::string m_qblink;
  
private: // State variables

  int m_system_result;
  int m_total_cmds;

  std::string m_path_env;
  
};

#endif 
