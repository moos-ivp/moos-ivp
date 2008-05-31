/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYCircle.cpp                                         */
/*    DATE: Dec 26th 2006                                        */
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

#include <math.h>
#include <stdlib.h>
#include "XYCircle.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

XYCircle::XYCircle()
{
  m_x   = 0;
  m_y   = 0;
  m_rad = 0;
}

//-------------------------------------------------------------
// Procedure: Constructor

XYCircle::XYCircle(double g_x, double g_y, double g_rad)
{
  set(g_x, g_y, g_rad);
}

//-------------------------------------------------------------
// Procedure: initialize

bool XYCircle::initialize(const string& g_str)
{
  vector<string> svector = parseString(g_str, ',');
  int vsize = svector.size();
  if(vsize==4)
    m_label = stripBlankEnds(svector[3]);

  if((vsize < 3) || (vsize > 4))
    return(false);
  
  for(int i=0; i<3; i++) { 
    svector[i] = stripBlankEnds(svector[i]);
    if(!isNumber(svector[i]))
      return(false);
  }

  m_x   = atof(svector[0].c_str());
  m_y   = atof(svector[1].c_str());
  m_rad = atof(svector[2].c_str());
      
  if(m_rad < 0) {
    set(0,0,0);
    return(false);
  }
  return(true);
}

//-------------------------------------------------------------
// Procedure: set

void XYCircle::set(double g_x, double g_y, double g_rad)
{
  m_x    = g_x;
  m_y    = g_y;
  m_rad  = g_rad;
  
  if(m_rad < 0) 
    m_rad = 0;
}

//-------------------------------------------------------------
// Procedure: getDescription()

string XYCircle::getDescription() const
{
  string str;
  str += doubleToString(m_x,2) + ",";
  str += doubleToString(m_y,2) + ",";
  str += doubleToString(m_rad,2) + ",";
  str += m_label;

  return(str);
}

//-------------------------------------------------------------
// Procedure: containsPoint

bool XYCircle::containsPoint(double g_x, double g_y) const
{
  double dist_to_center = hypot((g_x - m_x), (g_y - m_y));
  
  if(dist_to_center > m_rad)
    return(false);
  else
    return(true);
}

//-------------------------------------------------------------
// Procedure: ptDistToCircle
//      Note: Does NOT return zero if the point is contained w/in
//            the circle. If in the circle, distance returned is 
//            the distance out to the radius.

double XYCircle::ptDistToCircle(double x1, double y1) const
{
  double dist_to_center = hypot((x1-m_x), (y1-m_y));
  
  if(dist_to_center >= m_rad)
    return(dist_to_center - m_rad);
  else
    return(m_rad - dist_to_center);
}



//-------------------------------------------------------------
// Procedure: segIntersect
//      Note: Returns TRUE if segment is completely inside circle.
//            Even if the segment does not cross the circle
//            perimeter at all. 
//      Note: Use segIntersectStrict to determine if a segment
//            intersects the circle perimeter.

bool XYCircle::segIntersect(double x1, double y1, 
				   double x2, double y2) const
{
  // First a quick way of detecting a non-intersection (based on
  // an assumption that most such queries are not-even-close false)
  if((x1 > (m_x + m_rad)) && (x2 > (m_x + m_rad)))
    return(false);
  if((x1 < (m_x - m_rad)) && (x2 < (m_x - m_rad)))
    return(false);
  if((y1 > (m_y + m_rad)) && (y2 > (m_y + m_rad)))
    return(false);
  if((y1 < (m_y - m_rad)) && (y2 < (m_y - m_rad)))
    return(false);

  double dist_to_center = distPointToSeg(x1,y1,x2,y2,m_x,m_y);
  
  if(dist_to_center > m_rad)
    return(false);
  else
    return(true);
}


//-------------------------------------------------------------
// Procedure: segIntersectStrict
//      Note: Returns TRUE only if the segment crosses the circle
//            perimeter. Will return FALSE if the segment is 
//            wholly contained in the circle.

bool XYCircle::segIntersectStrict(double x1, double y1, 
				  double x2, double y2) const
{
  // First a quick way of detecting a non-intersection (based on
  // an assumption that most such queries are not-even-close false)
  if((x1 > (m_x + m_rad)) && (x2 > (m_x + m_rad)))
    return(false);
  if((x1 < (m_x - m_rad)) && (x2 < (m_x - m_rad)))
    return(false);
  if((y1 > (m_y + m_rad)) && (y2 > (m_y + m_rad)))
    return(false);
  if((y1 < (m_y - m_rad)) && (y2 < (m_y - m_rad)))
    return(false);

  double p1_dist_to_center = hypot((x1 - m_x), (y1 - m_y));
  double p2_dist_to_center = hypot((x2 - m_x), (y2 - m_y));
  
  // Case1 - both ends in the circle
  if((p1_dist_to_center < m_rad) && (p2_dist_to_center < m_rad))
    return(false);
  
  // Case2 - one end in, the other end out
  if((p1_dist_to_center >= m_rad) && (p2_dist_to_center < m_rad))
    return(true);
  
  // Case3 - other end in, one end out
  if((p1_dist_to_center < m_rad) && (p2_dist_to_center >= m_rad))
    return(true);
  
  // Case4 - both ends out of the circle
  else // mikerb May 30 2008
    //if((p1_dist_to_center > m_rad) && (p2_dist_to_center > m_rad))
    return(distPointToSeg(x1,y1,x2,y2,m_x,m_y) <= m_rad);
}




//-------------------------------------------------------------
// Procedure: segIntersectPts
//      Note: 

int XYCircle::segIntersectPts(double x1, double y1, double x2, 
			      double y2, double& rx1, double& ry1,
			      double& rx2, double& ry2) const
{
  double perp_x, perp_y;
  perpLineIntPt(x1, y1, x2, y2, m_x, m_y, perp_x, perp_y);
  
  double a_dist = hypot((perp_x - m_x), (perp_y - m_y));
  double c_dist = m_rad;
  double b_dist = sqrt((c_dist*c_dist)-(a_dist*a_dist));

  double y_low  = (y1<y2?y1:y2);
  double y_high = (y1<y2?y2:y1);
  double x_low  = (x1<x2?x1:x2);
  double x_high = (x1<x2?x2:x1);

  // 1219

  // Special-case the horizontal and vertical cases due to 
  // occasional rounding errors from the math library. For 
  // example, an horizontal line from (0,0) thru (10,0) might
  // result in an intersection point of (5, 0.000000001) which
  // would fail the bounding box test

  // Special Case 1: given segment is vertical
  if(x1==x2) {
    rx1 = x1;
    ry1 = perp_y - b_dist;
    rx2 = x1;
    ry2 = perp_y + b_dist;
  }
  // Special Case 2: given segment is horizontal
  else if(y1==y2) {
    rx1 = perp_x - b_dist;
    ry1 = y1;
    rx2 = perp_x + b_dist;
    ry2 = y1;
  }
  else {
    double rel_angle = relAng(perp_x, perp_y, m_x, m_y);
    double angle1 = angle360(rel_angle + 90);
    double angle2 = angle360(rel_angle - 90);
    projectPoint(angle1, b_dist, perp_x, perp_y, rx1, ry1);
    projectPoint(angle2, b_dist, perp_x, perp_y, rx2, ry2);
  }

  // Now apply a bounding box test to the resulting points.

  int count = 0;
  // If the first point is in the bounding box, count it.
  if((rx1 >= x_low) && (rx1 <= x_high) && 
     (ry1 >= y_low) && (ry1 <= y_high))
    count++;
  
  // If the 2nd point is in the bounding box, count it.
  if((rx2 >= x_low) && (rx2 <= x_high) && 
     (ry2 >= y_low) && (ry2 <= y_high)) {
    count++; 
    // If the 1st point was not, put this one in rx1, rx2
    if(count == 1) {
      rx1 = rx2;
      ry1 = ry2;
    }
  }
  return(count);
}

//-------------------------------------------------------------
// Procedure: segIntersectLen
//      Note: 

double XYCircle::segIntersectLen(double x1, double y1, 
				 double x2, double y2) const
{
  // If segment does not intersect the circle return zero.
  // This intersection test returns true even if the segment
  // is completely contained in the circle. Handled below.
  if(!segIntersect(x1,y1,x2,y2))
    return(0);
  
  double rx1, ry1, rx2, ry2;
  int count = segIntersectPts(x1,y1,x2,y2,rx1,ry1,rx2,ry2);

  // If segment is completely contained in the circle
  if(count == 0) // 
    return(hypot((x1-x2),(y1-y2)));

  // If segment passes in and out of the circle
  if(count == 2)
    return(hypot((rx1-rx2),(ry1-ry2)));


  // If segment crossess into and end inside the circle
  // Find the segment endpoint that is inside the circle
  if(count == 1) {
    // If (x1,y1) is closer to the center than (x2,y2)
    if((hypot((x1-m_x),(y1-m_y))) < (hypot((x2-m_x),(y2-m_y))))
      return(hypot((rx1-x1),(ry1-y1)));
    else
      return(hypot((rx1-x2),(ry1-y2)));
  }

  // Should never get here since count should be either 0,1, or 2
  return(0);
}


