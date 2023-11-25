/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYSquare.h                                           */
/*    DATE: Aug 27th 2005                                        */
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

#ifndef SQUARE_XY_HEADER
#define SQUARE_XY_HEADER

#include <string>
#include "XYObject.h"

class XYSquare : public XYObject {
public:
  XYSquare();
  XYSquare(double xylen);
  XYSquare(double xlen, double ylen);
  XYSquare(double xl, double hx, double yl, double yh);
  bool operator==(const XYSquare&) const;
  bool operator!=(const XYSquare&) const;
  virtual ~XYSquare() {}

  void   set(double, double, double, double);
  bool   containsPoint(double, double) const;
  double segIntersectLength(double, double, double, double) const;
  double segDistToSquare(double, double, double, double) const;
  double ptDistToSquareCtr(double, double) const;

  double get_min_x() const {return(m_xlow);}
  double get_max_x() const {return(m_xhigh);}
  double get_min_y() const {return(m_ylow);}
  double get_max_y() const {return(m_yhigh);}


  double getVal(int, int) const;
  void   shiftX(double v)     {m_xlow+=v; m_xhigh+=v;}
  void   shiftY(double v)     {m_ylow+=v; m_yhigh+=v;}
  double getLengthX() const   {return(m_xhigh - m_xlow);}
  double getLengthY() const   {return(m_yhigh - m_ylow);}
  bool   valid() const        {return(m_valid);}
  
  double getCenterX() const   {return((m_xhigh-m_xlow)/2 + m_xlow);}
  double getCenterY() const   {return((m_yhigh-m_ylow)/2 + m_ylow);}

  std::string get_spec(std::string s="") const;
  std::string get_spec_as_poly(std::string s="") const;

protected:
  double m_xlow;
  double m_xhigh;
  double m_ylow;
  double m_yhigh;
  bool   m_valid;
};
#endif



















