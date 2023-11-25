/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYFieldGenerator.cpp                                 */
/*    DATE: Jan 27th, 2012                                       */
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
#include <cstdlib>
#include "XYFieldGenerator.h"
#include "MBUtils.h"
#include "GeomUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

XYFieldGenerator::XYFieldGenerator()
{
  m_snap = 0.1;
  
  m_max_tries   = 5000;
  m_targ_amt    = 10;
  m_verbose     = false;
  m_flex_buffer = false;
  m_buffer_dist = 0;     
  
  srand(time(NULL));

}

//---------------------------------------------------------
// Procedure: addPolygon()

bool XYFieldGenerator::addPolygon(string spec)
{
  XYPolygon poly = string2Poly(spec);

  return(addPolygon(poly));
}

//---------------------------------------------------------
// Procedure: addPolygon()

bool XYFieldGenerator::addPolygon(XYPolygon poly)
{
  if(!poly.is_convex())
    return(false);
  
  m_polygons.push_back(poly);

  double this_poly_min_x = poly.get_min_x();
  double this_poly_min_y = poly.get_min_y();
  double this_poly_max_x = poly.get_max_x();
  double this_poly_max_y = poly.get_max_y();

  if(m_polygons.size() == 1) {
    m_poly_min_x = this_poly_min_x;
    m_poly_min_y = this_poly_min_y;
    m_poly_max_x = this_poly_max_x;
    m_poly_max_y = this_poly_max_y;
  }
  else {
    if(this_poly_min_x < m_poly_min_x)
      m_poly_min_x = this_poly_min_x;
    if(this_poly_min_y < m_poly_min_y)
      m_poly_min_y = this_poly_min_y;

    if(this_poly_max_x > m_poly_max_x)
      m_poly_max_x = this_poly_max_x;
    if(this_poly_max_y > m_poly_max_y)
      m_poly_max_y = this_poly_max_y;
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: setSnap()

bool XYFieldGenerator::setSnap(double snap_val)
{
  if(snap_val <= 0)
    return(false);

  m_snap = snap_val;
  return(true);
}
  
//---------------------------------------------------------
// Procedure: clear()

void XYFieldGenerator::clear()
{
  m_vpoints.clear();
}
  
//---------------------------------------------------------
// Procedure: getPolygon()

XYPolygon XYFieldGenerator::getPolygon(unsigned int ix)
{
  if(ix >= m_polygons.size()) {
    XYPolygon null_poly;
    return(null_poly);
  }

  return(m_polygons[ix]);
}
  

//---------------------------------------------------------
// Procedure: generatePoint()
//   Purpose: Generate a new XYPoint that resides one of the
//               field regions.
//            No check made here for buffer distance.
//            Could be used directly by the user of generator,
//               but is also used by generatePoints()

XYPoint XYFieldGenerator::generatePoint()
{
  XYPoint new_point;
  if(m_polygons.size() == 0)
    return(new_point);
  
  double xlen = m_poly_max_x - m_poly_min_x;
  double ylen = m_poly_max_y - m_poly_min_y;

  if((xlen <= 0) || (ylen <= 0))
    return(new_point);

  for(unsigned int i=0; i<m_max_tries; i++) {
    int    rand_int_x = rand() % 10000;
    int    rand_int_y = rand() % 10000;
    double rand_pct_x = (double)(rand_int_x) / 10000;
    double rand_pct_y = (double)(rand_int_y) / 10000;
    
    double rand_x, rand_y;
    rand_x = snapToStep(m_poly_min_x + (rand_pct_x * xlen), m_snap);
    rand_y = snapToStep(m_poly_min_y + (rand_pct_y * ylen), m_snap);

    if(isPointInRegion(rand_x, rand_y)) {
      new_point.set_vx(rand_x);
      new_point.set_vy(rand_y);
      return(new_point);
    }
  }
  
  // If failed after max_tries, return point at center of
  // first polygon.
  new_point.set_vx(m_polygons[0].get_center_x());
  new_point.set_vy(m_polygons[0].get_center_y());
  return(new_point);
}


//---------------------------------------------------------
// Procedure: addPoint(const XYPoint&)
//            Convenience function

bool XYFieldGenerator::addPoint(const XYPoint& point)
{
  return(addPoint(point.x(), point.y()));
}

//---------------------------------------------------------
// Procedure: addPoint(double vx, double vy)
//   Purpose: If point is in one of the field regions, add
//              it to the vector of points.
//            If point is too close to one of the other
//            points, search for nearby points that do not
//              violate the buffer distance requirement.
//   Returns: true if the point or a neighbor was added.
//            false otherwise.

bool XYFieldGenerator::addPoint(double vx, double vy)
{
  if(!isPointInRegion(vx, vy))
    return(false);

  XYPoint new_point(vx, vy);
  new_point.set_label("P" + uintToString(m_vpoints.size()));

  if(!isPointTooClose(new_point)) {
    m_nearest.clear();
    m_vpoints.push_back(new_point);
    return(true);
  }

  double adjustable_dist = m_buffer_dist + 1;

  for(unsigned int tries=0; tries<m_max_tries; tries++) {
    int rand_ang = rand() % 360;
    double tx = vx;
    double ty = vy;
    
    projectPoint(rand_ang, adjustable_dist, vx, vy, tx, ty); 

    if(isPointInRegion(tx, ty) && !isPointTooClose(tx, ty)) {
      new_point.set_vertex(tx, ty);
      m_vpoints.push_back(new_point);
      m_nearest.clear();
      return(true);
    }
    if((tries % 10) == 0)
      adjustable_dist += 1;
  }
  
  return(false);
}

//---------------------------------------------------------
// Procedure: generatePoints()

bool XYFieldGenerator::generatePoints(unsigned int amt)
{
  if(amt == 0)
    amt = m_targ_amt;

  if(m_verbose) {
    cout << "XYFieldGenerator::generatePoints()" << endl;
    cout << "Buffer_dist:" << m_buffer_dist << endl;
  }
  // Mode 1: Insist buffer distance must not be compromised.
  if(!m_flex_buffer)
    return(addAllRandomPoints(amt));

  // Mode 2: Keep lowering buffer distance until successful
  double orig_buffer_dist = m_buffer_dist;
  bool success = false;
  for(double i=1.0; ((i>0) && !success); i-=0.01) {
    m_buffer_dist *= i;
    success = addAllRandomPoints(amt);
    if(m_verbose) {
      cout << "buffer_dist: " << m_buffer_dist << endl;
      cout << "success:" << boolToString(success) << endl;
      cout << "created:" << uintToString(m_vpoints.size()) << endl;
    }
  }

  m_buffer_dist = orig_buffer_dist;
  return(success);
}
  
//---------------------------------------------------------
// Procedure: addAllRandomPoints()

bool XYFieldGenerator::addAllRandomPoints(unsigned int amt)
{
  m_vpoints.clear();
  bool ok = true;
  for(unsigned int i=0; ((i<amt) && ok); i++)
    ok = addRandomPoint();

  return(ok);
}
  

//---------------------------------------------------------
// Procedure: addRandomPoint()

bool XYFieldGenerator::addRandomPoint()
{
  m_nearest.clear();
  for(unsigned int i=0; i<m_max_tries; i++) {
    XYPoint new_point = generatePoint();
    if(!isPointTooClose(new_point)) {
      m_vpoints.push_back(new_point);
      return(true);
    }
  }
  return(false);
}
  

//---------------------------------------------------------
// Procedure: isPointTooClose()

bool XYFieldGenerator::isPointTooClose(const XYPoint& point)
{
  for(unsigned int i=0; i<m_vpoints.size(); i++) {
    if(distPointToPoint(point, m_vpoints[i]) < m_buffer_dist)
      return(true);
  }
  
  return(false);
}

//---------------------------------------------------------
// Procedure: isPointTooClose()

bool XYFieldGenerator::isPointTooClose(double vx, double vy)
{
  XYPoint point(vx, vy);
  for(unsigned int i=0; i<m_vpoints.size(); i++) {
    double dist = distPointToPoint(point, m_vpoints[i]);
    if(dist < m_buffer_dist)
      return(true);
  }
  
  return(false);
}

//---------------------------------------------------------
// Procedure: isPointInRegion()

bool XYFieldGenerator::isPointInRegion(const XYPoint& point)
{
  for(unsigned int j=0; j<m_polygons.size(); j++) 
    if(m_polygons[j].contains(point.x(), point.y()))
      return(true);
  
  return(false);
}

//---------------------------------------------------------
// Procedure: isPointInRegion()

bool XYFieldGenerator::isPointInRegion(double vx, double vy)
{
  for(unsigned int j=0; j<m_polygons.size(); j++) 
    if(m_polygons[j].contains(vx, vy))
      return(true);
  
  return(false);
}

//---------------------------------------------------------
// Procedure: getNewestPoint()

XYPoint XYFieldGenerator::getNewestPoint() const
{
  XYPoint null_pt;
  if(m_vpoints.size() == 0)
    return(null_pt);

  return(m_vpoints.back());
}

//---------------------------------------------------------
// Procedure: updateGlobalNearestVals()

void XYFieldGenerator::updateGlobalNearestVals(bool force)
{
  if(!force && (m_nearest.size() != 0))
    return;

  m_nearest.clear();
  m_global_nearest = -1;
  
  // Create an array of distances same size at points
  for(unsigned int i=0; i<m_vpoints.size(); i++) {
    double closest = -1;
    for(unsigned int j=0; j<m_vpoints.size(); j++) {
      if(i!=j) {
	double dist = distPointToPoint(m_vpoints[i], m_vpoints[j]);
	if((closest == -1) || (dist < closest))
	  closest = dist;
      }
    }
    m_nearest.push_back(closest);

    if((m_global_nearest < 0) || (closest < m_global_nearest))
      m_global_nearest = closest;
  }
}

//---------------------------------------------------------
// Procedure: getNearestVals()

vector<double> XYFieldGenerator::getNearestVals(bool force)
{
  updateGlobalNearestVals(force);
  return(m_nearest);
}

//---------------------------------------------------------
// Procedure: getGlobalNearest()

double XYFieldGenerator::getGlobalNearest(bool force)
{
  updateGlobalNearestVals(force);
  return(m_global_nearest);
}


