/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: StringNode.cpp                                       */
/*    DATE: Jan 9th 2009                                         */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <iostream>
#include "StringNode.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: print

void StringNode::print(int indent)
{
  cout << m_value << endl;
  unsigned int i, vsize = m_children.size();
  for(i=0; i<vsize; i++)
    m_children[i].print(indent + 2);
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

