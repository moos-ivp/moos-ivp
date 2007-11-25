/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LoiterEngine.h                                       */
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
 
#ifndef LOITER_ENGINE_HEADER
#define LOITER_ENGINE_HEADER

#include "XYPolygon.h"

class LoiterEngine {
 public:
  LoiterEngine() {m_clockwise=true;};
  ~LoiterEngine() {};

  void setPoly(const XYPolygon& poly) {m_polygon = poly;};
  void setCenter(double x, double y)  {m_polygon.new_center(x,y);};
  void setClockwise(bool);
  
  XYPolygon getPolygon() {return(m_polygon);};
  
 public:
  int acquireVertex(double os_hdg, double os_x, double os_y);
  
  int acquireVertexOut(double os_x, double os_y);
  
  int acquireVertexIn(double os_hdg, double os_x, double os_y);

 protected:
  XYPolygon m_polygon;
  bool      m_clockwise;
};

#endif


