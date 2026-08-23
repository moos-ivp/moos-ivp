/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYGenPolygon.cpp                                     */
/*    DATE: Oct 16th, 2020                                       */
/*    DATE: Nov 27th, 2025                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#include "XYFormatUtilsPoly.h"
#include "XYGenPolygon.h"
#include "MBUtils.h"
#include "GeomUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: setGenPoly()

bool XYGenPolygon::setGenPoly(XYSegList segl)
{
  // Ensure this sequences of vertices can actually produce a poly
  // No segment crosses another. Segment from V0 to V(n-1) also
  // does not cross any other vertex.
  if(segl.size() < 3)
    return(false);

  if(segl.segs_cross(true))
    return(false);
  
  m_segl_border = segl;
  m_cover_polys.clear();

  return(true);
}

//---------------------------------------------------------------
// Procedure: setGenPoly()

bool XYGenPolygon::setGenPoly(XYSegList segl, vector<XYPolygon> polys)
{
  // Ensure this sequences of vertices can actually produce a poly
  if(segl.size() < 3)
    return(false);

  if(segl.segs_cross(true))
    return(false);

  m_segl_border = segl;
  m_cover_polys = polys;
  return(true);
}

//---------------------------------------------------------------
// Procedure: contains()

bool XYGenPolygon::contains(double x, double y) const
{
  for(unsigned int i=0; i<m_cover_polys.size(); i++) {
    if(m_cover_polys[i].contains(x, y))
      return(true);
  }

  return(false);
}

//---------------------------------------------------------------
// Procedure: line_intersects()
//      Note: Individual polygons are checked. Disregard where line
//            may intersect. Unlike a convex polygon, a non-convex
//            poly may have an arbitrary number of intersection pts.
//      Note: We only care here about the Boolean return value.

bool XYGenPolygon::line_intersects(double x1, double y1,
				   double x2, double y2) const
{
  double rx1,ry1, rx2,ry2;
  
  for(unsigned int i=0; i<m_cover_polys.size(); i++) {
    if(m_cover_polys[i].line_intersects(x1,y1,x2,y2,rx1,ry1,rx2,ry2))
      return(true);
  }
  
  return(false);
}

//---------------------------------------------------------------
// Procedure: dist_to_poly()

double XYGenPolygon::dist_to_poly(double px, double py) const
{
  double min_dist = -1;

  for(unsigned int i=0; i<m_cover_polys.size(); i++) {
    double dist = m_cover_polys[i].dist_to_poly(px, py);
    if((i==0) || (dist < min_dist))
      min_dist = dist;
  }

  return(min_dist);
}

//---------------------------------------------------------------
// Procedure: dist_to_poly()
//      Note: Determine the distance between the line segment given
//            by x3,y3,x4,y4 to the polygon. An edge-by-edge check
//            is performed and the minimum returned.

double XYGenPolygon::dist_to_poly(double x3, double y3, 
				  double x4, double y4) const
{
  double min_dist = -1;
  
  for(unsigned int i=0; i<m_cover_polys.size(); i++) {
    double dist = m_cover_polys[i].dist_to_poly(x3,y3, x4,y4);
    if((i==0) || (dist < min_dist))
      min_dist = dist;
  }
  
  return(min_dist);
}

//---------------------------------------------------------------
// Procedure: shift_vert()

void XYGenPolygon::shift_vert(double amt)
{
  m_segl_border.shift_vert(amt);
  
  for(unsigned int i=0; i<m_cover_polys.size(); i++)
    m_cover_polys[i].shift_vert(amt);
}

//---------------------------------------------------------------
// Procedure: shift_horz()

void XYGenPolygon::shift_horz(double amt)
{
  m_segl_border.shift_horz(amt);

  for(unsigned int i=0; i<m_cover_polys.size(); i++)
    m_cover_polys[i].shift_horz(amt);
}


//---------------------------------------------------------------
// Procedure: area()

double XYGenPolygon::area() const
{
  double area = 0;
  for(unsigned int i=0; i<m_cover_polys.size(); i++)
    area += m_cover_polys[i].area();

  return(area);
}

//---------------------------------------------------------------
// Procedure: clear()

void XYGenPolygon::clear() 
{
  m_segl_border.clear();
  m_cover_polys.clear();
}


//---------------------------------------------------------------
// Procedure: clearCoverPolys()

void XYGenPolygon::clearCoverPolys() 
{
  m_cover_polys.clear();
}


//---------------------------------------------------------------
// Procedure: get_spec()
//   Example: border={},fill_color=red,duration=10 #
//            poly=(),fill_color=blue,vertex_size=2 #
//            poly=(),fill_color=blue,vertex_size=2 # ...


string XYGenPolygon::get_spec() const
{
  string spec = get_spec_bdr();

  for(unsigned int i=0; i<m_cover_polys.size(); i++) {
    string poly_spec = m_cover_polys[i].get_spec();
    poly_spec = findReplace(poly_spec, "pts=", "poly=");
    spec += " # " + poly_spec;
  }
  
  return(spec);
}

//---------------------------------------------------------------
// Procedure: get_spec_bdr()
//   Example: pts={},fill_color=red,duration=10,

string XYGenPolygon::get_spec_bdr() const
{
  string spec = m_segl_border.get_spec();
  spec = findReplace(spec, "pts=", "border=");
  return(spec);
}

//---------------------------------------------------------------
// Procedure: stringToGenPoly()
//   Example: border={},vertex_color=blue,vertex_size=2,edge_color=red #
//            poly={},fill_color=blue,vertex_size=2 #
//            poly={},fill_color=blue,vertex_size=2 #
//      Note: Vertices of the form: 3,4:8,9:0,0 


XYGenPolygon stringToGenPoly(string full_str)
{
  XYGenPolygon null_gpoly;
  XYGenPolygon gpoly;

  cout << "full_str: [" << full_str << "]" << endl;
  
  // Sanity check 1: Must begin with border={
  string part1 = biteStringX(full_str, '#');
  if(!strBegins(part1, "border={"))
    return(null_gpoly);


  biteString(part1, '{');
  string pts_str = biteString(part1, '}');

  // Part 1A: Get the SegList (border) points
  XYSegList segl;
  
  vector<string> svector = parseString(pts_str, ':');
  for(unsigned int i=0; i<svector.size(); i++) {
    string vertex = stripBlankEnds(svector[i]);
    string xstr = biteStringX(vertex, ',');
    string ystr = biteStringX(vertex, ',');
    if(!isNumber(xstr) || !isNumber(ystr))
      return(null_gpoly);
    double xval = atof(xstr.c_str());
    double yval = atof(ystr.c_str());
    segl.add_vertex(xval, yval);
  }

  if(segl.size() == 0)
    return(null_gpoly);
  
  // Part 1B: Get the other SegList properties
  svector = parseString(part1, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    segl.set_param(param, value);
  }

  // Part 2: Read any cover polys
  vector<XYPolygon> cover_polys;

  svector = parseString(full_str, '#');
  for(unsigned int i=0; i<svector.size(); i++) {
    string poly_str = stripBlankEnds(svector[i]);
    poly_str = findReplace(poly_str, "poly=", "pts=");
    cout << "poly_str:[" << poly_str << "]" << endl;
    if(poly_str != "") {
      XYPolygon poly = string2Poly(poly_str);
      if(poly.size() == 0)
	return(null_gpoly);
      cover_polys.push_back(poly);
    }
  }

  if(cover_polys.size() == 0)
    return(null_gpoly);
      
  gpoly.setGenPoly(segl, cover_polys);
  
  return(gpoly);
}

//---------------------------------------------------------------
// Procedure: distSegToExitGP()

double XYGenPolygon::distPtToExitGP(double px, double py) const
{
  // Assumption test
  if(!contains(px,py))
    return(0);  

  if(m_segl_border.size() < 3)
    return(0);
  
  double min_dist = -1;
  for(unsigned int i=0; i<m_segl_border.size(); i++) {
    // get first vertex of the ith edge
    double x1 = m_segl_border.get_vx(i);
    double y1 = m_segl_border.get_vy(i);
    // get second vertex of the ith edge
    double x2 = m_segl_border.get_vx(0);
    double y2 = m_segl_border.get_vy(0);
    if((i+1) < m_segl_border.size()) {
      x2 = m_segl_border.get_vx(i+1);
      y2 = m_segl_border.get_vy(i+1);
    }

    // determine if it crosses that edge and where
    double dist = distPointToSeg(x1,y1,x2,y2, px,py);
    if((min_dist < 0) || (dist < min_dist))
      min_dist = dist;
  }
  
  return(min_dist);
}

//---------------------------------------------------------------
// Procedure: distSegToExitGP()
//   Purpose: Calculate the length of the line segment x1,y1,x2,y2
//            that is in the genpoly, starting from x1,y1.
//      Note: Assumes first vertex (x1,y1) is in the genpoly. 

double XYGenPolygon::distSegToExitGP(double x1, double y1,
				     double x2, double y2,
				     bool& exited) const
{
  // We set exited=true unless both vertices are within the genpoly
  exited = true;

  // Assumption test
  if(!contains(x1,y1))
    return(0);  

  if(m_segl_border.size() < 3)
    return(0);
  
  // Special case 
  if(contains(x2,y2)) {
    exited = false;
    return(distPointToPoint(x1,y1,x2,y2));
  }

  double min_dist = -1;
  for(unsigned int i=0; i<m_segl_border.size(); i++) {
    // get first vertex of the ith edge
    double x3 = m_segl_border.get_vx(i);
    double y3 = m_segl_border.get_vy(i);
    // get second vertex of the ith edge
    double x4 = m_segl_border.get_vx(0);
    double y4 = m_segl_border.get_vy(0);
    if((i+1) < m_segl_border.size()) {
      x4 = m_segl_border.get_vx(i+1);
      y4 = m_segl_border.get_vy(i+1);
    }

    // determine if it crosses that edge and where
    double ix, iy;
    bool cross = segmentsCross(x1,y1,x2,y2, x3,y3,x4,y4, ix,iy);
    if(cross) {
      double dist = distPointToPoint(x1,y1, ix,iy);
      if((min_dist < 0) || (dist < min_dist))
	min_dist = dist;
    }
  }
  
  return(min_dist);
}

//---------------------------------------------------------------
// Procedure: distSeglToExitGP()
//   Purpose: Calculate the length of the segl that is in the
//            genpoly, starting from the first vertex of the segl.

double XYGenPolygon::distSeglToExitGP(const XYSegList& segl,
				      bool& exited) const
{
  // Sanity check proper segl
  if(!segl.valid()) 
    return(-3);

  double total_dist = 0;

  // Part 1: Calculate base seglist distance
  for(unsigned int i=0; (i<segl.size() && !exited); i++) {
    double x1 = segl.get_vx(i);
    double y1 = segl.get_vy(i);
    if((i+1) < segl.size()) {
      double x2 = segl.get_vx(i+1);
      double y2 = segl.get_vy(i+1);
      double dist = distSegToExitGP(x1,y1,x2,y2, exited);
      total_dist += dist;
    }
  }
  return(total_dist);
}

//---------------------------------------------------------------
// Procedure: distRayToExitGP()
//   Purpose: Calculate the length of the ray, rx,ry,ray_angle
//            that is in the genpoly, starting from rx,ry.
//      Note: Assumes first ray base (rx,ry) is in the genpoly. 

double XYGenPolygon::distRayToExitGP(double rx, double ry,
				     double ray_angle) const
{
  // Assumptions test
  if(!contains(rx,ry))
    return(0);  
  if(m_segl_border.size() < 3)
    return(0);
  
  double min_dist = -1;
  for(unsigned int i=0; i<m_segl_border.size(); i++) {
    // get first vertex of the ith edge
    double x1 = m_segl_border.get_vx(i);
    double y1 = m_segl_border.get_vy(i);
    // get second vertex of the ith edge
    double x2 = m_segl_border.get_vx(0);
    double y2 = m_segl_border.get_vy(0);
    if((i+1) < m_segl_border.size()) {
      x2 = m_segl_border.get_vx(i+1);
      y2 = m_segl_border.get_vy(i+1);
    }

    // determine if it crosses that edge and where
    double dist = distPointToSeg(x1,y1,x2,y2, rx,ry,ray_angle);
    if(dist > 0) {
      if((min_dist < 0) || (dist < min_dist))
	min_dist = dist;
    }
  }
  
  return(min_dist);
}


//---------------------------------------------------------------
// Procedure: distSeglrToExitGP()
//   Purpose: Calculate the length of the seglr that is in the
//            genpoly, starting from the first vertex of the seglr.

double XYGenPolygon::distSeglrToExitGP(const XYSeglr& seglr) const
{
  // Sanity check proper seglr
  if(!seglr.valid()) 
    return(-2);

  // Part 1: Calculate base seglist distance
  bool exited = false;
  XYSegList segl = seglr.getBaseSegList();
  double total_dist = distSeglToExitGP(segl, exited);

  // If the some portion of the segl left the genpoly, then
  // the ray distance is ignored
  if(exited)
    return(total_dist);
  
  // Part 2: Calculate ray distance
  double rx = seglr.getRayBaseX();
  double ry = seglr.getRayBaseY();
  double ray_angle = seglr.getRayAngle();
  double ray_dist = distRayToExitGP(rx,ry,ray_angle);
  if(ray_dist >= 0)
    total_dist += ray_dist;
  
  return(total_dist);
}

//---------------------------------------------------------------
// Procedure: distPtToEnterGP()
//   Purpose: Calculate the distance of the given point to the
//            closest line segment.

double XYGenPolygon::distPtToEnterGP(double px, double py) const
{
  // Edge cases
  if(contains(px,py))
    return(0);  
  if(m_segl_border.size() == 0)
    return(0);
  if(m_segl_border.size() == 1) {
    double sx = m_segl_border.get_vx(0);
    double sy = m_segl_border.get_vy(0);
    double dist = distPointToPoint(px,py, sx,sy); 
    return(dist);
  }
  
  double min_dist = -1;
  for(unsigned int i=0; i<m_segl_border.size(); i++) {
    // get first vertex of the ith edge
    double x1 = m_segl_border.get_vx(i);
    double y1 = m_segl_border.get_vy(i);
    // get second vertex of the ith edge
    double x2 = m_segl_border.get_vx(0);
    double y2 = m_segl_border.get_vy(0);
    if((i+1) < m_segl_border.size()) {
      x2 = m_segl_border.get_vx(i+1);
      y2 = m_segl_border.get_vy(i+1);
    }

    // determine if it crosses that edge
    double dist = distPointToSeg(x1,y1,x2,y2, px,py);
    if(dist > 0) {
      if((min_dist < 0) || (dist < min_dist))
	min_dist = dist;
    }
  }
  
  return(min_dist);
}

//---------------------------------------------------------------
// Procedure: distRayToEnterGP()
//   Purpose: Calculate the distance of the given point to the
//            closest line segment, AT THE GIVEN ANGLE.
//      Note: Unlike the cousin of this function without the angle
//            argument, this function may actually return -1, if 
//            ray does intersect the gpoly anywhere.

double XYGenPolygon::distRayToEnterGP(double px, double py,
				      double ray_angle) const
{
  // Edge cases
  if(contains(px,py))
    return(0);  
  if(m_segl_border.size() < 2)
    return(-1);
  
  double min_dist = -1;
  for(unsigned int i=0; i<m_segl_border.size(); i++) {
    // get first vertex of the ith edge
    double x1 = m_segl_border.get_vx(i);
    double y1 = m_segl_border.get_vy(i);
    // get second vertex of the ith edge
    double x2 = m_segl_border.get_vx(0);
    double y2 = m_segl_border.get_vy(0);
    if((i+1) < m_segl_border.size()) {
      x2 = m_segl_border.get_vx(i+1);
      y2 = m_segl_border.get_vy(i+1);
    }

    // determine if it crosses that edge
    double dist = distPointToSeg(x1,y1,x2,y2, px,py, ray_angle);
    if(dist > 0) {
      if((min_dist < 0) || (dist < min_dist))
	min_dist = dist;
    }
  }
  
  return(min_dist);
}


//---------------------------------------------------------------
// Procedure: cpaSegToGP()
//   Purpose: Calculate the closest point of approach (CPA) from 
//            the given line segment to the border of the GP,
//            by calculating the CPA to each edge on the GP and
//            taking the min


double XYGenPolygon::cpaSegToGP(double x1, double y1,
				double x2, double y2) const
{
  if(m_segl_border.size() < 2)
    return(-1);
  
  double min_dist = -1;
  for(unsigned int i=0; i<m_segl_border.size(); i++) {

    // get first vertex of the ith edge
    double x3 = m_segl_border.get_vx(i);
    double y3 = m_segl_border.get_vy(i);

    // get second vertex of the ith edge
    double x4 = m_segl_border.get_vx(0);
    double y4 = m_segl_border.get_vy(0);
    if((i+1) < m_segl_border.size()) {
      x4 = m_segl_border.get_vx(i+1);
      y4 = m_segl_border.get_vy(i+1);
    }

    // determine if it crosses that edge
    double dist = distSegToSeg(x1,y1,x2,y2, x3,y3,x4,y4);
    if(dist > 0) {
      if((min_dist < 0) || (dist < min_dist))
	min_dist = dist;
    }
  }
  
  return(min_dist);
}


//---------------------------------------------------------------
// Procedure: cpaSeglToGP()
//   Purpose: Calculate the closest point of approach (CPA) from 
//            the given SegList to the border of the GP, by
//            calculating the CPA to each edge on the GP from and
//            each edge in the SegList and then taking the min.


double XYGenPolygon::cpaSeglToGP(const XYSegList& segl,
				 bool verbose) const
{
  if(m_segl_border.size() < 2)
    return(-1);

  // Sanity check
  if(segl.size() == 0)
    return(-1);

  // Edge case
  if(segl.size() == 1) {
    double x1 = segl.get_vx(0);
    double y1 = segl.get_vy(0);
    double x2 = segl.get_vx(0);
    double y2 = segl.get_vy(0);
    double dist = cpaSegToGP(x1,y1,x2,y2);
    return(dist);
  }
  
  double min_dist = -1;
  for(unsigned int i=0; i<segl.size()-1; i++) {
    // get segment
    double x1 = segl.get_vx(i);
    double y1 = segl.get_vy(i);
    double x2 = segl.get_vx(i+1);
    double y2 = segl.get_vy(i+1);

    // determine dist of seg to (all edges of) GenPoly
    double dist = cpaSegToGP(x1,y1,x2,y2);
    if(verbose)
      cout << "dist[" << i << "]:" << doubleToString(dist) << endl;
    if(dist > 0) {
      if((min_dist < 0) || (dist < min_dist))
	min_dist = dist;
    }
  }
  
  return(min_dist);
}

//---------------------------------------------------------------
// Procedure: cpaSeglrToGP()
//   Purpose: Calculate the closest point of approach (CPA) from 
//            the given Seglr to the border of the GP, for a given
//            seglr length/distance. By setting the distance, this
//            defines seglist portion of the seglr.
//            Calculate CPA for each edge of the seglist to each
//            edge on the GP, and then taking the min.


double XYGenPolygon::cpaSeglrToGP(const XYSeglr& seglr,
				  double dist,
				  bool verbose) const
{
  if(m_segl_border.size() < 2)
    return(-1);

  // Get the portion of the seglr out to length=dist
  XYSegList segl = getDistSegList(seglr, dist);

  if(verbose)
    cout << " gp: segl: " << segl.get_spec() << endl;

  return(cpaSeglToGP(segl));
}

