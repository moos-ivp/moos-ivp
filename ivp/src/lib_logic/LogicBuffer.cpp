/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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

#include "MBUtils.h"
#include "LogicBuffer.h"
#include "LogicUtils.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

LogicBuffer::LogicBuffer()
{
  m_info_buffer = new InfoBuffer;
}

//-----------------------------------------------------------
// Destructor

LogicBuffer::~LogicBuffer()
{
  if(m_info_buffer)
    delete(m_info_buffer);
}

//-----------------------------------------------------------
// Procedure: addNewCondition

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
// Procedure: updateInfoBuffer

void LogicBuffer::updateInfoBuffer(string moosvar, string value)
{
  if(!m_info_buffer)
    return;
  if(m_logic_vars.count(moosvar) == 0)
    return;

  m_info_buffer->setValue(moosvar, value);
}

//-----------------------------------------------------------
// Procedure: updateInfoBuffer

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
  m_notable_condition = "";
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

set<string> LogicBuffer::getAllVarsSet() const
{
  // Get all the variable names from all present conditions.
  set<string> all_vars;
  unsigned int vsize = m_logic_conditions.size();
  for(unsigned int i=0; i<vsize; i++) {
    vector<string> svector = m_logic_conditions[i].getVarNames();
    for(unsigned int j=0; j<svector.size(); j++)
      all_vars.insert(svector[j]);
  }

  return(all_vars);
}
 
//-----------------------------------------------------------
// Procedure: getAllVars()

vector<string> LogicBuffer::getAllVars() const
{
  // Get all the variable names from all present conditions.
  vector<string> all_vars;
  unsigned int i, vsize = m_logic_conditions.size();
  for(i=0; i<vsize; i++) {
    vector<string> svector = m_logic_conditions[i].getVarNames();
    all_vars = mergeVectors(all_vars, svector);
  }
  all_vars = removeDuplicates(all_vars);

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
