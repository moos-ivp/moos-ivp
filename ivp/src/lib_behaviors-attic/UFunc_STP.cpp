/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: UFunc_STP.cpp                                        */
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
#include <assert.h>
#include "UFunc_STP.h"


//----------------------------------------------------------
// Procedure: Constructor
//
//               0   stepType   1
//
//                       -----------  <-- rightVal
//                       |
//                       |
//                       |
//  leftVal -->   -------|
//                   stepPoint

UFunc_STP::UFunc_STP(const char*  g_label,
		     double g_leftVal,
		     double g_rightVal,
		     double g_stepPoint,
		     int    g_stepType) :

  UFunc(g_label)
{
  leftVal   = g_leftVal;
  rightVal  = g_rightVal;
  stepPoint = g_stepPoint;
  stepType  = g_stepType;
}

//----------------------------------------------------------
// Procedure: Constructor

UFunc_STP::UFunc_STP() : UFunc("domain")
{
  leftVal   = 0.0;
  rightVal  = 1.0;
  stepPoint = 0.0;
  stepType  = 1;
}


//----------------------------------------------------------------
// Procedure: eval
//      Note: If val equals stepPoint, returns:
//            rightVal if stepType == 1
//            leftVal  if stepType == 0

double UFunc_STP::eval(double val) const
{
  if(val < stepPoint) return(leftVal);
  if((val > stepPoint) || stepType)
     return(rightVal);
}







