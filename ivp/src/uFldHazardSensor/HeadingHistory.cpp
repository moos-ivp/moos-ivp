/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HeadingHistory.cpp                                   */
/*    DATE: Jan 19th 2013                                        */
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

#include "AngleUtils.h"
#include "HeadingHistory.h"

using namespace std;

//---------------------------------------------------------
// Constructor

HeadingHistory::HeadingHistory()
{
  m_history_time = 60;
  m_history_amt  = 1000;
}

//---------------------------------------------------------
// Procedure: addHeadingTime

bool HeadingHistory::addHeadingTime(double heading, double time)
{
  // Add a new time/heading pair if either:
  // - the list is empty, or
  // - the new timestamp is greater than the most recently added.

  if((m_timestamps.size() == 0) || (time > m_timestamps.front())) {
    m_timestamps.push_front(time);
    m_headings.push_front(heading);
  }

  // Pop the back element (oldest) item from the list if either:
  // - the list has too many elements (m_history_amt), or
  // - the age of the oldest exceeds m_history_time.

  if((m_timestamps.size() > m_history_amt) ||
     ((m_timestamps.front() - m_timestamps.back()) > m_history_time)) {
    m_timestamps.pop_back();
    m_headings.pop_back();
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: getTurnRate

double HeadingHistory::getTurnRate(double time_interval) const
{
  if(time_interval <= 0)
    return(0);

  if((m_headings.size() == 0) || (m_timestamps.size() == 0))
    return(0);

  double latest_time = m_timestamps.front();
  double latest_hdg  = m_headings.front();

  double total_delta_time = 0;
  double total_delta_hdg  = 0;

  // Make a copy of this list since we going to alter the copy
  list<double> headings = m_headings;

  list<double>::const_iterator p;
  for(p=m_timestamps.begin(); p!=m_timestamps.end(); p++) {
    double tstamp = *p;    
    if((latest_time - tstamp) < time_interval) {
      double heading = headings.front();
      double delta = (heading - latest_hdg);
      delta = angle180(delta);
      if(delta < 0)
	delta *= -1;
      total_delta_hdg += delta;
      total_delta_time = latest_time - tstamp;
      latest_hdg = heading;
      headings.pop_front();
    }
  }

  if(total_delta_time == 0)
    return(0);

  double turn_rate = total_delta_hdg / total_delta_time;

  return(turn_rate);
}








