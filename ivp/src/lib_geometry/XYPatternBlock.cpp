/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYPatternBlock.cpp                                   */
/*    DATE: April 30th 2009                                      */
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

#ifdef _WIN32
   #include <string>
#endif

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "XYPatternBlock.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

XYPatternBlock::XYPatternBlock()
{
  m_blocklen = 80;
  m_blockwid = 40;
  m_swathwid = 10;
  m_angle    = 0;

  // Defaults are false for backward compatibility prior to 19.8.1
  m_core_wid = false;
  m_auto_drop = false;
}

//-----------------------------------------------------------
// Procedure: setParam

bool XYPatternBlock::setParam(string param, string value)
{
  param = tolower(param);
  double dval = atof(value.c_str());

  // A parameter change would invalidate any intermediate results
  m_lane_points.clear();
  m_lane_segments.clear();

  if((param == "block_width") && isNumber(value))
    return(setParam(param, dval));
  if((param == "block_length") && isNumber(value))
    return(setParam(param, dval));
  if((param == "swath_width") && isNumber(value))
    return(setParam(param, dval));
  if((param == "angle") && isNumber(value))
    return(setParam(param, dval));
  else if(param == "id_point") {
    string left = biteStringX(value, ',');
    string right = value;
    if(isNumber(left) && isNumber(right)) {
      XYPoint new_point(atof(left.c_str()), atof(right.c_str()));
      m_id_points.push_back(new_point);
      return(true);
    }
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: setParam

bool XYPatternBlock::setParam(string param, double value)
{
  param = tolower(param);

  if((param == "block_width") && (value > 0))
    m_blockwid = value; 
  else if((param == "block_length") && (value > 0))
    m_blocklen = value;
  else if((param == "swath_width") && (value > 0))
    m_swathwid = value;
  else if((param == "angle") && (value >= 1))
    m_angle = value;
  else
    return(false);
  
  return(true);
}


//-----------------------------------------------------------
// Procedure: addIDPoint

void XYPatternBlock::addIDPoint(const XYPoint& cpt)
{
  m_id_points.push_back(cpt);
}


//---------------------------------------------------------------
// Procedure: lanePoints
//      Note: Produce a set of points on either side of a center
//            point (cptx, cpty) at angles perpendicular to the given
//            angle. 

int XYPatternBlock::setLanePoints()
{
  vector<XYPoint> new_vector;
  
  double alter_ang1 = angle360(m_angle - 90);
  double alter_ang2 = angle360(m_angle + 90);

  // When core_wid enabled, prune a half a lane width from both
  // sides. Presumably the sensor has a range of at least half a
  // lane width so we can start/end a half lane from the block edge
  if(m_core_wid && (m_blockwid > m_swathwid))
    m_blockwid -= m_swathwid;
  
  double lane_pts_count_d = (m_blockwid / m_swathwid) + 1; 
#ifdef _WIN32
  int i, lane_pts_count = (int)(floor(lane_pts_count_d + 0.99999));
#else
  int i, lane_pts_count = (int)(trunc(lane_pts_count_d + 0.99999));
#endif
  
  double cptx, cpty;
  idPointCenter(cptx, cpty);

  for(i=0; i< lane_pts_count; i++) {
    double dist_from_pt = (m_blockwid/2.0) - ((double)(i) * m_swathwid);
    double ang_from_pt = alter_ang1;
    if(dist_from_pt < 0) {
      ang_from_pt = alter_ang2; 
      dist_from_pt *= -1;
    }
    XYPoint new_point;
    if(dist_from_pt == 0)
      new_point.set_vertex(cptx, cpty);
    else {
      double new_x, new_y;
      projectPoint(ang_from_pt, dist_from_pt, cptx, cpty, new_x, new_y);
      new_point.set_vertex(new_x, new_y);
    }
    string idstr = doubleToString(i,0);
    new_point.set_label(idstr);
    new_vector.push_back(new_point);
  }
  
  m_lane_points = new_vector;
  return(new_vector.size());
}



//---------------------------------------------------------------
// Procedure: setLaneSegments
//      Note: Produce a set of line segments of the given length
//            (blocklen) at the given angle, each with the one of the
//            given points (pts) as the center point.

int XYPatternBlock::setLaneSegments()
{
  vector<XYSegList> new_vector;

  double ang1 = angle360(m_angle);
  double ang2 = angle360(m_angle-180);

  unsigned int i, vsize = m_lane_points.size();
  for(i=0; i<vsize; i++) {
    XYPoint midpt = m_lane_points[i];
    XYPoint pt1 = projectPoint(ang1, (m_blocklen/2), midpt.x(), midpt.y());
    XYPoint pt2 = projectPoint(ang2, (m_blocklen/2), midpt.x(), midpt.y());

    XYSegList new_segl;
    new_segl.add_vertex(pt1.get_vx(), pt1.get_vy());
    new_segl.add_vertex(pt2.get_vx(), pt2.get_vy());

    new_vector.push_back(new_segl);
  }

  m_lane_segments = new_vector;
  return(m_lane_segments.size());
}


//---------------------------------------------------------------
// Procedure: setCompositeSegList
//      Note: Produce a single XYSegList comprised of each of the 
//            given XYSegLists. Each given XYSegList should be a 
//            single line segment. Each line segment should be
//            parallel and in order.

int XYPatternBlock::setCompositeSegList(double osx, double osy)
{

  XYSegList new_segl;
  unsigned int i, vsize = m_lane_segments.size();

  if(vsize == 0)
    return(0);

  // Quick check that all given segls are a single line segment.
  for(i=0; i<vsize; i++) 
    if(m_lane_segments[i].size() != 2)
      return(0);
  
  // First determine which Point will be the first/entry point. It will
  // be one of the two points in either the first segment or last segl.
  
  // By default, assume the first/entry point is the zero'th point in 
  // the zero'th segl.
  bool first_segl_zero = true;
  bool first_pt_zero = true;

  double x1 = m_lane_segments[0].get_vx(0);
  double y1 = m_lane_segments[0].get_vy(0);
  double x2 = m_lane_segments[0].get_vx(1);
  double y2 = m_lane_segments[0].get_vy(1);
  double x3 = m_lane_segments[vsize-1].get_vx(0);
  double y3 = m_lane_segments[vsize-1].get_vy(0);
  double x4 = m_lane_segments[vsize-1].get_vx(1);
  double y4 = m_lane_segments[vsize-1].get_vy(1);

  double dist1 = distPointToPoint(osx, osy, x1, y1);
  double dist2 = distPointToPoint(osx, osy, x2, y2);
  double dist3 = distPointToPoint(osx, osy, x3, y3);
  double dist4 = distPointToPoint(osx, osy, x4, y4);

  double min_dist = dist1;
  if(dist2 < min_dist) {
    first_segl_zero = true;
    first_pt_zero   = false;
    min_dist = dist2;
  }
  if(dist3 < min_dist) {
    first_segl_zero = false;
    first_pt_zero   = true;
    min_dist = dist3;
  }
  if(dist4 < min_dist) {
    first_segl_zero = false;
    first_pt_zero   = false;
    min_dist = dist4;
  }

  // Now we know our first/entry point, indicated by the values of
  // first_segl_zero and first_pt_zero. If first_segl zero is false
  // then we reverse the vector of segls, so that the first/entry
  // point is indeed in the zeroth segl.
  if(first_segl_zero == false) {
    std::reverse(m_lane_segments.begin(), m_lane_segments.end());
    //vector<XYSegList> new_segls;
    //for(i=vsize-1; i>0; i--)
    //  new_segls.push_back(m_lane_segmentse[i]);
    //segls = new_segls;
  }

  EdgeTagSet edge_tags;
  
  // Now build the combined XYSegList from the vector of segls. We
  // alternate taking the zeroth or end point from each segl for 
  // building the combined SegList.
  bool zeroth = first_pt_zero;
  for(i=0; i<vsize; i++) {
    double x0 = m_lane_segments[i].get_vx(0);
    double y0 = m_lane_segments[i].get_vy(0);
    double x1 = m_lane_segments[i].get_vx(1);
    double y1 = m_lane_segments[i].get_vy(1);
    if(zeroth) {
      new_segl.add_vertex(x0, y0);
      new_segl.add_vertex(x1, y1);
    }
    else {
      new_segl.add_vertex(x1, y1);
      new_segl.add_vertex(x0, y0);
    }
    zeroth = !zeroth;

    // Mark each of the non-turning legs as lanes
    if(new_segl.size() >= 2) {
      unsigned int ix1 = new_segl.size() - 2;
      unsigned int ix2 = new_segl.size() - 1;
      EdgeTag tag(ix1, ix2, "lane");
      edge_tags.addEdgeTag(tag);
    }
      
  }

  new_segl.set_edge_tags(edge_tags);
  m_composite_seglist = new_segl;
  return(m_composite_seglist.size());
}

//---------------------------------------------------------------
// Procedure: buildCompositeSegList

void XYPatternBlock::buildCompositeSegList(double osx, double osy)
{
  setLanePoints();
  setLaneSegments();
  setCompositeSegList(osx, osy);
}


//---------------------------------------------------------------
// Procedure: distanceToClosestEntry
//      Note: By default, any_segment is set to true and the shortest
//            distance is calculated by considering the endpoints of
//            ANY of the lane segments. If any_segment is false, only 
//            the first,last (or outer) lane segments are considered.
//   Returns: -1 if there are any problems
//            or Distance to the shortest entry point.


double XYPatternBlock::distanceToClosestEntry(double osx, double osy,
					    bool any_segment)
{
  // Assume the lane_points and lane_segments haven't been built yet.
  setLanePoints();
  setLaneSegments();

  // If the number of lane_segments is zero, something is badly wrong.
  unsigned int i, vsize = m_lane_segments.size();
  if(vsize == 0)
    return(-1);

  // Initialize the shortest distance calculated.
  double shortest_dist = -1;

  for(i=0; i<vsize; i++) {
    if((i==0) || (i=(vsize-1)) || any_segment) {
      int seg_size = m_lane_segments[i].size();
      if(seg_size != 2)
	return(-1);
      // The points at one end of the line segment
      double ptx0  = m_lane_segments[i].get_vx(0);
      double pty0  = m_lane_segments[i].get_vy(0);
      // The points at the other end of the line segment
      double ptx1  = m_lane_segments[i].get_vx(1);
      double pty1  = m_lane_segments[i].get_vy(1);
      double dist0 = hypot(osx-ptx0, osy-pty0);
      double dist1 = hypot(osx-ptx1, osy-pty1);
      if((shortest_dist == -1) || (dist0 < shortest_dist))
	shortest_dist = dist0;
      if(dist1 < shortest_dist)
	shortest_dist = dist1;
    }
  }
  return(shortest_dist);
}

//---------------------------------------------------------------
// Procedure: distanceToCrossAxis

double XYPatternBlock::distanceToCrossAxis(double osx, double osy) const
{
  if((m_id_points.size() == 0) || (m_lane_points.size() == 0))
    return(-1);

  // First get the two axis points that determing the cross axis line
  double axis_x1, axis_y1;
  idPointCenter(axis_x1, axis_y1);

  double axis_x2 = m_lane_points[0].get_vx();
  double axis_y2 = m_lane_points[0].get_vy();

  // Then find the perpendicular line intercept point
  double plix, pliy;
  perpLineIntPt(axis_x1, axis_y1, axis_x2, axis_y2, osx, osy, plix, pliy);

  // Then get the distance between the two points

  double dist = hypot((osx-plix), (osy-pliy));
  return(dist);
}


//-----------------------------------------------------------
// Procedure: idPointCenter
//   Purpose: Find the center point of the current set of 
//            id_points.

void XYPatternBlock::idPointCenter(double& retx, double& rety) const
{
  unsigned int i, vsize = m_id_points.size();
  if(vsize == 0)
    return;
  
  double x_total = 0;
  double y_total = 0;
  
  for(i=0; i<vsize; i++) {
    x_total += m_id_points[i].get_vx();
    y_total += m_id_points[i].get_vy();
  }

  retx = x_total / (double)(vsize);
  rety = y_total / (double)(vsize);
}









