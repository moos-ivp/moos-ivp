/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CoverEngine.cpp                                      */
/*    DATE: Nov 28th, 2025                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#include <cmath>
#include "CoverEngine.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "ConvexHullGenerator.h"

using namespace std;

//---------------------------------------------------------------
// Constructor()

CoverEngine::CoverEngine()
{
  m_collapse = true;
  m_verbose  = false;
}

//---------------------------------------------------------------
// Procedure: setPoints()

bool CoverEngine::setPoints(vector<double> vx, vector<double> vy)
{
  // Sanity check 
  if(vx.size() != vy.size())
    return(false);

  // Create an XYSegList from the points
  XYSegList segl;
  for(unsigned int i=0; i<vx.size(); i++) 
    segl.add_vertex(vx[i], vy[i]);

  return(setPoints(segl));
}

//---------------------------------------------------------------
// Procedure: setPoints(XYSegList)

bool CoverEngine::setPoints(XYSegList segl)
{
  if(segl.segs_cross())
    return(false);

  if(segl.is_clockwise())
    segl.reverse();

  clear();
  for(unsigned int i=0; i<segl.size(); i++) {
    m_vx.push_back(segl.get_vx(i));
    m_vy.push_back(segl.get_vy(i));
  }
  return(true);
}


//---------------------------------------------------------------
// Procedure: clear()

void CoverEngine::clear()
{
  m_vx.clear();
  m_vy.clear();
}

//---------------------------------------------------------------
// Procedure: getGenPoly()
  
XYGenPolygon CoverEngine::getGenPoly()
{
  //-------------------------------------------------
  // Part 1: Determine the cover polygons
  //-------------------------------------------------
  vector<XYPolygon> cover_polys = coverRecursive(m_vx, m_vy, 0);
    
  if(m_collapse)
    collapseNeighbors(cover_polys);

  //-------------------------------------------------
  // Part 2: Create the XYGenPolygon (segl + polys)
  //-------------------------------------------------
  // Part 2A: Create an XYSegList from the points
  XYSegList segl;
  for(unsigned int i=0; i<m_vx.size(); i++) 
    segl.add_vertex(m_vx[i], m_vy[i]);

  // Part 2B: Create the GenPoly with the two core components
  XYGenPolygon gpoly;
  gpoly.setGenPoly(segl, cover_polys);

  return(gpoly);
}


//---------------------------------------------------------------
// Procedure: coverRecursive()

vector<XYPolygon> CoverEngine::coverRecursive(vector<double> vx,
					      vector<double> vy,
					      int level)
{
  string indent(level, ' ');
  if(m_verbose) {
    cout << indent << "Vertices left: " << vx.size() << endl;
  }

  // Vector of Return Polys is initially empty
  vector<XYPolygon> cover_polys;
  if(level > 2000)
    return(cover_polys);


  if((vx.size() != vy.size()) || (vx.size() < 3)) {
    if(m_verbose)
      cout << indent << "END ++++++" << endl;
    return(cover_polys);
  }

  // Check if leaf node (given poly is convex)
  XYPolygon poly;
  for(unsigned int i=0; i<vx.size(); i++) 
    poly.add_vertex(vx[i], vy[i], false);

  poly.determine_convexity();
  if(poly.is_convex()) {
    poly.set_label(intToString(level));
    cover_polys.push_back(poly);
    if(m_verbose)
      cout << indent << "SOLVED: Last Poly Size: " << poly.size() << endl;
    return(cover_polys);
  }
    
  for(unsigned int i=0; i<vx.size(); i++) {

    if(!okTermIX(vx, vy, 2)) {
      shiftVertices(vx, vy);      
      continue;
    }

    vector<double> nvx = vx;
    vector<double> nvy = vy;
    
    for(unsigned int j=3; j<vx.size(); j++) {
      if(!okTermIX(vx, vy, j)) {
	XYPolygon carved_poly = carvePoly(nvx, nvy, j-1);
	carved_poly.set_label(intToString(level));
	cover_polys.push_back(carved_poly);
	vector<XYPolygon> remaining_polys = coverRecursive(nvx, nvy, level+1);
	for(unsigned int k=0; k<remaining_polys.size(); k++) {
	  XYPolygon rpoly = remaining_polys[k];
	  //  rpoly.set_label(intToString(level+k));
	  cover_polys.push_back(rpoly);
	}
	return(cover_polys);
      }
    }
  }  
  return(cover_polys);
}
  

//---------------------------------------------------------------
// Procedure: mergePolys()

XYPolygon CoverEngine::mergePolys(const XYPolygon& poly1,
				  const XYPolygon& poly2)
{
  XYPolygon null_poly;

  if(!poly1.is_convex() || !poly2.is_convex())
    return(null_poly);
  
  ConvexHullGenerator generator;
  for(unsigned int i=0; i<poly1.size(); i++)
    generator.addPoint(poly1.get_point(i));
  for(unsigned int i=0; i<poly2.size(); i++)
    generator.addPoint(poly2.get_point(i));

  XYPolygon hull_poly = generator.generateConvexHull();

  double hull_area  = hull_poly.area();
  double comb_area  = poly1.area() + poly2.area();

  hull_area = snapToStep(hull_area, 0.001);
  comb_area = snapToStep(comb_area, 0.001);

  
  if(hull_area == comb_area) {
    if(m_verbose)
      cout << "merge to hull!!!" << endl;
    return(hull_poly);
  }
 
  return(null_poly);
}


//---------------------------------------------------------------
// Procedure: collapseNeighors()

void CoverEngine::collapseNeighbors(vector<XYPolygon>& polys)
{
  if(polys.size() <= 1)
    return;

  for(unsigned int i=0; i<polys.size(); i++)
    polys[i].determine_convexity();
  
  bool done = false;
  while(!done) {
    bool merge_found = false;

    for(unsigned int i=0; i<polys.size()-1; i++) {
      for(unsigned int j=i+1; j<polys.size(); j++) {
	XYPolygon merged_poly = mergePolys(polys[i], polys[j]);

	if(merged_poly.size() != 0) {
	  polys[i] = merged_poly;
	  polys[j] = XYPolygon();
	  merge_found = true;
	}
      }
    }

    if(!merge_found)
      done = true;
    else {
      vector<XYPolygon> new_polys;
      for(unsigned int i=0; i<polys.size(); i++) {
	if(polys[i].size() != 0)
	  new_polys.push_back(polys[i]);
      }
      polys = new_polys;
    }
  }
}
    

//---------------------------------------------------------------
// Procedure: okTermIX()
//   Purpose: See if we can make a convex poly from the first few
//            points, given a vector of x-y points and an index ix.
//            The candidate poly will be at least 3 vertices, so ix
//            will be >= 2. If we can make a convex polygon from
//            vertices 0, 1, ..., ix, then simply return true. 
//            Also check to see if none of the later vertices are
//            contained within the convex poly created.
//
//                                                                  
//       v4                                                           
//        o-------------o v2
//        |           /                                              
//        |          /                                                  
//        |        /                                                  
//        o------o
//        v0     v1


bool CoverEngine::okTermIX(vector<double> vx, vector<double> vy, unsigned int ix)
{
  // Sanity check
  if(ix >= vx.size())
    return(false);
  
  // For initial 3 vertex poly, make sure it is a left turn
  if(ix == 2) {
    bool turns_left = threePointTurnLeft(vx[0],vy[0], vx[1],vy[1], vx[2],vy[2]);
    if(!turns_left) 
      return(false);
  }
  
  // Part 1: Build a poly from verts [0...ix], check for convexity
  XYPolygon poly;
  for(unsigned int i=0; i<=ix; i++)
    poly.add_vertex(vx[i], vy[i]);
   
  poly.determine_convexity();
  if(!poly.is_convex()) 
    return(false);

  // Part 2: Check that no remaining verts are within the poly
  for(unsigned int i=ix+1; i<vx.size(); i++)
    if(poly.contains(vx[i], vy[i]))
      return(false);
  
  return(true);
}

//---------------------------------------------------------------
// Procedure: carvePoly()
//
// Notes: 
// o Given a (possibly non-convex) polytong in the form of a
//   pair vectors. A vector of x coords, and one of y coords.
// o Before invoking, it has been determined that vertices
//   [0,ix] are to form a carved off poly (presumably convex)
// 
//                                                                  
//  v6       v5                                                 |
//  o-------o                                                   |    
//  |         \                                                 |
//  |           \                                               |
//  |             \ v4                                          |
//  |               o-------------o v2                          |
//  |               |           /                               |
//  |               |          /                                |
//  |               |        /                                  |
//  o---------------o------o                                    |
//  v7              v0     v1                                   |
//                                                              |
//         |                                                    |
//         V                                                    |
//                                                              |
//                                                              |
//  v3       v2                                                 |
//  o-------o                                                   |
//  |         \                                                 |
//  |           \                                               |
//  |             \  v1           v4                            |
//  |               o             o-------------o v2            |
//  |               |             |           /                 |
//  |               |             |          /                  |
//  |               |             |        /                    |
//  o---------------o             o------o                      |
//  v4              v0            v0     v1


XYPolygon CoverEngine::carvePoly(vector<double>& vx, vector<double>& vy, unsigned ix)
{
  XYPolygon null_poly;
  // Sanity check 1: Index needs to be in range
  if(ix >= vx.size())
    return(null_poly);
  
  // Create the carved off poly
  XYPolygon new_poly;
  for(unsigned int i=0; i<=ix; i++)
    new_poly.add_vertex(vx[i], vy[i]);

  // The remainder, still possibly non-convex poly
  vector<double> new_vx;
  vector<double> new_vy;
  new_vx.push_back(vx[0]);
  new_vy.push_back(vy[0]);
  new_vx.push_back(vx[ix]);
  new_vy.push_back(vy[ix]);
  
  for(unsigned int i=ix+1; i<vx.size(); i++) {
    new_vx.push_back(vx[i]);
    new_vy.push_back(vy[i]);
  }

  vx = new_vx;
  vy = new_vy;

  return(new_poly);
}
