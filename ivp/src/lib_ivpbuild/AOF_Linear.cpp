/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_Linear.cpp                                       */
/*    DATE: January 3rd, 2008                                    */
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

#include "AOF_Linear.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_Linear::setParam(const std::string& param, double val) 
{
  if(param == "mcoeff")
    m_coeff = val;
  else if(param == "ncoeff")
    n_coeff = val;
  else if(param == "bscalar")
    b_scalar = val;
  else
    return(false);
  return(true);
}

//----------------------------------------------------------------
// Procedure: evalBox

double AOF_Linear::evalBox(const IvPBox *ptbox) const
{
  double x_val = extract("x", ptbox);
  double y_val = extract("y", ptbox);
  
  return((m_coeff * x_val) + (n_coeff * y_val) + b_scalar);
}









