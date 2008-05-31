/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LoiterEngine.cpp                                     */
/*    DATE: May 6th, 2007                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <vector>
#include <math.h>
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "LoiterEngine.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: setClockwise
//   Purpose: Set the clockwise flag - adjust the polygon if needed

void LoiterEngine::setClockwise(bool g_clockwise)
{
  m_clockwise = g_clockwise;
  
  if(m_polygon.is_clockwise() != m_clockwise)
	m_polygon.reverse();
}

//-----------------------------------------------------------
// Procedure: acquireVertex
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
    return(acquireVertexOut(os_x, os_y));
}

//-----------------------------------------------------------
// Procedure: acquireVertexOut
//   Purpose: Determine which of the viewable vertices in the polygon
//            is the best point to serve as a vehicle entry point. 
//            Determination is based on which direction the vehicle
//            will be travelling around the polygon (clockwise or not).

int LoiterEngine::acquireVertexOut(double os_x, double os_y)
{
  if(!m_polygon.is_convex())
    return(-1);
  
  int vsize = m_polygon.size();
  
  int    index = -1;
  bool   fresh = true;
  double best_angle;
  for(int i=0; i<vsize; i++) {
    if(m_polygon.vertex_is_viewable(i, os_x, os_y)) {
      int j = i+1;
      if(i == vsize-1)
	j = 0;
      double x2 = m_polygon.get_vx(i);
      double y2 = m_polygon.get_vy(i);
      double x3 = m_polygon.get_vx(j);
      double y3 = m_polygon.get_vy(j);
      double angle = segmentAngle(os_x, os_y, x2,y2,x3,y3);
      if(m_clockwise) {
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

int LoiterEngine::acquireVertexIn(double os_hdg, double os_x, double os_y)
{
  if(!m_polygon.is_convex())
    return(-1);
  
  os_hdg = angle360(os_hdg);
  
  int i, vsize = m_polygon.size();
  
  // Determine which vertex in the polygon is closest
  // to direction the vehicle is currently heading toward
  double smallest_delta = 360;
  int    smallest_delta_ix;
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
    int ixx = i+1;
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
  pt_segangle[smallest_delta_ix] -= 2.0;
  
  // Find the highst ranking vertex (lowest angle value)
  double low_angle = pt_segangle[0];
  int    low_ix    = 0;
  for(i=1; i<vsize; i++) {
    if(pt_segangle[i] < low_angle) {
      low_ix = i;
      low_angle = pt_segangle[i];
    }
  }
  
  return(low_ix);
}


