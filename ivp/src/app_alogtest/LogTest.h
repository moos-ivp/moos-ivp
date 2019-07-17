/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LogTest.h                                            */
/*    DATE: April 18th, 2019                                     */
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

#ifndef LOG_TEST_HEADER
#define	LOG_TEST_HEADER

#include <vector>
#include <string>
#include "LogicCondition.h"
#include "ALogEntry.h"
#include "InfoBuffer.h"

class LogTest {
public:
  LogTest();
  ~LogTest() {};

public: // Configuring
  bool setStartTime(std::string);
  bool setEndTime(std::string);
  void setStartTime(double v)   {m_start_time=v;}
  void setEndTime(double v)     {m_end_time=v;}

  void setTestName(std::string s) {m_name = s;}

  bool addStartCondition(std::string);
  bool addEndCondition(std::string);

  bool addPassCondition(std::string);
  bool addFailCondition(std::string);

  void setInfoBuffer(InfoBuffer* ibuff) {m_info_buffer=ibuff;}
  
public: // Modifying or Querying

  bool updateState(const ALogEntry&);
  bool updateEndState();

  void addEvent(const ALogEntry&, std::string info="");
  
  bool isEmpty() const;
  void print() const;

  double getStartTime() const {return(m_start_time);}
  double getEndTime() const   {return(m_end_time);}

  std::string getState() const {return(m_state_pfail);}
  std::string getName() const {return(m_name);}
  std::string getFailReason() const {return(m_fail_reason);}
  
  std::vector<std::string> getVars() const {return(m_vars);}
  std::vector<std::string> getEvents();
  
protected: // Utility Functions
  bool addCondition(std::string, std::vector<LogicCondition>&);

  bool checkStartConditions();
  bool checkEndConditions();
  bool checkFailConditions();
  bool checkPassConditions();
  std::string checkConditions(std::vector<LogicCondition>);

  bool updateInfoBuffer(const ALogEntry&);

  std::string getEntryInfo(const ALogEntry&);
  
protected: // Config variables
  
  std::string m_name;

  double m_start_time;
  double m_end_time;
  
  std::vector<LogicCondition> m_start_conditions;
  std::vector<LogicCondition> m_end_conditions;
  std::vector<LogicCondition> m_pass_conditions;
  std::vector<LogicCondition> m_fail_conditions;
  
  std::vector<std::string> m_vars;

  InfoBuffer *m_info_buffer;
  
protected: // State variables

  std::vector<std::string> m_event_tstamp;
  std::vector<std::string> m_event_onoff;
  std::vector<std::string> m_event_pfail;
  std::vector<std::string> m_event_varname;
  std::vector<std::string> m_event_varvalue;
  std::vector<std::string> m_event_info;
    
  std::string m_state_onoff;
  std::string m_state_pfail;
  std::string m_fail_reason;
};

#endif	


