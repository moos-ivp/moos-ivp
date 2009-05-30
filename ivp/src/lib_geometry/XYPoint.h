/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYPoint.h                                            */
/*    DATE: July 17th, 2008                                      */
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
 
#ifndef XY_POINT_HEADER
#define XY_POINT_HEADER

#include <string>
#include "XYObject.h"

class XYPoint : public XYObject {
public:
  XYPoint() {m_x=0; m_y=0; m_z=0; m_size=1; m_valid=false;};
  XYPoint(double x, double y) 
    {m_x=x; m_y=y; m_z=0; m_size=1; m_valid=true;};

  virtual ~XYPoint() {};

  void set_vertex(double x, double y, double z=0) 
    {m_x=x; m_y=y; m_z=z; m_valid=true;};

  void clear();
  
public:
  void   shift_horz(double val) {m_x += val;};
  void   shift_vert(double val) {m_y += val;};;
  void   apply_snap(double snapval);
  void   print() const;

public:
  void      set_point_color(const std::string&);
  void      set_size(double val)    {m_size=val;};
  double    get_vx() const          {return(m_x);};
  double    get_vy() const          {return(m_y);};
  double    get_vz() const          {return(m_z);};
  double    get_size() const        {return(m_size);};
  bool      valid() const           {return(m_valid);};

  ColorPack get_point_color() const {return(m_point_color);};
  bool      point_color_set() const {return(m_point_color.set());};

  XYPoint   projectPt(XYPoint, double dist, double angle) const;

  std::string get_spec() const;

protected:
  double    m_x;
  double    m_y;
  double    m_z;
  double    m_size;
  bool      m_valid;

  ColorPack m_point_color;
};

#endif








