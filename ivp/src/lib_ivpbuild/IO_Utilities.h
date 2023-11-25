/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: IO_Utilities.h                                       */
/*    DATE: June 2nd 2004                                        */
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

#ifndef IO_UTILITIES_HEADER
#define IO_UTILITIES_HEADER

#include <vector>
#include <string>
#include <cstdio>
#include "IvPProblem.h"
#include "IvPFunction.h"
#include "ZAIC_PEAK.h"

bool   saveProblem(IvPProblem*, const std::string&);
bool   saveFunction(IvPFunction *ipf, const std::string&, bool append);
bool   savePDMap(PDMap*, FILE*);

IvPFunction* readFunction(const std::string&);

std::vector<IvPFunction*> readFunctions(const std::string&);
PDMap*       readPDMap(FILE *f, int dim, int cnt, IvPDomain dom, int deg);

void   printZAIC_PEAK(ZAIC_PEAK);

#endif


















