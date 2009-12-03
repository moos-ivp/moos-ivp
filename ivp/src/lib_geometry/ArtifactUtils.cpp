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

#include <sstream>
#include "XYSquare.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "MBUtils.h"
#include "math.h"
#include <stdlib.h>

using namespace std;

// --------------------
// Procedure: generateLawnmower
//     Generates a lawnmower pattern in a polygon, given the starting 
//     point, initial angle (degrees), and whether the first turn 
//     should be clockwise or counter-clockwise.

/// generateLawnmower() produces a lawnmower pattern in a polygon 
/// from an initial point
/// \param poly Polygon string that defines the boundaries
/// \param px0 Initial x to start from
/// \param py0 Initial y to start from
/// \param ang Angle to orient the lawnmower pattern.  [0-360], 0 is North, clockwise is positive
/// \param radius 1/2 the distance between adjacent swaths
/// \param clockwise Determines the direction of the first turn, defaults to true
const XYSegList generateLawnmower(const XYPolygon& poly, double px0, 
				  double py0, double ang, 
				  double radius, bool clockwise = true)
{
  // General algorithm:
  // 1) Create initial segment
  // 2) Create parallel segment 2*radius away
  //    Check to see if segment intercepts, extend/contract segment to fit inside
  //    If segment doesn't intercept, quit
  
  XYSegList segList;
  
  //cout << "Trying to add initial point\n";
  //cout << "Point is: " + doubleToString(px0) + ", " + doubleToString(py0) + "\n";
  
  // Check initial point, push it on
  if (poly.contains(px0, py0)){
    segList.add_vertex(px0, py0);
  }
  else return segList; // Return empty seglist
  
  //cout << "Below the initial check\n";
  
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
  
  //cout << "Returning a seglist: " + segList.get_spec() << endl;
  
  return segList;
  
}

/// generateLawnmowerFull() produces a full lawnmower pattern in a polygon
/// It does so by creating two half lawnmowers starting at the center of the polygon and
/// then joining them together.
/// \param poly Polygon string that defines the boundaries
/// \param ang Angle to orient the lawnmower pattern.  [0-360], 0 is North, clockwise is positive
/// \param radius 1/2 the distance between adjacent swaths
const XYSegList generateLawnmowerFull(const XYPolygon& poly, double ang, double radius){
  // Get the center of the polygon
  double x0 = poly.get_center_x();
  double y0 = poly.get_center_y();
  
  // Form the alpha and beta lawnmower patterns
  // beta is the lower half (with clockwise = true)
  XYSegList beta = generateLawnmower(poly, x0, y0, ang, radius, true);
  
  // alpha is the upper half (with clockwise also = true)
  XYSegList alpha = generateLawnmower(poly, x0, y0, angle360(ang + 180), radius, true);
  
  // Remove initial point from both patterns
  beta.delete_vertex(x0, y0);
  alpha.delete_vertex(x0, y0);
  
  // Reverse the alpha pattern and put the beta pattern after it
  alpha.reverse();
  
  for(unsigned int i=0; i<beta.size(); i++) {
    alpha.add_vertex(beta.get_vx(i), beta.get_vy(i));
  }
  
  return alpha;
}


/// generateArtifacts() produces a random vector of unique artifacts
/// \param step Discrete grid to snap artifacts to
/// \param num_artifacts Number of artifacts to generate
/// \param poly An XYPolygon to check to see if the points are within the bounds
const vector<string> generateArtifacts(double min_x, double max_x, 
				       double min_y, double max_y, 
				       double step, double num_artifacts, 
				       const XYPolygon& poly)
{
  vector<string> vArtifacts;
  
  srand(time(NULL));
  
  while(vArtifacts.size() < num_artifacts){
    // RAND_MAX is defined by the math headers, refers to the maximum random value
    double rand_x = snapToStep(min_x + rand() * (max_x - min_x) / RAND_MAX, step);
    double rand_y = snapToStep(min_y + rand() * (max_y - min_y) / RAND_MAX, step);
    
    // This portion should be fixed to not use the << operator for doubles
    // e.g., use Mike's dstringCompact(doubleToString(rand_x)) and get rid of
    // the string stream.
    ostringstream osArtifact;
    osArtifact << "X=" << rand_x << "," << "Y=" << rand_y;
    string sArtifact = osArtifact.str();
    
    if(poly.contains(rand_x, rand_y) && !vectorContains(vArtifacts, sArtifact)){ // New artifact to add
      vArtifacts.push_back(sArtifact);
    };
  };
  
  return(vArtifacts);
}

