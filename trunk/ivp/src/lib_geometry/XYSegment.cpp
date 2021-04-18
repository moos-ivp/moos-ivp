/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYSegment.cpp                                        */
/*    DATE: Mar 28th, 2009                                       */
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

#include <cmath>
#include <cstdlib>
#include <cstring>
#include "XYSegment.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

XYSegment::XYSegment()
{
  m_x1 = 0;
  m_y1 = 0;
  m_x2 = 0;
  m_y2 = 0; 
  m_length = 0;
  m_rang12 = 0;
  m_rang21 = 0;
}

//---------------------------------------------------------------
// Procedure: set

void XYSegment::set(double x1, double y1, double x2, double y2)
{
  m_x1 = x1;
  m_y1 = y1;
  m_x2 = x2;
  m_y2 = y2; 

  m_length = hypot((m_x1-m_x2), (m_y1-m_y2));
  m_rang12 = relAng(m_x1, m_y1, m_x2, m_y2);
  m_rang21 = relAng(m_x2, m_y2, m_x1, m_y1);
}

//---------------------------------------------------------------
// Procedure: set

void XYSegment::set(const XYPoint& pt1, const XYPoint pt2)
{
  m_x1 = pt1.get_vx();
  m_y1 = pt1.get_vy();
  m_x2 = pt2.get_vx();
  m_y2 = pt2.get_vy(); 

  m_length = hypot((m_x1-m_x2), (m_y1-m_y2));
  m_rang12 = relAng(m_x1, m_y1, m_x2, m_y2);
  m_rang21 = relAng(m_x2, m_y2, m_x1, m_y1);
}

//---------------------------------------------------------------
// Procedure: clear

void XYSegment::clear()
{
  m_x1 = 0;
  m_y1 = 0;
  m_x2 = 0;
  m_y2 = 0; 
  m_length = 0;
  m_rang12 = 0;
  m_rang21 = 0;
}


//---------------------------------------------------------------
// Procedure: shift_horz

void XYSegment::shift_horz(double shift_val)
{
  m_x1 += shift_val;
  m_x2 += shift_val;
}

//---------------------------------------------------------------
// Procedure: shift_vert

void XYSegment::shift_vert(double shift_val)
{
  m_y1 += shift_val;
  m_y2 += shift_val;
}

//---------------------------------------------------------------
// Procedure: get_spec
//   Purpose: Get a string specification of the seglist. We set 
//            the vertex precision to be at the integer by default.

string XYSegment::get_spec(int precision) const
{
  string spec;

  // Clip the precision to be between 0 and 6
  if(precision < 0)
    precision = 0;
  else if(precision > 6)
    precision = 6;
  
  if(m_label != "")
    spec += "label=" + m_label + "#"; 

  spec += "pts=";
  spec += doubleToStringX(m_x1, precision) + ",";
  spec += doubleToStringX(m_y1, precision) + ":";
  spec += doubleToStringX(m_x2, precision) + ",";
  spec += doubleToStringX(m_y2, precision);

  return(spec);
}

//---------------------------------------------------------------
// Procedure: reverse
//   Purpose: Swap the two endpoints

void XYSegment::reverse()
{
  double tmpx = m_x1;
  double tmpy = m_y1;

  m_x1 = m_x2;
  m_y1 = m_y2;
  m_x2 = tmpx;
  m_y2 = tmpy;
}










