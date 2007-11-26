/*****************************************************************/
/*    NAME: Andrew Shafer                                        */
/*    ORGN: MIT                                                  */
/*    FILE: ArtifactUtils.h                                      */
/*    DATE: Oct 24 2007                                          */
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

#include "ArtifactUtils.h"

#include "XYSquare.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "math.h"

// --------------------
// Procedure: generateLawnmower
//          Generates a lawnmower pattern in a polygon, given the starting point, initial angle (degrees),
//          and whether the first turn should be clockwise or counter-clockwise.
XYSegList generateLawnmower(const XYPolygon& poly, double px0, double py0, double ang, double radius, bool clockwise = true)
{
	// General algorithm:
	// 1) Create initial segment
	// 2) Create parallel segment 2*radius away
	//    Check to see if segment intercepts, extend/contract segment to fit inside
	//    If segment doesn't intercept, quit
	
	XYSegList segList;

	// Check initial point, push it on
	if (poly.contains(px0, py0)){
		segList.add_vertex(px0, py0);
	}
	else return segList; // Return empty seglist
	
	// Create initial segment
	double dDistance = poly.dist_to_poly(px0, py0, ang);
	double px1, py1;
	projectPoint(ang, dDistance, px0, py0, px1, py1);
	segList.add_vertex(px1, py1);
	
	// Initial segment is now px0, py0, px1, py1
	
	// Determine the values for x_shift and y_shift
	double x_shift, y_shift;
	double rpx, rpy;
	projectPoint(ang + (clockwise ? 90 : -90), 2*radius, px0, py0, rpx, rpy);
	x_shift = rpx - px0;
	y_shift = rpy - py0;
	
		// we will be reusing px0, py0, px1, py1 to represent the next two points to add
	bool carryon = true;
	while (carryon) {
		// Shift current segment
		XYSegList segTemp;
		segTemp.add_vertex(px0, py0);
		segTemp.add_vertex(px1, py1);
		segTemp.shift_horz(x_shift);
		segTemp.shift_vert(y_shift);
		
		px0 = segTemp.get_vx(0);
		py0 = segTemp.get_vy(0);
		px1 = segTemp.get_vx(1);
		py1 = segTemp.get_vy(1);
				
		// Check to see if segment intercepts polygon
		if (poly.seg_intercepts(px0, py0, px1, py1)){
			// Two cases:
			// 1) Point is inside polygon -- extend to closest edge along angle, angle+180
			// 2) Point is outside polygon -- only one distance is positive
			double distnormal = poly.dist_to_poly(px0, py0, ang);
			double distanti   = poly.dist_to_poly(px0, py0, angle360(ang + 180));
			if ((distnormal == -1) || (distanti == -1)) { // Point is outside polygon
				if (distnormal > distanti){ // Greater value is actual distance
					projectPoint(ang, distnormal, px0, py0, px0, py0);
				}
				else {
					projectPoint(angle360(ang + 180), distanti, px0, py0, px0, py0);
				}
			}
			else { // Point is inside polygon
				if (distnormal < distanti) { // Lesser value is actual distance
					projectPoint(ang, distnormal, px0, py0, px0, py0);
				}
				else {
					projectPoint(angle360(ang + 180), distanti, px0, py0, px0, py0);
				}
			}
			
			// Figure out which point to add next (closest to tail of segList), then add other
			if (distPointToPoint(px0, py0, segList.get_vx(segList.size()-1), segList.get_vy(segList.size()-1)) < 
				distPointToPoint(px1, py1, segList.get_vx(segList.size()-1), segList.get_vy(segList.size()-1))) {
					segList.add_vertex(px0, py0);
					segList.add_vertex(px1, py1);
			}
			else {
				segList.add_vertex(px1, py1);
				segList.add_vertex(px0, py0);
			}
		}
		else { // If the segment doesn't intersect, get out of here
			carryon = false;
		}
	}

}

