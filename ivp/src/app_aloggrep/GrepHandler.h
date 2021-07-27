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
  void setFinalEntryOnly(bool v)   {m_final_entry_only=v;}
  void setFinalTimeOnly(bool v)    {m_final_time_only=v;}
  void setFinalValueOnly(bool v)   {m_final_value_only=v;}
  void setValuesOnly(bool v)       {m_values_only=v;}
  void setTimesOnly(bool v)        {m_times_only=v;}
  void setSortEntries(bool v)      {m_sort_entries=v;}
  void setRemoveDuplicates(bool v) {m_rm_duplicates=v;}

 protected:

  bool checkRetain(std::string& line_raw);
  
  std::vector<std::string> getMatchedKeys();
  std::vector<std::string> getUnMatchedKeys();

  std::string quickPassGetVName(const std::string);
  
  void outputLine(const std::string& line);
  void ignoreLine(const std::string& line);
  
 protected: // Config vars

  std::string m_var_condition;
  bool        m_var_condition_met;
  bool        m_comments_retained;
  bool        m_badlines_retained;
  bool        m_gaplines_retained;
  bool        m_appcast_retained;
  bool        m_final_entry_only;
  bool        m_final_time_only;
  bool        m_final_value_only;
  bool        m_values_only;
  bool        m_times_only;

  bool        m_sort_entries;
  bool        m_rm_duplicates;
  
  double m_cache_size;
  
  FILE *m_file_in;
  FILE *m_file_out;

 protected: // State vars
  std::string m_final_line;
  std::string m_last_tstamp;
  
  std::vector<std::string> m_keys;
  std::vector<bool>        m_pmatch;

  double m_lines_removed;
  double m_lines_retained;
  double m_chars_removed;
  double m_chars_retained;
  bool   m_file_overwrite;

  double m_re_sorts;
  
  std::set<std::string> m_vars_retained;
};

#endif
