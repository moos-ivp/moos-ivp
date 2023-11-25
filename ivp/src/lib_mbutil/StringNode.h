/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: StringTree.h                                         */
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

#ifndef STRING_NODE_HEADER
#define STRING_NODE_HEADER

#include <vector>
#include <string>

class StringNode
{
public:
  StringNode(const std::string& s) {m_value = s;}
  ~StringNode() {}

  bool addParChild(const std::string& parent, 
		   const std::string& child);

  void print(unsigned int indent=0);
  std::vector<std::string> getPrintableSet(unsigned int index=0);

 void writeGraphviz(std::ostream& os);

protected:
  std::string             m_value;
  std::vector<StringNode> m_children;
};
#endif












