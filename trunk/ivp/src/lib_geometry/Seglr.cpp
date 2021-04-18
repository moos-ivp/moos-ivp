/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Seglr.cpp                                            */
/*    DATE: Mar 21st, 2015                                       */
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

#include "GeomUtils.h"
#include "Seglr.h" 

using namespace std;

//---------------------------------------------------------------
// Procedure: clear

void Seglr::clear()
{
  m_vx.clear();
  m_vy.clear();
  m_ray_angle = 0;
}

//---------------------------------------------------------------
// Procedure: addVertex()

void Seglr::addVertex(double x, double y)
{
  m_vx.push_back(x);
  m_vy.push_back(y);
}

//---------------------------------------------------------------
// Procedure: setVertex()

void Seglr::setVertex(double x, double y, unsigned int ix)
{
  if((ix >= m_vx.size()) || (ix >= m_vy.size()))
    return;

  m_vx[ix] = x;
  m_vy[ix] = y;
}

//---------------------------------------------------------------
// Procedure: setRayAngle
//      Note: No error checking. Consumers/users will need to do this.

void Seglr::setRayAngle(double angle)
{
  m_ray_angle = angle;
}

//---------------------------------------------------------------
// Procedure: getVX()

double Seglr::getVX(unsigned int ix) const
{
  if(ix >= m_vx.size())
    return(0);
  return(m_vx[ix]);
}

//---------------------------------------------------------------
// Procedure: getVY()

double Seglr::getVY(unsigned int ix) const
{
  if(ix >= m_vy.size())
    return(0);
  return(m_vy[ix]);
}

//---------------------------------------------------------------
// Procedure: getRayAngle

double Seglr::getRayAngle() const
{
  return(m_ray_angle);
}

//---------------------------------------------------------------
// Procedure: translateTo

void Seglr::translateTo(double new_base_x, double new_base_y)
{
  if((m_vx.size() == 0) || (m_vy.size() == 0))
    return;
    
  double curr_base_x = m_vx[0];
  double curr_base_y = m_vy[0];
  
  double trans_x = new_base_x - curr_base_x;
  double trans_y = new_base_y - curr_base_y;

  for(unsigned int i=0; i<m_vx.size(); i++) {
    m_vx[i] += trans_x;
    m_vy[i] += trans_y;
  }
}

//---------------------------------------------------------------
// Procedure: reflect

void Seglr::reflect()
{
  for(unsigned int i=0; i<m_vx.size(); i++) 
    m_vx[i] = -m_vx[i];
  m_ray_angle = (360.0 - m_ray_angle);
}


//---------------------------------------------------------------
// Procedure: CrossesLine()

bool Seglr::crossesLine(double x1, double y1, double x2, double y2,
			double& ix, double& iy, bool ray_first) const
{
  // Sanity check
  unsigned int vsize = m_vx.size();
  if(vsize == 0)
    return(false);

  // Part 1: If ray-check is first, Check if the ray interects
  if(ray_first) {
    bool crosses = lineRayCross(m_vx[vsize-1], m_vy[vsize-1], m_ray_angle,
				x1, y1, x2, y2, ix, iy);
    if(crosses)
      return(true);
  }

  // Part 2: Check if any of the initial line segments intersect
  for(unsigned int i=0; i<vsize; i++) {
    if((i+1) < vsize) {
      bool crosses = linesCross(m_vx[i], m_vy[i], m_vx[i+1], m_vy[i+1],
				x1, y1, x2, y2, ix, iy);
      if(crosses)
	return(true);
    }
  }

  // Part 3: If ray-check is last, now check if the ray interects
  if(!ray_first) {
    bool crosses = lineRayCross(m_vx[vsize-1], m_vy[vsize-1], m_ray_angle,
				x1, y1, x2, y2, ix, iy);
    if(crosses)
      return(true);
  }

  return(false);
}

//---------------------------------------------------------------
// Procedure: CrossesLine()

bool Seglr::crossesLine(double x1, double y1, double x2, double y2,
			bool ray_first) const
{
  double ix, iy;
  return(crossesLine(x1, y1, x2, y2, ix, iy, ray_first));
}


//---------------------------------------------------------------
// Procedure: getMinX()

double Seglr::getMinX() const
{
  double min_x = 0;
  for(unsigned int i=0; i<m_vx.size(); i++) {
    if((i==0) || (m_vx[i] < min_x))
      min_x = m_vx[i];
  }
  return(min_x);
}

//---------------------------------------------------------------
// Procedure: getMaxX()

double Seglr::getMaxX() const
{
  double max_x = 0;
  for(unsigned int i=0; i<m_vx.size(); i++) {
    if((i==0) || (m_vx[i] > max_x))
      max_x = m_vx[i];
  }
  return(max_x);
}

//---------------------------------------------------------------
// Procedure: getMinY()

double Seglr::getMinY() const
{
  double min_y = 0;
  for(unsigned int i=0; i<m_vy.size(); i++) {
    if((i==0) || (m_vy[i] < min_y))
      min_y = m_vy[i];
  }
  return(min_y);
}

//---------------------------------------------------------------
// Procedure: getMaxY()

double Seglr::getMaxY() const
{
  double max_y = 0;
  for(unsigned int i=0; i<m_vy.size(); i++) {
    if((i==0) || (m_vy[i] > max_y))
      max_y = m_vy[i];
  }
  return(max_y);
}

//---------------------------------------------------------------
// Procedure: getSpec

string Seglr::getSpec(int precision) const
{
  string spec = "pts={";

  for(unsigned int i=0; i<m_vx.size(); i++) {
    string sx = doubleToString(m_vx[i], precision);
    string sy = doubleToString(m_vy[i], precision);

    string pair = sx + "," + sy;
    if(i>0)
      spec += ":";
    spec += pair;
  }
  spec += "},ray=" + doubleToString(m_ray_angle, precision);

  return(spec);
}






