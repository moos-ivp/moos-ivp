/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ColorParse.h                                         */
/*    DATE: Aug 19th 2006                                        */
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

#ifndef COLOR_PARSE_UTIL_HEADER
#define COLOR_PARSE_UTIL_HEADER

#include <vector>
#include <string>

std::vector<double> colorParse(const std::string&);
std::vector<double> colorParse(const std::string&, bool& result);
std::vector<double> colorHexToVector(const std::string&);
std::vector<double> colorDecToVector(const std::string&);
std::string         colorNameToHex(const std::string&);
std::string         colorVectorToString(const std::vector<double>&);
std::string         termColor(const std::string& color="");

std::string         removeTermColors(std::string);

bool setColorOnString(std::string& color, std::string given_color);

bool isColor(const std::string&);
bool isTermColor(const std::string&);

void blu(const std::string&, const std::string& s="");
void blk(const std::string&, const std::string& s="");
void red(const std::string&, const std::string& s="");
void grn(const std::string&, const std::string& s="");
void mag(const std::string&, const std::string& s="");

double rOfRGB(const std::string);
double gOfRGB(const std::string);
double bOfRGB(const std::string);

#endif










