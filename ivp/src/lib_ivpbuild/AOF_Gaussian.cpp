/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AOF_Gaussian.cpp                                     */
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


#include "AOF_Gaussian.h"
#ifdef _WIN32
#   define _USE_MATH_DEFINES
#endif

#include <cmath>

using namespace std;

//----------------------------------------------------------------
// Procedure: setParam
 
bool AOF_Gaussian::setParam(const string& param, double value)
{
  if(param == "xcent")
    m_xcent = value;
  else if(param == "ycent")
    m_ycent = value;
  else if(param == "sigma")
    m_sigma = value;
  else if(param == "range")
    m_range = value;
  else
    return(false);
  return(true);
}


//----------------------------------------------------------------
// Procedure: evalPoint

double AOF_Gaussian::evalPoint(const vector<double>& point) const
{
  double xval = extract("x", point);
  double yval = extract("y", point);

  double dist = hypot((xval - m_xcent), (yval - m_ycent));
  double pct  = pow(M_E, -((dist*dist)/(2*(m_sigma * m_sigma))));

  return(pct * m_range);
}

//----------------------------------------------------------------
// Procedure: evalBox

double AOF_Gaussian::evalBox(const IvPBox *b) const
{
  double xval = 0;
  double yval = 0;

  m_domain.getVal(0, b->pt(0,0), xval);
  m_domain.getVal(1, b->pt(1,0), yval);
  
  double dist = hypot((xval - m_xcent), (yval - m_ycent));
  double pct  = pow(M_E, -((dist*dist)/(2*(m_sigma * m_sigma))));

  return(pct * m_range);
}

