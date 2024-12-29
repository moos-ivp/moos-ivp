/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RectHullGenerator.h                                  */
/*    DATE: Aug 21st 2020                                        */
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

#ifndef RECT_HULL_GENERATOR_HEADER
#define RECT_HULL_GENERATOR_HEADER

#include <list>
#include <vector>
#include "XYPoint.h"
#include "XYPolygon.h"

class RectHullGenerator
{
 public:
  RectHullGenerator();
  ~RectHullGenerator() {}

  void addPoint(XYPoint);
  void addPoint(double, double);
  void setRotationAngle(double); 
  
  XYPolygon generateRectHull();

  double getPolyHeight() const  {return(m_hgt);}
  double getPolyWidth() const   {return(m_wid);}
  XYPolygon getRectPoly() const {return(m_rpoly);}

private: // Configuration variables
  std::vector<XYPoint>  m_original_pts;

  double m_rotation_angle;
  
 private: // State variables, calculated results
  double    m_wid;
  double    m_hgt;
  XYPolygon m_rpoly;
};

#endif 
