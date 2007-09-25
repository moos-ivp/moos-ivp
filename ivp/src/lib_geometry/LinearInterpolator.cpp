/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LinearInterpolator.cpp                               */
/*    DATE: May 2nd, 2007                                        */
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

#include "LinearInterpolator.h"
#include "GeomUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: getPosition

bool LinearInterpolator::getPosition(double& r_xpos, double& r_ypos,
				     double g_timestamp)
{
  // Handle the error cases.
  double delta_time = g_timestamp - m_timestamp;
  if((m_decay_end < m_decay_start) || (delta_time < 0)) {
    r_xpos = m_xpos;
    r_ypos = m_ypos;
    return(false);
  }

  // Handle a special (easy) case.
  if(delta_time == 0) {
    r_xpos = m_xpos;
    r_ypos = m_ypos;
    return(true);
  }

  double distance = 0;
  if(delta_time <= m_decay_start)
    distance = m_spd * delta_time;
  
  if(delta_time > m_decay_start) {
    double decay_range = m_decay_end - m_decay_start;
    // Handle special case where decay is instantaneous
    if(decay_range <= 0)
      distance = m_spd * m_decay_start;
    else {
      double decay_rate = (m_spd / (m_decay_end - m_decay_start));
      if(delta_time <= m_decay_end) {
	double decay_time = delta_time - m_decay_start;
	double curr_speed = m_spd - (decay_rate * decay_time);
	double avg_speed  = (m_spd + curr_speed) / 2.0;
	distance = (m_spd * m_decay_start) + (avg_speed * decay_time);
      }
      else {
	distance = (m_spd * m_decay_start) + ((m_spd/2.0) * decay_range);
      }
    }
  }

  projectPoint(m_hdg, distance, m_xpos, m_ypos, r_xpos, r_ypos);

  r_xpos = 0;
  r_ypos = 0;
  return(true);
}


