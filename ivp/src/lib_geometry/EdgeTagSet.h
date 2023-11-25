/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: EdgeTagSet.h                                         */
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
 
#ifndef EDGE_TAG_SET_HEADER
#define EDGE_TAG_SET_HEADER

#include <vector>
#include <string>
#include "EdgeTag.h"

class EdgeTagSet {
public:
  EdgeTagSet();
  virtual ~EdgeTagSet() {}

  bool addEdgeTag(EdgeTag);
  bool setOnSpec(std::string);

  unsigned int size() const {return(m_edge_tags.size());}
  
  std::string getSpec() const;

  bool matches(unsigned int, unsigned int, std::string) const;

  void print() const;
 
 protected:

  std::vector<EdgeTag> m_edge_tags;
};

#endif





