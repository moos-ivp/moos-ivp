/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: QueryDB.h                                            */
/*    DATE: Dec 29th 2015                                        */
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

#include <map>
#include <string>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "LogicCondition.h"
#include "LogicBuffer.h"
#include "InfoBuffer.h"

class QueryDB : public AppCastingMOOSApp
{
 public:
  QueryDB(); 
  virtual ~QueryDB() {}
  
  bool Iterate();
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool OnConnectToServer();
  bool OnStartUp();

  bool setMissionFile(std::string);
  bool setServerPort(std::string);   
  bool addPassCondition(std::string);
  bool addFailCondition(std::string);
  bool addConfigCheckVar(std::string);
  bool setConfigCheckVarFormat(std::string);
  bool setConfigWaitTime(std::string);
  void setServerHost(std::string s)     {m_sServerHost=s;}
  void setConfigCommsLocally(bool v)    {m_configure_comms_locally=v;}

  std::string getMissionFile() const {return(m_mission_file);}
  std::string getServerHost() const  {return(m_sServerHost);}
  long int    getServerPort() const  {return(m_lServerPort);}
  
 protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();
  
 protected:
  void registerVariables();
  bool updateInfoBuffer(CMOOSMsg& msg);
  bool ConfigureComms();
  void reportCheckVars();

  void checkPassFailConditions();

 protected: // State vars
  LogicBuffer  m_pass_conditions;
  LogicBuffer  m_fail_conditions;

  InfoBuffer  *m_info_buffer;
  int          m_exit_value;
  double       m_elapsed_time;

  // When/if a condition fails, this holds the clue
  std::string  m_notable_condition;
  
 protected: // Config vars
  std::string  m_mission_file;
  std::string  m_check_var_format;
  
  std::vector<std::string> m_check_vars;
  
  double  m_max_time;
  bool    m_configure_comms_locally;
};

