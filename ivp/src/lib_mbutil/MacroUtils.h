/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: MacroUtils.h                                         */
/*    DATE: June 24th, 2020                                      */
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
 
#ifndef MACRO_UTILS_HEADER
#define MACRO_UTILS_HEADER

#include <string>
#include <vector>

std::string macroExpand(std::string, std::string, std::string);

std::string macroExpandBool(std::string, std::string, bool);

std::string macroExpand(std::string, std::string, double, int v=3);

std::string macroExpand(std::string, std::string, int);

std::string macroExpand(std::string, std::string, unsigned int);

std::string macroHashExpand(std::string, std::string);

bool hasMacro(std::string, std::string);

std::string getCounterMacro(std::string);

std::vector<std::string> getMacrosFromString(std::string,
					     char fchar='$',
					     char lchar='[');
std::string macroDefault(std::string);
std::string macroBase(std::string, std::string sep="*=");

std::string expandMacrosWithDefault(std::string);
std::string reduceMacrosToBase(std::string, std::string, std::string);

#endif

