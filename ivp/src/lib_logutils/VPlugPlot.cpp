/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: VPlugPlot.cpp                                        */
/*    DATE: Aug 9th, 2009                                        */
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

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include "VPlugPlot.h"
#include "MBUtils.h"
#include "LogUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor()

VPlugPlot::VPlugPlot()
{
  // Init config vars
  m_binval = 0;

  // Init state vars
  m_view_point_cnt   = 0;
  m_view_polygon_cnt = 0;
  m_view_seglist_cnt = 0;
  m_view_seglr_cnt   = 0;
  m_view_circle_cnt  = 0;
  m_view_arrow_cnt   = 0;
  m_grid_config_cnt  = 0;
  m_grid_delta_cnt   = 0;
  m_view_range_pulse_cnt = 0;
  m_view_comms_pulse_cnt = 0;
  m_view_marker_cnt = 0;
}

//---------------------------------------------------------------
// Procedure: addEvent()
//      Note: Likely called by:
//            Populator_VPlugPlots::populateFromEntries() or
//            Populator_VPlugPlots::populateFromEntry()

bool VPlugPlot::addEvent(const string& var, const string& val,
			 double time)
{
  double latest_vplug_time = -1;
  unsigned int vsize = m_time.size();
  if(vsize > 0)
    latest_vplug_time = m_time[vsize-1];

  if((latest_vplug_time == -1) ||
     (time > latest_vplug_time + m_binval)) {
    VPlug_GeoShapes new_vplug;
    if(latest_vplug_time != -1) 
      new_vplug = m_vplugs[vsize-1];
    m_vplugs.push_back(new_vplug);
    m_time.push_back(time);
    vsize++;
  }

  if(var == "VIEW_POINT") {
    m_vplugs[vsize-1].addPoint(val);
    m_view_point_cnt++;
  }
  else if(var == "VIEW_POLYGON") {
    m_vplugs[vsize-1].addPolygon(val);
    m_view_polygon_cnt++;
  }
  else if(var == "VIEW_SEGLIST") {
    m_vplugs[vsize-1].addSegList(val);
    m_view_seglist_cnt++;
  }
  else if(var == "VIEW_SEGLR") {
    m_vplugs[vsize-1].addSeglr(val);
    m_view_seglr_cnt++;
  }
  else if(var == "VIEW_CIRCLE") {
    m_vplugs[vsize-1].addCircle(val);
    m_view_circle_cnt++;
  }
  else if(var == "VIEW_ARROW") {
    m_vplugs[vsize-1].addArrow(val);
    m_view_arrow_cnt++;
  }
  else if(var == "GRID_CONFIG") {
    m_vplugs[vsize-1].addGrid(val);
    m_grid_config_cnt++;
  }
  else if(var == "GRID_DELTA") {
    m_vplugs[vsize-1].updateGrid(val);
    m_grid_delta_cnt++;
  }
  else if(var == "VIEW_RANGE_PULSE") {
    m_vplugs[vsize-1].addRangePulse(val);
    m_view_range_pulse_cnt++;
  }
  else if(var == "VIEW_COMMS_PULSE") {
    m_vplugs[vsize-1].addCommsPulse(val);
    m_view_comms_pulse_cnt++;
  }
  else if(var == "VIEW_MARKER") {
    m_vplugs[vsize-1].addMarker(val);
    m_view_marker_cnt++;
  }
  else if(var == "VIEW_TEXTBOX") {
    m_vplugs[vsize-1].addTextBox(val);
    m_view_textbox_cnt++;
  }
  return true;
}
     
//---------------------------------------------------------------
// Procedure: setBinVal()

void VPlugPlot::setBinVal(double binval)
{
  if(binval >= 0)
    m_binval = binval;
}

//---------------------------------------------------------------
// Procedure: getVPlugByIndex()

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
// Procedure: getVPlugByTime()

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

  unsigned int index = getIndexByTime(m_time, gtime);
  return(m_vplugs[index]);
}
     
//---------------------------------------------------------------
// Procedure: getMinTime()

double VPlugPlot::getMinTime() const
{
  if(m_time.size() > 0)
    return(m_time[0]);
  else
    return(0.0);
}

//---------------------------------------------------------------
// Procedure: getMaxTime()

double VPlugPlot::getMaxTime() const
{
  if(m_time.size() > 0)
    return(m_time[m_time.size()-1]);
  else
    return(0.0);
}


//---------------------------------------------------------------
// Procedure: print()

void VPlugPlot::print() const
{
  unsigned int i;
  cout << "VPlugPlot::print()" << endl;
  cout << " Vehicle Name: " << m_vehi_name << endl;
  for(i=0; i<m_time.size(); i++)
    cout << "time:" << m_time[i] << endl;
}

//---------------------------------------------------------------
// Procedure: summary()

void VPlugPlot::summary(string indent) const
{
  string str_point = uintToCommaString(m_view_point_cnt);
  string str_polygon = uintToCommaString(m_view_polygon_cnt);
  string str_seglist = uintToCommaString(m_view_seglist_cnt);
  string str_seglr = uintToCommaString(m_view_seglr_cnt);
  string str_circle = uintToCommaString(m_view_circle_cnt);
  string str_arrow = uintToCommaString(m_view_arrow_cnt);
  string str_grid_config = uintToCommaString(m_grid_config_cnt);
  string str_grid_delta = uintToCommaString(m_grid_delta_cnt);
  string str_range_pulse = uintToCommaString(m_view_range_pulse_cnt);
  string str_comms_pulse = uintToCommaString(m_view_comms_pulse_cnt);
  string str_marker = uintToCommaString(m_view_marker_cnt);

  if(m_view_point_cnt > 0)
    cout << indent << "VIEW_POINT total: " << str_point << endl;
  if(m_view_seglist_cnt > 0)
    cout << indent << "VIEW_SEGLIST total: " << str_seglist << endl;
  if(m_view_seglr_cnt > 0)
    cout << indent << "VIEW_SEGLR total: " << str_seglr << endl;
  if(m_view_circle_cnt > 0)
    cout << indent << "VIEW_CIRCLE total: " << str_circle << endl;
  if(m_view_arrow_cnt > 0)
    cout << indent << "VIEW_ARROW total: " << str_arrow << endl;
  if(m_grid_config_cnt > 0)
    cout << indent << "GRID_CONFIG total: " << str_grid_config << endl;
  if(m_grid_delta_cnt > 0)
    cout << indent << "GRID_DELTA total: " << str_grid_delta << endl;
  if(m_view_range_pulse_cnt > 0)
    cout << indent << "RANGE_PULSE total: " << str_range_pulse << endl;
  if(m_view_comms_pulse_cnt > 0)
    cout << indent << "COMMS_PULSE total: " << str_comms_pulse << endl;
  if(m_view_marker_cnt > 0)
    cout << indent << "VIEW_MARKER total: " << str_marker << endl;

}

//---------------------------------------------------------------
// Procedure: applySkew()

void VPlugPlot::applySkew(double skew)
{
  for(unsigned int i=0; i<m_time.size(); i++)
    m_time[i] += skew;
}
     

