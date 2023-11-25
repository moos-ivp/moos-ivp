/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BundleOut.cpp                                        */
/*    DATE: Jan 9th, 2022                                        */
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

#include <iostream>
#include "BundleOut.h"

using namespace std;

//---------------------------------------------------------
// Procedure: bundle_cout(list<string>)

void bundle_cout(const list<string>& lines)
{
  list<string>::const_iterator p;
  for(p=lines.begin(); p!=lines.end(); p++)
    cout << *p << endl;
}

//---------------------------------------------------------
// Procedure: bundle_cout(set<string>)

void bundle_cout(const set<string>& lines)
{
  set<string>::const_iterator p;
  for(p=lines.begin(); p!=lines.end(); p++)
    cout << *p << endl;
}

//---------------------------------------------------------
// Procedure: bundle_cout(vector<string>)

void bundle_cout(const vector<string>& lines)
{
  vector<string>::const_iterator p;
  for(p=lines.begin(); p!=lines.end(); p++)
    cout << *p << endl;
}


