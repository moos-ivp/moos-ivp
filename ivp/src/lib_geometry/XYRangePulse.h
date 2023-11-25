/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYRangePulse.h                                       */
/*    DATE: Feb 4th 2011                                         */
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

#ifndef XY_RANGE_PULSE_HEADER
#define XY_RANGE_PULSE_HEADER

#include <string>
#include <vector>
#include "XYObject.h"

class XYRangePulse : public XYObject {
public:
  XYRangePulse();
  XYRangePulse(double x, double y);
  virtual ~XYRangePulse() {}

  void   set_x(double v)          {m_x=v; m_x_set=true;}
  void   set_y(double v)          {m_y=v; m_y_set=true;}
  void   set_rad(double);
  void   set_linger(double);
  void   set_fill(double);
  void   set_fill_invariant(bool v) {m_fill_invariant=v;}

  double get_x()    const         {return(m_x);}
  double get_y()    const         {return(m_y);}
  double get_radius()  const      {return(m_rad);}
  double get_linger() const       {return(m_linger);}
  double get_fill() const         {return(m_fill);}
  double get_fill(double timestamp) const;

  bool   valid() const;

  std::string get_spec(std::string s="") const;

  std::vector<double> get_circle(double time, unsigned int pts=180) const;

 protected:
  void initialize();

protected:
  double    m_x;
  double    m_y;
  double    m_rad;
  double    m_linger;

  bool      m_x_set;
  bool      m_y_set;

  double    m_fill;
  bool      m_fill_invariant;
};
#endif

