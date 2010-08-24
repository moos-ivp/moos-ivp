/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: CircularUtils.h                                      */
/*    DATE: Jan 1st, 2007 Monday afternoon at Brueggers          */
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









