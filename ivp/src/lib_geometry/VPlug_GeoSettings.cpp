/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VPlug_GeoSettings.cpp                                */
/*    DATE: Aug 8th, 2009 (From GeoShapes.cpp)                   */
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
#include "VPlug_GeoSettings.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

VPlug_GeoSettings::VPlug_GeoSettings()
{
  setColorMapping("drop_point_vertex_color", "yellow");
  setColorMapping("datum_color", "red");

  m_attribute_map["drop_point_coords"] = "as-dropped";

  m_viewable_map["polygon_viewable_all"]     = true;
  m_viewable_map["polygon_viewable_labels"]  = true;
  m_viewable_map["seglist_viewable_all"]     = true;
  m_viewable_map["seglist_viewable_labels"]  = true;
  m_viewable_map["seglr_viewable_all"]       = true;
  m_viewable_map["seglr_viewable_labels"]    = true;
  m_viewable_map["drop_point_viewable_all"]  = true;
  m_viewable_map["marker_viewable_all"]      = true;
  m_viewable_map["marker_viewable_labels"]   = true;
  m_viewable_map["point_viewable_all"]       = true;
  m_viewable_map["point_viewable_labels"]    = true;
  m_viewable_map["vector_viewable_all"]      = true;
  m_viewable_map["vector_viewable_labels"]   = true;
  m_viewable_map["grid_viewable_all"]        = true;
  m_viewable_map["grid_viewable_labels"]     = true;
  m_viewable_map["comms_pulse_viewable_all"] = true;
  m_viewable_map["node_pulse_viewable_all"]  = true;
  m_viewable_map["range_pulse_viewable_all"] = true;
  m_viewable_map["circle_viewable_all"]      = true;
  m_viewable_map["circle_viewable_labels"]   = true;
  m_viewable_map["oparea_viewable_all"]      = true;
  m_viewable_map["oparea_viewable_labels"]   = true;
  m_viewable_map["tiff_viewable"]            = true;
  m_viewable_map["hash_viewable"]            = false;
  m_viewable_map["datum_viewable"]           = false;
  
  m_gsize_map["hash_delta"]             = 100;
  m_gsize_map["marker_edge_width"]      = 1.0;
  m_gsize_map["marker_scale"]           = 1.0;
  m_gsize_map["grid_edge_width"]        = 2.0;
  m_gsize_map["drop_point_vertex_size"] = 2.0;
  m_gsize_map["vector_length_zoom"]     = 2.0;
  m_gsize_map["datum_size"]             = 2;
  m_opaque_map["grid_opaqueness"]       = 0.3;

  m_attribute_map["polygon_label_pos"] = "top";
}

//-----------------------------------------------------------
// Procedure: setParam()
//     Notes: The possibly accepted parameters are unlimited, but 
//            must fit one of the below patterns. The following is 
//            a list (perhaps incomplete) of acceptable parameters.
//    Each accepting a "double string": 
//         polygon_line_width
//         seglist_line_width
//         grid_line_width
//         circle_line_width
//         point_vertex_size

bool VPlug_GeoSettings::setParam(string param, string value)
{
  // A bit of backward compatibility support
  if(param == "tiff_view")  param = "tiff_viewable";

  if((param == "polygon_viewable_all")     ||
     (param == "polygon_viewable_labels")  ||
     (param == "seglist_viewable_all")     ||
     (param == "seglist_viewable_labels")  ||
     (param == "seglr_viewable_all")       ||
     (param == "seglr_viewable_labels")    ||
     (param == "drop_point_viewable_all")  ||
     (param == "marker_viewable_all")      ||
     (param == "marker_viewable_labels")   ||
     (param == "grid_viewable_all")        ||
     (param == "grid_viewable_labels")     ||
     (param == "circle_viewable_all")      ||
     (param == "circle_viewable_labels")   ||
     (param == "point_viewable_all")       ||
     (param == "point_viewable_labels")    ||
     (param == "vector_viewable_all")      ||
     (param == "vector_viewable_labels")   ||
     (param == "oparea_viewable_all")      ||
     (param == "oparea_viewable_labels")   ||
     (param == "datum_viewable")           ||
     (param == "hash_viewable")            ||
     (param == "tiff_viewable")            ||
     (param == "range_pulse_viewable_all") ||
     (param == "comms_pulse_viewable_all") ||
     (param == "node_pulse_viewable_all"))
     return(setViewMapping(param, value));
  
  else if((param == "vector_label_color")      ||
	  (param == "drop_point_vertex_color") ||
	  (param == "datum_color"))
    return(setColorMapping(param, value));

  else if(param == "grid_opaqueness")
    return(setOpaqMapping(param, value));

  else if(param == "vector_length_zoom")
    return(setSizeMapping(param, value, 0, 4));
  else if(param == "hash_delta")
    return(setSizeMapping(param, value, 10, 10000));

  else if(param == "marker_edge_width")
    return(setSizeMapping(param, value, 1, 10));
  else if(param == "marker_scale")
    return(setSizeMapping(param, value, 0.1, 100));
  else if(param == "datum_size")
    return(setSizeMapping(param, value, 1, 10));
  else if(param == "drop_point_vertex_size")
    return(setSizeMapping(param, value, 1, 10));

  else if(param == "drop_point_coords") {
    m_attribute_map[param] = value;
    return(true);
  }
  else if((param == "polygon_label_pos") &&
	  ((tolower(value)=="top") || (tolower(value)=="mid"))) {
    m_attribute_map[param] = value;
    return(true);
  }
  else
    return(false);
  
  return(true);
}

//-------------------------------------------------------------
// Procedure: setViewMapping
//      Note: If toggling a parameter that has not been set before, 
//            this is interpreted as setting it now to true.


bool VPlug_GeoSettings::setViewMapping(string param, string value)
{
  param = tolower(param);
  value = tolower(value);

  if((value == "true") || (value == "on")) 
    m_viewable_map[param] = true;
  else if((value == "false") || (value == "off")) 
    m_viewable_map[param] = false;
  else if(value == "toggle") {
    map<string, bool>::iterator p;
    p = m_viewable_map.find(param);
    if(p == m_viewable_map.end())
      m_viewable_map[param] = true;
    else {
      bool curr_value = p->second;
      m_viewable_map[param] = !curr_value;
    }
  }
  else
    return(false);

  return(true);
}


//-------------------------------------------------------------
// Procedure: setSizeMapping
//      Note: Can accept strings such as:
//            "delta:12",  to change by a given amount
//            "scale:0.9", to scale by a given amount
//            "12",        to set exactly to the given value

bool VPlug_GeoSettings::setSizeMapping(string attr, string gsize)
{
  attr   = tolower(stripBlankEnds(attr));
  gsize  = tolower(stripBlankEnds(gsize));

  double current_size = m_gsize_map[attr];
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
  
  m_gsize_map[attr] = new_size;

  return(true);    
}


//-------------------------------------------------------------
// Procedure: setSizeMapping
//      Note: same as setSizeMapping(string, string) except with clipping
//            performed afterwards.

bool VPlug_GeoSettings::setSizeMapping(string attr, string gsize,
					double min, double max)
{
  bool result = setSizeMapping(attr, gsize);
  if(result && (min <= max)) 
    m_gsize_map[attr] = vclip(m_gsize_map[attr], min, max);

  return(result);
}

//-------------------------------------------------------------
// Procedure: setOpaqMapping

bool VPlug_GeoSettings::setOpaqMapping(string param, string value)
{
  // If nothing is known about the param, make a new assoc to allow
  // the "more" and "less" cases proceed.
  if(m_opaque_map.count(param) == 0) 
    m_opaque_map[param] = 0.5;

  if(value == "less")
    m_opaque_map[param] -= 0.1;
  else if(value == "more")
    m_opaque_map[param] += 0.1;
  else if(isNumber(value)) {
    double dval = atof(value.c_str());
    m_opaque_map[param] = dval;
  }
  else
    return(false);

  // OpenGL Transparency should be in the range [0,1]
  if(m_opaque_map[param] > 1.0)
    m_opaque_map[param] = 1;
  else if(m_opaque_map[param] < 0)
    m_opaque_map[param] = 0;

  return(true);
}


//-------------------------------------------------------------
// Procedure: setColorMapping
//            "label", "DarkKhaki"
//            "label"  " hex, bd, b7, 6b"
//            "label", "0.741, 0.718, 0.420"

bool VPlug_GeoSettings::setColorMapping(string attr, 
					string color_str)
{
  if(!isColor(color_str))
    return(false);
  
  attr = tolower(attr);
  
  ColorPack cpack(color_str);
  m_color_pck_map[attr] = cpack;
  m_color_str_map[attr] = color_str;

  return(true);    
}

//-------------------------------------------------------------
// Procedure: setAttrMapping

bool VPlug_GeoSettings::setAttrMapping(string attr, 
				       string value)
{
  m_attribute_map[attr] = value;
  return(true);    
}


//-----------------------------------------------------------
// Procedure: viewable
//      Note: If the attribute is not found, the returned bolean
//            can be determined by the optional default boolean value.
//      Note: Attributes are case-insensitive by always converting
//            to lower case when added to the structure. The queried
//            attribute is converted to lower case only if the query
//            initially fails. 

bool VPlug_GeoSettings::viewable(const string& attr, 
				 bool default_value) const
{
  map<string, bool>::const_iterator p;
  p = m_viewable_map.find(attr);
  if(p == m_viewable_map.end())
    p = m_viewable_map.find(tolower(attr));
  
  if(p != m_viewable_map.end())
    return(p->second);
  else 
    return(default_value);
}

//-----------------------------------------------------------
// Procedure: geosize
//      Note: If the attribute is not found, the returned double
//            can be determined by the optional default double value.
//      Note: Attributes are case-insensitive by always converting
//            to lower case when added to the structure. The queried
//            attribute is converted to lower case only if the query
//            initially fails. 

double VPlug_GeoSettings::geosize(const string& attr, 
				  double default_value) const
{
  map<string, double>::const_iterator p;
  p = m_gsize_map.find(attr);
  if(p == m_gsize_map.end())
    p = m_gsize_map.find(tolower(attr));

  if(p != m_gsize_map.end())
    return(p->second);
  else 
    return(default_value);
}

//-----------------------------------------------------------
// Procedure: opaqueness
//      Note: If the attribute is not found, the returned double
//            can be determined by the optional default double value.
//      Note: Attributes are case-insensitive by always converting
//            to lower case when added to the structure. The queried
//            attribute is converted to lower case only if the query
//            initially fails. 

double VPlug_GeoSettings::opaqueness(const string& attr, 
				     double default_value) const
{
  map<string, double>::const_iterator p;
  p = m_opaque_map.find(attr);
  if(p == m_opaque_map.end())
    p = m_opaque_map.find(tolower(attr));

  if(p != m_opaque_map.end())
    return(p->second);
  else 
    return(default_value);
}

//-----------------------------------------------------------
// Procedure: attribute
//      Note: If the attribute is not found, the returned string
//            can be determined by the optional default value.
//      Note: Attributes are case-insensitive by always converting
//            to lower case when added to the structure. The queried
//            attribute is converted to lower case only if the query
//            initially fails. 

string VPlug_GeoSettings::attribute(const string& attr, 
				    string default_str) const
{
  map<string, string>::const_iterator p;
  p = m_attribute_map.find(attr);
  if(p == m_attribute_map.end())
    p = m_attribute_map.find(tolower(attr));

  if(p != m_attribute_map.end())
    return(p->second);
  else 
    return(default_str);
}

//-------------------------------------------------------------
// Procedure: colorname
//      Note: If the attribute is not found, the returned color
//            vector can be determined by the optional def_color
//            argument. 
//      Note: Attributes args are case-insensitive by always converting
//            to lower case when added to the structure. The queried
//            attribute is converted to lower case only if the query
//            initially fails. 

string VPlug_GeoSettings::colorname(const string& attr, 
				    string def_color) const
{
  map<string, string>::const_iterator p;
  p = m_color_str_map.find(attr);
  if(p == m_color_str_map.end())
    p = m_color_str_map.find(tolower(attr));

  if(p != m_color_str_map.end())
    return(p->second);
  else
    return(def_color);
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

ColorPack VPlug_GeoSettings::geocolor(const string& attr, 
				      string def_color) const
{
  map<string, ColorPack>::const_iterator p;
  p = m_color_pck_map.find(attr);
  if(p == m_color_pck_map.end())
    p = m_color_pck_map.find(tolower(attr));

  if(p != m_color_pck_map.end())
    return(p->second);
  else {
    ColorPack cpack(def_color);
    return(cpack);
  }
}


//-------------------------------------------------------------
// Procedure: strvalue

string VPlug_GeoSettings::strvalue(string attr)
{
  // A bit of backward compatibility support
  if(attr == "tiff_view")  attr = "tiff_viewable";

  if((attr == "polygon_viewable_all")     ||
     (attr == "polygon_viewable_labels")  ||
     (attr == "seglist_viewable_all")     ||
     (attr == "seglist_viewable_labels")  ||
     (attr == "seglr_viewable_all")       ||
     (attr == "seglr_viewable_labels")    ||
     (attr == "point_viewable_all")       ||
     (attr == "point_viewable_labels")    ||
     (attr == "vector_viewable_all")      ||
     (attr == "vector_viewable_labels")   ||
     (attr == "grid_viewable_all")        ||
     (attr == "grid_viewable_labels")     ||
     (attr == "circle_viewable_all")      ||
     (attr == "circle_viewable_labels")   ||
     (attr == "oparea_viewable_all")      ||
     (attr == "oparea_viewable_labels")   ||
     (attr == "datum_viewable")           ||
     (attr == "hash_viewable")            ||
     (attr == "tiff_viewable")            ||
     (attr == "drop_point_viewable_all")  ||
     (attr == "marker_viewable_all")      ||
     (attr == "marker_viewable_labels")   ||
     (attr == "range_pulse_viewable_all") ||
     (attr == "node_pulse_viewable_all")  ||
     (attr == "comms_pulse_viewable_all")) {
    return(boolToString(viewable(attr, false)));
  }

  if(attr == "grid_opaqueness")
    return(doubleToString(opaqueness(attr, 0.5), 1));

  if(attr == "drop_point_coords") 
    return(attribute("drop_point_coords"));
  
  if((attr == "datum_size")             ||
     (attr == "drop_point_vertex_size") ||
     (attr == "hash_delta"))
    return(intToString(geosize(attr)));

  if(attr == "datum_color")
    return(m_color_str_map["datum_color"]);

  return("");
}









