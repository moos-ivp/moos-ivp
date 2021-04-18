/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYEncoders.h                                         */
/*    DATE: Nov 24th 2006 (Thanksgiving Waterloo)                */
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

#ifndef XY_ENCODERS_HEADER
#define XY_ENCODERS_HEADER

#include <string>
#include "XYGrid.h"
#include "XYSquare.h"

// Convert an XYSquare to string represntation
std::string XYSquareToString(const XYSquare&);

// Convert an XYGrid to string represntation
std::string XYGridToString(const XYGrid&);

// Create an XYGrid based on a string representation
XYGrid StringToXYGrid(const std::string&);

#endif



















