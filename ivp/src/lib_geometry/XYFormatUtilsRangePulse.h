/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYFormatUtilsRangePulse.h                            */
/*    DATE: Feb 5th, 2011                                        */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
 
#ifndef XY_FORMAT_UTILS_RANGE_PULSE_HEADER
#define XY_FORMAT_UTILS_RANGE_PULSE_HEADER

#include <string>
#include "XYRangePulse.h"

//---------------------------------------------------------------
// Create an XYRangePulse from a string specification. This function
// calls one of the string*2RangePulse functions below. Only this 
// function should be called by the user. The other functions
// are subject to change without regard to backward compatibility.
XYRangePulse string2RangePulse(std::string);

// *** IMPORTANT NOTE **** 
// The below functions are not intended to be invoked directly by 
// the user. They are subortinate functions called by the above
// function. While the above function will be maintained in the 
// future for backward compatibility, the below functions are not.

//---------------------------------------------------------------
// Create an XYRangePulse from a string specification. 
// This function is standard because it processes the string format
// used when a string is created from an existing XYRangePulse instance.
// Example: x=val, y=val, radius=val, duration=val, label=bobby
XYRangePulse stringStandard2RangePulse(std::string);

#endif









