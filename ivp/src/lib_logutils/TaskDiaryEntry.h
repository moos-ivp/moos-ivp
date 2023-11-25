/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TaskDiaryEntry.h                                     */
/*    DATE: November 27th, 2021                                  */
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

#ifndef TASK_DIARY_ENTRY_HEADER
#define TASK_DIARY_ENTRY_HEADER

#include <string>
#include "MBUtils.h"

class TaskDiaryEntry
{
public:
  TaskDiaryEntry() {m_task_time=0; m_result_time=0;}
  virtual ~TaskDiaryEntry() {};
  
public:
  void setTaskTime(double dval)        {m_task_time = dval;}
  void setResultTime(double dval)      {m_result_time = dval;}
  void setSourceNode(std::string s)    {m_src_node = s;}
  void setSourceApp(std::string s)     {m_src_app = s;}
  void setTaskInfo(std::string s)      {m_task_info = s;}
  void setResultInfo(std::string s)    {m_result_info = s;}
  void addDestNode(std::string s)      {m_dest_nodes.push_back(s);}

  double getTaskTime() const           {return(m_task_time);}
  double getResultTime() const         {return(m_result_time);}
  std::string getSourceNode() const    {return(m_src_node);}
  std::string getSourceApp() const     {return(m_src_app);}
  std::string getTaskInfo() const      {return(m_task_info);}
  std::string getResultInfo() const    {return(m_result_info);}

  std::vector<std::string> getDestNodes() const
  {return(m_dest_nodes);}

  std::string getDestNodesAll() const
  {return(stringVectorToString(m_dest_nodes));}
  
  
protected: 
  double m_task_time;
  double m_result_time;
  std::string m_src_node;
  std::string m_src_app;
  std::string m_task_info;
  std::string m_result_info;

  std::vector<std::string> m_dest_nodes;
};

#endif 


