/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: HelmReporter.cpp                                     */
/*    DATE: Mar 10th, 2010                                       */
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

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "MBUtils.h"
#include "HelmReporter.h"
#include "HelmReportUtils.h"
#include "LogUtils.h"
#include "ColorParse.h"

using namespace std;

//--------------------------------------------------------
// Constructor

HelmReporter::HelmReporter()
{
  m_report_life_events  = false;
  m_report_mode_changes = false;
  m_report_bhv_changes  = false;

  m_use_color = true;
  m_var_trunc = true;

  m_no_prev_report = true;
}


//--------------------------------------------------------
// Procedure: handle
//     Notes: 

bool HelmReporter::handle(const string& alogfile)
{
  if(alogfile == "") {
    cout << termColor("red");
    cout << "Alog file was not specified. Exiting now." << endl;
    cout << termColor();
    return(false);
  }

  FILE *file_ptr = fopen(alogfile.c_str(), "r");
  if(!file_ptr) {
    cout << termColor("red");
    cout << "Alog file not found or unable to open - exiting" << endl;
    cout << termColor();
    return(false);
  }
  
  cout << "Processing on file : " << alogfile << endl;

  unsigned int line_count  = 0;
  unsigned int life_events = 0;
  bool done = false;
  while(!done) {
    line_count++;
    string line_raw = getNextRawLine(file_ptr);

    if(m_report_life_events && !m_report_mode_changes && !m_report_bhv_changes) {
      if((line_count % 10000) == 0)
	cout << "+" << flush;
      if((line_count % 100000) == 0)
	cout << " (" << uintToCommaString(line_count) << ") lines" << endl;
    }

    bool line_is_comment = false;
    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      line_is_comment = true;

    if(line_raw == "eof") 
      done = true;
    
    if(!done && !line_is_comment) {
      string varname = getVarName(line_raw);
      string data = getDataEntry(line_raw);
      if(varname == "IVPHELM_LIFE_EVENT") { 
	m_life_events.addLifeEvent(data);
	life_events++;
      }
      if(m_report_bhv_changes && (varname == "IVPHELM_SUMMARY")) {
	string tstamp = getTimeStamp(line_raw);
	handleNewHelmSummary(data, tstamp);
      }
      if((m_report_mode_changes || m_report_bhv_changes) && 
	 (varname == "IVPHELM_MODESET")) {
	m_mode_var = biteString(data, '#');
      }
      if((m_report_mode_changes || m_report_bhv_changes) && 
	 (varname == m_mode_var)) {
	if(data != m_prev_mode_value) {
	  string tstamp = getTimeStamp(line_raw);
	  cout << "====================================================" << endl;
	  cout << tstamp << " Mode: " << data << endl;
	  m_prev_mode_value = data;
	}
      }
      if(vectorContains(m_watch_vars, varname)) {
	if(m_var_trunc)
	  cout << truncString(line_raw, 80) << endl;
	else
	  cout << line_raw << endl;
      }

    }
  }
  cout << endl << uintToCommaString(line_count) << " lines total." << endl;
  if(m_report_life_events)
    cout << uintToString(life_events) << " life events." << endl;

  if(file_ptr)
    fclose(file_ptr);

  return(true);
}

//--------------------------------------------------------
// Procedure: handleNewHelmSummary()

void HelmReporter::handleNewHelmSummary(string summary, string tstamp)
{
  HelmReport report;
  if(m_no_prev_report) {
    m_no_prev_report = false;
    report = string2HelmReport(summary);
  }
  else {
    report = string2HelmReport(summary, m_prev_report);
    if(!report.changedBehaviors(m_prev_report))
      return;
  }

  unsigned int iter = report.getIteration();
  
  // Want to report simple list of behaviors, not full reports
  // of behaviors which includes timestamps etc.
  bool   full = false;
  string bhvs_active   = report.getActiveBehaviors(full);
  string bhvs_running  = report.getRunningBehaviors(full);
  string bhvs_idle     = report.getIdleBehaviors(full);
  string bhvs_complete = report.getCompletedBehaviors(full);

  string color_active   = "";
  string color_running  = "";
  string color_idle     = "";
  string color_complete = "";
  if(m_watch_behavior != "") {
    if(strContains(bhvs_active, m_watch_behavior) &&
       !strContains(m_prev_report.getActiveBehaviors(full), m_watch_behavior)) {
      color_active = "blue";
    }
    if(strContains(bhvs_running, m_watch_behavior) &&
       !strContains(m_prev_report.getRunningBehaviors(full), m_watch_behavior)) {
      color_running = "blue";
    }
    if(strContains(bhvs_idle, m_watch_behavior) &&
       !strContains(m_prev_report.getIdleBehaviors(full), m_watch_behavior)) {
      color_idle = "blue";
    }
    if(strContains(bhvs_complete, m_watch_behavior) &&
       !strContains(m_prev_report.getCompletedBehaviors(full), m_watch_behavior)) {
      color_complete = "blue";
    }
  }

  cout << " - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;

  if(m_use_color) 
    cout << termColor(color_active);
  cout << tstamp << "    (" << iter << ") Active:    " << bhvs_active;
  if(color_active != "")
    cout << "             CHANGE";
  cout << termColor() << endl;

  if(m_use_color)
    cout << termColor(color_running);
  cout << tstamp << "    (" << iter << ") Running:   " << bhvs_running;
  if(color_running != "")
    cout << "             CHANGE";
  cout << termColor() << endl;

  if(m_use_color)
    cout << termColor(color_idle);
  cout << tstamp << "    (" << iter << ") Idle:      " << bhvs_idle;
  if(color_idle != "")
    cout << "             CHANGE";
  cout << termColor() << endl;


  if(bhvs_complete != "") {
    if(m_use_color)
      cout << termColor(color_complete);
    cout << tstamp << "    (" << iter << ") Completed: " << bhvs_complete;
    if(color_complete != "")
      cout << "             CHANGE";
    cout << termColor() << endl;
  }

  m_prev_report = report;
}


//--------------------------------------------------------
// Procedure: addWatchVar
//     Input: DESIRED_HEADING

void HelmReporter::addWatchVar(string var)
{
  if(strContainsWhite(var))
    return;
  if(vectorContains(m_watch_vars, var))
    return;
  
  m_watch_vars.push_back(var);
}


//--------------------------------------------------------
// Procedure: printReport
//     Notes: 

void HelmReporter::printReport()
{
  cout << endl << endl << endl;
  if(m_report_life_events) {
    vector<string> report_lines = m_life_events.getReport();
    unsigned int i, vsize = report_lines.size();
    for(i=0; i<vsize; i++)
      cout << report_lines[i] << endl;
  }
}








