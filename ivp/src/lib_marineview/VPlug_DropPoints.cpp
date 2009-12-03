/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VPlug_DropPoints.cpp                                 */
/*    DATE: June 13th, 2009                                      */
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
  if(param == "drop_point_viewable_all") {
    if((value == "true") || (value == "on"))
      m_viewable_all = true;
    else if((value == "false") || (value == "off"))
      m_viewable_all = false;
    else if(value == "toggle")
      m_viewable_all = !m_viewable_all;
  }
  else if(param == "drop_point_edit") {
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
      m_points[i].set_vertex_color(param);
  }
  else if(param == "drop_point_label_color") {
    unsigned int i, vsize = m_points.size();
    for(i=0; i<vsize; i++)
      m_points[i].set_label_color(param);
  }
  else if(param == "drop_point_vertex_size") {
    cout << "Setting drop_point_vertex_size to " << value << endl;
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
    m_points[vsize-1].set_label_color(m_coord_color_latlon);
  else
    m_points[vsize-1].set_label_color(m_coord_color_localg);
}

//-------------------------------------------------------------
// Procedure: getPoint(int)

XYPoint VPlug_DropPoints::getPoint(int index)
{
  if((index < 0) || (index >= m_points.size())) {
    XYPoint null_point;
    return(null_point);
  }
  else
    return(m_points[index]);
}

//-------------------------------------------------------------
// Procedure: getCoordinates(int)

string VPlug_DropPoints::getCoordinates(int index)
{
  if((index < 0) || (index >= m_points.size()))
    return("index-out-of-range");
  
  if(m_coord_mode == "lat-lon")
    return(m_points_latlon[index]);
  else if(m_coord_mode == "local-grid")
    return(m_points_localg[index]);
  else
    return(m_points_native[index]);
}
