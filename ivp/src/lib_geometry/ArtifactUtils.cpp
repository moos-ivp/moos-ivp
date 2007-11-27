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
			// Four cases:
			// 1) p0, p1 inside
			// 2) p0 inside, p1 outside
			// 3) p0 outside, p1 inside
			// 4) p0, p1 outside
			
			double distnormal0 = poly.dist_to_poly(px0, py0, ang);
			double distanti0   = poly.dist_to_poly(px0, py0, angle360(ang + 180));
			double distnormal1 = poly.dist_to_poly(px1, py1, ang);
			double distanti1   = poly.dist_to_poly(px1, py1, angle360(ang + 180));
			
			// Determine case:
			bool p0inside = poly.contains(px0, py0);
			bool p1inside = poly.contains(px1, py1);
			int situation;
			
			if (p0inside && p1inside) {situation = 1;}
			if (p0inside && !p1inside) {situation = 2;}
			if (!p0inside && p1inside) {situation = 3;}
			if (!p0inside && !p1inside) {situation = 4;}
			
			switch( situation ){
				case 1: // both inside, project 0 one way, 1 the other (segment is oriented later)
					projectPoint(ang, distnormal0, px0, py0, px0, py0);
					projectPoint(angle360(ang + 180), distanti1, px1, py1, px1, py1);
					break;
				case 2: // p1 outside, project it in, project p0 same way
					if (distnormal1 == -1) { // p1 is outside, project antinormal
						projectPoint(angle360(ang + 180), distanti1, px1, py1, px1, py1);
						projectPoint(angle360(ang + 180), distanti0, px0, py0, px0, py0);
					}
					else if (distanti1 == -1) { // p1 is outside, project normal
						projectPoint(ang, distnormal1, px1, py1, px1, py1);
						projectPoint(ang, distnormal0, px0, py0, px0, py0);
					}
					else {
						//std::cout << "generateLawnmower ERROR!! both distances are negative!";
					}						
					break;			
				case 3: // p0 outside, project it in, project p1 same way
					if (distnormal0 == -1) { // p0 is outside, project antinormal
						projectPoint(angle360(ang + 180), distanti1, px1, py1, px1, py1);
						projectPoint(angle360(ang + 180), distanti0, px0, py0, px0, py0);
					}
					else if (distanti0 == -1) { // p0 is outside, project normal
						projectPoint(ang, distnormal1, px1, py1, px1, py1);
						projectPoint(ang, distnormal0, px0, py0, px0, py0);
					}
					else {
						//std::cout << "generateLawnmower ERROR!! both distances are negative!";
					}	
					break;			
				case 4: // both points outside, project to closest segment
					if (distnormal0 > distanti0){ // Greater value is actual distance
						projectPoint(ang, distnormal0, px0, py0, px0, py0);
					}
					else {
						projectPoint(angle360(ang + 180), distanti0, px0, py0, px0, py0);
					}
					if (distnormal1 > distanti1){ // Greater value is actual distance
						projectPoint(ang, distnormal1, px1, py1, px1, py1);
					}
					else {
						projectPoint(angle360(ang + 180), distanti1, px1, py1, px1, py1);
					}
					break;
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

