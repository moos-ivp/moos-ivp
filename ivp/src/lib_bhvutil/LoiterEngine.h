/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LoiterEngine.h                                       */
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
 
#ifndef LOITER_ENGINE_HEADER
#define LOITER_ENGINE_HEADER

#include "XYPolygon.h"

class LoiterEngine {
 public:
  LoiterEngine();
  ~LoiterEngine() {}

  void   setClockwise(bool);
  void   setPoly(const XYPolygon& poly) {m_polygon = poly;}
  void   setCenter(double x, double y)  {m_polygon.new_center(x,y);}
  void   modPolyRad(double meters);
  void   setSpiralFactor(double v);

  double getCenterX() const      {return(m_polygon.get_center_x());}
  double getCenterY() const      {return(m_polygon.get_center_y());}
  bool   getClockwise() const    {return(m_clockwise);}
  double getSpiralFactor() const {return(m_spiral_factor);}
  double getRadius() const {return(m_polygon.max_radius());}

  unsigned int getPolyPts() const {return(m_polygon.size());}

  XYPolygon getPolygon() const   {return(m_polygon);}
  
  int  acquireVertex(double os_hdg, double os_x, double os_y);
  void resetClockwiseBest(double os_hdg, double os_x, double os_y);

 protected:
  unsigned int acquireVertexOut(double os_x, double os_y, bool clockwise);
  unsigned int acquireVertexIn(double os_hdg, double os_x, double os_y);

 protected:
  XYPolygon m_polygon;
  bool      m_clockwise;
  double    m_spiral_factor;
};

#endif












