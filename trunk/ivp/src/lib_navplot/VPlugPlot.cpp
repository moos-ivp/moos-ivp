/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VPlugPlot.cpp                                        */
/*    DATE: Aug 9th, 2009                                        */
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

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include "VPlugPlot.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: addEvent

bool VPlugPlot::addEvent(const string& var, const string& val, double time)
{
  double latest_vplug_time = -1;
  unsigned int vsize = m_time.size();
  if(vsize > 0)
    latest_vplug_time = m_time[vsize-1];
    
  if((latest_vplug_time == -1) || (time > latest_vplug_time)) {
    VPlug_GeoShapes new_vplug;
    if(latest_vplug_time != -1) 
      new_vplug = m_vplugs[vsize-1];
    m_vplugs.push_back(new_vplug);
    m_time.push_back(time);
    vsize++;
  }

  if(var == "VIEW_POINT")
    m_vplugs[vsize-1].addPoint(val);
  else if(var == "VIEW_POLYGON")
    m_vplugs[vsize-1].addPolygon(val);
  else if(var == "VIEW_SEGLIST")
    m_vplugs[vsize-1].addSegList(val);
  else if(var == "VIEW_CIRCLE")
    m_vplugs[vsize-1].addCircle(val);
  else if(var == "GRID_CONFIG")
    m_vplugs[vsize-1].addGrid(val);
  else if(var == "GRID_DELTA")
    m_vplugs[vsize-1].updateGrid(val);
  else if(var == "VIEW_RANGE_PULSE")
    m_vplugs[vsize-1].addRangePulse(val);
  else if(var == "VIEW_MARKER")
    m_vplugs[vsize-1].addMarker(val);
  return true;
}
     
//---------------------------------------------------------------
// Procedure: getVPlugByIndex

VPlug_GeoShapes VPlugPlot::getVPlugByIndex(unsigned int index) const
{
  if(index < m_time.size())
    return(m_vplugs[index]);
  else {
    VPlug_GeoShapes null_plug;
    return(null_plug);
  }
}

//---------------------------------------------------------------
// Procedure: getVPlugByTime

VPlug_GeoShapes VPlugPlot::getVPlugByTime(double gtime) const
{
  unsigned int vsize = m_time.size();
  if(vsize == 0) {
    VPlug_GeoShapes null_plug;
    return(null_plug);
  }

  if(gtime <= m_time[0]) {
    VPlug_GeoShapes null_plug;
    return(null_plug);
  }

  if(gtime >= m_time[vsize-1])
    return(m_vplugs[vsize-1]);

  unsigned int index = getIndexByTime(gtime);
  return(m_vplugs[index]);
}
     
//---------------------------------------------------------------
// Procedure: getMinTime

double VPlugPlot::getMinTime() const
{
  if(m_time.size() > 0)
    return(m_time[0]);
  else
    return(0.0);
}

//---------------------------------------------------------------
// Procedure: getMaxTime

double VPlugPlot::getMaxTime() const
{
  if(m_time.size() > 0)
    return(m_time[m_time.size()-1]);
  else
    return(0.0);
}


//---------------------------------------------------------------
// Procedure: print

void VPlugPlot::print() const
{
  unsigned int i;
  cout << "VPlugPlot::print()" << endl;
  cout << " Vehicle Name: " << m_vehi_name << endl;
  for(i=0; i<m_time.size(); i++)
    cout << "time:" << m_time[i] << endl;
}


//---------------------------------------------------------------
// Procedure: get_index_by_time
//   Purpose: Given a time, determine the highest index that has a
//            time less than or equal to the given time.

//  2 5 9 13 14 19 23 28 31 35 43 57
//             ^                 ^
//            25                 56

unsigned int VPlugPlot::getIndexByTime(double gtime) const
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


