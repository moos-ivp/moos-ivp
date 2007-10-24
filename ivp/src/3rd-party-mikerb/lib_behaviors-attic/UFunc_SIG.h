/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: UFunc_SIG.H                                          */
/*    DATE: June 14th, 2003                                      */
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

#ifndef UFUNC_SIG_HEADER
#define UFUNC_SIG_HEADER

#include "UFunc.h"

class UFunc_SIG: public UFunc {
public:
  UFunc_SIG(const char*  g_label,
	    double g_rangeLow,  double g_rangeHgt,
	    double g_stepPoint, double g_exp);
  UFunc_SIG();
  ~UFunc_SIG() {};
  UFunc_SIG &operator=(const UFunc_SIG &rhs) {
    setLabel(rhs.label);
    rangeLow = rhs.rangeLow;
    rangeHgt = rhs.rangeHgt;
    stepPoint = rhs.stepPoint;
    exp = rhs.exp;
  }

public:
  double  eval(double) const; // virtual defined

  double  getRangeLow() const  {return(rangeLow);};
  double  getRangeHgt() const  {return(rangeHgt);};
  double  getStepPoint() const {return(stepPoint);};
  double  getExp() const       {return(exp);};

  void    setRangeLow(double v)  {rangeLow = v;};
  void    setRangeHgt(double v)  {rangeHgt = v;};
  void    setStepPoint(double v) {stepPoint = v;};
  void    setExp(double v)       {exp = v;};

protected:
  double  rangeLow;
  double  rangeHgt;
  double  stepPoint;
  double  exp;
};

#endif






