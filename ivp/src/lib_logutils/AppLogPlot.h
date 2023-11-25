/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AppLogPlot.h                                         */
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

#ifndef APP_LOG_PLOT_HEADER
#define APP_LOG_PLOT_HEADER

#include <map>
#include <vector>
#include <string>
#include "AppLogEntry.h"

class AppLogPlot
{
 public:
  AppLogPlot() {};
  virtual ~AppLogPlot() {}

  // Setters
  void   setVName(std::string s)     {m_vname=s;}
  void   setAppName(std::string s)   {m_app_name=s;}
  
  void addAppLogEntry(double gtime, AppLogEntry entry);
  
  // Getters
  unsigned int size() const        {return(m_entries.size());}

  std::string  getAppName() const  {return(m_app_name);}

  bool         containsTime(double) const;

  AppLogEntry  getEntryByIndex(unsigned int index) const;
  AppLogEntry  getEntryByTime(double gtime) const;

  std::vector<AppLogEntry> getEntriesUpToTime(double gtime) const;
  std::vector<AppLogEntry> getEntriesPastTime(double gtime) const;

private:
  std::string m_vname;
  std::string m_app_name;

  std::vector<double>      m_time;
  std::vector<AppLogEntry> m_entries;
};


#endif





