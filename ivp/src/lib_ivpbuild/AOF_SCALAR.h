/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AOF_SCALAR.h                                         */
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
 
#ifndef AOF_SCALAR_HEADER
#define AOF_SCALAR_HEADER

#include "AOF.h"

class AOF_SCALAR: public AOF {
public:
  AOF_SCALAR(IvPDomain g_domain) : AOF(g_domain) 
    {m_scalar_val = 0;}
  ~AOF_SCALAR() {}

public:    
  double evalBox(const IvPBox*) const {return(m_scalar_val);}
  bool   setParam(const std::string& param, double val) 
    {
      if(param == "scalar") {
	m_scalar_val = val;
	return(true);
      }
      return(false);
    }

private:
  double m_scalar_val;
};

#endif


















