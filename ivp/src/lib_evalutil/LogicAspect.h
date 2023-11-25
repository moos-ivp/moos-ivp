/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LogicAspect.h                                        */
/*    DATE: Oct 5th, 2020                                        */
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

#ifndef LOGIC_ASPECT_HEADER
#define LOGIC_ASPECT_HEADER

#include <vector>
#include "VarDataPair.h"
#include "LogicBuffer.h"

class LogicAspect 
{
 public:
  LogicAspect();
  ~LogicAspect() {};

 public: // Setting up functions
  void setInfoBuffer(InfoBuffer*);
  
  bool addLeadCondition(std::string);
  bool addPassCondition(std::string);
  bool addFailCondition(std::string);

  std::string checkConfig();

 public: // Interaction functions
  void update();

  std::string getStatus() const {return(m_status);}

  bool hasPassFailConditions() const;
  bool isEvaluated() const  {return(m_evaluated);}
  bool isSatisfied() const  {return(m_satisfied);}
  bool enabled() const      {return(m_enabled);}  // is non-empty

  std::string getInfo();
  
  std::set<std::string> getLogicVars() const;

  std::list<std::string> getReport(int ix=-1, int total=-1) const;

  std::vector<std::string> getSpec() const;
  
private: 
  LogicBuffer m_lead_cbuff;
  LogicBuffer m_pass_cbuff;
  LogicBuffer m_fail_cbuff;
  
  bool m_evaluated;
  bool m_satisfied;
  bool m_enabled;

  InfoBuffer *m_info_buffer;
  
  std::string m_status;
};

#endif 
 

