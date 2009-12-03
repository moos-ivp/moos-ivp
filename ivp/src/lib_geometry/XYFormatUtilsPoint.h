/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYFormatUtilsPoint.h                                 */
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
 
#ifndef XY_FORMAT_UTILS_POINT_HEADER
#define XY_FORMAT_UTILS_POINT_HEADER

#include <string>
#include "XYPoint.h"

//---------------------------------------------------------------
// Create a point from a string specification. 
// Example: label=bobby, type=destination, x=val, y=val, z=val
XYPoint string2Point(std::string);

XYPoint stringPairs2Point(std::string);
XYPoint stringShort2Point(std::string);


#endif
