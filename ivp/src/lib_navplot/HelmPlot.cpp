/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmPlot.h                                           */
/*    DATE: July 15th, 2009                                      */
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

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include "HelmPlot.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

HelmPlot::HelmPlot()
{
  m_utc_start_time = 0;
  m_vehi_name = "unknown";
  m_vehi_type = "unknown";
  m_vehi_length = 0;
}

//---------------------------------------------------------------
// Procedure: set_vehi_type

void HelmPlot::set_vehi_type(string str)
{
  m_vehi_type = tolower(str);
}

//---------------------------------------------------------------
// Procedure: add_entry
//      Note: Time must be in ascending order. If new pair doesn't
//            obey, no action is taken, and false is returned.

bool HelmPlot::add_entry(double gtime, string gval)
{
  unsigned int tsize = m_time.size();

  if((tsize > 0) && (m_time[tsize-1] > gtime)) {
    return(false); 
  }

  string mode, utc, iter, idle, active, running, completed, decision;
  
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
      running = right;
    else if(left == "completed_bhvs")
      completed = right;
    else if(left == "active_bhvs")
      active = right;
    else if(left == "iter")
      iter = right;
    else if(left == "utc_time")
      utc = right;
    else if(left == "var") {
      if(decision != "")
	decision += ", ";
      string var = biteString(right, ':');
      string val = right;
      decision += var + "=" + val;
    }
  }

  bool fill_in = true;
  if((tsize > 1) && fill_in) {
    if(mode == "")
      mode = m_helm_mode[tsize-1];
    if(idle == "")
      idle = m_helm_idle_bhvs[tsize-1];
    if(active == "")
      active = m_helm_active_bhvs[tsize-1];
    if(running == "")
      running = m_helm_running_bhvs[tsize-1];
    if(completed == "")
      completed = m_helm_completed_bhvs[tsize-1];
    if(decision == "")
      decision  = m_helm_decision[tsize-1];
  }

  if((iter == "") || (utc == ""))
    return(false);
  
  // Make an abbreviated mode string
  string mode_short = modeShorten(mode);
   
  int iter_v = atoi(iter.c_str());
  if(iter_v < 0)
    iter_v = 0;
   
  m_time.push_back(gtime);
  m_helm_iter_s.push_back(iter);
  m_helm_iter_v.push_back((unsigned int)(iter_v));
  m_helm_utc.push_back(utc);
  m_helm_mode.push_back(mode);
  m_helm_mode_short.push_back(mode_short);
  m_helm_idle_bhvs.push_back(idle);
  m_helm_running_bhvs.push_back(running);
  m_helm_active_bhvs.push_back(active);
  m_helm_completed_bhvs.push_back(completed);
  m_helm_decision.push_back(decision);
  return(true);
}

//---------------------------------------------------------------
// Procedure: get_value_by_index

string HelmPlot::get_value_by_index(string qtype, unsigned int index) const
{  
  if(index >= m_time.size())
    return("index-out-of-range");
  
  if(qtype == "iter")
    return(m_helm_iter_s[index]);
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
  else if(qtype == "decision")
    return(m_helm_decision[index]);

  return("unknown");
}
     
//---------------------------------------------------------------
// Procedure: get_time_by_index

double HelmPlot::get_time_by_index(unsigned int index) const
{
  if(index < m_time.size())
    return(m_time[index]);
  else
    return(0);
}
     
//---------------------------------------------------------------
// Procedure: get_time_by_iter_add

double HelmPlot::get_time_by_iter_add(double ctime, 
				      unsigned int iter_offset) const
{
  unsigned int curr_index = get_index_by_time(ctime);
  unsigned int curr_iter  = m_helm_iter_v[curr_index]; 
  unsigned int targ_iter  = curr_iter + iter_offset;

  unsigned int targ_index = curr_index; // for now.

  bool done = false;
  while(!done) {
    if((targ_index+1) < m_time.size()) {
      targ_index++;
      unsigned int new_iter = m_helm_iter_v[targ_index];
      if(new_iter >= targ_iter)
	done = true;
    }
    else
      done = true;
  }
  return(m_time[targ_index]);
}
     
//---------------------------------------------------------------
// Procedure: get_time_by_iter_sub

double HelmPlot::get_time_by_iter_sub(double ctime, 
				      unsigned int iter_offset) const
{
  unsigned int curr_index = get_index_by_time(ctime);
  unsigned int curr_iter  = m_helm_iter_v[curr_index]; 
  unsigned int targ_iter  = 0;
  if(curr_iter >= iter_offset)
    targ_iter = (curr_iter - iter_offset);

  unsigned int targ_index = curr_index; // for now.

  bool done = false;
  while(!done) {
    if(targ_index > 0) {
      targ_index--;
      unsigned int new_iter = m_helm_iter_v[targ_index];
      if(new_iter <= targ_iter)
	done = true;
    }
    else
      done = true;
  }
  return(m_time[targ_index]);
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
// Procedure: get_iter_by_time

unsigned int HelmPlot::get_iter_by_time(double gtime) const
{
  int index = get_index_by_time(gtime);
  return(m_helm_iter_v[index]);
}
     
//---------------------------------------------------------------
// Procedure: get_value_by_time

string HelmPlot::get_value_by_time(string qtype, double gtime) const
{
  int index = get_index_by_time(gtime);

  if(qtype == "iter")
    return(m_helm_iter_s[index]);
  else if(qtype == "mode")
    return(m_helm_mode[index]);
  else if(qtype == "mode_short")
    return(m_helm_mode_short[index]);
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
  else if(qtype == "decision")
    return(m_helm_decision[index]);

  return("unknown");
}
     
//---------------------------------------------------------------
// Procedure: print

void HelmPlot::print() const
{
  unsigned int i;
  cout << "HelmPlot::print()" << endl;
  cout << " VehicleName: " << m_vehi_name << endl;
  cout << " VehicleType: " << m_vehi_type << endl;
  cout << " VehicleLength: " << m_vehi_length << endl;
  for(i=0; i<m_time.size(); i++) {
    cout << "time: " << m_time[i] << endl;
    cout << "  iter:   " << m_helm_iter_s[i] << endl;
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







