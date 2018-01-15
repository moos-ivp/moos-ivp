/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmReport.cpp                                       */
/*    DATE: Sep 26th, 2006                                       */
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
#include "HelmReport.h"
#include "BuildUtils.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

HelmReport::HelmReport() 
{ 
  initialize();
}

//-----------------------------------------------------------
// Procedure: initialize

void HelmReport::initialize()
{
  m_warning_count = 0;
  m_time_utc      = 0;
  m_iteration     = 0;
  m_ofnum         = 0;
  m_create_time   = 0;
  m_solve_time    = 0;
  m_halted        = false;
  m_max_create_time = 0;
  m_max_solve_time  = 0;
  m_max_loop_time   = 0;

  m_total_pcs_formed = 0;
  m_total_pcs_cached = 0;
}

//-----------------------------------------------------------
// Procedure: clear

void HelmReport::clear(bool clear_completed)
{
  initialize();

  m_bhvs_running_desc.clear();
  m_bhvs_running_time.clear();
  m_bhvs_running_upds.clear();
  
  m_bhvs_idle_desc.clear();
  m_bhvs_idle_time.clear();
  m_bhvs_idle_upds.clear();
  
  if(clear_completed) {
    m_bhvs_completed_desc.clear();
    m_bhvs_completed_time.clear();
    m_bhvs_completed_upds.clear();
  }

  m_bhvs_active_desc.clear();
  m_bhvs_active_time.clear();
  m_bhvs_active_upds.clear();
  m_bhvs_active_pwt.clear();
  m_bhvs_active_cpu.clear();
  m_bhvs_active_pcs.clear();
  m_bhvs_active_ipfs.clear();

  m_update_results.clear();

  m_messages.clear();
  m_decisions.clear();

  m_domain = IvPDomain();
}

//-----------------------------------------------------------
// Procedure: addDecision()

void HelmReport::addDecision(const string& var, double val) 
{
  m_decisions[var] = val;
}

//-----------------------------------------------------------
// Procedure: setTemplatingSummary()

void HelmReport::setTemplatingSummary(const vector<string>& summary)
{
  m_templating_summary = summary;
}


//-----------------------------------------------------------
// Procedure: clearDecisions()

void HelmReport::clearDecisions()
{
  m_decisions.clear();
}


//-----------------------------------------------------------
// Procedure: getTemplatingSummary()

vector<string> HelmReport::getTemplatingSummary() const
{
  return(m_templating_summary);
}

//-----------------------------------------------------------
// Procedure: getDecision()

double HelmReport::getDecision(const string& var) const
{
  map<string, double>::const_iterator p = m_decisions.find(var);
  if(p == m_decisions.end())
    return(0);
  else
    return(p->second);
}

//-----------------------------------------------------------
// Procedure: hasDecision()

bool HelmReport::hasDecision(const string& var) const
{
  map<string, double>::const_iterator p = m_decisions.find(var);
  if(p != m_decisions.end())
    return(true);
  else
    return(false);
}

//-----------------------------------------------------------
// Procedure: changedBehaviors

bool HelmReport::changedBehaviors(const HelmReport& report) const
{
  // Want to base changes on simple list of behaviors, not full reports
  // of behaviors which includes timestamps etc.
  bool full=false;  
  
  if(getActiveBehaviors(full)  != report.getActiveBehaviors(full))
    return(true);
  if(getRunningBehaviors(full) != report.getRunningBehaviors(full))
    return(true);
  if(getIdleBehaviors(full)    != report.getIdleBehaviors(full))
    return(true);
  if(getCompletedBehaviors(full) != report.getCompletedBehaviors(full))
    return(true);
  return(false);
}

//-----------------------------------------------------------
// Procedure: getDecisionSummary()

string HelmReport::getDecisionSummary() const
{
  string summary;

  unsigned int i, domain_size = m_domain.size();
  for(i=0; i<domain_size; i++) {
    string varname = m_domain.getVarName(i);
    if(i!=0)
      summary += ",";
    summary += "var=" + varname + ":";
    map<string,double>::const_iterator p = m_decisions.find(varname);
    if(p == m_decisions.end())
      summary += "varbalk";
    else
      summary += doubleToStringX(p->second);
  }
  return(summary);
}


//-----------------------------------------------------------
// Procedure: addActiveBHV

void HelmReport::addActiveBHV(const string& descriptor, double time, 
			      double pwt, int pcs, double cpu_time, 
			      const string& update_summary, 
			      unsigned int ipfs)
{
  m_bhvs_active_desc.push_back(descriptor);
  m_bhvs_active_time.push_back(time);
  m_bhvs_active_upds.push_back(update_summary);
  m_bhvs_active_pwt.push_back(pwt);
  m_bhvs_active_pcs.push_back(pcs);
  m_bhvs_active_cpu.push_back(cpu_time);
  m_bhvs_active_ipfs.push_back(ipfs);
}
  
//-----------------------------------------------------------
// Procedure: getActiveBehaviors

string HelmReport::getActiveBehaviors(bool full_report) const
{
  string return_str;
  unsigned int i, vsize = m_bhvs_active_desc.size();
  for(i=0; i<vsize; i++) {
    if(full_report) {
      if(i>0)
	return_str += ":";
      return_str += m_bhvs_active_desc[i];
      return_str += "$" + doubleToString(m_bhvs_active_time[i],2);
      return_str += "$" + doubleToString(m_bhvs_active_pwt[i]);
      return_str += "$" + intToString(m_bhvs_active_pcs[i]);
      return_str += "$" + doubleToString(m_bhvs_active_cpu[i]);
      return_str += "$" + m_bhvs_active_upds[i];
      return_str += "$" + uintToString(m_bhvs_active_ipfs[i]);
    }
    else {
      if(i>0)
	return_str += ",";
      return_str += m_bhvs_active_desc[i];
    }
  }

  if(return_str == "none")
    return_str = "";
  return(return_str);
}

//-----------------------------------------------------------
// Procedure: clearActiveBHVs

void HelmReport::clearActiveBHVs() 
{
  m_bhvs_active_desc.clear();
  m_bhvs_active_time.clear();
  m_bhvs_active_upds.clear();
  m_bhvs_active_pwt.clear();
  m_bhvs_active_cpu.clear();
  m_bhvs_active_pcs.clear();
  m_bhvs_active_ipfs.clear();
}

//-----------------------------------------------------------
// Procedure: addRunningBHV

void HelmReport::addRunningBHV(const string& descriptor, double time, 
			       const string& update_summary)
{
  m_bhvs_running_desc.push_back(descriptor);
  m_bhvs_running_time.push_back(time);
  m_bhvs_running_upds.push_back(update_summary);
}

//-----------------------------------------------------------
// Procedure: getRunningBehaviors
//   Example: "loiter$1252348077.59$4/4 : return$1252348047.12$0/0"

string HelmReport::getRunningBehaviors(bool full_report) const
{
  string return_str;
  unsigned int i, vsize = m_bhvs_running_desc.size();
  for(i=0; i<vsize; i++) {
    if(full_report) {
      if(i>0)
	return_str += ":";
      return_str += m_bhvs_running_desc[i];
      return_str += "$" + doubleToString(m_bhvs_running_time[i],2);
      return_str += "$" + m_bhvs_running_upds[i];
    }
    else {
      if(i>0)
	return_str += ",";
      return_str += m_bhvs_running_desc[i];
    }
  }
  if(return_str == "none")
    return_str = "";
  return(return_str);
}

//-----------------------------------------------------------
// Procedure: clearRunningBHVs

void HelmReport::clearRunningBHVs() 
{
  m_bhvs_running_desc.clear();
  m_bhvs_running_time.clear();
  m_bhvs_running_upds.clear();
}


//-----------------------------------------------------------
// Procedure: addIdleBHV
//            getIdleBehaviors

void HelmReport::addIdleBHV(const string& descriptor, double time,
			    const string& update_summary)
{
  m_bhvs_idle_desc.push_back(descriptor);
  m_bhvs_idle_time.push_back(time);
  m_bhvs_idle_upds.push_back(update_summary);
}

//-----------------------------------------------------------
// Procedure: getIdleBehaviors
//   Example: "loiter$1252348077.59$4/4 : return$1252348047.12$0/0"

string HelmReport::getIdleBehaviors(bool full_report) const
{
  string return_str;
  unsigned int i, vsize = m_bhvs_idle_desc.size();
  for(i=0; i<vsize; i++) {
    if(full_report) {
      if(i>0)
	return_str += ":";
      return_str += m_bhvs_idle_desc[i];
      return_str += "$" + doubleToString(m_bhvs_idle_time[i],2);
      return_str += "$" + m_bhvs_idle_upds[i];
    }
    else {
      if(i>0)
	return_str += ",";
      return_str += m_bhvs_idle_desc[i];
    }
  }
  if(return_str == "none")
    return_str = "";
  return(return_str);
}

//-----------------------------------------------------------
// Procedure: clearIdleBHVs

void HelmReport::clearIdleBHVs() 
{
  m_bhvs_idle_desc.clear();
  m_bhvs_idle_time.clear();
  m_bhvs_idle_upds.clear();
}

//-----------------------------------------------------------
// Procedure: addCompletedBHV

void HelmReport::addCompletedBHV(const string& descriptor, double time,
				 const string& update_summary)
{
  if(descriptor == "")
    return;

  m_bhvs_completed_desc.push_back(descriptor);
  m_bhvs_completed_time.push_back(time);
  m_bhvs_completed_upds.push_back(update_summary);
  

  if(m_bhvs_completed_desc.size() > 50) {
    m_bhvs_completed_desc.pop_front();
    m_bhvs_completed_time.pop_front();
    m_bhvs_completed_upds.pop_front();
  }
}

//-----------------------------------------------------------
// Procedure: getCompletedBehaviors
//   Example: "loiter$1252348077.59$4/4 : return$1252348047.12$0/0"

string HelmReport::getCompletedBehaviors(bool full_report) const
{
  string return_str;

  unsigned int i, vsize = m_bhvs_completed_desc.size();
  for(i=0; i<vsize; i++) {
    if(full_report) {
      if(i>0)
	return_str += ":";
      return_str += m_bhvs_completed_desc[i];
      return_str += "$" + doubleToString(m_bhvs_completed_time[i],2);
      return_str += "$" + m_bhvs_completed_upds[i];
    }
    else {
      if(i>0)
	return_str += ",";
      return_str += m_bhvs_completed_desc[i];
    }
  }
  if(return_str == "none")
    return_str = "";
  return(return_str);
}


//-----------------------------------------------------------
// Procedure: clearCompletedBHVs

void HelmReport::clearCompletedBHVs() 
{
  m_bhvs_completed_desc.clear();
  m_bhvs_completed_time.clear();
  m_bhvs_completed_upds.clear();
}


//-----------------------------------------------------------
// Procedure: getDomainString()
//   Example: [speed,0,5,26] [course,0,359,360] 

string HelmReport::getDomainString() const
{
  return(domainToString(m_domain));

#if 0
  string return_string;

  unsigned int i, vsize = m_domain.size();
  for(i=0; i<vsize; i++) {
    if(i>0)
      return_string += " ";
    return_string += "[" + m_domain.getVarName(i);
    return_string += "," + doubleToStringX(m_domain.getVarLow(i));
    return_string += "," + doubleToStringX(m_domain.getVarHigh(i));
    return_string += "," + uintToString(m_domain.getVarPoints(i));
    return_string += "]";
  }
  return(return_string);
#endif
}

//-----------------------------------------------------------
// Procedure: getReportAsString()

string HelmReport::getReportAsString() const
{
  HelmReport empty_report;
  return(getReportAsString(empty_report, true));
}


//-----------------------------------------------------------
// Procedure: getReportAsString(const HelmReport&)

string HelmReport::getReportAsString(const HelmReport& prep, bool full) const
{
  string report = "iter=" + uintToString(m_iteration);
  report += (",utc_time=" + doubleToString(m_time_utc, 2));

  if(full || (m_ofnum != prep.getOFNUM()))
    report += (",ofnum=" + uintToString(m_ofnum));
  if(full || (m_total_pcs_formed != prep.getTotalPcsFormed()))
    report += (",total_pcs_formed=" + uintToString(m_total_pcs_formed));
  if(full || (m_total_pcs_cached != prep.getTotalPcsCached()))
    report += (",total_pcs_cached=" + uintToString(m_total_pcs_cached));
  if(full || (m_warning_count != prep.getWarnings()))
    report += (",warnings=" + uintToString(m_warning_count));
  if(full || (m_solve_time != prep.getSolveTime()))
    report += (",solve_time=" + doubleToString(m_solve_time, 2));
  if(full || (m_create_time != prep.getCreateTime()))
    report += (",create_time=" + doubleToString(m_create_time, 2));

  if(full || (m_max_create_time != prep.getMaxCreateTime()))
    report += (",max_create_time=" + doubleToString(m_max_create_time, 2));
  if(full || (m_max_solve_time != prep.getMaxSolveTime()))
    report += (",max_solve_time=" + doubleToString(m_max_solve_time, 2));
  if(full || (m_max_loop_time != prep.getMaxLoopTime()))
    report += (",max_loop_time=" + doubleToString(m_max_loop_time, 2));

  double loop_time = m_create_time + m_solve_time;
  if(full || (loop_time != prep.getLoopTime()))
    report += (",loop_time=" + doubleToString(loop_time, 2));
  
  string decision_summary = getDecisionSummary();
  if(full || (decision_summary != prep.getDecisionSummary()))
    report += "," + decision_summary;

  if(full || (m_halted != prep.getHalted()))
    report += (",halted=" + boolToString(m_halted));

  if(full || (m_modes != prep.getModeSummary())) {
    report += ",modes=";
    if(m_modes == "")
      report += "none";
    else
      report += m_modes;
  }

  string running_bhvs = getRunningBehaviors();
  if(full || (running_bhvs != prep.getRunningBehaviors())) {
    report += ",running_bhvs=";
    if(running_bhvs == "")
      report += "none";
    else
      report += running_bhvs;
  }

  string active_bhvs = getActiveBehaviors();
  if(full || (active_bhvs != prep.getActiveBehaviors())) {
    report += ",active_bhvs=";
    if(active_bhvs == "")
      report += "none";
    else
      report += active_bhvs;
  }

  string idle_bhvs = getIdleBehaviors();
  if(full || (idle_bhvs != prep.getIdleBehaviors())) {
    report += ",idle_bhvs=";
    if(idle_bhvs == "")
      report += "none";
    else
      report += idle_bhvs;
  }

  string completed_bhvs = getCompletedBehaviors();
  if(full || (completed_bhvs != prep.getCompletedBehaviors())) {
    report += ",completed_bhvs=";
    if(completed_bhvs == "") {
      if(prep.getCompletedBehaviors() == "")
	report += "none";
    }
    else
      report += completed_bhvs;
  }

  string domain_str = domainToString(m_domain);
  if(full || (domain_str != prep.getDomainString()))
    report += ",ivpdomain=\"" + domain_str + "\"";

  if(full || (m_halt_message != prep.getHaltMsg())) {
    report += ",halt_msg=";
    if(m_halt_message == "")
      report += "none";
    else
      report += m_halt_message;
  }

  return(report);
}

//---------------------------------------------------------
// Procedure: print()

void HelmReport::print() const
{
  cout << "HelmReport:-----------------------------------" << endl;
  cout << "halt_message:" << m_halt_message << endl;
  cout << "active_bhvs:" << getActiveBehaviors() << endl;
  cout << "running_bhvs:" << getRunningBehaviors() << endl;
  cout << "completed_bhvs:" << getCompletedBehaviors() << endl;
  cout << "idle_bhvs:" << getIdleBehaviors() << endl;
  cout << "modes:" << m_modes << endl;
  cout << "warning_count:" << m_warning_count << endl;
  cout << "iteration:" << m_iteration << endl;
  cout << "ofnum:" << m_ofnum << endl;
  cout << "total_pcs_formed:" << m_total_pcs_formed << endl;
  cout << "total_pcs_cached:" << m_total_pcs_cached << endl;
  cout << "halted:" << boolToString(m_halted) << endl;
}


//---------------------------------------------------------
// Procedure: formattedSummary()
//   
// 
//  Helm Iteration: 2       (hz=0.25)(1)  (hz=0.25)(1)  (hz=0.25)(max)
//    IvP functions:  0
//    Mode(s):        ACTIVE:LOITERING
//    SolveTime:      0.00    (max=0.00)
//    CreateTime:     0.00    (max=0.00)
//    LoopTime:       0.00    (max=0.00)
//    Halted:         false   (0 warnings: 0 total)
//  Helm Decision: [speed,0,5,26] [course,0,359,360] 
//    course = 195
//    speed  = 1.2
//  Behaviors Active: ---------- (1)
//    loiter [0.25] (pwt=100.00000) (pcs=9) (cpu=0.00000) (upd=0/0)
//  Behaviors Running: --------- (0)
//  Behaviors Idle: ------------ (2)
//    waypt_return, station-keep
//  Behaviors Completed: ------- (0)
//
list<string> HelmReport::formattedSummary(double curr_time, bool verbose) const
{
  list<string> rlist;

  string str;

  rlist.push_back("  Helm Iteration:  " + uintToString(m_iteration));
  rlist.push_back("  IvP Functions:   " + uintToString(m_ofnum));
  rlist.push_back("  Pieces (Formed): " + uintToString(m_total_pcs_formed));
  rlist.push_back("  Pieces (Cached): " + uintToString(m_total_pcs_cached));
  rlist.push_back("  Mode(s):         " + m_modes);

  str =  "  SolveTime:   " + doubleToString(m_solve_time,2);
  str += "   (max=" + doubleToString(m_max_solve_time,2) + ")";
  rlist.push_back(str);

  str =  "  CreateTime:  " + doubleToString(m_create_time,2);
  str += "   (max=" + doubleToString(m_max_create_time,2) + ")";
  rlist.push_back(str);

  str =  "  LoopTime:    " + doubleToString(getLoopTime(),2);
  str += "   (max=" + doubleToString(m_max_loop_time,2) + ")";
  rlist.push_back(str);

  str = "  Halted:         " + boolToString(m_halted);
  str += "   (" + uintToString(m_warning_count) + " warnings)";
  rlist.push_back(str);

  string domain_str = "[" + domainToString(m_domain) + "]";
  domain_str = findReplace(domain_str, ":", "] [");
  rlist.push_back("Helm Decision: " + domain_str);
  
  map<string, double>::const_iterator p;
  unsigned int i, vsize = m_domain.size();
  for(i=0; i<vsize; i++) {
    string varname = m_domain.getVarName(i);
    p = m_decisions.find(varname);
    if(p == m_decisions.end())
      rlist.push_back("  " + varname + " = no-decision");
    else
      rlist.push_back("  " + varname + " = " + doubleToStringX(p->second));
  }
  
  string active_bhvs    = uintToString(m_bhvs_active_desc.size());
  string running_bhvs   = uintToString(m_bhvs_running_desc.size());
  string idle_bhvs      = uintToString(m_bhvs_idle_desc.size());
  string completed_bhvs = uintToString(m_bhvs_completed_desc.size());

  string spawnable_bhvs = uintToString(m_templating_summary.size());
  rlist.push_back("Behaviors Spawnable: --------- (" + spawnable_bhvs + ")");
  for(unsigned i=0; i<m_templating_summary.size(); i++) {
    str = "  " + m_templating_summary[i];
    rlist.push_back(str);
  }

  rlist.push_back("Behaviors Active: ---------- (" + active_bhvs + ")");
  vsize = m_bhvs_active_desc.size();
  for(i=0; i<vsize; i++) {
    str = "  " + m_bhvs_active_desc[i];
    string time_in_state = timeInState(curr_time, m_bhvs_active_time[i]);
    str += " [" + time_in_state + "]";
    str += " (pwt=" + doubleToStringX(m_bhvs_active_pwt[i]) + ")";
    str += " (pcs=" + intToString(m_bhvs_active_pcs[i]) + ")";
    str += " (cpu=" + doubleToStringX(m_bhvs_active_cpu[i]) + ")";
    str += " (upd=" + m_bhvs_active_upds[i] + ")";
    rlist.push_back(str);
  }

  
  rlist.push_back("Behaviors Running: --------- (" + running_bhvs + ")");
  vsize = m_bhvs_running_desc.size();
  for(i=0; i<vsize; i++) {
    str = "  " + m_bhvs_running_desc[i];
    string time_in_state = timeInState(curr_time, m_bhvs_running_time[i]);
    str += " [" + time_in_state + "]";
    rlist.push_back(str);
  }

  rlist.push_back("Behaviors Idle: ------------ (" + idle_bhvs + ")");
  vsize = m_bhvs_idle_desc.size();
  if(verbose) {
    for(i=0; i<vsize; i++) {
      str = "  " + m_bhvs_idle_desc[i];
      string time_in_state = timeInState(curr_time, m_bhvs_idle_time[i]);
      str += " [" + time_in_state + "]";
      rlist.push_back(str);
    }
  }
  else {
    str = "  ";
    for(i=0; i<vsize; i++) {
      if(i>0)
	str += ", ";
      str += m_bhvs_idle_desc[i];
      string time_in_state = timeInState(curr_time, m_bhvs_idle_time[i]);
      str += "[" + time_in_state + "]";
    }
    rlist.push_back(str);
  }

  rlist.push_back("Behaviors Completed: ------- (" + completed_bhvs + ")");
  vsize = m_bhvs_completed_desc.size();
  if(verbose) {
    for(i=0; i<vsize; i++) {
      str = "  {" + m_bhvs_completed_desc[i] + "}";
      string time_in_state = timeInState(curr_time, m_bhvs_completed_time[i]);
      str += " [" + time_in_state + "]";
      rlist.push_back(str);
    }
  }
  else {
    if(vsize > 0) {
      str = "  ";
      for(i=0; i<vsize; i++) {
	if(i>0) 
	  str += ", ";
	str += m_bhvs_completed_desc[i];
	string time_in_state = timeInState(curr_time, m_bhvs_completed_time[i]);
	str += "[" + time_in_state + "]";
      }
      rlist.push_back(str);
    }
  }
  
  return(rlist);
}

//---------------------------------------------------------
// Procedure: timeInState()

string HelmReport::timeInState(double curr_time, double mark_time) const
{
  string s_time_in_state = "always";
  if(mark_time > 0) {
    double d_time_in_state = curr_time - mark_time;
    s_time_in_state = doubleToString(d_time_in_state,2);
  }
  return(s_time_in_state);
}









