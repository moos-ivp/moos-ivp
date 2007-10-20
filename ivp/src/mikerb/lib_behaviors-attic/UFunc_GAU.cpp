/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: UFunc_GAU.cpp                                        */
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
#include "UFunc_GAU.h"

//----------------------------------------------------------
// Procedure: Constructor

UFunc_GAU::UFunc_GAU(const char*  g_label,
		     double g_mean,
		     double g_stdDev) :
  UFunc(g_label)
{
  mean   = g_mean;
  stdDev = g_stdDev;
}

//----------------------------------------------------------
// Procedure: Constructor

UFunc_GAU::UFunc_GAU() : UFunc("domain")
{
  mean   = 0.0;
  stdDev = 1.0;
}

//----------------------------------------------------------------
// Procedure: eval

double UFunc_GAU::eval(double val) const
{
  double e    = 2.718281;
  double sqrt2pi = 2.506628;

  double v1 = 1.0 / (stdDev * sqrt2pi);
  double v2 = -0.5 * ( pow(val-mean,2) / pow(stdDev,2) );

  return(v1 * pow(e,v2));
}







