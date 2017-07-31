/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: UFieldUtils.cpp                                      */
/*    DATE: Sep 27th 2012                                        */
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

#include "UFieldUtils.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Procedure: isValidPShareRoute
//   Example: localhost:9000
//            192.169.0.1:9200
//            multicast_8

bool isValidPShareRoute(const string& route)
{
  vector<string> svector = parseString(route, ':');
  unsigned int vsize = svector.size();

  // Handle the multicast case
  // svector[0] = multicast_8
  if(vsize == 1) {
    vector<string> jvector = parseString(svector[0], '_');
    unsigned int jsize = jvector.size();
    if(jsize != 2)
      return(false);
    if(jvector[0] != "multicast")
      return(false);
    if(!isNumber(jvector[1]))
      return(false);
    return(true);
  }

  // Handle the UDP cast
  // svector[0] = localhost
  // svector[1] = 9000
  else if(vsize == 2) {
    if(!isValidIPAddress(svector[0]))
      return(false);
    if(!isNumber(svector[1]))
      return(false);
    return(true);
  }
  
  return(false);
}








