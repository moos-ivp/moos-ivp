/*****************************************************************/
/*    NAME: Andrew Shafer                                        */
/*    ORGN: MIT                                                  */
/*    FILE: ArtifactUtils.h                                      */
/*    DATE: Oct 24 2007                                          */
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

#ifndef XY_ARTIFACTUTILS_HEADER
#define XY_ARTIFACTUTILS_HEADER


#include "XYSegList.h"
#include "XYPolygon.h"

const XYSegList generateLawnmower(const XYPolygon&, double, double, double, double, bool);
const XYSegList generateLawnmowerFull(const XYPolygon&, double, double);
const std::vector<std::string> generateArtifacts(double, double, double, double, double, double, const XYPolygon&);


#endif

