/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYHazard.cpp                                         */
/*    DATE: Jan 28, 2012                                         */
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

#include <cstdlib>
#include "XYHazard.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

XYHazard::XYHazard()
{
  m_x     = 0;
  m_y     = 0;
  m_width = -1;  // Indicates unspecified by the user
}

//-----------------------------------------------------------
// Procedure: setShape

bool XYHazard::setShape(string shape_str)
{
  shape_str = tolower(shape_str);
  if((shape_str == "circle") || (shape_str == "diamond") ||
     (shape_str == "square") || (shape_str == "triangle") ||
     (shape_str == "efield") || (shape_str == "gateway")) {
    m_shape = shape_str;
    return(true);
  }
  return(false);
}

//-----------------------------------------------------------
// Procedure: setWidth

bool XYHazard::setWidth(string width)
{
  if(!isNumber(width))
    return(false);

  double double_width = atof(width.c_str());
  if(double_width < 0)
    return(false);

  m_width = double_width;
  return(true);
}

//-----------------------------------------------------------
// Procedure: setColor()

bool XYHazard::setColor(string color_str)
{
  if(!isColor(color_str))
    return(false);
  m_color = color_str;
  return(true);
}

//-----------------------------------------------------------
// Procedure: getSpec()

string XYHazard::getSpec() const
{
  string str;
  str += "x=" + doubleToStringX(m_x);
  str += ",y=" + doubleToStringX(m_y);
  if(m_type != "")
    str += ",type=" + m_type;
  if(m_label != "")
    str += ",label=" + m_label;
  if(m_color != "")
    str += ",color=" + m_color;
  if(m_width >= 0)
    str += ",width=" + doubleToStringX(m_width);
  
  return(str);
}

