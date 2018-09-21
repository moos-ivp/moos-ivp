/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Seglr.h                                              */
/*    DATE: Mar 21st, 2015                                       */
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
 
#ifndef SEGLR_HEADER
#define SEGLR_HEADER

#include <string>
#include <vector>

class Seglr {
public:
  Seglr(double ray_angle=0) {m_ray_angle=ray_angle;}
  virtual ~Seglr() {}

  // Setters
  void addVertex(double x, double y);
  void setVertex(double x, double y, unsigned int index);
  void setRayAngle(double angle);
  void clear();

  // Getters
  double getVX(unsigned int) const;
  double getVY(unsigned int) const;
  double getRayAngle() const;

  // Modifiers
  void translateTo(double x, double y);
  void reflect();

  // Analysis
  unsigned int size() const {return(m_vx.size());}

  bool crossesLine(double x1, double y1, double x2, double y2,
		   double& ix, double& iy, bool ray_first=true) const;

  bool crossesLine(double x1, double y1, double x2, double y2,
		   bool ray_first=true) const;

  
  std::string getSpec(int precision=2) const;

  double getMinX() const;
  double getMaxX() const;
  double getMinY() const;
  double getMaxY() const;

protected:
  std::vector<double> m_vx;
  std::vector<double> m_vy;
  double              m_ray_angle;
};

#endif




