/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LogicBuffer.cpp                                      */
/*    DATE: Sep 19th 2010 On the flight to IPAM                  */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "LogicBuffer.h"
#include "LogicUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: addNewCondition()

bool LogicBuffer::addNewCondition(string str_value)
{
  LogicCondition new_condition;
  bool ok = new_condition.setCondition(str_value);
  if(ok) {
    m_logic_conditions.push_back(new_condition);
    m_logic_vars = getLogicVars(m_logic_conditions);
  }
    
  return(ok);
}

//-----------------------------------------------------------
// Procedure: setInfoBuffer()

void LogicBuffer::setInfoBuffer(InfoBuffer *info_buffer)
{
  m_info_buffer = info_buffer;
}

//-----------------------------------------------------------
// Procedure: updateInfoBuffer()

void LogicBuffer::updateInfoBuffer(string moosvar, string value)
{
  if(!m_info_buffer)
    return;
  if(m_logic_vars.count(moosvar) == 0)
    return;

  m_info_buffer->setValue(moosvar, value);
}

//-----------------------------------------------------------
// Procedure: setCurrTime()

void LogicBuffer::setCurrTime(double curr_time)
{
  if(!m_info_buffer)
    return;

  m_info_buffer->setCurrTime(curr_time);
}

//-----------------------------------------------------------
// Procedure: getCurrTime()

double LogicBuffer::getCurrTime() const
{
  if(!m_info_buffer)
    return(0);

  double curr_time = m_info_buffer->getCurrTime();
  return(curr_time);
}

//-----------------------------------------------------------
// Procedure: updateInfoBuffer()

void LogicBuffer::updateInfoBuffer(string moosvar, double value)
{
  if(!m_info_buffer)
    return;
  if(m_logic_vars.count(moosvar) == 0)
    return;
  
  m_info_buffer->setValue(moosvar, value);
}

//-----------------------------------------------------------
// Procedure: checkConditions()

bool LogicBuffer::checkConditions(string required)
{
  if(!m_info_buffer)
    return(false);

  // Phase 1: get all the variable names from all present conditions.
  vector<string> condition_vars = getUniqueVars(m_logic_conditions);
  
  // Phase 2: get values of all variables from the info_buffer and 
  // propogate these values down to all the logic conditions.
  for(unsigned int i=0; i<condition_vars.size(); i++) {
    string varname = condition_vars[i];
    bool   ok_s, ok_d;
    string s_result = m_info_buffer->sQuery(varname, ok_s);
    double d_result = m_info_buffer->dQuery(varname, ok_d);
    
    if(ok_s) {
      for(unsigned int j=0; j<m_logic_conditions.size(); j++)
	m_logic_conditions[j].setVarVal(varname, s_result);
    }

    if(ok_d) {
      for(unsigned int j=0; j<m_logic_conditions.size(); j++)
      m_logic_conditions[j].setVarVal(varname, d_result);
    }
  }

    
  // Phase 3: evaluate all logic conditions.
  m_notable_condition = "required=" + required;
  if(required == "any") {
    for(unsigned int i=0; i<m_logic_conditions.size(); i++) {
      bool satisfied = m_logic_conditions[i].eval();    
      if(satisfied) {
	m_notable_condition = m_logic_conditions[i].getRawCondition();
	return(true);
      }
    }
    return(false);
  }

  // required == all (the default)
  for(unsigned int i=0; i<m_logic_conditions.size(); i++) {
    bool satisfied = m_logic_conditions[i].eval();    
    if(!satisfied) {
      m_notable_condition = m_logic_conditions[i].getRawCondition();
      return(false);
    }
  }
  return(true);
}


//-----------------------------------------------------------
// Procedure: getAllVarsSet()
//   Purpose: Get all the var names from all present conditions.

set<string> LogicBuffer::getAllVarsSet() const
{
  set<string> all_vars = getLogicVars(m_logic_conditions);
  return(all_vars);
}
 
//-----------------------------------------------------------
// Procedure: getAllVars()

vector<string> LogicBuffer::getAllVars() const
{
  vector<string> all_vars;

  set<string> all_vars_set = getLogicVars(m_logic_conditions);
  set<string>::iterator p;
  for(p=all_vars_set.begin(); p!=all_vars_set.end(); p++)
    all_vars.push_back(*p);

  return(all_vars);
}

//-----------------------------------------------------------
// Procedure: getInfoBuffReport()

vector<string> LogicBuffer::getInfoBuffReport(bool allvars) const
{
  vector<string> null_vector;
  if(!m_info_buffer)
    return(null_vector);

  if(allvars) {
    vector<string> vars = getAllVars();
    return(m_info_buffer->getReport(allvars));    
  }
  
  return(m_info_buffer->getReport());    
}

//-----------------------------------------------------------
// Procedure: getSpec()

vector<string> LogicBuffer::getSpec(string pad) const
{
  vector<string> spec;
  for(unsigned int i=0; i<m_logic_conditions.size(); i++)
    spec.push_back(pad + m_logic_conditions[i].getRawCondition());

  return(spec);
}

