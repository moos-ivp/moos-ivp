/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ProxPoint.cpp                                        */
/*    DATE: Nov 23rd 2018                                        */
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

#include "ProxPoint.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

ProxPoint::ProxPoint()
{
  m_cpa = 0;
  m_cpa_dist = 0;
}

//--------------------------------------------------------
// Procedure: overloaded less than operator

bool operator< (const ProxPoint& pt1, const ProxPoint& pt2)
{
  if(pt1.getCPADist() < pt2.getCPADist())
    return(true);
  else
    return(false);
}

