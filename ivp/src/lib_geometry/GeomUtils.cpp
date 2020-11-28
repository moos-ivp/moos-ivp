/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GeomUtils.cpp                                        */
/*    DATE: May 8, 2005 Sunday Morning at Bruegger's             */
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
#include "GeomUtils.h"
#include "AngleUtils.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

//---------------------------------------------------------------
// Procedure: distPointToPoint

double distPointToPoint(double x1, double y1, double x2, double y2)
{
  return(hypot((x1-x2), (y1-y2)));
}
  
double distPointToPoint(const XYPoint& pt1, const XYPoint& pt2)
{
  return(hypot((pt1.get_vx() - pt2.get_vx()), 
	       (pt1.get_vy() - pt2.get_vy())));
}
  
double distToPoint(double x1, double y1, double x2, double y2)
{
  return(hypot((x1-x2), (y1-y2)));
}
  
//---------------------------------------------------------------
// Procedure: distPointToSeg()
//   Purpose: Return the distance from the segment given by x1,y1 
//            and x2,y2 to the point given by px,py

double distPointToSeg(double x1, double y1, double x2, 
		      double y2, double px, double py)
{
  double ix, iy;
  perpSegIntPt(x1, y1, x2, y2, px, py, ix, iy);
  return(distPointToPoint(px, py, ix, iy));
}

//---------------------------------------------------------------
// Procedure: distPointToSeg()
//   Purpose: Return the distance from the segment given by x1,y1 
//            and x2,y2 to the point given by px,py

double distPointToSeg(double x1, double y1, double x2, 
		      double y2, double px, double py,
		      double& ix, double& iy)
{
  perpSegIntPt(x1, y1, x2, y2, px, py, ix, iy);
  return(distPointToPoint(px, py, ix, iy));
}

//---------------------------------------------------------------
// Procedure: distToSegment()

double distToSegment(double x1, double y1, double x2, 
		     double y2, double px, double py)
{
  double ix, iy;
  perpSegIntPt(x1, y1, x2, y2, px, py, ix, iy);
  return(distPointToPoint(px, py, ix, iy));
}

//---------------------------------------------------------------
// Procedure: distPointToSeg
//   Purpose: Return the distance from the segment given by x1,y1 
//            and x2,y2 to the point given by px,py along a given 
//            angle. If ray doesn't intersect segment, return -1.

double distPointToSeg(double x1, double y1, double x2, double y2,
		      double px, double py, double angle)
{
  double dist_to_pt1 = distPointToPoint(x1, y1, px, py);
  double dist_to_pt2 = distPointToPoint(x2, y2, px, py);
  
  double max_dist = dist_to_pt1;
  if(dist_to_pt2 > max_dist)
    max_dist = dist_to_pt2;

  double px2, py2;
  projectPoint(angle, (max_dist*2), px, py, px2, py2);

  bool cross = segmentsCross(x1,y1,x2,y2,px,py,px2,py2);
  if(!cross)
    return(-1);

  double intx, inty;
  linesCross(x1,y1,x2,y2,px,py,px2,py2,intx,inty);

  return(distPointToPoint(px,py,intx,inty));
  
}


//---------------------------------------------------------------
// Procedure: distSegToSeg
//   Purpose: Return the distance from the segment given by x1,y1 
//            and x2,y2 to the segment given by x3,y3 and x4,y4.

double distSegToSeg(double x1, double y1, double x2, double y2,
		    double x3, double y3, double x4, double y4)
{
  if(segmentsCross(x1,y1,x2,y2,x3,y3,x4,y4))
    return(0);

  // Case 1: point x3,y3 to the segment (x1,y1 and x2,y2)
  double min_dist = distPointToSeg(x1,y1,x2,y2,x3,y3);

  // Case 2: point x4,y4 to the segment (x1,y1 and x2,y2)
  double two_dist = distPointToSeg(x1,y1,x2,y2,x4,y4);
  if(two_dist < min_dist)
    min_dist = two_dist;

  // Case 3: point x1,y1 to the segment (x3,y3 and x4,y4)
  double three_dist = distPointToSeg(x3,y3,x4,y4,x1,y1);
  if(three_dist < min_dist)
    min_dist = three_dist;

  // Case 4: point x2,y2 to the segment (x3,y3 and x4,y4)
  double four_dist = distPointToSeg(x3,y3,x4,y4,x2,y2);
  if(four_dist < min_dist)
    min_dist = four_dist;

  return(min_dist);
}

//---------------------------------------------------------------
// Procedure: lines_cross
//     Cases: Vert - Vert (1)
//            Horz - Horz (2)
//            Horz - Vert (3)
//            Vert - Horz (4)
//
//            Vert - Norm (5)
//            Horz - Norm (6)
//            Norm - Vert (7)
//            Norm - Horz (8)
//
//            Norm - Norm (9)

bool linesCross(double x1, double y1, double x2, double y2,
		double x3, double y3, double x4, double y4,
		double& ix, double& iy) 
{
  ix = 0; 
  iy = 0;
  bool line1_vert = (x1==x2);
  bool line1_horz = (y1==y2);
  bool line2_vert = (x3==x4);
  bool line2_horz = (y3==y4);
  
  // Case 1 - both lines vertical
  if(line1_vert && line2_vert) {
    if(x1==x3) {
      ix = x1; iy = 0;
      return(true);
    }
    else
      return(false);
  }

  // Case 2 - both lines horizontal
  if(line1_horz && line2_horz) {
    if(y1==y3) {
      iy = y1; ix = 0;
      return(true);
    }
    else
      return(false);
  }

  // Case 3 - line1 horizontal line2 vertical
  if(line1_horz && line2_vert) {
    iy = y1; ix = x3;
    return(true);
  }

  // Case 4 - line1 vertical line2 horizontal
  if(line1_vert && line2_horz) {
    iy = y3; ix = x1;
    return(true);
  }

  // Case 5 - line1 vertical line2 normal
  if(line1_vert && !line2_horz & !line2_vert) {
    ix = x1;
    double slope_b = (y4-y3) / (x4-x3);
    double inter_b = y3 - (slope_b * x3);
    iy = (slope_b * ix) + inter_b;
    return(true);
  }

  // Case 6 - line1 horizontal line2 normal
  if(line1_horz && !line2_horz & !line2_vert) {
    iy = y1;
    double slope_b = (y4-y3) / (x4-x3);
    double inter_b = y3 - (slope_b * x3);
    ix = (iy - inter_b) / slope_b;
    return(true);
  }

  // Case 7 - line1 normal line2 vertical
  if(!line1_vert && !line1_horz & line2_vert) {
    ix = x3;
    double slope_a = (y2-y1) / (x2-x1);
    double inter_a = y1 - (slope_a * x1);
    iy = (slope_a * ix) + inter_a;
    return(true);
  }

  // Case 8 - line1 normal line2 horizontal
  if(!line1_horz && !line1_vert & line2_horz) {
    iy = y3;
    double slope_a = (y2-y1) / (x2-x1);
    double inter_a = y1 - (slope_a * x1);
    ix = (iy - inter_a) / slope_a;
    return(true);
  }

  // Case 9 - the general case
  // First find slope and intercept of the two lines. (y = mx + b)
  double slope_a = (y2-y1) / (x2-x1);
  double slope_b = (y4-y3) / (x4-x3);
  double inter_a = y1 - (slope_a * x1);
  double inter_b = y3 - (slope_b * x3);

  if(slope_a == slope_b) {
    if(inter_a == inter_b) {
      ix = x1; iy = y1;
      return(true);
    }
    else
      return(false);
  }

  // Then solve for x. m1(x) + b1 = m2(x) + b2.
  ix = (inter_a - inter_b) / (slope_b - slope_a);

  // Then plug ix into one of the line equations.
  iy = (slope_a * ix) + inter_a;

  return(true);
}


//---------------------------------------------------------------
// Procedure: segmentsCross
//     Cases: Vert - Vert (1)
//            Horz - Horz (2)
//            Horz - Vert (3)
//            Vert - Horz (4)
//
//            Vert - Norm (5)
//            Horz - Norm (6)
//            Norm - Vert (7)
//            Norm - Horz (8)
//
//            Norm - Norm (9)

bool segmentsCross(double x1, double y1, double x2, double y2,
		   double x3, double y3, double x4, double y4) 
{
  // Special case - if the segments share an endpoint. Checked
  // for here since, due to rounding errors, not always caught
  // by the general case.

  if((x1==x3) && (y1==y3)) return(true);
  if((x1==x4) && (y1==y4)) return(true);
  if((x2==x3) && (y2==y3)) return(true);
  if((x2==x4) && (y2==y4)) return(true);

  bool seg1_vert = (x1==x2);
  bool seg1_horz = (y1==y2);
  bool seg2_vert = (x3==x4);
  bool seg2_horz = (y3==y4);
  // bool seg1_pt = ((x1==x2)&&(y1==y2));
  // bool seg2_pt = ((x3==x4)&&(y3==y4));

  // Case 1 Vert-Vert
  if(seg1_vert && seg2_vert) {
    if(x1 != x3) 
      return(false);
    if(y1 < y2) {
      if((y3 > y2) && (y4 > y2))
	return(false);
      if((y3 < y1) && (y4 < y1))
	return(false);
      return(true);
    }
    else {
      if((y3 > y1) && (y4 > y1))
	return(false);
      if((y3 < y2) && (y4 < y2))
	return(false);
      return(true);
    }
  }
  // Case 2 Horz - Horz
  if(seg1_horz && seg2_horz) {
    if(y1 != y3) //** 
      return(false);
    if(x1 < x2) {
      if((x3 > x2) && (x4 > x2))
	return(false);
      if((x3 < x1) && (x4 < x1))
	return(false);
      return(true);
    }
    else {
      if((x3 > x1) && (x4 > x1))
	return(false);
      if((x3 < x2) && (x4 < x2))
	return(false);
      return(true);
    }
  }
  // Case 3 Horz-Vert
  if(seg1_horz && seg2_vert) {
    if((y1 > y3) && (y1 > y4))
      return(false);
    if((y1 < y3) && (y1 < y4))
      return(false);
    if((x1 < x3) && (x2 < x3))
      return(false);
    if((x1 > x3) && (x2 > x3))
      return(false);
    return(true);
  }
  // Case 4 Vert-Horz
  if(seg1_vert && seg2_horz) {
    if((y3 > y1) && (y3 > y2))
      return(false);
    if((y3 < y1) && (y3 < y2))
      return(false);
    if((x3 < x1) && (x4 < x1))
      return(false);
    if((x3 > x1) && (x4 > x1))
      return(false);
    return(true);
  }
  // Case 5 Vert-Norm
  if(seg1_vert && !seg2_vert && !seg2_horz) {
    if((x1 > x3) && (x1 > x4))
      return(false);
    if((x1 < x3) && (x1 < x4))
      return(false);
    double slope = (y4-y3)/(x4-x3);
    double intercept = y4 - (slope * x4);
    double inty = (slope * x1) + intercept;
    if((inty > y1) && (inty > y2))
      return(false);
    if((inty < y1) && (inty < y2))
      return(false);
    return(true);
  }
  // Case 6 Horz-Norm
  if(seg1_horz && !seg2_vert && !seg2_horz) {
    if((y1 > y3) && (y1 > y4))
      return(false);
    if((y1 < y3) && (y1 < y4))
      return(false);
    double slope = (y4-y3)/(x4-x3);
    double intercept = y4 - (slope * x4);
    double intx = (y1 - intercept) / slope;
    if((intx > x1) && (intx > x2))
      return(false);
    if((intx < x1) && (intx < x2))
      return(false);
    return(true);
  }
  // Case 7 Norm-Vert
  if(!seg1_vert && !seg1_horz && seg2_vert) {
    if((x3 > x1) && (x3 > x2))
      return(false);
    if((x3 < x1) && (x3 < x2))
      return(false);
    double slope = (y2-y1)/(x2-x1);
    double intercept = y2 - (slope * x2);
    double inty = (slope * x3) + intercept;
    if((inty > y3) && (inty > y4))
      return(false);
    if((inty < y3) && (inty < y4))
      return(false);
    return(true);
  }
  // Case 8 Norm-Horz
  if(!seg1_vert && !seg1_horz && seg2_horz) {
    if((y3 < y1) && (y3 < y2))
      return(false);
    if((y3 > y1) && (y3 > y2))
      return(false);
    double slope = (y2-y1)/(x2-x1);
    double intercept = y2 - (slope * x2);
    double intx = (y3 - intercept) / slope;
    if((intx > x3) && (intx > x4))
      return(false);
    if((intx < x3) && (intx < x4))
      return(false);
    return(true);
  }
  // Case 9 Norm-Norm
  if(!seg1_vert && !seg1_horz && !seg2_vert && !seg2_horz) {
    double slope1 = (y2-y1)/(x2-x1);
    double intercept1 = y2 - (slope1 * x2);
    double slope2 = (y4-y3)/(x4-x3);
    double intercept2 = y4 - (slope2 * x4);
    if(slope1 == slope2) {           // bug fix mikerb Nov0120
      if(intercept1 != intercept2)
	return(false);
      double xmin12 = x1;
      double xmax12 = x2;
      if(x1>x2) {
	xmin12 = x2;
	xmax12 = x1;
      }
      if((x3 < xmin12) && (x4 < xmin12))
	return(false);
      if((x3 > xmax12) && (x4 > xmax12))
	return(false);
      return(true);
    }
    
    double intx = (intercept2 - intercept1) / (slope1 - slope2);

    if((intx < x1) && (intx < x2))
      return(false);
    if((intx > x1) && (intx > x2))
      return(false);
    if((intx < x3) && (intx < x4))
      return(false);
    if((intx > x3) && (intx > x4))
      return(false);
    return(true);
  }

  return(false);
}

//---------------------------------------------------------------
// Procedure: lineRayCross
//     Cases: Ray Vert - Line Vert (1)
//            Ray Horz - Line Horz (2)
//            Ray Horz - Line Vert (3)
//            Ray Vert - Line Horz (4)
//
//            Ray Vert - Line Norm (5)
//            Ray Horz - Line Norm (6)
//            Ray Norm - Line Vert (7)
//            Ray Norm - Line Horz (8)
//
//            Ray Norm - Line Norm (9)

bool lineRayCross(double x1, double y1, double ray_angle,
		  double x3, double y3, double x4, double y4, 
		  double& ix, double& iy) 
{
  ix = 0; 
  iy = 0;
  bool ray_vert  = ((ray_angle==0)  || (ray_angle==180));
  bool ray_horz  = ((ray_angle==90) || (ray_angle==270));
  bool line_vert = (x3==x4);
  bool line_horz = (y3==y4);

  // Case 1 - both ray and line vertical (intersection pt not unique)
  //          choose an intersection point on the line segment given
  //          by the line definition
  if(ray_vert && line_vert) {
    if(x1==x3) {
      ix = x3; iy = y3;
      return(true);
    }
    else
      return(false);
  }
  // Case 2 - both ray and line horizontal (intersection pt not unique)
  //          choose an intersection point on the line segment given
  //          by the line definition
  if(ray_horz && line_horz) {
    if(y1==y3) {
      iy = y3; ix = x3;
      return(true);
    }
    else
      return(false);
  }

  // Case 3 - ray horizontal line vertical
  if(ray_horz && line_vert) {
    if((x3 >= x1) && (ray_angle == 90)) {
      ix = x3, iy = y1;
      return(true);
    }
    if((x3 <= x1) && (ray_angle == 270)) {
      ix = x3, iy = y1;
      return(true);
    }
    return(false);
  }

  // Case 4 - ray vertical line horizontal
  if(ray_vert && line_horz) {
    if((y3 >= y1) && (ray_angle == 0)) {
      ix = x1, iy = y3;
      return(true);
    }
    if((y3 <= y1) && (ray_angle == 180)) {
      ix = x1, iy = y3;
      return(true);
    }
    return(false);
  }

  // Case 5 - ray vertical line normal
  if(ray_vert && !line_horz & !line_vert) {
    ix = x1;
    double slope_b = (y4-y3) / (x4-x3);
    double inter_b = y3 - (slope_b * x3);
    iy = (slope_b * ix) + inter_b;
    if((iy >= y1) && (ray_angle == 0))
      return(true);
    if((iy <= y1) && (ray_angle == 180))
      return(true);
    ix=0;iy=0;
    return(false);
  }

  // Case 6 - ray horizontal line normal
  if(ray_horz && !line_horz & !line_vert) {
    iy = y1;
    double slope_b = (y4-y3) / (x4-x3);
    double inter_b = y3 - (slope_b * x3);
    ix = (iy - inter_b) / slope_b;
    if((ix >= x1) && (ray_angle == 90))
      return(true);
    if((ix <= x1) && (ray_angle == 270))
      return(true);    
    ix=0;iy=0;
    return(false);
  }

  // Case 7 - ray normal line vertical
  if(!ray_vert && !ray_horz & line_vert) {
    // if "normal" ray create an artificial second vertex on the ray
    double x2,y2;
    projectPoint(ray_angle, 10, x1, y1, x2, y2);

    ix = x3;
    double slope_a = (y2-y1) / (x2-x1);
    double inter_a = y1 - (slope_a * x1);
    iy = (slope_a * ix) + inter_a;
    if((ix >= x1) && (ray_angle > 0) && (ray_angle < 180))
      return(true);
    if((ix <= x1) && (ray_angle > 180) && (ray_angle < 360))
      return(true);
    return(false);
  }

  // Case 8 - ray normal line horizontal
  if(!ray_horz && !ray_vert & line_horz) {
    // if "normal" ray create an artificial second vertex on the ray
    double x2,y2;
    projectPoint(ray_angle, 10, x1, y1, x2, y2);

    iy = y3;
    double slope_a = (y2-y1) / (x2-x1);
    double inter_a = y1 - (slope_a * x1);
    ix = (iy - inter_a) / slope_a;
    if((iy >= y1) && ((ray_angle < 90) || (ray_angle > 270)))
      return(true);
    if((iy <= y1) && (ray_angle > 90) && (ray_angle < 270))
      return(true);
    return(false);
  }

  // Case 9 - the general case
  // First find slope and intercept of the two lines. (y = mx + b)
  // if "normal" ray create an artificial second vertex on the ray
  double x2,y2;
  projectPoint(ray_angle, 10, x1, y1, x2, y2);
  double slope_a = (y2-y1) / (x2-x1);
  double slope_b = (y4-y3) / (x4-x3);
  double inter_a = y1 - (slope_a * x1);
  double inter_b = y3 - (slope_b * x3);
  
  if(slope_a == slope_b) {    // Special case: parallel lines
    if(inter_a == inter_b) {  // If identical/overlapping, just pick
      ix = x3; iy = y3;       // any point. vertex of line is fine.
      return(true);
    }
    else
      return(false);
  }

  // Then solve for x. m1(x) + b1 = m2(x) + b2.
  ix = (inter_a - inter_b) / (slope_b - slope_a);

  // Then plug ix into one of the line equations.
  iy = (slope_a * ix) + inter_a;

  if(ray_angle < 90) {
    if((ix >= x1) && (iy >= y1))
      return(true);
  }
  else if(ray_angle < 180) {
    if((ix >= x1) && (iy <= y1))
      return(true);
  }
  else if(ray_angle < 270) {
    if((ix <= x1) && (iy <= y1))
      return(true);
  }
  else {
    if((ix <= x1) && (iy >= y1))
      return(true);
  }

  ix = 0;
  iy = 0;
  return(false);
}


//---------------------------------------------------------------
// Procedure: lineSegCross
//     Cases: Seg Vert - Line Vert (1)
//            Seg Horz - Line Horz (2)
//            Seg Horz - Line Vert (3)
//            Seg Vert - Line Horz (4)
//
//            Seg Vert - Line Norm (5)
//            Seg Horz - Line Norm (6)
//            Seg Norm - Line Vert (7)
//            Seg Norm - Line Horz (8)
//
//            Ray Norm - Line Norm (9)

bool lineSegCross(double sx1, double sy1, double sx2, double sy2, 
		  double lx3, double ly3, double lx4, double ly4, 
		  double& ix1, double& iy1, double& ix2, double& iy2) 
{
  ix1 = 0; 
  iy1 = 0;
  bool seg_vert  = (sx1==sx2);
  bool seg_horz  = (sy1==sy2);
  bool line_vert = (lx3==lx4);
  bool line_horz = (ly3==ly4);

  // Case 1 - both seg and line vertical (intersection pt not unique)
  //          choose an intersection point on the line segment. 
  if(seg_vert && line_vert) {
    if(sx1==lx3) {
      ix1 = sx1;
      iy1 = sy1;
      ix2 = sx2;
      iy2 = sy2;
      return(true);
    }
    else
      return(false);
  }
  // Case 2 - both seg and line horizontal (intersection pt not unique)
  //          choose an intersection point on the line segment.
  if(seg_horz && line_horz) {
    if(sy1==ly3) {
      ix1 = sx1;
      iy1 = sy1;
      ix2 = sx2;
      iy2 = sy2;
      return(true);
    }
    else
      return(false);
  }

  // Case 3 - seg horizontal line vertical
  if(seg_horz && line_vert) {
    if((lx3 < sx1) && (lx3 < sx2))
      return(false);
    if((lx3 > sx1) && (lx3 > sx2))
      return(false);
    ix1 = ix2 = lx3;
    iy1 = iy2 = sy1;
    return(true);
  }

  // Case 4 - seg vertical line horizontal
  if(seg_vert && line_horz) {
    if((ly3 < sy1) && (ly3 < sy2))
      return(false);
    if((ly3 > sy1) && (ly3 > sy2))
      return(false);
    ix1 = ix2 = sx1;
    iy1 = iy2 = ly3;    
    return(true);
  }

  // Case 5 - seg vertical line normal
  if(seg_vert && !line_horz & !line_vert) {
    double maybe_ix = sx1;
    double slope_b = (ly4-ly3) / (lx4-lx3);
    double inter_b = ly3 - (slope_b * lx3);
    double maybe_iy = (slope_b * maybe_ix) + inter_b;
    if((maybe_iy > sy1) && (maybe_iy > sy2))
      return(false);
    if((maybe_iy < sy1) && (maybe_iy < sy2))
      return(false);
    ix1 = ix2 = maybe_ix;
    iy1 = iy2 = maybe_iy;
    return(true);
  }

  // Case 6 - seg horizontal line normal
  if(seg_horz && !line_horz & !line_vert) {
    double maybe_iy = sy1;
    double slope_b = (ly4-ly3) / (lx4-lx3);
    double inter_b = ly3 - (slope_b * lx3);
    double maybe_ix = (maybe_iy - inter_b) / slope_b;
    if((maybe_ix < sx1) && (maybe_ix < sx2))
      return(false);
    if((maybe_ix > sx1) && (maybe_ix > sx2))
      return(false);
    ix1 = maybe_ix;
    ix2 = maybe_ix;
    iy1 = maybe_iy;
    iy2 = maybe_iy;
    return(true);
  }

  // Case 7 - seg normal line vertical
  if(!seg_vert && !seg_horz & line_vert) {
    double maybe_ix = lx3;
    double slope_a = (sy2-sy1) / (sx2-sx1);
    double inter_a = sy1 - (slope_a * sx1);
    double maybe_iy = (slope_a * maybe_ix) + inter_a;
    if((maybe_iy > sy1) && (maybe_iy > sy2))
      return(false);
    if((maybe_iy < sy1) && (maybe_iy < sy2))
      return(false);
    ix1 = ix2 = maybe_ix;
    iy1 = iy2 = maybe_iy;
    return(true);
  }

  // Case 8 - seg normal line horizontal
  if(!seg_horz && !seg_vert & line_horz) {
    double maybe_iy = ly3;
    double slope_a = (sy2-sy1) / (sx2-sx1);
    double inter_a = sy1 - (slope_a * sx1);
    double maybe_ix = (maybe_iy - inter_a) / slope_a;
    if((maybe_ix < sx1) && (maybe_ix < sx2))
      return(false);
    if((maybe_ix > sx1) && (maybe_ix > sx2))
      return(false);
    ix1 = ix2 = maybe_ix;
    iy1 = iy2 = maybe_iy;
    return(true);

  }

  // Case 9 - the general case
  // First find slope and intercept of the two lines. (y = mx + b)
  // if "normal" ray create an artificial second vertex on the ray
  double slope_a = (sy2-sy1) / (sx2-sx1);
  double slope_b = (ly4-ly3) / (lx4-lx3);
  double inter_a = sy1 - (slope_a * sx1);
  double inter_b = ly3 - (slope_b * lx3);
  
  if(slope_a == slope_b) {    // Special case: parallel lines
    if(inter_a == inter_b) {  // If identical/overlapping, pick the 
      ix1 = sx1;      // the two vertices of the seg
      iy1 = sy1;   
      ix2 = sx2;
      iy2 = sy2;   
      return(true);
    }
    else
      return(false);
  }

  // Then solve for x. m1(x) + b1 = m2(x) + b2.
  double maybe_ix = (inter_a - inter_b) / (slope_b - slope_a);

  // Then plug ix into one of the line equations.
  double maybe_iy = (slope_a * maybe_ix) + inter_a;

  if((maybe_ix < sx1) && (maybe_ix < sx2))
    return(false);
  if((maybe_ix > sx1) && (maybe_ix > sx2))
    return(false);
  if((maybe_iy < sy1) && (maybe_iy < sy2))
    return(false);
  if((maybe_iy > sy1) && (maybe_iy > sy2))
    return(false);
  ix1 = ix2 = maybe_ix;
  iy1 = iy2 = maybe_iy;
  return(true);
}


//---------------------------------------------------------------
// Procedure: segmentAngle
//   Purpose: Return the angle between the two segments given by
//            the segment x1,y1 and x2,y2 and the segment x2,y2 
//            and x3,y3.
//            The return value is between -179.9999 and 180.0
//               
//   2o-----o3    3o-----o2       2o------o3    o3      o2         |
//    |                  |        /             |       | \        |
//    |  +90        -90  |       /   +45       o2 +0    |  \ +135  |
//    |                  |      /               |       |   \      |
//    o1                 o1    o1               o1      o1  3o     |
//               

double segmentAngle(double x1, double y1, double x2, 
		    double y2, double x3, double y3)
{
  // Handle special cases
  if((x1==x2) && (y1==y2)) return(0.0);
  if((x2==x3) && (y2==y3)) return(0.0);

  double angle1 = relAng(x1,y1, x2,y2);
  if(angle1 > 180) 
    angle1 -= 360;

  double angle2 = relAng(x2,y2, x3,y3);
  if(angle2 > 180) 
    angle2 -= 360;

  double result = angle2 - angle1;
  if(result > 180)
    result -= 360;
  if(result <= -180)
    result += 360;
  return(result);
}
  

//---------------------------------------------------------------
// Procedure: perpSegIntPt
//   Purpose: Determine the point on the given segment closest to
//            the point which would make a line perpendicular if
//            using the given query point. 
//      Note: The line given by qx,qy and rx,ry may not be
//            perpendiculary to the given line segment (see Case B)
//                                                                 
//                                                                 
//          CASE A                     CASE B                   |   
//          ======                     ======                   |
//                                                              |  
//       o             o             Q             Q            |
//       |             |                            \           |          
//       |             |                             \          |     
//  Q    |   ==>  Q----X                   o     ==>  X         |
//       |             |                   |          |         |
//       |             |                   |          |         |
//       |             |                   |          |         |
//       o             o                   o          o         | 

void perpSegIntPt(double x1, double y1, double x2, double y2, 
		  double qx, double qy, double& rx, double& ry)
{
  double delta_x = x1<x2 ? x2-x1 : x1-x2;
  double delta_y = y1<y2 ? y2-y1 : y1-y2;

  // handle the special case where the segment is vertical
  // Or nearly vertical
  if(delta_x < 0.0000001) {
    rx = x1;
    if(y2 > y1) {
      if(qy > y2)
	ry = y2;
      else if(qy < y1)
	ry = y1;
      else
	ry = qy;
    }
    else {
      if(qy > y1) 
	ry = y1;
      else if(qy < y2)
	ry = y2;
      else
	ry = qy;
    }
    return;
  }

  // handle the special case where the segment is horizontal
  // Or nearly vertical
  if(delta_y < 0.0000001) {
    ry = y1;
    if(x2 > x1) {
      if(qx > x2) 
	rx = x2;
      else if(qx < x1)
	rx = x1;
      else
	rx = qx;
    }
    else {
      if(qx > x1) 
	rx = x1;
      else if(qx < x2)
	rx = x2;
      else
	rx = qx;
    }
    return;
  }

  // Now handle the general case

  double seg_m = (y2-y1) / (x2-x1);
  double seg_b = y1 - (seg_m * x1);
  double oth_m = -1.0 / seg_m;
  double oth_b = qy - (oth_m * qx);
  double int_x = (oth_b - seg_b) / (seg_m - oth_m);
  double int_y = (oth_m * int_x) + oth_b;

  if(x2 > x1) {
    if(int_x < x1)  int_x = x1;
    if(int_x > x2)  int_x = x2;
  }
  else {
    if(int_x < x2)  int_x = x2;
    if(int_x > x1)  int_x = x1;
  }

  if(y2 > y1) {
    if(int_y < y1)  int_y = y1;
    if(int_y > y2)  int_y = y2;
  }
  else {
    if(int_y < y2)  int_y = y2;
    if(int_y > y1)  int_y = y1;
  }

  rx = int_x;
  ry = int_y;
}

//---------------------------------------------------------------
// Procedure: perpLineIntPt

void perpLineIntPt(double x1, double y1, double x2, double y2, 
		   double qx, double qy, double& rx, double& ry)
{
  double xdelta = x1-x2;
  if(xdelta < 0)
    xdelta = -xdelta;
  // handle special case where line is vertical or essentially vertical
  if(xdelta < 0.0001) {
    rx = x1;
    ry = qy;
    return;
  }

  // handle special case where segment is horizontal or essentially so
  double ydelta = y1-y2;
  if(ydelta < 0)
    ydelta = -ydelta;
  if(ydelta < 0.0001) {
    rx = qx;
    ry = y1;
    return;
  }

  // Now handle the general case
  double seg_m = (y2-y1) / (x2-x1);
  double seg_b = y1 - (seg_m * x1);
  double oth_m = -1.0 / seg_m;
  double oth_b = qy - (oth_m * qx);

  rx = (oth_b - seg_b) / (seg_m - oth_m);
  ry = (oth_m * rx) + oth_b;
}

//---------------------------------------------------------------
// Procedure: projectPoint
//      Note: We special-case the axis angles to help ensure that
//            vertical and horizontal line segments aren't tripped
//            up by rounding errors.

void projectPoint(double degval, double dist, double cx, 
		  double cy, double &rx, double &ry)
{
  if((degval < 0) || (degval >= 360))
    degval = angle360(degval);

  if(degval == 0) {
    rx = cx;
    ry = cy + dist;
  }
  else if(degval == 90) {
    rx = cx + dist;
    ry = cy;
  }
  else if(degval == 180) {
    rx = cx;
    ry = cy - dist;
  }
  else if(degval == 270) {
    rx = cx - dist;
    ry = cy;
  }
  else {
    double radang  = degToRadians(degval);
    double delta_x = sin(radang) * dist;
    double delta_y = cos(radang) * dist;
    rx = cx + delta_x;
    ry = cy + delta_y;
  }
}


//---------------------------------------------------------------
// Procedure: projectPoint
//   Purpose: Same as the other projectPoint function except this 
//            function returns an XYPoint object.

XYPoint projectPoint(double degval, double dist, double cx, double cy)
{
  if((degval < 0) || (degval >= 360))
    degval = angle360(degval);
  
  double radang  = degToRadians(degval);
  double delta_x = sin(radang) * dist;
  double delta_y = cos(radang) * dist;

  double rx = cx + delta_x;
  double ry = cy + delta_y;

  XYPoint return_point(rx, ry);
  return(return_point);
}


//---------------------------------------------------------------
// Procedure: addVectors
//   Purpose: 

void addVectors(double deg1, double mag1, double deg2, 
		double mag2, double &rdeg, double &rmag)
{
  double x0, y0, x1, y1, x2, y2;

  deg1 = angle360(deg1);
  deg2 = angle360(deg2);

  x0 = 0;
  y0 = 0;
  projectPoint(deg1, mag1, x0, y0, x1, y1);
  projectPoint(deg2, mag2, x1, y1, x2, y2);

  rdeg = relAng(x0, y0, x2, y2);
  rmag = distPointToPoint(x0, y0, x2, y2);
}

//---------------------------------------------------------------
// Procedure: bearingMinMaxToPoly
//   Purpose: From a point outside a convex polygons, determine
//            the two bearing angles to the polygon forming a
//            pseudo tangent.
//   Returns: true if x,y is NOT in the polygon, false otherwise.

bool bearingMinMaxToPoly(double osx, double osy, const XYPolygon& poly,
			 double& bmin, double& bmax)
{
  // Create some placeholder variables that the general function
  // requires but are not needed for this simpler version.
  double bmin_dist = 0;
  double bmax_dist = 0;
  return(bearingMinMaxToPolyX(osx, osy, poly, bmin, bmax,
			      bmin_dist, bmax_dist));
}


//---------------------------------------------------------------
// Procedure: bearingMinMaxToPolyZ
//   Purpose: From a point outside a convex polygons, determine
//            the two bearing angles to the polygon forming a
//            pseudo tangent.
//            This version also calculates the distance to the
//            min/max points.
//   Returns: true if x,y is NOT in the polygon, false otherwise.

bool bearingMinMaxToPolyX(double osx, double osy,
			  const XYPolygon& poly,
			  double& bmin, double& bmax,
			  double& bmin_dist, double& bmax_dist)
{
  //===========================================================
  // Step 1: Sanity checks 
  //===========================================================
  if(poly.size() == 0)
    return(false);
  if(poly.is_convex() && poly.contains(osx, osy))
    return(false);
  
  //===========================================================
  // Step 2: Determine all the angles, noting their quadrants.
  //===========================================================
  bool quad_1 = false;    // [0-90)
  bool quad_2 = false;    // [90-180)
  bool quad_3 = false;    // [180-270)
  bool quad_4 = false;    // [270-360)

  vector<double> angles;
  unsigned int i, psize = poly.size();
  for(i=0; i<psize; i++) {
    double angle = relAng(osx, osy, poly.get_vx(i), poly.get_vy(i));
    angles.push_back(angle);
    if((angle >= 0) && (angle < 90))
      quad_1 = true;
    else if((angle >= 90) && (angle < 180))
      quad_2 = true;
    else if((angle >= 180) && (angle < 270))
      quad_3 = true;
    else
      quad_4 = true;
  }

  bool wrap_around = false;
  //============================================================
  // Step 3: Determine if we have an angle wrap-around situation
  //============================================================
  if(quad_1  &&  !quad_2  && !quad_3  &&  !quad_4)    // Case 1:  1
    wrap_around = false;
  else if(!quad_1 &&   quad_2 && !quad_3  && !quad_4) // Case 2:  2
    wrap_around = false;
  else if(!quad_1 &&  !quad_2 &&  quad_3  && !quad_4) // Case 3:  3
    wrap_around = false;
  else if(!quad_1 &&  !quad_2 && !quad_3  &&  quad_4) // Case 4:  4
    wrap_around = false;

  else if(quad_1  &&   quad_2 && !quad_3  && !quad_4) // Case 5:  1,2
    wrap_around = false;
  else if(!quad_1 &&   quad_2 &&  quad_3  && !quad_4) // Case 6:  2,3
    wrap_around = false;
  else if(!quad_1 &&  !quad_2 &&  quad_3  &&  quad_4) // Case 7:  3,4
    wrap_around = false;
  else if(quad_1  &&  !quad_2 && !quad_3  &&  quad_4) // Case 8:  4,1  
    wrap_around = true;
  else if(quad_1  &&   quad_2 &&  quad_3  && !quad_4) // Case 9:  1,2,3 
    wrap_around = false;
  else if(!quad_1 &&   quad_2 &&  quad_3  &&  quad_4) // Case 10: 2,3,4 
    wrap_around = false;
  else if(quad_1  &&  !quad_2 &&  quad_3  &&  quad_4) // Case 11: 3,4,1
    wrap_around = true;
  else if(quad_1  &&   quad_2 && !quad_3  &&  quad_4) // Case 12: 4,1,2
    wrap_around = true;

  else if(quad_1  &&  !quad_2 &&  quad_3  && !quad_4) { // Case 13: 1,3
    double px, py;
    poly.closest_point_on_poly(osx, osy, px, py);
    double os_angle = relAng(osx, osy, px, py);
    if((os_angle > 45) && (os_angle <= 225))
      wrap_around = false;
    else
      wrap_around = true;
  }
  else if(!quad_1 && quad_2 && !quad_3 && quad_4) { // Case 14: 2,4
    double px, py;
    poly.closest_point_on_poly(osx, osy, px, py);
    double os_angle = relAng(osx, osy, px, py);
    if((os_angle > 135) && (os_angle <= 315))
      wrap_around = false;
    else
      wrap_around = true;
  }

  //===========================================================
  // Step 4: Now determine the "min" and "max" bearing angles
  //===========================================================
  unsigned int bmin_ix = 0;
  unsigned int bmax_ix = 0;
  if(!wrap_around) {
    bmin = 360;
    bmax = 0;
    for(i=0; i<psize; i++) {
      if(angles[i] < bmin) {
	bmin = angles[i];
	bmin_ix = i;
      }
      if(angles[i] > bmax) {
	bmax = angles[i];
	bmax_ix = i;
      }
    }
  }
  else {
    bmin = 360;
    bmax = 0;
    for(i=0; i<psize; i++) {
      if((angles[i] > 180) && (angles[i] < bmin)) {
	bmin = angles[i];
	bmin_ix = i;
      }
      else if((angles[i] <= 180) && (angles[i] > bmax)) {
	bmax = angles[i];
	bmax_ix = i;
      }
    }
  }

  //===========================================================
  // Step 5: Calculate distances to the bmin,bmax vertices
  //===========================================================
  double px1 = poly.get_vx(bmin_ix);
  double py1 = poly.get_vy(bmin_ix);
  double px2 = poly.get_vx(bmax_ix);
  double py2 = poly.get_vy(bmax_ix);
  bmin_dist = hypot(osx-px1, osy-py1);
  bmax_dist = hypot(osx-px2, osy-py2);

  return(true);
}


//---------------------------------------------------------------
// Procedure: distCircleToLine()

double distCircleToLine(double cx, double cy, double radius,
			double px1, double py1, double px2, double py2)
{
  // Step 1: Find the point on the line that forms a perpendicular with
  // the circle center
  double ix=0;
  double iy=0;
  perpLineIntPt(px1, py1, px2, py2, cx, cy, ix, iy);

  // Step 2: Determine the distance from the circle center to the line
  double dist = hypot(cx-ix, cy-iy);

  // Step 3: If circle does not intersect the line, return the difference
  if(dist > radius)
    return(dist-radius);

  // Step 4: Otherwise circle intersects the line so return zero.
  return(0);
}


//---------------------------------------------------------------
// Procedure: randPointInPoly()
//   Purpose: Find a random point inside a given polygon.
//            Try a random points inside the polygon's bounding box.
//   Returns: true if random point found. 


bool randPointInPoly(const XYPolygon& poly, double& rx, double& ry,
		     unsigned int tries)
{
  if(!poly.is_convex())
    return(false);
  
  double xmin = poly.get_min_x();
  double xmax = poly.get_max_x();
  double ymin = poly.get_min_y();
  double ymax = poly.get_max_y();

  if((xmax <= xmin) || (ymax <= ymin))
    return(false);
  
  double xrng = xmax - xmin;
  double yrng = ymax - ymin;
  
  int xrng_int = (int)(xrng);
  int yrng_int = (int)(yrng);

  for(unsigned int i=0; i<tries; i++) {
    int rand_x = rand() % xrng_int;
    int rand_y = rand() % yrng_int;

    double px = xmin + (double)(rand_x);
    double py = ymin + (double)(rand_y);
    
    if(poly.contains(px, py)) {
      rx = px;
      ry = py;
      return(true);
    }
  }

  rx = poly.get_center_x();
  ry = poly.get_center_y();

  return(false);
}


//---------------------------------------------------------------
// Procedure: crossRaySegl()
//   Purpose: Determine if the given ray crosses anywhere in the given
//            SegList, and if so return the intersection point.
//            If multiple intersection points, return the intersection
//            point closes to the end of the ray.

bool crossRaySegl(double px, double py, double ph,
		  const XYSegList& segl,
		  double& rx, double& ry, double& range)
{
  // Unless filled in with an intersection, rx and ry will be zero.
  rx = 0;
  ry = 0;
  range = -1;
  
  unsigned int vsize = segl.size();

  // Special case #1 the seglist is empty
  if(vsize == 0)
    return(false);
  // Special case #2 the seglist contains only a single point
  if(vsize == 1) {
    double vx = segl.get_vx(0);
    double vy = segl.get_vy(0);
    return(lineRayCross(px, py, ph, vx, vy, vx, vy, rx, ry));
  }

  // For each line segment
  for(unsigned int i=0; i<vsize-1; i++) {

    double x1 = segl.get_vx(i);
    double y1 = segl.get_vy(i);
    double x2 = segl.get_vx(i+1);
    double y2 = segl.get_vy(i+1);

    double ix, iy;
    bool may_intersect = lineRayCross(px, py, ph, x1, y1, x2, y2, ix, iy);

    if(may_intersect) {
      bool x_intersect = false;
      if((x1==x2) && (x1==ix))
	x_intersect = true;
      else if((x1>x2) && (ix>=x2) && (ix<=x1))
	x_intersect = true;
      else {
	if((ix>=x1) && (ix<=x2))
	  x_intersect = true;
      }

      bool y_intersect = false;
      if((y1==y2) && (y1==iy))
	y_intersect = true;
      else if((y1>y2) && (iy>=y2) && (iy<=y1))
	y_intersect = true;
      else {
	if((iy>=y1) && (iy<=y2))
	  y_intersect = true;
      }

      if(x_intersect && y_intersect) {
	double this_range = hypot(px-ix, py-iy);
	if((range == -1) || (this_range < range)) {
	  rx = ix;
	  ry = iy;
	  range = this_range;
	}
      }
    }
  }

  if(range > 0)
    return(true);  
  return(false);
}


//---------------------------------------------------------------
// Procedure: distPointToRay()
//   Purpose: Determine distance from the given point to the given ray

double distPointToRay(double px, double py,
		      double rx, double ry, double ray_angle,
		      double& ix, double& iy)
{
  // Special case #1 the two points are the same
  if((rx == px) && (ry == py)) {
    ix = rx;
    iy = ry;
    return(0);
  }

  // Turn the point into a line segment that is perpendicular to the ray
  double x3,y3;
  double perp_angle = angle360(ray_angle+90);
  projectPoint(perp_angle, 10, px, py, x3, y3);
    
  bool crosses = lineRayCross(rx, ry, ray_angle, px, py, x3, y3, ix, iy);

  // If it doesn't cross, the point is "behind" the ray and CPA is the
  // base of the ray.
  if(!crosses) {
    ix = rx;
    iy = ry;
    return(hypot(rx-px, ry-py));
  }
  
  return(hypot(ix-px, iy-py));
}


//---------------------------------------------------------------
// Procedure: segRayCPA
//   Purpose: Determine the minimum distance from the given
//            segment to the given ray.
//      NOTE: This algorithm ASSUMES THE RAY AND SEGMENT DO NOT CROSS.
//            It assumes a prior check has determined that the ray does
//            not cross the segment, and this is being used simply to
//            determine how close the ray gets to the segment at its
//            closest point.
//            By not doing a check for crossing, it is much faster.

double segRayCPA(double rx, double ry, double ray_angle,
		 double x1, double y1, double x2, double y2,
		 double& ix, double& iy)
{
  double ix1, iy1, ix2, iy2, ix3, iy3;
  double dist1 = distPointToRay(x1,y1, rx,ry,ray_angle, ix1, iy1);
  double dist2 = distPointToRay(x2,y2, rx,ry,ray_angle, ix2, iy2);

  double dist3 = distPointToSeg(x1,y1, x2,y2, rx,ry, ix3,iy3);
  
  if((dist1 < dist2) && (dist1 < dist3)) {
    ix = ix1;
    iy = iy1;
    return(dist1);
  }
  if(dist2 < dist3) {
    ix = ix2;
    iy = iy2;
    return(dist2);    
  }

  ix = ix3;
  iy = iy3;
  return(dist3);    
}


//---------------------------------------------------------------
// Procedure: seglRayCPA
//   Purpose: Determine the minimum distance from the given
//            seglist to the given ray
//      NOTE: This algorithm ASSUMES THE RAY AND SEGLIST DO NOT CROSS.
//            It assumes a prior check has determined that the ray does
//            not cross the seglist, and this is being used simply to
//            determine how close the ray gets to the seglist at its
//            closest vertex.
//            By not doing a check for crossing, it is much faster.

double seglRayCPA(double rx, double ry, double ray_angle,
		  const XYSegList& segl,
		  double& ix, double& iy)
{
  // Special case of empty seglist
  if(segl.size() == 0) {
    ix = 0;
    iy = 0;
    return(0);
  }

  // General case of non-empty seglist
  double cpa = -1;  
  for(unsigned int i=0; i<segl.size(); i++) {
    double vx = segl.get_vx(i);
    double vy = segl.get_vy(i);
    double intx, inty;
    double dist = distPointToRay(vx,vy, rx,ry,ray_angle, intx,inty);
    if((cpa < 0) || (dist < cpa)) {
      cpa = dist;
      ix = intx;
      iy = inty;
    }
  }
  return(cpa);    
}

//---------------------------------------------------------------
// Procedure: seglSeglDist
//   Purpose: Determine the minimum distance from the given segment
//            to the given seglist
//      NOTE: This algorithm ASSUMES THE SEG AND SEGLIST DO NOT CROSS.
//            It assumes a prior check has determined that the seg does
//            not cross the seglist, and this is being used simply to
//            determine how close the seg gets to the seglist at its
//            closest vertex.
//            By not doing a check for crossing, it is much faster.

double segSeglDist(double x1, double y1, double x2, double y2,
		   const XYSegList& segl, double& rx, double& ry)
{
  // Special case 1: empty seglist
  if(segl.size() == 0) {
    rx = 0;
    ry = 0;
    return(0);
  }
  
  // Special case 2: seglist with one vertex
  if(segl.size() == 1) {
    double vx = segl.get_vx(0);
    double vy = segl.get_vy(0);
    perpSegIntPt(x1, y1, x2, y2, vx, vy, rx, ry);
    return(distPointToSeg(x1, y1, x2, y2, vx, vy));
  }

  // General case of a seglist of at least two vertices
  double cpa = -1;  
  for(unsigned int i=0; i<segl.size()-1; i++) {
    double x3 = segl.get_vx(i);
    double y3 = segl.get_vy(i);
    double x4 = segl.get_vx(i+1);
    double y4 = segl.get_vy(i+1);
    
    double dist1 = distPointToSeg(x3, y3, x4, y4, x1, y1);
    double dist2 = distPointToSeg(x3, y3, x4, y4, x2, y2);
    if((cpa < 0) || (dist1 < cpa)) {
      rx = x1;
      ry = y1;
      cpa = dist1;
    }
    if(dist2 < cpa) {
      rx = x2;
      ry = y2;
      cpa = dist2;
    }
  }

  // Part 2: check the distance all vertices in the seglist to
  //         the line segment
  for(unsigned int i=0; i<segl.size()-1; i++) {
    double px = segl.get_vx(i);
    double py = segl.get_vy(i);
    
    double dist = distPointToSeg(x1, y1, x2, y2, px, py);

    if((cpa < 0) || (dist < cpa)) {
      rx = px;
      ry = py;
      cpa = dist;
    }
  }

  return(cpa);    
}


//---------------------------------------------------------------
// Procedure: crossRaySeg()
//   Purpose: Determines if the given ray and line segment cross, 
//            and if so, calculates the point

bool crossRaySeg(double rx, double ry, double ray_angle,
		 double x1, double y1, double x2, double y2,
		 double &ix, double& iy)
{
  ix = 0;
  iy = 0;
  
  double kx, ky;
  bool ray_crosses_line = lineRayCross(rx, ry, ray_angle,
				       x1, y1, x2, y2, kx, ky);
  if(!ray_crosses_line)
    return(false);

  double xmin = x1;
  double xmax = x2;
  if(x1 > x2) {
    xmin = x2;
    xmax = x1;
  }
  // If the seg is essentially vertical, assume the x component
  // of the intersection point is on the segment.
  if((xmax - xmin) > 0.000001) {
    if((kx < xmin) || (kx > xmax))
      return(false);
  }
    
  double ymin = y1;
  double ymax = y2;
  if(y1 > y2) {
    ymin = y2;
    ymax = y1;
  }
  // If the seg is essentially horizontal, assume the y component
  // of the intersection point is on the segment.
  if((ymax - ymin) > 0.000001) {
    if((ky < ymin) || (ky > ymax))
      return(false);
  }

  ix = kx;
  iy = ky;
  return(true);
}


//---------------------------------------------------------------
// Procedure: lineCircleIntPts
//   Purpose: Given a line and a circle, determine where the line
//            intersects the circle. It will return the number of
//            intersection points and fill in the return points if
//            they exist.

int lineCircleIntPts(double x1, double y1, double x2, double y2,
		     double cx, double cy, double crad,
		     double& rx1, double& ry1,
		     double& rx2, double& ry2)
{
  // If a point is not defined, will still return 0,0
  rx1 = 0;
  ry1 = 0;
  rx2 = 0;
  ry2 = 0;

  // Handle case if the given "line" is actually a point. ugh
  if((x1==x2) && (y1==y2)) {
    double pdist = hypot(cx-x1, cy-y1);
    if(pdist == crad) {
      rx1 = x1;
      ry1 = y1;
      return(1);
    }
    return(0);
  }

  double pix, piy;
  perpLineIntPt(x1,y1, x2,y2, cx,cy, pix, piy);
  //cout << "** PerpLineIntPt  pix=" << pix << ", piy=" << piy << endl;

  double dist = hypot(cx-pix, cy-piy);
  //cout << "   dist=" << dist << endl;
  // Case 1: No intersection
  if(dist > crad)
    return(0);
  
  // Case 2: Tangent
  if(dist == crad) {
    rx1 = pix;
    ry1 = piy;
    return(1);
  }

  // Case 3: Line goes through center of circle
  if(dist == 0) {
    double line_ang1 = relAng(x1,y1, x1,y2);
    double line_ang2 = line_ang1 + 180;
    if(line_ang2 >= 360)
      line_ang2 -= 360;
    
    projectPoint(line_ang1, crad, cx,cy, rx1,ry1);
    projectPoint(line_ang2, crad, cx,cy, rx2,ry2);
    return(2);
  }

  // Case 4: General intersection case 
  double ang_to_perp_int = relAng(cx,cy, pix,piy);
  double delta_ang = (acos(dist / crad)) / M_PI * 180;
  double line_ang1 = ang_to_perp_int - delta_ang;
  double line_ang2 = ang_to_perp_int + delta_ang;

  //cout << " ) ang_to_per_int=" << ang_to_perp_int << endl;
  //cout << " ) delta_ang=" << delta_ang << endl;
  //cout << " ) line_ang1=" << line_ang1 << endl;
  //cout << " ) line_ang2=" << line_ang2 << endl;

  if(line_ang1 < 0)
    line_ang1 += 360;
  if(line_ang2 >= 360)
    line_ang2 -= 360;
  
  projectPoint(line_ang1, crad, cx,cy, rx1,ry1);
  projectPoint(line_ang2, crad, cx,cy, rx2,ry2);
  return(2);
}


//---------------------------------------------------------------
// Procedure: segCircleIntPts
//   Purpose: Given a line and a circle, determine where the line
//            intersects the circle. It will return the number of
//            intersection points and fill in the return points if
//            they exist.

int segCircleIntPts(double x1, double y1, double x2, double y2,
		    double cx, double cy, double crad,
		    double& rx1, double& ry1,
		    double& rx2, double& ry2)
{
  // First determine whether or where the line, defined by the
  // given segment, intersects the circle.
  int cross_pts = lineCircleIntPts(x1,y1,x2,y2, cx,cy,crad,
				   rx1,ry1,rx2,ry2);

  //cout << "segCircleIntPts:" << endl;
  //cout << "   cross_pts: " << cross_pts << endl;
  //cout << "   rx1: " << rx1 << endl;
  //cout << "   ry1: " << ry1 << endl;
  //cout << "   rx2: " << rx2 << endl;
  //cout << "   ry2: " << ry2 << endl;
    
  
  if(cross_pts == 0)
    return(0);

  double xmin = x1;
  double xmax = x2;
  if(x1 > x2) {
    xmin = x2;
    xmax = x1;
  }

  double ymin = y1;
  double ymax = y2;
  if(y1 > y2) {
    ymin = y2;
    ymax = y1;
  }

  // If the y-range is ~zero (~horizontal line), then we do not
  // check if the point lies on in the y-range. We rely on the
  // the point-generating function in lineCircleIntPts to assure
  // us that a point was generated on the ~horizontal line.
  // Same in the case with a ~vertical line.
  bool pt1_in_seg = true;
  if((ymax-ymin) > 0.000001) {
    if((ry1 < ymin) || (ry1 > ymax))
      pt1_in_seg = false;
  }
  if((xmax-xmin) > 0.000001) {
    if((rx1 < xmin) || (rx1 > xmax))
      pt1_in_seg = false;
  }
  
  if(cross_pts == 1) 
    return(pt1_in_seg);

  // By the above, we know we're dealing with TWO cross points

  bool pt2_in_seg = true;
  if((ymax-ymin) > 0.000001) {
    if((ry2 < ymin) || (ry2 > ymax))
      pt2_in_seg = false;
  }
  if((xmax-xmin) > 0.000001) {
    if((rx2 < xmin) || (rx2 > xmax))
      pt2_in_seg = false;
  }

  
  if(pt1_in_seg && pt2_in_seg)
    return(2);

  if(pt1_in_seg && !pt2_in_seg) {
    rx2 = 0;
    ry2 = 0;
    return(1);
  }
  
  if(!pt1_in_seg && pt2_in_seg) {
    rx1 = rx2;
    ry1 = ry2;
    rx2 = 0;
    ry2 = 0;
    return(1);
  }

  rx1 = 0;
  ry1 = 0;
  rx2 = 0;
  ry2 = 0;
  return(0);

}

//---------------------------------------------------------------
// Procedure: distPointToLine

double distPointToLine(double px, double py, double x1, double y1,
		       double x2, double y2)
{
  double ix,iy;
  perpLineIntPt(x1,y1,x2,y2,px,py,ix,iy);

  return(hypot(px-ix, py-iy));
}
  

//---------------------------------------------------------------
// Procedure: distPointToSegl

double distPointToSegl(double px, double py, const XYSegList& segl)
{
  if(segl.size() == 0)
    return(-1);

  if(segl.size() == 1) {
    double vx = segl.get_vx(0);
    double vy = segl.get_vy(0);
    return(hypot(px-vx, py-vy));
  }

  double min_dist = -1;
  for(unsigned int i=0; i<segl.size()-1; i++) {
    double vx1 = segl.get_vx(i);
    double vy1 = segl.get_vy(i);
    double vx2 = segl.get_vx(i+1);
    double vy2 = segl.get_vy(i+1);
    double dist = distPointToSeg(vx1,vy1, vx2,vy2, px, py);
    if((min_dist < 0) || (dist < min_dist))
      min_dist = dist;
  }
  return(min_dist);
}
  



//---------------------------------------------------------------
// Procedure: polyRayCPA
//   Purpose: Determine the closest point of approach of a point
//            originating at rx,ry and moving along the ray in
//            the direction of ray_angle, to the given polygon

double polyRayCPA(double rx, double ry, double ray_angle,
		  const XYPolygon& poly, double& rix, double& riy)
{
  // Part 1: Sanity check: This function requires a convex polygon
  // therefore >=3 vertices
  if(!poly.is_convex())
    return(-1);

  // Part 2: If the ray crosses any of the polygon segments, we're
  // done, cpa=0
  for(unsigned int i=0; i<poly.size(); i++) {
    // Segment first vertex
    double x1 = poly.get_vx(i);
    double y1 = poly.get_vy(i);
    // Segment second vertex
    double x2 = poly.get_vx(0);
    double y2 = poly.get_vy(0);
    if((i+1) < poly.size()) {
      x2 = poly.get_vx(i+1);
      y2 = poly.get_vy(i+1);
    }
    double ix = 0;
    double iy = 0;
    if(crossRaySeg(rx, ry, ray_angle, x1, y1, x2, y2, ix, iy)) {
      rix = ix;
      riy = iy;
      return(0);
    }
  }    
  // Part 3: Ray does notcross any of the polygon segments, so now
  // we calculate the ray CPA for all segments and take the min
  double min_cpa = -1;
  for(unsigned int i=0; i<poly.size(); i++) {
    // Segment first vertex
    double x1 = poly.get_vx(i);
    double y1 = poly.get_vy(i);
    // Segment second vertex
    double x2 = poly.get_vx(0);
    double y2 = poly.get_vy(0);
    if((i+1) < poly.size()) {
      x2 = poly.get_vx(i+1);
      y2 = poly.get_vy(i+1);
    }
    double ix = 0;
    double iy = 0;
    
    double cpa = segRayCPA(rx, ry, ray_angle, x1, y1, x2, y2, ix, iy);
    if((min_cpa < 0) || (cpa < min_cpa)) {
      min_cpa = cpa;
      rix = ix;
      riy = iy;
    }    
  }
  return(min_cpa);  
}


//---------------------------------------------------------------
// Procedure: randPointOnPoly()
//   Purpose: Find a random point on the boundary of the poly that
//            is in line-of-site from the given viewing vertex.
//            Try a random points inside the polygon's bounding box.
//   Returns: true if random point found. 
//            false otherwise, e.g., if viewpont is within poly

bool randPointOnPoly(double vx, double vy,
		     const XYPolygon& poly,
		     double& rx, double& ry)
{
  if(!poly.is_convex())
    return(false);
  if(poly.contains(vx, vy))
    return(false);

  double bmin = 0;
  double bmax = 0;
  bool ok = bearingMinMaxToPoly(vx,vy, poly, bmin,bmax);
  if(!ok)
    return(false);

  double bng_range = angleDiff(bmin, bmax);
  int  bng_choices = (int)(bng_range + 1);
 
  int rand_bng = rand() % bng_choices;

  double bng = angle360(bmin + (double)(rand_bng));

  double dist_to_poly = poly.dist_to_poly(vx, vy, bng);
  if(dist_to_poly < 0)
    return(false);
  
  double ix,iy;
  projectPoint(bng, dist_to_poly, vx, vy, ix, iy);
  rx = ix;
  ry = iy;
  return(true);
}


//---------------------------------------------------------------
// Procedure: polyWidth()

double polyWidth(XYPolygon poly, double angle)
{
  poly.rotate(angle);

  double xmin = 0;
  double xmax = 0;
  
  unsigned int vsize = poly.size();
  for(unsigned int i=0; i<vsize; i++) {
    double px = poly.get_vx(i);
    if(i == 0) {
      xmin = px;
      xmax = px;
    }
    else if(px < xmin)
      xmin = px;
    else if(px > xmax)
      xmax = px;
  }

  return(xmax - xmin);
}

//---------------------------------------------------------------
// Procedure: polyHeight()

double polyHeight(XYPolygon poly, double angle)
{
  poly.rotate(angle);

  double ymin = 0;
  double ymax = 0;
  
  unsigned int vsize = poly.size();
  for(unsigned int i=0; i<vsize; i++) {
    double py = poly.get_vy(i);
    if(i == 0) {
      ymin = py;
      ymax = py;
    }
    else if(py < ymin)
      ymin = py;
    else if(py > ymax)
      ymax = py;
  }

  return(ymax - ymin);
}


//---------------------------------------------------------------
// Procedure: shiftVertices()
//   Purpose: Shift each vertex index to be lower by one, and moving
//            the first vertex to the end.
//            A convenience function for certain other geometric
//            calculations. 

void shiftVertices(vector<double>& vx, vector<double>& vy)
{
  if((vx.size() != vy.size()) || (vx.size() < 2))
    return;
  
  vector<double> new_vx;
  vector<double> new_vy;
  for(unsigned int i=1; i<vx.size(); i++) {
    new_vx.push_back(vx[i]);
    new_vy.push_back(vy[i]);
  }
  new_vx.push_back(vx[0]);
  new_vy.push_back(vy[0]);

  vx = new_vx;
  vy = new_vy;
}


  
