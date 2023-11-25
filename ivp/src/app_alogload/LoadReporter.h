/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LoadReporter.h                                       */
/*    DATE: December 3rd, 2019                                   */
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

#ifndef ALOG_LOAD_REPORTER_HEADER
#define ALOG_LOAD_REPORTER_HEADER

#include <vector>
#include <string>
#include <set>

class LoadReporter
{
 public:
  LoadReporter();
  ~LoadReporter() {}

  void setVerbose();
  void setTerse()      {m_terse=true; m_verbose=false;}
  void setNearMode()   {m_near_mode = true;}
  bool addALogFile(std::string);
  void report();

  
  
 protected:
  bool breachCount(std::string alogfile);
  
 protected: // State Variables

  unsigned int m_breach_count;
  unsigned int m_near_breach_count;
  
 protected: // Configuration Variables

  bool m_verbose;
  bool m_terse;

  bool m_near_mode;
  
  std::vector<std::string> m_alog_files;

};

#endif










