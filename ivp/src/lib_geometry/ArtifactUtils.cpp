/************************************************************/
/*    NAME: Andrew Shafer                                   */
/*    ORGN: MIT                                             */
/*    FILE: ArtifactUtils.cpp                               */
/*    DATE: Oct 24 2007                                     */
/************************************************************/

#include "ArtifactUtils.h"

#include "XYSquare.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "math.h"

// --------------------
// Procedure: generateLawnmower
//          Generates a lawnmower pattern in a polygon, given the starting point, initial angle,
//          and whether the first turn should be clockwise or counter-clockwise.
XYSegList generateLawnmower(const XYPolygon& poly, double px, double py, double ang, double radius, bool clockwise = true)
{
	XYSegList segList;

	// Check initial point, push it on
	if (poly.contains(px, py)){
		segList.add_vertex(px, py);
	}
	else return segList;
	
	// Find distance to next side, subtract equivalent radius and go add that point;
	double dDistance = poly.dist_to_poly(px, py, ang);
	double newx, newy; // used later
	double edge_dist = radius / sin(degToRadians(ang));
	dDistance -= edge_dist;
	
	while (dDistance > 0) {
		// Project new point
		projectPoint(ang, dDistance, px, py, newx, newy);
		segList.add_vertex(newx, newy);
		
		// Determine which way to turn
		
	}
}
