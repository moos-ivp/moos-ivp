/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: NavPlot.cpp                                          */
/*    DATE: May 14th, 2008                                       */
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
#include <stdio.h>
#include <stdlib.h>
#include "GridPlot.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: initialize
//      Note: Takes a string which is typically the GRID_CONFIG string
//            found in a .alog file.
// Example:
//         GRID_CONFIG  label,ALPHA:-108,-138:-71,-229: 
//                    204,-118:167,-27@15,15 

bool GridPlot::initialize(double gtime, const string& str)
{
  m_gridlabel = "";
  m_time.clear();
  m_grids.clear();

  XYGrid first_grid;
  bool ok = first_grid.initialize(str);
  
  if(!ok)
    return(false);
  
  m_gridlabel = first_grid.getLabel();
  m_time.push_back(gtime);
  m_grids.push_back(first_grid);
  
  return(true);
}

//---------------------------------------------------------------
// Procedure: applyDelta
//      Note: Takes a string which is typically the GRID_DELTA string
//            found in a .alog file.
// Example:
//         GRID_DELTA_LOCAL = "ALPHA@41,0,0.27223,0,0" 

bool GridPlot::applyDelta(double gtime, const string& str)
{

  int vsize = m_time.size();

  if(vsize == 0)
    return(false);
  
  if(gtime < m_time[vsize-1]) {
    cout << "ApplyDelta fault 0 " << gtime << ", " << m_time[vsize-1] << endl;
    return(false);
  }

  XYGrid new_grid = m_grids[vsize-1];

  bool ok = new_grid.processDelta(str);
  if(ok) {
    m_time.push_back(gtime);
    m_grids.push_back(new_grid);
  }

  return(ok);
  
}


//---------------------------------------------------------------
// Procedure: getTimeByIndex

double GridPlot::getTimeByIndex(unsigned int index) const
{
  if(m_time.size() == 0)
    return(0);
  
  if(index < 0)
    return(m_time[0]);
  else if(index >= m_time.size())
    return(m_time[m_time.size()-1]);
  else
    return(m_time[index]);
}

//---------------------------------------------------------------
// Procedure: getGridByIndex

XYGrid GridPlot::getGridByIndex(unsigned int index) const
{
  XYGrid null_grid;
  
  if(m_grids.size() == 0)
    return(null_grid);
  
  if(index < 0)
    return(m_grids[0]);
  else if(index >= m_grids.size())
    return(m_grids[m_grids.size()-1]);
  else
    return(m_grids[index]);
}
     
//---------------------------------------------------------------
// Procedure: getGridByTime
//   Purpose: Find the grid for a given point in time.
//            If the point in time happens between to elements
//            in the time array, use the oldest one.

XYGrid GridPlot::getGridByTime(double qtime) const
{
  XYGrid null_grid;
  
  // Special case: if the instance is "empty"
  int vsize = m_time.size();
  if(vsize == 0)
    return(null_grid);

  // Special case: if the query time is outside the GridPlot
  // time range, return the extreme value.
  if(qtime >=  m_time[vsize-1])
    return(m_grids[vsize-1]);
  if(qtime <= m_time[0])
    return(m_grids[0]);
  
  // Determine highest index w/ time <= qtime
  int index = getIndexByTime(qtime);
  
  return(m_grids[index]);
}
     
//---------------------------------------------------------------
// Procedure: getIndexByTime
//   Purpose: Given a query time, determine the highest index that 
//            has a time less than or equal to the query time.
//            Search is log(n)

int GridPlot::getIndexByTime(double qtime) const
{
  int vsize = m_time.size();
  
  // Special case: if the query time is outside the NavPlot 
  // time range, return the extreme value.
  if(qtime <= m_time[0])
    return(0);
  if(qtime >= m_time[vsize-1])
    return(vsize-1);
  
  // Handle general case
  int jump  = vsize / 2;
  int index = vsize / 2;
  bool done = false;
  while(!done) {
    if(jump > 1)
      jump = jump / 2;
    if(m_time[index] <= qtime) {
      if(m_time[index+1] > qtime)
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
// Procedure: getMinTime

double GridPlot::getMinTime() const
{
  if(m_time.size() > 0)
    return(m_time[0]);
  else
    return(0);
}

//---------------------------------------------------------------
// Procedure: getMaxTime

double GridPlot::getMaxTime() const
{
  if(m_time.size() > 0)
    return(m_time[m_time.size()-1]);
  else
    return(0);
}
