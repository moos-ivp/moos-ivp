/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ULV_TimeStamp.cpp                                    */
/*    DATE: Sep 25th, 2011                                       */
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

#include <cstdio>
#include <cstdlib>   // atof/atoi
#include <iostream>
#include "ULV_TimeStamp.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

ULV_TimeStamp::ULV_TimeStamp()
{
  m_utc_time       = 0; 
  m_helm_iteration = 0;
}

//---------------------------------------------------------------
// Procedure: getString()

string ULV_TimeStamp::getString() const
{
  string rval = "time=" + doubleToString(m_utc_time, 2);
  
  if(m_vname != "")
    rval += ",vname=" + m_vname;
  if(m_helm_iteration > 0)
    rval += ",iter" + uintToString(m_helm_iteration);
  
  return(rval);
}


//---------------------------------------------------------------
// Procedure: setFromString(string)

bool ULV_TimeStamp::setFromString(const string& str)
{
  vector<string> svector = parseString(str, ',');
  unsigned int i, vsize = svector.size();

  for(i=0; i<vsize; i++) {
    string left  = biteStringX(svector[i], '=');
    string right = svector[i];
    if(left == "time")
      m_utc_time = atof(right.c_str());
    else if(left == "vname")
      m_vname = right;
    else if(left == "iter")
      m_helm_iteration = (unsigned int)(atoi(right.c_str()));
  }

  if(m_utc_time > 0)
    return(true);
  return(false);
}





