/****************************************************************/
/*   NAME: Mike Benjamin                                        */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: MissionEval.h                                        */
/*   DATE: Sep 19th, 2020                                       */
/****************************************************************/

#ifndef MISSION_EVAL_HEADER
#define MISSION_EVAL_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include <vector>
#include <string>
#include "VarDataPair.h"
#include "VCheckSet.h"
#include "LogicAspect.h"
#include "MailFlagSet.h"
#include "InfoBuffer.h"
#include "LogicTestSequence.h"


class MissionEval : public AppCastingMOOSApp
{
 public:
  MissionEval();
  ~MissionEval() {};
  
 protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();

protected: // Utility functions
  void postResults();
  void postFlags(const std::vector<VarDataPair>&);

  std::string expandMacros(std::string) const;
  
 protected:
  void registerVariables();

 private: // Configuration variables

  // Vars for posting results
  std::vector<VarDataPair> m_result_flags;
  std::vector<VarDataPair> m_pass_flags;
  std::vector<VarDataPair> m_fail_flags;

  bool        m_result_flags_posted;
  std::string m_mission_result;

  MailFlagSet m_mfset;
  
 private: // State variables 
  LogicTestSequence m_logic_tests;
  std::string       m_logic_tests_status_prev;
  
  VCheckSet   m_vcheck_set;
  std::string m_vcheck_status_prev;

  std::set<std::string> m_reg_vars;

  InfoBuffer *m_info_buffer;
};

#endif 
 
