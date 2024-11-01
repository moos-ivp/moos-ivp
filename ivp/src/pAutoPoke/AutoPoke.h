/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: AutoPoke.h                                      */
/*    DATE: October 30th, 2024                              */
/************************************************************/

#ifndef AUTO_POKE_HEADER
#define AUTO_POKE_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "VarDataPair.h"

class AutoPoke : public AppCastingMOOSApp
{
 public:
  AutoPoke() ;
  ~AutoPoke() {};

protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
protected: 
  bool buildReport();
  void postFlags(const std::vector<VarDataPair>&);
  
protected:
  void registerVariables();
  
private: // Configuration variables
  unsigned int m_required_nodes;
  
  std::vector<VarDataPair> m_flags;
  
 private: // State variables
  unsigned int m_total_nodes;
  bool m_flags_posted;

};

#endif 
