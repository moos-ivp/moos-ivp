/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYArc.h                                              */
/*    DATE: Dec 27th 2006                                        */
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

#ifndef ARC_XY_HEADER
#define ARC_XY_HEADER

#include <string>
#include "XYCircle.h"

class XYArc : public XYCircle {
public:
  XYArc();
  XYArc(double x, double y, double rad, double langle, double rangle);
  XYArc(double x, double y, double rad, double langle, double rangle,
	double ax1, double ay1, double ax2, double ay2);
  virtual ~XYArc() {};

  bool   initialize(const std::string&);
  void   set(double x, double y, double radius, 
	     double langle, double rangle);
  void   set(double x, double y, double radius, 
	     double langle, double rangle, double ax1,
	     double ay1, double ax2, double ay2);
  void   setLangle(double v);
  void   setRangle(double v);

  double getLangle() const    {return(m_langle);};
  double getRangle() const    {return(m_rangle);};

  double lengthUnits() const;
  double lengthDegrees() const;
  bool   containsAngle(double) const;
  bool   containsPoint(double, double) const;

  bool   segIntersectStrict(double, double, double, double) const;
  double ptDistToArc(double, double) const;
  
  int    segIntersectPts(double x1, double y1, 
			 double x2, double y2, 
			 double& rx1, double& ry1, 
			 double& rx2, double& ry2) const;

  std::string toString();

  double getAX1() {return(m_ax1);};
  double getAY1() {return(m_ay1);};
  double getAX2() {return(m_ax2);};
  double getAY2() {return(m_ay2);};

protected:
  void   setBoundingBox();
  double cutLength(double, int);
  double cutLengthXPlus(double);

protected:
  double m_langle; // The angle to the start-point
  double m_rangle; // The angle to the end-point

private:
  double m_ax1;    // The x position of the start-point
  double m_ay1;    // The y position of the start-point
  double m_ax2;    // The x position of the end-point
  double m_ay2;    // The y position of the end-point

  double m_xlow;   // The lowest  x position of the arc  
  double m_xhigh;  // The highest x position of the arc  
  double m_ylow;   // The lowest  y position of the arc  
  double m_yhigh;  // The highest y position of the arc  
};
#endif









