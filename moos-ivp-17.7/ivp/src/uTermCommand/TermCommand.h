/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: TermCommand.h                                        */
/*    DATE: June 26 2007                                         */
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

#ifndef TERM_COMMAND_HEADER
#define TERM_COMMAND_HEADER

#include <vector>
#include <string>
#include "MOOS/libMOOS/MOOSLib.h"


class TermCommand : public CMOOSApp
{
public:
  TermCommand();
  virtual ~TermCommand() {}
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
  void handleCharInput(char);

protected:
  void addCommand(std::string);
  void printMapping();
  void postCommand(unsigned int);

  std::vector<int> getPartialKeyMatches();
  int              getFullKeyMatch();

  void tabExpand();
  void handleArrow(char c);


public:
  CMOOSLock  m_tc_mutex;

protected:

  std::vector<std::string>  m_var_key;
  std::vector<std::string>  m_var_name;
  std::vector<std::string>  m_var_type;
  std::vector<std::string>  m_var_val;

  unsigned int  m_iteration;

  int  m_memory_ix;

  std::string m_cmd_buffer;
  std::vector<std::string> m_cmds_prev;
  std::string m_cmd_partial;
};

#endif









