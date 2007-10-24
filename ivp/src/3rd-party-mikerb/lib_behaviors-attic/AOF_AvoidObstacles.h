/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_AvoidObstacles.h                                 */
/*    DATE: Aug 2nd, 2006                                        */
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
 
#ifndef AOF_AVOID_OBSTACLES_HEADER
#define AOF_AVOID_OBSTACLES_HEADER

#include "AOF.h"
#include "XYPolygon.h"

class IvPDomain;
class AOF_AvoidObstacles: public AOF {
public:
  AOF_AvoidObstacles(IvPDomain);
  ~AOF_AvoidObstacles() {};

public: // virtual functions
  double evalBox(const IvPBox*) const; 
  bool   setParam(const std::string&, double);
  bool   initialize();

  void   addObstacle(XYPolygon, double outer=0, double inner=0);

private:
  int    crs_ix;  // Index of "course" variable in IvPDomain

  double os_x;
  double os_y;
  bool   os_x_set;
  bool   os_y_set;

  std::vector<XYPolygon> obstacles;
  std::vector<double>    inner_dist;
  std::vector<double>    outer_dist;
};

#endif







