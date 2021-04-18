/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: EdgeTagSet.cpp                                       */
/*    DATE: Aug 16th, 2020                                       */
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
#include "EdgeTagSet.h"

using namespace std;

//---------------------------------------------------------------
// Constructor()

EdgeTagSet::EdgeTagSet()
{
}


//---------------------------------------------------------------
// Procedure: addEdgeTag()

bool EdgeTagSet::addEdgeTag(EdgeTag tag)
{
  if(!tag.valid())
    return(false);

  m_edge_tags.push_back(tag);
  return(true);
}

//---------------------------------------------------------------
// Procedure: setOnSpec()
//  Examples: tags={2:3:end#4:5:end#6:7:end}
//            {2:3:end#4:5:end#6:7:end
//            2:3:end

bool EdgeTagSet::setOnSpec(string str)
{
  if(strBegins(str, "tags="))
    biteStringX(str, '=');

  // Remove braces - has no effect if there are no braces
  str = stripBraces(str);

  vector<EdgeTag> tags;
  
  vector<string> svector = parseString(str, '#');
  for(unsigned int i=0; i<svector.size(); i++) {
    string spec = svector[i];
    EdgeTag tag;
    bool ok = tag.setOnSpec(spec);
    if(!ok)
      return(false);
    else
      tags.push_back(tag);
  }

  // All new tags ok, so merge to m_edge_tags
  m_edge_tags.insert(m_edge_tags.end(), tags.begin(), tags.end());
  
  return(false);
}

//---------------------------------------------------------------
// Procedure: getSpec()

string EdgeTagSet::getSpec() const
{
  if(m_edge_tags.size() == 0)
    return("");
  
  string str = "tags={";
  for(unsigned int i=0; i<m_edge_tags.size(); i++) {
    string tag_spec = m_edge_tags[i].getSpec();
    if(i > 0)
      str += "#";
    str += tag_spec;
  }

  str += "}";
  return(str);
}


//---------------------------------------------------------------
// Procedure: matches()
//   Returns: true if at least one of edge tag matches
//            false otherwise

bool EdgeTagSet::matches(unsigned int ix1, unsigned int ix2,
			 string tag) const
{
  for(unsigned int i=0; i<m_edge_tags.size(); i++) {
    if(m_edge_tags[i].matches(ix1, ix2, tag))
      return(true);
  }

  return(false);
}


//---------------------------------------------------------------
// Procedure: print()

void EdgeTagSet::print() const
{
  cout << "EdgeTagSet: " << m_edge_tags.size() << " tags" << endl;

  for(unsigned int i=0; i<m_edge_tags.size(); i++) 
    m_edge_tags[i].print();
}

