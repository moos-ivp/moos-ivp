/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LinearExtrapolator.cpp                               */
/*    DATE: May 2nd, 2007                                        */
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

#include "LinearExtrapolator.h"
#include "GeomUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor()

LinearExtrapolator::LinearExtrapolator()
{
  m_xpos         = 0;
  m_ypos         = 0;
  m_spd          = 0;
  m_hdg          = 0;
  m_decay_start  = 0;
  m_decay_end    = 0;
  m_timestamp    = 0;
  m_position_set = false;

  m_decay_maxed  = false;
}

//---------------------------------------------------------------
// Procedure: getPosition()

bool LinearExtrapolator::getPosition(double& r_xpos, double& r_ypos,
				     double g_timestamp)
{
  // If there is no point to extrapolate from set to zero and return
  if(!m_position_set) {
    r_xpos = 0;
    r_ypos = 0;
    m_failure_reason = "unknown contact position";
    return(false);
  }

  // Handle the error cases.
  double delta_time = g_timestamp - m_timestamp;
  if((m_decay_end < m_decay_start) || (delta_time < -0.1)) {
    r_xpos = m_xpos;
    r_ypos = m_ypos;
    m_failure_reason = "negative delta time, possible clock skew";
    m_failure_reason += ", dtime=" + doubleToString(delta_time, 5);
    return(false);
  }
  m_failure_reason = "";

  // Handle a special (easy) case.
  if(delta_time == 0) {
    r_xpos = m_xpos;
    r_ypos = m_ypos;
    return(true);
  }

  // By default, assume more decay remaining unless calculated otherwise
  // below.
  m_decay_maxed = false;

  double distance = 0;
  if(delta_time <= m_decay_start)
    distance = m_spd * delta_time;
  
  if(delta_time > m_decay_start) {
    double decay_range = m_decay_end - m_decay_start;
    // Handle special case where decay is instantaneous
    if(decay_range <= 0) {
      distance = m_spd * m_decay_start;
      m_decay_maxed = true;
    }
    else {
      if(delta_time <= m_decay_end) {
	double decay_rate = (m_spd / (m_decay_end - m_decay_start));
	double decay_time = delta_time - m_decay_start;
	double curr_speed = m_spd - (decay_rate * decay_time);
	double avg_speed  = (m_spd + curr_speed) / 2.0;
	distance = (m_spd * m_decay_start) + (avg_speed * decay_time);
      }
      else { 
	m_decay_maxed = true;
	distance = (m_spd * m_decay_start) + ((m_spd/2.0) * decay_range);
      }
    }
  }

  projectPoint(m_hdg, distance, m_xpos, m_ypos, r_xpos, r_ypos);
  return(true);
}













