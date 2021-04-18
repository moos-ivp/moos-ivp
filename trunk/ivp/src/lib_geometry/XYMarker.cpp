/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYMarker.cpp                                         */
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
#include "XYMarker.h" 

using namespace std;

//---------------------------------------------------------------
// Constructor

XYMarker::XYMarker()
{
  m_x     = 0;  // meters
  m_y     = 0;  // meters
  m_width = 5;  // meters
  m_range = 0;  // meters
  m_type  = "circle";

  m_x_set     = false;
  m_y_set     = false;
  m_width_set = false;
  m_range_set = false;
  m_type_set  = false;
}

//---------------------------------------------------------------
// Procedure: set_type

bool XYMarker::set_type(string str)
{
  if((str == "circle")  ||  (str == "square") ||
     (str == "diamond") ||  (str == "efield") ||
     (str == "gateway") ||  (str == "triangle")) {
    m_type = str;
    m_type_set = true;
    return(true);
  }
  return(false);
}

//---------------------------------------------------------------
// Procedure: set_width

void XYMarker::set_width(double width)
{
  if(width < 0)
    width = 0;
  m_width = width;
  m_width_set = true;
}

//---------------------------------------------------------------
// Procedure: set_range

void XYMarker::set_range(double range)
{
  if(range < 0)
    range = 0;
  m_range = range;
  m_range_set = true;
}

//---------------------------------------------------------------
// Procedure: get_spec

string XYMarker::get_spec(string param) const
{
  string spec;

  spec += "x=";
  spec += doubleToStringX(m_x, 2);
  spec += ",y=";
  spec += doubleToStringX(m_y, 2);
  spec += ",width=";
  spec += doubleToStringX(m_width, 2);

  if(m_range_set)
    spec += ",range=" + doubleToStringX(m_range,2);

  if(color_set("primary_color")) {
    string color = get_color("primary_color").str();
    aug_spec(spec, "primary_color=" + color);
  }
  if(color_set("secondary_color")) {
    string color = get_color("secondary_color").str();
    aug_spec(spec, "secondary_color=" + color);
  }

  if(m_type != "")
    aug_spec(spec, "type=" + m_type);
  if(m_owner != "")
    aug_spec(spec, "owner=" + m_owner);

  string obj_spec = XYObject::get_spec(param);
  if(obj_spec != "")
    spec += ("," + obj_spec);
  
  return(spec);
}




