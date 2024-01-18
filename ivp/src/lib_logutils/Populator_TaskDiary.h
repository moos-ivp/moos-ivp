/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Populator_TaskDiary.h                                */
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

#ifndef POPULATOR_MISSION_TASK_DIARY_HEADER
#define POPULATOR_MISSION_TASK_DIARY_HEADER

#include <string>
#include <vector>
#include <map>
#include "TaskDiary.h"
#include "ALogEntry.h"

class Populator_TaskDiary
{
public:
  Populator_TaskDiary() {m_verbose=false;}
  ~Populator_TaskDiary() {}

  bool      addALogFile(std::string filename);
  bool      addKLogFile(std::string filename,
			std::string node,
			double utc_start_time);

  bool      populateFromALogs();
  bool      populateFromKLogs();
  
  void      setVerbose()   {m_verbose = true;}
  TaskDiary getTaskDiary() {return(m_task_diary);}

protected:
  double handleALogFile(std::string filename);
  bool   handleKLogFile(unsigned int);
  
protected:
  bool m_verbose;
  
  TaskDiary m_task_diary;

  std::vector<std::string> m_alog_files;

  std::vector<std::string> m_klog_files;
  std::vector<double>      m_klog_files_utc;
  std::vector<std::string> m_klog_files_node;
  
};
#endif 
