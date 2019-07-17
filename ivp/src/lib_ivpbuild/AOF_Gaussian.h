/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_Gaussian.h                                       */
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
 
#ifndef AOF_GAUSSIAN_HEADER
#define AOF_GAUSSIAN_HEADER

#include <vector>
#include <string>
#include "AOF.h"

class AOF_Gaussian: public AOF {
 public:
  AOF_Gaussian(IvPDomain domain) : AOF(domain) 
    {m_xcent=0; m_ycent=0; m_sigma=1; m_range=100;}
  ~AOF_Gaussian() {}
  
 public:
  double evalBox(const IvPBox *b) const;  // Virtual Defined
  double evalPoint(const std::vector<double>& point) const;
  bool   setParam(const std::string&, double);

private:
  double m_xcent;  
  double m_ycent;
  double m_sigma;  
  double m_range;
};

#endif

















