/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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

  bool handle();
  bool setALogFile(std::string);
  void addKey(std::string str);
  void printReport();
  void setFileOverWrite(bool v)     {m_file_overwrite=v;}
  void setCommentsRetained(bool v)  {m_comments_retained=v;}
  void setBadLinesRetained(bool v)  {m_badlines_retained=v;}
  void setGapLinesRetained(bool v)  {m_gaplines_retained=v;}
  void setAppCastRetained(bool v)   {m_appcast_retained=v;}
  void setSortEntries(bool v)       {m_sort_entries=v;}
  void setMakeReport(bool v)        {m_make_report=v;}
  void setRemoveDups(bool v)        {m_rm_duplicates=v;}
  void setKeepKey(bool v)           {m_keep_key=v;}

  void setFinalOnly(bool v)         {m_final_only=v;}
  void setFirstOnly(bool v)
  {m_first_only=v; m_make_report=false; m_comments_retained=false;}

  void addSubPattern(std::string s) {m_subpat.push_back(s);}
  bool setFormat(std::string);
  void setColSep(char c);

 protected:

  bool checkRetain(std::string& line_raw);
  void outputLine(const std::string& line, bool last=false);
  void ignoreLine(const std::string& line);
    
  std::string quickPassGetVName(const std::string);
  
 protected: // Config vars

  bool   m_comments_retained;
  bool   m_badlines_retained;
  bool   m_gaplines_retained;
  bool   m_appcast_retained;
  bool   m_sort_entries;
  bool   m_rm_duplicates;

  bool   m_final_only;
  bool   m_first_only;
  bool   m_format_vals;
  bool   m_format_vars;
  bool   m_format_srcs;
  bool   m_format_time;
  bool   m_make_report;
  bool   m_keep_key;
  char   m_colsep;
  
  double m_cache_size;
  
  std::string m_filename_in;
  std::vector<std::string> m_subpat;
  
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

  unsigned int m_re_sorts;
  
  std::set<std::string> m_vars_retained;
};

#endif

