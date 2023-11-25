/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ModelTaskDiary.cpp                                   */
/*    DATE: Nov 27th 2021                                        */
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
#include <cstdio>
#include <cstdlib>
#include "ModelTaskDiary.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "ACTable.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

ModelTaskDiary::ModelTaskDiary()
{
  m_curr_time = 0;

  m_wrap           = false;
  m_future         = false;
  m_truncate       = false;
  m_show_separator = false;
}

//-------------------------------------------------------------
// procedure: setTaskDiary()

void ModelTaskDiary::setTaskDiary(const TaskDiary& task_diary)
{
  m_task_diary = task_diary;
}

//-------------------------------------------------------------
// Procedure: setTime()

void ModelTaskDiary::setTime(double gtime)
{
  m_curr_time = gtime;
}

//-------------------------------------------------------------
// Procedure: getLinesUpToNow()

vector<string> ModelTaskDiary::getLinesUpToNow()
{
  vector<TaskDiaryEntry> entries;
  entries = m_task_diary.getDiaryEntriesUpToTime(m_curr_time);
    
  vector<string> lines;
  lines = m_task_diary.formattedLines(entries, m_show_separator, m_wrap);

  return(lines);
}

//-------------------------------------------------------------
// Procedure: getLinesPastNow()

vector<string> ModelTaskDiary::getLinesPastNow() const
{
  vector<TaskDiaryEntry> entries;
  entries = m_task_diary.getDiaryEntriesPastTime(m_curr_time);

  vector<string> lines;
  lines = m_task_diary.formattedLines(entries, m_show_separator, m_wrap);

  return(lines);
}



//-------------------------------------------------------------
// Procedure: processLines()
//   Purpose: Do the work of converting all TaskDiaryEntries into 
//            a set of user consumable lines. Applying:
//              a) separator lines if activated
//              b) truncation and/or wrapping if activated

vector<string> ModelTaskDiary::processLines(vector<TaskDiaryEntry> entries) const
{
  ACTable actab(4,2);
  actab.setColumnJustify(0, "right");
  actab << "Time | Source   | Received By  | Mission Task ";
  actab.addHeaderLines();
 
  for(unsigned int i=0; i<entries.size(); i++) {  
    TaskDiaryEntry entry = entries[i];
    if((i != 0) && m_show_separator)
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

