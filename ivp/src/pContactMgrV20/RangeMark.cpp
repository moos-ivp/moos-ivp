/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RangeMark.cpp                                        */
/*    DATE: July 19 2020                                         */
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

#include "RangeMark.h"

using namespace std;


//--------------------------------------------------------
// Constructor

RangeMark::RangeMark(string contact, double range)
{
  m_contact = contact;
  m_range   = range;
}

//--------------------------------------------------------
// Procedure: setRangeMark()

void RangeMark::setRangeMark(string contact, double range)
{
  m_contact = contact;
  m_range   = range;
}


//--------------------------------------------------------
// Procedure: overloaded less than operator

bool operator< (const RangeMark& one, const RangeMark& two)
{
  if(one.getRange() < two.getRange())
    return(true);
  else
    return(false);
}

//--------------------------------------------------------
// Procedure: overload equals operator

bool operator== (const RangeMark& one, const RangeMark& two)
{
  return(one.getRange() == two.getRange());
}


