/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYPoint.h                                            */
/*    DATE: July 17th, 2008                                      */
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
 
#ifndef XY_POINT_HEADER
#define XY_POINT_HEADER

#include <string>
#include "XYObject.h"

class XYPoint : public XYObject {
 public:
  XYPoint()
  {m_x=0; m_y=0; m_z=0; m_valid=false; m_sdigits=2; m_pt_trans=0;}
  XYPoint(double x, double y, double z=0)
  {m_x=x; m_y=y; m_z=z; m_valid=true; m_sdigits=2; m_pt_trans=0;}
  XYPoint(double x, double y, std::string l)
  {m_x=x; m_y=y; m_z=0; m_valid=true; m_sdigits=2; m_pt_trans=0; m_label=l;}

  virtual ~XYPoint() {}
  
  void clear();
  
  void set_vertex(double x, double y, double z=0) 
    {m_x=x; m_y=y; m_z=z; m_valid=true;}

  void set_vx(double v)       {m_x=v;}
  void set_vy(double v)       {m_y=v;}
  void set_vz(double v)       {m_z=v;}
  void set_pt_trans(double);
  
  void set_spec_digits(unsigned int v);
  void invalidate() {m_valid=false;}
  
public:
  void shift_x(double val) {m_x += val;}
  void shift_y(double val) {m_y += val;}
  void shift_z(double val) {m_z += val;}
  void apply_snap(double snapval);

public:
  double get_vx() const       {return(m_x);}
  double get_vy() const       {return(m_y);}
  double get_vz() const       {return(m_z);}
  double get_pt_trans() const {return(m_pt_trans);}
  double x() const            {return(m_x);}
  double y() const            {return(m_y);}
  double z() const            {return(m_z);}
  bool   valid() const        {return(m_valid);}
  double magnitude() const    {return((m_x*m_x)+(m_y*m_y));}
  
  void   projectPt(const XYPoint&, double ang, double dist);

  std::string get_spec_xy(char c=',') const;
  std::string get_spec(std::string s="") const;
  std::string get_spec_inactive() const;

protected:
  double m_x;
  double m_y;
  double m_z;
  bool   m_valid;
  int    m_sdigits;
  double m_pt_trans;
};

bool operator< (const XYPoint& one, const XYPoint& two);
bool operator== (const XYPoint& one, const XYPoint& two);


#endif

