/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_Gaussian.cpp                                     */
/*    DATE: June 9th 2008                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
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
#if 0
double AOF_Gaussian::evalPoint(const vector<double>& point) const
{
  double xval = extract("x", point);
  double yval = extract("y", point);

  double dist = hypot((xval - m_xcent), (yval - m_ycent));
  dist -= 80;
  if(dist < 0)
    dist = 0;

  double pct  = pow(M_E, -((dist*dist)/(2*(m_sigma * m_sigma))));

  return(pct * m_range);
}
#endif
#if 1
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
#endif

