/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AngleUtils.cpp                                       */
/*    DATE: Nov 26, 2000                                         */
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
  return( radAngleWrap( (90.0-degval)*M_PI/180.0 ) );
}


//---------------------------------------------------------------
// Procedure: radToHeading
// Converts radians in a counterclockwize x(E) - y(N) coordinate system
// to true heading (clockwize from N).

double radToHeading(double radval)
{
  return( angle360( 90.0-(radval / M_PI) * 180) );
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
  while(degval >= 360)
    degval -= 360.0;
  while(degval < 0)
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











