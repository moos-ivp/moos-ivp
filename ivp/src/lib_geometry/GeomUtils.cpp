/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GeomUtils.cpp                                        */
/*    DATE: May 8, 2005 Sunday Morning at Bruegger's             */
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
#include <cmath>
#include <cstdlib>
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: distPointToPoint

double distPointToPointXXX(double x1, double y1, double x2, double y2)
{
  return(hypot((x1-x2), (y1-y2)));
}
  
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
// Procedure: distPointToSeg
//   Purpose: Return the distance from the segment given by x1,y1 
//            and x2,y2 to the point given by px,py

double distPointToSeg(double x1, double y1, double x2, 
		      double y2, double px, double py)
{
  double ix, iy;
  perpSegIntPt(x1, y1, x2, y2, px, py, ix, iy);
  return(distPointToPoint(px, py, ix, iy));
}

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
    if(slope1 == slope2)
      return(intercept1 == intercept2);
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

#if 1
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
  if(ray_vert && line_vert) {
    if(x1==x3) {
      ix = x1; iy = 0;
      return(true);
    }
    else
      return(false);
  }
  // Case 2 - both ray and line horizontal (intersection pt not unique)
  if(ray_horz && line_horz) {
    if(y1==y3) {
      iy = y1; ix = 0;
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
      ix = x1; iy = y1;       // any point. vertex of ray is fine.
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
  else if(ray_angle < 180) {
    if((ix <= x1) && (iy >= y1))
      return(true);
  }

  ix = 0;
  iy = 0;
  return(false);
}
#endif


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
//   Purpose: 
//            

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
//   Purpose: 
//            

void perpLineIntPt(double x1, double y1, double x2, double y2, 
		   double qx, double qy, double& rx, double& ry)
{
  // handle the special case where the segment is vertical
  if(x1 == x2) {
    rx = x1;
    ry = qy;
    return;
  }

  // handle the special case where the segment is horizontal
  if(y1 == y2) {
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
//   Purpose: From a point outside a convex polygons, determine the two 
//            bearing angles to the polygon forming a pseudo tangent.
//   Returns: true if x,y is NOT in the polygon, false otherwise.

bool bearingMinMaxToPoly(double osx, double osy, const XYPolygon& poly,
			 double& bmin, double& bmax)
{
  // Step 1: Sanity checks
  if(poly.size() == 0)
    return(false);
  if(poly.is_convex() && poly.contains(osx, osy))
    return(false);
  
  // Step 2: Determine all the angles, noting their quadrants.
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
  // Step 3: Determine if we have an angle wrap-around situation
  if(quad_1  &&  !quad_2  && !quad_3  &&  !quad_4)          // Case 1:  1
    wrap_around = false;
  else if(!quad_1 &&   quad_2  && !quad_3  &&  !quad_4)     // Case 2:  2
    wrap_around = false;
  else if(!quad_1 &&  !quad_2  &&  quad_3  &&  !quad_4)     // Case 3:  3
    wrap_around = false;
  else if(!quad_1 &&  !quad_2  && !quad_3  &&   quad_4)     // Case 4:  4
    wrap_around = false;

  else if(quad_1  &&   quad_2  && !quad_3  &&  !quad_4)     // Case 5:  1,2
    wrap_around = false;
  else if(!quad_1 &&   quad_2  &&  quad_3  &&  !quad_4)     // Case 6:  2,3
    wrap_around = false;
  else if(!quad_1 &&  !quad_2  &&  quad_3  &&   quad_4)     // Case 7:  3,4
    wrap_around = false;
  else if(quad_1  &&  !quad_2  && !quad_3  &&   quad_4)     // Case 8:  4,1   YYYY
    wrap_around = true;
  else if(quad_1  &&   quad_2  &&  quad_3  &&  !quad_4)     // Case 9:  1,2,3 
    wrap_around = false;
  else if(!quad_1 &&   quad_2  &&  quad_3  &&   quad_4)     // Case 10: 2,3,4 
    wrap_around = false;
  else if(quad_1  &&  !quad_2  &&  quad_3  &&   quad_4)     // Case 11: 3,4,1 YYYY
    wrap_around = true;
  else if(quad_1  &&   quad_2  && !quad_3  &&   quad_4)     // Case 12: 4,1,2 YYYY
    wrap_around = true;

  else if(quad_1  &&  !quad_2  &&  quad_3  &&  !quad_4) {   // Case 13: 1,3
    double px, py;
    poly.closest_point_on_poly(osx, osy, px, py);
    double os_angle = relAng(osx, osy, px, py);
    if((os_angle > 45) && (os_angle <= 225))
      wrap_around = false;
    else
      wrap_around = true;
  }
  else if(!quad_1 &&   quad_2  && !quad_3  &&   quad_4) {   // Case 14: 2,4
    double px, py;
    poly.closest_point_on_poly(osx, osy, px, py);
    double os_angle = relAng(osx, osy, px, py);
    if((os_angle > 135) && (os_angle <= 315))
      wrap_around = false;
    else
      wrap_around = true;
  }

  // Step 4: Now determine the "min" and "max" bearing angles
  if(!wrap_around) {
    bmin = 360;
    bmax = 0;
    for(i=0; i<psize; i++) {
      if(angles[i] < bmin)
	bmin = angles[i];
      else if(angles[i] > bmax)
	bmax = angles[i];
    }
  }
  else {
    bmin = 360;
    bmax = 0;
    for(i=0; i<psize; i++) {
      if((angles[i] > 180) && (angles[i] < bmin))
	bmin = angles[i];
      else if((angles[i] <= 180) && (angles[i] > bmax))
	bmax = angles[i];
    }
  }
  
  return(true);
}
