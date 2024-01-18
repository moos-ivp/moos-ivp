/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYOval.h                                             */
/*    DATE: March 12th 2022                                      */
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

#ifndef OVAL_XY_HEADER
#define OVAL_XY_HEADER

#include <string>
#include "XYObject.h"
#include "XYPolygon.h"
#include "XYPoint.h"

class XYOval : public XYObject {
public:
  XYOval(double x=0, double y=0, double rad=0, double len=0, double ang=0);
  virtual ~XYOval() {}
  
  void   setXY(double, double);
  void   setRadius(double);
  void   setAngle(double);
  void   setLength(double);

  void   setX(double v)       {setXY(v, m_y);}
  void   setY(double v)       {setXY(m_x, v);}
  void   modX(double v)       {setX(m_x+v);}
  void   modY(double v)       {setY(m_y+v);}
  void   modRadius(double v)  {setRadius(m_rad+v);}
  void   modAngle(double v)   {setAngle(m_ang+v);}
  void   modLength(double v)  {setLength(m_len+v);}

  void   set_spec_digits(int v)   {if((v>=0) && (v<=6)) m_sdigits=v;}
  
  void   setDrawDegs(double v)  {m_draw_degs=v;}
  
  bool   valid() const;
  
  double getX() const         {return(m_x);}
  double getY() const         {return(m_y);}
  double getRadius() const    {return(m_rad);}
  double getLength() const    {return(m_len);}
  double getAngle() const     {return(m_ang);}

  XYPoint getCenterPt();
  XYPoint getEndPt1();
  XYPoint getEndPt2();
  XYPolygon getRectPoly();
  XYPolygon getOvalPoly(double degs=5);
  
  double get_min_x() const    {return(m_xmin);}
  double get_max_x() const    {return(m_xmax);}
  double get_min_y() const    {return(m_ymin);}
  double get_max_y() const    {return(m_ymax);}
  
  double getDrawDegs() {return(m_draw_degs);}

  std::string get_spec(std::string s="") const;

  bool                isSetPointCache() {return(m_pt_cache.size() > 0);}
  void                clearPointCache() {m_pt_cache.clear();}
  void                setPointCache(double degs=5);
  std::vector<double> getPointCache() const {return(m_pt_cache);}

  void setBoundaryCache();
  
  // True if on or inside Oval, need not intersect perimeter
  bool   containsPoint(double, double);

  void   print();
  
protected:
  double   m_x;
  double   m_y;
  double   m_rad;
  double   m_len;
  double   m_ang;

  int      m_sdigits;

  bool     m_x_set;
  bool     m_y_set;
  bool     m_ang_set;

protected: // Variables to support caching

  XYPolygon m_poly;  // Inner rectacle
  double m_cx1;      // Two radii centers
  double m_cy1;
  double m_cx2;
  double m_cy2;

  double m_xmin;
  double m_xmax;
  double m_ymin;
  double m_ymax;
  
  bool   m_boundary_cache_set;
  double m_draw_degs; 

  std::vector<double> m_pt_cache;
};

XYOval stringToOval(std::string);
#endif
