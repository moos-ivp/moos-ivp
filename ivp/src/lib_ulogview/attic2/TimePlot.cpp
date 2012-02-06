/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: TimePlot.cpp                                         */
/*    DATE: Sep 27th, 2011                                       */
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

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "TimePlot.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: getTimeByIndex

double TimePlot::getTimeByIndex(unsigned int index) const
{
  if(index < m_time.size())
    return(m_time[index]);
  else
    return(0);
}
     
//---------------------------------------------------------------
// Procedure: getTimeUTCByIndex

double TimePlot::getTimeUTCByIndex(unsigned int index) const
{
  if(index < m_time.size())
    return(m_time[index] + m_logstart_utc);
  else
    return(0);
}
     
//---------------------------------------------------------------
// Procedure: getIndexByTime
//   Purpose: Given a time, determine the highest index that has a
//            time less than or equal to the given time.

//  2 5 9 13 14 19 23 28 31 35 43 57
//             ^                 ^
//            25                 56

unsigned int TimePlot::getIndexByTime(double gtime) const
{
  unsigned int vsize = m_time.size();
  if(vsize == 0)
    return(0);

  // Handle special cases
  if(gtime <= m_time[0])
    return(0);
  if(gtime >= m_time[vsize-1])
    return(vsize-1);

  int jump  = vsize / 2;
  int index = vsize / 2;
  bool done = false;
  while(!done) {
    //cout << "[" << index << "," << jump << "]" << flush;
    if(jump > 1)
      jump = jump / 2;
    if(m_time[index] <= gtime) {
      if(m_time[index+1] > gtime)
	done = true;
      else
	index += jump;
    }
    else
      index -= jump;
  }
  return(index);
}


//---------------------------------------------------------------
// Procedure: getIndexByTimeUTC

unsigned int TimePlot::getIndexByTimeUTC(double utc_time) const
{
  double local_time = utc_time - m_logstart_utc;
  return(getIndexByTime(local_time));
}


//---------------------------------------------------------------
// Procedure: getMinTime

double TimePlot::getMinTime() const
{
  if(m_time.size() > 0)
    return(m_time[0]);
  else
    return(0);
}

//---------------------------------------------------------------
// Procedure: getMinTimeUTC

double TimePlot::getMinTimeUTC() const
{
  if(m_time.size() > 0)
    return(m_time[0] + m_logstart_utc);
  else
    return(0);
}

//---------------------------------------------------------------
// Procedure: getMaxTime

double TimePlot::getMaxTime() const
{
  if(m_time.size() > 0)
    return(m_time[m_time.size()-1]);
  else
    return(0);
}

//---------------------------------------------------------------
// Procedure: getMaxTimeUTC

double TimePlot::getMaxTimeUTC() const
{
  if(m_time.size() > 0)
    return(m_time[m_time.size()-1] + m_logstart_utc);
  else
    return(m_logstart_utc);
}

//---------------------------------------------------------------
// Procedure: containsTime
//      Note: Assumes time series is strictly monotonic

bool TimePlot::containsTime(double local_time) const
{
  unsigned int vsize = m_time.size();
  if(vsize == 0)
    return(false);
  
  if(local_time < m_time[0])
    return(false);
  if(local_time > m_time[vsize-1])
    return(false);
  return(true);
}

//---------------------------------------------------------------
// Procedure: containsTimeUTC
//      Note: Assumes time series is strictly monotonic

bool TimePlot::containsTimeUTC(double utc_time) const
{
  unsigned int vsize = m_time.size();
  if(vsize == 0)
    return(false);
  

  double local_time = utc_time - m_logstart_utc;

  cout << "local_time:" << local_time << endl;
  cout << " min_time:" << m_time[0] << endl;
  cout << " max_time:" << m_time[vsize-1] << endl;

  if(local_time < m_time[0])
    return(false);
  if(local_time > m_time[vsize-1])
    return(false);
  return(true);
}

