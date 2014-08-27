/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AngleUtils.cpp                                       */
/*    DATE: Nov 26, 2000                                         */
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
#include <cmath>
#include "AngleUtils.h"

#ifndef M_PI
#define M_PI 3.1415926
#endif


//-------------------------------------------------------------
// Procedure: angleFromThreePoints
//   Purpose: Returns the angle in a triangle given by three points
//            The particular angle of the three angles in the triangle
//            is the angle at the first given point.
//
//  a^2 = b^2 + c^2 - 2bc cos(A)   Law of Cosines
//  
//           b^2 + c^2 - a^2
//  cos(A) = ---------------
//                 2bc
//
//                                            (x2,y2)    B
//                                                     o
//                                                /   |
//                                             /     |
//                                         /        |
//                                     /           |
//                         (c)     /              |
//                             /                 |
//                         /                    |  (a)
//                     /                       |
//                 /                          |
//             /                             |
//         /                                |
//     /                                   |
//   o-------------------------------------o (x3,y3)
// A (x1,y1)       (b)                     C
//
//

double angleFromThreePoints(double x1, double y1, 
			    double x2, double y2,
			    double x3, double y3)
{ 
  double a = hypot((x2-x3), (y2-y3));  // pythag distance 
  double b = hypot((x1-x3), (y1-y3));  // pythag distance 
  double c = hypot((x1-x2), (y1-y2));  // pythag distance 

  double numerator   = (b*b)+(c*c)-(a*a);
  double denominator = (2*b*c);
  if(denominator == 0)
    return(0);

  double rhs = numerator / denominator;

  double angle_radians = acos(rhs);

  double angle_degrees = ((angle_radians / M_PI) * 180);

  return(angle_degrees);
}

//-------------------------------------------------------------
// Procedure: threePointTurnLeft
//
//      Note: From Cormen, Leiserson, Rivest and Stein:

bool threePointTurnLeft(double x0, double y0, 
			double x1, double y1,
			double x2, double y2)
{ 
  
  double ax = x2-x0;
  double ay = y2-y0;
  double bx = x1-x0; 
  double by = y1-y0; 
  // Now compute the cross product of a x b
  
  double cross_product = (ax*by) - (bx*ay);

  if(cross_product < 0)
    return(true);

  return(false);
}



//-------------------------------------------------------------
// Procedure: relAng
//   Purpose: Returns relative angle of pt B to pt A. Treats A
//            as the center.
//
//                   0
//                   |
//                   |
//         270 ----- A ----- 90      
//                   |
//                   |
//                  180

double relAng(double xa, double ya, double xb, double yb)
{ 
  if((xa==xb)&&(ya==yb))
    return(0);

  double w   = 0;
  double sop = 0;

  if(xa < xb) {
    if(ya==yb)  
      return(90.0);
    else
      w = 90.0;
  }
  if(xa > xb) {
    if(ya==yb)  
      return(270.0);
    else
      w = 270.0;
  }
  if(ya < yb) {
    if(xa == xb) 
      return(0.0);
    if(xb > xa) 
      sop = -1.0;
    else 
      sop =  1.0;
  }
  if(yb < ya) {
    if(xa == xb) 
      return(180);
    if(xb >  xa) 
      sop =  1.0;
    else 
      sop = -1.0;
  }

  double ydiff = yb-ya;
  double xdiff = xb-xa;
  if(ydiff<0) ydiff = ydiff * -1.0;
  if(xdiff<0) xdiff = xdiff * -1.0;

  double avalPI = atan(ydiff/xdiff);
  double avalDG = radToDegrees(avalPI);
  double retVal = (avalDG * sop) + w;

  retVal = angle360(retVal);

  return(retVal);
}

//-------------------------------------------------------------
// Procedure: relAng
//   Purpose: Returns relative angle of pt B to pt A. Treats A
//            as the center.
//
//                   0
//                   |
//                   |
//         270 ----- A ----- 90      
//                   |
//                   |
//                  180

double relAng(const XYPoint& a, const XYPoint& b)
{ 
  return(relAng(a.get_vx(), a.get_vy(), b.get_vx(), b.get_vy()));
}


//---------------------------------------------------------------
// Procedure: radAngleWrap

double radAngleWrap(double radval) 
{
  if((radval <= M_PI) && (radval >= -M_PI))
    return(radval);

  if(radval > M_PI) 
    return(radval - (2*M_PI));
  else
    return(radval + (2*M_PI));
}


//---------------------------------------------------------------
// Procedure: degToRadians

double degToRadians(double degval) 
{
  return((degval/180.0) * M_PI);
}


//---------------------------------------------------------------
// Procedure: radToDegrees

double radToDegrees(double radval)
{
  return((radval / M_PI) * 180);
}


//---------------------------------------------------------------
// Procedure: headingToRadians
// Converts true heading (clockwize from N) to
// radians in a counterclockwize x(E) - y(N) coordinate system
// .

double headingToRadians(double degval) 
{
  return(radAngleWrap( (90.0-degval)*M_PI/180.0));
}


//---------------------------------------------------------------
// Procedure: radToHeading
// Converts radians in a counterclockwize x(E) - y(N) coordinate system
// to true heading (clockwize from N).

double radToHeading(double radval)
{
  return(angle360( 90.0-(radval / M_PI) * 180));
}


//---------------------------------------------------------------
// Procedure: speedInHeading
//   Purpose: Given a heading and speed of a vehicle, and another heading,
//            determine the speed of the vehicle in that heading.

double speedInHeading(double osh, double osv, double heading)
{
  // Part 0: handle simple special case
  if(osv == 0)
    return(0);

  // Part 1: determine the delta heading [0, 180]
  double delta = angle180(osh - heading);
  if(delta < 0)
    delta *= -1;
  
  // Part 2: Handle easy special cases
  if(delta == 0)
    return(osv);
  if(delta == 180)
    return(-osv);
  if(delta == 90)
    return(0);

  // Part 3: Handle the general case
  double radians = degToRadians(delta);
  double answer  = cos(radians) * osv;
  
  return(answer);
}


//---------------------------------------------------------------
// Procedure: angle180
//   Purpose: Convert angle to be strictly in the rang (-180, 180].

double angle180(double degval)
{
  while(degval > 180)
    degval -= 360.0;
  while(degval <= -180)
    degval += 360.0;
  return(degval);
}

//---------------------------------------------------------------
// Procedure: angle360
//   Purpose: Convert angle to be strictly in the rang [0, 360).

double angle360(double degval)
{
  while(degval >= 360.0)
    degval -= 360.0;
  while(degval < 0.0)
    degval += 360.0;
  return(degval);
}

//---------------------------------------------------------------
// Procedure: angleDiff
//   Purpose: Determine the difference in angle degrees between 
//            two given angles, ensuring the range [0, 180).

double angleDiff(double ang1, double ang2)
{
  ang1 = angle360(ang1);
  ang2 = angle360(ang2);
  double diff;
  if(ang2 > ang1)
    diff = ang2 - ang1;
  else
    diff = ang1 - ang2;

  if(diff >= 180)
    diff = 360 - diff;
  return(diff);
}

//---------------------------------------------------------------
// Procedure: aspectDiff
//   Purpose: Determine the difference in degrees between the two
//            given aspect angles, ensuring the range [0, 90].

double aspectDiff(double ang1, double ang2)
{
  double angle_diff_1 = angleDiff(ang1, ang2);
  double angle_diff_2 = angleDiff(ang1, ang2+180);

  if(angle_diff_1 < angle_diff_2)
    return(angle_diff_1);
  else
    return(angle_diff_2);
}

//---------------------------------------------------------------
// Procedure: containsAngle
//   Purpose: Given a range of angle, in the domain [0, 360),
//            determine if the query angle lies within.

bool containsAngle(double aval, double bval, double qval)
{
  // Convert to [0, 360) rather than assume.
  aval = angle360(aval);
  bval = angle360(bval);
  
  if(aval == bval)
    return(qval == bval);

  if(fabs(bval-aval) == 180)
    return(true);

  if(aval > bval) {
    double tmp = aval;
    aval = bval;
    bval = tmp;
  }
    
  if((bval-aval) > 180)
    return((qval >= bval) || (qval <= aval));
  else
    return((qval >= aval) && (qval <= bval));
}

//---------------------------------------------------------------
// Procedure: relBearing
//   Purpose: returns the relative bearing of a contact at position cnx,cny to
//            ownship positioned as osx,osy at a heading of osh.
//   Returns: Value in the range [0,360).

double  relBearing(double osx, double osy, double osh, double cnx, double cny)
{
  double angle_os_to_cn = relAng(osx, osy, cnx, cny);
  
  double raw_rel_bearing = angle_os_to_cn - osh;  

  return(angle360(raw_rel_bearing));
}

//---------------------------------------------------------------
// Procedure: absRelBearing
//   Purpose: returns the absolute relative bearing, for example:
//            359 becomes 1
//            270 becomes 90
//            181 becomes 179
//            180 becomes 180
//   Returns: Value in the range [0,180].


double absRelBearing(double osx, double osy, double osh, double cnx, double cny)
{
  double rel_bearing = relBearing(osx, osy, osh, cnx, cny);
  
  double abs_relative_bearing = angle180(rel_bearing);
  if(abs_relative_bearing < 0)
    abs_relative_bearing *= -1;
  
  return(abs_relative_bearing);
}

//---------------------------------------------------------------
// Procedure: totAbsRelBearing
//   Returns: Value in the range [0,360].


double totAbsRelBearing(double osx, double osy, double osh, 
			double cnx, double cny, double cnh)
{
  double abs_rel_bearing_os_cn = absRelBearing(osx, osy, osh, cnx, cny);
  double abs_rel_bearing_cn_os = absRelBearing(cnx, cny, cnh, osx, osy);

  return(abs_rel_bearing_os_cn + abs_rel_bearing_cn_os);
}


//---------------------------------------------------------------
// Procedure: polyAft
//   Returns: true if the convex polygon is entirely aft of the vehicle
//            given its present position and heading


bool polyAft(double osx, double osy, double osh, XYPolygon poly, double xbng)

{
  // The xbng parameter generalizes this function. Normally a point is "aft" of
  // ownship its relative bearing is in the range (90,270). With the xbng 
  // parameter, "aft" can be generalized, e.g., xbng=10 means the polygon must
  // be at least 10 degrees abaft of beam.

  if(xbng < -90)
    xbng = -90;
  else if(xbng > 90)
    xbng = 90;

  unsigned int i, psize = poly.size();
  for(i=0; i<psize; i++) {
    double vx = poly.get_vx(i);
    double vy = poly.get_vy(i);
    
    double relbng = relBearing(osx, osy, osh, vx, vy);
    if(relbng <= (90+xbng)) 
      return(false);
    if(relbng >= (270-xbng)) 
      return(false);
  }
  return(true);
}



