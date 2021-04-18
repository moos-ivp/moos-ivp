/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AngleUtils.h                                         */
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
 
#ifndef ANGLEUTILS_HEADER
#define ANGLEUTILS_HEADER

#include <list>
#include "XYPoint.h"
#include "XYPolygon.h"

double  angleFromThreePoints(double x1, double y1, double x2, double y2,
			     double x3, double y3);
bool    threePointTurnLeft(double x0, double y0, double x1, double y1,
			   double x2, double y2);
double  threePointXProduct(double x0, double y0, double x1, double y1,
			   double x2, double y2);

double  relAng(double xa, double ya, double xb, double yb);
double  relAng(const XYPoint& a, const XYPoint& b);
double  radAngleWrap(double radians);
double  degToRadians(double degrees);
double  radToDegrees(double radians);
double  angle180(double degrees);
double  angle360(double degrees);
double  angleDiff(double, double);
double  aspectDiff(double, double);

double  angleMinAcute(double, double);
double  angleMaxAcute(double, double);
double  angleMinReflex(double, double);
double  angleMaxReflex(double, double);

double  angleMinContains(double, double, double);
double  angleMaxContains(double, double, double);
double  angleMinExcludes(double, double, double);
double  angleMaxExcludes(double, double, double);

// true heading/E-N conversion added by henrik
double  radToHeading(double radians);
double  headingToRadians(double degrees);

double  speedInHeading(double osh, double osv, double heading);

double  relBearing(double osx, double osy, double osh, double cnx, double cny);
double  absRelBearing(double osx, double osy, double osh, double cnx, double cny);
double  totAbsRelBearing(double osx, double osy, double osh, 
			 double cnx, double cny, double cnh);

bool    containsAngle(double deg1, double deg2, double deg3);

bool    polyAft(double osx, double osy, double osh, XYPolygon poly, double xbng=0);

double  turnGap(double osx, double osy, double osh, double tradius,
		double px1, double py1, double px2, double py2, bool tright);

double  headingAvg(std::list<double>);
double  headingAvg(double, double);


#endif

