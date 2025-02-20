/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: MayFinish.h                                     */
/*    DATE: October 31st, 2024                              */
/************************************************************/

#ifndef UMAY_FINISH_HEADER
#define UMAY_FINISH_HEADER

#include <string>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class MayFinish : public AppCastingMOOSApp
{
 public:
   MayFinish();
  ~MayFinish() {};

  bool setFinishVar(std::string);
  bool setFinishVal(std::string);
  bool setMaxDBUpTime(std::string);
  
 protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();

protected:
  void registerVariables();

 private: // Config vars
  std::string m_finish_var;
  std::string m_finish_val;
  double      m_max_db_uptime;
  
private: // State variables
  std::string m_current_val;
  int         m_exit_val;
  double      m_now_db_uptime;
};

#endif 
