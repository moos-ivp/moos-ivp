/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYPolygon.cpp                                        */
/*    DATE: Apr 20th, 2005                                       */
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

#include <cmath>
#include "XYPolygon.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif


//---------------------------------------------------------------
// Procedure: Constructor

XYPolygon::XYPolygon()
{
  m_convex_state = false;
  m_transparency = 0.5;
}

//---------------------------------------------------------------
// Procedure: add_vertex
//    o A call to "determine_convexity()" is made since this
//      operation may result in a change in the convexity.
//    o The check_convexity option allows a bunch of vertices to be
//      added and then just check for convexity at the end. 

bool XYPolygon::add_vertex(double x, double y, bool check_convexity)
{
  XYSegList::add_vertex(x,y);
  m_side_xy.push_back(-1);
  
  // With new vertex, we don't know if the new polygon is valid
  if(check_convexity) {
    determine_convexity();
    return(m_convex_state);
  }
  else
    return(true);
}

//---------------------------------------------------------------
// Procedure: add_vertex
//    o A call to "determine_convexity()" is made since this
//      operation may result in a change in the convexity.
//    o The check_convexity option allows a bunch of vertices to be
//      added and then just check for convexity at the end. 

bool XYPolygon::add_vertex(double x, double y, 
			   double z, bool check_convexity)
{
  XYSegList::add_vertex(x,y,z);
  m_side_xy.push_back(-1);
  
  // With new vertex, we don't know if the new polygon is valid
  if(check_convexity) {
    determine_convexity();
    return(m_convex_state);
  }
  else
    return(true);
}

//---------------------------------------------------------------
// Procedure: add_vertex
//    o A call to "determine_convexity()" may be made since this
//      operation may result in a change in the convexity.
//    o The check_convexity option allows a bunch of vertices to be
//      added and then just check for convexity at the end. 

bool XYPolygon::add_vertex(double x, double y, double z, 
			   string property, bool check_convexity)
{
  XYSegList::add_vertex(x, y, z, property);
  m_side_xy.push_back(-1);
  
  // With new vertex, we don't know if the new polygon is valid
  if(check_convexity) {
    determine_convexity();
    return(m_convex_state);
  }
  else
    return(true);
}

//---------------------------------------------------------------
// Procedure: alter_vertex
//   Purpose: Given a new vertex, find the existing vertex that is
//            closest, and replace it with the new one.
//      Note: A call to "determine_convexity()" is made since this
//            operation may result in a change in the convexity.

bool XYPolygon::alter_vertex(double x, double y, double z)
{
  XYSegList::alter_vertex(x,y);

  determine_convexity();
  return(m_convex_state);
}

//---------------------------------------------------------------
// Procedure: grow_by_pct
//      Note: A call to "determine_convexity()" is made since this
//            operation may result in a change in the convexity.

void XYPolygon::grow_by_pct(double pct)
{
  XYSegList::grow_by_pct(pct);
  determine_convexity();
}

//---------------------------------------------------------------
// Procedure: grow_by_amt
//      Note: A call to "determine_convexity()" is made since this
//            operation may result in a change in the convexity.

void XYPolygon::grow_by_amt(double amt)
{
  XYSegList::grow_by_amt(amt);
  determine_convexity();
}

//---------------------------------------------------------------
// Procedure: delete_vertex
//   Purpose: Given a new vertex, find the existing vertex that is
//            closest, and delete it.
//      Note: A call to "determine_convexity()" is made since this
//            operation may result in a change in the convexity.

bool XYPolygon::delete_vertex(double x, double y)
{
  unsigned int vsize = m_vx.size();
  if(vsize == 0)
    return(false);

  unsigned int i, ix = closest_vertex(x, y); 

  vector<int>  new_xy;
  
  for(i=0; i<ix; i++)
    new_xy.push_back(m_side_xy[i]);
  for(i=ix+1; i<vsize; i++)
    new_xy.push_back(m_side_xy[i]);
  
  m_side_xy  = new_xy;

  XYSegList::delete_vertex(x,y);

  determine_convexity();
  return(m_convex_state);
}

//---------------------------------------------------------------
// Procedure: insert_vertex
//   Purpose: Given a new vertex, find the existing segment that is
//            closest, and add the vertex between points
//      Note: A call to "determine_convexity()" is made since this
//            operation may result in a change in the convexity.

bool XYPolygon::insert_vertex(double x, double y, double z)
{
  unsigned int vsize = m_vx.size();
  if(vsize <= 1)
    return(add_vertex(x, y, z));

  unsigned int i, ix = XYPolygon::closest_segment(x, y); 

  vector<int> new_xy;
  
  for(i=0; i<=ix; i++) 
    new_xy.push_back(m_side_xy[i]);
  new_xy.push_back(2);
  for(i=ix+1; i<vsize; i++)
    new_xy.push_back(m_side_xy[i]);
  
  m_side_xy  = new_xy;

  XYSegList::insert_vertex(x,y,z);

  determine_convexity();
  return(m_convex_state);
}

//---------------------------------------------------------------
// Procedure: clear

void XYPolygon::clear()
{
  XYSegList::clear();
  m_side_xy.clear();
  m_convex_state = false;
}


//---------------------------------------------------------------
// Procedure: is_clockwise()
//      Note: Determine if the ordering of points in the internal
//            vector of stored points constitutes a clockwise walk
//            around the polygon. Algorithm base on progression of
//            relative angle from the center. Result is somewhat
//            undefined if the polygon is not convex. If it is 
//            "nearly" convex, it should still be accurate.

bool XYPolygon::is_clockwise() const
{
  unsigned int i, vsize = m_vx.size();
  if(vsize < 3)
    return(false);

  int inc_count = 0;
  int dec_count = 0;

  double cx = get_center_x();
  double cy = get_center_y();

  for(i=0; i<vsize; i++) {
    unsigned int j = i+1; 
    if(j == vsize)
      j = 0;
    double relative_angle_1 = relAng(cx, cy, m_vx[i], m_vy[i]);
    double relative_angle_2 = relAng(cx, cy, m_vx[j], m_vy[j]);
    if(relative_angle_2 > relative_angle_1)
      inc_count++;
    else
      dec_count++;
  }

  bool clockwise;
  if(inc_count > dec_count)
    clockwise = true;
  else
    clockwise = false;

  return(clockwise);
}

//---------------------------------------------------------------
// Procedure: apply_snap
//      Note: A call to "determine_convexity()" is made since this
//            operation may result in a change in the convexity.
//      Note: Will not allow a snap that changes the convexity state
//            from TRUE to FALSE
//   Returns: true if the snap was successfully 

bool XYPolygon::apply_snap(double snapval)
{
  vector<double> tmp_m_vx = m_vx;
  vector<double> tmp_m_vy = m_vy;

  // Determine if it is convex prior to applying the snapval
  bool start_convex = is_convex();

  XYSegList::apply_snap(snapval);
  determine_convexity();
  if(is_convex() || !start_convex)
    return(true);
  else {
    m_vx = tmp_m_vx;
    m_vy = tmp_m_vy;
    determine_convexity();
    return(false);
  }
}

//---------------------------------------------------------------
// Procedure: reverse
//      Note: A call to "determine_convexity()" is made since this
//            operation needs to have m_side_xy[i] reset for each i.

void XYPolygon::reverse()
{
  XYSegList::reverse();
  determine_convexity();
}

//---------------------------------------------------------------
// Procedure: rotate
//      Note: A call to "determine_convexity()" is made since this
//            operation needs to have m_side_xy[i] reset for each i.


void XYPolygon::rotate(double val)
{
  XYSegList::rotate(val);
  determine_convexity();
}


//---------------------------------------------------------------
// Procedure: rotate
//      Note: A call to "determine_convexity()" is made since this
//            operation needs to have m_side_xy[i] reset for each i.


void XYPolygon::rotate(double val, double cx, double cy)
{
  XYSegList::rotate(val, cx, cy);
  determine_convexity();
}


//---------------------------------------------------------------
// Procedure: contains

bool XYPolygon::contains(double x, double y) const
{
  if(!m_convex_state)
    return(false);

  unsigned int ix, vsize = m_vx.size();
  if(vsize == 0)
    return(false);

  double x1, y1, x2, y2 = 0;
  for(ix=0; ix<vsize; ix++) {

    x1 = m_vx[ix];
    y1 = m_vy[ix];
    
    int ixx = ix+1;
    if(ix == vsize-1)
      ixx = 0;
    
    x2 = m_vx[ixx];
    y2 = m_vy[ixx];

    int vside = side(x1, y1, x2, y2, x, y);
    if((vside != 2) && (vside != m_side_xy[ix]))
      return(false);
  }
  return(true);
}

//---------------------------------------------------------------
// Procedure: contains
//   Purpose: Returns true if the given polygon is convex and all its
//            vertices are contained within this polygon.

bool XYPolygon::contains(const XYPolygon& inner_poly) const
{
  if(!m_convex_state)
    return(false);
  if(!inner_poly.is_convex())
    return(false);

  unsigned int ip_size = inner_poly.size();
  for(unsigned int i=0; i<ip_size; i++) {
    double x = inner_poly.get_vx(i);
    double y = inner_poly.get_vy(i);
    if(!contains(x,y))
      return(false);
  }
  return(true);
}

//---------------------------------------------------------------
// Procedure: intersects

bool XYPolygon::intersects(const XYPolygon &poly) const
{
  unsigned int this_size = m_vx.size();
  unsigned int poly_size = poly.size();
  
  if(this_size == 0)
    return(false);
  if(poly_size == 0)
    return(false);

  // First check that no vertices from "this" polygon are
  // contained in the given polygon
  unsigned int i;
  for(i=0; i<this_size; i++) {
    double x = m_vx[i];
    double y = m_vy[i];
    if(poly.contains(x, y))
      return(true);
  }
  // Then check that no vertices from the given polygon are
  // contained in "this" polygon
  for(i=0; i<poly_size; i++) {
    double x = poly.get_vx(i);
    double y = poly.get_vy(i);
    if(this->contains(x, y))
      return(true);
  }

  // Then check that no segments from "this" polygon intersect
  // the given polygon
  for(i=0; i<this_size; i++) {
    double x1 = this->get_vx(i);
    double y1 = this->get_vy(i);
    double x2 = this->get_vx(0);
    double y2 = this->get_vy(0);
    if((i+1) < this_size) {
      x2 = this->get_vx(i+1);
      y2 = this->get_vy(i+1);
    }
    if(poly.seg_intercepts(x1,y1,x2,y2))
      return(true);
  }

  return(false);
}


//---------------------------------------------------------------
// Procedure: dist_to_poly()

double XYPolygon::dist_to_poly(const XYPolygon &poly) const
{
  // Part 1: simple check for intersection test
  if(intersects(poly))
    return(0);

  double min_dist = -1;

  // Part 2: check distance of all vertices in THIS polygon to
  // the given polygon
  for(unsigned int i=0; i<size(); i++) {
    double dist = poly.dist_to_poly(get_vx(i), get_vy(i));
    if((dist < min_dist) || (min_dist == -1)) {
      min_dist = dist;
    }
  }

  // Part 3: check distance of all vertices in the given polygon to
  // the vertices in THIS polygon
  for(unsigned int i=0; i<poly.size(); i++) {
    double dist = dist_to_poly(poly.get_vx(i), poly.get_vy(i));
    if((dist < min_dist) || (min_dist == -1)) {
      min_dist = dist;
    }
  }

  return(min_dist);
}


//---------------------------------------------------------------
// Procedure: dist_to_poly

double XYPolygon::dist_to_poly(double px, double py) const
{
  unsigned int ix, vsize = m_vx.size();
  if(vsize == 0)
    return(-1);

  if(vsize == 1)
    return(distPointToPoint(px, py, m_vx[0], m_vy[0]));
  
  if(vsize == 2)
    return(distPointToSeg(m_vx[0], m_vy[0], 
			  m_vx[1], m_vy[1], px, py)); 
	   
  // Distance to poly is given by the shortest distance to any
  // one of the edges.
  double dist = 0;
  for(ix=0; ix<vsize; ix++) {
    int ixx = ix+1;
    if(ix == vsize-1)
      ixx = 0;

    double idist = distPointToSeg(m_vx[ix], m_vy[ix], 
				  m_vx[ixx], m_vy[ixx], 
				  px, py); 
    if((ix==0) || (idist < dist))
      dist = idist;
  }

  return(dist);
}

//---------------------------------------------------------------
// Procedure: dist_to_poly
//      Note: Determine the distance between the line segment given
//            by x3,y3,x4,y4 to the polygon. An edge-by-edge check
//            is performed and the minimum returned.

double XYPolygon::dist_to_poly(double x3, double y3, 
			       double x4, double y4) const
{
  unsigned int ix, vsize = m_vx.size();
  
  if(vsize == 0)
    return(-1);
  
  if(vsize == 1)
    return(distPointToSeg(x3,y3,x4,y4, m_vx[0], m_vy[0]));
  
  if(vsize == 2)
    return(distSegToSeg(m_vx[0], m_vy[0], 
			m_vx[1], m_vy[1], x3,y3,x4,y4)); 
	   
  // Distance to poly is given by the shortest distance to any
  // one of the edges.
  double dist = 0;
  for(ix=0; ix<vsize; ix++) {
    int ixx = ix+1;
    if(ix == vsize-1)
      ixx = 0;

    double idist = distSegToSeg(m_vx[ix], m_vy[ix], 
				m_vx[ixx], m_vy[ixx], 
				x3, y3, x4, y4); 
    if((ix==0) || (idist < dist))
      dist = idist;
  }
  
  return(dist);
}

//---------------------------------------------------------------
// Procedure: dist_to_poly
//      Note: Determine the distance between the point given by px,py
//            to the polygon along a given angle. An edge-by-edge check
//            is performed and the minimum returned. 
//   Returns: -1 if given ray doesn't intersect the polygon

double XYPolygon::dist_to_poly(double px, double py, double angle) const 
{
  unsigned int ix, vsize = m_vx.size();
  if(vsize == 0)
    return(-1);
  
  if(vsize == 1)
    return(distPointToSeg(m_vx[0], m_vy[0], 
			  m_vx[0], m_vy[0], px,py,angle)); 
  
  if(vsize == 2)
    return(distPointToSeg(m_vx[0], m_vy[0], 
			  m_vx[1], m_vy[1], px,py,angle)); 
	   
  // Distance to poly is given by the shortest distance to any
  // one of the edges.
  double dist = -1;
  bool   first_hit = true;

  for(ix=0; ix<vsize; ix++) {

    double x1 = m_vx[ix];
    double y1 = m_vy[ix];
    
    int ixx = ix+1;
    if(ix == vsize-1)
      ixx = 0;
    
    double x2 = m_vx[ixx];
    double y2 = m_vy[ixx];

    double idist = distPointToSeg(x1,y1,x2,y2,px,py, angle); 
    if(idist != -1)
      if(first_hit || (idist < dist)) {
	dist = idist;
	first_hit = false;
      }
  }
  
  return(dist);
}

//---------------------------------------------------------------
// Procedure: seg_intercepts
//   Purpose: Return true if the given segment intercepts the 
//              polygon. Checks are made whether the segment crosses
//              any of the polygon edges. Intersection is also true
//              if the segment is entirely within the polygon.

bool XYPolygon::seg_intercepts(double x1, double y1, 
			       double x2, double y2) const
{
  unsigned int ix, vsize = m_vx.size();
  if(vsize == 0)
    return(false);

  double x3,y3,x4,y4;

  if(vsize == 1) {
    x3 = x4 = m_vx[0];
    y3 = y4 = m_vy[0];
    return(segmentsCross(x1,y1,x2,y2,x3,y3,x4,y4));
  }

  // Special case 2 vertices, otherwise the single edge will be checked
  // twice if handled by the general case.
  if(vsize == 2) {
    x3 = m_vx[0];
    y3 = m_vy[0];
    x4 = m_vx[1];
    y4 = m_vy[1];
    return(segmentsCross(x1,y1,x2,y2,x3,y3,x4,y4));
  }

  // Now handle the general case of more than two vertices
  // First check if one of the ends of the segment are contained
  // in the polygon

  if(contains(x1,y1) || contains(x2,y2))
    return(true);

  // Next check if the segment intersects any of the polgyon edges.
  for(ix=0; ix<vsize; ix++) {
    unsigned int ixx = ix+1;
    if(ix == vsize-1)
      ixx = 0;
    x3 = m_vx[ix];
    y3 = m_vy[ix];
    x4 = m_vx[ixx];
    y4 = m_vy[ixx];

    bool result = segmentsCross(x1,y1,x2,y2,x3,y3,x4,y4);
    if(result == true)
      return(true);
  }
  return(false);
}

//---------------------------------------------------------------
// Procedure: vertex_is_viewable
//   Purpose: Determine if the line segment given by the vertex ix, 
//            and the point x1,y1, intersects the polygon *only*
//            at the vertex. If so, we say that the vertex is 
//            "viewable" from the given point.
//      Note: We return false if the given point is contained in 
//            the polygon


bool XYPolygon::vertex_is_viewable(unsigned int ix, double x1, double y1) const
{
  unsigned int vsize = m_vx.size();
  if(vsize == 0)
    return(false);
  
  // Simple Range check
  if(ix >= vsize)
    return(false);

  // Special case, poly has one vertex, viewable from any point.
  if(vsize == 1)
    return(true);

  double x2,y2;
  x2 = m_vx[ix];
  y2 = m_vy[ix];

  // Special case, the query point and query vertex are the same
  if((x1==x2) && (y1==y2))
    return(true);

  // Special case, the query point is contained by (or *on*) the
  // polygon, return false. (except of course for the special case
  // handled above where the query vertex and point are the same)
  if(contains(x1,y1))
    return(false);

  // Special case, 2 vertices. Tricky since the general case does not
  // properly handle a point that lays on the line, but not the line
  // segment, given by the one edge in the polygon.
  if(vsize == 2) {
    int ixx = 0;  
    if(ix == 0)   // set index of the "other" vertex.
      ixx = 1;

    double x = m_vx[ixx];
    double y = m_vy[ixx];
    // if the other vertex point is on the query line segment, false
    if(segmentsCross(x1,y1,x2,y2,x,y,x,y))
      return(false);
    return(true);
  }

  // Now handle the general case of more than two vertices
  // Next check how many polygon edges intersect the query segment.
  // Answer should be at least two since the query segment will 
  // interesct the two edges that share the query vertex. 
  // If the query segment intersects more, it must have passed thru
  // the polygon, so we declare false.

  unsigned int i, count = 0;
  double x3, y3, x4, y4;
  for(i=0; ((i<vsize) && (count <= 2)); i++) {
    unsigned int j = i+1;
    if(i == vsize-1)
      j = 0;
    x3 = m_vx[i];
    y3 = m_vy[i];
    x4 = m_vx[j];
    y4 = m_vy[j];

    bool res = segmentsCross(x1,y1,x2,y2,x3,y3,x4,y4);
    if(res)
      count++;
   }

  if(count > 2)
    return(false);
  else
    return(true);
}

//---------------------------------------------------------------
// Procedure: side
//   Purpose: determines which "side" of the line given by x1,y1 
//            x2,y2 the point x3,y3 lies on. Returns either 0, 1, 2. 
//            0 indicates the point is below.
//            1 indicates the point is above.
//            2 indicates the point is on the line.
//            For vertical lines, we declare the right is "below".
//            If the given line is a point, we say the point is on the line
//
//             x            x        
//            /             |        
//       1   /  0         1 | 0        
//          /               |        
//         /                |        
//        x                 x          
//                            

int XYPolygon::side(double x1, double y1, double x2, double y2,
		    double x3, double y3) const
{
  // Handle special cases
  if(x1 == x2) {
    if(y1 == y2)
      return(2);
    else {
      if(x3 >  x1) return(0);
      if(x3 <  x1) return(1);
      if(x3 == x1) return(2);
    }
  }
    
  // Find the line equation y = mx + b
  double rise = y2 - y1;
  double run  = x2 - x1;
  double m    = rise / run;
  double b    = y2 - (m * x2);

  // calculate the value of y on the line for x3
  double y = (m * x3) + b;

  // Determine which side the point lies on
  if(y > y3)  
    return(0);
  else if(y < y3)  
    return(1);
  else  // if(y == y3) 
    return(2);
}

//---------------------------------------------------------------
// Procedure: set_side
//      Note: An edge given by index ix is the edge from ix to ix+1
//            When ix is the last vertex, the edge is from ix to 0
//      Note: If an edge in a polygon is valid, all other vertices
//            in the polygon on are the same side. This function
//            determines which side that is. 
//   Returns: 0 if all points are on the 0 side
//            1 if all points are on the 1 side
//            -1 if not all points are on the same side, OR, if all
//               points are *on* the segment.
//      Note: This function serves as more than a convexity test. 
//            The side of each each is an intermediate value that
//            is used in the dist_to_poly() and contains() function.

void XYPolygon::set_side(int ix)
{
  int vsize = m_vx.size();
  if((ix < 0) || (ix >= vsize))
    return;

  // Handle special cases
  if(vsize == 1) m_side_xy[0] = -1;
  if(vsize == 2) m_side_xy[1] = -1;
  if(vsize <= 2)
    return;

  double x1,y1,x2,y2,x3,y3;

  x1 = m_vx[ix];
  y1 = m_vy[ix];

  int ixx = ix+1;
  if(ix == vsize-1)
    ixx = 0;

  x2 = m_vx[ixx];
  y2 = m_vy[ixx];

  m_side_xy[ix] = -1;

  bool fresh = true;
  for(int j=0; j<vsize; j++) {
    if((j!=ix) && (j!=ixx)) {
      x3 = m_vx[j];
      y3 = m_vy[j];
      int iside = side(x1,y1,x2,y2,x3,y3);

      if(iside != 2) {
	if(fresh) {
	  m_side_xy[ix] = iside;
	  fresh = false;
	}
	else
	  if(iside != m_side_xy[ix]) {
	    m_side_xy[ix] = -1;
	  }
      }
    }
  }
}

//---------------------------------------------------------------
// Procedure: determine_convexity
//   Purpose: determine whether the object represents a convex 
//            polygon. We declare that a polygon is *not* convex
//            unless it contains at least three points. 
//            Most of the work is done by the calls to set_side().
//

void XYPolygon::determine_convexity()
{
  unsigned int i;
  for(i=0; i<size(); i++)
    set_side(i);

  m_convex_state = (size() >= 3);
  for(i=0; i<size(); i++)
    m_convex_state = m_convex_state && (m_side_xy[i] != -1);
}


//---------------------------------------------------------------
// Procedure: max_radius
//   Purpose: Determine the maximum distance between the center of the
//            polygon and any of its vertices.

double XYPolygon::max_radius() const
{
  double max_dist_so_far = 0;
  double cx = get_center_x();
  double cy = get_center_y();

  for(unsigned int i=0; i<m_vx.size(); i++) {
    double delta_x = cx = m_vx[i];
    double delta_y = cy = m_vy[i];
    double dist = hypot(delta_x, delta_y);
    if(dist > max_dist_so_far)
      max_dist_so_far = dist;
  }
  return(max_dist_so_far);
}

//---------------------------------------------------------------
// Procedure: closest_point_on_poly
//   Purpose: Determine the point on the polygon (on an edge or 
//            vertex) closest to the given point.
//   Returns: true if the polygon is convex, false otherwise

bool XYPolygon::closest_point_on_poly(double sx, double sy,
				      double& rx, double& ry) const
{
  // Sanity check - Calcualtions below assume poly is convex.
  if(!is_convex())
    return(false);

  // Sanity check - The size of the point vectors should be non-zero. Otherwise
  // the convexity test would have failed. But we set and check here again.
  unsigned int vsize = m_vx.size();
  if(vsize == 0)
    return(false);

  // Step 1: Get the center of the polygon
  double cx = get_center_x();
  double cy = get_center_y();

  // Sanity check - if the given point is the center of the poly
  // In this case the return point is just one of the vertices.
  if((sx==cx) && (sy==cy)) {
    rx = m_vx[0];
    ry = m_vy[0];
  }

  // Step 2: Verify our test segment. By default it is sx,sy,cx,cy.
  // But if sx,sy is inside the polygon, we have to extend it.
  if(contains(sx, sy)) {
    // Determine the max pseudo radius of the polygon
    double maximum_radius = max_radius();
    double ang = relAng(cx, cy, sx, sy);
    projectPoint(ang, maximum_radius*2, cx, cy, sx, sy);
  }

  // Step 3: Now that we have our test segment, find which poly segment
  // intersects and then find the intersection point.
  

  for(unsigned ix=0; ix<vsize; ix++) {
    unsigned int ixx = ix+1;
    if(ix == vsize-1)
      ixx = 0;
    double x3 = m_vx[ix];
    double y3 = m_vy[ix];
    double x4 = m_vx[ixx];
    double y4 = m_vy[ixx];

    bool segs_intersect = segmentsCross(cx,cy,sx,sy,x3,y3,x4,y4);
    if(segs_intersect) {
      linesCross(cx,cy,sx,sy,x3,y3,x4,y4,rx,ry);
      return(true);
    }
  }
  
  // Final sanity check. One of poly edges should have passed the intersection
  // test above and set the intersection point, and returned true. If not, 
  // false is returned here.
  return(false);
}


//---------------------------------------------------------------
// Procedure: exportSegList
//   Purpose: Build an XYSegList from the polygon. Make the first 
//            point in the XYSegList the point in the polygon
//            that is closest to the x,y point.

XYSegList XYPolygon::exportSegList(double x, double y)
{
  unsigned int start_index = 0;
  double shortest_dist = -1;

  unsigned int i, vsize = m_vx.size();
  for(i=0; i<vsize; i++) {
    double vx = m_vx[i];
    double vy = m_vy[i];
    double dist = hypot((x-vx), (y-vy));
    if((i==0) || (dist < shortest_dist)) {
      shortest_dist = dist;
      start_index = i;
    }
  }

  XYSegList new_segl;
  unsigned int count = 0;
  
  while(count < vsize) {
    unsigned int index = start_index + count;
    if(index >= vsize) 
      index -= vsize;
    new_segl.add_vertex(m_vx[index], m_vy[index]);
    count++;
  }

  return(new_segl);
}









