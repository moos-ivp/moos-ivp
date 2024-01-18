/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ObstacleFieldGenerator.cpp                           */
/*    DATE: Oct 18th 2017                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include <cmath>
#include "ObstacleFieldGenerator.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "XYFormatUtilsPoly.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

ObstacleFieldGenerator::ObstacleFieldGenerator()
{
  m_min_poly_size = 6;
  m_max_poly_size = 10;
  m_min_range = 2;
  m_precision = 0.1;    // vertices chosen at 1/10th meter
  m_amount    = 1;
  m_begin_id  = 0;

  m_poly_vertices = 8;
  m_gen_tries = 10000; // Max rand attempts to find poly fit
  
  m_verbose = true;
}

//---------------------------------------------------------
// Procedure: setPolygon()

bool ObstacleFieldGenerator::setPolygon(string str)
{
  XYPolygon poly = string2Poly(str);
  return(setPolygon(poly));
}

//---------------------------------------------------------
// Procedure: setPolygon()

bool ObstacleFieldGenerator::setPolygon(XYPolygon poly)
{
  if(!poly.is_convex()) {
    cout << "Warning: non-convex polygon." << endl;
    return(false);
  }

  m_poly_region = poly;
  return(true);
}

//---------------------------------------------------------
// Procedure: setAmount()

bool ObstacleFieldGenerator::setAmount(string str)
{
  if(!isNumber(str))
    return(false);

  int ival = atoi(str.c_str());
  if(ival <= 0)
    return(false);

  m_amount = (unsigned int)(ival);
  return(true);
}

//---------------------------------------------------------
// Procedure: setAmount()

void ObstacleFieldGenerator::setAmount(unsigned int val)
{
  m_amount = val;
}

//---------------------------------------------------------
// Procedure: setMinRange()

bool ObstacleFieldGenerator::setMinRange(string str)
{
  return(setNonNegDoubleOnString(m_min_range, str));
}

//---------------------------------------------------------
// Procedure: setMinRange()

bool ObstacleFieldGenerator::setMinRange(double dval)
{
  if(dval < 0)
    return(false);
  m_min_range = dval;
  return(true);
}


//---------------------------------------------------------
// Procedure: setObstacleMinSize()

bool ObstacleFieldGenerator::setObstacleMinSize(string str)
{
  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());
  return(setObstacleMinSize(dval));
}

//---------------------------------------------------------
// Procedure: setObstacleMinSize()

bool ObstacleFieldGenerator::setObstacleMinSize(double dval)
{
  if(dval <= 0)
    return(false);

  m_min_poly_size = dval;
  if(m_max_poly_size < m_min_poly_size)
    m_max_poly_size = m_min_poly_size;

  return(true);
}

//---------------------------------------------------------
// Procedure: setPrecision()

bool ObstacleFieldGenerator::setPrecision(double dval)
{
  if(dval <= 0)
    return(false);
  if(dval == 1000)
    return(false);

  m_precision = dval;
  return(true);
}


//---------------------------------------------------------
// Procedure: setObstacleMaxSize()

bool ObstacleFieldGenerator::setObstacleMaxSize(string str)
{
  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());
  return(setObstacleMaxSize(dval));
}

//---------------------------------------------------------
// Procedure: setObstacleMaxSize()

bool ObstacleFieldGenerator::setObstacleMaxSize(double dval)
{
  if(dval <= 0)
    return(false);

  m_max_poly_size = dval;
  if(m_min_poly_size > m_max_poly_size)
    m_min_poly_size = m_max_poly_size;

  return(true);
}

//---------------------------------------------------------
// Procedure: setPolyVertices()

bool ObstacleFieldGenerator::setPolyVertices(string str)
{
  int ival = atoi(str.c_str());
  if(ival < 3)
    return(false);

  return(setObstacleMaxSize((unsigned int)(ival)));
}


//---------------------------------------------------------
// Procedure: setPolyVertices()

bool ObstacleFieldGenerator::setPolyVertices(unsigned int val)
{
  if(val < 3)
    return(false);

  m_poly_vertices = val;
  return(true);
}


//---------------------------------------------------------
// Procedure: setGenTries()

bool ObstacleFieldGenerator::setGenTries(string str)
{
  return(setUIntOnString(m_gen_tries, str));
}


//---------------------------------------------------------
// Procedure: setGenTries()

bool ObstacleFieldGenerator::setGenTries(unsigned int val)
{
  m_gen_tries = val;
  return(true);
}


//---------------------------------------------------------
// Procedure: generate()

bool ObstacleFieldGenerator::generate()
{
  srand(time(NULL));

  bool ok = true;
  for(unsigned int i=0; (ok && (i<m_amount)); i++) {
    ok = ok && generateObstacle(m_gen_tries);
  }
  
  if(m_verbose) {
    cout << "region    = " << m_poly_region.get_spec() << endl;
    cout << "min_range = " << doubleToStringX(m_min_range,2) << endl;
    cout << "min_size  = " << doubleToStringX(m_min_poly_size,2) << endl;
    cout << "max_size  = " << doubleToStringX(m_max_poly_size,2) << endl;
    
    if(ok) {
      for(unsigned int i=0; i<m_obstacles.size(); i++) 
	cout << "poly = " << m_obstacles[i].get_spec() << endl;
    }
  }

  return(ok);
}

//---------------------------------------------------------
// Procedure: generateObstacle()
//      Note: Attempts will be tries+1. So "tries" is more aptly
//            "retries".
 

bool ObstacleFieldGenerator::generateObstacle(unsigned int tries)
{
  if(!m_poly_region.is_convex())
    return(false);
    
  double minx = m_poly_region.get_min_x();
  double miny = m_poly_region.get_min_y();
  double maxx = m_poly_region.get_max_x();
  double maxy = m_poly_region.get_max_y();

  double xlen = maxx - minx;
  double ylen = maxy - miny;

  double diameter = m_min_poly_size;
  if(m_max_poly_size > m_min_poly_size) {
    int rand_int_r = rand() % 1000;
    double rand_pct_r = (double)(rand_int_r) / 1000;
    diameter = m_min_poly_size;
    diameter += ((m_max_poly_size - m_min_poly_size) * rand_pct_r);
  }
  double radius = diameter/2;
  
  for(unsigned int k=0; k<tries+1; k++) {
  
    int rand_int_x = rand() % 10000;
    int rand_int_y = rand() % 10000;
    
    double rand_pct_x = (double)(rand_int_x) / 10000;
    double rand_pct_y = (double)(rand_int_y) / 10000;

    double rand_x = minx + (rand_pct_x * xlen);
    double rand_y = miny + (rand_pct_y * ylen);

    // Reject if poly center point is not in the overall region
    if(!m_poly_region.contains(rand_x, rand_y))
      continue;

    // Reject if poly center point is in an existing obstacle
    bool pt_in_obstacle = false;
    for(unsigned int i=0; i<m_obstacles.size(); i++) {
      if(m_obstacles[i].contains(rand_x, rand_y)) {
	pt_in_obstacle = true;
      }
    }
    if(pt_in_obstacle)
      continue;
    
    //"radial:: x=val, y=val, radius=val, pts=val, snap=val, label=val"
    string str = "format=radial, x=" + doubleToString(rand_x,1); 
    str += ", y=" + doubleToString(rand_y,1);
    str += ",radius=" + doubleToString(radius);
    str += ",snap=" + doubleToStringX(m_precision,3);
    str += ",pts=" + uintToString(m_poly_vertices);
    str += ",label=ob_" + uintToString(m_begin_id + m_obstacles.size());
 
    XYPolygon try_poly = string2Poly(str);
    if(!try_poly.is_convex()) {
      //cout << "     try_poly is not convex." << endl;
      return(false);
    }
    
    // Reject if poly is not in the overall region
    if(!m_poly_region.contains(try_poly))
      continue;

    // Reject if poly intersects any existing obstacle
    bool poly_ints_obstacle = false;
    for(unsigned int i=0; i<m_obstacles.size(); i++) {
      if(m_obstacles[i].intersects(try_poly)) {
	poly_ints_obstacle = true;
      }
    }
    if(poly_ints_obstacle)
      continue;
    
    // Reject if poly is too close to any existing obstacle
    bool poly_closeto_obstacle = false;
    for(unsigned int i=0; i<m_obstacles.size(); i++) {
      if(m_obstacles[i].dist_to_poly(try_poly) < m_min_range) {
	poly_closeto_obstacle = true;
      }
    }
    if(poly_closeto_obstacle)
      continue;

    // Success!!!
    m_obstacles.push_back(try_poly);
    return(true);
  }
  return(false);
}

