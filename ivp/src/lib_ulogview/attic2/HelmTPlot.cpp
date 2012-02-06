/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmTPlot.h                                          */
/*    DATE: Jul1509, Sep2811                                     */
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
#include <cassert>
#include "HelmTPlot.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

HelmTPlot::HelmTPlot() : TimePlot()
{
  m_vehi_type = "unknown";
  m_vehi_length = 0;
}

//---------------------------------------------------------------
// Procedure: set_vehi_type

void HelmTPlot::setVehiType(string str)
{
  m_vehi_type = tolower(str);
}

//---------------------------------------------------------------
// Procedure: add_entry
//      Note: Time must be in ascending order. If new pair doesn't
//            obey, no action is taken, and false is returned.

bool HelmTPlot::addEntry(double gtime, string gval)
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
// Procedure: getValueByIndex

string HelmTPlot::getValueByIndex(string qtype, unsigned int index) const
{  
  cout << "qtype:" << qtype << " index:" << index << endl;

  string result = "unknown"; 
  if(index >= m_time.size())
    result = "index-out-of-range";
  else if(qtype == "iter")
    result = m_helm_iter_s[index];
  else if(qtype == "mode")
    result = m_helm_mode[index];
  else if(qtype == "utc")
    result = m_helm_utc[index];
  else if(qtype == "idle")
    result = m_helm_idle_bhvs[index];
  else if(qtype == "running")
    result = m_helm_running_bhvs[index];
  else if(qtype == "completed")
    result = m_helm_completed_bhvs[index];
  else if(qtype == "active")
    result = m_helm_active_bhvs[index];
  else if(qtype == "decision")
    result = m_helm_decision[index];

  cout << "result:" << result << endl;
  return(result);
}
     
//---------------------------------------------------------------
// Procedure: getTimeByIndex

double HelmTPlot::getTimeByIndex(unsigned int index) const
{
  if(index < m_time.size())
    return(m_time[index]);
  else
    return(0);
}
     
//---------------------------------------------------------------
// Procedure: getTimeByIterAdd

double HelmTPlot::getTimeByIterAdd(double ctime, 
				   unsigned int iter_offset) const
{
  unsigned int curr_index = getIndexByTime(ctime);
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
// Procedure: getTimeByIterSub

double HelmTPlot::getTimeByIterSub(double ctime, 
				   unsigned int iter_offset) const
{
  unsigned int curr_index = getIndexByTime(ctime);
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
// Procedure: getIterByTime

unsigned int HelmTPlot::getIterByTime(double gtime) const
{
  unsigned int index = getIndexByTime(gtime);
  return(m_helm_iter_v[index]);
}
     
//---------------------------------------------------------------
// Procedure: getIterByIndex

unsigned int HelmTPlot::getIterByIndex(unsigned int index) const
{
  if(index >= m_helm_iter_v.size())
    return(0);
  return(m_helm_iter_v[index]);
}
     
//---------------------------------------------------------------
// Procedure: getIterByTimeUTC

unsigned int HelmTPlot::getIterByTimeUTC(double gtime) const
{
  unsigned int index = getIndexByTimeUTC(gtime);
  return(m_helm_iter_v[index]);
}
     
//---------------------------------------------------------------
// Procedure: getValueByTime

string HelmTPlot::getValueByTime(string qtype, double gtime) const
{
  double time_utc = m_logstart_utc + gtime;
  return(getValueByTimeUTC(qtype, time_utc));
}


//---------------------------------------------------------------
// Procedure: getValueByTimeUTC

string HelmTPlot::getValueByTimeUTC(string qtype, double gtime) const
{
  unsigned int index = getIndexByTimeUTC(gtime);

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

void HelmTPlot::print() const
{
  unsigned int i;
  cout << "HelmPlot::print()" << endl;
  cout << " VehicleName: " << m_vname << endl;
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
// Procedure: getIndexByIter
//   Purpose: Given an iter, determine the highest index that has a
//            iter less than or equal to the given iter.

//  2 5 9 13 14 19 23 28 31 35 43 57
//             ^                 ^
//            25                 56

unsigned int HelmTPlot::getIndexByIter(unsigned int iter) const
{
  unsigned int vsize = m_helm_iter_v.size();
  if(vsize == 0)
    return(0);

  // Handle special cases
  if(iter <= m_helm_iter_v[0])
    return(0);
  if(iter >= m_helm_iter_v[vsize-1])
    return(vsize-1);

  int jump  = vsize / 2;
  int index = vsize / 2;
  bool done = false;
  while(!done) {
    //cout << "[" << index << "," << jump << "]" << flush;
    if(jump > 1)
      jump = jump / 2;
    if(m_helm_iter_v[index] <= iter) {
      if(m_helm_iter_v[index+1] > iter)
	done = true;
      else
	index += jump;
    }
    else
      index -= jump;
  }
  return(index);
}
