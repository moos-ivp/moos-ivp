/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IO_Utilities.h                                       */
/*    DATE: June 2nd 2004                                        */
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

#ifndef IO_UTILITIES_HEADER
#define IO_UTILITIES_HEADER

#include <vector>
#include <string>
#include <stdio.h>
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









