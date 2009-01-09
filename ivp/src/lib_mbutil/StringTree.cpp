/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: StringTree.cpp                                       */
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

#include "StringTree.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: addParChild

bool StringTree::addParChild(const string& parent, 
			     const string& child)
{
  if(parent == "") {
    StringNode new_node(child);
    m_nodes.push_back(new_node);
    return(true);
  }
  
  unsigned int i, vsize = m_nodes.size();
  for(i=0; i<vsize; i++) {
    bool added = m_nodes[i].addParChild(parent, child);
    if(added)
      return(true);
  }

  return(false);
}


//-------------------------------------------------------------
// Procedure: getPrintableSet

vector<string> StringTree::getPrintableSet()
{
  vector<string> rvector;
  return(rvector);
}

//-------------------------------------------------------------
// Procedure: print

void StringTree::print()
{
  unsigned int i, vsize = m_nodes.size();
  for(i=0; i<vsize; i++)
    m_nodes[i].print();
}

