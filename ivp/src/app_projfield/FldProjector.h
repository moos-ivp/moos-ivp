/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng, MIT Cambridge MA             */
/*    FILE: FldProjector.h                                       */
/*    DATE: Nov 19th, 2023                                       */
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

#ifndef FLD_PROJECTOR_HEADER
#define FLD_PROJECTOR_HEADER

#include <vector>
#include <string>
#include <map>
#include "XYPoint.h"

class FldProjector
{
 public:
  FldProjector(double rx=0, double ry=0, double ang=0);
  ~FldProjector() {}

  void setRootX(double x)  {m_root_x=x;}
  void setRootY(double y)  {m_root_y=y;}
  void setAngle(double a)  {m_angle=a;}
  void setGrid(double g)   {m_grid=g;}
  void setCellsX(double i) {m_cells_x=i;}
  void setCellsY(double j) {m_cells_y=j;}
  void setOffset(double v) {m_offset=v;}
  
  void buildProjection();
  void print();
  
 protected:

 private:

  double m_root_x;
  double m_root_y;
  double m_angle;
  double m_grid;
  double m_cells_x;
  double m_cells_y;
  double m_offset;

  std::map<std::string, XYPoint> m_map_projections;
};

#endif 
