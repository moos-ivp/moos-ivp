/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TaskDiary.h                                          */
/*    DATE: November 14th, 2021                                  */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#ifndef TASK_DIARY_HEADER
#define TASK_DIARY_HEADER

#include <vector>
#include <string>
#include "ALogEntry.h"
#include "TaskDiaryEntry.h"

class TaskDiary
{
public:
  TaskDiary() {};
  virtual ~TaskDiary() {};
  
public:
  void addALogEntry(ALogEntry entry)  {m_alog_entries.push_back(entry);}
  std::vector<ALogEntry> getALogEntries() const {return(m_alog_entries);}

  void processAllEntries(double min_start_time);

  std::vector<TaskDiaryEntry> getDiaryEntriesUpToTime(double) const;
  std::vector<TaskDiaryEntry> getDiaryEntriesPastTime(double) const;
  std::vector<TaskDiaryEntry> getDiaryEntries() const;

  unsigned int size() const {return(m_task_entries.size());}

  std::vector<std::string> formattedLines(std::vector<TaskDiaryEntry>,
					  bool show_separator,
					  bool wrap_lines) const;
  
 protected: // utilities 
  void order();
  void removeDuplicates();
  void adjustTimes(double);
  void detectTasks();
  void detectTaskResults();
  bool contentsMatch(std::string s1, std::string s2, std::string fld) const;
  
protected: 
  // Raw set of ALog entries re: MISSION_TASK posting over all nodes  
  std::vector<ALogEntry>      m_alog_entries;

  // Processed set of Tasks, one per group task assignment
  std::vector<TaskDiaryEntry> m_task_entries;
};

#endif 

