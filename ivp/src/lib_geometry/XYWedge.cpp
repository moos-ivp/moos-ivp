/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYWedge.cpp                                          */
/*    DATE: Sep 10th 2015                                        */
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
#include <cstdlib>
#include "XYWedge.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "MBUtils.h"

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265359
#endif

//-------------------------------------------------------------
// Procedure: Constructor

XYWedge::XYWedge()
{
  // Configuration variables
  m_x = 0;
  m_y = 0;
  m_radlow = 0;
  m_radhgh = 0;
  m_langle = 0;
  m_hangle = 0;

  // State variables (set indicators)
  m_x_set = false;
  m_y_set = false;
  m_radlow_set = false;
  m_radhgh_set = false;
  m_langle_set = false;
  m_hangle_set = false;

  // State variables (cached values)
  m_xmin = 0;
  m_xmax = 0;
  m_ymin = 0;
  m_ymax = 0;

  m_llx = 0;
  m_lly = 0;
  m_lhx = 0;
  m_lhy = 0;
  m_hlx = 0;
  m_hly = 0;
  m_hhx = 0;
  m_hhy = 0;

  m_initialized = false;
}

//-------------------------------------------------------------
// Procedure: setX

void XYWedge::setX(double x)
{
  m_x = x;
  m_x_set = true;
  m_initialized = false;
}

//-------------------------------------------------------------
// Procedure: setY

void XYWedge::setY(double y)
{
  m_y = y;
  m_y_set = true;
  m_initialized = false;
}


//-------------------------------------------------------------
// Procedure: setRadLow

bool XYWedge::setRadLow(double radius)
{
  if(radius < 0)
    return(false);
  
  m_radlow = radius;
  m_radlow_set = true;

  // Ensure radlow is always less than or equal to radhgh
  if(m_radlow > m_radhgh)
    m_radhgh = m_radlow;

  m_initialized = false;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setRadHigh

bool XYWedge::setRadHigh(double radius)
{
  if(radius < 0)
    return(false);
  
  m_radhgh = radius;
  m_radhgh_set = true;

  // Ensure radlow is always less than or equal to radhgh
  if(m_radhgh < m_radlow)
    m_radlow = m_radhgh;

  m_initialized = false;
  return(true);
}


//-------------------------------------------------------------
// Procedure: setLangle

void XYWedge::setLangle(double angle)
{
  m_langle = angle360(angle);
  m_langle_set = true;
  m_initialized = false;
}

//-------------------------------------------------------------
// Procedure: setHangle

void XYWedge::setHangle(double angle)
{
  m_hangle = angle360(angle);
  m_hangle_set = true;
  m_initialized = false;
}



//-------------------------------------------------------------
// Procedure: isValid()

bool XYWedge::isValid() const
{
  cout << "langle_set " << boolToString(m_langle_set) << endl;
  cout << "hangle_set " << boolToString(m_hangle_set) << endl;
  cout << "m_x_set " << boolToString(m_x_set) << endl;
  cout << "m_y_set " << boolToString(m_y_set) << endl;
  cout << "m_radlow_set " << boolToString(m_radlow_set) << endl;
  cout << "m_radhgh_set " << boolToString(m_radhgh_set) << endl;
  
  return(m_langle_set && m_hangle_set && m_x_set && m_y_set &&
	 m_radlow_set && m_radhgh_set);
}

//-------------------------------------------------------------
// Procedure: initialize

bool XYWedge::initialize(double degrees_per_pt)
{
  // Part 1: Sanity checks
  cout << "XYWedge::initialize() 111" << endl;
  if(m_initialized)
    return(true);
  cout << "XYWedge::initialize() 222" << endl;

  if(!isValid())
    return(false);
  cout << "XYWedge::initialize() 333" << endl;

  // Part 2: Initialize the wedge corner points
  projectPoint(m_langle, m_radlow, m_x, m_y, m_llx, m_lly);
  projectPoint(m_langle, m_radhgh, m_x, m_y, m_lhx, m_lhy);
  projectPoint(m_hangle, m_radlow, m_x, m_y, m_hlx, m_hly);
  projectPoint(m_hangle, m_radhgh, m_x, m_y, m_hhx, m_hhy);

  // Part 3: Set initial rectilinear bounds from the corner points
  m_xmin = m_llx;
  if(m_lhx < m_xmin) m_xmin = m_lhx;
  if(m_hlx < m_xmin) m_xmin = m_hlx;
  if(m_hhx < m_xmin) m_xmin = m_hhx;

  m_xmax = m_llx;
  if(m_lhx > m_xmax) m_xmax = m_lhx;
  if(m_hlx > m_xmax) m_xmax = m_hlx;
  if(m_hhx > m_xmax) m_xmax = m_hhx;

  m_ymin = m_lly;
  if(m_lhy < m_ymin) m_ymin = m_lhy;
  if(m_hly < m_ymin) m_ymin = m_hly;
  if(m_hhy < m_ymin) m_ymin = m_hhy;

  m_ymax = m_lly;
  if(m_lhy > m_ymax) m_ymax = m_lhy;
  if(m_hly > m_ymax) m_ymax = m_hly;
  if(m_hhy > m_ymax) m_ymax = m_hhy;

  // Part 4: Possibly modify the rectilinear bounds from the round
  // parts of the wedge
  double arclen = angle360(m_hangle - m_langle);

  // Check the case where the north (0 degree) axis is in the arc
  if((m_langle + arclen) >= 360)
    m_ymax = m_y + m_radhgh;

  // Check the case where the east (90 degree) axis is in the arc
  if((m_langle < 90) && ((m_langle + arclen) > 90))
    m_xmax = m_x + m_radhgh;
  if((m_langle > 90) && ((m_langle + arclen) > 450))
    m_xmax = m_x + m_radhgh;
  
  // Check the case where the south (180 degree) axis is in the arc
  if((m_langle < 180) && ((m_langle + arclen) > 180))
    m_ymin = m_y - m_radhgh;
  if((m_langle > 180) && ((m_langle + arclen) > 540))
    m_ymin = m_y - m_radhgh;

  // Check the case where the west (270 degree) axis is in the arc
  if((m_langle < 270) && ((m_langle + arclen) > 270))
    m_xmin = m_x - m_radhgh;
  if((m_langle > 270) && ((m_langle + arclen) > 630))
    m_xmin = m_x - m_radhgh;

  // Part 5: Build the drawpt cache for rendering
  // clear the cache
  m_pt_cache.clear();
  // Draw the first edge
  m_pt_cache.push_back(m_llx);
  m_pt_cache.push_back(m_lly);
  m_pt_cache.push_back(m_lhx);
  m_pt_cache.push_back(m_lhy);
  
  double delta = degrees_per_pt;
  // Draw the outer arc
  for(double deg=m_langle; deg<m_hangle; deg+=delta) {
    double new_x, new_y;
    projectPoint(deg, m_radhgh, m_x, m_y, new_x, new_y);
    m_pt_cache.push_back(new_x);
    m_pt_cache.push_back(new_y);
  }
  // Draw the second edge
  m_pt_cache.push_back(m_hhx);
  m_pt_cache.push_back(m_hhy);
  m_pt_cache.push_back(m_hlx);
  m_pt_cache.push_back(m_hly);

  // Draw the inner arc
  for(double deg=m_hangle; deg>m_langle; deg-=delta) {
    double new_x, new_y;
    projectPoint(deg, m_radlow, m_x, m_y, new_x, new_y);
    m_pt_cache.push_back(new_x);
    m_pt_cache.push_back(new_y);
  }

  m_initialized = true;
  return(true);
}


//-------------------------------------------------------------
// Procedure: get_spec()

string XYWedge::get_spec(unsigned int precision, std::string param) const
{
  string spec;
  spec += "x=" + doubleToString(m_x, precision);
  spec += ",y=" + doubleToString(m_y, precision);

  spec += ",rad_low=" + doubleToStringX(m_radlow, precision);
  spec += ",rad_hgh=" + doubleToStringX(m_radhgh, precision);
  spec += ",ang_low=" + doubleToStringX(m_langle, precision);
  spec += ",ang_hgh=" + doubleToStringX(m_hangle, precision);
  
  string obj_spec = XYObject::get_spec(param);
  if(obj_spec != "") {
    if(spec != "")
      spec += ",";
    spec += obj_spec;
  }

  return(spec);
}

