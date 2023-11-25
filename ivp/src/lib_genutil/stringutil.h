/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE:                                                      */
/*    DATE:                                                      */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
#ifndef STRINGUTIL_HEADER
#define STRINGUTIL_HEADER

#include <vector>
#include <string>

// Splits the string s into the different substrings that are separated by
// the string 'separator'.  The separator will not appear in the returned
// strings.
std::vector<std::string> tokenize(const std::string & s, const std::string & separator);

// Prints the strings in the vector, one on each line, to the output string.
//  Actual for m on each line is <prefix><string><suffix>
std::string vect_to_string(std::string prefix, std::string suffix, const std::vector<std::string> & v);

#endif








