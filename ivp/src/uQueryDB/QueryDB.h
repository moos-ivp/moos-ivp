/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
#include "MOOS/libMOOS/MOOSLib.h"
#include "LogicCondition.h"
#include "InfoBuffer.h"

class QueryDB : public CMOOSApp  
{
 public:
  QueryDB(std::string g_server="localhost", long int g_port=9000); 
  virtual ~QueryDB() {}
  
  bool Iterate();
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool OnConnectToServer();
  bool OnStartUp();

  bool setLogicCondition(std::string);
  void setConfigCommsLocally(bool v)    {m_configure_comms_locally=v;}
  void setVerbose(bool v)               {m_verbose=v;}
  void setWaitTime(double v)            {m_wait_time=v;}
  
 protected:
  void registerVariables();
  bool updateInfoBuffer(CMOOSMsg& msg);
  bool ConfigureComms();
  bool checkCondition();
  bool allMailReceived() const;
  void printReport();

 protected: // Index for each is unique per variable name
  double m_db_time;

  LogicCondition  m_logic_condition;
  InfoBuffer     *m_info_buffer;

  double m_wait_time;
  double m_start_time;
  int    m_iteration;
  bool   m_verbose;
  bool   m_configure_comms_locally;
};









