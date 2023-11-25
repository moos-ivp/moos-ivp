/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AppLogPlot.cpp                                       */
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

#include "AppLogPlot.h"
#include "MBUtils.h"
#include "LogUtils.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: addAppLogEntry

void AppLogPlot::addAppLogEntry(double gtime, AppLogEntry entry)
{
  m_time.push_back(gtime);
  m_entries.push_back(entry);
}

//----------------------------------------------------------------
// Procedure: getEntryByIndex()

AppLogEntry AppLogPlot::getEntryByIndex(unsigned int ix) const
{
  AppLogEntry null_entry;
  if(ix >= m_entries.size())
    return(null_entry);

  return(m_entries[ix]);
}


//---------------------------------------------------------------
// Procedure: containsTime()

bool AppLogPlot::containsTime(double gtime) const
{
  unsigned int vsize = m_time.size();
  if(vsize == 0)
    return(false);

  if((gtime < m_time[0]) || (gtime > m_time[vsize-1]))
    return(false);

  return(true);
}
     

//----------------------------------------------------------------
// Procedure: getEntryByTime()

AppLogEntry AppLogPlot::getEntryByTime(double gtime) const
{
  AppLogEntry null_entry;
  if(!containsTime(gtime))
    return(null_entry);

  unsigned int ix = getIndexByTime(m_time, gtime);
  return(m_entries[ix]);
}


//----------------------------------------------------------------
// Procedure: getEntriesUpToTime()

vector<AppLogEntry> AppLogPlot::getEntriesUpToTime(double gtime) const
{
  vector<AppLogEntry> entries;

  if(!containsTime(gtime))
    return(entries);

  unsigned int now_index = getIndexByTime(m_time, gtime);
  unsigned int beg_index = 0;
  if(now_index >= 50)
    beg_index = now_index - 50;  

  for(unsigned int i=beg_index; i<=now_index; i++) 
    entries.push_back(m_entries[i]);

  return(entries);
}

//----------------------------------------------------------------
// Procedure: getEntriesPastTime()

vector<AppLogEntry> AppLogPlot::getEntriesPastTime(double gtime) const
{
  vector<AppLogEntry> entries;

  if(!containsTime(gtime))
    return(entries);

  unsigned int now_index = getIndexByTime(m_time, gtime);

  unsigned int end_index = m_entries.size();
  if((now_index + 50) < m_entries.size())
    end_index = now_index + 50;

  for(unsigned int i=now_index+1; i<end_index; i++) 
    entries.push_back(m_entries[i]);

  return(entries);
}


