/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: NavPlot.cpp                                          */
/*    DATE: June 1st, 2005                                       */
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

#include <iostream>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "NavPlot.h"
#include "MBUtils.h"
#include "AngleUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: set_value
//      Note: Time must be in ascending order. If new pair doesn't
//            obey, no action is taken, and false is returned.

bool NavPlot::set_value(double gtime, double gxval, 
			double gyval, double gcval)
{
  int tsize = time.size();

  if((tsize == 0) || (time[tsize-1] <= gtime)) {
    time.push_back(gtime);
    xval.push_back(gxval);
    yval.push_back(gyval);

    // The "cval" for index i is the course or relative angle between
    // point i and point i+1. We can't determine cval[0] until the 
    // second point is provided - that's the extra twist below. We
    // declare cval[0]=0 in the meanwhile.

    if(gcval != 999) {
      cval.push_back(gcval);
      return(true);
    }
    
    int curr_ix = time.size() - 1;
    if(curr_ix == 0)
      cval.push_back(0);
    else {
      double pxval = xval[curr_ix - 1];
      double pyval = yval[curr_ix - 1];
      double pcval = cval[curr_ix - 1];
      double course = pcval;
      if((pxval != gxval) || (pyval != gyval))
	course = relAng(pxval, pyval, gxval, gyval);
      cval.push_back(course);
    }
    return(true);
  }
  else
    return(false);
}

//---------------------------------------------------------------
// Procedure: get_time_by_index

double NavPlot::get_time_by_index(int index) const
{
  if(time.size() == 0)
    return(0);

  if(index < 0)
    return(time[0]);
  else if(index >= time.size()) {
    cout << "time.size():" << time.size() << endl;
    return(time[time.size()-1]);
  }
  else
    return(time[index]);
}
     
//---------------------------------------------------------------
// Procedure: get_xval_by_index

double NavPlot::get_xval_by_index(int index) const
{
  if(xval.size() == 0)
    return(0);

  if(index < 0)
    return(xval[0]);
  else if(index >= xval.size())
    return(xval[xval.size()-1]);
  else
    return(xval[index]);
}
     
//---------------------------------------------------------------
// Procedure: get_yval_by_index

double NavPlot::get_yval_by_index(int index) const
{
  if(yval.size() == 0)
    return(0);

  if(index < 0)
    return(yval[0]);
  else if(index >= yval.size())
    return(yval[yval.size()-1]);
  else
    return(yval[index]);
}
     
//---------------------------------------------------------------
// Procedure: get_cval_by_index

double NavPlot::get_cval_by_index(int index) const
{
  if(cval.size() == 0)
    return(0);

  if(index < 0)
    return(cval[0]);
  else if(index >= cval.size())
    return(cval[cval.size()-1]);
  else
    return(cval[index]);
}
     
//---------------------------------------------------------------
// Procedure: get_xval_by_time
//   Purpose: Find the x value for a given point in time.
//            If the point in time happens between to elements
//            in the time array, interpolate between them.

double NavPlot::get_xval_by_time(double qtime) const
{
  // Special case: if the NavPlot instance is "empty"
  int vsize = time.size();
  if(vsize == 0)
    return(0);

  // Special case: if the query time is outside the NavPlot 
  // time range, return the extreme value.
  if(qtime >=  time[vsize-1])
    return(xval[vsize-1]);
  if(qtime <= time[0])
    return(xval[0]);

  // Determine highest index w/ time <= qtime
  int index = get_index_by_time(qtime);

  // If query time exactly at an index just return xval[index]
  if(qtime == time[index])
    return(xval[index]);

  // if query time not exactly at an index, interpolate.
  double val1 = xval[index];
  double val2 = xval[index+1];
  double val_range  = val2 - val1;
  double time_range = time[index+1] - time[index];

  assert(time_range >= 0);

  double pct_time = (qtime - time[index]) / time_range;
  double rval = (pct_time * val_range) + val1;

  return(rval);
}
     
//---------------------------------------------------------------
// Procedure: get_yval_by_time
//   Purpose: Find the y value for a given point in time.
//            If the point in time happens between to elements
//            in the time array, interpolate between them.

double NavPlot::get_yval_by_time(double qtime) const
{
  // Special case: if the NavPlot instance is "empty"
  int vsize = time.size();
  if(vsize == 0)
    return(0);

  // Special case: if the query time is outside the NavPlot 
  // time range, return the extreme value.
  if(qtime >=  time[vsize-1])
    return(yval[vsize-1]);
  if(qtime <= time[0])
    return(yval[0]);

  // Determine highest index w/ time <= qtime
  int index = get_index_by_time(qtime);

  // If query time exactly at an index just return yval[index]
  if(qtime == time[index])
    return(yval[index]);

  // if query time not exactly at an index, interpolate.
  double val1 = yval[index];
  double val2 = yval[index+1];  
  double val_range  = val2 - val1;
  double time_range = time[index+1] - time[index];

  assert(time_range >= 0);

  double pct_time = (qtime - time[index]) / time_range;
  double rval = (pct_time * val_range) + val1;

  return(rval);
}

//---------------------------------------------------------------
// Procedure: get_cval_by_time
//   Purpose: Find the course (theta) value for a given point in
//            time. If the point in time happens between to elements
//            in the time array, interpolate between them.
//            Care must be taken for interpolating between to thetas
//            that cross the 360 degree mark. 
//            For example, 10% between 355 and 5 is 356, not 320.

double NavPlot::get_cval_by_time(double qtime) const
{
  // Special case: if the NavPlot instance is "empty"
  int vsize = time.size();
  if(vsize == 0)
    return(0);

  // Special case: if the query time is outside the NavPlot 
  // time range, return the extreme value.
  if(qtime >=  time[vsize-1])
    return(cval[vsize-1]);
  if(qtime <= time[0])
    return(cval[0]);

  // Determine highest index w/ time <= qtime
  int index = get_index_by_time(qtime);

  // If query time exactly at an index just return cval[index]
  if(qtime == time[index])
    return(yval[index]);

  // if query time not exactly at an index, interpolate.
  double val1 = cval[index];
  double val2 = cval[index+1];
  double time_range = time[index+1] - time[index];
  double val_range  = val2 - val1;
  val_range = angle180(val_range);

  assert(time_range >= 0);

  double pct_time = (qtime - time[index]) / time_range;
  double rval = (pct_time * val_range) + val1;
  rval = angle360(rval);

  return(rval);
}

//---------------------------------------------------------------
// Procedure: get_index_by_time
//   Purpose: Given a query time, determine the highest index that 
//            has a time less than or equal to the query time.
//            Search is log(n)

int NavPlot::get_index_by_time(double qtime) const
{
  int vsize = time.size();

  // Special case: if the query time is outside the NavPlot 
  // time range, return the extreme value.
  if(qtime <= time[0])
    return(0);
  if(qtime >= time[vsize-1])
    return(vsize-1);
  
  // Handle general case
  int jump  = vsize / 2;
  int index = vsize / 2;
  bool done = false;
  while(!done) {
    if(jump > 1)
      jump = jump / 2;
    if(time[index] <= qtime) {
      if(time[index+1] > qtime)
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
// Procedure: get_min_time

double NavPlot::get_min_time() const
{
  if(time.size() > 0)
    return(time[0]);
  else
    return(0);
}

//---------------------------------------------------------------
// Procedure: get_max_time

double NavPlot::get_max_time() const
{
  if(time.size() > 0)
    return(time[time.size()-1]);
  else
    return(0);
}


//---------------------------------------------------------------
// Procedure: print

void NavPlot::print() const
{
  cout << "NavPlot::print()" << endl;
  for(int i=0; i<time.size(); i++) {
    cout << "time:" << time[i] << "  x:" << xval[i];
    cout <<   "  y:" << yval[i] << endl;
  }
}








