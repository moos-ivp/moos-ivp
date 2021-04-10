/****************************************************************/
/*   NAME: Mike Benjamin                                        */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: LCheckSet.cpp                                        */
/*   DATE: Oct 5th, 2020                                        */
/****************************************************************/

#include "MBUtils.h"
#include "ACTable.h"
#include "LCheckSet.h"

using namespace std;

//---------------------------------------------------------
// Constructor

LCheckSet::LCheckSet()
{
  m_lead_conditions_met = false;
  m_eval_conditions_evaluated = false;
  m_eval_conditions_met = false;
}

//------------------------------------------------------------
// Procedure: addLeadCondition()

bool LCheckSet::addLeadCondition(string str)
{
  return(m_lead_cbuff.addNewCondition(str));
}

//------------------------------------------------------------
// Procedure: addPassCondition()

bool LCheckSet::addPassCondition(string str)
{
  return(m_pass_cbuff.addNewCondition(str));
}

//------------------------------------------------------------
// Procedure: addFailCondition()

bool LCheckSet::addFailCondition(string str)
{
  return(m_fail_cbuff.addNewCondition(str));
}

//------------------------------------------------------------
// Procedure: update()

void LCheckSet::update(double curr_time)
{
  m_lead_cbuff.setCurrTime(curr_time);
  m_pass_cbuff.setCurrTime(curr_time);
  m_fail_cbuff.setCurrTime(curr_time);

  // If lead conditions not yet met, try again
  if(!m_lead_conditions_met)
    m_lead_conditions_met = m_lead_cbuff.checkConditions();
  
  if(!m_lead_conditions_met) {
    string notable_cond = m_lead_cbuff.getNotableCondition();
    m_lcheck_status = "unmet_lead_cond:" + notable_cond;
    return;
  }
  
  if(m_eval_conditions_evaluated)
    return;
  m_eval_conditions_evaluated = true;

  // Check Pass/Fail conditions. 
  bool all_pass_conds_met = m_pass_cbuff.checkConditions("all");
  bool any_fail_conds_met = m_fail_cbuff.checkConditions("any");

  if(!all_pass_conds_met) {
    string notable_cond = m_pass_cbuff.getNotableCondition();
    m_lcheck_status = "unmet_pass_cond:" + notable_cond;
    m_eval_conditions_met = false;
  }
  else if(any_fail_conds_met) {
    string notable_cond = m_fail_cbuff.getNotableCondition();
    m_lcheck_status = "met_fail_cond:" + notable_cond;
    m_eval_conditions_met = false;
  }
  else {
    m_lcheck_status = "pass";
    m_eval_conditions_met = true;
  }
}


//------------------------------------------------------------
// Procedure: checkConfig()

string LCheckSet::checkConfig()
{
  set<string> lead_vars = m_lead_cbuff.getAllVarsSet();
  set<string> pass_vars = m_pass_cbuff.getAllVarsSet();
  set<string> fail_vars = m_fail_cbuff.getAllVarsSet();
  m_logic_vars.insert(lead_vars.begin(), lead_vars.end());
  m_logic_vars.insert(pass_vars.begin(), pass_vars.end());
  m_logic_vars.insert(fail_vars.begin(), fail_vars.end());

  string warning;
  // Sanity check 1: eval conditions with no lead condition
  if((m_pass_cbuff.size() > 0) || (m_fail_cbuff.size() > 0)) {
    if(m_lead_cbuff.size() == 0)
      warning = "Logic-based checks need a lead_condition";
  }

  // Sanity check 2: no eval conditions but lead condition given
  if((m_pass_cbuff.size() == 0) && (m_fail_cbuff.size() == 0)) {
    if(m_lead_cbuff.size() > 0)
      warning = "Logic-based checks have pointless lead_condition";
  }

  if(!enabled()) {
    m_eval_conditions_evaluated = true;
    m_eval_conditions_met = true;
    m_lcheck_status = "unused";
  }

  if(m_lead_cbuff.size() != 0)
    m_lcheck_status = "unmet_lead_conds";

  return(warning);
}


//---------------------------------------------------------
// Procecure: enabled()

bool LCheckSet::enabled() const
{
  if((m_pass_cbuff.size() > 0) || (m_fail_cbuff.size() > 0)) 
    if(m_lead_cbuff.size() > 0)
      return(true);
  
  return(false);
}

//---------------------------------------------------------
// Procecure: isEvaluated()
 
bool LCheckSet::isEvaluated() const
{
  return(m_eval_conditions_evaluated);
}
  

//---------------------------------------------------------
// Procecure: isSatisfied()

bool LCheckSet::isSatisfied() const
{
  return(m_eval_conditions_met);
}
  
//------------------------------------------------------------
// Procedure: handleMail()

void LCheckSet::handleMail(std::string key, std::string sval)
{
  if(m_logic_vars.count(key)) {
    m_lead_cbuff.updateInfoBuffer(key, sval);
    m_pass_cbuff.updateInfoBuffer(key, sval);
    m_fail_cbuff.updateInfoBuffer(key, sval);
  }
}

//------------------------------------------------------------
// Procedure: handleMail()

void LCheckSet::handleMail(std::string key, double dval)
{
  if(m_logic_vars.count(key)) {
    m_lead_cbuff.updateInfoBuffer(key, dval);
    m_pass_cbuff.updateInfoBuffer(key, dval);
    m_fail_cbuff.updateInfoBuffer(key, dval);
  }
}


//------------------------------------------------------------
// Procedure: getInfoBufferReport()

set<string> LCheckSet::getInfoBufferReport() const
{
  set<string> total_report;

  vector<string> lead_report = m_lead_cbuff.getInfoBuffReport(true);
  vector<string> pass_report = m_lead_cbuff.getInfoBuffReport(true);
  vector<string> fail_report = m_lead_cbuff.getInfoBuffReport(true);

  for(unsigned int i=0; i<lead_report.size(); i++)
    total_report.insert(lead_report[i]);
  for(unsigned int i=0; i<pass_report.size(); i++)
    total_report.insert(pass_report[i]);
  for(unsigned int i=0; i<fail_report.size(); i++)
    total_report.insert(fail_report[i]);

  return(total_report);
}


//---------------------------------------------------------
// Procecure: getReport()

list<string> LCheckSet::getReport() const
{
  list<string> rep;
  
  string lead_count_str = uintToString(m_lead_cbuff.size());
  string pass_count_str = uintToString(m_pass_cbuff.size());
  string fail_count_str = uintToString(m_fail_cbuff.size());
  string lvar_count_str = uintToString(m_logic_vars.size());
  
  rep.push_back("LCheck Config:");
  rep.push_back("============================================");
  rep.push_back("   lead_conditions: " + lead_count_str);
  rep.push_back("   pass_conditions: " + pass_count_str);
  rep.push_back("   fail_conditions: " + fail_count_str);
  rep.push_back("   logic_variables: " + lvar_count_str);
  rep.push_back("");
  
  string lcon_met_str  = boolToString(m_lead_conditions_met);
  string econ_eval_str = boolToString(m_eval_conditions_evaluated);
  string econ_met_str  = boolToString(m_eval_conditions_met);
  rep.push_back("");
  rep.push_back("LCheck State:                               ");
  rep.push_back("============================================");
  rep.push_back(" lead_conditions_met    = " + lcon_met_str);
  rep.push_back(" eval_conditions_evaled = " + econ_eval_str);
  rep.push_back(" eval_conditions_met    = " + econ_met_str);
  rep.push_back(" mission_eval_status    = " + m_lcheck_status);

  double lead_curr_time = m_lead_cbuff.getCurrTime();
  rep.push_back("");
  rep.push_back("InfoBuffer: (lead conditions)");
  rep.push_back("============================================");
  rep.push_back(" curr_time: " + doubleToString(lead_curr_time,2));
  vector<string> ibl_report = m_lead_cbuff.getInfoBuffReport();
  if(ibl_report.size() == 0)
    rep.push_back("<empty>");
  for(unsigned int i=0; i<ibl_report.size(); i++)
    rep.push_back(ibl_report[i]);

  rep.push_back("");
  double pass_curr_time = m_pass_cbuff.getCurrTime();
  rep.push_back("InfoBuffer: (pass conditions) ");
  rep.push_back("============================================");
  rep.push_back(" curr_time: " + doubleToString(pass_curr_time,2));
  vector<string> ibp_report = m_pass_cbuff.getInfoBuffReport();
  if(ibp_report.size() == 0)
    rep.push_back("<empty>");
  for(unsigned int i=0; i<ibp_report.size(); i++)
    rep.push_back(ibp_report[i]);

  rep.push_back("");
  double fail_curr_time = m_fail_cbuff.getCurrTime();
  rep.push_back("InfoBuffer: (fail conditions)");
  rep.push_back("============================================");
  rep.push_back(" curr_time: " + doubleToString(fail_curr_time,2));
  vector<string> ibf_report = m_fail_cbuff.getInfoBuffReport();
  if(ibf_report.size() == 0)
    rep.push_back("<empty>");
  for(unsigned int i=0; i<ibf_report.size(); i++)
    rep.push_back(ibf_report[i]);

  return(rep);
}
