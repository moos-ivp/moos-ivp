/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: GeomUtils.h                                          */
/*    DATE: May 8, 2005 Sunday morning at Brueggers              */
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
 
#ifndef XY_GEOM_UTILS_HEADER
#define XY_GEOM_UTILS_HEADER

#include <vector>
#include "XYPoint.h"
#include "XYPolygon.h"
#include "XYSeglr.h"
#include "HintHolder.h"

// Determines the distance between two points
double distPointToPointXXX(double x1, double y1, double x2, double y2);
double distPointToPoint(double x1, double y1, double x2, double y2);
double distPointToPoint(const XYPoint& pt1, const XYPoint& pt2);

// Determine the distance from a line segment to a point (px,py)
double distPointToSeg(double x1, double y1, double x2, 
		      double y2, double px, double py);

// Determine the distance from a line segment to a point (px,py)
double distPointToSeg(double x1, double y1, double x2, 
		      double y2, double px, double py,
		      double& ix, double& iy);

// Determine the distance from a line segment to a point (px,py)
// at a particular given angle. Returns -1 if doesn't intersect
double distPointToSeg(double x1, double y1, double x2, double y2,
                      double px, double py, double ANGLE);

// Determine the distance between two line segments
double distSegToSeg(double x1, double y1, double x2, double y2, 
		    double x3, double y3, double x4, double y4);

// Determine whether and where two lines intersect
bool   linesCross(double x1, double y1, double x2, double y2,
		  double x3, double y3, double x4, double y4,
		  double &ix, double& iy);

// Determine whether and where a given ray and line intersect
bool   lineRayCross(double rx, double ry, double ray_angle,
		    double x1, double y1, double x2, double y2,
		    double &ix, double& iy);

// Determine whether and where a given line and segment intersect
bool   lineSegCross(double sx1, double sy1, double sx2, double sy2,
		    double lx3, double ly3, double lx4, double ly4,
		    double &ix1, double& iy1, double& ix2, double& iy2);

// Determine if two line segments intersect
bool   segmentsCross(double x1, double y1, double x2, double y2,
		     double x3, double y3, double x4, double y4);

// Determine the angle between the two segments x1,y1<-->x2,y2
// and  x2,y2<-->x3,y3
double segmentAngle(double x1, double y1, double x2, double y2,
		    double x3, double y3);

// Determine a point on a segment that, with another point, makes
// a line that is a right angle to the segment
void    perpSegIntPt(double x1, double y1, double x2, double y2,
		     double qx, double qy, double& rx, double& ry);

// Determine a point on a line that, with another point, makes
// a line that is a right angle to the given line
void   perpLineIntPt(double x1, double y1, double x2, double y2,
		     double qx, double qy, double& rx, double& ry);

// Determine a point that is a certain angle and distance from 
// another point
void    projectPoint(double ANGLE, double DIST, double cx, 
		     double cy,    double& rx,  double &ry);
XYPoint projectPoint(double ANGLE, double DIST, double cx, double cy);
XYPoint projectPoint(double ANGLE, double DIST, XYPoint pt);

XYPoint midPoint(const XYPoint& pt1, const XYPoint& pt2);

void   addVectors(double deg1, double mag1, double deg2, double mag2,
		  double& rdeg, double& rmag); 

bool   bearingMinMaxToPoly(double x, double y, const XYPolygon& poly,
			   double& bmin, double& bmax);

bool   bearingMinMaxToPolyX(double x, double y, const XYPolygon& poly,
			    double& bmin, double& bmax,
			    double& bmin_dist, double& bmax_dist);

double distCircleToLine(double cx, double cy, double radius,
			double px1, double py1, double px2, double py2);

// Below is new after 17.7

bool randPointInPoly(const XYPolygon&, double& ix, double& iy,
		     unsigned int tries=1000);

bool crossRaySegl(double rx, double ry, double ray_angle,
		  const XYSegList& segl,
		  double& ix, double& iy, double& idist);

double distPointToRay(double px, double py, double rx, double ry,
		      double ray_angle, double &ix, double& iy);

double segRayCPA(double rx, double ry, double ray_angle,
		 double x1, double y1, double x2, double y2,
		 double &ix, double& iy);

double seglRayCPA(double rx, double ry, double ray_angle,
		  const XYSegList& segl,
		  double& ix, double& iy);

double segSeglDist(double x1, double y1, double x2, double y2,
		   const XYSegList& segl,
		   double& ix, double& iy);
		   
bool crossRaySeg(double rx, double ry, double ray_angle,
		 double x1, double y1, double x2, double y2,
		 double &ix, double& iy);

int lineCircleIntPts(double x1, double y1, double x2, double y2,
		     double cx, double cy, double cradius,
		     double& ix1, double& iy1,
		     double& ix2, double& iy2);

int segCircleIntPts(double x1, double y1, double x2, double y2,
		    double cx, double cy, double cradius,
		    double& ix1, double& iy1,
		    double& ix2, double& iy2);

double distPointToLine(double px, double py, double x1, double y1,
		       double x2, double y2);

double distPointToSegl(double px, double py, const XYSegList& segl);

// Below is new after 19.8.1

double polyRayCPA(double rx, double ry, double ray_angle,
		  const XYPolygon& poly, double& ix, double& iy);

bool randPointOnPoly(double vx, double vy, const XYPolygon&,
		     double& ix, double& iy);

double polyWidth(XYPolygon, double angle=0);
double polyHeight(XYPolygon, double angle=0);

void shiftVertices(std::vector<double>& vx, std::vector<double>& vy); 

double polyAspectRatio(XYPolygon);

// Below is new after 22.8

bool setPointOnString(XYPoint& point, std::string str);
bool setSegListOnString(XYSegList& point, std::string str);

bool modSegLen(double x1, double y1, double x2, double y2,
	       double& rx1, double& ry1, double& rx2, double& ry2,
	       double new_length);

bool modSegLen(XYPoint p1, XYPoint p2, XYPoint& rp1, XYPoint& rp2,
	       double new_length);

bool modSegAng(double x1, double y1, double x2, double y2,
	       double& rx1, double& ry1, double& rx2, double& ry2,
	       double new_angle);

bool modSegAng(XYPoint p1, XYPoint p2, XYPoint& rp1, XYPoint& rp2,
	       double new_angle);

bool modSegLoc(double x1, double y1, double x2, double y2,
	       double& rx1, double& ry1, double& rx2, double& ry2,
	       double cx, double cy);

bool modSegLoc(XYPoint p1, XYPoint p2, XYPoint& rp1, XYPoint& rp2,
	       XYPoint center);

XYPoint getSegCenter(const XYPoint& p1, const XYPoint& p2); 


// Determine if two line segments intersect
bool   segmentsCross(double x1, double y1, double x2, double y2,
		     double x3, double y3, double x4, double y4,
		     double& ix, double& iy);

double   distSegToSeg(double x1, double y1, double x2, double y2,
		      double x3, double y3, double x4, double y4,
		      double& ix, double& iy);

double   distSegToPoly(double x1, double y1, double x2, double y2,
		       const XYPolygon& poly, double& ix, double& iy);

double distSeglToPoly(const XYSegList& segl, const XYPolygon& poly,
		      double& ix, double& iy);

double distSeglrToPoly(const XYSeglr& seglr, const XYPolygon& poly,
		       double& ix, double& iy, double& stemdist,
		       bool verbose=false);

double distSeglrToPoint(const XYSeglr& seglr, const XYPoint& pt,
			double& ix, double& iy, double& stemdist,
			bool verbose=false);

double distSeglToPoint(const XYSegList& segl, const XYPoint& pt,
		       double& ix, double& iy);

double distSeglToPoint(const XYSegList& segl, const XYPoint& pt,
		       double& ix, double& iy, double& stemdist);

double stemDistSeglFromPoint(const XYSegList& segl,
			     double px, double py);



// DEPRECATED INTERFACES
double distToPoint(double x1, double y1, double x2, double y2);

double distToSegment(double x1, double y1, double x2, 
		     double y2, double px, double py);

#endif


