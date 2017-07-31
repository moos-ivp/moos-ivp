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

#include <iostream>
#include "LogicBuffer.h"
#include "MBUtils.h"

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

bool LogicBuffer::addNewCondition(const string& str_value)
{
  LogicCondition new_condition;
  bool ok = new_condition.setCondition(str_value);
  if(ok)
    m_logic_conditions.push_back(new_condition);

  cout << "ok result: " << ok << endl;
  return(ok);
}

//-----------------------------------------------------------
// Procedure: updateInfoBuffer

bool LogicBuffer::updateInfoBuffer(const string& moosvar,
				   const string& value)
{
  if(!m_info_buffer)
    return(false);

  return(m_info_buffer->setValue(moosvar, value));
}

//-----------------------------------------------------------
// Procedure: updateInfoBuffer

bool LogicBuffer::updateInfoBuffer(const string& moosvar,
				   double value)
{
  if(!m_info_buffer)
    return(false);
  
  return(m_info_buffer->setValue(moosvar, value));
}

//-----------------------------------------------------------
// Procedure: checkConditions()

bool LogicBuffer::checkConditions()
{
  if(!m_info_buffer) 
    return(false);

  unsigned int i, j, vsize, csize;

  // Phase 1: get all the variable names from all present conditions.
  vector<string> all_vars;
  csize = m_logic_conditions.size();
  for(i=0; i<csize; i++) {
    vector<string> svector = m_logic_conditions[i].getVarNames();
    all_vars = mergeVectors(all_vars, svector);
  }
  all_vars = removeDuplicates(all_vars);

  // Phase 2: get values of all variables from the info_buffer and 
  // propogate these values down to all the logic conditions.
  vsize = all_vars.size();
  for(i=0; i<vsize; i++) {
    string varname = all_vars[i];
    bool   ok_s, ok_d;
    string s_result = m_info_buffer->sQuery(varname, ok_s);
    double d_result = m_info_buffer->dQuery(varname, ok_d);

    for(j=0; (j<csize)&&(ok_s); j++)
      m_logic_conditions[j].setVarVal(varname, s_result);
    for(j=0; (j<csize)&&(ok_d); j++)
      m_logic_conditions[j].setVarVal(varname, d_result);
  }

  // Phase 3: evaluate all logic conditions. Return true only if all
  // conditions evaluate to be true.
  for(i=0; i<csize; i++) {
    bool satisfied = m_logic_conditions[i].eval();
    if(!satisfied)
      return(false);
  }
  return(true);  
}


//-----------------------------------------------------------
// Procedure: getAllVars()

vector<string> LogicBuffer::getAllVars()
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
 









