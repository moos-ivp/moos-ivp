/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYHazard.cpp                                         */
/*    DATE: Jan 28, 2012                                         */
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

#include <cstdlib>
#include "XYHazard.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

XYHazard::XYHazard()
{
  m_x  = 0;
  m_y  = 0;
  m_hr = 0.5;

  m_aspect = 0;
  m_aspect_rng_min = 0;
  m_aspect_rng_max = 0;

  m_x_set  = false;
  m_y_set  = false;
  m_hr_set = false;

  m_aspect_set = false;
  m_aspect_rng_min_set = false;
  m_aspect_rng_max_set = false;

  // Drawing hints
  m_width  = -1;  // Indicates unspecified by the user

}

//-----------------------------------------------------------
// Procedure: setAspect

bool XYHazard::setAspect(double aspect)
{
  if((aspect < 0) || (aspect >= 360))
    return(false);

  m_aspect = aspect;
  m_aspect_set = true;
  return(true);
}

//-----------------------------------------------------------
// Procedure: setAspectRange
//      Note: Rather than call the individual setRange* functions, we
//            handle this way to make sure that if one of the two 
//            ranges is faulty, no action whatsoever is taken.
//
// Constraints: min >= 0
//              max >= 0
//              min <= max
//              (min+max) <= 90

bool XYHazard::setAspectRange(double min, double max)
{
  if(min < 0)
    return(false);
  if(max < 0)
    return(false);
  if(min > max)
    return(false);
  if((min+max) > 90)
    return(false);
  
  m_aspect_rng_min = min;
  m_aspect_rng_max = max;
  m_aspect_rng_min_set = true;
  m_aspect_rng_max_set = true;
  return(true);
}

//-----------------------------------------------------------
// Procedure: setAspectRangeMin
// 
// Constraints: min >= 0
//              max >= 0
//              min <= max
//              (min+max) <= 90

bool XYHazard::setAspectRangeMin(double min)
{
  if((min < 0) || (min > 90))
    return(false);
  
  if(m_aspect_rng_max_set) {
    if(min > m_aspect_rng_max)
      return(false);
    if((min + m_aspect_rng_max) > 90)
      return(false);
  }

  m_aspect_rng_min = min;
  m_aspect_rng_min_set = true;
  return(true);
}

//-----------------------------------------------------------
// Procedure: setAspectRangeMax
// 
// Constraints: min >= 0
//              max >= 0
//              min <= max
//              (min+max) <= 90

bool XYHazard::setAspectRangeMax(double max)
{
  if((max < 0) || (max > 90))
    return(false);

  if(m_aspect_rng_min_set) {
    if(max < m_aspect_rng_min)
      return(false);
    if((max + m_aspect_rng_min) > 90)
      return(false);
  }

  m_aspect_rng_max = max;
  m_aspect_rng_max_set = true;
  return(true);
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
// Procedure: incCounter

void XYHazard::incCounter(unsigned int amt, string label)
{
  m_counters[label] += amt;
}

//-----------------------------------------------------------
// Procedure: setCounter

void XYHazard::setCounter(unsigned int amt, string label)
{
  m_counters[label] = amt;
}

//-----------------------------------------------------------
// Procedure: setCounter

unsigned int XYHazard::getCounter(string label)
{
  if(m_counters.count(label))
    return(m_counters[label]);
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: getSpec()
//      Note: The noshow argument contains a list of comma-separated
//            fields to NOT include in the string specification

string XYHazard::getSpec(string noshow) const
{
  string str;
  if(!strContains(noshow,"xpos"))
    str += "x=" + doubleToStringX(m_x,2);

  if(!strContains(noshow,"ypos"))
    str += ",y=" + doubleToStringX(m_y,2);

  if((m_label != "") && !strContains(noshow,"label"))
    str += ",label=" + m_label;

  if((m_type != "") && !strContains(noshow,"type"))
    str += ",type=" + m_type;

  if((m_color != "") && !strContains(noshow,"color"))
    str += ",color=" + m_color;

  if((m_shape != "") && !strContains(noshow,"shape"))
    str += ",shape=" + m_color;

  if((m_width >= 0) && !strContains(noshow,"width"))
    str += ",width=" + doubleToStringX(m_width);

  if((m_type!="hazard") && m_hr_set && !strContains(noshow,"hr"))
    str += ",hr=" + doubleToStringX(m_hr);

  if(m_aspect_set && !strContains(noshow,"aspect"))
    str += ",aspect=" + doubleToStringX(m_aspect);
  
  if(!strContains(noshow,"aspect_range")) {
    if(m_aspect_rng_min_set)
      str += ",aspect_min=" + doubleToStringX(m_aspect_rng_min);
    if(m_aspect_rng_max_set)
      str += ",aspect_max=" + doubleToStringX(m_aspect_rng_max);
  }

  unsigned int strlen = str.length();
  if((strlen > 0) && (str[0] == ','))
    str = str.substr(1, strlen-1);

  return(str);
}








