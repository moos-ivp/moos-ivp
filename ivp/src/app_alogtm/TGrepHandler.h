/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TGrepHandler.h                                       */
/*    DATE: October 5th, 2020                                    */
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

#ifndef ALOG_TGREP_HANDLER_HEADER
#define ALOG_TGREP_HANDLER_HEADER

#include <vector>
#include <string>
#include <set>
#include <map>

class TGrepHandler
{
 public:
  TGrepHandler();
  ~TGrepHandler() {}

  bool setLogFile(std::string);
  bool setTimeMark(std::string);
  bool setMinGap(std::string);
  bool setMaxTDelta(std::string);
  bool setMaxVDelta(std::string);
  bool setSigDigits(std::string);
  void setVerbose()                {m_verbose = true;}
  void setTestFormat()             {m_test_format = true;}
  bool addKey(std::string);
  bool okALogFile() const          {return(m_file_in != 0);}
  bool handle();
    
 protected: // Config vars

  FILE*  m_file_in;
  double m_min_gap;
  bool   m_verbose;

  double m_max_tdelta;
  double m_max_vdelta;
  
  std::string m_mark_var;
  std::string m_mark_val;
  bool m_test_format;

  std::set<std::string> m_keys;

  unsigned int m_sig_digits;
  
 protected: // State vars

  std::set<std::string> m_unique_posts;

  std::map<std::string, double> m_latest_rep;
  
  double m_mark_time;
  bool   m_mark_made;
};

#endif

