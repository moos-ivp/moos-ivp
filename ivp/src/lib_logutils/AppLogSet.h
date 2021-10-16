/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AppLogSet.h                                          */
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

#ifndef APP_LOG_SET_HEADER
#define APP_LOG_SET_HEADER

#include <vector>
#include <string>
#include "AppLogEntry.h"

class AppLogSet
{
 public:
  AppLogSet() {};
  virtual ~AppLogSet() {}

  // Setters
  void addAppLogEntry(AppLogEntry entry) {m_entries.push_back(entry);}
  
  // Getters
  unsigned int size() const {return(m_entries.size());}

  AppLogEntry  getEntry(unsigned int) const;

private:
  std::vector<AppLogEntry> m_entries;

};


#endif




