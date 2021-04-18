/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ProxPoint.h                                          */
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

#ifndef PROX_POINT_HEADER
#define PROX_POINT_HEADER

#include <string>

class ProxPoint {
public:
  ProxPoint();
  virtual ~ProxPoint() {}

  void setCPA(double v)     {m_cpa=v;}
  void setCPADist(double v) {m_cpa_dist=v;}
  void setType(int v)       {m_type=v;}
  
  double getCPA() const     {return(m_cpa);}
  double getCPADist() const {return(m_cpa_dist);}
  int    getType() const    {return(m_type);}

private:
  double m_cpa;
  double m_cpa_dist;
  int    m_type;
};

bool operator< (const ProxPoint& pt1, const ProxPoint& pt2);
bool operator> (const ProxPoint& pt1, const ProxPoint& pt2);
#endif


















