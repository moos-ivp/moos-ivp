/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYFormatUtilsPoly.h                                  */
/*    DATE: May 16, 2009 After bbq @ Hows                        */
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
 
#ifndef XY_FORMAT_UTILS_POLY_HEADER
#define XY_FORMAT_UTILS_POLY_HEADER

#include <string>
#include "XYPolygon.h"

//---------------------------------------------------------------
// #0 Create a polygon from a string specification. This function
// will call one of the string*Poly(string) functions below.
XYPolygon string2Poly(std::string);

// *** IMPORTANT NOTE **** 
// The below functions are not intended to be invoked directly by 
// the user. They are subortinate functions called by the above
// function. While the above function will be maintained in the 
// future for backward compatibility, the below functions are not.

// #1 Create a polygon given a set of points
// Examples: [pts="10,15:20,25:30,35", label=foobar]
XYPolygon stringStandard2Poly(std::string);

// #2 Create a polygon given a set of points
// Examples: [0,0:10,0:5,5]
//           [10,15:20,25:30,35:label,foobar]
XYPolygon stringAbbreviated2Poly(std::string);

//---------------------------------------------------------------
// #3 Create a polygon that approximates an ellipse
// Example: "ellipse:: x=val, y=val, major=val, minor=val, 
//    degs=val, rads=val, pts=val, snap_value=val, label=val"
XYPolygon stringEllipse2Poly(std::string);

// #4 Create a polygon that approximates a circle.
// Example: "radial:: x=val, y=val, radius=val, pts=val, snap=val, 
//    label=val"
XYPolygon stringRadial2Poly(std::string);

// #5 Create a polygon that approximates a circle.
// Example: "radial:xval,yval,radius,pts,snapval,label 
XYPolygon stringShortRadial2Poly(std::string);

// #6 Create a polygon that approximates radial pie wedge
// Example: "wedge:: x=val, y=val, lang=val, rang=val,
//    pts=val, range=val, snap_value=val, label=val"
XYPolygon stringPieWedge2Poly(std::string);

// #7 Create a polygon that approximates radial range wedge
// Example: "type=wedge, x=val, y=val, lang=val, rang=val,
//    pts=val, range=val, snap_value=val, label=val"
XYPolygon stringRangeWedge2Poly(std::string);

// #8 Create a rectangle from two given points
// Example: "x1=val, y1=val, x2=val, y2=val, axis_pad=val, 
//          perp_pad=val, snap_val=val, label=val"
XYPolygon stringPylon2Poly(std::string);

// Below added Nov 27th, 2023
bool setPolyOnString(XYPolygon& poly, std::string spec,
		     std::string label="");

// Below added Jan 11th, 2024
XYPolygon stringOval2Poly(std::string);

#endif










