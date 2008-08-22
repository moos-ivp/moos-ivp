/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ColorParse.h                                         */
/*    DATE: Aug 19th 2006                                        */
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

#ifndef COLOR_PARSE_UTIL_HEADER
#define COLOR_PARSE_UTIL_HEADER

#include <vector>
#include <string>

std::vector<double> colorParse(const std::string&);
std::vector<double> colorHexToVector(const std::string&);
std::vector<double> colorDecToVector(const std::string&);
std::string         colorNameToHex(const std::string&);

bool isColor(const std::string&);

double rOfRGB(const std::string);
double gOfRGB(const std::string);
double bOfRGB(const std::string);

#endif

