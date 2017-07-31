/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ConvexHull.h                                         */
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
  ConvexHullGenerator() {}
  ~ConvexHullGenerator() {}

   void addPoint(double, double);
   void addPoint(double, double, std::string);

   XYPolygon generateConvexHull();
   XYPoint   getRootPoint() {return(m_root);}

 protected:
   void   findRoot();
   void   sortPoints();

 private: // Configuration variables
   std::vector<XYPoint>  m_original_pts;

 private: // State variables
   XYPoint               m_root;
   std::vector<XYPoint>  m_points;
   
   std::list<XYPoint>    m_stack;
};

#endif 




