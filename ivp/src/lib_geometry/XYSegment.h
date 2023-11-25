/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYSegment.h                                          */
/*    DATE: Mar 28, 2009                                         */
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
 
#ifndef XY_SEGMENT_HEADER
#define XY_SEGMENT_HEADER

#include <string>
#include "XYObject.h"
#include "XYPoint.h"

class XYSegment : public XYObject {
public:
  XYSegment();
  XYSegment(double x1, double y1, double x2, double y2);
  virtual ~XYSegment() {}

  // XYSegList create and edit functions
  void   set(double x1, double y1, double x2, double y2);
  void   set(const XYPoint&, const XYPoint);
  void   clear();

public:
  void   shift_horz(double val);
  void   shift_vert(double val);
  void   apply_snap(double snapval);
  void   reverse();

  bool   intersects(XYSegment);
  
public:
  double length() const {return(m_length);}
  double get_x1() const {return(m_x1);}
  double get_y1() const {return(m_y1);}
  double get_x2() const {return(m_x2);}
  double get_y2() const {return(m_y2);}
  double getRAng12() const {return(m_rang12);}
  double getRAng21() const {return(m_rang21);}

  std::string get_spec(int vertex_precision=1) const;

protected:
  double m_x1;
  double m_y1;
  double m_x2;
  double m_y2;

  double m_length;
  double m_rang12;
  double m_rang21;
};

#endif











