/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ArcUtils.cpp                                         */
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

#include <iostream>
#include <cmath>
#include "ArcUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

//---------------------------------------------------------------
// Procedure: angleInArc()
//   Purpose: Determine if the given query angle(heading) lies 
//            between the two other given angles. The range goes
//            clockwise from langle to rangle.

bool angleInArc(double langle, double rangle, double query_angle)
{
  // Pre-Check: Make sure all angles in range [0,360)
  if((langle < 0) || (langle >= 360))
    langle = angle360(langle);
  if((rangle < 0) || (rangle >= 360))
    rangle = angle360(rangle);
  if((query_angle < 0) || (query_angle >= 360))
    query_angle = angle360(query_angle);

  // Secial Case: left equals right angle
  if(langle == rangle)
    return(langle == query_angle);

  // Normal case 1: e.g., 45-180 contains 160
  if(langle < rangle)
    return((query_angle >= langle) && (query_angle <= rangle));

  // Normal case 2: e.g., 350-10 contains 5
  return((query_angle >= langle) || (query_angle <= rangle));
}

//---------------------------------------------------------------
// Procedure: pointInArc()
//   Purpose: For a given point determine if the angle from the center
//            of the arc to the point is an angle in [langle,rangle].

bool pointInArc(double px, double py, double ax, double ay,
		double ar, double langle, double rangle)
{
  // Pre-Check: Make sure all angles in range [0,360)
  if((langle < 0) || (langle >= 360))
    langle = angle360(langle);
  if((rangle < 0) || (rangle >= 360))
    rangle = angle360(rangle);

  double relang = relAng(ax,ay,px,py);

  bool ang_inarc = angleInArc(langle, rangle, relang);
  return(ang_inarc);
}

//---------------------------------------------------------------
// Procedure: distPointToArc()
//   Purpose: Determine the range from the given point to the 
//            closest point on the given arc.


double distPointToArc(double px, double py, double ax, double ay,
		      double ar, double langle, double rangle)
{
  // Pre-Check: Make sure all angles in range [0,360)
  if((langle < 0) || (langle >= 360))
    langle = angle360(langle);
  if((rangle < 0) || (rangle >= 360))
    rangle = angle360(rangle);
  
  if(pointInArc(px,py, ax,ay,ar, langle,rangle)) {
    double dist_pt_to_ctr = hypot(px-ax, py-ay);
    if(dist_pt_to_ctr < ar)
      return(ar - dist_pt_to_ctr);
    else
      return(dist_pt_to_ctr - ar);
  }

  double lftx, lfty;
  projectPoint(langle, ar, ax, ay, lftx, lfty);
  double dist_pt_to_lft = hypot(px-lftx, py-lfty);

  double rgtx, rgty;
  projectPoint(rangle, ar, ax, ay, rgtx, rgty);
  double dist_pt_to_rgt = hypot(px-rgtx, py-rgty);

  if(dist_pt_to_lft < dist_pt_to_rgt)
    return(dist_pt_to_lft);

  return(dist_pt_to_rgt);
}

//---------------------------------------------------------------
// Procedure: distPointToArcPt()
//   Purpose: Determine the range from the given point to the 
//            closest point on the given arc.
//      Note: Also return the point on arc that is closest to
//            the given point


double distPointToArcPt(double px, double py, double ax, double ay,
			double ar, double langle, double rangle,
			double& ix, double& iy)
{
  // Pre-Check: Make sure all angles in range [0,360)
  if((langle < 0) || (langle >= 360))
    langle = angle360(langle);
  if((rangle < 0) || (rangle >= 360))
    rangle = angle360(rangle);

  double dist_pt_to_ctr = hypot(px-ax, py-ay);

  // Handle Special case where point it on the arc center
  if(dist_pt_to_ctr == 0) {
    projectPoint(langle, ar, ax, ay, ix, iy);
    return(0);
  }
  
  double relang = relAng(ax,ay,px,py);
  bool ang_inarc = angleInArc(langle, rangle, relang);

  if(ang_inarc) {
    projectPoint(relang, ar, ax, ay, ix, iy);
    if(dist_pt_to_ctr < ar) {
      return(ar - dist_pt_to_ctr);
    }
    else
      return(dist_pt_to_ctr - ar);
  }

  double lftx, lfty;
  projectPoint(langle, ar, ax, ay, lftx, lfty);
  double dist_pt_to_lft = hypot(px-lftx, py-lfty);

  double rgtx, rgty;
  projectPoint(rangle, ar, ax, ay, rgtx, rgty);
  double dist_pt_to_rgt = hypot(px-rgtx, py-rgty);

  if(dist_pt_to_lft < dist_pt_to_rgt) {
    projectPoint(langle, ar, ax, ay, ix, iy);
    return(dist_pt_to_lft);
  }

  projectPoint(rangle, ar, ax, ay, ix, iy);
  return(dist_pt_to_rgt);
}

//---------------------------------------------------------------
// Procedure: arclen()
//   Purpose: Determine the length of arc given by its inscribed
//            angle and its radius.

double arclen(double angle, double radius)
{
  angle = angle360(angle);
  double pct = angle / 360;
  double circumference = radius * radius * 3.1415926536;
  return(pct * circumference);
}

//---------------------------------------------------------------
// Procedure: arcturn()
//   Purpose: Determine the end position for vehicle beginning a
//            turn arc at a given position and heading. And given
//            an inscribed angle and radius.
//      Note: A positive inscribed angle indicates a righthand or
//            starboard turn. A negative angle is left/port turn.

void arcturn(double px, double py, double ph, double ang,
	     double radius, double& rx, double& ry)
{
  // Sanity checks
  if((ang == 0) || (radius <= 0)) {
    rx = px;
    ry = py;
    return;
  }
     
  double offset   = 0;
  double dest_ang = 0;
  if(ang > 0) {
    offset = angle360(ang+90);
    dest_ang = angle360(offset + 180 + ang);
  }
  else {
    offset = angle360(ang-90);
    dest_ang = angle360(offset + 180 - ang);
  }
  
  double cx,cy;
  projectPoint(offset, radius, px,py, cx,cy);

  projectPoint(dest_ang, radius, cx,cy, rx,ry);
}

//---------------------------------------------------------------
// Procedure: arcSegCross()
//   Purpose: Determine if the given segment crosses the given arc

bool arcSegCross(double x1, double y1, double x2, double y2,
		 double ax, double ay, double ar,
		 double langle, double rangle)
{
  // Sanity checks / Special cases
  if(ar <= 0)
    return(false);

  // Three general cases based on the segment end-points
  //    (1) both end-points are less than the arc radius
  //    (2) both end-points are greater than the arc radius
  //        This has two sub-cases
  //    (3) One end-point is less and one greater than arc-radius
  //        This has three-sub-cases
  double dist1 = hypot(x1-ax, y1-ay);
  double dist2 = hypot(x2-ax, y2-ay);
  //cout << "dist1: " << dist1 << endl;
  //cout << "dist2: " << dist2 << endl;
  
  // Case 1: If max distance from ax,ay to the seg is < ar, done  
  if((dist1 < ar) && (dist2 < ar))
    return(false);

  // Case 2: If min distance from ax,ay to the seg is > ar, done  
  if((dist1 > ar) && (dist2 > ar)) {
    double ix, iy;
    double min_dist = distPointToSeg(x1,y1,x2,y2, ax,ay, ix,iy);
    // Case 2A: The easy case
    if(min_dist > ar)
      return(false);

    // Case 2B: The harder case. We know the perp_int_pt does indeed
    // make a perpendicular line, with the center point, to the line
    // segment. And we know this point is in the radius. We just need
    // to find the two angles to the two points where the line segment
    // intersects the arc. If either angle is in the arc, return true
    double dist_to_perp_int_pt = hypot(ax-ix, ay-iy);
    double ang_to_perp_int_pt = relAng(ax,ay, ix,iy);
    double delta_ang = acos(dist_to_perp_int_pt / ar) / M_PI * 180;
    double ang1 = angle360(ang_to_perp_int_pt - delta_ang);
    if(angleInArc(langle, rangle, ang1))
      return(true);
    double ang2 = angle360(ang_to_perp_int_pt + delta_ang);
    if(angleInArc(langle, rangle, ang2))
      return(true);
  }
    
  // From here on we'll find it useful to know the angle from the
  // arc center to each of the two segment end points. Also whether
  // the segment endpoints are contained in the arc angle.
  double ang1 = relAng(ax, ay, x1, y1);
  double ang2 = relAng(ax, ay, x2, y2);
  bool inarc1 = angleInArc(langle, rangle, ang1);
  bool inarc2 = angleInArc(langle, rangle, ang2);

  // Case 3A: If segment vertex happens to be exactly the
  // arc range, this is worth checking right here. If the vertex is
  // in-arc return true. Otherwise cannot know for sure if false.
  if(dist1 == ar) { 
    if(inarc1)
      return(true);
  }
  if(dist2 == ar) { 
    if(inarc2)
      return(true);
  }


  // Cases 3 B-D: One vertex has a range to the arc center less
  // than the radius, and one greater than the radius.

  // Case 3B: If both points in-arc return
  if(inarc1 && inarc2)
    return(true);

  // Case 3C: One or both seg end points are in the arc angles.
  // We do know that the segment crosses the circle somewhere.
  // We find that cross point and see if in the arc angles.

  double rx1, ry1, rx2, ry2;
  int cross_pts = segCircleIntPts(x1,y1,x2,y2,ax,ay,ar,
				  rx1,ry1,rx2,ry2);

  //cout << "  cross_pts: " << cross_pts << endl;
  //cout << "  rx1: " << rx1 << endl;
  //cout << "  ry1: " << ry1 << endl;

  if(cross_pts == 1) {
    double ang_to_cross_pt = relAng(ax,ay,rx1,ry1);
    return(angleInArc(langle, rangle, ang_to_cross_pt));
  }  

  return(false);
}

//---------------------------------------------------------------
// Procedure: arcSegCrossPts()
//   Purpose: Determine if the given segment crosses the given arc

int arcSegCrossPts(double x1, double y1, double x2, double y2,
		   double ax, double ay, double ar,
		   double langle, double rangle,
		   double& ix1, double& iy1,
		   double& ix2, double& iy2)
{
  ix1 = 0;
  iy1 = 0;
  ix2 = 0;
  iy2 = 0;
  // Sanity checks / Special cases
  if(ar <= 0)
    return(0);

  // Three general cases based on the segment end-points
  //    (1) both end-points are less than the arc radius
  //    (2) both end-points are greater than the arc radius
  //        This has two sub-cases
  //    (3) One end-point is less and one greater than arc-radius
  //        This has three-sub-cases
  double dist1 = hypot(x1-ax, y1-ay);
  double dist2 = hypot(x2-ax, y2-ay);
  
  // Case 1: If max distance from ax,ay to the seg is < ar, done  
  if((dist1 < ar) && (dist2 < ar))
    return(0);

  // Case 2: If min distance from ax,ay to the seg is > ar, done  
  if((dist1 > ar) && (dist2 > ar)) {
    double ix, iy;
    double min_dist = distPointToSeg(x1,y1,x2,y2, ax,ay, ix,iy);
    if(min_dist > ar)
      return(0);
  }
  // Cases 3: 
  int cross_pts = segCircleIntPts(x1,y1,x2,y2, ax,ay,ar, ix1,iy1,ix2,iy2);
  if(cross_pts == 0)
    return(0);

  if(cross_pts == 1) {
    double ang_to_cross_pt1 = relAng(ax,ay,ix1,iy1);
    bool inarc1 = angleInArc(langle, rangle, ang_to_cross_pt1);
    if(inarc1)
      return(1);
    else {
      ix1 = 0;
      iy1 = 0;
      return(0);
    }
  }

  if(cross_pts == 2) {
    double ang_to_cross_pt1 = relAng(ax,ay,ix1,iy1);
    double ang_to_cross_pt2 = relAng(ax,ay,ix2,iy2);
    bool inarc1 = angleInArc(langle, rangle, ang_to_cross_pt1);
    bool inarc2 = angleInArc(langle, rangle, ang_to_cross_pt2);
    if(inarc1 && inarc2)
      return(2);
    else if(inarc1 && !inarc2) {
      ix2 = 0;
      iy2 = 0;
      return(1);
    }
    else if(!inarc1 && inarc2) {
      ix1 = ix2;
      iy1 = iy2;
      ix2 = 0;
      iy2 = 0;
      return(1);
    }
  }
  ix1 = 0;
  iy1 = 0;
  ix2 = 0;
  iy2 = 0;
  return(0);
}

//---------------------------------------------------------------
// Procedure: arcSegListCross()
//   Purpose: Determine if the given Segment the given arc

bool arcSegListCross(double ax, double ay, double ar,
		     double langle, double rangle,
		     XYSegList segl)
{
  // Special case 1:
  if(segl.size() == 0)
    return(false);

  // Special case 2:
  if(segl.size() == 1) {
    double px = segl.get_vx(0);
    double py = segl.get_vy(0);
    if(distPointToArc(px,py, ax,ay,ar, langle,rangle) == 0)
      return(true);
    return(false);
  }
 
  // General case:
  for(unsigned int i=0; i<segl.size()-1; i++) {
    double x1 = segl.get_vx(i);
    double y1 = segl.get_vy(i);
    double x2 = segl.get_vx(i+1);
    double y2 = segl.get_vy(i+1);
    if(arcSegCross(x1,y1,x2,y2, ax,ay,ar, langle,rangle))
      return(true);
  }
  return(false);
}

//---------------------------------------------------------------
// Procedure: arcSegListCrossPts()
//   Purpose: Determine if the given seglist crosses the given arc.
//   Returns: Number of cross points - unbounded since the seglist
//            is unbounded.
//            Als returns the cross points.

int arcSegListCrossPts(double ax, double ay, double ar,
		       double langle, double rangle,
		       XYSegList segl,
		       vector<double>& vpx, vector<double>& vpy)
{
  // Disregard anything that may have existed in the return vectors
  vpx.clear();
  vpy.clear();
  
  // Special case 1:
  if(segl.size() == 0)
    return(0);

  // Special case 2:
  if(segl.size() == 1) {
    double px = segl.get_vx(0);
    double py = segl.get_vy(0);
    if(distPointToArc(px,py, ax,ay,ar, langle,rangle) == 0) {
      vpx.push_back(px);
      vpy.push_back(py);
      return(1);
    }
    return(0);
  }
 
  // General case:
  int total_xpts = 0;
  for(unsigned int i=0; i<segl.size()-1; i++) {
    double x1 = segl.get_vx(i);
    double y1 = segl.get_vy(i);
    double x2 = segl.get_vx(i+1);
    double y2 = segl.get_vy(i+1);

    double ix1,iy1,ix2,iy2;
    int xpts = arcSegCrossPts(x1,y1,x2,y2, ax,ay,ar, langle,rangle,
			      ix1,iy1,ix2,iy2);
    if(xpts > 0) {
      vpx.push_back(ix1);
      vpy.push_back(iy1);
    }
    if(xpts > 1) {
      vpx.push_back(ix2);
      vpy.push_back(iy2);
    }
    total_xpts += xpts;
  }
  return(total_xpts);
}

//===============================================================
// Group 2: Want CPA information when arc does not intersect
//===============================================================

//---------------------------------------------------------------
// Procedure: distSegToArx
//   Purpose: Determine shortest distance from the given segment
//            to the given arc.

double distSegToArc(double x1, double y1, double x2, double y2,
		    double ax, double ay, double ar,
		    double langle, double rangle)
{
  if(arcSegCross(x1,y1,x2,y2, ax,ay,ar, langle,rangle))
    return(0);

  // Part 0: Handle a really simple case: arc is a point
  if(langle == rangle) {
    double px, py;
    projectPoint(langle, ar, ax, ay, px, py);
    return(distPointToSeg(x1,y1, x2,y2, px,py));
  }
  
  double dist_to_ctr1 = hypot(x1-ax, y1-ay);
  double dist_to_ctr2 = hypot(x2-ax, y2-ay);

  // Part 1: Handle a really simple case: the seg is completely
  //         in the arc circle.
  if((dist_to_ctr1 <= ar) && (dist_to_ctr2 <= ar)) {
    double dist_to_arc1 = distPointToArc(x1,y1, ax,ay,ar,
					 langle,rangle);
    double dist_to_arc2 = distPointToArc(x2,y2, ax,ay,ar,
					 langle,rangle);
    if(dist_to_arc1 < dist_to_arc2)
      return(dist_to_arc1);
    return(dist_to_arc2);
  }

  // For all other cases, the min distance will be the minimum of
  // four values:
  // (1) arc vertex 1 to the segment
  // (2) arc vertex 2 to the segment
  // (3) seg vertex 1 to the arc
  // (4) seg vertex 2 to the arc
  // Except when the min dist is from the perpendicular intersect pt
  // made between the line and the arc. Once we rule that out we
  // can simply calculate and take the min of the other four values.

  // The Exception case can only happen if both seg vertices are at
  // a range to the arc center, greater than the arc range. Since we
  // have this handy, check here:

  if((dist_to_ctr1 > ar) && (dist_to_ctr2 > ar)) {
  
    // Find the perpendicular line intersect point and 
    // determine if it lies on segment
    double ix,iy;
    perpLineIntPt(x1,y1,x2,y2, ax,ay, ix,iy);
    
    double xmin = x1;
    double xmax = x2;
    if(x1 > x2) {
      xmin = x2;
      xmax = x1;
    }    
    double ymin = y1;
    double ymax = y2;
    if(y1 > y2) {
      ymin = y2;
      ymax = y1;
    }
    
    // We are confident that the perLineIntPt generates a point on
    // the given line. So if the line is ~vertical or ~horizontal,
    // we do not check the x value in the case of vertical or the
    // y value in the case of horizontal.   
    bool plipt_in_seg = true;
    if((ymax-ymin) > 0.000001) {
      if((iy < ymin) || (iy > ymax))
	plipt_in_seg = false;
    }
    if((xmax-xmin) > 0.000001) {
      if((ix < xmin) || (ix > xmax))
	plipt_in_seg = false;
    }
    
    double plipt_rng = hypot(ix-ax, iy-ay);
    
    if(plipt_in_seg && (plipt_rng >= ar)) {
      double plipt_ang = relAng(ax,ay,ix,iy);
      bool plipt_in_arc = angleInArc(langle,rangle,plipt_ang);
      if(plipt_in_arc) {
	double min_dist_to_arc = (plipt_rng - ar);
	return(min_dist_to_arc);
      }
    }
  }

  // Now that the exception case has been ruled out, calculate the
  // min distance from the four points:
  double ax1, ay1;
  projectPoint(langle,ar,ax,ay, ax1,ay1);
  double ax2, ay2;
  projectPoint(rangle,ar,ax,ay, ax2,ay2);

  double dist_a1_to_seg = distPointToSeg(x1,y1,x2,y2, ax1,ay1);
  double dist_a2_to_seg = distPointToSeg(x1,y1,x2,y2, ax2,ay2);
  double dist_s1_to_arc = distPointToArc(x1,y1, ax,ay,ar, langle,rangle);
  double dist_s2_to_arc = distPointToArc(x2,y2, ax,ay,ar, langle,rangle);

  double min_dist = dist_a1_to_seg;
  if(dist_a2_to_seg < min_dist)
    min_dist = dist_a2_to_seg;
  if(dist_s1_to_arc < min_dist)
    min_dist = dist_s1_to_arc;
  if(dist_s2_to_arc < min_dist)
    min_dist = dist_s2_to_arc;

  return(min_dist);
}

//---------------------------------------------------------------
// Procedure: distSegToArcPts
//   Purpose: Determine shortest distance from the given segment
//            to the given arc.
//            Also return the point(s) where the minimu

double distSegToArcPts(double x1, double y1, double x2, double y2,
		       double ax, double ay, double ar,
		       double langle, double rangle, double thresh,
		       vector<double>& vix,
		       vector<double>& viy,
		       vector<double>& vdist)
{
  double ix1,iy1,ix2,iy2;
  int xpts = arcSegCrossPts(x1,y1,x2,y2, ax,ay,ar, langle,rangle,
			    ix1,iy1,ix2,iy2);

  if((xpts < 0) || (xpts > 2))
    return(-1);
  
  // Part A: Two pts intersect, report both
  if(xpts == 2) {
    vix.push_back(ix1);
    viy.push_back(iy1);
    vdist.push_back(0);
    vix.push_back(ix2);
    viy.push_back(iy2);
    vdist.push_back(0);
    return(0);
  }    

  // Part B: One pt intersect, report just the one
  if(xpts == 1) {
    vix.push_back(ix1);
    viy.push_back(iy1);
    vdist.push_back(0);
    return(0);
  }
  
  
  // Part C: No points intersect, but we may want to report points on
  // the arc that have distances to the segment below the threshold.

  // Part C1: check the case where both vertices of the line segment
  //          are beyond the arc range. In this case we just want to
  //          check if the perpendicular intersection point is in the
  //          arc range. If so, we just return the perp_int_pt.
  double dist_to_ctr1 = hypot(x1-ax, y1-ay);
  double dist_to_ctr2 = hypot(x2-ax, y2-ay);

  if((dist_to_ctr1 > ar) && (dist_to_ctr2 > ar)) {
  
    // Find the perpendicular line intersect point and 
    // determine if it lies on segment
    double ix,iy;
    perpLineIntPt(x1,y1,x2,y2, ax,ay, ix,iy);
    
    double xmin = x1;
    double xmax = x2;
    if(x1 > x2) {
      xmin = x2;
      xmax = x1;
    }    
    double ymin = y1;
    double ymax = y2;
    if(y1 > y2) {
      ymin = y2;
      ymax = y1;
    }
    
    // We are confident that the perLineIntPt generates a point on
    // the given line. So if the line is ~vertical or ~horizontal,
    // we do not check the x value in the case of vertical or the
    // y value in the case of horizontal.   
    bool plipt_in_seg = true;
    if((ymax-ymin) > 0.000001) {
      if((iy < ymin) || (iy > ymax))
	plipt_in_seg = false;
    }
    if((xmax-xmin) > 0.000001) {
      if((ix < xmin) || (ix > xmax))
	plipt_in_seg = false;
    }
    
    double plipt_rng = hypot(ix-ax, iy-ay);
    
    if(plipt_in_seg && (plipt_rng >= ar)) {
      double plipt_ang = relAng(ax,ay,ix,iy);
      bool plipt_in_arc = angleInArc(langle,rangle,plipt_ang);
      if(plipt_in_arc) {
	double min_dist_to_arc = (plipt_rng - ar);
	vix.push_back(ix);
	viy.push_back(iy);
	vdist.push_back(min_dist_to_arc);
	return(min_dist_to_arc);
      }
    }
  }

  // Part C2: Other


  double ax1, ay1;
  projectPoint(langle,ar,ax,ay, ax1,ay1);
  double ax2, ay2;
  projectPoint(rangle,ar,ax,ay, ax2,ay2);
  double dist_a1_to_seg = distPointToSeg(x1,y1,x2,y2, ax1,ay1);
  double dist_a2_to_seg = distPointToSeg(x1,y1,x2,y2, ax2,ay2);
  double dist_s1_to_arc = distPointToArc(x1,y1, ax,ay,ar, langle,rangle);
  double dist_s2_to_arc = distPointToArc(x2,y2, ax,ay,ar, langle,rangle);

  double min_dist = dist_a1_to_seg;
  if(dist_a2_to_seg < min_dist)
    min_dist = dist_a2_to_seg;
  if(dist_s1_to_arc < min_dist)
    min_dist = dist_s1_to_arc;
  if(dist_s2_to_arc < min_dist)
    min_dist = dist_s2_to_arc;

  if(min_dist > thresh)
    return(min_dist);

  if(dist_a1_to_seg <= thresh) {
    vix.push_back(ax1);
    viy.push_back(ay1);
    vdist.push_back(dist_a1_to_seg);
  }
  if(dist_a1_to_seg <= thresh) {
    vix.push_back(ax1);
    viy.push_back(ay1);
    vdist.push_back(dist_a1_to_seg);
  }
  
  if(xpts == 0) {
    return(distSegToArc(x1,y1,x2,y2, ax,ay,ar, langle,rangle));
  }

  return(0);
}



//---------------------------------------------------------------
// Procedure: distSegListToArc
//   Purpose: Determine shortest distance from the given seglist
//            to the given arc.

double distSegListToArc(double ax, double ay, double ar, 
			double langle, double rangle,
			const XYSegList& segl)
{
  // Special case 1:
  if(segl.size() == 0)
    return(-1);

  // Special case 2:
  if(segl.size() == 1) {
    double px = segl.get_vx(0);
    double py = segl.get_vy(0);
    return(distPointToArc(px,py, ax,ay,ar, langle,rangle));
  }
 
  // General case:
  double min_dist = -1;
  for(unsigned int i=0; i<segl.size()-1; i++) {
    double x1 = segl.get_vx(i);
    double y1 = segl.get_vy(i);
    double x2 = segl.get_vx(i+1);
    double y2 = segl.get_vy(i+1);

    double dist = distSegToArc(x1,y1,x2,y2, ax,ay,ar, langle,rangle);
    if((min_dist < 0) || (dist < min_dist))
      min_dist = dist;
  }
  return(min_dist);
}

//---------------------------------------------------------------
// Procedure: distPtsOnArc
//   Purpose: Given two points presumed to reside on the given arc
//            calculate the distance along the arc
//      Note: both points will be checked to ensure that they are
//            on the arc. The radial threshold and angular threshold
//            are tolerances that can be set by the caller but have
//            a default value set in the header file.

double distPtsOnArc(double x1, double y1, double x2, double y2,
		    double ax, double ay, double ar, 
		    double langle, double rangle,
		    double rad_thresh, double ang_thresh)

{
  // Part 1: Ensure the two points are on the arc w.r.t. arc radius
  if(rad_thresh < 0)
    rad_thresh = 0;
  double pt1_range = hypot(x1-ax, y1-ay);
  if((pt1_range < (ar-rad_thresh)) || (pt1_range > (ar+rad_thresh)))
    return(-1);
  double pt2_range = hypot(x2-ax, y2-ay);
  if((pt2_range < (ar-rad_thresh)) || (pt2_range > (ar+rad_thresh)))
     return(-1);

  // Part 2: Ensure the two points are on the arc w.r.t. arc angle
  if(ang_thresh < 0)
    ang_thresh = 0;
  double pt1_ang = relAng(ax,ay, x1,y1);
  if(!angleInArc(langle-ang_thresh, rangle+ang_thresh, pt1_ang))
    return(-1);
  double pt2_ang = relAng(ax,ay, x2,y2);
  if(!angleInArc(langle-ang_thresh, rangle+ang_thresh, pt2_ang))
    return(-1);
  
  double angle_diff = angle180(pt2_ang) - angle180(pt1_ang);

  double circumference = 3.1451926535897 * 2 * ar;
  double dist = (angle_diff / 360) * circumference;
    
  return(dist);
}


//---------------------------------------------------------------
// Procedure: distPointOnArc
//   Purpose: Given a point presumed to reside on the given arc
//            calculate the distance along the arc
//      Note: The point will be checked to ensure that it is
//            on the arc. The radial threshold and angular threshold
//            are tolerances that can be set by the caller but have
//            a default value set in the header file.

double distPointOnArc(double px, double py, 
		      double ax, double ay, double ar, 
		      double langle, double rangle,
		      bool langle_is_origin,
		      double rad_thresh, double ang_thresh)

{
  // Part 1: Ensure the two points are on the arc w.r.t. arc radius
  if(rad_thresh < 0)
    rad_thresh = 0;
  double pt_range = hypot(px-ax, py-ay);
  if((pt_range < (ar-rad_thresh)) || (pt_range > (ar+rad_thresh)))
    return(-1);
  
  // Part 2: Ensure the two points are on the arc w.r.t. arc angle
  if(ang_thresh < 0)
    ang_thresh = 0;
  double pt_ang = relAng(ax,ay, px,py);
  if(!angleInArc(langle-ang_thresh, rangle+ang_thresh, pt_ang))
    return(-2);

  double angle_diff = 0;
  if(langle_is_origin) {
    if(pt_ang > langle)
      angle_diff = pt_ang - langle;
    else
      angle_diff = (360-langle) + pt_ang;
  }
  else {
    if(pt_ang < rangle)
      angle_diff = rangle - pt_ang;
    else
      angle_diff = (360-pt_ang) + rangle;
  }

#if 0
  if(langle_is_origin)
    angle_diff = angle180(pt_ang) - angle180(langle);
  else
    angle_diff = angle180(rangle) - angle180(pt_ang);
#endif
  
  double circumference = M_PI * 2 * ar;
  double dist = (angle_diff / 360) * circumference;
    
  return(dist);
}


//---------------------------------------------------------------
// Procedure: cpasRaySegl
// 

unsigned int cpasRaySegl(double rx, double ry, double ray_angle,
			 const XYSegList& segl, double thresh,
			 vector<double>& vcpa, vector<double>& vray)
{
  // Special case: Seglist is empty
  if(segl.size() == 0)
    return(0);

  // Special case: Seglist contains only one vertex
  if(segl.size() == 1) {
    double px = segl.get_vx(0);
    double py = segl.get_vy(0);
    double ix,iy;
    double dist = distPointToRay(px,py, rx,ry,ray_angle, ix,iy);
    if(dist <= thresh) {
      double dist_to_ray_origin = hypot(rx-ix, ry-iy);
      vcpa.push_back(dist);
      vray.push_back(dist_to_ray_origin);
    }
    return(1);
  }
  
  // General case: Seglist contains at least two vertices
  for(unsigned int i=0; i<segl.size()-1; i++) {

    // Part 1: On the first leg only, check the leading vertex. 
    // Otherwise we'll assume this vertex was checked on the
    // previous leg at the 2nd vertex.
    if(i==0) {
      double px = segl.get_vx(i);
      double py = segl.get_vy(i);
      double ix,iy;
      double dist = distPointToRay(px,py, rx,ry,ray_angle, ix,iy);
      if(dist <= thresh) {
	double dist_to_ray_origin = hypot(rx-ix, ry-iy);
	//double px2 = segl.get_vx(i+1);
	//double py2 = segl.get_vy(i+1);
	//double dist_raybase_to_seg = distPointToSeg(px,py, px2,py2, rx,ry);
	vcpa.push_back(dist);
	vray.push_back(dist_to_ray_origin);
	//vray.push_back(dist_raybase_to_seg);
      }
    }

    // Part 2: check if the current segment intersects the ray

    // define the current seg
    double x1 = segl.get_vx(i);
    double y1 = segl.get_vy(i);
    double x2 = segl.get_vx(i+1);
    double y2 = segl.get_vy(i+1);
    // check if it crosses
    double ix,iy;
    if(crossRaySeg(rx,ry,ray_angle, x1,y1,x2,y2, ix,iy)) {
      double dist_to_ray_origin = hypot(rx-ix, ry-iy);
      //double dist_raybase_to_seg = distPointToSeg(x1,y1,x2,y2, rx,ry);
      vcpa.push_back(0);
      vray.push_back(dist_to_ray_origin);
      //vray.push_back(dist_raybase_to_seg);
    }

    // Part 3: check the 2nd vertex of the current seg
    double px = segl.get_vx(i+1);
    double py = segl.get_vy(i+1);
    double dist = distPointToRay(px,py, rx,ry,ray_angle, ix,iy);
    if(dist <= thresh) {
      double dist_to_ray_origin = hypot(rx-ix, ry-iy);
      //double dist_raybase_to_seg = distPointToSeg(x1,y1,x2,y2, rx,ry);
      //if((i+2) < segl.size()) {
      //double x3 = segl.get_vx(i+2);
      //double y3 = segl.get_vy(i+2);
      //double dist_raybase_to_seg2 = distPointToSeg(x2,y2,x3,y3, rx,ry);
      //if(dist_raybase_to_seg2 < dist_raybase_to_seg)
      //  dist_raybase_to_seg = dist_raybase_to_seg2;
      //}
	
      vcpa.push_back(dist);
      vray.push_back(dist_to_ray_origin);
      //vray.push_back(dist_raybase_to_seg);
    }
    
  }
  return(vcpa.size());
}



//---------------------------------------------------------------
// Procedure: cpasArcSegl
//      Note: For a given arc and seglist determine the CPA points
//            and for each CPA point calculate (a) the actual CPA,
//            and the distance along the arc to one end or the other.
//            origin x,y points are provided to indicate which end
//            of the arc is the origin.

unsigned int cpasArcSegl(double ax, double ay, double ar,
			 double langle, double rangle,
			 const XYSegList& segl, double thresh,
			 bool langle_is_origin,
			 vector<double>& vcpa, vector<double>& vray)
{
  // Special case: Seglist is empty
  if(segl.size() == 0)
    return(0);

  // Special case: Seglist contains only one vertex
  if(segl.size() == 1) {
    double px = segl.get_vx(0);
    double py = segl.get_vy(0);
    double ix,iy;
    double dist = distPointToArcPt(px,py, ax,ay,ar, langle,rangle, ix,iy);
    if(dist <= thresh) {
      double dist_to_arc_origin = distPointOnArc(px,py, ax,ay,ar,
						  langle,rangle,
						  langle_is_origin);
      vcpa.push_back(dist);
      vray.push_back(dist_to_arc_origin);
    }
    return(1);
  }

  // General case: Seglist contains at least two vertices
  for(unsigned int i=0; i<segl.size()-1; i++) {

    // Part 1: On the first leg only, check the leading vertex. 
    // Otherwise we'll assume this vertex was checked on the
    // previous leg at the 2nd vertex.

    if(i==0) {
      double px = segl.get_vx(i);
      double py = segl.get_vy(i);
      double ix,iy;
      double dist_to_arc = distPointToArcPt(px,py, ax,ay,ar,
					    langle,rangle, ix,iy);
      if(dist_to_arc <= thresh) {
	double dist_to_arc_origin = distPointOnArc(ix,iy, ax,ay,ar,
						   langle,rangle,
						   langle_is_origin);
	vcpa.push_back(dist_to_arc);
	vray.push_back(dist_to_arc_origin);
      }
    }

    // define the current seg and min/max x/y values
    double x1 = segl.get_vx(i);
    double y1 = segl.get_vy(i);
    double x2 = segl.get_vx(i+1);
    double y2 = segl.get_vy(i+1);
    double xmin = (x1 <= x2) ? x1 : x2;
    double xmax = (x1 <= x2) ? x2 : x1;
    double ymin = (y1 <= y2) ? y1 : y2;
    double ymax = (y1 <= y2) ? y2 : y1;
    

    // Part 2: Check if the current leg has a CPA in the middle of
    // the leg to the arc

    bool tangent_cpa = false;
    double ix,iy;
    perpLineIntPt(x1,y1,x2,y2, ax,ay, ix,iy);
    
    // We are confident that the perLineIntPt generates a point on
    // the given line. So if the line is ~vertical or ~horizontal,
    // we do not check the x value in the case of vertical or the
    // y value in the case of horizontal.   
    bool plipt_in_seg = true;
    if((ymax-ymin) > 0.000001) {
      if((iy < ymin) || (iy > ymax))
	plipt_in_seg = false;
    }
    if((xmax-xmin) > 0.000001) {
      if((ix < xmin) || (ix > xmax))
	plipt_in_seg = false;
    }
    
    double plipt_rng = hypot(ix-ax, iy-ay);
    
    if(plipt_in_seg && (plipt_rng >= ar)) {
      double plipt_ang = relAng(ax,ay,ix,iy);
      bool plipt_in_arc = angleInArc(langle,rangle,plipt_ang);
      if(plipt_in_arc) {
	tangent_cpa = true;
	double dist_to_arc = (plipt_rng - ar);
	projectPoint(plipt_ang, ar, ax,ay, ix,iy);
	double dist_to_arc_origin = distPointOnArc(ix,iy, ax,ay,ar,
						   langle,rangle,
						   langle_is_origin);
	vcpa.push_back(dist_to_arc);
	vray.push_back(dist_to_arc_origin);

      }
    }
  
    // Part 3: check if the current segment intersects the ray
    if(!tangent_cpa) {
      double ix1,iy1, ix2,iy2;
      int pts = arcSegCrossPts(x1,y1,x2,y2, ax,ay,ar,langle,rangle,
			       ix1,iy1, ix2,iy2);
      if(pts > 0) {
	double dist_to_arc_origin = distPointOnArc(ix1,iy1, ax,ay,ar,
						   langle,rangle,
						   langle_is_origin);
	vcpa.push_back(0);
	vray.push_back(dist_to_arc_origin);
      }
      if(pts > 1) {
	double dist_to_arc_origin = distPointOnArc(ix2,iy2, ax,ay,ar,
						   langle,rangle,
						   langle_is_origin);
	vcpa.push_back(0);
	vray.push_back(dist_to_arc_origin);
      }
    }

    // Part 4: Check the second vertex of this leg. 
    double px = segl.get_vx(i+1);
    double py = segl.get_vy(i+1);
    double dist_to_arc = distPointToArcPt(px,py, ax,ay,ar,
					  langle,rangle, ix,iy);
    if(dist_to_arc <= thresh) {
      double dist_to_arc_origin = distPointOnArc(ix,iy, ax,ay,ar,
						 langle,rangle,
						 langle_is_origin);
      vcpa.push_back(dist_to_arc);
      vray.push_back(dist_to_arc_origin);
    }
  }  
  return(vcpa.size());
}

