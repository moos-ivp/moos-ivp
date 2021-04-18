/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_MGaussian.cpp                                    */
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

#include "MBUtils.h"
#include "AOF_MGaussian.h"

#ifdef _WIN32
#   define _USE_MATH_DEFINES
#endif
#include <cmath>

using namespace std;

//----------------------------------------------------------------
// Procedure: setParam
 
bool AOF_MGaussian::setParam(const string& param, const string& val)
{
  m_xcent.push_back(tokDoubleParse(val, "xcent", ',', '='));
  m_ycent.push_back(tokDoubleParse(val, "ycent", ',', '='));
  m_range.push_back(tokDoubleParse(val, "range", ',', '='));
  m_sigma.push_back(tokDoubleParse(val, "sigma", ',', '='));
  return(true);
}

//----------------------------------------------------------------
// Procedure: evalPoint

double AOF_MGaussian::evalPoint(const vector<double>& point) const
{
  if(point.size() != 2)  // Simple error checking
    return(0);

  double return_value = 0;
  for(unsigned int i=0; i<m_xcent.size(); i++) {
    double dist = hypot((point[0]-m_xcent[i]), (point[1]-m_ycent[i]));
    double pct  = pow(M_E, -((dist*dist)/(2*(m_sigma[i]*m_sigma[i]))));
    return_value += (pct * m_range[i]);
  }
  return(return_value);
}








