/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYSquare.cpp                                         */
/*    DATE: Aug 27th 2005                                        */
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

#include <cmath>
#include <stdlib.h>
#include "XYSquare.h"
#include "GeomUtils.h"

//-------------------------------------------------------------
// Procedure: Constructor

XYSquare::XYSquare()
{
  xlow  = 0;
  xhigh = 0;
  ylow  = 0;
  yhigh = 0;
  valid_state = false;
}

//-------------------------------------------------------------
// Procedure: Constructor

XYSquare::XYSquare(double gxl, double gxh, double gyl, double gyh)
{
  set(gxl, gxh, gyl, gyh);
}

//-------------------------------------------------------------
// Procedure: Operator==

bool XYSquare::operator==(const XYSquare &other) const
{
  return(xlow==other.xlow && xhigh == other.xhigh &&
         ylow==other.ylow && yhigh == other.yhigh);
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
  xlow  = gxl;
  xhigh = gxh;
  ylow  = gyl;
  yhigh = gyh;
  
  if((xlow <= xhigh) && (ylow <= yhigh))
    valid_state = true;
  else
    valid_state = false;
}

//-------------------------------------------------------------
// Procedure: containsPoint

bool XYSquare::containsPoint(double gx, double gy) const
{
  if(gx < xlow)
    return(false);
  if(gx > xhigh)
    return(false);
  if(gy < ylow)
    return(false);
  if(gy > yhigh)
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
  if((x1 < xlow)  && (x2 < xlow))    return(0);
  if((x1 > xhigh) && (x2 > xhigh))   return(0);
  if((y1 < ylow)  && (y2 < ylow))    return(0);
  if((y1 > yhigh) && (y2 > yhigh))   return(0);

  // Handle special case where the segment is vertical
  if(x1==x2) {
    if((x1 < xlow) || (x1 > xhigh))
      return(0);
    else {
      if(y1 > yhigh) y1=yhigh;
      if(y1 < ylow)  y1=ylow;
      if(y2 > yhigh) y2=yhigh;
      if(y2 < ylow)  y2=ylow;
      double diff = y2-y1;
      if(diff > 0)
	return(diff);
      else
	return(-diff);
    }
  }
  // Handle special case where the segment is horizontal
  if(y1==y2) {
    if((y1 < ylow) || (y1 > yhigh))
      return(0);
    else {
      if(x1 > xhigh) x1=xhigh;
      if(x1 < xlow)  x1=xlow;
      if(x2 > xhigh) x2=xhigh;
      if(x2 < xlow)  x2=xlow;
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
  if(x1 < xlow) {
    x1 = xlow;
    y1 = (m*x1) + b;
  }
  else if(x1 > xhigh) {
    x1 = xhigh;
    y1 = (m*x1) + b;
  }

  // Adjust the y1 value if needed. If so, then adjust the x1 
  // value according to the parameters of the line.
  if(y1 < ylow) {
    y1 = ylow;
    x1 = (y1-b)/m;
  }
  else if(y1 > yhigh) {
    y1 = yhigh;
    x1 = (y1-b)/m;
  }

  // Adjust the x2 value if needed. If so, then adjust the y2 
  // value according to the parameters of the line.
  if(x2 < xlow) {
    x2 = xlow;
    y2 = (m*x2) + b;
  }
  else if(x2 > xhigh) {
    x2 = xhigh;
    y2 = (m*x2) + b;
  }

  // Adjust the y2 value if needed. If so, then adjust the x2 
  // value according to the parameters of the line.
  if(y2 < ylow) {
    y2 = ylow;
    x2 = (y2-b)/m;
  }
  else if(y2 > yhigh) {
    y2 = yhigh;
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
  dist[0] = distSegToSeg(x1,y1,x2,y2, xlow,ylow,xhigh,ylow);
  dist[1] = distSegToSeg(x1,y1,x2,y2, xhigh,ylow,xhigh,yhigh);
  dist[2] = distSegToSeg(x1,y1,x2,y2, xhigh,yhigh,xlow,yhigh);
  dist[3] = distSegToSeg(x1,y1,x2,y2, xlow,yhigh,xlow,ylow);

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
  double x2 = xlow + ((xhigh-xlow)/2);
  double y2 = ylow + ((yhigh-ylow)/2);
  return(hypot((x1-x2), (y1-y2)));
}

//-------------------------------------------------------------
// Procedure: getVal

double XYSquare::getVal(int xy, int lh) const
{
  if(xy==0)
    if(lh==0)
      return(xlow);
    else
      return(xhigh);
  else
    if(lh==0)
      return(ylow);
    else
      return(yhigh);
}









