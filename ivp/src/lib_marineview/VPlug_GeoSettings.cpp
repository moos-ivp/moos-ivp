/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VPlug_GeoSettings.cpp                                */
/*    DATE: Aug 8th, 2009 (From GeoShapes.cpp)                   */
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
#include <cstdlib>
#include "VPlug_GeoSettings.h"
#include "MBUtils.h"
#include "XYFormatUtilsSegl.h"
#include "XYFormatUtilsPoly.h"
#include "XYFormatUtilsPoint.h"
#include "XYFormatUtilsCircle.h"
#include "ColorParse.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

VPlug_GeoSettings::VPlug_GeoSettings()
{
  setColorMapping("polygon_edge_color", "yellow");
  setColorMapping("polygon_vertex_color", "red");
  setColorMapping("polygon_label_color", "khaki");
  setColorMapping("seglist_edge_color", "white");
  setColorMapping("seglist_vertex_color", "dark_blue");
  setColorMapping("seglist_label_color", "orange");
  setColorMapping("marker_edge_color", "black");
  setColorMapping("marker_label_color", "white");
  setColorMapping("circle_edge_color", "yellow");
  setColorMapping("grid_edge_color", "white");
  setColorMapping("point_vertex_color", "yellow");

  m_viewable_map["polygon_viewable_all"]    = true;
  m_viewable_map["polygon_viewable_labels"] = true;
  m_viewable_map["seglist_viewable_all"]    = true;
  m_viewable_map["seglist_viewable_labels"] = true;
  m_viewable_map["marker_viewable_all"]     = true;
  m_viewable_map["marker_viewable_labels"]  = true;
  m_viewable_map["point_viewable_all"]      = true;
  m_viewable_map["point_viewable_labels"]   = true;
  m_viewable_map["vector_viewable_all"]     = true;
  m_viewable_map["vector_viewable_labels"]  = true;
  m_viewable_map["grid_viewable_all"]       = true;
  m_viewable_map["grid_viewable_labels"]    = true;
  m_viewable_map["comms_pulses_viewable_all"] = true;
  m_viewable_map["circle_viewable_all"] = true;
  
  m_gsize_map["polygon_edge_width"]  = 1.0;
  m_gsize_map["polygon_vertex_size"] = 3.0;
  m_gsize_map["seglist_edge_width"]  = 1.0;
  m_gsize_map["seglist_vertex_size"] = 3.0;
  m_gsize_map["marker_edge_width"]   = 1.0;
  m_gsize_map["marker_scale"]        = 1.0;
  m_gsize_map["grid_edge_width"]     = 2.0;
  m_gsize_map["circle_edge_width"]   = 2.0;
  m_gsize_map["point_vertex_size"]   = 4.0;
  m_gsize_map["vector_length_zoom"]  = 2.0;

  m_trans_map["grid_transparency"]   = 0.3;
}

//-----------------------------------------------------------
// Procedure: setParam()
//     Notes: The possibly accepted parameters are unlimited, but 
//            must fit one of the below patterns. The following is 
//            a list (perhaps incomplete) of acceptable parameters.
//    Each accepting a "double string": 
//         polygon_line_width
//         polygon_vertex_size
//         seglist_line_width
//         seglist_vertex_size
//         grid_line_width
//         circle_line_width
//         point_vertex_size

bool VPlug_GeoSettings::setParam(const string& param, string value)
{
  if(param == "polygon_viewable_all")
    return(setViewableMapping(param, value));
  else if(param == "polygon_viewable_labels")
    return(setViewableMapping(param, value));
  else if(param == "seglist_viewable_all")
    return(setViewableMapping(param, value));
  else if(param == "seglist_viewable_labels")
    return(setViewableMapping(param, value));
  else if(param == "marker_viewable_all")
    return(setViewableMapping(param, value));
  else if(param == "marker_viewable_labels")
    return(setViewableMapping(param, value));
  else if(param == "grid_viewable_all")
    return(setViewableMapping(param, value));
  else if(param == "grid_viewable_labels")
    return(setViewableMapping(param, value));
  else if(param == "circle_viewable_all")
    return(setViewableMapping(param, value));
  else if(param == "circle_viewable_labels")
    return(setViewableMapping(param, value));
  else if(param == "point_viewable_all")
    return(setViewableMapping(param, value));
  else if(param == "point_viewable_labels")
    return(setViewableMapping(param, value));
  else if(param == "vector_viewable_all")
    return(setViewableMapping(param, value));
  else if(param == "vector_viewable_labels")
    return(setViewableMapping(param, value));
  else if(param == "grid_viewable_all")
    return(setViewableMapping(param, value));
  else if(param == "grid_viewable_labels")
    return(setViewableMapping(param, value));
  else if(param == "comms_pulses_viewable_all")
    return(setViewableMapping(param, value));
  else if(param == "circle_viewable_all")
    return(setViewableMapping(param, value));

  else if(param == "polygon_label_color")
    return(setColorMapping(param, value));
  else if(param == "seglist_label_color")
    return(setColorMapping(param, value));
  else if(param == "point_label_color")
    return(setColorMapping(param, value));
  else if(param == "marker_label_color")
    return(setColorMapping(param, value));
  else if(param == "vector_label_color")
    return(setColorMapping(param, value));
  else if(param == "circle_label_color")
    return(setColorMapping(param, value));
  else if(param == "grid_label_color")
    return(setColorMapping(param, value));

  else if(param == "polygon_edge_color")
    return(setColorMapping(param, value));
  else if(param == "polygon_vertex_color")
    return(setColorMapping(param, value));
  else if(param == "seglist_edge_color")
    return(setColorMapping(param, value));
  else if(param == "seglist_vertex_color")
    return(setColorMapping(param, value));
  else if(param == "circle_edge_color")
    return(setColorMapping(param, value));
  else if(param == "grid_edge_color")
    return(setColorMapping(param, value));
  else if(param == "point_vertex_color")
    return(setColorMapping(param, value));

  else if(param == "grid_transparency")
    return(setTransparencyMapping(param, value));

  else if(param == "vector_length_zoom")
    return(setGSizeMapping(param, value, 4));

  else if(param == "polygon_edge_width")
    return(setGSizeMapping(param, value, 10));
  else if(param == "polygon_vertex_size")
    return(setGSizeMapping(param, value, 20));
  else if(param == "seglist_edge_width")
    return(setGSizeMapping(param, value, 10));
  else if(param == "seglist_vertex_size")
    return(setGSizeMapping(param, value, 20));
  else if(param == "point_vertex_size")
    return(setGSizeMapping(param, value));
  else if(param == "marker_edge_width")
    return(setGSizeMapping(param, value));
  else if(param == "marker_scale")
    return(setGSizeMapping(param, value));

#if 0
  else if(strContains(param, "_width"))
    return(setGSizeMapping(param, value, 10));
  else if(strContains(param, "_size"))
    return(setGSizeMapping(param, value, 10));
#endif
  else
    return(false);
  
  return(true);
}

//-------------------------------------------------------------
// Procedure: setColorMapping
//            "label", "DarkKhaki"
//            "label"  " hex, bd, b7, 6b"
//            "label", "0.741, 0.718, 0.420"

bool VPlug_GeoSettings::setColorMapping(string attribute, 
					string color_str)
{
  if(!isColor(color_str))
    return(false);

  attribute = tolower(stripBlankEnds(attribute));
  color_str = stripBlankEnds(color_str);

  ColorPack cpack(color_str);
  m_color_map[attribute] = cpack;

  return(true);    
}


//-------------------------------------------------------------
// Procedure: setViewableMapping

bool VPlug_GeoSettings::setViewableMapping(string param, string value)
{
  param = tolower(param);
  value = tolower(value);

  cout << "setViewableMapping - param: " << param << "  value:" << value << endl;

  if((value == "true") || (value == "on")) {
    m_viewable_map[param] = true;
    return(true);
  }
  if((value == "false") || (value == "off")) {
    m_viewable_map[param] = false;
    return(true);
  }
  if(value != "toggle")
    return(false);
  else {
    map<string, bool>::iterator p;
    p = m_viewable_map.find(param);
    if(p == m_viewable_map.end())
      m_viewable_map[param] = true;
    else {
      bool curr_value = p->second;
      m_viewable_map[param] = !curr_value;
    }
  }


  return(true);
}


//-------------------------------------------------------------
// Procedure: setTransparencyMapping

bool VPlug_GeoSettings::setTransparencyMapping(string param, string value)
{
  // If nothing is known about the param, make a new assoc and return.
  if(m_trans_map.count(param) == 0) {
    m_trans_map[param] = 0.5;
    return(true);
  }

  if(value == "less")
    m_trans_map[param] += 0.05;
  else if(value == "more")
    m_trans_map[param] -= 0.05;
  else if(isNumber(value)) {
    double dval = atof(value.c_str());
    m_trans_map[param] += dval;
  }
  else
    return(false);

  // OpenGL Transparency should be in the range [0,1]
  if(m_trans_map[param] > 1.0)
    m_trans_map[param] = 1;
  else if(m_trans_map[param] < 0)
    m_trans_map[param] = 0;

  return(true);
}


//-------------------------------------------------------------
// Procedure: setGSizeMapping
//      Note: Can accept strings such as:
//            "delta:12",  to change by a given amount
//            "scale:0.9", to scale by a given amount
//            "12",        to set exactly to the given value

bool VPlug_GeoSettings::setGSizeMapping(string attribute, string gsize,
					     double optional_max_limit)
{
  attribute = tolower(stripBlankEnds(attribute));
  gsize = tolower(stripBlankEnds(gsize));

  double current_size = m_gsize_map[attribute];
  double new_size     = 0;

  if(strBegins(gsize, "delta:")) {
    string delta_str = biteString(gsize, ':');
    string delta_val = stripBlankEnds(gsize);
    if(!isNumber(delta_val))
      return(false);
    double delta = atof(delta_val.c_str());
    new_size = current_size + delta;
  }

  else if(strBegins(gsize, "scale:")) {
    string scale_str = biteString(gsize, ':');
    string scale_val = stripBlankEnds(gsize);
    if(!isNumber(scale_val))
      return(false);
    double scale = atof(scale_val.c_str());
    new_size = current_size * scale;
  }
  else {
    if(!isNumber(gsize))
      return(false);
    new_size = atof(gsize.c_str());
  }

  if(new_size < 0)
    new_size = 0;
  if((optional_max_limit > 0) && (new_size > optional_max_limit))
    new_size = optional_max_limit;
  
  m_gsize_map[attribute] = new_size;

  return(true);    
}

//-------------------------------------------------------------
// Procedure: geocolor
//      Note: If the attribute is not found, the returned color
//            vector can be determined by the optional def_color
//            argument. 
//      Note: Attributes are case-insensitive by always converting
//            to lower case when added to the structure. The queried
//            attribute is converted to lower case only if the query
//            initially fails. 

ColorPack VPlug_GeoSettings::geocolor(const string& attribute, 
					   string def_color)
{
  map<string, ColorPack>::iterator p;
  p = m_color_map.find(attribute);
  if(p == m_color_map.end())
    p = m_color_map.find(tolower(attribute));

  if(p != m_color_map.end())
    return(p->second);
  else {
    ColorPack cpack(def_color);
    return(cpack);
  }
}

//-----------------------------------------------------------
// Procedure: viewable
//      Note: If the attribute is not found, the returned bolean
//            can be determined by the optional default boolean value.
//      Note: Attributes are case-insensitive by always converting
//            to lower case when added to the structure. The queried
//            attribute is converted to lower case only if the query
//            initially fails. 

bool VPlug_GeoSettings::viewable(const string& attribute, 
				 bool default_value)
{
  map<string, bool>::iterator p;
  p = m_viewable_map.find(attribute);
  if(p == m_viewable_map.end())
    p = m_viewable_map.find(tolower(attribute));
  
  if(p != m_viewable_map.end())
    return(p->second);
  else 
    return(default_value);
}

//-----------------------------------------------------------
// Procedure: getGSizeMapping
//      Note: If the attribute is not found, the returned double
//            can be determined by the optional default double value.
//      Note: Attributes are case-insensitive by always converting
//            to lower case when added to the structure. The queried
//            attribute is converted to lower case only if the query
//            initially fails. 

double VPlug_GeoSettings::geosize(const string& attribute, 
				  double default_value)
{
  map<string, double>::iterator p;
  p = m_gsize_map.find(attribute);
  if(p == m_gsize_map.end())
    p = m_gsize_map.find(tolower(attribute));

  if(p != m_gsize_map.end())
    return(p->second);
  else 
    return(default_value);
}

//-----------------------------------------------------------
// Procedure: transparency
//      Note: If the attribute is not found, the returned double
//            can be determined by the optional default double value.
//      Note: Attributes are case-insensitive by always converting
//            to lower case when added to the structure. The queried
//            attribute is converted to lower case only if the query
//            initially fails. 

double VPlug_GeoSettings::transparency(const string& attribute, 
				       double default_value)
{
  if(m_trans_map.count(attribute))
    return(m_trans_map[attribute]);
  
  string lower_attribute = tolower(attribute);
  if(m_trans_map.count(lower_attribute))
    return(m_trans_map[lower_attribute]);
  
  return(default_value);
}

//-----------------------------------------------------------
// Procedure: getParamReport
//   Purpose: Return a set of strings where each is of the type:
//            "parameter = value".
//            The set returned should be enough to bring future
//            instantiations back to the same current state 
//            should the vector param-value pairs be re-applied 
//            using the setParam interface. This allows a user
//            to "save his/her preferences" in a file or some
//            other configuration block.

vector<string> VPlug_GeoSettings::getParamReport() const
{
  vector<string> svect;

  svect.push_back("// Parameters for Geometry Shapes");

  map<string, ColorPack>::const_iterator p1;
  for(p1=m_color_map.begin(); p1!= m_color_map.end(); p1++) {
    string param = p1->first;
    string value = p1->second.str();
    svect.push_back(param + " = " + value);
  }
  
  map<string, bool>::const_iterator p2;
  for(p2=m_viewable_map.begin(); p2!= m_viewable_map.end(); p2++) {
    string param = p2->first;
    string value = boolToString(p2->second);
    svect.push_back(param + " = " + value);
  }

  map<string, double>::const_iterator p3;
  for(p3=m_gsize_map.begin(); p3!= m_gsize_map.end(); p3++) {
    string param = p3->first;
    string value = dstringCompact(doubleToString(p3->second, 4));
    svect.push_back(param + " = " + value);
  }

  return(svect);
}

