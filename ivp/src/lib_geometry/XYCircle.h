/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
#include "XYObject.h"
#include "XYPoint.h"

class XYCircle : public XYObject {
public:
  XYCircle();
  XYCircle(double x, double y, double radius);
  virtual ~XYCircle() {};

  bool   initialize(const std::string&);
  bool   set(double, double, double);
  void   setX(double v)          {m_x=v; m_x_set=true;}
  void   setY(double v)          {m_y=v; m_y_set=true;}
  void   setRad(double v)        {m_rad=v; m_rad_set=true;if(v<0) m_rad=0;};
  void   setDuration(double v)   {m_duration=v;};
  void   set_spec_digits(int v)  {if((v>=0) && (v<=6)) m_sdigits=v;};

  bool   valid() const {return(m_x_set && m_y_set && m_rad_set);};

  double getX()    const         {return(m_x);};
  double getY()    const         {return(m_y);};
  double getDuration() const     {return(m_duration);};
  double getRad()  const         {return(m_rad);};
  void   alterX(double v)        {m_x+=v;};
  void   alterY(double v)        {m_y+=v;};
  void   alterRad(double v)      {m_rad+=v; if(m_rad<0) m_rad=0;};
  void   alterRadPct(double v)   {m_rad*=v; if(m_rad<0) m_rad=0;};

  double get_min_x() const {return(m_x - m_rad);};
  double get_max_x() const {return(m_x + m_rad);};
  double get_min_y() const {return(m_y - m_rad);};
  double get_max_y() const {return(m_y + m_rad);};

  std::string get_spec(std::string s="") const;
  
  void                setPointCache(unsigned int);
  std::vector<double> getPointCache() const {return(m_pt_cache);};

  // True if on or inside circle, need not intersect perimeter
  bool   containsPoint(double, double) const;
  bool   containsPoint(const XYPoint&) const;

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
  double   m_x;
  double   m_y;
  double   m_rad;
  int      m_sdigits;
  double   m_duration;

  bool     m_x_set;
  bool     m_y_set;
  bool     m_rad_set;

  std::vector<double> m_pt_cache;

};
#endif











