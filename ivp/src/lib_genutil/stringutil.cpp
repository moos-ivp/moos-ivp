/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
#include "stringutil.h"

#include <iostream>
#include <sstream>

using namespace std;

//==============================================================================

std::vector<std::string> tokenize(const std::string & s, const std::string & separator) {
  vector<string> v;

  if (s.length() == 0) {
    return v;
  }

  string::size_type pos1 = 0;

  bool done = false;
  while (! done) {
    string::size_type pos2 = s.find(separator, pos1);
    string token = s.substr(pos1, pos2 - pos1);
    v.push_back(token);

    if (pos2 == string::npos) {
      done = true;
    }
    else {
      pos1 = pos2 + separator.length();
    }
  }

  return v;
}

//==============================================================================

std::string vect_to_string(std::string prefix, std::string suffix, 
    const std::vector<std::string> & v) {

  ostringstream os;

  for (vector<string>::const_iterator pos = v.begin(); pos != v.end(); ++pos) {
    os << prefix << (*pos) << suffix << endl;
  }

  return os.str();
}








