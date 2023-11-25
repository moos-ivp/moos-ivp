/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: HelmPlot.cpp                                         */
/*    DATE: Jul1509, Sep2811                                     */
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
#include "HelmPlot.h"
#include "MBUtils.h"
#include "LogUtils.h"
#include "HelmReportUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

HelmPlot::HelmPlot()
{
  m_vehi_type = "unknown";
  m_vehi_length = 0;
}

//---------------------------------------------------------------
// Procedure: setVehiType

void HelmPlot::setVehiType(string str)
{
  m_vehi_type = tolower(str);
}

//---------------------------------------------------------------
// Procedure: addEtry
//      Note: Time must be in ascending order. If new pair doesn't
//            obey, no action is taken, and false is returned.

bool HelmPlot::addEntry(double gtime, string gval)
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

string HelmPlot::getValueByIndex(string query, unsigned int index) const
{  
  string result = "unknown"; 
  if(index >= m_time.size())
    result = "index-out-of-range";
  
  const HelmReport& report = m_helm_reports[index];
  
  if(query == "iter")
    result = uintToString(report.getIteration());
  else if(query == "mode")
    result = report.getModeSummary();
  else if(query == "idle")
    result = report.getIdleBehaviors();
  else if(query == "running")
    result = report.getRunningBehaviors();
  else if(query == "completed")
    result = report.getCompletedBehaviors();
  else if(query == "active")
    result = report.getActiveBehaviors();
  else if(query == "decision")
    result = report.getDecisionSummary();

  return(result);
}

//---------------------------------------------------------------
// Procedure: getValueByTime

string HelmPlot::getValueByTime(string query, double gtime) const
{
  unsigned int index = getIndexByTime(m_time, gtime);

  string value = getValueByIndex(query, index);
  return(value);
}

//---------------------------------------------------------------
// Procedure: getTimeByIndex

double HelmPlot::getTimeByIndex(unsigned int index) const
{
  if(index < m_time.size())
    return(m_time[index]);
  return(0);
}
     


//---------------------------------------------------------------
// Procedure: getTimeByIterAdd

double HelmPlot::getTimeByIterAdd(double ctime, 
				   unsigned int iter_offset) const
{
  unsigned int curr_index = getIndexByTime(m_time, ctime);
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

double HelmPlot::getTimeByIterSub(double ctime, 
				   unsigned int iter_offset) const
{
  unsigned int curr_index = getIndexByTime(m_time, ctime);
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

unsigned int HelmPlot::getIterByTime(double gtime) const
{
  unsigned int index = getIndexByTime(m_time, gtime);
  return(m_helm_reports[index].getIteration());
}
     
//---------------------------------------------------------------
// Procedure: getIterByIndex

unsigned int HelmPlot::getIterByIndex(unsigned int index) const
{
  if(index >= m_helm_reports.size())
    return(0);
  return(m_helm_reports[index].getIteration());
}
     
//---------------------------------------------------------------
// Procedure: containsTime 
//      Note: Assumes time series is strictly monotonic 

bool HelmPlot::containsTime(double local_time) const
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
// Procedure: print

void HelmPlot::print() const
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






