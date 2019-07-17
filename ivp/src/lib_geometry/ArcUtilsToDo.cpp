/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ArcUtils.cpp                                         */
/*    DATE: Nov 18th, 2018                                       */
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

using namespace std;

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

