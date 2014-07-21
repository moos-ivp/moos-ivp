/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYMarker.cpp                                         */
/*    DATE: July 18th, 2008                                      */
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

#include <iostream>
#include "XYMarker.h" 

using namespace std;

//---------------------------------------------------------------
// Constructor

XYMarker::XYMarker()
{
  m_x     = 0;
  m_y     = 0;
  m_width = 0;
  m_type  = "circle";
}

//---------------------------------------------------------------
// Procedure: set_type

bool XYMarker::set_type(const string& str)
{
  if((str == "circle")  ||  (str == "square") ||
     (str == "diamond") ||  (str == "efield") ||
     (str == "gateway") ||  (str == "triangle")) {
    m_type = str;
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

  string obj_spec = XYObject::get_spec(param);
  if(obj_spec != "")
    spec += ("," + obj_spec);
  
  return(spec);
}





