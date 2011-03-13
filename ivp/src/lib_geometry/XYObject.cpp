/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYObject.cpp                                         */
/*    DATE: July 18th, 2008                                      */
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
#include <cmath>
#include <cstdlib>
#include <cstring>
#include "XYObject.h" 

using namespace std;

//---------------------------------------------------------------
// Constructor

XYObject::XYObject()
{
  m_active      = true;
  m_time        = 0;
  m_time_set    = false;
  m_vertex_size = -1;
  m_edge_size   = -1;
}

//---------------------------------------------------------------
// Procedure: clear

void XYObject::clear()
{
  m_active      = true; 
  m_time        = 0; 
  m_time_set    = false; 
  m_label       = ""; 
  m_type        = "";
  m_source      = ""; 
  m_msg         = ""; 
  m_vertex_size = -1;
  m_edge_size   = -1;

  m_label_color.clear();
  m_vertex_color.clear();
  m_edge_color.clear();
}


//---------------------------------------------------------------
// Procedure: set_label_color()
// Procedure: set_vertex_color()
// Procedure: set_edge_color()
// Procedure: set_vertex_size()
// Procedure: set_edge_size()

void XYObject::set_label_color(const string& str)
{
  if(isColor(str))
    m_label_color.setColor(str);
}
void XYObject::set_vertex_color(const string& str)
{
  if(isColor(str))
    m_vertex_color.setColor(str);
}
void XYObject::set_edge_color(const string& str)
{
  if(isColor(str))
    m_edge_color.setColor(str);
}
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
  if(m_label_color.set())
    aug_spec(spec, "label_color=" + m_label_color.str(':')); 
  if(m_msg != "")
    aug_spec(spec, "msg=" + m_msg); 
  if(m_type != "")
    aug_spec(spec, "type=" + m_type); 
  if(m_source != "")
    aug_spec(spec, "source=" + m_source); 
  if(edge_color_set())
    aug_spec(spec, "edge_color=" + m_edge_color.str(':')); 
  if(m_vertex_color.set())
    aug_spec(spec, "vertex_color=" + m_vertex_color.str(':')); 
  if(m_time_set) {
    string time_str = doubleToStringX(m_time,2);
    aug_spec(spec, "time=" + time_str);
  } 
  if(vertex_size_set()) {
    string size_str = doubleToStringX(m_vertex_size,1);
    aug_spec(spec, "vertex_size=" + size_str);
  }
  if(edge_size_set()) {
    string size_str = doubleToStringX(m_edge_size,1); 
    aug_spec(spec, "edge_size=" + size_str);
  }
  
  return(spec);

}

//---------------------------------------------------------------
// Procedure: set_param()

bool XYObject::set_param(const string& param, const string& value)
{
  if(param == "label") 
    set_label(value);
  else if(param == "msg")
    set_msg(value);
  else if(param == "type")
    set_type(value);
  else if(param == "source")
    set_source(value);
  else if(param == "time")
    set_time(atof(value.c_str()));
  else if(param == "vertex_size")
    set_vertex_size(atof(value.c_str()));
  else if(param == "vertex_color")
    set_vertex_color(value);
  else if(param == "edge_size")
    set_edge_size(atof(value.c_str()));
  else if(param == "edge_color")
    set_edge_color(value);
  else if(param == "label_color")
    set_label_color(value);
  else if((param == "active") && isBoolean(value))
    set_active(tolower(value) == "true");
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
