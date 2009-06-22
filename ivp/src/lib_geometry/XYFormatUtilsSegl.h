/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYFormatUtilsSegl.h                                  */
/*    DATE: May 18th, 2009 (Reworked from XYBuildUtils.h)        */
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
 
#ifndef XY_FORMAT_UTILS_SEGLIST_HEADER
#define XY_FORMAT_UTILS_SEGLIST_HEADER

#include <string>
#include "XYPolygon.h"
#include "XYSegList.h"
#include "XYPoint.h"

// Create an XYSegList from a string specification. This function 
// will call one of the string*SegList(string) functions below.
XYSegList string2SegList(std::string);

// Create a seglist given a set of points
// Examples: "pts:    10,15 : 20,25 : 30,35 : label,foobar"
//           "points: label, foobar : 10,15 : 20,25 : 30,35"
XYSegList stringPoints2SegList(std::string);

// Create a seglist given a set of points describing a zigzag
// Example: "zigzag: startx, starty, angle, length, period,
//            amplitude, snapval"
// Example: "startx, starty, angle, length, period,
//            amplitude"
XYSegList stringZigZag2SegList(std::string);

// Create a seglist given a set of points for a lawnmower pattern
// Example: "format=lawnmower, x=0, y=8, width=100, height=80, 
//          degs=45, swath=20, startx=-40, starty=80, start=tl"

XYSegList stringLawnmower2SegList(std::string);

#endif
