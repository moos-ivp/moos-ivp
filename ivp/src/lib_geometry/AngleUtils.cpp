/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AngleUtils.cpp                                       */
/*    DATE: Nov 26, 2000                                         */
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
#include <cstdlib>
#include <cmath>
#include "AngleUtils.h"
#include "GeomUtils.h"

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
//        x2,y2  o-<<----------o (x1,y1)                                    
//                            /                                       
//                          /                                         
//                        /                                           
//                      /                                            
//                    /                                               
//          (x0,y0) o                                                
//                                                      
//      Note: From Cormen, Leiserson, Rivest and Stein:

bool threePointTurnLeft(double x0, double y0, 
			double x1, double y1,
			double x2, double y2)
{ 
  double cross_product = threePointXProduct(x0,y0, x1,y1, x2,y2);

  if(cross_product < -0.01)
    return(true);

  return(false);
}

//-------------------------------------------------------------
// Procedure: threePointXProduct
//                                                      
//        x2,y2  o-<<----------o (x1,y1)                                    
//                            /                                       
//                          /                                         
//                        /                                           
//                      /                                            
//                    /                                               
//          (x0,y0) o                                                
//                                                      
//      Note: From Cormen, Leiserson, Rivest and Stein:

double threePointXProduct(double x0, double y0, 
			  double x1, double y1,
			  double x2, double y2)
{ 
  double ax = x2-x0;
  double ay = y2-y0;
  double bx = x1-x0; 
  double by = y1-y0; 

  // Now compute the cross product of a x b
  double cross_product = (ax*by) - (bx*ay);

  return(cross_product);
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
  else if(xa > xb) {
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
  else if(yb < ya) {
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
// Procedure: angleMinAcute
//   Purpose: Given the two angles (headings from a center pt),
//            determine in which direction they make an acute angle
//            and return the lesser of the two.
//  Examples: angleMinAcute(10,20) = 10
//            angleMinAcute(10,350) = 350
//            angleMinAcute(89,271) = 271
//
//  (10,20) = 10     10-20 = -10   = 350   *larger:choose(10)
//                   20-10 = 10    = 10
//
//  (10,350) = 350   10-350 = -340 = 20    
//                   350-10 = 340  = 340   *larger:choose(350)
//
//  (89,271) = 271   89-271 = -182 = 178
//                   271-89 = 182  = 182   *larger:choose(271)

double angleMinAcute(double ang1, double ang2)
{
  if(angle360(ang1-ang2) >= angle360(ang2-ang1))
    return(ang1);

  return(ang2);
}

//---------------------------------------------------------------
// Procedure: angleMaxAcute
//   Purpose: Given the two angles (headings from a center pt),
//            determine in which direction they make an acute angle
//            and return the greater of the two.
//  Examples: angleMaxAcute(10,20) = 20
//            angleMaxAcute(10,350) = 10
//            angleMaxAcute(89,271) = 89
//
//  (10,20) = 20     10-20 = -10   = 350
//                   20-10 = 10    = 10    *smaller:choose(20)
//
//  (10,350) = 10    10-350 = -340 = 20    *smaller:choose(10)
//                   350-10 = 340  = 340
//
//  (89,271) = 271   89-271 = -182 = 178   *smaller:choose(89)
//                   271-89 = 182  = 182

double angleMaxAcute(double ang1, double ang2)
{
  if(angle360(ang1-ang2) < angle360(ang2-ang1))
    return(ang1);

  return(ang2);
}

//---------------------------------------------------------------
// Procedure: angleMinReflex
//   Purpose: Given the two angles (headings from a center pt),
//            determine in which direction they make a reflex angle
//            (geq 180 degs) and return the lesser of the two.
//  Examples: angleMinReflex(10,20) = 20
//            angleMinReflex(10,350) = 10
//            angleMdinReflex(89,271) = 89
//
//  (10,20) = 20     10-20 = -10   = 350
//                   20-10 = 10    = 10    *smaller:choose(20)
//
//  (10,350) = 10    10-350 = -340 = 20    *smaller:choose(10)
//                   350-10 = 340  = 340
//
//  (89,271) = 89    89-271 = -182 = 178   *smaller:choose(89)
//                   271-89 = 182  = 182

double angleMinReflex(double ang1, double ang2)
{
  if(angle360(ang1-ang2) < angle360(ang2-ang1))
    return(ang1);

  return(ang2);
}

//---------------------------------------------------------------
// Procedure: angleMaxReflex
//   Purpose: Given the two angles (headings from a center pt),
//            determine in which direction they make a reflex angle
//            (geq 180 degs) and return the greater of the two.
//  Examples: angleMaxReflex(10,20) = 10
//            angleMaxReflex(10,350) = 350
//            angleMaxReflex(89,271) = 271
//
//  (10,20) = 10     10-20 = -10   = 350
//                   20-10 = 10    = 10
//
//  (10,350) = 350   10-350 = -340 = 20
//                   350-10 = 340  = 340
//
//  (89,271) = 271   89-271 = -182 = 178
//                   271-89 = 182  = 182

double angleMaxReflex(double ang1, double ang2)
{
  if(angle360(ang1-ang2) >= angle360(ang2-ang1))
    return(ang1);

  return(ang2);
}

//---------------------------------------------------------------
// Procedure: angleMinContains
//   Purpose: Given three angles (headings from a center pt).
//            Assuming first two angles are not equal, the 3rd angle
//            must be between first two in one wrap direction or
//            another. Determine direction and report min/start angle.
//  Examples: angleMinContains(10,20,5) = 10
//            angleMinContains(10,350,2)   = 350
//            angleMinContains(10,350,20)  = 10
//            angleMinContains(89,271,0)   = 271
//            angleMinContains(89,271,180) = 89

double angleMinContains(double ang1, double ang2, double ang3)
{
  // Sanity check
  if(ang1 == ang2)
    return(ang1);

  // Order the two angles
  double anglow = ang1; 
  double anghgh = ang2; 
  if(anglow > anghgh) {
    anglow = ang2;
    anghgh = ang1;
  }

  // Check the range
  if((ang3 >= anglow) && (ang3 <= anghgh))
    return(anglow);
  return(anghgh);
}

//---------------------------------------------------------------
// Procedure: angleMaxContains
//   Purpose: Given three angles (headings from a center pt).
//            Assuming first two angles are not equal, the 3rd angle
//            must be between first two in one wrap direction or
//            another. Determine direction and report max/end angle.
//  Examples: angleMaxContains(10,20,5) = 20
//            angleMaxContains(10,350,2)   = 10
//            angleMaxContains(10,350,20)  = 350
//            angleMaxContains(89,271,0)   = 89
//            angleMaxContains(89,271,180) = 271

double angleMaxContains(double ang1, double ang2, double ang3)
{
  // Sanity check
  if(ang1 == ang2)
    return(ang1);

  // Order the two angles
  double anglow = ang1; 
  double anghgh = ang2; 
  if(anglow > anghgh) {
    anglow = ang2;
    anghgh = ang1;
  }

  // Check the range
  if((ang3 >= anglow) && (ang3 <= anghgh))
    return(anghgh);
  return(anglow);
}

//---------------------------------------------------------------
// Procedure: angleMinExcludes
//   Purpose: Given three angles (headings from a center pt).
//            Assuming first two angles are not equal, the 3rd angle
//            must be outside first two in one wrap direction or
//            another. Determine direction and report min/start angle.
//  Examples: angleMinExcludes(10,20,5) = 20
//            angleMinExcludes(10,350,2)   = 10
//            angleMinExcludes(10,350,20)  = 350
//            angleMinExcludes(89,271,0)   = 89
//            angleMinExcludes(89,271,180) = 271

double angleMinExcludes(double ang1, double ang2, double ang3)
{
  // Sanity check
  if(ang1 == ang2)
    return(ang1);

  // Order the two angles
  double anglow = ang1; 
  double anghgh = ang2; 
  if(anglow > anghgh) {
    anglow = ang2;
    anghgh = ang1;
  }

  // If either angle equals the third, then there really is no
  // reasonable answer. This should have been checked by the caller.
  // But since this is a "min" function, we'll just return lowest angle.
  if((ang1 == ang3) || (ang2 == ang3))
    return(anglow);
  
  // Check the range
  if((ang3 < anglow) || (ang3 > anghgh))
    return(anglow);
  return(anghgh);
}

//---------------------------------------------------------------
// Procedure: angleMaxExcludes
//   Purpose: Given three angles (headings from a center pt).
//            Assuming first two angles are not equal, the 3rd angle
//            must be outside first two in one wrap direction or
//            another. Determine direction and report max/end angle.
//  Examples: angleMaxExcludes(10,20,5) = 10
//            angleMaxExcludes(10,350,2)   = 350
//            angleMaxExcludes(10,350,20)  = 10
//            angleMaxExcludes(89,271,0)   = 271
//            angleMaxExcludes(89,271,180) = 89

double angleMaxExcludes(double ang1, double ang2, double ang3)
{
  // Sanity check
  if(ang1 == ang2)
    return(ang1);

  // Order the two angles
  double anglow = ang1; 
  double anghgh = ang2; 
  if(anglow > anghgh) {
    anglow = ang2;
    anghgh = ang1;
  }

  // If either angle equals the third, then there really is no
  // reasonable answer. This should have been checked by the caller.
  // But since this is a "max" function, we'll just return lowest angle.
  if((ang1 == ang3) || (ang2 == ang3))
    return(anghgh);
  
  // Check the range
  if((ang3 < anglow) || (ang3 > anghgh))
    return(anghgh);
  return(anglow);
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
//      Note: The test angle range, in terms of wrap-around, will
//            be the range that forms an ACUTE angle. Thus if the
//            first two args are (350,10) or (10,350), these are
//            treated the same.
//  Examples: 10, 20, 15    --> true
//            20, 10, 15    --> true
//            20, 350, 15   --> true
//            100, 280, 99  --> true (180 range accepts all)
//            100, 280, 101 --> true (180 range accepts all)


bool containsAngle(double aval, double bval, double qval)
{
  // Convert to [0, 360) rather than assume.
  aval = angle360(aval);
  bval = angle360(bval);
  
  if(aval == bval)
    return(qval == bval);

  // If the given angle is 180, then all query angles will pass
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

  return(angle360(raw_rel_bearing)); // Super important to put in [0,360)
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
  if(poly.size() == 0)
    return(false);
  
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

//---------------------------------------------------------------
// Procedure: turnGap
//   Purpose: Determine the min distance between a given line and a circle
//            formed by ownship immediately starting a circular turn from
//            its present position, with the given radius, turning in the
//            given direction.

double turnGap(double osx, double osy, double osh, double tradius,
	       double px1, double py1, double px2, double py2, bool tright)
{
  // Step 1: Find the angle between ownship and the circle center depending
  //         on whether the vehicle is turning hard right or left
  double angle_from_os = 0;
  if(tright)
    angle_from_os = angle360(osh + 90);
  else
    angle_from_os = angle360(osh - 90);

  // Step 2: Find the circle center
  double cx, cy;
  projectPoint(angle_from_os, tradius, osx, osy, cx, cy);

  // Step 3: Calculate the distance (gap)
  double dist = distCircleToLine(cx, cy, tradius, px1, py1, px2, py2);

  return(dist);
}

//---------------------------------------------------------------
// Procedure: headingAvg()
//   Purpose: Determine the average heading given a list of headings

double headingAvg(std::list<double> heading_vals)
{
  double ssum = 0.0;
  double csum = 0.0;

  std::list<double>::iterator p;
  for(p = heading_vals.begin(); p!=heading_vals.end(); p++) {
    double hdg = *p;
  
    double s = sin(hdg * M_PI / 180.0);
    double c = cos(hdg * M_PI / 180.0);

    ssum += s;
    csum += c;    
  }

  double avg = atan2(ssum, csum) * 180.0 / M_PI;
  if(avg < 0.0)
    avg += 360.0;

  return(avg);
}

//---------------------------------------------------------------
// Procedure: headingAvg()
//   Purpose: Determine the average heading given a two headings
//      Note: Convenience function

double headingAvg(double h1, double h2)
{
  std::list<double> pair;
  pair.push_front(h1);
  pair.push_front(h2);
  return(headingAvg(pair));
}

//----------------------------------------------------------------
// Procedure: portTurn()
//   Purpose: Given a current ownship heading and prospective new
//            heading, determine if this would result in a port or
//            starboard turn. Presuming a vessel would turn port
//            for delta headings in the range of [0,180) and
//            starboard turns in the range of (0,180]. 

bool portTurn(double osh, double hdg)
{
  osh = angle360(osh);
  hdg = angle360(hdg);
  if(hdg > osh) {
    if((hdg - osh) < 180)
      return(false);
  }
  else {
    if((osh - hdg) > 180)
      return(false);
  }
  return(true);
}

//----------------------------------------------------------------
// Procedure: ptPortOfOwnship()
//   Purpose: Given ownship position and heading, and given point
//            Determine if the point is on the port side.

bool ptPortOfOwnship(double osx, double osy, double osh,
		     double ptx, double pty)
{
  // Sanity check
  if((osx == ptx) && (osy == pty))
    return(false);

  double rel_bng = relBearing(osx, osy, osh, ptx, pty);
  if((rel_bng > 180) && (rel_bng < 360))
    return(true);
  
  return(false);
}

//----------------------------------------------------------------
// Procedure: ptStarOfOwnship()
//   Purpose: Given ownship position and heading, and given point
//            Determine if the point is on the starboard side.

bool ptStarOfOwnship(double osx, double osy, double osh,
		     double ptx, double pty)
{
  // Sanity check
  if((osx == ptx) && (osy == pty))
    return(false);

  double rel_bng = relBearing(osx, osy, osh, ptx, pty);
  if((rel_bng > 0) && (rel_bng < 180))
    return(true);
  
  return(false);
}

//----------------------------------------------------------------
// Procedure: polyPortOfOwnship()
//   Purpose: Given ownship position and heading, and given poly
//            Determine if the poly is completely on the port side.

bool polyPortOfOwnship(double osx, double osy, double osh,
		       XYPolygon poly)
{
  // Sanity check
  if(!poly.is_convex())
    return(false);

  for(unsigned int i=0; i<poly.size(); i++) {
    double vx = poly.get_vx(i);
    double vy = poly.get_vy(i);
    if(!ptPortOfOwnship(osx, osy, osh, vx, vy))
      return(false);
  }
  
  return(true);
}

//----------------------------------------------------------------
// Procedure: polyStarOfOwnship()
//   Purpose: Given ownship position and heading, and given poly
//            Determine if the poly is completely on the
//            starboard side.

bool polyStarOfOwnship(double osx, double osy, double osh,
		       XYPolygon poly)
{
  // Sanity check
  if(!poly.is_convex())
    return(false);

  for(unsigned int i=0; i<poly.size(); i++) {
    double vx = poly.get_vx(i);
    double vy = poly.get_vy(i);
    if(!ptStarOfOwnship(osx, osy, osh, vx, vy))
      return(false);
  }
  
  return(true);
}


