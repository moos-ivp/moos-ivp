/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: UFunc_LIN.cpp                                        */
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

#include <math.h>
#include "UFunc_LIN.h"

//----------------------------------------------------------
// Procedure: Constructor

UFunc_LIN::UFunc_LIN(const char*  g_label,
		     double g_slope,
		     double g_intercept) :
  UFunc(g_label)
{
  slope     = g_slope;
  intercept = g_intercept;
}

//----------------------------------------------------------
// Procedure: Constructor

UFunc_LIN::UFunc_LIN() : UFunc("domain")
{
  slope     = 0.0;
  intercept = 0.0;
}



//----------------------------------------------------------------
// Procedure: eval

double UFunc_LIN::eval(double val) const
{
  return((slope*val)+intercept);
}







