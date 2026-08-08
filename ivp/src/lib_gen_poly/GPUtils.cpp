/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: GPUtils.cpp                                          */
/*    DATE: Aug 07, 2026                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#include "MBUtils.h"
#include "GPUtils.h"
#include "GeomUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: distSegInPoly()
//   Purpose: Return the distance of the segment contained in poly,
//      Note: If first vertex is on a poly edge, and does not cross 
//            another edge, the dist is zero.
//      Note: If first seg vertex is on a poly edge, and it will cross
//            at most one other edge. Return the dist to that edge.
//      Note: If first seg vertex not at all in poly, return zero.
//            If completely in poly, just the line seg length

double distSegInPoly(double x1, double y1, double x2, double y2,
		     const XYPolygon& poly)
{
  // Sanity Check
  if(!poly.is_convex())
    return(-1);

  bool p1_in_poly = poly.contains(x1,y1);
  bool p2_in_poly = poly.contains(x2,y2);

  // Case 1: Both vertices inside the poly,
  if(p1_in_poly && p2_in_poly)
    return(distPointToPoint(x1,y1, x2,y2));
  
  // Case 2: Both vertices outside the poly,
  // If both vertices of the segment are outside the poly, then
  // rely on the line_intersects function to determin what part,
  // if any, interseects the polygon. Use the entry/exit points
  // to determine the distance.
  if(!p1_in_poly && !p2_in_poly) {
    double ix1,iy1, ix2,iy2;
    if(poly.line_intersects(x1,y1, x2,y2, ix1,iy1, ix2,iy2))
      return(distPointToPoint(ix1,iy1, ix2,iy2));
    else
      return(0);
  }

  // Case 3: One but not both vertices are in the poly.
  // To simplify, swap if needed so p1 is in, and p2 is out.
  if(!p2_in_poly) {
    double tx = x1;
    double ty = y1;
    x1 = x2;
    y1 = y2;
    x2 = tx;
    y2 = ty;
  }
  
  // Check if the segment crosses any of the polygon segments
  double min_dist = -1;
  for(unsigned int i=0; i<poly.size(); i++) {
    // Segment first vertex
    double x3 = poly.get_vx(i);
    double y3 = poly.get_vy(i);
    // Segment second vertex
    double x4 = poly.get_vx(0);
    double y4 = poly.get_vy(0);
    if((i+1) < poly.size()) {
      x4 = poly.get_vx(i+1);
      y4 = poly.get_vy(i+1);
    }

    double ix = 0;
    double iy = 0;
    double kdist = distSegToSeg(x1,y1, x2,y2, x3,y3, x4,y4, ix,iy);

    // If there is an intersection (kdist > 0), we want the dist 
    // from x1,y1 to the edge intersection point, NOT the other vertex
    if(kdist > 0) {
      double dist = distPointToPoint(x1,y1, ix,iy);
      if((min_dist < 0) || (dist < min_dist)) {
	min_dist = dist;
      }
    }
  }
  return(min_dist);
}


//---------------------------------------------------------------
// Procedure: distRayInPoly()
//   Purpose: Return the distance of the ray contained in poly,

double distRayInPoly(double rx, double ry, double ray_angle,
		     const XYPolygon& poly)
{
  // Sanity Check
  if(!poly.is_convex())
    return(-1);

  // If root vertex is in poly
  if(poly.contains(rx,ry)) {
    for(unsigned int i=0; i<poly.size(); i++) {
      // Segment first vertex
      double x3 = poly.get_vx(i);
      double y3 = poly.get_vy(i);
      // Segment second vertex
      double x4 = poly.get_vx(0);
      double y4 = poly.get_vy(0);
      if((i+1) < poly.size()) {
	x4 = poly.get_vx(i+1);
	y4 = poly.get_vy(i+1);
      }
      double ix,iy;
      if(crossRaySeg(rx,ry,ray_angle, x3,y3,x4,y4, ix,iy))
	return(distPointToPoint(rx,ry, ix,iy));
    }
  }

  // If root vertex is not in poly
  else {
    // Calc 2nd vertex to use existing function using a line
    double rx2, ry2;
    projectPoint(ray_angle, 10, rx,ry, rx2,ry2);

    double ix1,iy1,ix2,iy2;
    bool res = poly.line_intersects(rx,ry,rx2,ry, ix1,iy1,ix2,iy2);
    if(res)
      return(distPointToPoint(ix1,iy2, ix2,iy2));
    else
      return(0);
  }    
    
  return(0);
}


//---------------------------------------------------------------
// Procedure: distSegListInPoly()
//   Purpose: Return dist/length of seglist contained in poly,

double distSegListInPoly(const XYSegList& segl, const XYPolygon& poly)
{
  // Sanity Check valid poly
  if(!poly.is_convex())
    return(-1);

  // Edge case check
  if(segl.size() < 2)
    return(0);

  double segl_dist = 0;
  for(unsigned int i=0; i<segl.size(); i++) {
    double x1 = segl.get_vx(i);
    double y1 = segl.get_vy(i);
    if((i+1) < segl.size()) {
      double x2 = segl.get_vx(i+1);
      double y2 = segl.get_vy(i+1);
      segl_dist += distSegInPoly(x1,y1, x2,y2, poly);
    }
  }

  return(segl_dist);
}

//---------------------------------------------------------------
// Procedure: distSeglrInPoly()
//   Purpose: Return the distance of the seglr contained in poly,

double distSeglrInPoly(const XYSeglr& seglr, const XYPolygon& poly)
{
  // Sanity Check valid poly and seglr
  if(!poly.is_convex() || (seglr.size() == 0))
    return(-1);

  // Part 1: Calculate the base seglist distance
  XYSegList segl = seglr.getBaseSegList();
  double segl_dist = distSegListInPoly(segl, poly);

  // Part 1: Calculate the base ray distance
  double rx = seglr.getRayBaseX();
  double ry = seglr.getRayBaseY();
  double ray_angle = seglr.getRayAngle();
  double ray_dist = distRayInPoly(rx,ry,ray_angle, poly);  

  double total_dist = segl_dist + ray_dist;

  return(total_dist);
}

