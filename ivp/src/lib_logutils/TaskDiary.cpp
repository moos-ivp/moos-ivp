/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TaskDiary.cpp                                        */
/*    DATE: November 14th, 2021                                  */
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
#include <iostream>
#include <algorithm>
#include "MBUtils.h"
#include "TaskDiary.h"
#include "ACTable.h"

using namespace std;

//---------------------------------------------------------
// Procedure: processAllEntries()

void TaskDiary::processAllEntries(double min_start_time)
{
  order();
  removeDuplicates();
  adjustTimes(-min_start_time);

  detectTasks();
  detectTaskResults();
}

//---------------------------------------------------------
// Procedure: getDiaryEntries()

vector<TaskDiaryEntry> TaskDiary::getDiaryEntries() const
{
  return(m_task_entries);
}

//---------------------------------------------------------
// Procedure: getDiaryEntriesUpToTime()

vector<TaskDiaryEntry> TaskDiary::getDiaryEntriesUpToTime(double gtime) const
{
  vector<TaskDiaryEntry> entries;

  for(unsigned int i=0; i<m_task_entries.size(); i++) {
    if(m_task_entries[i].getTaskTime() <= gtime)
      entries.push_back(m_task_entries[i]);
  }

  return(entries);
}

//---------------------------------------------------------
// Procedure: getDiaryEntriesPastTime()

vector<TaskDiaryEntry> TaskDiary::getDiaryEntriesPastTime(double gtime) const
{
  vector<TaskDiaryEntry> entries;

  for(unsigned int i=0; i<m_task_entries.size(); i++) {
    if(m_task_entries[i].getTaskTime() > gtime)
      entries.push_back(m_task_entries[i]);
  }
  
  return(entries);
}

//-------------------------------------------------------------
// Procedure: getFormattedLines()
//   Purpose: Do the work of converting all TaskDiaryEntries into 
//            a set of user consumable lines. Applying:
//              a) separator lines if activated
//              b) truncation and/or wrapping if activated

#if 0
vector<string> TaskDiary::formattedLines(vector<TaskDiaryEntry> entries,
					 bool show_separator) const
{
  ACTable actab(4,2);
  actab.setColumnJustify(0, "right");
  actab << "Time | Source   | Received By  | Mission Task ";
  actab.addHeaderLines();
 
  for(unsigned int i=0; i<entries.size(); i++) {  
    TaskDiaryEntry entry = entries[i];
    if((i != 0) && show_separator)
      actab.addHeaderLines();

    string time     = doubleToString(entry.getTaskTime(),4);
    string info     = entry.getTaskInfo();
    string src_app  = entry.getSourceApp();
    string src_node = entry.getSourceNode();
    string dest_nodes = entry.getDestNodesAll();
    string res_time = doubleToString(entry.getResultTime(),4);
    string result   = entry.getResultInfo();
    if(result != "") {
      result = findReplace(result, ":", "=");
      result = findReplace(result, "#", ", ");
    }

    src_node = "(" + src_node + ")";
    actab << time     << src_app  << dest_nodes << info;
    actab << res_time << src_node << ""         << result;
  }

  vector<string> all_lines = actab.getTableOutput();
    
  return(all_lines);
}
#endif


//-------------------------------------------------------------
// Procedure: getFormattedLines()
//   Purpose: Do the work of converting all TaskDiaryEntries into 
//            a set of user consumable lines. Applying:
//              a) separator lines if activated
//              b) truncation and/or wrapping if activated

vector<string> TaskDiary::formattedLines(vector<TaskDiaryEntry> entries,
					 bool show_separator,
					 bool wrap_lines) const
{
  ACTable actab(4,2);
  actab.setColumnJustify(0, "right");
  actab << "Time | Source   | Received By  | Mission Task ";
  actab.addHeaderLines();
 
  for(unsigned int i=0; i<entries.size(); i++) {  
    TaskDiaryEntry entry = entries[i];
    if((i != 0) && show_separator)
      actab.addHeaderLines();

    string time     = doubleToString(entry.getTaskTime(),4);
    string info     = entry.getTaskInfo();
    string src_app  = entry.getSourceApp();
    string src_node = entry.getSourceNode();
    string dest_nodes = entry.getDestNodesAll();
    string res_time = doubleToString(entry.getResultTime(),4);
    string result   = entry.getResultInfo();
    if(result != "") {
      result = findReplace(result, ":", "=");
      result = findReplace(result, "#", ", ");
    }
    src_node = "(" + src_node + ")";

    // ================================================
    // No Wrap
    // ================================================
    if(!wrap_lines) {
      actab << time     << src_app  << dest_nodes << info;
      actab << res_time << src_node << ""         << result;
      continue;
    }

    // ================================================
    // Handle Wrap Line Option
    // ================================================
    // Part A: Handle Mission Task (top) portion
    vector<string> vdest = parseStringQ(dest_nodes, ',', 12);
    vector<string> vinfo = parseStringQ(info, ',', 50);
    
    unsigned int max_size = vdest.size();
    if(vinfo.size() > max_size)
      max_size = vinfo.size();

    for(unsigned int j=0; j<max_size; j++) {
      string j_dest;
      if(j < vdest.size())
	j_dest = vdest[j];
      string j_info;
      if(j < vinfo.size())
	j_info = vinfo[j];
      if(j==0)
	actab << time << src_app  << j_dest << j_info;
      else
	actab << ""   << ""       << j_dest << j_info;
    }

    // Part b: Handle Mission Result (bottom) portion
    vector<string> vresult = parseStringQ(result, ',', 50);
    for(unsigned int j=0; j<vresult.size(); j++) {
      string j_result = vresult[j];
      if(j==0)
	actab << res_time << src_node << "" << j_result;
      else
	actab << ""       << ""       << "" << j_result;
    }
  }

  vector<string> all_lines = actab.getTableOutput();
    
  return(all_lines);
}


//---------------------------------------------------------
// Procedure: order()

void TaskDiary::order()
{
  std::sort(m_alog_entries.begin(), m_alog_entries.end());
}

//---------------------------------------------------------
// Procedure: removeDuplicates()

void TaskDiary::removeDuplicates()
{
  if(m_alog_entries.size() == 0)
    return;
  
  vector<ALogEntry> new_entries;
  new_entries.push_back(m_alog_entries[0]);
  
  for(unsigned int i=1; i<m_alog_entries.size(); i++) {
    unsigned int esize = new_entries.size();
    bool duplicate = false;
    for(unsigned int j=0; j<10; j++) {
      if(esize >= j) {
	unsigned int ix = esize - j;
	if(m_alog_entries[i] == new_entries[ix])
	  duplicate = true;
      }
    }
    if(!duplicate)
      new_entries.push_back(m_alog_entries[i]);
  }

  m_alog_entries = new_entries;
}

//---------------------------------------------------------
// Procedure: adjustTimes()

void TaskDiary::adjustTimes(double tval)
{
  for(unsigned int i=0; i<m_alog_entries.size(); i++) 
    m_alog_entries[i].setTimeStamp(m_alog_entries[i].getTimeStamp() + tval);
}

//---------------------------------------------------------
// Procedure: detectTasks()

void TaskDiary::detectTasks()
{
  // Part 1: Grab all entries containing MISSION_TASK. Note that for
  // any given task, there will be multiple MISSION_TASK postings one
  // per vehicle. We won't worry about this for now.
  vector<TaskDiaryEntry> task_entries;
  
  for(unsigned int i=0; i<m_alog_entries.size(); i++) {
    string var = m_alog_entries[i].getVarName();
    if(var == "MISSION_TASK") {
      string src_app = m_alog_entries[i].getSource();
      string src_node = "unknown";
      if(src_app == "pMarineViewer")
	src_node = "shore";
      else if(src_app == "pHelmIvP")
	src_node = "self";
      else if(src_app == "uFldMessageHandler") {
	string aux = m_alog_entries[i].getSrcAux();
	src_node = biteStringX(aux, '+');
	src_app  = aux;
      }

      TaskDiaryEntry task_entry;
      task_entry.setTaskTime(m_alog_entries[i].getTimeStamp());
      task_entry.setTaskInfo(m_alog_entries[i].getStringVal());
      task_entry.addDestNode(m_alog_entries[i].getNode());       
      task_entry.setSourceApp(src_app); 
      task_entry.setSourceNode(src_node); 
      task_entries.push_back(task_entry);
    }
  }

  // Part 2: Go through the task entries and (a) merge entries
  // with same task_type and task_id, and note the list of vehicles
  // all working on this task.
  if(task_entries.size() == 0)
    return;

  m_task_entries.clear();
  m_task_entries.push_back(task_entries[0]);
  
  for(unsigned int j=1; j<task_entries.size(); j++) {
    TaskDiaryEntry entry = task_entries[j];
    bool distinct = false;
    if(entry.getTaskInfo() != m_task_entries.back().getTaskInfo())
      distinct = true;
    if(entry.getSourceApp() != m_task_entries.back().getSourceApp())
      distinct = true;

    if(distinct) 
      m_task_entries.push_back(entry);
    else {
      // For raw task entries, the destination should be a single node
      string dest_node = entry.getDestNodesAll(); 
      m_task_entries.back().addDestNode(dest_node);
    }
  }
}

 
//---------------------------------------------------------
// Procedure: detectTaskResults()

void TaskDiary::detectTaskResults()
{
  // Sanity check - if no tasks are detected/known
  if(m_task_entries.size() == 0)
    return;
  
  // Part 1: Grab all entries containing TASK_WON. Just note
  // the timestamp and the task info
  vector<double> noted_results_time;
  vector<string> noted_results_info;
  
  for(unsigned int i=0; i<m_alog_entries.size(); i++) {
    string var = m_alog_entries[i].getVarName();
    if(var == "TASK_WON") {
      double time = m_alog_entries[i].getTimeStamp();
      string info = m_alog_entries[i].getStringVal();
      noted_results_time.push_back(time);
      noted_results_info.push_back(info);
    }
  }

  // Part 2: For each task result, assign the result to one
  // of the known tasks.
  for(unsigned int i=0; i<noted_results_time.size(); i++) {
    double result_time = noted_results_time[i];
    string result_info = noted_results_info[i];

    for(unsigned int j=0; j<m_task_entries.size(); j++) {
      // skip if this task already has a result
      if(m_task_entries[j].getResultInfo() != "")
	continue;

      // skip all remaining tasks if this result is earlier
      // than the currently considered task
      double task_time = m_task_entries[j].getTaskTime();
      string task_info = m_task_entries[j].getTaskInfo();
      if(result_time <= task_time)
	break;

      // detect a match
      if(contentsMatch(task_info, result_info, "id") &&
	 contentsMatch(task_info, result_info, "type")) {
	string winner = tokStringParse(result_info, "winner", ',', '=');
	string others = tokStringParse(result_info, "others", ',', '=');
	string result = "[" + winner + "] " + others;
	m_task_entries[j].setResultInfo(result);
	m_task_entries[j].setResultTime(result_time);
      }
    }
  }
}


//---------------------------------------------------------
// Procedure: contentsMatch()
//   Purpose: For the given two strings of comma-separated
//            components of param=value pairs, determine if
//            the two strings match on the given field.

bool TaskDiary::contentsMatch(string s1, string s2, string field) const
{
  string val1 = tokStringParse(s1, field, ',', '=');
  string val2 = tokStringParse(s2, field, ',', '=');

  return(val1 == val2);
}





