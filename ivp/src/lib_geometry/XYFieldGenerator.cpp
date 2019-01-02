/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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

using namespace std;

//---------------------------------------------------------
// Constructor

XYFieldGenerator::XYFieldGenerator()
{
  m_snap = 0.1;
}

//---------------------------------------------------------
// Procedure: addPolygon

bool XYFieldGenerator::addPolygon(string spec)
{
  XYPolygon poly = string2Poly(spec);
  
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
// Procedure: getPolygon

XYPolygon XYFieldGenerator::getPolygon(unsigned int ix)
{
  if(ix >= m_polygons.size()) {
    XYPolygon null_poly;
    return(null_poly);
  }

  return(m_polygons[ix]);
}
  

//---------------------------------------------------------
// Procedure: generatePoint

XYPoint XYFieldGenerator::generatePoint()
{
  XYPoint new_point;

  double xlen = m_poly_max_x - m_poly_min_x;
  double ylen = m_poly_max_y - m_poly_min_y;

  if((xlen <= 0) || (ylen <= 0))
    return(new_point);

  bool         contained = false;
  unsigned int max_tries = 1000;
  
  double rand_x, rand_y;
  
  while(!contained && (max_tries > 0)) {
    int    rand_int_x = rand() % 10000;
    int    rand_int_y = rand() % 10000;
    double rand_pct_x = (double)(rand_int_x) / 10000;
    double rand_pct_y = (double)(rand_int_y) / 10000;
    
    rand_x = snapToStep(m_poly_min_x + (rand_pct_x * xlen), m_snap);
    rand_y = snapToStep(m_poly_min_y + (rand_pct_y * ylen), m_snap);
    
    unsigned int j, jsize = m_polygons.size();
    for(j=0; j<jsize; j++) 
      if(m_polygons[j].contains(rand_x, rand_y))
	contained = true;
  }
  
  if(contained) {
    new_point.set_vx(rand_x);
    new_point.set_vy(rand_y);
  }
  return(new_point);
}
