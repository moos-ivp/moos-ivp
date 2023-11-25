/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BuildUtilsBeta.h                                     */
/*    DATE: March 15th, 2023                                     */
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

#ifndef BUILD_UTIL_HEADER
#define BUILD_UTIL_HEADER

#include <string>
#include "PDMap.h"
#include "BoxSet.h"

//--------------------------------------------------------------------
// Generic Box Utils

IvPBox  build1DBox(IvPDomain, std::string domstr, double vmin, double vmax);
IvPBox  build1DBox(IvPDomain, std::string domstr, double val);

IvPBox  build2DBox(IvPDomain, std::string domstr1, std::string domstr2,
		   double vmin1, double vmax1, double vmin2, double vmax2);
IvPBox  build2DPointBox(IvPDomain, std::string domstr1, std::string domstr2,
			double val1, double val2);


#endif


