/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CommandSummary.h                                     */
/*    DATE: July 8th, 2016                                       */
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

#ifndef COMMAND_SUMMARY_HEADER
#define COMMAND_SUMMARY_HEADER

#include <string>
#include <vector>
#include <list>
#include <map>

class CommandSummary 
{
 public:
  CommandSummary();
  virtual ~CommandSummary() {}

  void addPosting(std::string var, std::string val,
		  std::string pid, bool post_test=false);

  void addAck(std::string);
  
  bool reportPending() const {return(m_report_pending);}

  std::vector<std::string> getCommandReport();
  
 protected: // sychronized lists always same size
  std::list<std::string>      m_post_vars;
  std::list<std::string>      m_post_vals;
  std::list<std::string>      m_post_pids;
  std::list<bool>             m_post_test;

  std::map<std::string, bool> m_post_acks;

  bool         m_report_pending;
};

#endif 




