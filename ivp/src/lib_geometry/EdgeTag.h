/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: EdgeTag.h                                            */
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
 
#ifndef EDGE_TAG_HEADER
#define EDGE_TAG_HEADER

#include <string>

class EdgeTag {
public:
  EdgeTag();
  EdgeTag(unsigned int, unsigned int, std::string);
  virtual ~EdgeTag() {}

  bool   setIndices(unsigned int, unsigned int);
  void   setTag(std::string tag) {m_tag=tag;}

  unsigned int getIndex1() const {return(m_index1);}
  unsigned int getIndex2() const {return(m_index2);}

  std::string getTag() const {return(m_tag);}
  std::string getSpec() const;

  bool setOnSpec(std::string);
  bool valid() const;
  bool matches(unsigned int, unsigned int) const;
  bool matches(unsigned int, unsigned int, std::string) const;

  void print() const;  

 protected:
  unsigned int m_index1;
  unsigned int m_index2;
  std::string  m_tag;
};

#endif




