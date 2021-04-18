/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CircularUtils.h                                      */
/*    DATE: Jan 1st, 2007 Monday afternoon at Brueggers          */
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
 
#ifndef XY_CIRCULAR_UTILS_HEADER
#define XY_CIRCULAR_UTILS_HEADER

#include <string>
#include "XYArc.h"

// Determine a new point and heading given a starting point, heading
// and arc radius and port/starboard indication.
bool   arcFlight(double x1, double y1, double heading, double radius,
		 double dist, bool port, double& rx, double& ry,
		 double& rheading);

XYArc  arcFlight(double x1, double y1, double heading, double radius,
		 double dist, bool port);


#endif


















