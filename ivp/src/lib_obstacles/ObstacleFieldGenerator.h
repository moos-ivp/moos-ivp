/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
  bool   setPolygon(XYPolygon);

  bool   setAmount(std::string);
  void   setAmount(unsigned int);

  bool   setMinRange(std::string);
  bool   setMinRange(double);

  bool   setObstacleMinSize(std::string);
  bool   setObstacleMinSize(double);

  bool   setObstacleMaxSize(std::string);
  bool   setObstacleMaxSize(double);

  bool   setPolyVertices(std::string);
  bool   setPolyVertices(unsigned int);

  bool   setPrecision(double);

  bool   setGenTries(std::string);
  bool   setGenTries(unsigned int);
  
  void   setBeginID(unsigned int v) {m_begin_id=v;}
  void   setVerbose(bool v) {m_verbose=v;}
  
  bool   generate();

  std::vector<XYPolygon> getObstacles() const {return(m_obstacles);}

 protected:
  bool   generateObstacle(unsigned int tries);

 protected: // Config variables
  XYPolygon m_poly_region;

  double   m_min_poly_size;
  double   m_max_poly_size;
  double   m_min_range;
  double   m_precision;

  bool     m_verbose;

  unsigned int m_amount;
  unsigned int m_begin_id;

  unsigned int m_poly_vertices;
  unsigned int m_gen_tries;
  
  std::vector<XYPolygon> m_obstacles;  
};

#endif 









