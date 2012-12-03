/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmTPlot.cpp                                        */
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
#include "HelmReportUtils.h"

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

  if((tsize > 0) && (m_time[tsize-1] > gtime)) 
    return(false); 

  HelmReport prev_report;
  if(tsize > 0)
    prev_report = m_helm_reports[tsize-1];

  HelmReport new_report = string2HelmReport(gval, prev_report);

  m_time.push_back(gtime);
  m_helm_reports.push_back(new_report);
  return(true);
}

//---------------------------------------------------------------
// Procedure: getValueByIndex

string HelmTPlot::getValueByIndex(const string& qtype, 
				   unsigned int index) const
{  
  string result = "unknown"; 
  if(index >= m_time.size())
    result = "index-out-of-range";
  
  const HelmReport& report = m_helm_reports[index];
  
  if(qtype == "iter")
    result = uintToString(report.getIteration());
  else if(qtype == "mode")
    result = report.getModeSummary();
  else if(qtype == "utc")
    result = doubleToString(report.getTimeUTC());
  else if(qtype == "idle")
    result = report.getIdleBehaviors();
  else if(qtype == "running")
    result = report.getRunningBehaviors();
  else if(qtype == "completed")
    result = report.getCompletedBehaviors();
  else if(qtype == "active")
    result = report.getActiveBehaviors();
  else if(qtype == "decision")
    result = report.getDecisionSummary();

  return(result);
}

//---------------------------------------------------------------
// Procedure: getValueByTime

string HelmTPlot::getValueByTime(const string& qtype, double gtime) const
{
  double time_utc = m_logstart_utc + gtime;
  return(getValueByTimeUTC(qtype, time_utc));
}


//---------------------------------------------------------------
// Procedure: getValueByTimeUTC

string HelmTPlot::getValueByTimeUTC(const string& qtype, double gtime) const
{
  unsigned int index = getIndexByTimeUTC(gtime);
  return(getValueByIndex(qtype, index));
}
     
     
//---------------------------------------------------------------
// Procedure: getTimeByIterAdd

double HelmTPlot::getTimeByIterAdd(double ctime, 
				   unsigned int iter_offset) const
{
  unsigned int curr_index = getIndexByTime(ctime);
  unsigned int curr_iter  = m_helm_reports[curr_index].getIteration(); 
  unsigned int targ_iter  = curr_iter + iter_offset;

  unsigned int targ_index = curr_index; // for now.

  bool done = false;
  while(!done) {
    if((targ_index+1) < m_time.size()) {
      targ_index++;
      unsigned int new_iter = m_helm_reports[targ_index].getIteration();
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
  unsigned int curr_iter  = m_helm_reports[curr_index].getIteration(); 
  unsigned int targ_iter  = 0;
  if(curr_iter >= iter_offset)
    targ_iter = (curr_iter - iter_offset);

  unsigned int targ_index = curr_index; // for now.

  bool done = false;
  while(!done) {
    if(targ_index > 0) {
      targ_index--;
      unsigned int new_iter = m_helm_reports[targ_index].getIteration();
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
  return(m_helm_reports[index].getIteration());
}
     
//---------------------------------------------------------------
// Procedure: getIterByIndex

unsigned int HelmTPlot::getIterByIndex(unsigned int index) const
{
  if(index >= m_helm_reports.size())
    return(0);
  return(m_helm_reports[index].getIteration());
}
     
//---------------------------------------------------------------
// Procedure: getIterByTimeUTC

unsigned int HelmTPlot::getIterByTimeUTC(double gtime) const
{
  unsigned int index = getIndexByTimeUTC(gtime);
  return(m_helm_reports[index].getIteration());
}

//---------------------------------------------------------------
// Procedure: print

void HelmTPlot::print() const
{
  unsigned int i, vsize = m_time.size();
  cout << "HelmPlot::print()" << endl;
  cout << " VehicleName: " << m_vname << endl;
  cout << " VehicleType: " << m_vehi_type << endl;
  cout << " VehicleLength: " << m_vehi_length << endl;
  for(i=0; i<vsize; i++) {
    const HelmReport& report = m_helm_reports[i];
    cout << "time: " << m_time[i] << endl;
    cout << "  iter:   " << report.getIteration() << endl;
    cout << "  mode:   " << report.getModeSummary() << endl;
    cout << "  utc:    " << report.getTimeUTC() << endl;
    cout << "  active: " << report.getActiveBehaviors() << endl;
    cout << "  idle:   " << report.getIdleBehaviors() << endl;
    cout << "  running: " << report.getRunningBehaviors() << endl;
    cout << "  completed: " << report.getCompletedBehaviors() << endl;
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
  unsigned int vsize = m_helm_reports.size();
  if(vsize == 0)
    return(0);

  // Handle special cases
  if(iter <= m_helm_reports[0].getIteration())
    return(0);
  if(iter >= m_helm_reports[vsize-1].getIteration())
    return(vsize-1);

  int jump  = vsize / 2;
  int index = vsize / 2;
  bool done = false;
  while(!done) {
    //cout << "[" << index << "," << jump << "]" << flush;
    if(jump > 1)
      jump = jump / 2;
    if(m_helm_reports[index].getIteration() <= iter) {
      if(m_helm_reports[index+1].getIteration() > iter)
	done = true;
      else
	index += jump;
    }
    else
      index -= jump;
  }
  return(index);
}

