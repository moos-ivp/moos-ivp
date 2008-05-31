/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IPF_Plot.cpp                                         */
/*    DATE: Feb 24th, 2007                                       */
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

#include <iostream>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "IPF_Plot.h"
#include "MBUtils.h"
#include "AngleUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: add_entry
//      Note: Time must be in ascending order. If new pair doesn't
//            obey, no action is taken, and false is returned.

bool IPF_Plot::add_entry(double gtime, const string& gstr)
{
  int tsize = m_time_stamps.size();

  if((tsize == 0) || (m_time_stamps[tsize-1] <= gtime)) {
    m_time_stamps.push_back(gtime);
    m_ipf_strings.push_back(gstr);
    return(true);
  }
  else
    return(false);
}

//---------------------------------------------------------------
// Procedure: get_time_by_index

double IPF_Plot::get_time_by_index(unsigned int index) const
{
  if(m_time_stamps.size() == 0)
    return(0);

  if(index < 0)
    return(m_time_stamps[0]);
  else if(index >= m_time_stamps.size()) {
    cout << "time.size():" << m_time_stamps.size() << endl;
    return(m_time_stamps[m_time_stamps.size()-1]);
  }
  else
    return(m_time_stamps[index]);
}
     
//---------------------------------------------------------------
// Procedure: get_ipf_by_index

string IPF_Plot::get_ipf_by_index(unsigned int index) const
{
  if(m_ipf_strings.size() == 0)
    return("");

  if(index < 0)
    return(m_ipf_strings[0]);
  else if(index >= m_ipf_strings.size())
    return(m_ipf_strings[m_ipf_strings.size()-1]);
  else
    return(m_ipf_strings[index]);
}
     
//---------------------------------------------------------------
// Procedure: get_ipf_by_time
//   Purpose: Find the ipf for a given point in time.
//            If the point in time happens between two elements
//            in the time array, choose the lower indexed.

string IPF_Plot::get_ipf_by_time(double qtime) const
{
  // Special case: if the IPF_Plot instance is "empty"
  int vsize = m_time_stamps.size();
  if(vsize == 0) {
    return("");
  }

  // Special case: if the query time is outside the IPF_Plot 
  // time range, return the extreme value.
  if(qtime >=  m_time_stamps[vsize-1]) {
    return(nullHeadingSpeedIPF());
    //return(m_ipf_strings[vsize-1]);
  }
  if(qtime <= m_time_stamps[0]) {
    return(nullHeadingSpeedIPF());
    //return(m_ipf_strings[0]);
  }
  // Determine highest index w/ time <= qtime
  int index = get_index_by_time(qtime);

  return(m_ipf_strings[index]);

}
 
//---------------------------------------------------------------
// Procedure: nullHeadingSpeedIPF
//   Purpose:

string IPF_Plot::nullHeadingSpeedIPF() const
{
  string str = "H,14,46:bhv_waypt_b,2,1,1,100,D,course;0;359;360:speed;0;4;41,G,359,40,F,0,359,0,40,0,0,0";
  return(str);
}
    
//---------------------------------------------------------------
// Procedure: get_index_by_time
//   Purpose: Given a query time, determine the highest index that 
//            has a time less than or equal to the query time.
//            Search is log(n)

int IPF_Plot::get_index_by_time(double qtime) const
{
  int vsize = m_time_stamps.size();

  // Special case: if the query time is outside the IPF_Plot 
  // time range, return the extreme value.
  if(qtime <= m_time_stamps[0])
    return(0);
  if(qtime >= m_time_stamps[vsize-1])
    return(vsize-1);
  
  // Handle general case
  int jump  = vsize / 2;
  int index = vsize / 2;
  bool done = false;
  while(!done) {
    if(jump > 1)
      jump = jump / 2;
    if(m_time_stamps[index] <= qtime) {
      if(m_time_stamps[index+1] > qtime)
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
// Procedure: get_min_time

double IPF_Plot::get_min_time() const
{
  if(m_time_stamps.size() > 0)
    return(m_time_stamps[0]);
  else
    return(0);
}

//---------------------------------------------------------------
// Procedure: get_max_time

double IPF_Plot::get_max_time() const
{
  if(m_time_stamps.size() > 0)
    return(m_time_stamps[m_time_stamps.size()-1]);
  else
    return(0);
}


//---------------------------------------------------------------
// Procedure: print

void IPF_Plot::print() const
{
  unsigned int i;
  cout << "IPF_Plot::print()" << endl;
  for(i=0; i<m_time_stamps.size(); i++)
    cout << "time:" << m_time_stamps[i] << 
      "  ipf:" << m_ipf_strings[i] << endl;
}








