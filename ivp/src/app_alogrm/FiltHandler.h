/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: FiltHandler.h                                        */
/*    DATE: August 6th, 2008                                     */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
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
  ~FiltHandler() {};

  bool handle(const std::string&, const std::string& str="");
  bool setParam(const std::string& param, const std::string& value);
  void printReport();

 protected:

  std::vector<std::string> m_keys;
  std::vector<bool>        m_pmatch;

  bool  m_chuck_strings;
  bool  m_chuck_numbers;
  bool  m_timeshift;
  bool  m_clean;

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

