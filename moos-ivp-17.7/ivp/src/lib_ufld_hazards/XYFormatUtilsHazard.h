/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYFormatUtilsHazard.h                                */
/*    DATE: Jan 27th, 2012                                       */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
 
#ifndef XY_FORMAT_UTILS_HAZARD_HEADER
#define XY_FORMAT_UTILS_HAZARD_HEADER

#include <string>
#include "XYHazard.h"

//---------------------------------------------------------------
// Create an XYHazard from a string specification. This function
// calls one of the string*2Hazard functions below. Only this 
// function should be called by the user. The other functions
// are subject to change without regard to backward compatibility.
XYHazard string2Hazard(std::string);

// *** IMPORTANT NOTE **** 
// The below functions are not intended to be invoked directly by 
// the user. They are subortinate functions called by the above
// function. While the above function will be maintained in the 
// future for backward compatibility, the below functions are not.

//---------------------------------------------------------------
// Create an XYHazard from a string specification. 
// This function is standard because it processes the string format
// used when a string is created from an existing XYHazard instance.
// Example: x=val, y=val, type=val
XYHazard stringStandard2Hazard(std::string);

#endif









