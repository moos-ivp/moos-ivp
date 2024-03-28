/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LoiterEngine.cpp                                     */
/*    DATE: May 6th, 2007                                        */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <vector>
#include <cmath>
#include "GeomUtils.h"
#include "ColorParse.h"
#include "AngleUtils.h"
#include "LoiterEngine.h"

using namespace std;

//-----------------------------------------------------------
// Constructor()

LoiterEngine::LoiterEngine()
{
  m_clockwise      = true;
  m_spiral_factor  = -2;
}

//-----------------------------------------------------------
// Procedure: setClockwise()
//   Purpose: Set the clockwise flag - adjust the polygon if needed

void LoiterEngine::setClockwise(bool g_clockwise)
{
  m_clockwise = g_clockwise;
  
  if(m_polygon.is_clockwise() != m_clockwise)
    m_polygon.reverse();
}

//-----------------------------------------------------------
// Procedure: modPolyRad()

void LoiterEngine::modPolyRad(double val)
{
  double min_rad = 5; // arbitrary min
  double cur_rad = m_polygon.max_radius();
  double new_rad = cur_rad + val;


  cout << "min_rad:" << min_rad << endl;
  cout << "cur_rad:" << cur_rad << endl;
  cout << "new_rad:" << new_rad << endl;

  if(new_rad < min_rad)
    new_rad = min_rad;

  double amt = new_rad - cur_rad;
  cout << "amt:" << amt << endl;
  
  m_polygon.grow_by_amt(amt);  
  double new_cur_rad = m_polygon.max_radius();
  cout << "new_cur_rad:" << new_cur_rad << endl;
}

//-----------------------------------------------------------
// Procedure: setSpiralFactor()
//      Note: 0 is little or no spiraling
//            100 is lots of spiraling

void LoiterEngine::setSpiralFactor(double val)
{
  if(m_spiral_factor < 0)
    m_spiral_factor = 0;
  if(m_spiral_factor > 100)
    m_spiral_factor = 100;

  double pct = val / 100.0;
  double range_low  = -75;
  double range_high = -1;
  double range = range_high - range_low;

  m_spiral_factor = range_low + (pct * range);
}

//-----------------------------------------------------------
// Procedure: acquireVertex()
//   Purpose: Determine which of the viewable vertices in the polygon
//            is the best point to serve as a vehicle entry point. 
//            Determination is based on which direction the vehicle
//            will be travelling around the polygon (clockwise or not).

int LoiterEngine::acquireVertex(double os_hdg, double os_x, double os_y)
{
  if(!m_polygon.is_convex())
    return(-1);
  
  if(m_polygon.contains(os_x, os_y))
    return(acquireVertexIn(os_hdg, os_x, os_y));
  else
    return(acquireVertexOut(os_x, os_y, m_clockwise));
}

//-----------------------------------------------------------
// Procedure: resetClockwiseBest
//   Purpose: Determine whether a clockwise or counterclockwise 
//            entry angle would be best, minimizing the turn needed
//            from the vehicle's current heading.

void LoiterEngine::resetClockwiseBest(double os_hdg, double os_x, 
				      double os_y)
{
  if(!m_polygon.is_convex())
    return;

  if(m_polygon.contains(os_x, os_y))
    return;

  unsigned int v_clockwise  = acquireVertexOut(os_x, os_y, true);
  unsigned int v_cclockwise = acquireVertexOut(os_x, os_y, false);

  double cvx  = m_polygon.get_vx(v_clockwise);
  double cvy  = m_polygon.get_vy(v_clockwise);
  double ccvx = m_polygon.get_vx(v_cclockwise);
  double ccvy = m_polygon.get_vy(v_cclockwise);

  double hdg_clockwise  = relAng(os_x, os_y, cvx, cvy);
  double hdg_cclockwise = relAng(os_x, os_y, ccvx, ccvy);

  // Get deltas in the range [-180, 180]
  double delta_clockwise  = angle180(os_hdg - hdg_clockwise);
  double delta_cclockwise = angle180(os_hdg - hdg_cclockwise);

  // Get deltas abs value in the range [0, 180]
  if(delta_clockwise < 0)
    delta_clockwise *= -1;
  if(delta_cclockwise < 0)
    delta_cclockwise *= -1;

  if(delta_clockwise < delta_cclockwise)
    setClockwise(true);
  else
    setClockwise(false);
}

//-----------------------------------------------------------
// Procedure: acquireVertexOut
//   Purpose: Determine which of the viewable vertices in the polygon
//            is the best point to serve as a vehicle entry point. 
//            Determination is based on which direction the vehicle
//            will be travelling around the polygon (clockwise or not).

unsigned int LoiterEngine::acquireVertexOut(double os_x, double os_y, 
					    bool clockwise)
{
  if(!m_polygon.is_convex())
    return(0);
  if(m_polygon.contains(os_x, os_y))
    return(0);

  unsigned int i, vsize = m_polygon.size();

  unsigned int index = 0;
  bool   fresh = true;
  double best_angle = 0;
  for(i=0; i<vsize; i++) {
    if(m_polygon.vertex_is_viewable(i, os_x, os_y)) {
      unsigned int j = i+1;
      if(i == vsize-1)
	j = 0;
      double x2 = m_polygon.get_vx(i);
      double y2 = m_polygon.get_vy(i);
      double x3 = m_polygon.get_vx(j);
      double y3 = m_polygon.get_vy(j);

      double angle = segmentAngle(os_x, os_y, x2, y2, x3, y3);
      if(clockwise) {
	if(fresh || (angle > best_angle)) {
	  fresh = false;
	  index = i;
	  best_angle = angle;
	}
      }
      else {
	if(fresh || (angle < best_angle)) {
	  fresh = false;
	  index = i;
	  best_angle = angle;
	}
      }
    }
  }  
  return(index);
}


//-----------------------------------------------------------
// Procedure: acquireVertexIn

unsigned int LoiterEngine::acquireVertexIn(double os_hdg, double os_x, 
					   double os_y)
{
  if(!m_polygon.is_convex())
    return(0);
  if(!m_polygon.contains(os_x, os_y))
    return(0);
  
  os_hdg = angle360(os_hdg);
  
  unsigned int i, vsize = m_polygon.size();
  
  // Determine which vertex in the polygon is closest
  // to direction the vehicle is currently heading toward
  double smallest_delta = 360;
  unsigned int smallest_delta_ix = 0;
  for(i=0; i<vsize; i++) {
    double p_x = m_polygon.get_vx(i);
    double p_y = m_polygon.get_vy(i);
    double p_ang = relAng(os_x, os_y, p_x, p_y);
    double delta_ang = fabs(os_hdg - p_ang);
    if(delta_ang < smallest_delta) {
      smallest_delta = delta_ang;
      smallest_delta_ix = i;
    }
  }

  // Calc the angle made by maneuver to a vertex and the 
  // subsequent vertex. Take the absolut value.
  //   2o-----o3    3o-----o2      o3      o2            |
  //    |                  |       |       | \           |
  //    |  +90        -90  |      o2 +0    |  \ +135     |
  //    |                  |       |       |   \         |
  //    o1                 o1      o1      o1  3o        |
		   
  vector<double> pt_segangle;
  for(i=0; i<vsize; i++) {
    unsigned int ixx = i+1;
    if(ixx==vsize)
      ixx=0;
    
    double p_xa  = m_polygon.get_vx(i);
    double p_ya  = m_polygon.get_vy(i);
    double p_xb  = m_polygon.get_vx(ixx);
    double p_yb  = m_polygon.get_vy(ixx);
    double p_ang = segmentAngle(os_x, os_y, p_xa, p_ya, p_xb, p_yb);
    
    pt_segangle.push_back(fabs(p_ang));
  }
  
  // Give a bonus to the vertex to which we are heading already
  pt_segangle[smallest_delta_ix] += m_spiral_factor;
  
  // Find the highst ranking vertex (lowest angle value)
  double low_angle = pt_segangle[0];
  unsigned int low_ix = 0;
  for(i=1; i<vsize; i++) {
    if(pt_segangle[i] < low_angle) {
      low_ix = i;
      low_angle = pt_segangle[i];
    }
  }
  return(low_ix);
}










