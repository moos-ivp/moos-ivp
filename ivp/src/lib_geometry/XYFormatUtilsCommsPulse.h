/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYFormatUtilsCommsPulse.h                            */
/*    DATE: Dec 5th, 2011                                        */
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
 
#ifndef XY_FORMAT_UTILS_COMMS_PULSE_HEADER
#define XY_FORMAT_UTILS_COMMS_PULSE_HEADER

#include <string>
#include "XYCommsPulse.h"

//---------------------------------------------------------------
// Create an XYCommsPulse from a string specification. This function
// calls one of the string*2Point functions below. This is the only
// function that should be called by the user. The other functions
// are subject to change without regard to backward compatibility.
XYCommsPulse string2CommsPulse(std::string);

// *** IMPORTANT NOTE **** 
// The below functions are not intended to be invoked directly by 
// the user. They are subortinate functions called by the above
// function. While the above function will be maintained in the 
// future for backward compatibility, the below functions are not.

//---------------------------------------------------------------
// Create an XYCommsPulse from a string specification. 
// This function is standard because it processes the string format
// used when a string is created from an existing XYCommsPulse instance.
// Example: sx=val, sy=val, tx=val, ty=val, beam_width=10, 
//          duration=3, fill=0.5, label=bobby

XYCommsPulse stringStandard2CommsPulse(std::string);

#endif










