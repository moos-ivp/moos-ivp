/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: FiltHandler.h                                        */
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

#ifndef FILT_HANDLER_HEADER
#define FILT_HANDLER_HEADER

#include <set>
#include <vector>
#include <string>

class FiltHandler
{
 public:
  FiltHandler();
  ~FiltHandler() {}

  bool handle(const std::string&, const std::string& str="");
  bool setParam(const std::string& param, const std::string& value);
  void setBadLinesRetained(bool v) {m_badlines_retained=v;}
  void printReport();

 protected:
  void outputLine(const std::string& line, const std::string& var="");
  void ignoreLine(const std::string& line, const std::string& var="");

 protected:
  std::vector<std::string> m_keys;
  std::vector<bool>        m_pmatch;

  bool  m_chuck_strings;
  bool  m_chuck_numbers;
  bool  m_timeshift;
  bool  m_clean;
  bool  m_badlines_retained;

  double m_lines_removed;
  double m_lines_retained;
  double m_chars_removed;
  double m_chars_retained;
  bool   m_file_overwrite;

  std::set<std::string> m_vars_retained;
  std::set<std::string> m_vars_removed;

  FILE *m_file_in;
  FILE *m_file_out;

  double m_logstart;
};

#endif










