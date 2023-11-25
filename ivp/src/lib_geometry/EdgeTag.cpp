/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: EdgeTag.cpp                                          */
/*    DATE: Aug 14th, 2020                                       */
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
#include "MBUtils.h"
#include "EdgeTag.h"

using namespace std;

//---------------------------------------------------------------
// Constructor()

EdgeTag::EdgeTag()
{
  m_index1 = 0;
  m_index2 = 0;
}



//---------------------------------------------------------------
// Constructor()

EdgeTag::EdgeTag(unsigned int index1, unsigned int index2, string tag)
{
  if(!setIndices(index1, index2))
    return;

  m_tag = tag;
}

//---------------------------------------------------------------
// Procedure: setIndices()

bool EdgeTag::setIndices(unsigned int index1, unsigned int index2)
{
  if(((index1 + 1) == index2) || (index2 + 1) == index1) {
    m_index1 = index1;
    m_index2 = index2;
    return(true);
  }

  return(false);
}

//---------------------------------------------------------------
// Procedure: valid()

bool EdgeTag::valid() const
{
  if(((m_index1 + 1) != m_index2) && (m_index2 + 1) != m_index1)
    return(false);
  if(m_tag == "")
    return(false);

  return(true);
}

//---------------------------------------------------------------
// Procedure: getSpec()
//   Example: "56:55:yellow"

string EdgeTag::getSpec() const
{
  string str = uintToString(m_index1);
  str += ":" + uintToString(m_index2);
  str += ":" + m_tag;

  return(str);
}

//---------------------------------------------------------------
// Procedure: setOnSpec("56:55:yellow")

bool EdgeTag::setOnSpec(string str)
{
  vector<string> svector = parseString(str, ':');
  if(svector.size() < 3)
    return(false);

  string ix1 = stripBlankEnds(svector[0]);
  string ix2 = stripBlankEnds(svector[1]);
  string tag = stripBlankEnds(svector[2]);

  if(!isNumber(ix1) || !isNumber(ix2) || (tag == ""))
    return(false);

  int nx1 = atoi(ix1.c_str());
  int nx2 = atoi(ix2.c_str());
  if((nx1 < 0) || (nx2 < 0))
    return(false);

  if(!setIndices(nx1, nx2))
    return(false);
  
  m_tag = tag;

  return(true);
}

//---------------------------------------------------------------
// Procedure: matches()
//   Returns: true if the given two indices match in either order
//            false otherwise.

bool EdgeTag::matches(unsigned int ix1, unsigned int ix2) const
{
  if((ix1 == m_index1) && (ix2 == m_index2))
    return(true);
  if((ix2 == m_index1) && (ix1 == m_index2))
    return(true);

  return(false);
}

//---------------------------------------------------------------
// Procedure: matches()

bool EdgeTag::matches(unsigned int ix1, unsigned int ix2,
		      string tag) const
{
  if(!matches(ix1, ix2))
    return(false);

  if(tag != m_tag)     
    return(false);

  return(true);
}


//---------------------------------------------------------------
// Procedure: print()

void EdgeTag::print() const
{
  cout << m_index1 << ", " << m_index2 << ", " << m_tag << endl;
}


