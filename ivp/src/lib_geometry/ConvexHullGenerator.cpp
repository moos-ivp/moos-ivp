/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ConvexHullGenerator.cpp                              */
/*    DATE: Aug 25th 2014                                        */
/*    DATE: Jun 8th  2020 Major revisions                        */
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
#include "ConvexHullGenerator.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

//---------------------------------------------------------
// Procedure: addPoint

void ConvexHullGenerator::addPoint(double x, double y)
{
  XYPoint pt(x,y);
  m_original_pts.push_back(pt);
}

//---------------------------------------------------------
// Procedure: addPoint

void ConvexHullGenerator::addPoint(XYPoint point)
{
  m_original_pts.push_back(point);
}

//---------------------------------------------------------
// Procedure: addPoint

void ConvexHullGenerator::addPoint(double x, double y, string label)
{
  XYPoint pt(x,y);
  pt.set_label(label);
  m_original_pts.push_back(pt);
}


//---------------------------------------------------------
// Procedure: generateConvexHull
//      Note: Based on Graham-Scan in CLR page 1031

XYPolygon ConvexHullGenerator::generateConvexHull()
{
  list<XYPoint> pt_stack;
  XYPolygon null_poly;
  if(m_original_pts.size() == 0)
    return(null_poly);

  //========================================================================
  // Part 1: Select a root point and perform the sortPoints() scan
  //========================================================================

  // Pick the starting point, the lowest y val, using lowest x bresk ties
  findRoot();

  // Sort points by angle to the root, dropping points that have the same
  // angle by droping the closer of the two.
  sortPoints();

  //========================================================================
  // Part 2: Handle special cases of only 1 or 2 pts remaining after scan
  //========================================================================

  // If only two points are left, handle special case
  if(m_points.size() == 1) {
    XYPolygon new_poly = generateConvexHullTwoPts(m_root, m_points[0]);
    return(new_poly);
  }

    // If only one point, the root is left, then handle special case
  if(m_points.size() == 0) {
    XYPolygon new_poly = generateConvexHullOnePt(m_root);
    return(new_poly);
  }
  
  //========================================================================
  // Part 3: Handle general case with at least three points after sortPoints
  //========================================================================
  pt_stack.push_front(m_root);
  pt_stack.push_front(m_points[0]);

  for(unsigned int i=1; i<m_points.size(); i++) {
    bool done = false;
    while(!done && (pt_stack.size() > 0)) {
      list<XYPoint>::iterator p = pt_stack.begin();
      double x1 = p->x();
      double y1 = p->y();
      p++;
      double x0 = p->x();
      double y0 = p->y();
      
      double x2 = m_points[i].x();
      double y2 = m_points[i].y();
      bool left_turn = threePointTurnLeft(x0,y0,x1,y1,x2,y2);
      if(!left_turn) {
	pt_stack.pop_front();
	if(pt_stack.size() == 1)
	  done = true;
      }
      else
	done = true;
    }
    pt_stack.push_front(m_points[i]);
  }

  //========================================================================
  // Part 3B: Scan is done and at least three points survived
  //========================================================================
  if(pt_stack.size() >= 3) {
    XYPolygon new_poly;
    list<XYPoint>::iterator p;
    for(p=pt_stack.begin(); p!=pt_stack.end(); p++)
      new_poly.add_vertex(p->x(), p->y());
    new_poly.determine_convexity();
    
    if(m_settling_enabled && !new_poly.is_convex())
      new_poly = new_poly.crossProductSettle();

    // If new_poly is not convex, it must be due to settling dealing with
    // thus far undetected colinearity. And there must be only two points.
    // Get those two points and pass them to the two-point handler
    if(new_poly.size() == 2) {
      XYPoint a(new_poly.get_vx(0), new_poly.get_vy(0));
      XYPoint b(new_poly.get_vx(1), new_poly.get_vy(1));
      XYPolygon new_poly = generateConvexHullTwoPts(a, b);
      return(new_poly);
    }

    new_poly.determine_convexity();
    return(new_poly);
  }

  //========================================================================
  // Part 3C: Scan is done and two or less points survived
  //========================================================================
  if(pt_stack.size() == 2) {
    XYPoint a = pt_stack.front();
    XYPoint b = pt_stack.back();
    XYPolygon new_poly = generateConvexHullTwoPts(a, b);
    return(new_poly);
  }

  // Else stack size is one, only possible case left, essentially meaning
  // all original points were at the same location.
  XYPoint a = pt_stack.front();
  XYPolygon new_poly = generateConvexHullOnePt(a);
  return(new_poly);
}

  
//---------------------------------------------------------
// Procedure: generateConvexHullTwoPts()

XYPolygon ConvexHullGenerator::generateConvexHullTwoPts(XYPoint a,
							XYPoint b)
{
  XYPolygon null_poly;

  double x1 = a.x();
  double y1 = a.y();
  double x2 = b.x();
  double y2 = b.y();

  double dist = hypot(x1-x2, y1-y2);
  if(dist < 0.001)
    return(generateConvexHullOnePt(a));
  
  double distx = dist / 10.0;

  double ang    = relAng(x1,y1, x2,y2);
  double ang90  = angle360(ang+90);
  double ang270 = angle360(ang+270);

  double px1,py1;
  projectPoint(ang90, distx, x1,y1, px1,py1);
  double px2,py2;
  projectPoint(ang270, distx, x1,y1, px2,py2);

  double px3,py3;
  projectPoint(ang270, distx, x2,y2, px3,py3);
  double px4,py4;
  projectPoint(ang90, distx, x2,y2, px4,py4);
  
  XYPolygon new_poly;
  new_poly.add_vertex(px1, py1);
  new_poly.add_vertex(px2, py2);
  new_poly.add_vertex(px3, py3);
  new_poly.add_vertex(px4, py4);

  new_poly.determine_convexity();
  return(new_poly);
}

  
//---------------------------------------------------------
// Procedure: generateConvexHullOnePt()

XYPolygon ConvexHullGenerator::generateConvexHullOnePt(XYPoint a)
{
  double x1 = a.x();
  double y1 = a.y();

  double dist = 0.1;

  double px1,py1;
  projectPoint(30,  dist, x1,y1, px1,py1);
  double px2,py2;
  projectPoint(150, dist, x1,y1, px2,py2);
  double px3,py3;
  projectPoint(270, dist, x1,y1, px3,py3);
  
  XYPolygon new_poly;
  new_poly.add_vertex(px1, py1);
  new_poly.add_vertex(px2, py2);
  new_poly.add_vertex(px3, py3);

  new_poly.determine_convexity();
  return(new_poly);
}

  

//---------------------------------------------------------
// Procedure: findRoot
//   Purpose: The first step of the Graham-Scan algorithm is to
//            identify the root point. In this case it's the
//            point with the lowest y-value. Ties are broken
//            using the lowest x value. Duplicate points, if 
//            any, will be sorted out in later parts of the
//            generator:
//   Actions: m_root will take on the root point
//            All other points into the vector m_points.

void ConvexHullGenerator::findRoot()
{
  // Sanity check
  if(m_original_pts.size() == 0)
    return;

  // Find the index of point with the lowest y-value
  double       lowest_x_val = m_original_pts[0].get_vx();
  double       lowest_y_val = m_original_pts[0].get_vy();
  unsigned int lowest_y_ix  = 0;
  for(unsigned int i=1; i<m_original_pts.size(); i++) {
    bool newpt = false;
    if(m_original_pts[i].y() < lowest_y_val)
      newpt = true;
    else if((m_original_pts[i].y() == lowest_y_val) &&
	    (m_original_pts[i].x() < lowest_x_val)) {
      newpt = true;
    }

    if(newpt) {
      lowest_y_val = m_original_pts[i].y();
      lowest_x_val = m_original_pts[i].x();
      lowest_y_ix = i;
    }
  }

  // Make the point with the lowest y-value the "root"
  m_root = m_original_pts[lowest_y_ix];
  
  // Put all non-root points into a separate group
  for(unsigned int i=0; i<m_original_pts.size(); i++) {
    if(i != lowest_y_ix) {
      double rx = m_root.x();
      double ry = m_root.y();
      // Remove any points that are identical with the root
      if((rx != m_original_pts[i].x()) || (ry != m_original_pts[i].y()))
	m_points.push_back(m_original_pts[i]);
    }
  }
}


//---------------------------------------------------------
// Procedure: sortPoints

void ConvexHullGenerator::sortPoints()
{
  // Part 1: Sanity check
  if(m_points.size() == 0)
    return;

  // Part 2: Find the original radial angles ranging from [-180, 180]
  vector<double> radial_angles;
  double rx = m_root.x();
  double ry = m_root.y();
  
  for(unsigned int i=0; i<m_points.size(); i++) {
    double angle = relAng(rx, ry, m_points[i].x(), m_points[i].y());
    angle = angle180(angle);
    radial_angles.push_back(angle);
  }

  // Part 3: Sort based on radial angle, but keep the radial angle 
  //         associated with each point for detecting ties.
  vector<XYPoint> new_points;
  vector<double>  new_angles;
  bool done = false;
  while(!done) {
    double highest_angle_sofar = -181;
    unsigned int index = m_points.size();
    for(unsigned int i=0; i<m_points.size(); i++) {
      if((radial_angles[i] != -999) && (radial_angles[i] > highest_angle_sofar)) {
	index = i;
	highest_angle_sofar = radial_angles[i];
      }
    }
    if(index == m_points.size())
      done = true;
    else {
      m_points[index].set_vz(radial_angles[index]);
      radial_angles[index] = -999;
      new_points.push_back(m_points[index]);
      new_angles.push_back(radial_angles[index]);
    }
  }

  // Part 4: Go through the sorted points and detect sets of points
  //         with identical radial angles, and for each such set
  //         remove all but the point farthest from the root point.
  //            
  // Part 4A: Make a linked list of points where each point stores its
  //          previously calculated radial angle in the z component.
  list<XYPoint> ptlist;
  for(unsigned int i=0; i<new_points.size(); i++) 
    ptlist.push_back(new_points[i]);
  // Part 4B: Go through the list and do the actual work of detecting
  //          sets of points with identical radial angles.

  // Declare a threshold for declaring angles to be unique. Straight
  // inequality check allows essentially colinear points to go unhandled.
  double delta_angle_thresh = 0.1;

  new_points.clear();
  done = false;
  while(!done) {
    XYPoint pta = ptlist.front();
    ptlist.pop_front();
    if(ptlist.size() == 0) {
      new_points.push_back(pta);
      done = true;
    }
    else {
      bool finished = false;
      while(!finished) {
	XYPoint ptb = ptlist.front();
	double delta = pta.z() - ptb.z();
	if(delta < 0)
	  delta = -delta;
	if(delta > delta_angle_thresh) {
	  new_points.push_back(pta);
	  finished = true;
	}
	else {
	  ptlist.pop_front();
	  double dista = distPointToPoint(rx, ry, pta.x(), pta.y());
	  double distb = distPointToPoint(rx, ry, ptb.x(), ptb.y());
	  if(distb > dista) 
	    pta = ptb;
	  if(ptlist.size() == 0) {
	    new_points.push_back(pta);
	    finished = true;
	    done = true;
	  }
	}
      }
    }
  }
	  
  // Index 0 should now contain the point with highest angle wrt root.
  m_points = new_points;
}
