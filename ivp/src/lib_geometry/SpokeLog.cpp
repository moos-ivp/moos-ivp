/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: SpokeLog.cpp                                      */
/*    DATE: Dec 28th 2025                                        */
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

#include <iostream>
#include "MBUtils.h"
#include "SpokeLog.h"

using namespace std;

//----------------------------------------------------------
// Constructor()

SpokeLog::SpokeLog(string id)
{
  // Group 1 (intermediate values)
  m_id = id;
  m_dist_from_os = 0;
  
  // Group 2 (primary values)
  m_cpa      = -1;
  m_time_cpa = -1;
  
  m_passes_cn = false;
  m_passes_cn_port = false;
  m_passes_cn_star = false;
  m_passes_cn_port_dist = -1;
  m_passes_cn_star_dist = -1;
  m_passes_cn_spd = -1;
  
  m_crosses_cn = false;
  m_crosses_cn_bow = false;
  m_crosses_cn_stern = false;
  m_crosses_cn_bow_dist = -1;
  m_crosses_cn_stern_dist = -1;
  m_crosses_cn_spd = -1;
}

//----------------------------------------------------------
// Procedure: print()

void SpokeLog::print()
{
  cout << "--------------------------------------" << endl;
  cout << "id: " << m_id << endl;
  cout << "dist: " << doubleToString(m_dist_from_os,2) << endl;
  
  cout << "m_cpa:" << doubleToString(m_cpa,2) << endl;
  cout << "m_time_cpa:" << doubleToString(m_time_cpa,2) << endl;
  
  cout << "m_passes_cn:" << boolToString(m_passes_cn) << endl;
  cout << "m_passes_cn_port:" << doubleToString(m_passes_cn_port,2) << endl;
  cout << "m_passes_cn_star:" << doubleToString(m_passes_cn_star,2) << endl;
  cout << "m_passes_cn_port_dist:" << doubleToString(m_passes_cn_port_dist,2) << endl;
  cout << "m_passes_cn_star_dist:" << doubleToString(m_passes_cn_star_dist,2) << endl;
  cout << "m_passes_cn_spd:" << doubleToString(m_passes_cn_spd,2) << endl;
  
  cout << "m_crosses_cn:" << boolToString(m_crosses_cn) << endl;
  cout << "m_crosses_cn_bow:" << boolToString(m_crosses_cn_bow) << endl;
  cout << "m_crosses_cn_stern:" << boolToString(m_crosses_cn_stern) << endl;
  cout << "m_crosses_cn_bow_dist:" << doubleToString(m_crosses_cn_bow_dist) << endl;
  cout << "m_crosses_cn_spd:" << doubleToString(m_crosses_cn_spd) << endl;
}
