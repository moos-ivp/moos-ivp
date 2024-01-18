/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Populator_TaskDiary.cpp                              */
/*    DATE: Nov 26th, 2021                                       */
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

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "Populator_TaskDiary.h"
#include "MBUtils.h"
#include "LogUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: addALogFile()

bool Populator_TaskDiary::addALogFile(string filename)
{
  if(!okFileToRead(filename))
    return(false);
  if(vectorContains(m_alog_files, filename))
    return(false);

  m_alog_files.push_back(filename);

  return(true);
}

//---------------------------------------------------------------
// Procedure: populateFromALogs()

bool Populator_TaskDiary::populateFromALogs()
{
  double min_start_time = 0;

  // Populate the Task Diary
  for(unsigned int i=0; i<m_alog_files.size(); i++) {
    double start_time = handleALogFile(m_alog_files[i]);
    if((i==0) || (start_time < min_start_time))
      min_start_time = start_time;
  }

  m_task_diary.processAllEntries(min_start_time);

  if(min_start_time <= 0)
    return(false);
  
  return(true);
}


//---------------------------------------------------------------
// Procedure: populateFromKLogs()

bool Populator_TaskDiary::populateFromKLogs()
{
  double min_start_time = 0;

  // Populate the Task Diary
  for(unsigned int i=0; i<m_klog_files.size(); i++) {
    bool ok = handleKLogFile(i);
    if(!ok)
      return(false);
    double start_time = m_klog_files_utc[i];
    if((i==0) || (start_time < min_start_time))
      min_start_time = start_time;
  }

  m_task_diary.processAllEntries(min_start_time);

  if(min_start_time <= 0)
    return(false);
  
  return(true);
}


//--------------------------------------------------------
// Procedure: handleALogFile()
//   Returns: The UTC start time, to allow handler to find the
//            handler to find the overall min start time across
//            all alog files.

double Populator_TaskDiary::handleALogFile(string logfile)
{
  double utc_start_time = getLogStartFromFile(logfile);
  
  //cout << "Populator_TaskDiary::handleALogFIle: " << logfile << endl;
  FILE *fileptr = fopen(logfile.c_str(), "r");
  if(!fileptr) {
    cout << "alog file not found or unable to open: " << logfile << endl;
    cout << "Exiting." << endl;
    return(0);
  }

  // Need to find out the node (MOOS Community) name. This field
  // is not filled out automatically in the getNextRawAlogEntry step

  // PASS 1: Discern some infor needed for the second pass:
  //   (a) UTC start time
  //   (b) Community name
  //   (c) list of BCM_REPORT variables (BCM_REPORT_REQUEST)
  //   (d) list of task alert variables (TM_ALERT_REQUEST)
  // The start time and community name are typically obtained in
  // the first line or two of the file. Likely a two-pass approach
  // would not be needed for just these two vars. But the report
  // and alert vars could be present anywhere in the file. The
  // requests typically are logged before the named vars are logged,
  // but we like to make little/no assumption on log file ordering.
  
  string node_name;
  vector<string> bcm_report_vars;
  vector<string> task_alert_vars;

  bool done = false;
  while(!done) {
    bool all_strings = false;
    ALogEntry entry = getNextRawALogEntry(fileptr, all_strings);
    string status = entry.getStatus();
    // Check for the end of the file
    if(status == "eof")
      done = true;

    // If otherwise a "normal" line, process
    else if(status != "invalid") {
      string var = entry.getVarName();
      if((var == "DB_TIME") && (node_name == "")) {
	// Get the community name
	string source = entry.getSource(); // e.g. MOOSDB_abe
	biteString(source, '_');
	node_name = source;
	// Get the start time
	//double dval = entry.getDoubleVal();
	//double tstamp = entry.getTimeStamp();
	//utc_start_time = dval - tstamp;
      }
      else if(var == "BCM_REPORT_REQUEST") {
	string sval = entry.getStringVal(); 
	string bcm_rep_var = tokStringParse(sval, "var", ',', '=');
	if((bcm_rep_var != "") && !vectorContains(bcm_report_vars, bcm_rep_var))
	  bcm_report_vars.push_back(bcm_rep_var);
      }
      else if(var == "TM_ALERT_REQUEST") {
	string sval = entry.getStringVal(); 
	string tm_alert_var = tokStringParse(sval, "var", ',', '=');
	if((tm_alert_var != "") && !vectorContains(task_alert_vars, tm_alert_var))
	  task_alert_vars.push_back(tm_alert_var);
      }
    }
  }

  if(m_verbose) {
    cout << "UTC start time: " << doubleToStringX(utc_start_time) << endl;
    cout << "node_name: " << node_name << endl;
    for(unsigned int i=0; i<bcm_report_vars.size(); i++)
      cout << "bcm_var: " << bcm_report_vars[i] << endl;
    for(unsigned int i=0; i<task_alert_vars.size(); i++)
      cout << "task_alert_var: " << task_alert_vars[i] << endl;
  }
    
  
  // PASS 2: Grab all related vars. Two Notes: we will manually attach
  // the node name to each alog entry and we will replace the timestamp
  // from being in local time to absolute utc time
  
  fclose(fileptr);
  fileptr = fopen(logfile.c_str(), "r");
  
  done = false;
  while(!done) {
    bool all_strings = false;
    ALogEntry entry = getNextRawALogEntry(fileptr, all_strings);
    string status = entry.getStatus();

    if(status == "eof")
      break;
    if(status == "invalid")
      continue;

    string var = entry.getVarName();
    bool keep = false;
    if(var == "MISSION_TASK")
      keep = true;
    else if(var == "MISSION_TASK_ALL")
      keep = true;
    else if(var == "TASK_STATE")
      keep = true;
    else if(var == "TASK_WON")
      keep = true;
    else if(var == "TASK_MGR_STAT")
      keep = true;
    else if(var == "TM_ALERT_REQUEST")
      keep = true;
    else if(var == "BCM_REPORT_REQUEST")
      keep = true;
    else if((var == "NODE_MESSAGE_LOCAL") || (var == "NODE_MESSAGE")) {
      string sval = entry.getStringVal(); 
      string varname = tokStringParse(sval, "var_name", ',', '=');
      if(varname == "MISSION_TASK")
	keep = true;
    }
    else if(vectorContains(bcm_report_vars, var))
      keep = true;
    else if(vectorContains(task_alert_vars, var))
      keep = true;

    if(keep) {
      entry.setNode(node_name);
      double local_tstamp = entry.getTimeStamp();
      double utc_tstamp = utc_start_time + local_tstamp;
      entry.setTimeStamp(utc_tstamp);
      m_task_diary.addALogEntry(entry);
    }
  }
  
  fclose(fileptr);
  return(utc_start_time);
}


//--------------------------------------------------------
// Procedure: addKLogFile()

bool Populator_TaskDiary::addKLogFile(string filename,
				      string node_name,
				      double utc_start_time)
{
  if(!okFileToRead(filename))
    return(false);
  if(vectorContains(m_klog_files, filename))
    return(false);

  m_klog_files.push_back(filename);
  m_klog_files_node.push_back(node_name);
  m_klog_files_utc.push_back(utc_start_time);
  
  return(true);
}

//--------------------------------------------------------
// Procedure: handleKLogFile()

bool Populator_TaskDiary::handleKLogFile(unsigned int ix)
{
  if(ix >= m_klog_files.size())
    return(false);

  string klogfile = m_klog_files[ix];
  double utc_start_time = m_klog_files_utc[ix];
  string node_name = m_klog_files_node[ix];
  
  //cout << "Populator_TaskDiary::handleKLogFile: " << klogfile << endl;
  FILE *fileptr = fopen(klogfile.c_str(), "r");
  if(!fileptr) {
    cout << "klog file not found or unable to open: " << klogfile << endl;
    cout << "Exiting." << endl;
    return(false);
  }
  
  bool done = false;
  while(!done) {
    bool all_strings = false;
    ALogEntry entry = getNextRawALogEntry(fileptr, all_strings);
    string status = entry.getStatus();

    if(status == "eof")
      break;
    if(status == "invalid")
      continue;

    //cout << "+" << flush;
    entry.setNode(node_name);
    double local_tstamp = entry.getTimeStamp();
    double utc_tstamp = utc_start_time + local_tstamp;
    entry.setTimeStamp(utc_tstamp);
    m_task_diary.addALogEntry(entry);
    //cout << "/" << flush;
  }
  //cout << endl;
  //cout << "Populator_TaskDiary::handleKLogFile: Done: " << klogfile << endl;
  fclose(fileptr);
  return(true);
}



