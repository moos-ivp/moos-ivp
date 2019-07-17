/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYSegList.h                                          */
/*    DATE: Apr 29, 2005                                         */
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
 
#ifndef XY_SEGLIST_HEADER
#define XY_SEGLIST_HEADER

#include <vector>
#include <string>
#include "XYObject.h"
#include "XYPoint.h"
#include <iostream>

class XYSegList : public XYObject {
public:
  XYSegList() {}
  virtual ~XYSegList() {}

  // XYSegList create and edit functions
  void   add_vertex(const XYPoint&, std::string s="");
  void   add_vertex(double, double, double=0, std::string s="");       
  void   alter_vertex(double, double, double=0, std::string s="");
  void   delete_vertex(double, double);
  void   delete_vertex(unsigned int);
  void   insert_vertex(double, double, double=0, std::string s="");
  void   clear();

public:
  void   shift_horz(double val);
  void   shift_vert(double val);
  void   grow_by_pct(double pct);
  void   grow_by_amt(double amt);
  void   apply_snap(double snapval);
  void   rotate(double degrees);
  void   rotate(double degrees, double cx, double cy);
  void   reverse();
  void   new_center(double x, double y);
  void   new_centroid(double x, double y);
  bool   valid() const;

public:
  unsigned int size() const     {return(m_vx.size());}

  double get_vx(unsigned int) const;
  double get_vy(unsigned int) const;
  double get_vz(unsigned int) const;
  std::string get_vprop(unsigned int) const;
  double get_center_x() const;
  double get_center_y() const;
  double get_centroid_x() const;
  double get_centroid_y() const;
  double get_min_x() const;
  double get_max_x() const;
  double get_min_y() const;
  double get_max_y() const;
  double get_avg_x() const;
  double get_avg_y() const;
  double dist_to_point(double x, double y) const;
  double dist_to_ctr(double x, double y) const;
  double max_dist_to_ctr() const;
  bool   segs_cross(bool loop=true) const;
  double length();

  std::string get_spec(unsigned int vertex_prec=1) const;
  std::string get_spec(std::string param) const;
  std::string get_spec(unsigned int vertex_prec, std::string param) const;
  std::string get_spec_pts(unsigned int vertex_prec=1) const;

  unsigned int closest_vertex(double, double) const; 
  unsigned int closest_segment(double, double, bool implseg=true) const;

protected:
  void   grow_pt_by_pct(double, double, double, double&, double&);
  void   grow_pt_by_amt(double, double, double, double&, double&);
  void   rotate_pt(double, double, double, double&, double&);

protected:
  std::vector<double> m_vx;
  std::vector<double> m_vy;
  std::vector<double> m_vz;
  std::vector<std::string> m_vprop;

  double   m_transparency;

};

#endif




