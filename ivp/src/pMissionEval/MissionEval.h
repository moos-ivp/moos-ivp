/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: MissionEval.h                                        */
/*    DATE: Sep 19th, 2020                                       */
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

#ifndef MISSION_EVAL_HEADER
#define MISSION_EVAL_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include <vector>
#include <string>
#include "VarDataPair.h"
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
  void handleMailMissionHash(std::string, double msg_time);
  bool handleConfigColumnFormat(std::string);
  
  void postPreResults();
  void postResults();
  void postFlags(const std::vector<VarDataPair>&);

  void findMacroVars();
  
  std::string expandMacros(std::string) const;
  
 protected:
  void registerVariables();

 private: // Configuration variables

  MailFlagSet m_mfset;
  
  // Vars for posting results
  std::vector<VarDataPair> m_result_flags;
  std::vector<VarDataPair> m_pass_flags;
  std::vector<VarDataPair> m_fail_flags;

  std::vector<std::string> m_prereport_columns;
  std::vector<std::string> m_report_columns;
  std::string m_report_file;
  std::string m_report_line_format;

  std::string m_mission_form; // e.g. alpha,berta,legrun,joust   
  std::string m_mission_mod; // e.g. alpha,berta,legrun,joust   
  
 private: // State variables 
  LogicTestSequence m_logic_tests;
  std::string       m_logic_tests_status_prev;

  bool        m_prereport_posted;
  bool        m_result_flags_posted;
  std::string m_mission_result;

  std::string m_report_latest_line;
  
  std::string m_mission_hash;
  std::string m_mhash_short;
  double      m_mhash_utc;
  double      m_db_uptime;

  std::set<std::string> m_macro_vars;
  
  InfoBuffer *m_info_buffer;
};

#endif 
 

