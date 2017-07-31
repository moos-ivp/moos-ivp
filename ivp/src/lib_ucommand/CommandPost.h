/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CommandPost.h                                        */
/*    DATE: July 10th, 2016                                      */
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

#ifndef COMMAND_POST_HEADER
#define COMMAND_POST_HEADER

#include "CommandItem.h"
#include <string>

class CommandPost
{
 public:
  CommandPost() {m_cmd_test=false;}
  virtual ~CommandPost() {}

  void setCommandItem(CommandItem item) {m_cmd_item=item;}
  void setCommandTarg(std::string targ) {m_cmd_targ=targ;}
  void setCommandTest(bool test)        {m_cmd_test=test;}
  void setCommandPID(std::string pid)   {m_cmd_pid=pid;}
  
  CommandItem getCommandItem() {return(m_cmd_item);}
  std::string getCommandTarg() {return(m_cmd_targ);}
  bool        getCommandTest() {return(m_cmd_test);}
  std::string getCommandPID()  {return(m_cmd_pid);}
  
 protected: 
  CommandItem  m_cmd_item; 
  std::string  m_cmd_targ; 
  bool         m_cmd_test; 
  std::string  m_cmd_pid; 
};

#endif 




