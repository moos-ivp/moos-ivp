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
  // Init Config Params
  m_decay_beg  = -1;
  m_decay_end  = -1;

  // Init State vars
  m_xpos       = 0;
  m_ypos       = 0;
  m_spd        = 0;
  m_hdg        = 0;
  m_timestamp  = 0;

  m_position_set = false;
  m_decay_maxed  = false;
}

//---------------------------------------------------------------
// Procedure: setDecay()

bool LinearExtrapolator::setDecay(double beg, double end)
{
  if((beg == -1) && (end == -1))      // ok to shut off
    m_decay_beg=beg, m_decay_end=end;
  else if((beg < 0) || (end < 0))     // no other neg vals ok
    return(false);
  else if(beg > end)  // cannot begin after end 
    return(false);
  else {
    m_decay_beg = beg;
    m_decay_end = end;
  }

  return(true);
}

//---------------------------------------------------------------
// Procedure: getPosition()

bool LinearExtrapolator::getPosition(double& r_xpos, double& r_ypos,
				     double g_timestamp)
{
  // Sanity Check 1: If no point to extrap FROM, return newpt 0,0
  if(!m_position_set) {
    r_xpos = 0;
    r_ypos = 0;
    m_failure_reason = "unknown contact position";
    return(false);
  }

  // By default the extrap return position is the current position
  r_xpos = m_xpos;
  r_ypos = m_ypos;

  // Sanity check 2: Must have valid decay range
  if((m_decay_beg >= 0) || (m_decay_end >= 0)) {
    if(m_decay_end < m_decay_beg) {
      m_failure_reason = "invalid decay range";
      return(false);
    }
  }
  // Sanity check 3: Negative delta time should be flagged
  double delta_time = g_timestamp - m_timestamp;
  if(delta_time < -0.1) {
    m_failure_reason = "negative delta time, possible clock skew";
    m_failure_reason += ", delta_time=" + doubleToString(delta_time, 5);
    return(false);
  }
  m_failure_reason = "";

  
  // Sanity check 4: No delta time, return position is curr position.
  if(delta_time == 0) 
    return(true);

  // By default, set more decay remaining unless calc otherwise below.
  m_decay_maxed = false;
  
  double distance = 0;
  if((m_decay_beg < 0) && (m_decay_end < 0)) { // If no decay policy in use
   distance = m_spd * delta_time;
  }
  else { // Decay policy IS in use
    if(delta_time <= m_decay_beg)  // No decay yet.
      distance = m_spd * delta_time;
    else {
      distance = m_spd * m_decay_beg;
      double decay_range = m_decay_end - m_decay_beg;

      if(delta_time <= m_decay_end) {
	double decay_rate = (m_spd / (m_decay_end - m_decay_beg));
	double decay_time = delta_time - m_decay_beg;
	double curr_speed = m_spd - (decay_rate * decay_time);
	double avg_speed  = (m_spd + curr_speed) / 2.0;
	distance += (avg_speed * decay_time);
      }
      else { 
	m_decay_maxed = true;
	distance += ((m_spd/2.0) * decay_range);
      }
    }
  }

  projectPoint(m_hdg, distance, m_xpos, m_ypos, r_xpos, r_ypos);
  return(true);
}













