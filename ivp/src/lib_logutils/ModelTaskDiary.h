/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ModelTaskDiary.h                                     */
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

#ifndef TASK_DIARY_MODEL_HEADER
#define TASK_DIARY_MODEL_HEADER

#include <vector>
#include <string>
#include "TaskDiary.h"
#include "TaskDiaryEntry.h"

class ModelTaskDiary
{
 public:
  ModelTaskDiary();
  virtual ~ModelTaskDiary() {}

  // Setters
  void   setTime(double tstamp);
  void   setTaskDiary(const TaskDiary&);

  void   setShowSeparator(bool v)   {m_show_separator=v;}
  void   setTruncateVal(bool v)     {m_truncate=v;}
  void   setWrapVal(bool v)         {m_wrap=v;}
  void   setFutureVal(bool v)       {m_future=v;}

  // Getters
  double getCurrTime() const      {return(m_curr_time);}
  bool   getShowSeparator() const {return(m_show_separator);}
  bool   getTruncateVal() const   {return(m_truncate);}
  bool   getWrapVal() const       {return(m_wrap);}
  bool   getFutureVal() const     {return(m_future);}

  std::vector<std::string>  getLinesUpToNow();
  std::vector<std::string>  getLinesPastNow() const;

protected:
  std::vector<std::string>  processLines(std::vector<TaskDiaryEntry>) const;
  
private:
  double    m_curr_time;
  TaskDiary m_task_diary;

  bool      m_show_separator;
  bool      m_truncate;
  bool      m_wrap;
  bool      m_future;
};

#endif


