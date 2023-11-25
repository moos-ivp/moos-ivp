/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ArcUtilsToDo.h                                       */
/*    DATE: Nov 2nd, 2018 Friday evening on campus               */
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

#ifndef GEOM_ARC_UTILS_2DO_HEADER
#define GEOM_ARC_UTILS_2DO_HEADER

#include <vector>
#include "XYSegList.h"

double distSegToArcPts(double x1, double y1, double x2, double y2,
		       double ax, double ay, double ar,
		       double langle, double rangle, double thresh,
		       std::vector<double>& vix,
		       std::vector<double>& viy,
		       std::vector<double>& dist);
#endif


