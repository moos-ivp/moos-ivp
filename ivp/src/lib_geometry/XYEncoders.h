/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYEncoders.h                                         */
/*    DATE: Nov 24th 2006 (Thanksgiving Waterloo)                */
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

#ifndef XY_ENCODERS_HEADER
#define XY_ENCODERS_HEADER

#include <string>
#include "XYGrid.h"
#include "XYSegList.h"
#include "XYPolygon.h"
#include "XYSquare.h"

// Convert an XYSegList to string represntation
std::string XYSegListToString(const XYSegList&);

// Convert an XYPolygon to string represntation
std::string XYPolygonToString(const XYPolygon&, int digits=0);

// Convert an XYSquare to string represntation
std::string XYSquareToString(const XYSquare&);

// Convert an XYGrid to string represntation
std::string XYGridToString(const XYGrid&);

// Create an XYGrid based on a string representation
XYGrid StringToXYGrid(const std::string&);

#endif










