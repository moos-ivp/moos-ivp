/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYFieldGenerator.h                                   */
/*    DATE: Jan 27th, 2012                                       */
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

#ifndef XY_FIELD_GENERATOR_HEADER
#define XY_FIELD_GENERATOR_HEADER

#include <vector>
#include <string>
#include "XYPolygon.h"
#include "XYFormatUtilsPoly.h"

class XYFieldGenerator
{
 public:
  XYFieldGenerator();
  virtual ~XYFieldGenerator() {}

  bool addPolygon(std::string);
  bool addPolygon(XYPolygon);
  bool setSnap(double);
  
  XYPoint generatePoint();

  unsigned int polygonCount() {return(m_polygons.size());}
  unsigned int size()         {return(m_polygons.size());}
  XYPolygon    getPolygon(unsigned int);

 protected: // Config variables
  std::vector<XYPolygon>    m_polygons;

  double   m_snap;

 protected: // State variables
  double   m_poly_min_x;
  double   m_poly_min_y;
  double   m_poly_max_x;
  double   m_poly_max_y;
};

#endif 








