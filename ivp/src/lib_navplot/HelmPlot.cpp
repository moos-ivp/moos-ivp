/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HelmPlot.h                                           */
/*    DATE: July 15th, 2009                                      */
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

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include "HelmPlot.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: add_helm_entry
//      Note: Time must be in ascending order. If new pair doesn't
//            obey, no action is taken, and false is returned.

bool HelmPlot::add_entry(double gtime, string gval)
{
  int tsize = m_time.size();

  if((tsize > 0) && (m_time[tsize-1] > gtime))
    return(false);

  string mode, utc, iter, idle, active, running, completed;
  

  vector<string> kvector = parseString(gval, ',');
  unsigned int k, ksize = kvector.size();
  for(k=0; k<ksize; k++) {
    string left  = stripBlankEnds(biteString(kvector[k],'='));
    string right = stripBlankEnds(kvector[k]);
    if(left == "modes")
      mode = right;
    else if(left == "idle_bhvs")
      idle = right;
    else if(left == "running_bhvs")
      active = right;
    else if(left == "completed_bhvs")
      running = right;
    else if(left == "active_bhvs")
      completed = right;
    else if(left == "iter")
      iter = right;
    else if(left == "utc_time")
      utc = right;
  }
  if((mode == "") || (iter == "") || (idle == "") || (utc == "") ||
     (active == "") || (running=="") || (completed == ""))
    return(false);
  
  m_time.push_back(gtime);
  m_helm_iter.push_back(iter);
  m_helm_utc.push_back(utc);
  m_helm_mode.push_back(mode);
  m_helm_idle_bhvs.push_back(idle);
  m_helm_running_bhvs.push_back(running);
  m_helm_active_bhvs.push_back(active);
  m_helm_completed_bhvs.push_back(completed);
  return(true);
}

//---------------------------------------------------------------
// Procedure: get_value_by_index

string HelmPlot::get_value_by_index(string qtype, unsigned int index) const
{  
  if((index < 0) || (index >= m_time.size()))
    return("index-out-of-range");
  
  if(qtype == "iter")
    return(m_helm_iter[index]);
  else if(qtype == "mode")
    return(m_helm_mode[index]);
  else if(qtype == "utc")
    return(m_helm_utc[index]);
  else if(qtype == "idle")
    return(m_helm_idle_bhvs[index]);
  else if(qtype == "running")
    return(m_helm_running_bhvs[index]);
  else if(qtype == "completed")
    return(m_helm_completed_bhvs[index]);
  else if(qtype == "active")
    return(m_helm_active_bhvs[index]);
}
     
//---------------------------------------------------------------
// Procedure: get_time_by_index

double HelmPlot::get_time_by_index(unsigned int index) const
{
  if((index >= 0) && (index < m_time.size()))
    return(m_time[index]);
  else
    return(0);
}
     
//---------------------------------------------------------------
// Procedure: get_index_by_time
//   Purpose: Given a time, determine the highest index that has a
//            time less than or equal to the given time.

//  2 5 9 13 14 19 23 28 31 35 43 57
//             ^                 ^
//            25                 56

int HelmPlot::get_index_by_time(double gtime) const
{
  int vsize = m_time.size();

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
// Procedure: get_value_by_time

string HelmPlot::get_value_by_time(string qtype, double gtime) const
{
  int index = get_index_by_time(gtime);

  if(qtype == "iter")
    return(m_helm_iter[index]);
  else if(qtype == "mode")
    return(m_helm_mode[index]);
  else if(qtype == "utc")
    return(m_helm_utc[index]);
  else if(qtype == "idle")
    return(m_helm_idle_bhvs[index]);
  else if(qtype == "running")
    return(m_helm_running_bhvs[index]);
  else if(qtype == "completed")
    return(m_helm_completed_bhvs[index]);
  else if(qtype == "active")
    return(m_helm_active_bhvs[index]);
}
     
//---------------------------------------------------------------
// Procedure: print

void HelmPlot::print() const
{
  unsigned int i;
  cout << "HelmPlot::print()" << endl;
  cout << " VehicleName: " << m_vehicle_name << endl;
  cout << " VehicleType: " << m_vehicle_type << endl;
  cout << " VehicleLength: " << m_vehicle_length << endl;
  for(i=0; i<m_time.size(); i++) {
    cout << "time: " << m_time[i] << endl;
    cout << "  iter:   " << m_helm_iter[i] << endl;
    cout << "  mode:   " << m_helm_mode[i] << endl;
    cout << "  utc:    " << m_helm_utc[i] << endl;
    cout << "  active: " << m_helm_active_bhvs[i] << endl;
    cout << "  idle:   " << m_helm_idle_bhvs[i] << endl;
    cout << "  running: " << m_helm_running_bhvs[i] << endl;
    cout << "  completed: " << m_helm_completed_bhvs[i] << endl;
  }
}

//---------------------------------------------------------------
// Procedure: get_min_time

double HelmPlot::get_min_time() const
{
  if(m_time.size() > 0)
    return(m_time[0]);
  else
    return(0.0);
}

//---------------------------------------------------------------
// Procedure: get_max_time

double HelmPlot::get_max_time() const
{
  if(m_time.size() > 0)
    return(m_time[m_time.size()-1]);
  else
    return(0.0);
}


