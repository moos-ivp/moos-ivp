/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: IO_GeomUtils.h                                       */
/*    DATE: Sep 4th, 2005                                        */
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

#ifndef IO_GEOM_UTILS_HEADER
#define IO_GEOM_UTILS_HEADER

#include <string>
#include <vector>
#include "XYPolygon.h"
#include "XYGrid.h"
#include "XYSquare.h"
#include "XYCircle.h"
#include "XYArc.h"
#include "XYHexagon.h"

std::vector<std::string> readPolyStringsFromFile(const std::string&);

std::vector<XYPolygon> readPolysFromFile(const std::string&);
std::vector<XYGrid>    readGridsFromFile(const std::string&);
std::vector<XYCircle>  readCirclesFromFile(const std::string&);
std::vector<XYArc>     readArcsFromFile(const std::string&);
std::vector<XYHexagon> readHexagonsFromFile(const std::string&);

std::string  GridToString(const XYGrid&);

void printSquare(const XYSquare&);

#endif












