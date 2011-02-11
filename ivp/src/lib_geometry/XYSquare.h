/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
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

class XYSquare {
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

  double getVal(int, int) const;
  void   shiftX(double v)     {xlow+=v; xhigh+=v;};
  void   shiftY(double v)     {ylow+=v; yhigh+=v;};
  double getLengthX() const   {return(xhigh-xlow);};
  double getLengthY() const   {return(yhigh-ylow);};
  bool   isValid() const      {return(valid_state);};
  
  double getCenterX() const   {return((xhigh-xlow)/2 + xlow);};
  double getCenterY() const   {return((yhigh-ylow)/2 + ylow);};

protected:
  double xlow;
  double xhigh;
  double ylow;
  double yhigh;
  bool   valid_state;
};
#endif









