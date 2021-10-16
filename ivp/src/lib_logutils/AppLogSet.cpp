/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AppLogSet.cpp                                      */
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

#include "AppLogSet.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: addAppLogEntry

void AppLogSet::addAppLogEntry(string app_name, AppLogEntry entry)
{
  m_map_entries[app_name].push_back(entry);
}

//----------------------------------------------------------------
// Procedure: getEntry()

AppLogEntry AppLogSet::getEntry(string app_name, unsigned int ix) const
{
  AppLogEntry null_entry;
  map<string, std::vector<AppLogEntry> >::const_iterator p;
  p = m_map_entries.find(app_name);
  if(p == m_map_entries.end())
    return(null_entry);

  vector<AppLogEntry> entries = p->second;
  if(ix >= entries.size())
    return(null_entry);

  return(entries[ix]);
}

