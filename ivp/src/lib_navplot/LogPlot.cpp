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
// Procedure: set_value
//      Note: Time must be in ascending order. If new pair doesn't
//            obey, no action is taken, and false is returned.


bool LogPlot::set_value(double gtime, double gval)
{
  int tsize = time.size();

  if((tsize == 0) || (time[tsize-1] <= gtime)) {
    time.push_back(gtime);
    value.push_back(gval);
    if((tsize == 0) || (gval > max_val))
      max_val = gval;
    if((tsize == 0) || (gval < min_val))
      min_val = gval;
    return(true);
  }
  else
    return(false);
}

//---------------------------------------------------------------
// Procedure: get_value_by_index

double LogPlot::get_value_by_index(unsigned int index) const
{
  if((index >= 0) && (index < time.size()))
    return(value[index]);
  else
    return(0);
}
     
//---------------------------------------------------------------
// Procedure: get_time_by_index

double LogPlot::get_time_by_index(unsigned int index) const
{
  if((index >= 0) && (index < time.size()))
    return(time[index]);
  else
    return(0);
}
     
//---------------------------------------------------------------
// Procedure: get_value_by_time

double LogPlot::get_value_by_time(double gtime) const
{
  int vsize = time.size();

  if(vsize == 0)
    return(0);

  if(gtime >=  time[vsize-1])
    return(value[vsize-1]);

  if(gtime <= time[0])
    return(value[0]);

  int index = get_index_by_time(gtime);
  
  if(gtime == time[index])
    return(value[index]);

  double val1 = value[index];
  double val2 = value[index+1];
  
  double val_range  = val2 - val1;
  double time_range = time[index+1] - time[index];

  assert(time_range >= 0);

  double pct_time = (gtime - time[index]) / time_range;

  double rval = (pct_time * val_range) + val1;

  return(rval);
}
     
//---------------------------------------------------------------
// Procedure: get_min_time

double LogPlot::get_min_time() const
{
  if(time.size() > 0)
    return(time[0]);
  else
    return(0.0);
}

//---------------------------------------------------------------
// Procedure: get_max_time

double LogPlot::get_max_time() const
{
  if(time.size() > 0)
    return(time[time.size()-1]);
  else
    return(0.0);
}


//---------------------------------------------------------------
// Procedure: print

void LogPlot::print() const
{
  unsigned int i;
  cout << "LogPlot::print()" << endl;
  cout << " Varname: " << varname << endl;
  cout << " Vehicle: " << vehicle << endl;
  for(i=0; i<time.size(); i++) {
    cout << "time:" << time[i] << "  val:" << value[i] << endl;
  }
}


//---------------------------------------------------------------
// Procedure: get_index_by_time
//   Purpose: Given a time, determine the highest index that has a
//            time less than or equal to the given time.

//  2 5 9 13 14 19 23 28 31 35 43 57
//             ^                 ^
//            25                 56

int LogPlot::get_index_by_time(double gtime) const
{
  int vsize = time.size();

  // Handle special cases
  if(gtime <= time[0])
    return(0);
  if(gtime >= time[vsize-1])
    return(vsize-1);

  int jump  = vsize / 2;
  int index = vsize / 2;
  bool done = false;
  while(!done) {
    //cout << "[" << index << "," << jump << "]" << flush;
    if(jump > 1)
      jump = jump / 2;
    if(time[index] <= gtime) {
      if(time[index+1] > gtime)
	done = true;
      else
	index += jump;
    }
    else
      index -= jump;
  }
  return(index);
}








