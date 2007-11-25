/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: FunctionEncoder.h                                    */
/*    DATE: April 17th 2006 (Patriots Day)                       */
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

#endif










