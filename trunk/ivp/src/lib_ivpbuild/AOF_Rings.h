/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_Rings.h                                          */
/*    DATE: Very old - modernized Jan 21 2006                    */
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
 
#ifndef AOF_RINGS_HEADER
#define AOF_RINGS_HEADER

#include <vector>
#include <string>
#include <cstdio>
#include "AOF_Ring.h"
#include "IvPDomain.h"

class AOF_Rings: public AOF {
public:
  AOF_Rings(IvPDomain);
  virtual ~AOF_Rings() {}

public:  
  double evalBox(const IvPBox *b) const;  // Virtual Defined
  bool   setParam(const std::string&, double);
  bool   setParam(const std::string&, const std::string&);

public:
  int         size() {return(m_rings.size());}
  void        print() const;
  std::string latexSTR(int full=1) const;   

private:
  double m_snapval;

  std::vector<AOF_Ring> m_rings;
};

#endif

















