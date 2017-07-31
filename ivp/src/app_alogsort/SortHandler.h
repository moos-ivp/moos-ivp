/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SortHandler.h                                        */
/*    DATE: June 22nd, 2013                                      */
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

#ifndef ALOG_SORT_HANDLER_HEADER
#define ALOG_SORT_HANDLER_HEADER

#include <vector>
#include <string>
#include <set>

class SortHandler
{
 public:
  SortHandler();
  ~SortHandler() {}

  bool handleSort(const std::string&, const std::string& str="");
  bool handleCheck(const std::string&);
  void printReport();
  void setCacheSize(unsigned int v) {m_cache_size=v;}
  void setFileOverWrite(bool v)     {m_file_overwrite=v;}
  
 protected:
  unsigned int m_cache_size;
  unsigned int m_total_lines;
  unsigned int m_re_sorts;

  bool  m_file_overwrite;

  FILE *m_file_in;
  FILE *m_file_out;
};

#endif









