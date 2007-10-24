/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: UFunc_LIN.H                                          */
/*    DATE: June 16th, 2003                                      */
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

#ifndef UFUNC_LIN_HEADER
#define UFUNC_LIN_HEADER

#include "UFunc.h"

class UFunc_LIN: public UFunc {
public:
  UFunc_LIN(const char*, double, double);
  UFunc_LIN();
  ~UFunc_LIN() {};
  UFunc_LIN &operator=(const UFunc_LIN &rhs) {
    setLabel(rhs.label);
    slope = rhs.slope;
    intercept = rhs.intercept;
  }

public:
  double  eval(double) const; // virtual defined

  double getSlope() const  {return(slope);};
  double getIntercept() const   {return(intercept);};

  void   setSlope(double v)     {slope = v;};
  void   setIntercept(double v) {intercept = v;};

protected:
  double  slope;
  double  intercept;
};

#endif






