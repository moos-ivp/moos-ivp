/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LogicTestSequenct.cpp                                */
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

#include <iostream>
#include "MBUtils.h"
#include "LogicTestSequence.h"

using namespace std;

//---------------------------------------------------------
// Constructor

LogicTestSequence::LogicTestSequence()
{
  m_info_buffer = 0;

  // enabled means non-empty, properly configured
  m_enabled   = false; 
  m_evaluated = false; 
  m_satisfied = false; 
  m_status    = "pending";

  // m_currix is lowest index LogicAspect yet to be evaluated
  m_currix    = 0;

  // Start with an empty logic_aspect at index 0.
  LogicAspect aspect;
  m_aspects.push_back(aspect);
}

//------------------------------------------------------------
// Procedure: setInfoBuffer()

void LogicTestSequence::setInfoBuffer(InfoBuffer *info_buffer)
{
  for(unsigned int i=0; i<m_aspects.size(); i++)
    m_aspects[i].setInfoBuffer(info_buffer);
  
  m_info_buffer = info_buffer;
}

//------------------------------------------------------------
// Procedure: addLeadCondition()

bool LogicTestSequence::addLeadCondition(string str)
{
  // The m_aspects vector always has at least one component
  unsigned int index = m_aspects.size()-1;
  
  // If a lead condition follows previous pass/fail conditions, then
  // interpret this as starting a new lead condition in a new aspect.
  if(m_aspects[index].hasPassFailConditions()) {
    LogicAspect new_aspect;
    new_aspect.setInfoBuffer(m_info_buffer);
    m_aspects.push_back(new_aspect);
    index++;
  }

  bool result = m_aspects[index].addLeadCondition(str);
  return(result);
}

//------------------------------------------------------------
// Procedure: addPassCondition()
//   Purpose: Add pass condition to most recently added LogicAspect

bool LogicTestSequence::addPassCondition(string str)
{
  // The m_aspects vector always has at least one component
  unsigned int index = m_aspects.size()-1;

  return(m_aspects[index].addPassCondition(str));
}

//------------------------------------------------------------
// Procedure: addFailCondition()
//   Purpose: Add pass condition to most recently added LogicAspect

bool LogicTestSequence::addFailCondition(string str)
{
  // The m_aspects vector always has at least one component
  unsigned int index = m_aspects.size()-1;

  return(m_aspects[index].addFailCondition(str));
}

//------------------------------------------------------------
// Procedure: checkConfig()
//   Purpuse: Usually called after initial configuration. Will return
//            a single warning, for the first aspect with a warning.

string LogicTestSequence::checkConfig()
{
  for(unsigned int i=0; i<m_aspects.size(); i++) {
    string warning = m_aspects[i].checkConfig();
    if(warning != "") {
      warning = "LogicAspect " + uintToString(0) + ": " + warning; 
      return(warning);
    }
  }
  
  m_enabled = true;
  return("");
}

//------------------------------------------------------------
// Procedure: update()

void LogicTestSequence::update()
{
  // If overall sequence evaluation previously completed, just return
  if(m_evaluated)
    return;
  
  // If all aspects evaluated with no failure, declare overall satisfied
  if(m_currix >= m_aspects.size()) {
    m_evaluated = true;
    m_satisfied = true;
    return;
  }


  // Update the the current aspect
  m_aspects[m_currix].update();
  //m_status = "[" + intToString(m_currix) + "]: ";
  m_status = m_aspects[m_currix].getStatus();

  // If current aspect is not evaluated, just return
  if(!m_aspects[m_currix].isEvaluated())
    return;

  // If this aspect is not satified, we declare the overall evaluation to
  // be complete, and not satisfied.
  if(!m_aspects[m_currix].isSatisfied()) {
    m_satisfied = false;
    m_evaluated = true;
    return;
  }

  // Otherwise if this aspect IS satisfied, on to the next one!
  m_currix++;

  // If this was the last aspect: since this was evaluated and
  // satisfied, the whole sequence is marked evaluated and satisfied
  if(m_currix >= m_aspects.size()) {
    m_evaluated = true;
    m_satisfied = true;
    return;
  }
}

//------------------------------------------------------------
// Procedure: getLogicVars()

set<string> LogicTestSequence::getLogicVars() const
{
  set<string> logic_vars;
  for(unsigned int i=0; i<m_aspects.size(); i++) {
    set<string> vars = m_aspects[i].getLogicVars();
    logic_vars.insert(vars.begin(), vars.end());
  }

  return(logic_vars);
}

//------------------------------------------------------------
// Procedure: getSpec()

vector<string> LogicTestSequence::getSpec() const
{
  vector<string> spec;
  string aspect_cnt = uintToString(m_aspects.size());
  spec.push_back("LogicTestSequence: Total Aspects:" + aspect_cnt); 
  spec.push_back("Enabled:   " + boolToString(m_enabled));
  spec.push_back("Evaluated: " + boolToString(m_evaluated));
  spec.push_back("Satisfied: " + boolToString(m_satisfied));
  spec.push_back("Status:    " + m_status);
  
  for(unsigned int i=0; i<m_aspects.size(); i++) {
    string sep = "++++++++++ aspect[" + intToString(i) + "] +++++++++++++"; 
    spec.push_back(sep);
    vector<string> aspec = m_aspects[i].getSpec();
    spec = mergeVectors(spec, aspec);
  }

  return(spec);
}


//---------------------------------------------------------
// Procecure: getReport()

list<string> LogicTestSequence::getReport() const
{
  list<string> report;

  string cnt = uintToString(m_aspects.size());
  report.push_back("Logic Test Sequence: Total Aspects (" + cnt + " total)");
  report.push_back("============================================");
  
  if(m_currix < m_aspects.size()) {
    unsigned int amt = m_aspects.size();
    list<string> rep = m_aspects[m_currix].getReport(m_currix+1, amt);
    report.merge(rep);
  }

  if(!m_info_buffer)
    return(report);

  vector<string> logic_vars;

  set<string> logic_vars_set = getLogicVars();
  set<string>::iterator p;
  for(p=logic_vars_set.begin(); p!=logic_vars_set.end(); p++)
    logic_vars.push_back(*p);
  
  report.push_back("");
  report.push_back("InfoBuffer:");
  report.push_back("============================================");
  vector<string> ibuff_report = m_info_buffer->getReport(logic_vars, true);
  if(ibuff_report.size() == 0)
    report.push_back("<empty>");
  for(unsigned int i=0; i<ibuff_report.size(); i++)
    report.push_back(ibuff_report[i]);

  return(report);
}

