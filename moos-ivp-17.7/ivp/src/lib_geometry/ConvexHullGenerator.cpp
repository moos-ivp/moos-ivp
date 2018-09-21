/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ConvexHullGenerator.cpp                              */
/*    DATE: Aug 25th 2014                                        */
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

void ConvexHullGenerator::addPoint(double x, double y, string label)
{
  XYPoint pt(x,y);
  pt.set_label(label);
  m_original_pts.push_back(pt);
}


//---------------------------------------------------------
// Procedure: generateConvexHull

XYPolygon ConvexHullGenerator::generateConvexHull()
{
  XYPolygon null_poly;
  if(m_original_pts.size() == 0)
    return(null_poly);

  findRoot();
  // Make sure we still have at least three points (root + two)
  // after the findRoot() since some points may be removed if they
  // were found to be identical to the root.
  if(m_points.size() < 2)
    return(null_poly);

  sortPoints();
  // Make sure we still have at least three points (root + two)
  // after the sortPoint() since some points may be removed if they
  // were found to have identical radial angles to the root
  if(m_points.size() < 2)
    return(null_poly);

  m_stack.push_front(m_root);
  m_stack.push_front(m_points[0]);
  m_stack.push_front(m_points[1]);

  for(unsigned int i=2; i<m_points.size(); i++) {
    bool done = false;
    while(!done && (m_stack.size() > 0)) {
      list<XYPoint>::iterator p = m_stack.begin();
      double x1 = p->x();
      double y1 = p->y();
      p++;
      double x0 = p->x();
      double y0 = p->y();
      double x2 = m_points[i].x();
      double y2 = m_points[i].y();
      bool left_turn = threePointTurnLeft(x0,y0,x1,y1,x2,y2);
      if(!left_turn)
	m_stack.pop_front();
      else
	done = true;
    }
    m_stack.push_front(m_points[i]);
  }
  
  XYPolygon new_poly;
  list<XYPoint>::iterator p;
  for(p=m_stack.begin(); p!=m_stack.end(); p++)
    new_poly.add_vertex(p->x(), p->y());

  return(new_poly);
}

  

//---------------------------------------------------------
// Procedure: findRoot

void ConvexHullGenerator::findRoot()
{
  // Sanity check
  if(m_original_pts.size() == 0)
    return;

  // Find the index of point with the lowest y-value
  double       lowest_y_val = m_original_pts[0].get_vy();
  unsigned int lowest_y_ix  = 0;
  for(unsigned int i=1; i<m_original_pts.size(); i++) {
    if(m_original_pts[i].y() < lowest_y_val) {
      lowest_y_val = m_original_pts[i].y();
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
	if(pta.z() != ptb.z()) {
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

#if 0
  cout << "************************************************" << endl;
  for(unsigned int i=0; i<m_points.size(); i++) {
    XYPoint pt = m_points[i];
    cout << "[" << i << "]: [" << pt.get_label() << "]" << 
      ",x=" << pt.x() << ",y=" << pt.y() << ",z=" << pt.z() << endl;
  }
  cout << "************************************************" << endl;
#endif

}




