/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VarTPlot.cpp                                         */
/*    DATE: Oct1011                                              */
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

#include <list>
#include <iostream>
#include "VarTPlot.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

VarTPlot::VarTPlot()
{
  m_base_utc = 0;
}


//---------------------------------------------------------------
// Procedure: addEntry
//      Note: Time must be in ascending order. If new pair doesn't
//            obey, no action is taken, and false is returned.

bool VarTPlot::addEntry(const ALogEntry& entry)
{
  unsigned int tsize = m_entry.size();

  if((tsize == 0) || (m_entry[tsize-1].time() <= entry.time())) {
    m_entry.push_back(entry);
    return(true);
  }
  else
    return(false);
}


//---------------------------------------------------------------
// Procedure: getEntryByIndex

ALogEntry VarTPlot::getEntryByIndex(unsigned int index) const
{  
  if(index < m_entry.size())
    return(m_entry[index]);
  return(ALogEntry());
}

//---------------------------------------------------------------
// Procedure: getEntryByTime

ALogEntry VarTPlot::getEntryByTime(double gtime) const
{
  ALogEntry null_entry;
  if(!containsTime(gtime))
    return(null_entry);

  unsigned int index = getIndexByTime(gtime);
  return(m_entry[index]);
}
     
//---------------------------------------------------------------
// Procedure: getEntryByTimeUTC

ALogEntry VarTPlot::getEntryByTimeUTC(double utc_time) const
{
  ALogEntry null_entry;
  if(!containsTimeUTC(utc_time))
    return(null_entry);

  unsigned int index = getIndexByTimeUTC(utc_time);
  return(m_entry[index]);
}
     
//---------------------------------------------------------------
// Procedure: containsTime

bool VarTPlot::containsTime(double gtime) const
{
  unsigned int vsize = m_entry.size();
  if(vsize == 0)
    return(false);

  if((gtime < m_entry[0].time()) || (gtime > m_entry[vsize-1].time()))
    return(false);

  return(true);
}
     
//---------------------------------------------------------------
// Procedure: containsTimeUTC

bool VarTPlot::containsTimeUTC(double utc_time) const
{
  double local_time = utc_time - m_base_utc;
  return(containsTime(local_time));
}
     

//---------------------------------------------------------------
// Procedure: print

void VarTPlot::print() const
{
  unsigned int i;
  cout << "VarTPlot::print()" << endl;
  cout << " Vehicle  Name: " << m_vehiname << endl;
  cout << " Variable Name: " << m_varname << endl;
  for(i=0; i<m_entry.size(); i++) {
    cout << "time:" << m_entry[i].time();
    if(m_entry[i].isNumerical())
      cout << "  val:" << m_entry[i].getDoubleVal() << endl;
    else
      cout << "  val:" << m_entry[i].getStringVal() << endl;
  }
}


//---------------------------------------------------------------
// Procedure: getIndexByTime
//   Purpose: Given a time, determine the highest index that has a
//            time less than or equal to the given time.

//  2 5 9 13 14 19 23 28 31 35 43 57
//             ^                 ^
//            25                 56

unsigned int VarTPlot::getIndexByTime(double gtime) const
{
  unsigned int vsize = m_entry.size();
  if(vsize == 0)
    return(0);

  // Handle special cases
  if(gtime <= m_entry[0].time())
    return(0);
  if(gtime >= m_entry[vsize-1].time())
    return(vsize-1);

  int jump  = vsize / 2;
  int index = vsize / 2;
  bool done = false;
  while(!done) {
    //cout << "[" << index << "," << jump << "]" << flush;
    if(jump > 1)
      jump = jump / 2;
    if(m_entry[index].time() <= gtime) {
      if(m_entry[index+1].time() > gtime)
	done = true;
      else
	index += jump;
    }
    else
      index -= jump;
  }
  return(index);
}


//---------------------------------------------------------------
// Procedure: getIndexByTimeUTC

unsigned int VarTPlot::getIndexByTimeUTC(double utc_time) const
{
  double local_time = utc_time - m_base_utc;
  return(getIndexByTime(local_time));
}


