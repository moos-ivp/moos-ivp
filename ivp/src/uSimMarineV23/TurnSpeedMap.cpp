/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: SimEngine.cpp                                        */
/*    DATE: Feb 4th, 2022                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include <cmath>
#include "TurnSpeedMap.h"

using namespace std;

//-----------------------------------------------------------------
// Constructor()

TurnSpeedMap::TurnSpeedMap()
{
  m_full_speed = 1;
  m_null_speed = 0;
  m_full_rate  = 100;
  m_null_rate  = 0;
}

//-----------------------------------------------------------------
// Procedure: setFullSpeed(double)

bool TurnSpeedMap::setFullSpeed(double full_speed)
{
  if(full_speed <= 0)
    return(false);

  m_full_speed = full_speed;

  // Ensure that m_null_speed is not higher than m_full_speed
  if(m_null_speed > m_full_speed)
    m_null_speed = m_full_speed;

  return(true);
}

//-----------------------------------------------------------------
// Procedure: setNullSpeed(double)

bool TurnSpeedMap::setNullSpeed(double null_speed)
{
  if(null_speed <= 0)
    return(false);

  m_null_speed = null_speed;

  // Ensure that m_full_speed is not less than m_null_speed
  if(m_full_speed < m_null_speed)
    m_full_speed = m_null_speed;

  return(true);
}

//-----------------------------------------------------------------
// Procedure: setFullRate(double)

bool TurnSpeedMap::setFullRate(double full_rate)
{
  if((full_rate <= 0) || (full_rate > 100))
    return(false);

  m_full_rate = full_rate;

  // Ensure that m_null_rate is not higher than m_full_rate
  if(m_null_rate > m_full_rate)
    m_null_rate = m_full_rate;

  return(true);
}

//-----------------------------------------------------------------
// Procedure: setNullRate(double)

bool TurnSpeedMap::setNullRate(double null_rate)
{
  if((null_rate < 0) || (null_rate > 100))
    return(false);

  m_null_rate = null_rate;

  // Ensure that m_full_rate is not less than m_null_rate
  if(m_full_rate < m_null_rate)
    m_full_rate = m_null_rate;

  return(true);
}


//--------------------------------------------------------------
// Procedure: getTurnRate()
//
//           ^
// full_rate |            o---------------------------------
//           |          / |         Default vehicle: cannot
//           |       /    |         turn at zero spd, but can 
//           |    /       |         turn at any non-zero sped
//           | /          |
// null_rate o--------------------------------------------->
//               |       | 
//             null_   full_
//             speed   speed
//
//
//           ^
// full_rate |           o---------------------------------
//           |         / |          A boat with bare steerage
//           |       /   |          cannot turn if not moving
//           |     /     |          some min (null_speed) spd
//           |   /       |
// null_rate o--------------------------------------------->
//               |       | 
//             null_   full_
//             speed   speed
// 
//
//           ^
//           
// full_rate |             o---------------------------------
//           |          /  |        A boat with some other mode
//           |      /      |        of turning e.g. bow thruster
//           |  /          |        can essentially turn in 
// null_rate o             |        place
//           |             |
//           o--------------------------------------------->
//           |             | 
//         null_       full_
//         speed       speed

double TurnSpeedMap::getTurnRate(double spd)
{
  // Sanity check
  if((m_full_speed < 0) || (m_full_rate < 0))
    return(0);
  
  if(spd <= m_null_speed)
    return(m_null_rate);
  
  if(spd >= m_full_speed)
    return(m_full_rate);

  // Sanity check (m_full_speed - m_null_speed) > 0. Should never
  // be the case with the above two test cases.
  double speed_range = m_full_speed - m_null_speed;
  double rate_range  = m_full_rate  - m_null_rate;
  
  if(speed_range <= 0)
    return(0);

  double spd_delta = spd - m_null_speed;
  double pct = spd_delta / speed_range;

  double rate = (pct * rate_range) + m_null_rate;

  return(rate);
}
    


  


