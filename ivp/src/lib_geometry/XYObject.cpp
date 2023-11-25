/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYObject.cpp                                         */
/*    DATE: July 18th, 2008                                      */
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
#include <cmath>
#include <cstdlib>
#include <cstring>
#include "XYObject.h" 

using namespace std;

//---------------------------------------------------------------
// Constructor

XYObject::XYObject()
{
  m_active       = true;
  m_time         = 0;
  m_vertex_size  = -1;
  m_edge_size    = -1;
  m_transparency = 0;
  m_duration     = -1;
  
  m_time_set         = false;
  m_transparency_set = false;
  m_duration_set     = false;
}

//---------------------------------------------------------------
// Procedure: clear

void XYObject::clear()
{
  m_active       = true; 
  m_time         = 0; 
  m_vertex_size  = -1;
  m_edge_size    = -1;
  m_transparency = 0;
  m_duration     = -1;

  m_time_set     = false; 
  m_transparency_set = false;
  m_duration_set     = false;

  m_label        = ""; 
  m_msg          = ""; 
  m_id           = ""; 

  m_color_map.clear();
}

//---------------------------------------------------------------
// Procedure: set_color()

void XYObject::set_color(const string& key, const string& color)
{
  ColorPack cpack(color);
  if(cpack.set())
    m_color_map[key] = cpack;
}

//---------------------------------------------------------------
// Procedure: set_color()

void XYObject::set_color(const string& key, const ColorPack& color)
{
  m_color_map[key] = color;
}

//---------------------------------------------------------------
// Procedure: get_color()

ColorPack XYObject::get_color(const string& key) const
{
  ColorPack return_cpack;

  map<string, ColorPack>::const_iterator p;
  p = m_color_map.find(key);
  if(p != m_color_map.end())
    return_cpack = p->second;

  return(return_cpack);
}

//---------------------------------------------------------------
// Procedure: get_color()

string XYObject::get_color_str(const string& key) const
{
  ColorPack return_cpack;

  map<string, ColorPack>::const_iterator p;
  p = m_color_map.find(key);
  if(p != m_color_map.end())
    return_cpack = p->second;

  return(return_cpack.str());
}

//---------------------------------------------------------------
// Procedure: color_set()

bool XYObject::color_set(const string& key) const
{
  return(m_color_map.count(key) > 0);
}


//---------------------------------------------------------------
// Procedure: set_vertex_size()
// Procedure: set_edge_size()

void XYObject::set_vertex_size(double val)
{
  if(val >= 0) 
    m_vertex_size = val;
}
void XYObject::set_edge_size(double val)
{
  if(val >= 0) 
    m_edge_size = val;
}

//---------------------------------------------------------------
// Procedure: set_transparency
//   Purpose: Set a drawing hint for the transparency used in rendering
//            the fill color of the polygon. 0 is invisible, 1 is opaque.

void XYObject::set_transparency(double transparency)
{
  if(transparency < 0)
    transparency = 0;
  else if(transparency > 1)
    transparency = 1;
  
  m_transparency = transparency;
  m_transparency_set = true;
}

//---------------------------------------------------------------
// Procedure: set_duration()
//   Purpose: Set a drawing hint for object duration. -1 is the 
//            default and indicates that the object should persist 
//            indefinitely.

void XYObject::set_duration(double duration)
{
  if(duration < 0)
    duration = -1;
  
  m_duration = duration;
  m_duration_set = true;
}

//---------------------------------------------------------------
// Procedure: set_vertex_color_size()
//   Purpose: Convenience function

void XYObject::set_vertex_color_size(string vcolor, double vsize)
{
  set_vertex_color(vcolor);
  set_vertex_size(vsize);
}

//---------------------------------------------------------------
// Procedure: get_spec()

std::string XYObject::get_spec(string param) const
{
  string spec;
  
  param = tolower(param);

  // If the caller explicitly wants to declare the active state, then
  // respect it. Otherwise declare the active state only if false, 
  // since unspecified is interpreted as active=true.
  if(param == "active=true") 
    spec += "active=true";
  else if(param == "active=false") 
    spec += "active=false";
  else {
    if(m_active == false)
      spec += "active=false";
  }
   
  if(m_label != "")
    aug_spec(spec, "label=" + m_label); 
  if(m_msg != "")
    aug_spec(spec, "msg=" + m_msg); 
  if(m_id != "")
    aug_spec(spec, "id=" + m_id); 
  if(color_set("label"))
    aug_spec(spec, "label_color=" + get_color("label").str(':'));
  if(color_set("edge"))
    aug_spec(spec, "edge_color=" + get_color("edge").str(':')); 
  if(color_set("vertex"))
    aug_spec(spec, "vertex_color=" + get_color("vertex").str(':')); 
  if(color_set("fill"))
    aug_spec(spec, "fill_color=" + get_color("fill").str(':')); 

  if(m_time_set) {
    string time_str = doubleToStringX(m_time,2);
    aug_spec(spec, "time=" + time_str);
  } 
  if(m_duration_set) {
    string duration_str = doubleToStringX(m_duration,2);
    aug_spec(spec, "duration=" + duration_str);
  } 
  if(vertex_size_set()) {
    string size_str = doubleToStringX(m_vertex_size,1);
    aug_spec(spec, "vertex_size=" + size_str);
  }
  if(edge_size_set()) {
    string size_str = doubleToStringX(m_edge_size,1); 
    aug_spec(spec, "edge_size=" + size_str);
  }
  if(m_transparency_set) {
    string trans_str = doubleToStringX(m_transparency,2); 
    aug_spec(spec, "fill_transparency=" + trans_str);
  }
  
  return(spec);
}

//---------------------------------------------------------------
// Procedure: set_param()

bool XYObject::set_param(const string& param, const string& value)
{
  if(param == "label") 
    set_label(value);
  else if(param == "vertex_size")
    set_vertex_size(atof(value.c_str()));
  else if(param == "vertex_color")
    set_color("vertex", value);
  else if(param == "type")
    set_type(value);
  else if(param == "msg")
    set_msg(value);
  else if(param == "id")
    set_id(value);
  else if(param == "time")
    set_time(atof(value.c_str()));
  else if((param == "duration") || (param == "dur"))
    set_duration(atof(value.c_str()));
  else if(param == "edge_size")
    set_edge_size(atof(value.c_str()));

  else if(param == "edge_color")
    set_color("edge", value);
  else if(param == "label_color")
    set_color("label", value);
  else if(param == "fill_color")
    set_color("fill", value);
  else if(param == "fill_transparency")
    set_transparency(atof(value.c_str()));

  else if((param == "active") && isBoolean(value))
    set_active(tolower(value) == "true");

  // "source" and "type" are deprecated fields. We don't want to return false
  // if we find one of these, so check here
  else if((param == "source") || (param == "type"))
    return(true);

  else
    return(false);
  return(true);
}


//---------------------------------------------------------------
// Procedure: aug_spec()
//   Purpose: Just augment the given string with the new part, adding
//            a comma separator when necessary.

void XYObject::aug_spec(string& orig, string new_part) const
{
  if(orig != "")
    orig += ",";
  orig += new_part;
}

//---------------------------------------------------------------
// Procedure: expired()

bool XYObject::expired(double curr_time) const
{
  if(!m_duration_set || !m_time_set)
    return(false);
  if((m_duration < 0) || (m_time <= 0))
    return(false);

  double elapsed = curr_time - m_time;
  if(elapsed <= m_duration)
    return(false);

  return(true);
}










