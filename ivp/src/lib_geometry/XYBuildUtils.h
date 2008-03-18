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

XYPolygon stringToPoly(std::string);

XYPolygon stringPairsToEllipsePoly(std::string);
XYPolygon stringPairsToRadialPoly(std::string);
XYPolygon stringShortToRadialPoly(std::string);
XYPolygon stringShortToPointsPoly(std::string);

  
#endif
