/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYSquare.cpp                                         */
/*    DATE: Aug 27th 2005                                        */
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
#include "XYSquare.h"
#include "GeomUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

XYSquare::XYSquare()
{
  m_xlow  = 0;
  m_xhigh = 0;
  m_ylow  = 0;
  m_yhigh = 0;
  m_valid = false;
}

//-------------------------------------------------------------
// Procedure: Constructor

XYSquare::XYSquare(double gxl, double gxh, double gyl, double gyh)
{
  set(gxl, gxh, gyl, gyh);
}

//-------------------------------------------------------------
// Procedure: Constructor

XYSquare::XYSquare(double xlen, double ylen)
{
  set(0, xlen, 0, ylen);
}

//-------------------------------------------------------------
// Procedure: Constructor

XYSquare::XYSquare(double xylen)
{
  set(0, xylen, 0, xylen);
}

//-------------------------------------------------------------
// Procedure: Operator==

bool XYSquare::operator==(const XYSquare &other) const
{
  return((m_xlow  == other.m_xlow)  && 
	 (m_xhigh == other.m_xhigh) &&
         (m_ylow  == other.m_ylow)  && 
	 (m_yhigh == other.m_yhigh));
}

//-------------------------------------------------------------
// Procedure: Operator!=

bool XYSquare::operator!=(const XYSquare &other) const
{
  return !(*this == other);
}

//-------------------------------------------------------------
// Procedure: set

void XYSquare::set(double gxl, double gxh, double gyl, double gyh)
{
  m_xlow  = gxl;
  m_xhigh = gxh;
  m_ylow  = gyl;
  m_yhigh = gyh;
  
  if((m_xlow <= m_xhigh) && (m_ylow <= m_yhigh))
    m_valid = true;
  else
    m_valid = false;
}

//-------------------------------------------------------------
// Procedure: containsPoint

bool XYSquare::containsPoint(double gx, double gy) const
{
  if(gx < m_xlow)
    return(false);
  if(gx > m_xhigh)
    return(false);
  if(gy < m_ylow)
    return(false);
  if(gy > m_yhigh)
    return(false);

  return(true);
}

//-------------------------------------------------------------
// Procedure: segIntersectLength
//      Note: 

double XYSquare::segIntersectLength(double x1, double y1, 
				    double x2, double y2) const
{
  // First a quick check for non-intersection - based on the 
  // assumption that most segments will not intersect and we
  // want a quick way to confirm non-intersection.
  if((x1 < m_xlow)  && (x2 < m_xlow))    return(0);
  if((x1 > m_xhigh) && (x2 > m_xhigh))   return(0);
  if((y1 < m_ylow)  && (y2 < m_ylow))    return(0);
  if((y1 > m_yhigh) && (y2 > m_yhigh))   return(0);

  // Handle special case where the segment is vertical
  if(x1==x2) {
    if((x1 < m_xlow) || (x1 > m_xhigh))
      return(0);
    else {
      if(y1 > m_yhigh) y1=m_yhigh;
      if(y1 < m_ylow)  y1=m_ylow;
      if(y2 > m_yhigh) y2=m_yhigh;
      if(y2 < m_ylow)  y2=m_ylow;
      double diff = y2-y1;
      if(diff > 0)
	return(diff);
      else
	return(-diff);
    }
  }
  // Handle special case where the segment is horizontal
  if(y1==y2) {
    if((y1 < m_ylow) || (y1 > m_yhigh))
      return(0);
    else {
      if(x1 > m_xhigh) x1=m_xhigh;
      if(x1 < m_xlow)  x1=m_xlow;
      if(x2 > m_xhigh) x2=m_xhigh;
      if(x2 < m_xlow)  x2=m_xlow;
      double diff = x2-x1;
      if(diff > 0)
	return(diff);
      else
	return(-diff);
    }
  }

  // Now that we've handled the special cases and can guarantee
  // no divisions by zero, determine the parameters of the line

  double m = (y2-y1) / (x2-x1);  // "slope" in y = mx + b
  double b = y1 - (m * x1);      // "intercept" in y = mx + b

  // Adjust the x1 value if needed. If so, then adjust the y1 
  // value according to the parameters of the line.
  if(x1 < m_xlow) {
    x1 = m_xlow;
    y1 = (m*x1) + b;
  }
  else if(x1 > m_xhigh) {
    x1 = m_xhigh;
    y1 = (m*x1) + b;
  }

  // Adjust the y1 value if needed. If so, then adjust the x1 
  // value according to the parameters of the line.
  if(y1 < m_ylow) {
    y1 = m_ylow;
    x1 = (y1-b)/m;
  }
  else if(y1 > m_yhigh) {
    y1 = m_yhigh;
    x1 = (y1-b)/m;
  }

  // Adjust the x2 value if needed. If so, then adjust the y2 
  // value according to the parameters of the line.
  if(x2 < m_xlow) {
    x2 = m_xlow;
    y2 = (m*x2) + b;
  }
  else if(x2 > m_xhigh) {
    x2 = m_xhigh;
    y2 = (m*x2) + b;
  }

  // Adjust the y2 value if needed. If so, then adjust the x2 
  // value according to the parameters of the line.
  if(y2 < m_ylow) {
    y2 = m_ylow;
    x2 = (y2-b)/m;
  }
  else if(y2 > m_yhigh) {
    y2 = m_yhigh;
    x2 = (y2-b)/m;
  }

  // Now that both points have been "clipped", just return the 
  // linear distance between the two.
  return(distPointToPoint(x1,y1,x2,y2));
}

//-------------------------------------------------------------
// Procedure: segDistToSquare
//      Note: 

double XYSquare::segDistToSquare(double x1, double y1, 
				 double x2, double y2) const
{
  double dist[4];
  dist[0] = distSegToSeg(x1,y1,x2,y2, m_xlow,m_ylow,m_xhigh,m_ylow);
  dist[1] = distSegToSeg(x1,y1,x2,y2, m_xhigh,m_ylow,m_xhigh,m_yhigh);
  dist[2] = distSegToSeg(x1,y1,x2,y2, m_xhigh,m_yhigh,m_xlow,m_yhigh);
  dist[3] = distSegToSeg(x1,y1,x2,y2, m_xlow,m_yhigh,m_xlow,m_ylow);

  double min_dist = dist[0];
  for(int i=1; i<4; i++)
    if(dist[i] < min_dist)
      min_dist = dist[i];
  
  return(min_dist);
}

//-------------------------------------------------------------
// Procedure: ptDistToSquareCtr
//      Note: 

double XYSquare::ptDistToSquareCtr(double x1, double y1) const
{
  double x2 = m_xlow + ((m_xhigh-m_xlow)/2);
  double y2 = m_ylow + ((m_yhigh-m_ylow)/2);
  return(hypot((x1-x2), (y1-y2)));
}

//-------------------------------------------------------------
// Procedure: getVal

double XYSquare::getVal(int xy, int lh) const
{
  if(xy==0)
    if(lh==0)
      return(m_xlow);
    else
      return(m_xhigh);
  else
    if(lh==0)
      return(m_ylow);
    else
      return(m_yhigh);
}

//---------------------------------------------------------------
// Procedure: get_spec

string XYSquare::get_spec(string param) const
{
  string spec;

  spec += "xlow="   + doubleToStringX(m_xlow,  3);
  spec += ",xhigh=" + doubleToStringX(m_xhigh, 3);
  spec += ",ylow="  + doubleToStringX(m_ylow,  3);
  spec += ",yhigh=" + doubleToStringX(m_yhigh, 3);
  
  spec += "," + XYObject::get_spec(param);

  return(spec);
}

//---------------------------------------------------------------
// Procedure: get_spec_as_poly

string XYSquare::get_spec_as_poly(string param) const
{
  string spec = "pts={";

  string xl = doubleToStringX(m_xlow,3);
  string xh = doubleToStringX(m_xhigh,3);
  string yl = doubleToStringX(m_ylow,3);
  string yh = doubleToStringX(m_yhigh,3);

  spec += xl + "," + yl + ":";
  spec += xh + "," + yl + ":";
  spec += xh + "," + yh + ":";
  spec += xl + "," + yh + "}";
  
  spec += "," + XYObject::get_spec(param);

  return(spec);
}










