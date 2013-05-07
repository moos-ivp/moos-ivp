/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYFieldGenerator.h                                   */
/*    DATE: Jan 27th, 2012                                       */
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
  virtual ~XYFieldGenerator() {};

  bool addPolygon(std::string);

  XYPoint generatePoint();

  unsigned int polygonCount() {return(m_polygons.size());}
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

