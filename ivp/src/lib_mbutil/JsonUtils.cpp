/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: JsonUtils.cpp                                        */
/*    DATE: January 12th, 2025                                   */
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

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: jsonToCsp()
//  Examples:
//  (1) 
//     {"TargetID":"38886", "IsValid":"true"}
//     TargetID=38886, IsValid=true
//  (2)
//     {"x":"24", "y":"129", "name":"henry"}
//     x=24, y=129, name=henry
//  (3)
//     {"x":"24", "y":"129", "names":"{abe,ben,cal}"}
//     x=24, y=129, names={abe,ben,cal}


string jsonToCsp(string str)
{
  str = stripBlankEnds(str);

  if(isQuoted(str))
    str = stripQuotes(str);
  if(isBraced(str))
    str = stripBraces(str);

  string rstr;
  vector<string> svector = parseStringQ(str, ',');

  for(unsigned int i=0; i<svector.size(); i++) {
    string pair  = svector[i];
    string left  = stripQuotes(biteStringX(pair, ':'));
    string right = stripQuotes(pair);
    if(i != 0)
      rstr += ",";
      
    rstr += (left + "=" + right);
  }
  return(rstr);
}

//-------------------------------------------------------------
// Procedure: cspToJson()
//  Examples:
//  (1) 
//     TargetID=38886, IsValid=true
//     {"TargetID":"38886", "IsValid":"true"}
//  (2)
//     x=24, y=129, name=henry
//     {"x":"24", "y":"129", "name":"henry"}


string cspToJson(string str)
{
  str = stripBlankEnds(str);

  if(isBraced(str))
    return("");

  if(isQuoted(str))
    str = stripQuotes(str);

  string rstr = "{";
  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string pair  = svector[i];
    string left  = biteStringX(pair, '=');
    string right = pair;
    if(!isQuoted(left))
      left = "\"" + left + "\"";

    if(!isQuoted(right) && !isNumber(right))
      right = "\"" + right + "\"";

    if(i != 0)
      rstr += ",";
      
    rstr += left + ":" + right;
  }
  
  rstr += "}";
  return(rstr);
}


