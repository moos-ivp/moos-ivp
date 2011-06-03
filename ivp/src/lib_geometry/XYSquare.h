/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYSquare.h                                           */
/*    DATE: Aug 27th 2005                                        */
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

#ifndef SQUARE_XY_HEADER
#define SQUARE_XY_HEADER

#include <string>
#include "XYObject.h"

class XYSquare : public XYObject {
public:
  XYSquare();
  XYSquare(double, double, double, double);
  bool operator==(const XYSquare&) const;
  bool operator!=(const XYSquare&) const;
  virtual ~XYSquare() {};

  void   set(double, double, double, double);
  bool   containsPoint(double, double) const;
  double segIntersectLength(double, double, double, double) const;
  double segDistToSquare(double, double, double, double) const;
  double ptDistToSquareCtr(double, double) const;

  double get_min_x() const {return(m_xlow);};
  double get_max_x() const {return(m_xhigh);};
  double get_min_y() const {return(m_ylow);};
  double get_max_y() const {return(m_yhigh);};


  double getVal(int, int) const;
  void   shiftX(double v)     {m_xlow+=v; m_xhigh+=v;};
  void   shiftY(double v)     {m_ylow+=v; m_yhigh+=v;};
  double getLengthX() const   {return(m_xhigh - m_xlow);};
  double getLengthY() const   {return(m_yhigh - m_ylow);};
  bool   valid() const        {return(m_valid);};
  
  double getCenterX() const   {return((m_xhigh-m_xlow)/2 + m_xlow);};
  double getCenterY() const   {return((m_yhigh-m_ylow)/2 + m_ylow);};

  std::string get_spec(std::string s="") const;

protected:
  double m_xlow;
  double m_xhigh;
  double m_ylow;
  double m_yhigh;
  bool   m_valid;
};
#endif










