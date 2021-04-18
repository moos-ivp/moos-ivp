/****************************************************************/
/*   NAME: Mike Benjamin                                        */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: LogicAspect.cpp                                      */
/*   DATE: Oct 5th, 2020                                        */
/****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "LogicAspect.h"

using namespace std;

//---------------------------------------------------------
// Constructor

LogicAspect::LogicAspect()
{
  m_evaluated = false;
  m_satisfied = false;
  m_enabled   = false;
  m_status    = "pending";

  m_info_buffer = 0;
}

//------------------------------------------------------------
// Procedure: setInfoBuffer()
//      Note: The three LogicConditions are capable of working
//            with their own InfoBuffer. By setting buffer here,
//            we are tying it to an externally shared buffer.

void LogicAspect::setInfoBuffer(InfoBuffer *info_buffer)
{
  m_lead_cbuff.setInfoBuffer(info_buffer);
  m_pass_cbuff.setInfoBuffer(info_buffer);
  m_fail_cbuff.setInfoBuffer(info_buffer);

  m_info_buffer = info_buffer;
}

//------------------------------------------------------------
// Procedure: addLeadCondition()

bool LogicAspect::addLeadCondition(string str)
{
  return(m_lead_cbuff.addNewCondition(str));
}

//------------------------------------------------------------
// Procedure: addPassCondition()

bool LogicAspect::addPassCondition(string str)
{
  return(m_pass_cbuff.addNewCondition(str));
}

//------------------------------------------------------------
// Procedure: addFailCondition()

bool LogicAspect::addFailCondition(string str)
{
  return(m_fail_cbuff.addNewCondition(str));
}

//------------------------------------------------------------
// Procedure: hasPassFailConditions()

bool LogicAspect::hasPassFailConditions() const
{
  if((m_pass_cbuff.size() == 0) && (m_fail_cbuff.size() == 0))
    return(false);

  return(true);
}

//------------------------------------------------------------
// Procedure: getInfo()

string LogicAspect::getInfo()
{
  string str;
  str += "lead_cnt=" + uintToString(m_lead_cbuff.size());
  str += ", pass_cnt=" + uintToString(m_pass_cbuff.size());
  str += ", fail_cnt=" + uintToString(m_fail_cbuff.size());

  return(str);
}

//------------------------------------------------------------
// Procedure: update()

void LogicAspect::update()
{
  if(m_evaluated)
    return;

  
  bool lead_conditions_met = m_lead_cbuff.checkConditions();
  if(!lead_conditions_met) {
    string notable_cond = m_lead_cbuff.getNotableCondition();
    m_status = "unmet_lead_cond: [" + notable_cond + "]";
    return;
  }
  
  // Check Pass/Fail conditions. 
  bool all_pass_conds_met = m_pass_cbuff.checkConditions("all");
  bool any_fail_conds_met = m_fail_cbuff.checkConditions("any");

  if(!all_pass_conds_met) {
    string notable_cond = m_pass_cbuff.getNotableCondition();
    m_status = "unmet_pass_cond: [" + notable_cond + "]";
  }
  else if(any_fail_conds_met) {
    string notable_cond = m_fail_cbuff.getNotableCondition();
    m_status = "met_fail_cond: [" + notable_cond + "]";
  }
  else {
    m_status = "pass";
    m_satisfied = true;
  }

  m_evaluated = true;
}


//------------------------------------------------------------
// Procedure: getLogicVars()

set<string> LogicAspect::getLogicVars() const
{
  set<string> logic_vars;
  
  set<string> lead_vars = m_lead_cbuff.getAllVarsSet();
  set<string> pass_vars = m_pass_cbuff.getAllVarsSet();
  set<string> fail_vars = m_fail_cbuff.getAllVarsSet();
  logic_vars.insert(lead_vars.begin(), lead_vars.end());
  logic_vars.insert(pass_vars.begin(), pass_vars.end());
  logic_vars.insert(fail_vars.begin(), fail_vars.end());

  return(logic_vars);
}


//------------------------------------------------------------
// Procedure: checkConfig()

string LogicAspect::checkConfig()
{
  unsigned int lead_cnt = m_lead_cbuff.size();
  unsigned int pass_cnt = m_pass_cbuff.size();
  unsigned int fail_cnt = m_fail_cbuff.size();
  
  if((lead_cnt == 0) && (pass_cnt == 0) && (fail_cnt == 0))
    return("No lead conditions or pass/fail conditions");
  
  if(lead_cnt == 0)
    return("No lead_conditions");

  if((pass_cnt == 0) && (fail_cnt == 0)) 
    return("No pass/fail conditions");

  m_enabled = true;
  return("");
}

//------------------------------------------------------------
// Procedure: getSpec()

vector<string> LogicAspect::getSpec() const
{
  vector<string> spec;

  spec.push_back("LogicAspect:");

  spec.push_back("Lead Conditions:");
  vector<string> lead_spec = m_lead_cbuff.getSpec("  ");
  spec = mergeVectors(spec, lead_spec);

  spec.push_back("Pass Conditions:");
  vector<string> pass_spec = m_pass_cbuff.getSpec("  ");
  spec = mergeVectors(spec, pass_spec);

  spec.push_back("Fail Conditions:");
  vector<string> fail_spec = m_fail_cbuff.getSpec("  ");
  spec = mergeVectors(spec, fail_spec);

  spec.push_back("Enabled:   " + boolToString(m_enabled));
  spec.push_back("Evaluated: " + boolToString(m_evaluated));
  spec.push_back("Satisfied: " + boolToString(m_satisfied));
  spec.push_back("Status:    " + m_status);
  
  return(spec);
}


//---------------------------------------------------------
// Procecure: getReport()

list<string> LogicAspect::getReport(int ix, int total) const
{
  list<string> rep;

  string stat;
  if((ix >= 0) && (total >= 0))
    stat = intToString(ix) + " of " + intToString(total);
  
  string lead_count_str = uintToString(m_lead_cbuff.size());
  string pass_count_str = uintToString(m_pass_cbuff.size());
  string fail_count_str = uintToString(m_fail_cbuff.size());
  
  rep.push_back("LogicAspect Config: " + stat);
  rep.push_back("============================================");
  rep.push_back("  lead_conditions: " + lead_count_str);
  rep.push_back("  pass_conditions: " + pass_count_str);
  rep.push_back("  fail_conditions: " + fail_count_str);
  rep.push_back("");
  
  string evaluated_str = boolToString(m_evaluated);
  string satisfied_str = boolToString(m_satisfied);
  rep.push_back("LogicAspect State: " + stat);
  rep.push_back("============================================");
  rep.push_back("  evaluated = " + evaluated_str);
  rep.push_back("  satisfied = " + satisfied_str);
  rep.push_back("  status    = " + m_status);

  return(rep);
}
