/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_Linear.h                                         */
/*    DATE: 1996                                                 */
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
 
#ifndef AOF_LINEAR_HEADER
#define AOF_LINEAR_HEADER

#include "AOF.h"

class AOF_Linear: public AOF {
 public:
  AOF_Linear(IvPDomain domain) : AOF(domain)
  {m_coeff=0; n_coeff=0; b_scalar=0;}
  ~AOF_Linear() {}

public:    
  double evalBox(const IvPBox*) const;
  bool   setParam(const std::string& param, double val); 
  
private:
  double m_coeff;
  double n_coeff;
  double b_scalar;
};

#endif








