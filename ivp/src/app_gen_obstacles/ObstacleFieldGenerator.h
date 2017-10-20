/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ObstacleFieldGenerator.h                             */
/*    DATE: Oct 18th 2017                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef OBSTACLE_FIELD_GENERATOR_HEADER
#define OBSTACLE_FIELD_GENERATOR_HEADER

#include <vector>
#include <string>
#include <set>
#include "XYPolygon.h"
#include "XYFormatUtilsPoly.h"

class ObstacleFieldGenerator
{
 public:
  ObstacleFieldGenerator();
  virtual ~ObstacleFieldGenerator() {}

  bool   setPolygon(std::string);
  bool   setAmount(std::string);
  bool   setMinRange(std::string);
  bool   setObstacleMinSize(std::string);
  bool   setObstacleMaxSize(std::string);

  bool   generate();

 protected:
  bool   generateObstacle(unsigned int tries);

 protected: // Config variables
  XYPolygon m_poly_region;

  double   m_min_poly_size;
  double   m_max_poly_size;
  double   m_min_range;

  unsigned int m_amount;
  
  std::vector<XYPolygon> m_obstacles;
  
};

#endif 








