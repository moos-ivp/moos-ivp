/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ConvexHullGenerator.h                                */
/*    DATE: Aug 26th 2014                                        */
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

#ifndef P_CONVEX_HULL_GENERATOR_HEADER
#define P_CONVEX_HULL_GENERATOR_HEADER

#include <list>
#include <vector>
#include "XYPoint.h"
#include "XYPolygon.h"

class ConvexHullGenerator
{
 public:
  ConvexHullGenerator() {m_settling_enabled=true;}
  ~ConvexHullGenerator() {}

  void addPoint(XYPoint);
  void addPoint(double, double);
  void addPoint(double, double, std::string);

  void disableSettling() {m_settling_enabled=false;}
  
  XYPolygon generateConvexHull();
  XYPoint   getRootPoint() {return(m_root);}
  
 protected: // helper funcctions
  void   findRoot();
  void   sortPoints();

  XYPolygon generateConvexHullTwoPts(XYPoint, XYPoint);
  XYPolygon generateConvexHullOnePt(XYPoint);

 private: // Configuration variables
  std::vector<XYPoint>  m_original_pts;
  bool                  m_settling_enabled;

 private: // State variables
  XYPoint               m_root;
  std::vector<XYPoint>  m_points;
};

#endif 

