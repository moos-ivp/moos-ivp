/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: UFunc_STP.H                                          */
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

#ifndef UFUNC_STP_HEADER
#define UFUNC_STP_HEADER

#include "UFunc.h"

class UFunc_STP: public UFunc {
public:
  UFunc_STP(const char*, double, double, double, int);
  UFunc_STP();
  ~UFunc_STP() {};
  UFunc_STP &operator=(const UFunc_STP &rhs) {
    setLabel(rhs.label);
    leftVal = rhs.leftVal;
    rightVal = rhs.rightVal;
    stepPoint = rhs.stepPoint;
    stepType = rhs.stepType;
  }

public:
  double  eval(double) const; // virtual defined

  double  getLeftVal() const   {return(leftVal);};
  double  getRightVal() const  {return(rightVal);};
  double  getStepPoint() const {return(stepPoint);};
  int     getStepType() const  {return(stepType);};

  void    setLeftVal(double v)   {leftVal = v;};
  void    setRightVal(double v)  {rightVal = v;};
  void    setStepPoint(double v) {stepPoint = v;};
  void    setStepType(int v)     {stepType = v;};

protected:
  double  leftVal;
  double  rightVal;
  double  stepPoint;
  int     stepType;
};

#endif







