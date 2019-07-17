/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LogTest.cpp                                          */
/*    DATE: April 19, 2019                                        */
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
#include "LogTest.h"
#include "MBUtils.h"

using namespace std;

// --------------------------------------------------------
// Constructor 

LogTest::LogTest() 
{
  m_start_time = -1;
  m_end_time   = -1;

  m_state_onoff = "off";
  m_state_pfail = "unsat";

  // Store the values of variable in a buffer
  m_info_buffer = new InfoBuffer();
} 

// --------------------------------------------------------
// Procedure: setStartTime()
//      Note: We allow negative numbers, to indicate that no
//            start time logic is to be applied.

bool LogTest::setStartTime(string str)
{
  if(!isNumber(str))
    return(false);

  m_start_time = atof(str.c_str());
  return(true);
}

// --------------------------------------------------------
// Procedure: setEndTime()
//      Note: We allow negative numbers, to indicate that no
//            end time logic is to be applied.

bool LogTest::setEndTime(string str)
{
  if(!isNumber(str))
    return(false);

  m_end_time = atof(str.c_str());
  return(true);
}

// --------------------------------------------------------
// Procedure: addStartCondition()

bool LogTest::addStartCondition(string value)
{
  return(addCondition(value, m_start_conditions));
} 

// --------------------------------------------------------
// Procedure: addEndCondition()

bool LogTest::addEndCondition(string value)
{
  return(addCondition(value, m_end_conditions));
} 

// --------------------------------------------------------
// Procedure: addPassCondition()

bool LogTest::addPassCondition(string value)
{
  return(addCondition(value, m_pass_conditions));
} 

// --------------------------------------------------------
// Procedure: addFailCondition()

bool LogTest::addFailCondition(string value)
{
  return(addCondition(value, m_fail_conditions));
} 


// -------------------------------------------------------
// Procedure: updateState()
//   Returns: true if state has changed on this call
//     Notes: Possible on/off states:
//            (1) off, if
//                curr_time is less than start time
//                curr_time is greater than end time, or eof
//            (2) on, otherwise
//     Notes: Possible pfail states:
//            (1) unsat, if
//                there are unmet start conditions
//            (2) pending, if
//                there are pass conditions not yet met
//                no fail conditions have been encountered
//                (Note if timeout in this state, result is fail)
//            (3) passing, if
//                all pass conditions have been met
//                no fail conditions met so far, but time remains
//                (Note if timeout in this state, result is pass)
//            (4) passed,
//                all pass conditions met, and
//                no fail conditions met, and
//                end time has been exceeded, or eof
//            (5) failed
//                one or more pass conditions NOT met, or
//                one or more fail conditions ARE met
//                                                   

bool LogTest::updateState(const ALogEntry& entry)
{
  // Sanity checks
  if(!m_info_buffer)
    return(false);
  if(m_state_pfail == "failed") // terminal state
    return(false);
  if(m_state_pfail == "passed") // terminal state
    return(false);

  // Sanity check - handle if this is the last (eof marker) entry.
  double entry_time = entry.getTimeStamp();
  if(entry.getStatus() == "eof") {
    bool changed = updateEndState();
    if(changed) {
      addEvent(entry, "eof");
    }
    return(changed);
  }
  
  string prev_state_onoff = m_state_onoff;
  string prev_state_pfail = m_state_pfail;

  bool changed = false;
  
  // Part 1: In off state, check start time and start conditions
  if(m_state_onoff == "off") {
    if((m_start_time > 0) && (entry_time >= m_start_time)) {
      m_state_onoff = "on";
      m_state_pfail = "pending";
      changed = true;
      addEvent(entry, "start-time");
    }
  }

  // Note: We consider the world to be "changed" if the info_buffer
  // was changed OR if we just now transitioned state_onoff to on.
  changed = changed || updateInfoBuffer(entry);
  

  // Part 2: Check start conditions 
  if(changed && (m_state_onoff == "on") && (m_state_pfail == "unsat")) {
    if(checkStartConditions()) {
      m_state_pfail = "pending";
      string info = getEntryInfo(entry);
      addEvent(entry);
    }
  }
  
  // Part 3: Will check pfail conditions if (a) on (b) change in
  //         the info_buffer, and (c) curr pfail state is not unsat

  if(changed && (m_state_onoff == "on") && (m_state_pfail != "unsat")) {
    
    // Part 3A: Regardless of pending or passing, check fail conditions
    bool any_fail_conds_met = checkFailConditions();
    if(any_fail_conds_met) {
      m_state_onoff = "off";
      m_state_pfail = "failed";
      m_fail_reason = "met fail condition";
      string info = getEntryInfo(entry);
      addEvent(entry); 
      return(true);
    }

    // Part 3B: If pending, check if we transition to passing
    if(m_state_pfail == "pending") {
      bool all_pass_conds_met = checkPassConditions();
      if(all_pass_conds_met) {
	m_state_pfail = "passing";
	string info = getEntryInfo(entry);
	addEvent(entry); 
      }
    }

    // Part 3C: Then check end conditions, potentially completing
    if(checkEndConditions()) {
      bool changed = updateEndState();
      if(changed) {
	addEvent(entry);
      }
    }
  }
  
  // Part 4: Check if end time has been reached
  if((m_end_time > 0) && (entry_time >= m_end_time)) {
    bool changed = updateEndState();
    if(changed) {
      addEvent(entry, "end-time");
    }
  }

  // Finally - determine if all the above resulted in a change
  // of state for this test, and return accordingly.
  bool change_of_state = false;
  if(m_state_onoff != prev_state_onoff)
    change_of_state = true;
  if(m_state_pfail != prev_state_pfail)
    change_of_state = true;

  return(change_of_state);
}

// --------------------------------------------------------
// Procedure: updateEndState()
//   Returns: true if state has changed on this call
//      Note: If "pending" change to "failed"
//            If "passing" change to "passed"

bool LogTest::updateEndState()
{
  if(m_state_pfail == "pending") {
    m_state_onoff = "off";
    m_state_pfail = "failed";
    m_fail_reason = "unmet pass conditions";
    return(true);
  }

  if(m_state_pfail == "passing") {
    m_state_onoff = "off";
    m_state_pfail = "passed";
    return(true);
  }
  
  return(false);
}

// --------------------------------------------------------
// Procedure: isEmpty()

bool LogTest::isEmpty() const
{
  if((m_start_time > 0) || (m_end_time > 0))
    return(false);
  if(m_start_conditions.size() > 0)
    return(false);
  if(m_end_conditions.size() > 0)
    return(false);
  if(m_pass_conditions.size() > 0)
    return(false);
  if(m_fail_conditions.size() > 0)
    return(false);
  return(true);
} 

// --------------------------------------------------------
// Procedure: print()

void LogTest::print() const
{
  string name = m_name;
  if(m_name == "")
    name = "no_name";

  cout << "===========================================" << endl;
  cout << "Test Name: " << name << endl;
  cout << "===========================================" << endl;

  cout << "  Start time: " << doubleToStringX(m_start_time, 2) << endl;
  cout << "    End time: " << doubleToStringX(m_end_time, 2) << endl;

  cout << "Start Conds: [" << m_start_conditions.size() << "]" << endl;
  for(unsigned int i=0; i<m_start_conditions.size(); i++)
    cout << "  " << m_start_conditions[i].getRawCondition() << endl;

  cout << "End Conds: [" << m_end_conditions.size() << "]" << endl;
  for(unsigned int i=0; i<m_end_conditions.size(); i++)
    cout << "  " << m_end_conditions[i].getRawCondition() << endl;

  cout << "Pass Conds: [" << m_pass_conditions.size() << "]" << endl;
  for(unsigned int i=0; i<m_pass_conditions.size(); i++)
    cout << "  " << m_pass_conditions[i].getRawCondition() << endl;

  cout << "Fail Conds: [" << m_fail_conditions.size() << "]" << endl;
  for(unsigned int i=0; i<m_fail_conditions.size(); i++)
    cout << "  " << m_fail_conditions[i].getRawCondition() << endl;

  cout << "Vars: [" << m_vars.size() << "]" << endl;
  for(unsigned int i=0; i<m_vars.size(); i++) {
    if(i != 0)
      cout << ",";
    cout << m_vars[i];
  }
  cout << endl;
} 


// --------------------------------------------------------
// Procedure: getEvents()

vector<string> LogTest::getEvents()
{
  // Part 1: Pad the columns
  m_event_tstamp = padVector(m_event_tstamp);
  m_event_onoff = padVector(m_event_onoff);
  m_event_pfail = padVector(m_event_pfail);
  m_event_varname = padVector(m_event_varname);
  m_event_varvalue = padVector(m_event_varvalue);

  // Part 2: Build the lines
  vector<string> rvector;
  for(unsigned int i=0; i<m_event_tstamp.size(); i++) {
    string line = m_event_tstamp[i] + " ";
    line += m_event_onoff[i] + " ";
    line += m_event_pfail[i] + "  ";
    if(m_event_info[i] != "")
      line += m_event_info[i];
    else 
      line += m_event_varname[i] + " " + m_event_varvalue[i];
    rvector.push_back(line);
  }

  return(rvector);
}


// --------------------------------------------------------
// Procedure: addCondition()
//   Purpose: Converts given logic condition in string form into
//            a LogicCondition structure and adds the condition to
//            the given vector of LogicConditions.
//   Returns: true if the specified condition was able to be added
//            to the specified vector of flags 
//            false otherwise.

bool LogTest::addCondition(string value, vector<LogicCondition> &flags)
{
  LogicCondition new_condition;
  if(new_condition.setCondition(value))
    flags.push_back(new_condition);
  else 
    return(false);

  // Update m_vars with the variables involved in this condition
  m_vars = mergeVectors(m_vars, new_condition.getVarNames());
  m_vars = removeDuplicates(m_vars);
  return(true);
} 

// --------------------------------------------------------
// Procedure: checkStartConditions()
//   Returns: true if all conditions met, or there are no conditions
//            false otherwise (there exist un-met start conditions)

bool LogTest::checkStartConditions()
{
  // If there are no start conditions, there are no un-met conditions
  if(m_start_conditions.size() == 0)
    return(true);
  
  string result = checkConditions(m_start_conditions);
  if(result == "all")
    return(true);
  
  return(false);
}

// --------------------------------------------------------
// Procedure: checkEndConditions()
//   Returns: true if all conditions met, or there are no conditions
//            false otherwise (there exist un-met end conditions)

bool LogTest::checkEndConditions()
{
  // If there are no end conditions, there are no un-met conditions
  if(m_end_conditions.size() == 0)
    return(false);
  
  string result = checkConditions(m_end_conditions);
  if(result == "all")
    return(true);
  
  return(false);
}

// --------------------------------------------------------
// Procedure: checkPassConditions()
//   Returns: true if all conditions met, or there are no conditions
//            false otherwise (there exist un-met pass conditions)

bool LogTest::checkPassConditions()
{
  if(m_pass_conditions.size() == 0)
    return(true);

  string result = checkConditions(m_pass_conditions);
  if(result == "all")
    return(true);
  
  return(false);
}

// --------------------------------------------------------
// Procedure: checkFailConditions()
//   Returns: true if any of the fail conditions are met
//            false if none of the fail conditions are met
//            false if there are no fail conditions

bool LogTest::checkFailConditions()
{
  if(m_fail_conditions.size() == 0)
    return(false);

  string result = checkConditions(m_fail_conditions);
  if(result == "none")
    return(false);
  
  return(true);
}

// --------------------------------------------------------
// Procedure: checkConditions
//   Purpose: Checks the info buffer to see if the given
//            conditions have been satisfied.
//   Returns: "none" if null info_buffer
//            "none" if empty set of conditions
//            "none" if no conditions satisfied
//            "some" if some but not all conditions satisfied
//            "all"  if all conditions satisfied

string LogTest::checkConditions(vector<LogicCondition> conditions) 
{
  // If the info buffer isn't initialized, return none
  if(!m_info_buffer)
    return("none");
  
  // If the flags are empty, return none
  if(conditions.empty())
    return("none");
  
  // Phase 1: get all the variable names from all present conditions.
  vector<string> all_vars;
  for(unsigned int i=0; i<conditions.size(); i++) {
    vector<string> svector = conditions[i].getVarNames();
    all_vars = mergeVectors(all_vars, svector);
  }
  all_vars = removeDuplicates(all_vars);
  
  // Phase 2: get values of all variables from the info_buffer and
  // propogate these values down to all the logic conditions.
  for(unsigned int i=0; i<all_vars.size(); i++) {
    string varname = all_vars[i];
    bool ok_s, ok_d;
    string s_result = m_info_buffer->sQuery(varname, ok_s);
    double d_result = m_info_buffer->dQuery(varname, ok_d);
    
    for(unsigned int j=0; j<conditions.size(); j++) {
      if(ok_s)
	conditions[j].setVarVal(varname, s_result);
      if(ok_d)
	conditions[j].setVarVal(varname, d_result);
    }
  }
  
  // Phase 3: evaluate all logic conditions. Return true only if all
  // conditions evaluate to be true.
  unsigned int num_sat = 0;
  for(unsigned int i=0; i<conditions.size(); i++) {
    bool satisfied = conditions[i].eval();
    if(satisfied)
      num_sat++;
  } 

  if(num_sat == 0)
    return("none");
  if(num_sat < conditions.size())
    return("some");

  return("all");
} 

// --------------------------------------------------------
// Procedure: addEvent()

void LogTest::addEvent(const ALogEntry& entry, string info)
{
  double ctime = entry.getTimeStamp();
  string tstamp = doubleToString(ctime,3);
  m_event_tstamp.push_back(tstamp);
  m_event_onoff.push_back(m_state_onoff);
  m_event_pfail.push_back(m_state_pfail);

  if(info == "") {
    string name  = entry.getVarName();
    string sdata = entry.getStringVal();
    double ddata = entry.getDoubleVal();
    string varval = sdata;  
    if(entry.isNumerical())
      varval = doubleToStringX(ddata,5);
    m_event_varname.push_back(name);
    m_event_varvalue.push_back(varval);
    m_event_info.push_back("");
  }
  else {
    m_event_varname.push_back("");
    m_event_varvalue.push_back("");
    m_event_info.push_back(info);
  }
}

// --------------------------------------------------------
// Procedure: updateInfoBuffer
//   Purpose: Updates an info buffer to maintain a list of variable 
//            and their values at the current time.
//   Returns: true if the info buffer was updated; false otherwise

bool LogTest::updateInfoBuffer(const ALogEntry& entry)
{
  string name  = entry.getVarName();
  string sdata = entry.getStringVal();
  double ddata = entry.getDoubleVal();
  double ctime = entry.getTimeStamp();
  
  // Check if the name is empty and if the
  if(!name.empty()) {
    // Update the timestamp
    m_info_buffer->setValue("ALOG_TIMESTAMP", ctime);
    m_info_buffer->setCurrTime(ctime);
    
    // Check if m_vars contains this variable - only update if true
    if(vectorContains(m_vars, name)) {
      // Set the value depending on the type
      if(entry.isNumerical()) 
	return(m_info_buffer->setValue(name, ddata));
      else
	return(m_info_buffer->setValue(name, sdata));
    } 
  } 
  return(false);
}

// --------------------------------------------------------
// Procedure: getEntryInfo
//   Purpose: Convenience function for getting the varname,
//            value and source from an alog entry.

string LogTest::getEntryInfo(const ALogEntry& entry)
{
  string name  = entry.getVarName();
  string sdata = entry.getStringVal();
  double ddata = entry.getDoubleVal();
  string src   = entry.getSource();

  if(name == "")
    return("");
  
  string retstr = name + " " + src;
  // Set the value depending on the type
  if(entry.isNumerical()) 
    retstr += doubleToStringX(ddata);
  else
    retstr += sdata;

  return(retstr);
}

