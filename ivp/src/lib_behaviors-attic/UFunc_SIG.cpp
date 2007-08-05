/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: UFunc_SIG.cpp                                        */
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

#include <math.h>
#include <assert.h>
#include "UFunc_SIG.h"

//----------------------------------------------------------
// Procedure: Constructor

UFunc_SIG::UFunc_SIG(const char*  g_label,
		     double g_rangeLow,
		     double g_rangeHgt,
		     double g_stepPoint,
		     double g_exp) :
  UFunc(g_label)
{
  rangeLow  = g_rangeLow;
  rangeHgt  = g_rangeHgt;
  stepPoint = g_stepPoint;
  exp       = g_exp;

  assert(rangeHgt > 0);
}

//----------------------------------------------------------
// Procedure: Constructor

UFunc_SIG::UFunc_SIG() : UFunc("domain")
{
  rangeLow  = 0.0;
  rangeHgt  = 1.0;
  stepPoint = 0.5;
  exp       = 0.5;
}



//----------------------------------------------------------------
// Procedure: eval

double UFunc_SIG::eval(double val) const
{
  double e = 2.718281;

  double denom = (1.0+ pow(e,(-1.0*exp*(val-stepPoint))));

  return(rangeLow + (rangeHgt/denom));
}







