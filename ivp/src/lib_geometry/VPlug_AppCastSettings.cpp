/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VPlug_AppCastSettings.cpp                            */
/*    DATE: Sep 20th, 2012                                       */
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
#include "VPlug_AppCastSettings.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

VPlug_AppCastSettings::VPlug_AppCastSettings()
{
  m_attribute_map["nodes_font_size"]      = "medium";
  m_attribute_map["procs_font_size"]      = "medium";
  m_attribute_map["appcast_font_size"]    = "small";
  m_attribute_map["appcast_color_scheme"] = "indigo";
  m_attribute_map["refresh_mode"]         = "events";

  m_gsize_map["appcast_height"] = 70;
  m_gsize_map["appcast_width"]  = 30;

  m_viewable_map["full_screen"]      = false;
  m_viewable_map["appcast_viewable"] = false;
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

bool VPlug_AppCastSettings::setParam(const string& param, string value)
{
  if(param == "full_screen") {
    return(setViewMapping(param, value));
  }

  // If full screen is on, and appcasting is being set to anything but 
  // false, then turning ON appcasting and turn OFF fullscreen
  else if(param == "appcast_viewable") {
    if(m_viewable_map["full_screen"] && (tolower(value)!="false")) {
      setViewMapping("full_screen", "false");
      setViewMapping("appcast_viewable", "true");
      return(true);
    }
    return(setViewMapping(param, value));
  }
  
  else if((param == "appcast_font_size") && (value == "bigger")) {
    if(m_attribute_map["appcast_font_size"] == "xsmall")
      setAttrMapping(param, "small");
    else if(m_attribute_map["appcast_font_size"] == "small")
      setAttrMapping(param, "medium");
    else if(m_attribute_map["appcast_font_size"] == "medium")
      setAttrMapping(param, "large");
    else if(m_attribute_map["appcast_font_size"] == "large")
      setAttrMapping(param, "xlarge");
  }

  else if((param == "appcast_font_size") && (value == "smaller")) {
    if(m_attribute_map["appcast_font_size"] == "xlarge")
      setAttrMapping(param, "large");
    else if(m_attribute_map["appcast_font_size"] == "large")
      setAttrMapping(param, "medium");
    else if(m_attribute_map["appcast_font_size"] == "medium")
      setAttrMapping(param, "small");
    else if(m_attribute_map["appcast_font_size"] == "small")
      setAttrMapping(param, "xsmall");
  }

  else if((param == "procs_font_size")   ||
	  (param == "nodes_font_size")   ||
	  (param == "appcast_font_size") ||
	  (param == "refresh_mode")) {
    return(setAttrMapping(param, value));
  }

  // Possible values: white, indigo, beige
  else if(param == "appcast_color_scheme") {
    value = tolower(stripBlankEnds(value));
    if((value == "white") || (value == "indigo") || (value == "beige"))
      return(setAttrMapping(param, value));
    else if(value == "toggle") {
      if(attribute("appcast_color_scheme") == "")
	return(setAttrMapping(param, "white"));
      if(attribute("appcast_color_scheme") == "indigo")
	return(setAttrMapping(param, "beige"));
      if(attribute("appcast_color_scheme") == "beige")
	return(setAttrMapping(param, "white"));
      if(attribute("appcast_color_scheme") == "white")
	return(setAttrMapping(param, "indigo"));
    }
  }

  else if(param == "appcast_width") {
    if(!isNumber(value)) 
      return(setSizeMapping("appcast_width", value, 20, 70));
    double dval = atof(value.c_str());
    if(dval >= 70)       setSizeMapping("appcast_width", "70");
    else if(dval >= 65)  setSizeMapping("appcast_width", "65");
    else if(dval >= 60)  setSizeMapping("appcast_width", "60");
    else if(dval >= 55)  setSizeMapping("appcast_width", "55");
    else if(dval >= 50)  setSizeMapping("appcast_width", "50");
    else if(dval >= 45)  setSizeMapping("appcast_width", "45");
    else if(dval >= 40)  setSizeMapping("appcast_width", "40");
    else if(dval >= 35)  setSizeMapping("appcast_width", "35");
    else if(dval >= 30)  setSizeMapping("appcast_width", "30");
    else if(dval >= 25)  setSizeMapping("appcast_width", "25");
    else                 setSizeMapping("appcast_width", "20");
  }

  else if(param == "appcast_height") {
    if(!isNumber(value))
      return(setSizeMapping("appcast_height", value, 30, 90));
    double dval = atof(value.c_str());
    if(dval >= 90)       setSizeMapping("appcast_height", "90");
    else if(dval >= 85)  setSizeMapping("appcast_height", "85");
    else if(dval >= 80)  setSizeMapping("appcast_height", "80");
    else if(dval >= 75)  setSizeMapping("appcast_height", "75");
    else if(dval >= 70)  setSizeMapping("appcast_height", "70");
    else if(dval >= 65)  setSizeMapping("appcast_height", "65");
    else if(dval >= 60)  setSizeMapping("appcast_height", "60");
    else if(dval >= 55)  setSizeMapping("appcast_height", "55");
    else if(dval >= 50)  setSizeMapping("appcast_height", "50");
    else if(dval >= 45)  setSizeMapping("appcast_height", "45");
    else if(dval >= 40)  setSizeMapping("appcast_height", "40");
    else if(dval >= 35)  setSizeMapping("appcast_height", "35");
    else                 setSizeMapping("appcast_height", "30");
  }
  else
    return(false);
  
  return(true);
}

//-------------------------------------------------------------
// Procedure: setViewMapping
//      Note: If toggling a parameter that has not been set before, 
//            this is interpreted as setting it now to true.


bool VPlug_AppCastSettings::setViewMapping(string param, string value)
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

bool VPlug_AppCastSettings::setSizeMapping(string attr, string gsize)
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

bool VPlug_AppCastSettings::setSizeMapping(string attr, string gsize,
					double min, double max)
{
  bool result = setSizeMapping(attr, gsize);
  if(result && (min <= max)) 
    m_gsize_map[attr] = vclip(m_gsize_map[attr], min, max);

  return(result);
}

//-------------------------------------------------------------
// Procedure: setColorMapping
//            "label", "DarkKhaki"
//            "label"  " hex, bd, b7, 6b"
//            "label", "0.741, 0.718, 0.420"

bool VPlug_AppCastSettings::setColorMapping(string attr, 
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

bool VPlug_AppCastSettings::setAttrMapping(string attr, 
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

bool VPlug_AppCastSettings::viewable(const string& attr, 
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

double VPlug_AppCastSettings::geosize(const string& attr, 
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
// Procedure: attribute
//      Note: If the attribute is not found, the returned string
//            can be determined by the optional default value.
//      Note: Attributes are case-insensitive by always converting
//            to lower case when added to the structure. The queried
//            attribute is converted to lower case only if the query
//            initially fails. 

string VPlug_AppCastSettings::attribute(const string& attr, 
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

string VPlug_AppCastSettings::colorname(const string& attr, 
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

ColorPack VPlug_AppCastSettings::geocolor(const string& attr, 
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

string VPlug_AppCastSettings::strvalue(const string& attr)
{
  if((attr == "full_screen")         ||
     (attr == "appcast_viewable")) {
    return(boolToString(viewable(attr, false)));
  }

  if((attr == "nodes_font_size")     ||
     (attr == "procs_font_size")     ||
     (attr == "appcast_font_size")   ||
     (attr == "refresh_mode")        ||
     (attr == "appcast_color_scheme"))
    return(attribute(attr));
  
  if((attr == "appcast_width")             ||
     (attr == "appcast_height"))
    return(doubleToString(geosize(attr), 0));

  return("");
}









