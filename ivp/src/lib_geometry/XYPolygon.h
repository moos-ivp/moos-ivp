/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYPolygon.h                                          */
/*    DATE: Apr 29, 2005                                         */
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
 
#ifndef XY_POLYGON_HEADER
#define XY_POLYGON_HEADER

#include <vector>
#include <string>
#include "XYSegList.h"

class XYPolygon : public XYSegList {
public:
  XYPolygon();
  ~XYPolygon() {};

  // Polygon create and edit functions
  bool   add_vertex(double, double, double, bool=true);
  bool   add_vertex(double, double, bool=true);
  bool   alter_vertex(double, double, double=0);
  bool   delete_vertex(double, double);
  bool   insert_vertex(double, double, double=0);
  bool   is_clockwise() const;

  void   clear();
  bool   apply_snap(double snapval);
  void   reverse();
  void   rotate(double);

public:
  bool   contains(double, double) const;
  bool   intersects(const XYPolygon&) const;
  double dist_to_poly(double px, double py) const;
  double dist_to_poly(double x1, double y1, double x2, double y2) const;
  double dist_to_poly(double px, double py, double angle) const;
  bool   seg_intercepts(double, double, double, double) const;
  bool   vertex_is_viewable(int, double, double) const;
  bool   is_convex() const  {return(convex_state);};
  void   determine_convexity();

protected:
  int    side(double x1, double y1, double x2, 
	      double y2, double x3, double y3) const;
  void   set_side(int);

private:
  std::vector<int> side_xy;

  bool convex_state;
};

#endif









