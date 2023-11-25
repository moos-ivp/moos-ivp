/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LogicTestSequence.h                                  */
/*    DATE: Apr 10th, 2021                                       */
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

#ifndef LOGIC_TEST_SEQUENCE_HEADER
#define LOGIC_TEST_SEQUENCE_HEADER

#include <vector>
#include "LogicAspect.h"

class LogicTestSequence 
{
 public:
  LogicTestSequence();
  ~LogicTestSequence() {};

 public: // Setting up functions
  void setInfoBuffer(InfoBuffer*);
  
  bool addLeadCondition(std::string);
  bool addPassCondition(std::string);
  bool addFailCondition(std::string);

  std::string checkConfig();

 public: // Interaction functions
  
  void update();

  std::string getStatus() const {return(m_status);}

  bool enabled() const     {return(m_enabled);}
  bool isEvaluated() const {return(m_evaluated);}
  bool isSatisfied() const {return(m_satisfied);}

  std::set<std::string> getLogicVars() const;

  std::list<std::string> getReport() const;

  unsigned int size() const      {return(m_aspects.size());}
  unsigned int currIndex() const {return(m_currix);}

  std::vector<std::string> getSpec() const;
  
private: 
  std::vector<LogicAspect> m_aspects;

  InfoBuffer *m_info_buffer;

  unsigned int m_currix;

  bool m_evaluated;
  bool m_satisfied;
  bool m_enabled;

  std::string  m_status;
};

#endif 
 

