/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: FunctionEncoder.h                                    */
/*    DATE: April 17th 2006 (Patriots Day)                       */
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

#ifndef FUNCTION_ENCODER_HEADER
#define FUNCTION_ENCODER_HEADER

#include <string>
#include "IvPFunction.h"

// Convert an IvPFunction to string represntation
std::string IvPFunctionToString(IvPFunction*);

// Convert an IvPFunction to a vector of strings
std::vector<std::string> IvPFunctionToVector(const std::string&, 
					     const std::string&, int);

// Create an IvPFunction based on a string representation
IvPFunction *StringToIvPFunction(const std::string&);

// Create an IvPFunction Context String without building the function
std::string StringToIvPContext(const std::string&);

// Extract the IvPDomain without building the function
IvPDomain IPFStringToIvPDomain(const std::string&);

#endif






