/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GrepHandler.h                                        */
/*    DATE: August 6th, 2008                                     */
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

#ifndef ALOG_GREP_HANDLER_HEADER
#define ALOG_GREP_HANDLER_HEADER

#include <vector>
#include <string>
#include <set>

class GrepHandler
{
 public:
  GrepHandler();
  ~GrepHandler() {}

  bool handle(std::string, std::string str="");
  void addKey(std::string str);
  void printReport();
  void setFileOverWrite(bool v)    {m_file_overwrite=v;}
  void setCommentsRetained(bool v) {m_comments_retained=v;}
  void setBadLinesRetained(bool v) {m_badlines_retained=v;}
  void setGapLinesRetained(bool v) {m_gaplines_retained=v;}
  void setAppCastRetained(bool v)  {m_appcast_retained=v;}
  
 protected:
  std::vector<std::string> getMatchedKeys();
  std::vector<std::string> getUnMatchedKeys();

  std::string quickPassGetVName(const std::string);
  
  void outputLine(const std::string& line, const std::string& varname="");
  void ignoreLine(const std::string& line, const std::string& varname="");
  
 protected:

  std::vector<std::string> m_keys;
  std::vector<bool>        m_pmatch;

  double m_lines_removed;
  double m_lines_retained;
  double m_chars_removed;
  double m_chars_retained;
  bool   m_file_overwrite;

  std::string m_var_condition;
  bool        m_var_condition_met;
  bool        m_comments_retained;
  bool        m_badlines_retained;
  bool        m_gaplines_retained;
  bool        m_appcast_retained;
  
  std::set<std::string> m_vars_retained;
  std::set<std::string> m_vars_removed;
  
  FILE *m_file_in;
  FILE *m_file_out;
};

#endif









