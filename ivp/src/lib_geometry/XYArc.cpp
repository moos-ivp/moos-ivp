/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYArc.cpp                                            */
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
#include "XYArc.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "MBUtils.h"

using namespace std;

#ifndef M_PI
#define M_PI 3.1415926
#endif

//-------------------------------------------------------------
// Procedure: Constructor

XYArc::XYArc() : XYCircle(0,0,0)
{
  m_langle = 0;
  m_rangle = 0;
  m_ax1    = 0;
  m_ay1    = 0;
  m_ax2    = 0;
  m_ay2    = 0;
}

//-------------------------------------------------------------
// Procedure: Constructor

XYArc::XYArc(double g_x, double g_y, double g_rad, 
	     double g_langle, double g_rangle) :
  XYCircle(g_x, g_y, g_rad)
{
  m_langle = g_langle;
  m_rangle = g_rangle;
  if((g_langle >= 360) || (g_langle < 0))
    m_langle = angle360(m_langle);
  if((g_rangle >= 360) || (g_rangle < 0))
    m_rangle = angle360(m_rangle);

  projectPoint(m_langle, m_rad, m_x, m_y, m_ax1, m_ay1);
  projectPoint(m_rangle, m_rad, m_x, m_y, m_ax2, m_ay2);
  setBoundingBox();
}

//-------------------------------------------------------------
// Procedure: Constructor
//      Note: Meant to serve as a fast way to construct the arc
//            if the caller has determined all the parameters 
//            and double-checked the validity, this method provides
//            a fast way to set the Arc with no more error checks.
//            USE WITH CAUTION.


XYArc::XYArc(double g_x, double g_y, double g_rad, 
	     double g_langle, double g_rangle, double g_ax1,
	     double g_ay1, double g_ax2, double g_ay2) :
  XYCircle(g_x, g_y, g_rad)
{
  m_langle = g_langle;
  m_rangle = g_rangle;
  m_ax1    = g_ax1;
  m_ay1    = g_ay1;
  m_ax2    = g_ax2;
  m_ay2    = g_ay2;

  setBoundingBox();
}

//-------------------------------------------------------------
// Procedure: initialize

bool XYArc::initialize(const string& g_str)
{
  vector<string> svector = parseString(g_str, ',');
  int vsize = svector.size();
  if(vsize != 5)
    return(false);
  
  for(int i=0; i<vsize; i++) { 
    svector[i] = stripBlankEnds(svector[i]);
    if(!isNumber(svector[i]))
      return(false);
  }

  m_x      = atof(svector[0].c_str());
  m_y      = atof(svector[1].c_str());
  m_rad    = atof(svector[2].c_str());
  m_langle = atof(svector[3].c_str());
  m_rangle = atof(svector[4].c_str());
      
  if(m_rad < 0) {
    set(0,0,0,0,0);
    return(false);
  }

  projectPoint(m_langle, m_rad, m_x, m_y, m_ax1, m_ay1);
  projectPoint(m_rangle, m_rad, m_x, m_y, m_ax2, m_ay2);

  setBoundingBox();
  return(true);
}

//-------------------------------------------------------------
// Procedure: setLangle/Rangle

void XYArc::setLangle(double g_angle)
{
  m_langle = angle360(g_angle);
  projectPoint(m_langle, m_rad, m_x, m_y, m_ax1, m_ay1);
  setBoundingBox();
}

void XYArc::setRangle(double g_angle)
{
  m_rangle = angle360(g_angle);
  projectPoint(m_rangle, m_rad, m_x, m_y, m_ax2, m_ay2);
  setBoundingBox();
}


//-------------------------------------------------------------
// Procedure: set

void XYArc::set(double g_x, double g_y, double g_rad,
		double g_langle, double g_rangle)
{
  XYCircle::set(g_x, g_y, g_rad);
  
  m_langle = g_langle;
  m_rangle = g_rangle;

  if((g_langle >= 360) || (g_langle < 0))
    m_langle = angle360(m_langle);
  if((g_rangle >= 360) || (g_rangle < 0))
    m_rangle = angle360(m_rangle);
  setBoundingBox();
}

//-------------------------------------------------------------
// Procedure: set
//      Note: Meant to serve as a fast way to initialize the arc
//            if the caller has determined all the parameters 
//            and double-checked the validity, this method provides
//            a fast way to set the Arc with no more error checks.
//            USE WITH CAUTION.

void XYArc::set(double g_x, double g_y, double g_rad,
		double g_langle, double g_rangle, double g_ax1,
		double g_ay1, double g_ax2, double g_ay2)
{
  XYCircle::set(g_x, g_y, g_rad);
  
  m_langle = g_langle;
  m_rangle = g_rangle;
  m_ax1    = g_ax1;
  m_ay1    = g_ay1;
  m_ax2    = g_ax2;
  m_ay2    = g_ay2;

  setBoundingBox();
}


//-------------------------------------------------------------
// Procedure: lengthUnits

double XYArc::lengthUnits() const
{
  double degree_length = lengthDegrees();
  double circumference = (M_PI * (2 * m_rad));
  return(circumference * (degree_length / 360));
}


//-------------------------------------------------------------
// Procedure: lengthDegrees

double XYArc::lengthDegrees() const
{
  if((m_rangle >=0) && (m_rangle <= m_langle))
    return(m_langle - m_rangle);
  else
    return(m_langle + (360 - m_rangle));
}

//-------------------------------------------------------------
// Procedure: containsAngle

bool XYArc::containsAngle(double g_angle) const
{
  if((m_rangle >=0) && (m_rangle <= m_langle))
    return((g_angle <= m_langle) && (g_angle >= m_rangle));
  else
    return(!((g_angle > m_langle) && (g_angle < m_rangle)));
}

//-------------------------------------------------------------
// Procedure: containsPoint
// Andrew Shafer, 8 FEB 2008
// Overloads the XYCircle containsPoint to more accurately
// reflect when an arc "contains" a point.
// Used by sensor model to determine if a FLIR-type system
// detects an artifact.

bool XYArc::containsPoint(double x, double y) const
{
  return (XYCircle::containsPoint(x, y) && containsAngle(relAng(m_x, m_y, x, y)));
}

//-------------------------------------------------------------
// Procedure: segIntersectStrict
//      Note: Returns TRUE only if the segment crosses the arc
//            perimeter. Will return FALSE if the segment is 
//            wholly contained in the arc.

bool XYArc::segIntersectStrict(double x1, double y1, 
			       double x2, double y2) const
{
  if(!XYCircle::segIntersectStrict(x1,y1,x2,y2))
    return(false);
  
  double rx1, ry1, rx2, ry2;
  int count = segIntersectPts(x1,y1,x2,y2,rx1,ry1,rx2,ry2);
  if(count == 1)
    return(containsAngle(relAng(m_x, m_y, rx1, ry1)));
  if(count == 2) 
    return(containsAngle(relAng(m_x, m_y, rx1, ry1)) ||
	   containsAngle(relAng(m_x, m_y, rx2, ry2)));
  
  // Should never get here since count should be either 1 or 2 
  // if XYCircle::secIntersect() returns true;
  return(false);
}

//-------------------------------------------------------------
// Procedure: ptDistToArc
//      Note: Returns the distance to the arc if the relative
//            angle from the center to the given point is within
//            the angle range given by langle,rangle.
//            If not, it returns the shorter of the two distances
//            between the given point and the arc endpoints.

double XYArc::ptDistToArc(double x1, double y1) const
{
  double relang = relAng(m_x, m_y, x1, y1);
  if(containsAngle(relang))
    return(ptDistToCircle(x1,y1));

  double dist1 = hypot((x1-m_ax1),(y1-m_ay1));
  double dist2 = hypot((x1-m_ax2),(y1-m_ay2));
  
  if(dist1 < dist2)
    return(dist1);
  else
    return(dist2);
}

//-------------------------------------------------------------
// Procedure: segIntersectPts
//    Determine the intesection points of the given line segment 
//    and the arc. The number of points could be 0, 1, or 2. The 
//    return value indicates how many such points. If only 1 point 
//    intersects, this point resides in rx1,ry1 and rx2,ry2 is to 
//    be ignored

int XYArc::segIntersectPts(double x1, double y1, double x2, 
			   double y2, double& rx1, double& ry1,
			   double& rx2, double& ry2) const
{
  int count = XYCircle::segIntersectPts(x1,y1,x2,y2,rx1,ry1,rx2,ry2);
  
  if(count == 0)
    return(0);

  else if(count == 1) {
    if(!containsAngle(relAng(m_x, m_y, rx1, ry1)))
      return(0);
    else
      return(1);
  }

  else if(count == 2) {
    if(!containsAngle(relAng(m_x, m_y, rx2, ry2)))
      count--;

    if(!containsAngle(relAng(m_x, m_y, rx1, ry1))) {
      if(count == 1)
	return(0);
      else {
	rx1 = rx2;
	ry1 = ry2;
	return(1);
      }
    }
    return(count);
  }
  // Should never execute this since a line segment intersects
  // an arc at at most two points.
  else 
    return(0);
}


//-------------------------------------------------------------
// Procedure: toString
//      Note: 

string XYArc::toString()
{
  string str = "arc:";
  str += doubleToString(m_x) + ",";
  str += doubleToString(m_y) + ",";
  str += doubleToString(m_rad) + ",";
  str += doubleToString(m_langle) + ",";
  str += doubleToString(m_rangle);
  return(str);
}


//-------------------------------------------------------------
// Procedure: setBoundingBox
//      Note: Should be called by any of the constructor, set or 
//            initialiaze functions that would change the parameters
//            of the arc.

void XYArc::setBoundingBox()
{
  // Handle the X-LOW case
  if(containsAngle(270))
    m_xlow = m_x - m_rad;
  else {
    if(m_ax2 < m_ax1)
      m_xlow = m_ax2;
    else
      m_xlow = m_ax1;
  }
  
  // Handle the X-HIGH case
  if(containsAngle(90))
    m_xhigh = m_x + m_rad;
  else {
    if(m_ax2 > m_ax1)
      m_xhigh = m_ax2;
    else
      m_xhigh = m_ax1;
  }
  
  // Handle the Y-LOW case
  if(containsAngle(180))
    m_ylow = m_y - m_rad;
  else {
    if(m_ay2 < m_ay1)
      m_ylow = m_ay2;
    else
      m_ylow = m_ay1;
  }
  
  // Handle the Y-HIGH case
  if(containsAngle(0))
    m_yhigh = m_y + m_rad;
  else {
    if(m_ay2 > m_ay1)
      m_yhigh = m_ay2;
    else
      m_yhigh = m_ay1;
  }
}


//-------------------------------------------------------------
// Procedure: cutLength()
//      Note: Type 0 - return length >= given x value
//            Type 1 - return length <= given x value
//            Type 2 - return length >= given y value
//            Type 3 - return length <= given y value

double XYArc::cutLength(double g_val, int g_type)
{
  if(g_type == 0) {
    if(g_val >= m_xhigh)
      return(0);
    if(g_val <= m_xlow)
      return(lengthUnits());
  }

  if(g_type == 1) {
    if(g_val >= m_xhigh)
      return(lengthUnits());
    if(g_val <= m_xlow)
      return(0);
  }

  if(g_type == 2) {
    if(g_val >= m_yhigh)
      return(0);
    if(g_val <= m_ylow)
      return(lengthUnits());
  }

  if(g_type == 3) {
    if(g_val >= m_yhigh)
      return(lengthUnits());
    if(g_val <= m_ylow)
      return(0);
  }
  return(0);
}


//-------------------------------------------------------------
// Procedure: cutLengthXPlus
//
//  Determine the length of the Arc that lies in the XY plane 
//  to the right or greater than a cutline given by x=g_val.


double XYArc::cutLengthXPlus(double g_val)
{
  // If the cutline is greater or equal to the high-x bound, 
  // just return zero.
  if(g_val >= m_xhigh)
    return(0);

  // If the cutline is less or equal to the low-x bound, return
  // the entire length of the XYArc.
  if(g_val <= m_xlow)
    return(lengthUnits());

  // Langle always in the southern hemisphere, 
  // Rangle always in the northern hemisphere.

  double langle_i, rangle_i;
  if(g_val > m_x) {
    double trig_ang = acos((g_val - m_x) / m_rad);
    langle_i = 90 + trig_ang;
    rangle_i = 90 - trig_ang;
  }
  else if(g_val < m_x) {
    double trig_ang = acos((m_x - g_val) / m_rad);
    langle_i = 270 - trig_ang;
    rangle_i = 270 + trig_ang;
  }
  else if(g_val == m_x) {
    langle_i = 180;
    rangle_i = 0;
  }
  
  double langle, rangle;
  // Case 1
  if((m_ax1 > g_val) && (m_ax2 <= g_val)) {
    langle = m_langle;
    rangle = rangle_i;
    if((rangle <= langle) && (rangle >= 0))
      return(((langle-rangle) / 360) * 2 * M_PI);
    else
      return(((langle+(360-rangle)) / 360) * 2 * M_PI);
  }
  // Case 2
  else if((m_ax1 <= g_val) && (m_ax2 > g_val)) {
    langle = langle_i;
    rangle = m_rangle;
    if((rangle <= langle) && (rangle >= 0))
      return(((langle-rangle) / 360) * 2 * M_PI);
    else
      return(((langle+(360-rangle)) / 360) * 2 * M_PI);
  }
  // Case 3
  else if((m_ax1 <= g_val) && (m_ax2 <= g_val)) {
    langle = langle_i;
    rangle = rangle_i;
    if((rangle <= langle) && (rangle >= 0))
      return(((langle-rangle) / 360) * 2 * M_PI);
    else
      return(((langle+(360-rangle)) / 360) * 2 * M_PI);
  }
  // Case 4
  else { //if((ax1 > g_val) && (ax2 > g_val))
    double langle1 = m_langle;
    double rangle1 = rangle_i;
    double langle2 = langle_i;
    double rangle2 = m_rangle;
    double val1, val2;
    if((rangle1 <= langle1) && (rangle1 >= 0))
      val1 = ((langle1-rangle1) / 360) * 2 * M_PI;
    else
      val1 = ((langle1+(360-rangle1)) / 360) * 2 * M_PI;
    val2 = ((langle2 - rangle2) / 360) * 2 * M_PI;
    return(val1 + val2);
  }
  return(0);
}


