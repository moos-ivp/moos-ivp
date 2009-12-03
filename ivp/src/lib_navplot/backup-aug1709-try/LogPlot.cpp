/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LogPlot.cpp                                          */
/*    DATE: May 31st, 2005                                       */
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

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include "LogPlot.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

LogPlot::LogPlot()
{
  m_min_val  = 0; 
  m_max_val  = 0; 
  m_median   = 0;
  m_median_set = false;
  m_utc_start_time = 0; 
}


//---------------------------------------------------------------
// Procedure: set_value
//      Note: Time must be in ascending order. If new pair doesn't
//            obey, no action is taken, and false is returned.


bool LogPlot::set_value(double gtime, double gval)
{
  unsigned int tsize = m_time.size();

  if((tsize == 0) || (m_time[tsize-1] <= gtime)) {
    m_time.push_back(gtime);
    m_value.push_back(gval);
    if((tsize == 0) || (gval > m_max_val))
      m_max_val = gval;
    if((tsize == 0) || (gval < m_min_val))
      m_min_val = gval;
    m_median_set = false;
    return(true);
  }
  else
    return(false);
}

//---------------------------------------------------------------
// Procedure: get_median
//   Purpose: Calculate the median value of all points added so far.

double LogPlot::get_median()
{
  // The median value is stored locally - only calculated it if it
  // has not been calculated already. Subsequent additions to the 
  // log will clear the median value;
  if(m_median_set)
    return(m_median);

  list<double> vlist;

  // First put all the log values in the list.
  unsigned int k, ksize = m_value.size();
  for(k=0; k<ksize; k++)
    vlist.push_back(m_value[k]);

  // Then sort them - ascending or descending does not matter.
  vlist.sort();

  // Increment an interator half way through the list and get
  // the value at that point as the median.
  unsigned int lsize = vlist.size();
  list<double>::iterator p = vlist.begin();
  for(k=0; k<(lsize/2); k++)
    p++;

  m_median     = *p;
  m_median_set = true;

  return(m_median);
}
     
//---------------------------------------------------------------
// Procedure: get_value_by_index

double LogPlot::get_value_by_index(unsigned int index) const
{
  if(index < m_time.size())
    return(m_value[index]);
  else
    return(0);
}
     
//---------------------------------------------------------------
// Procedure: get_time_by_index

double LogPlot::get_time_by_index(unsigned int index) const
{
  if(index < m_time.size())
    return(m_time[index]);
  else
    return(0);
}
     
//---------------------------------------------------------------
// Procedure: get_value_by_time

double LogPlot::get_value_by_time(double gtime) const
{
  unsigned int vsize = m_time.size();

  if(vsize == 0)
    return(0);

  if(gtime >= m_time[vsize-1])
    return(m_value[vsize-1]);

  if(gtime <= m_time[0])
    return(m_value[0]);

  unsigned int index = get_index_by_time(gtime);
  
  if(gtime == m_time[index])
    return(m_value[index]);

  double val1 = m_value[index];
  double val2 = m_value[index+1];
  
  double val_range  = val2 - val1;
  double time_range = m_time[index+1] - m_time[index];

  assert(time_range >= 0);

  double pct_time = (gtime - m_time[index]) / time_range;

  double rval = (pct_time * val_range) + val1;

  return(rval);
}
     
//---------------------------------------------------------------
// Procedure: get_min_time

double LogPlot::get_min_time() const
{
  if(m_time.size() > 0)
    return(m_time[0]);
  else
    return(0.0);
}

//---------------------------------------------------------------
// Procedure: get_max_time

double LogPlot::get_max_time() const
{
  if(m_time.size() > 0)
    return(m_time[m_time.size()-1]);
  else
    return(0.0);
}


//---------------------------------------------------------------
// Procedure: print

void LogPlot::print() const
{
  unsigned int i;
  cout << "LogPlot::print()" << endl;
  cout << " Variable Name: " << m_varname << endl;
  cout << " Vehicle  Name: " << m_vehi_name << endl;
  for(i=0; i<m_time.size(); i++) {
    cout << "time:" << m_time[i] << "  val:" << m_value[i] << endl;
  }
}


//---------------------------------------------------------------
// Procedure: get_index_by_time
//   Purpose: Given a time, determine the highest index that has a
//            time less than or equal to the given time.

//  2 5 9 13 14 19 23 28 31 35 43 57
//             ^                 ^
//            25                 56

unsigned int LogPlot::get_index_by_time(double gtime) const
{
  unsigned int vsize = m_time.size();

  // Handle special cases
  if(gtime <= m_time[0])
    return(0);
  if(gtime >= m_time[vsize-1])
    return(vsize-1);

  int jump  = vsize / 2;
  int index = vsize / 2;
  bool done = false;
  while(!done) {
    //cout << "[" << index << "," << jump << "]" << flush;
    if(jump > 1)
      jump = jump / 2;
    if(m_time[index] <= gtime) {
      if(m_time[index+1] > gtime)
	done = true;
      else
	index += jump;
    }
    else
      index -= jump;
  }
  return(index);
}








