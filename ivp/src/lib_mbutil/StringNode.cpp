/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: StringNode.cpp                                       */
/*    DATE: Jan 9th 2009                                         */
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

#include <algorithm>
#include <iostream>
#include "StringNode.h"
#include "MBUtils.h"


using namespace std;

//-------------------------------------------------------------
// Procedure: print

void StringNode::print(unsigned int indent)
{
  unsigned int i, vsize = m_children.size();
  for(i=0; i<indent; i++)
    cout << " ";
 
  cout << m_value << endl;
  for(i=0; i<vsize; i++)
    m_children[i].print(indent + 2);
}


//-------------------------------------------------------------
// Procedure: writeGraphviz

void StringNode::writeGraphviz(ostream& os)
{
  unsigned int i, vsize = m_children.size();
  
  std::string clean_label=m_value;
  std::replace(clean_label.begin(), clean_label.end(), '-', '_');
  
  
  // branch node
  for(i=0; i<vsize; i++) {
    os << clean_label << "--";
    m_children[i].writeGraphviz(os);
  }
  
  // leaf node
  if(!vsize)
    os << clean_label << "\n  ";
}


//-------------------------------------------------------------
// Procedure: getPrintableSet

vector<string> StringNode::getPrintableSet(unsigned int indent)
{
  vector<string> rvector;
  string this_nodes_str;

  unsigned int i, vsize = m_children.size();
  for(i=0; i<indent; i++)
    this_nodes_str += "  ";
 
  this_nodes_str += m_value;
  rvector.push_back(this_nodes_str);

  for(i=0; i<vsize; i++) {
    vector<string> ivector = m_children[i].getPrintableSet(indent+2);
    rvector = mergeVectors(rvector, ivector);
  }
  return(rvector);
}


//-------------------------------------------------------------
// Procedure: addParChild

bool StringNode::addParChild(const string& parent, 
			     const string& child)
{
  if(parent == child)
    return(false);

  if(parent == m_value) {
    StringNode new_node(child);
    m_children.push_back(new_node);
    return(true);
  }

  unsigned int i, vsize = m_children.size();
  for(i=0; i<vsize; i++) {
    bool result = m_children[i].addParChild(parent, child);
    if(result == true)
      return(true);
  }

  return(false);
}










