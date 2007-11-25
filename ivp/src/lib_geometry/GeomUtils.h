/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: GeomUtils.h                                          */
/*    DATE: May 8, 2005 Sunday morning at Brueggers              */
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
 
#ifndef XY_GEOM_UTILS_HEADER
#define XY_GEOM_UTILS_HEADER

// Determines the distance between two points
double distPointToPoint(double x1, double y1, double x2, double y2);

// Determine the distance from a line segment to a point (px,py)
double distPointToSeg(double x1, double y1, double x2, 
		      double y2, double px, double py);

// Determine the distance from a line segment to a point (px,py)
// at a particular given angle. Returns -1 if doesn't intersect
double distPointToSeg(double x1, double y1, double x2, double y2,
                      double px, double py, double ANGLE);
		      

// Determine the distance between two line segments
double distSegToSeg(double x1, double y1, double x2, double y2, 
		    double x3, double y3, double x4, double y4);

// Determine where two lines intersect
bool   linesCross(double x1, double y1, double x2, double y2,
		  double x3, double y3, double x4, double y4,
		  double &ix, double& iy);

// Determine if two line segments intersect
bool   segmentsCross(double x1, double y1, double x2, double y2,
		     double x3, double y3, double x4, double y4);

// Determine the angle between the two segments x1,y1<-->x2,y2
// and  x2,y2<-->x3,y3
double segmentAngle(double x1, double y1, double x2, double y2,
		    double x3, double y3);

// Determine a point on a segment that, with another point, makes
// a line that is a right angle to the segment
void   perpSegIntPt(double x1, double y1, double x2, double y2,
		    double qx, double qy, double& rx, double& ry);

// Determine a point on a line that, with another point, makes
// a line that is a right angle to the given line
void   perpLineIntPt(double x1, double y1, double x2, double y2,
		     double qx, double qy, double& rx, double& ry);

// Determine a point that is a certain angle and distance from 
// another point
void   projectPoint(double ANGLE, double DIST, double cx, 
		    double cy,    double& rx,  double &ry);

void   addVectors(double deg1, double mag1, double deg2, double mag2,
		  double& rdeg, double& rmag); 

// DEPRECATED INTERFACES
double distToPoint(double x1, double y1, double x2, double y2);

double distToSegment(double x1, double y1, double x2, 
		     double y2, double px, double py);
  
#endif









