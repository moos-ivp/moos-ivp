/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYCircle.h                                           */
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

#ifndef CIRCLE_XY_HEADER
#define CIRCLE_XY_HEADER

#include <string>

class XYCircle {
public:
  XYCircle();
  XYCircle(double, double, double);
  virtual ~XYCircle() {};

  bool   initialize(const std::string&);
  void   set(double, double, double);
  void   setX(double v)          {m_x=v;}
  void   setY(double v)          {m_y=v;}
  void   setRad(double v)        {m_rad=v; if(v<0) m_rad=0;};
  void   setLabel(std::string s) {m_label=s;};

  double getX()    const         {return(m_x);};
  double getY()    const         {return(m_y);};
  double getRad()  const         {return(m_rad);};
  std::string getLabel() const   {return(m_label);};
  std::string getDescription() const;

  void   alterX(double v)        {m_x+=v;};
  void   alterY(double v)        {m_y+=v;};
  void   alterRad(double v)      {m_rad+=v; if(m_rad<0) m_rad=0;};
  void   alterRadPct(double v)   {m_rad*=v; if(m_rad<0) m_rad=0;};
  
  
  // True if on or inside circle, need not intersect perimeter
  bool   containsPoint(double, double) const;

  // Distance to circle perim, distances inside are non-zero.
  double ptDistToCircle(double, double) const;

  // True if seg on or inside circle, need not intersect perim
  bool   segIntersect(double, double, double, double) const;

  // True if seg intersects circle perimeter, false if all inside
  bool   segIntersectStrict(double, double, double, double) const;

  // Return the (<=2) segment pts that intersect the circle.
  int    segIntersectPts(double x1, double y1, double x2, 
			 double y2, double& rx1, double& ry1, 
			 double& rx2, double& ry2) const;

  // Return length of given segment that lies within the circle
  double segIntersectLen(double, double, double, double) const;
  
protected:
  double       m_x;
  double       m_y;
  double       m_rad;
  std::string  m_label;
};
#endif









