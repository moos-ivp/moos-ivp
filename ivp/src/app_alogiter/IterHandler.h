/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IterHandler.h                                        */
/*    DATE: Dec 22nd, 2013                                       */
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

#ifndef ALOG_ITER_HANDLER_HEADER
#define ALOG_ITER_HANDLER_HEADER

#include <vector>
#include <string>
#include <set>
#include <map>

class IterHandler
{
 public:
  IterHandler();
  ~IterHandler() {}

  bool handle(const std::string&);
  void printReport();
  void printReportGap();
  void printReportLen();
  void printReportSummary();
  
 protected:

  std::map<std::string, double>       m_map_app_gap_max;
  std::map<std::string, double>       m_map_app_gap_total;
  std::map<std::string, unsigned int> m_map_app_gap_count;
  std::map<std::string, unsigned int> m_map_app_gap_count_1p25;
  std::map<std::string, unsigned int> m_map_app_gap_count_1p50;
  std::map<std::string, unsigned int> m_map_app_gap_count_2p00;

  std::map<std::string, double>       m_map_app_len_max;
  std::map<std::string, double>       m_map_app_len_total;
  std::map<std::string, unsigned int> m_map_app_len_count;
  std::map<std::string, unsigned int> m_map_app_len_count_0p25;
  std::map<std::string, unsigned int> m_map_app_len_count_0p50;
  std::map<std::string, unsigned int> m_map_app_len_count_0p75;
  std::map<std::string, unsigned int> m_map_app_len_count_1p00;

  std::vector<std::string> m_keys;
  std::vector<bool>        m_pmatch;

  double m_lines_removed;
  double m_lines_retained;
  double m_chars_removed;
  double m_chars_retained;
  bool   m_file_overwrite;

  std::set<std::string> m_vars_retained;
  std::set<std::string> m_vars_removed;
  
  FILE *m_file_in;
};

#endif









