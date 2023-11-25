/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AppLogEntry.h                                        */
/*    DATE: Oct 15th 2021                                        */
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

#ifndef APP_LOG_ENTRY_HEADER
#define APP_LOG_ENTRY_HEADER

#include <vector>
#include <string>

class AppLogEntry
{
 public:
  AppLogEntry(double tstamp=0)  {m_tstamp=tstamp; m_iteration=0;}
  virtual ~AppLogEntry() {}

  // Setters
  void setIteration(unsigned int ival)                {m_iteration=ival;}
  void setAppLogLines(std::vector<std::string> lines) {m_lines=lines;}
  
  // Getters
  unsigned int size() const {return(m_lines.size());}

  double       getTStamp() const    {return(m_tstamp);}
  std::string  getAppName() const   {return(m_appname);}
  unsigned int getIteration() const {return(m_iteration);}
  std::string  getLine(unsigned int) const;
  std::string  getTruncLine(unsigned int) const;

  std::vector<std::string> getLines(bool tag=false) const;
  std::vector<std::string> getTruncLines();
  std::vector<std::string> getWrapLines();

  //bool valid() const {return(m_lines.size() != 0);}
  bool valid() const {return(true);}
  
private:
  double       m_tstamp;

  std::string  m_appname;
  unsigned int m_iteration;

  std::vector<std::string> m_lines;
};

AppLogEntry stringToAppLogEntry(std::string, bool verbose=false);

#endif

