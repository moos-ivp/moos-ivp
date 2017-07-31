/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VPlug_DropPoints.cpp                                 */
/*    DATE: June 13th, 2009                                      */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "VPlug_DropPoints.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

VPlug_DropPoints::VPlug_DropPoints()
{
  m_viewable_all = true;
  m_coord_mode = "as-dropped";
  m_coord_color_localg = "yellow";
  m_coord_color_latlon = "lightgreen";
}


//-----------------------------------------------------------
// Procedure: setParam()
//     Notes: The possibly accepted parameters are unlimited, but 
//            must fit one of the below patterns. The following is 
//            a list (perhaps incomplete) of acceptable parameters.
//
//         drop_point_viewable_all
//         drop_point_coords
//         drop_point_vertex_color
//         drop_point_vertex_size

bool VPlug_DropPoints::setParam(const string& param, string value)
{
  value = tolower(value);
#if 0
  if(param == "drop_point_viewable_all") {
    if((value == "true") || (value == "on"))
      m_viewable_all = true;
    else if((value == "false") || (value == "off"))
      m_viewable_all = false;
    else if(value == "toggle")
      m_viewable_all = !m_viewable_all;
  }
#endif
  if(param == "drop_point_edit") {
    if(value == "clear")
      m_points.clear();
    else if((value == "clear_last") && (m_points.size() > 0))
      m_points.pop_back();
    else
      return(false);
  }
  else if(param == "drop_point_coords") {
    unsigned int i, vsize = m_points.size();
    if(value == "as-dropped")
      for(i=0; i<vsize; i++)
	m_points[i].set_label(m_points_native[i]);
    else if(value == "lat-lon")  
      for(i=0; i<vsize; i++)
	m_points[i].set_label(m_points_latlon[i]);
    else if(value == "local-grid")  
      for(i=0; i<vsize; i++)
	m_points[i].set_label(m_points_localg[i]);
    else
      return(false);
  }
  else if(param == "drop_point_vertex_color") {
    unsigned int i, vsize = m_points.size();
    for(i=0; i<vsize; i++)
      m_points[i].set_color("vertex", param);
  }
  else if(param == "drop_point_label_color") {
    unsigned int i, vsize = m_points.size();
    for(i=0; i<vsize; i++)
      m_points[i].set_color("label", param);
  }
  else if(param == "drop_point_vertex_size") {
    if(!isNumber(value))
      return(false);
    double dval = vclip(atof(value.c_str()), 1, 20);
    unsigned int i, vsize = m_points.size();
    for(i=0; i<vsize; i++)
      m_points[i].set_vertex_size(dval);
  }
  else
    return(false);

  return(true);
}

//-----------------------------------------------------------
// Procedure: addPoint

void VPlug_DropPoints::addPoint(const XYPoint& new_point)
{
  m_points.push_back(new_point);
  m_points_latlon.push_back("");
  m_points_localg.push_back("");
  m_points_native.push_back("");
}

//-----------------------------------------------------------
// Procedure: addPoint

void VPlug_DropPoints::addPoint(const XYPoint& new_point, 
				const string& latlon_str,
				const string& localg_str,
				const string& native_str)
{
  m_points.push_back(new_point);
  m_points_latlon.push_back(latlon_str);
  m_points_localg.push_back(localg_str);
  m_points_native.push_back(native_str);

  unsigned int vsize = m_points.size();
  
  if(latlon_str == native_str)
    m_points[vsize-1].set_color("label", m_coord_color_latlon);
  else
    m_points[vsize-1].set_color("label", m_coord_color_localg);
}

//-------------------------------------------------------------
// Procedure: getPoint(int)

XYPoint VPlug_DropPoints::getPoint(unsigned int index) const
{
  if(index >= m_points.size()) {
    XYPoint null_point;
    return(null_point);
  }
  else
    return(m_points[index]);
}

//-------------------------------------------------------------
// Procedure: getCoordinates(int)

string VPlug_DropPoints::getCoordinates(unsigned int index) const
{
  if(index >= m_points.size())
    return("index-out-of-range");
  
  if(m_coord_mode == "lat-lon")
    return(m_points_latlon[index]);
  else if(m_coord_mode == "local-grid")
    return(m_points_localg[index]);
  else
    return(m_points_native[index]);
}









