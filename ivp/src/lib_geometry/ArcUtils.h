/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ArcUtils.h                                           */
/*    DATE: Nov 2nd, 2018 Friday evening on campus               */
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

#ifndef GEOM_ARC_UTILS_HEADER
#define GEOM_ARC_UTILS_HEADER

#include <vector>
#include "XYSegList.h"
#include "XYArc.h"

// Unit test: testAngleInArc
bool angleInArc(double langle, double rangle, double query_angle);

bool pointInArc(double px, double py, double ax, double ay,
		double ar, double langle, double rangle);

// Unit test: testDistPointToArc
double distPointToArc(double px, double py, double ax, double ay,
		      double ar, double langle, double rangle);

double distPointToArcPt(double px, double py, double ax, double ay,
			double ar, double langle, double rangle,
			double&ix, double& iy);

double arclen(double inscribed_angle, double radius);

void arcturn(double px, double py, double ph, double ang,
	     double radius, double& rx, double& ry);


//===============================================================
// Group 1: Whether and where arcs, segments, seglists intersect
//===============================================================

// Unit test: testArcSegCross 
bool arcSegCross(double x1, double y1, double x2, double y2,
		 double ax, double ay, double ar,
		 double langle, double rangle);

// Unit test: testArcSegCrossPts
int arcSegCrossPts(double x1, double y1, double x2, double y2,
		   double ax, double ay, double ar,
		   double langle, double rangle,
		   double& ix1, double& iy1,
		   double& ix2, double& iy2);

// Unit test: tbd
bool arcSegListCross(double ax, double ay, double ar,
		     double langle, double rangle,
		     XYSegList seglist);

// Unit test: tbd
int arcSegListCrossPts(double ax, double ay, double ar,
		       double langle, double rangle,
		       XYSegList seglist,
		       std::vector<double>& rx,
		       std::vector<double>& ry);

//===============================================================
// Group 2: Want CPA information when arc does not intersect
//===============================================================

// Unit test: testDistSegToArc
double distSegToArc(double x1, double y1, double x2, double y2,
		    double ax, double ay, double ar,
		    double langle, double rangle);

// Unit test: tbd
double distSegListToArc(double ax, double ay, double ar, 
			double langle, double rangle,
			const XYSegList& seglist);

//===============================================================
// Group 3: Other
//===============================================================

// Unit test: tbd
double distPtsOnArc(double x1, double y1, double x2, double y2,
		    double ax, double ay, double ar,
		    double langle, double rangle,
		    double rad_thresh=0.1, double ang_thresh=0.1);

// Unit test: testDistPointOnArc
double distPointOnArc(double px, double py, 
		      double ax, double ay, double ar,
		      double langle, double rangle,
		      bool langle_is_origin=true,
		      double rad_thresh=0.1, double ang_thresh=0.1);

// Unit test: testCpasRaySegl
unsigned int cpasRaySegl(double rx, double ry, double ray_angle,
			 const XYSegList& segl, double thresh,	      
			 std::vector<double>& vcpa,
			 std::vector<double>& vray);

// Unit test: tbd
unsigned int cpasArcSegl(double ax, double ay, double ar,
			 double langle, double rangle,
			 const XYSegList& segl, double thresh,
			 bool langle_is_origin,
			 std::vector<double>& vcpa,
			 std::vector<double>& vray);
#endif

