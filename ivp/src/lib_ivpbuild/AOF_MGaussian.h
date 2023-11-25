/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AOF_MGaussian.h                                      */
/*    DATE: June 9th 2008                                        */
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
 
#ifndef AOF_MGAUSSIAN_HEADER
#define AOF_MGAUSSIAN_HEADER

#include <vector>
#include <string>
#include "AOF.h"

class AOF_MGaussian: public AOF {
 public:
  AOF_MGaussian(IvPDomain domain) : AOF(domain) {}
  ~AOF_MGaussian() {}
  
public:
  double evalPoint(const std::vector<double>& point) const;
  bool   setParam(const std::string&, const std::string&);

private:
  std::vector<double>  m_xcent;
  std::vector<double>  m_ycent;
  std::vector<double>  m_sigma;
  std::vector<double>  m_range;
};

#endif


















