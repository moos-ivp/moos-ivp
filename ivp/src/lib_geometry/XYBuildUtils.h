/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYBuildUtils.h                                       */
/*    DATE: May 16, 2008 Sunday afternoon at Brueggers           */
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
 
#ifndef XY_GEOM_STR_UTILS_HEADER
#define XY_GEOM_STR_UTILS_HEADER

#include <string>
#include "XYPolygon.h"
#include "XYSegList.h"

//---------------------------------------------------------------
// Create a polygon from a string specification. This function will
// call one of the string*Poly(string) functions below.
XYPolygon stringToPoly(std::string);

// Create a polygon that approximates an ellipse
// Example: "type=elipse, x=val, y=val, major=val, minor=val, 
//    degs=val, rads=val, pts=val, snap_value=val, label=val"
XYPolygon stringPairsToEllipsePoly(std::string);

// Create a rectangle from two given points
// Example: "x1=val, y1=val, x2=val, y2=val, axis_pad=val, 
//          perp_pad=val, snap_val=val, label=val"
XYPolygon stringPairsToPylonPoly(std::string);

// Create a polygon that approximates a circle.
// Example: "radial:: x=val, y=val, radius=val, pts=val, snap=val, 
//    label=val"
XYPolygon stringPairsToRadialPoly(std::string);

// Create a polygon that approximates a circle.
// Examples: "radial: px, py, prad, ppts, snapval, label"
//           "px, py, prad, ppts, snapval, label"
XYPolygon stringShortToRadialPoly(std::string);

// Create a polygon given a set of points
// Examples: "pts:    10,15 : 20,25 : 30,35 : label,foobar"
//           "points: label, foobar : 10,15 : 20,25 : 30,35"
XYPolygon stringShortToPointsPoly(std::string);


//---------------------------------------------------------------

// Create an XYSegList from a string specification. This function 
// will call one of the string*SegList(string) functions below.
XYSegList stringToSegList(std::string);


// Create a seglist given a set of points
// Examples: "pts:    10,15 : 20,25 : 30,35 : label,foobar"
//           "points: label, foobar : 10,15 : 20,25 : 30,35"
XYSegList stringShortToPointsSegList(std::string);

// Create a seglist given a set of points describing a zigzag
// Example: "zigzag: startx, starty, angle, length, period,
//            amplitude, snapval"
// Example: "startx, starty, angle, length, period,
//            amplitude"
XYSegList stringShortToZigZagSegList(std::string);
  
#endif
