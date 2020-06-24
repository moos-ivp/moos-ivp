/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYPolyExpander.cpp                                   */
/*    DATE: Sep 7th, 2019                                        */
/*    DATE: Jun 8th  2020 revisions and centroid bug fix         */
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
#include "XYPolyExpander.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: Constructor

XYPolyExpander::XYPolyExpander()
{
  // Buffer dist between original and buff poly. By default same.
  m_buff = 0;

  // degree delta as circular corners are approximated
  m_deg_delta = 5;

  // If non-zero, thresh is applied to incoming points. New points
  // nearly on top of an existing point will be dropped.
  m_vertex_proximity_thresh = 0;

  // If true, the polygon cross-product settling algorithm will
  // be applied to remove nearly colinear points if a non-convex
  // polygon is produced due to rounding errors.
  m_settling_enabled = true;
}

//---------------------------------------------------------------
// Procedure: setDegreeDelta()
//      Note: This param sets the change in angle as new points are
//            created around corners.

void XYPolyExpander::setDegreeDelta(double delta)
{
  // Apply some reasonable bounds. Delta too low results in poly with
  // excessive number of vertices. Delta too high results in poly with
  // crudely uneven buffer distances from the original poly.
  if(delta < 0.2)
    delta = 0.2;
  if(delta > 45)
    delta = 45;

  m_deg_delta = delta;
}

//---------------------------------------------------------------
// Procedure: setVertexProximityThresh()
//      Note: The vertex_proximity_thresh is applied to new points
//            added to the expander to reject points that are
//            effectively identical. By default this is off.

void XYPolyExpander::setVertexProximityThresh(double thresh)
{
  // Set reasonable bounds. Probably no right answer but a thresh
  // of  more than 10 meters apart seems unreasonable. 
  if(thresh > 10)
    thresh = 10;
  
  m_vertex_proximity_thresh = thresh;
}

//---------------------------------------------------------------
// Procedure: getBufferPoly()
//      Note: The main function call for this class

XYPolygon XYPolyExpander::getBufferPoly(double buff)
{
  clear();

  XYPolygon null_poly;
  if(!m_poly_orig.is_convex())
    return(null_poly);

  // By default the buffer poly is just the original
  m_poly_buff = m_poly_orig;
  m_buff = buff;

  bool ok = expandSegments();
  if(ok)
    ok = buildCorners();
  if(ok)
    ok = buildNewPoly();
  
  return(m_poly_buff);
}

//---------------------------------------------------------------
// Procedure: setPoly()

bool XYPolyExpander::setPoly(XYPolygon poly)
{
  // Sanity check
  if(!poly.is_convex())
    return(false);
  
  m_poly_orig = poly;
  return(true);
}


//---------------------------------------------------------------
// Procedure: expandSegments()

bool XYPolyExpander::expandSegments()
{
  // Sanity check for convexity (also implies num vertices > 2)
  if(!m_poly_orig.is_convex())
    return(false);

  double cx = m_poly_orig.get_centroid_x();
  double cy = m_poly_orig.get_centroid_y();
  
  vector<double> px;
  vector<double> py;
  
  for(unsigned int i=0; i<m_poly_orig.size(); i++) {
    // Part 1: Get next two vertices
    double vx1 = m_poly_orig.get_vx(i);
    double vy1 = m_poly_orig.get_vy(i);
    double vx2 = 0;
    double vy2 = 0;
    if((i+1) < m_poly_orig.size()) {
      vx2 = m_poly_orig.get_vx(i+1);
      vy2 = m_poly_orig.get_vy(i+1);
    }
    else {
      vx2 = m_poly_orig.get_vx(0);
      vy2 = m_poly_orig.get_vy(0);
    }

    // Part 2: Determine the expand angle. Perpendicular to 
    // the edge, but must determine which angle is "out".
    double bng_from_1to2 = relAng(vx1,vy1, vx2,vy2);
    double bng_from_1toC = relAng(vx1,vy1, cx,cy);
    // The two possible choices are bng1 and bng2
    double bng1 = angle360(bng_from_1to2 - 90);
    double bng2 = angle360(bng_from_1to2 + 90);
    // The right one is the one that has the biggest delta angle
    // compared to the angle from v1 to the center
    double delta1 = angleDiff(bng1, bng_from_1toC);
    double delta2 = angleDiff(bng2, bng_from_1toC);
    double bng_exp = bng1;
    if(delta1 < delta2)
      bng_exp = bng2;


    // Part 3: Expand the vertices
    double nx1,ny1;
    projectPoint(bng_exp, m_buff, vx1,vy1, nx1,ny1);
    px.push_back(nx1);
    py.push_back(ny1);
    double nx2,ny2;
    projectPoint(bng_exp, m_buff, vx2,vy2, nx2,ny2);
    px.push_back(nx2);
    py.push_back(ny2);    
  }

  // Part 4: Shift the px,py vectors one step, so each step of two
  // through the vector represents the two expansion points for each
  // vertex in the original poly.
  unsigned int psize = px.size();
  vector<double> npx;
  vector<double> npy;
  npx.push_back(px[psize-1]);
  npy.push_back(py[psize-1]);
  for(unsigned int k=0; k<psize-1; k++) {
    npx.push_back(px[k]);
    npy.push_back(py[k]);
  }

  // Part 5: Create a x/y vector same size as number of vertices
  // where each index contains the two expanded vertex points in
  // the right order.
  for(unsigned int k=0; k<psize; k=k+2) {
    m_px1.push_back(npx[k]);
    m_py1.push_back(npy[k]);
    m_px2.push_back(npx[k+1]);
    m_py2.push_back(npy[k+1]);
  }

  return(true);
}

//---------------------------------------------------------------
// Procedure: buildCorners()

bool XYPolyExpander::buildCorners()
{
  // Sanity check
  unsigned int psize = m_px1.size();
  if(psize == 0)
    return(false);

  // Part 2: Build the corners
  bool clockwise = m_poly_orig.is_clockwise();

  for(unsigned int i=0; i<m_px1.size(); i++) {
    double vx = m_poly_orig.get_vx(i);
    double vy = m_poly_orig.get_vy(i);
        
    double ang1 = relAng(vx,vy, m_px1[i], m_py1[i]);
    double ang2 = relAng(vx,vy, m_px2[i], m_py2[i]);

    double delta = angleDiff(ang1, ang2);
    double dsteps = (delta / m_deg_delta);
    if(dsteps < 0)
      dsteps = 0;
    unsigned int steps = (unsigned int)(dsteps);

    vector<double> corners_nx;
    vector<double> corners_ny;
    m_ipx.push_back(corners_nx);
    m_ipy.push_back(corners_ny);
    if(clockwise) {
      for(unsigned int j=1; j<steps; j++) {
	double angx = angle360(ang1 + ((double)(j))*(m_deg_delta));
	double nx,ny;
	projectPoint(angx, m_buff, vx,vy, nx,ny);
	m_ipx[i].push_back(nx);
	m_ipy[i].push_back(ny);
      }
    }
    else {
      for(unsigned int j=1; j<steps; j++) {
	double angx = angle360(ang1 - ((double)(j))*(m_deg_delta));
	double nx,ny;
	projectPoint(angx, m_buff, vx,vy, nx,ny);
	m_ipx[i].push_back(nx);
	m_ipy[i].push_back(ny);
      }
    }
  }

  return(true);
}

//---------------------------------------------------------------
// Procedure: buildNewPoly() 
//   Purpose: Given (a) expanded edges, and (b) points representing
//            rounded corners, stitch them together for single poly
 
bool XYPolyExpander::buildNewPoly()
{
  XYPolygon new_poly;

  for(unsigned int i=0; i<m_px1.size(); i++) {
    // Arg false means dont check for convexity on each addition.    
    new_poly.add_vertex(m_px1[i], m_py1[i], false);
    for(unsigned int j=0; j<m_ipx[i].size(); j++) {
      new_poly.add_vertex(m_ipx[i][j], m_ipy[i][j]);
    }
    new_poly.add_vertex_delta(m_px2[i], m_py2[i], 1, false);
  }

  new_poly.determine_convexity();

  // If non-convex, make an attempt to adjust theh poly by removing
  // the most colinear points until the poly is convex
  if(!new_poly.is_convex() && m_settling_enabled)
    new_poly = new_poly.crossProductSettle();

  // If the poly is still convex, return while retaining the
  // original unexpanded poly as the expanded poly.
  if(!new_poly.is_convex())
    return(false);

  m_poly_buff = new_poly;
  return(true);
}


//---------------------------------------------------------------
// Procedure: clear()

void XYPolyExpander::clear()
{
  m_px1.clear();
  m_py1.clear();
  m_px2.clear();
  m_py2.clear();

  m_ipx.clear();
  m_ipy.clear();
}


