/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: VCheckSet.cpp                                        */
/*    DATE: Sep 19th, 2020                                       */
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

#include <algorithm>
#include "MBUtils.h"
#include "VCheckSet.h"
#include "VarDataPairUtils.h"
#include "ACTable.h"

using namespace std;

//---------------------------------------------------------
// Constructor

VCheckSet::VCheckSet()
{
  // Init config vars
  m_fail_on_first = false;
  m_max_report_size = 40;
  
  // Init state vars
  m_start_time   = -1;
  m_elapsed_time = -1;
  m_total_passed = 0;
  m_total_failed = 0;
}


//---------------------------------------------------------
// Procecure: addVCheck()

bool VCheckSet::addVCheck(string str)
{
  VCheck vcheck;
  bool ok = vcheck.setVarCheck(str);
  if(!ok)
    return(false);

  m_vchecks.push_back(vcheck);
  m_vcheck_vars.insert(vcheck.getVarName());

  std::sort(m_vchecks.begin(), m_vchecks.end());
  std::reverse(m_vchecks.begin(), m_vchecks.end());
  
  return(true);
}
  
//---------------------------------------------------------
// Procecure: setStartTrigger()

bool VCheckSet::setStartTrigger(string str)
{
  if(m_start_trigger.get_var() != "")
    return(false);

  bool ok = setVarDataPairOnString(m_start_trigger, str);
  if(!ok)
    return(false);

  m_vcheck_vars.insert(m_start_trigger.get_var());
  return(true);
}
  
//---------------------------------------------------------
// Procecure: setFailOnFirst()

bool VCheckSet::setFailOnFirst(string str)
{
  return(setBooleanOnString(m_fail_on_first, str));
}
  
//---------------------------------------------------------
// Procecure: setMaxReportSize()

bool VCheckSet::setMaxReportSize(string str)
{
  return(setUIntOnString(m_max_report_size, str));
}
  
//---------------------------------------------------------
// Procecure: enabled()

bool VCheckSet::enabled() const
{
  if(m_start_trigger.valid() && (m_vchecks.size() > 0))
    return(true);
  return(false);
}
  
//---------------------------------------------------------
// Procecure: getStatus()

string VCheckSet::getStatus() const
{
  if(!enabled())
    return("unused");
  
  if(!isEvaluated())
    return("pending");
  
  if(!isSatisfied())
    return("fail");
  
  return("pass");
}

//---------------------------------------------------------
// Procecure: checkConfig()

string VCheckSet::checkConfig() const
{
  string msg;
  
  // These two warning situations are mutually exclusive
  
  if((m_vchecks.size() > 0) && !m_start_trigger.valid())
    msg = "A valid vcheck_start is missing";
  
  if((m_vchecks.size() == 0) && m_start_trigger.valid())
    msg = "A valid vcheck is missing";
  
  return(msg);
}

  
//---------------------------------------------------------
// Procecure: handleMail()

void VCheckSet::handleMail(string key, string sval, double dval, double mtime)
{
  if(key == m_start_trigger.get_var())
    return(handleMailKey(key, sval, dval, mtime));
  else
    return(handleMailAll(key, sval, dval, mtime));
}


//---------------------------------------------------------
// Procecure: handleMailKey()

void VCheckSet::handleMailKey(string key, string sval, double dval, double mtime)
{
  // If already triggered, take no further action
  if(m_start_time > 0)
    return;
  
  // Handle case where trigger is string
  if(m_start_trigger.is_string())
    if(m_start_trigger.get_sdata() == sval) 
      m_start_time = mtime;
  
  // Handle case where trigger is double
  if(!m_start_trigger.is_string())
    if(m_start_trigger.get_ddata() == dval) 
      m_start_time = mtime;
  
}
 
 
//---------------------------------------------------------
// Procecure: isEvaluated()
 
bool VCheckSet::isEvaluated() const
{
  for(unsigned int i=0; i<m_vchecks.size(); i++) {
    if(!m_vchecks[i].isEvaluated())
      return(false);
    else {
      if(m_fail_on_first && !m_vchecks[i].isSatisfied())
	return(true);
    }
  }
  return(true);
}
  

//---------------------------------------------------------
// Procecure: isSatisfied()
 
bool VCheckSet::isSatisfied() const
{
  for(unsigned int i=0; i<m_vchecks.size(); i++) {
    if(!m_vchecks[i].isSatisfied())
      return(false);
  }
  return(true);
}
  
//---------------------------------------------------------
// Procecure: getPassRate()
 
double VCheckSet::getPassRate() const
{
  double total_tests = (double)(m_total_passed + m_total_failed);
  if(total_tests == 0)
    return(0);

  return((double)(m_total_passed) / total_tests);
}
  

//---------------------------------------------------------
// Procecure: handleMailAll()

void VCheckSet::handleMailAll(string key, string sval, double dval, double mtime)
{
  if(m_vcheck_vars.count(key) == 0)
    return;
  if(m_start_time < 0)
    return;
  
  for(unsigned int i=0; i<m_vchecks.size(); i++) {
    if(m_vchecks[i].getVarName() != key)
      continue;
    if(m_vchecks[i].isEvaluated())
      continue;

    // Case A: If vcheck is a time_delta vcheck, we're checking to
    // see if the posting, defined by the vcheck, matches this
    // posting, and if it occurred within the time_delta specified
    // in this vcheck.
    double curr_elapsed = mtime - m_start_time;
    if(m_vchecks[i].getMaxTimeDelta() > 0) {
      // check 1: the values must match
      bool val_match = false;
      if(m_vchecks[i].getValString()!= "") {
	if(m_vchecks[i].getValString() == sval)
	  val_match = true;
      }
      else {
	if(m_vchecks[i].getValDouble() == dval)
	  val_match = true;
      }
      
      // check 2: the post time must fall in the time window
      bool time_match = false;
      double max_tdelta = m_vchecks[i].getMaxTimeDelta();
      double delta_time = curr_elapsed - m_vchecks[i].getTimeStamp();
      if(delta_time < 0)
	delta_time *= -1;
      if(delta_time < max_tdelta)
	time_match = true;
      
      if(val_match && time_match) {
	m_vchecks[i].setEvaluated(true);
	m_vchecks[i].setSatisfied(true);
	m_vchecks[i].setActualDelta(delta_time);
	m_vchecks[i].setActualTime(curr_elapsed);
	m_vchecks[i].setActualValDbl(dval);
	m_vchecks[i].setActualValStr(sval);
	m_total_passed++;
      }
    }
    // Case B: If vcheck is a value_delta vcheck, it is evaluated
    // as soon as a message has been posted, matching the varname,
    // and matching or later than the timestamp of the vcheck. It
    // is meant for variables like NAV_X or NAV_DEPTH which are
    // continuously posted throughout a mission.
    else if(m_vchecks[i].getMaxValDelta() > 0) {
      if(m_vchecks[i].getTimeStamp() <= curr_elapsed) {
	double val_delta = dval - m_vchecks[i].getValDouble();
	if(val_delta < 0)
	  val_delta *= -1;
	m_vchecks[i].setEvaluated(true);
	if(val_delta <= m_vchecks[i].getMaxValDelta()) {
	  m_vchecks[i].setSatisfied(true);
	  m_total_passed++;
	}
	else {
	  m_vchecks[i].setSatisfied(false);
	  m_total_failed++;
	  augFailReport(m_vchecks[i], dval, curr_elapsed);
	}
	
	m_vchecks[i].setActualDelta(val_delta);
	m_vchecks[i].setActualTime(curr_elapsed);
	m_vchecks[i].setActualValDbl(dval);
      }
    }
  }
}


//---------------------------------------------------------
// Procecure: update()
//   Purpose: This function examines all the time-based vchecks,
//            the variables that must post exactly some value
//            within a given time window. This function catches
//            the failures that result from timing-out, noting
//            that the time window has passed and the post did
//            not happen. 

void VCheckSet::update(double curr_time)
{
  // If time-based checks have not been triggered yet, just return
  if(m_start_time < 0)
    return;

  m_elapsed_time = curr_time - m_start_time;

  // Go through each of the vchecks and, for any that are based
  // on a max time delta, see if it has timed out. If so, mark.
  for(unsigned int i=0; i<m_vchecks.size(); i++) {
    if(m_vchecks[i].isEvaluated())
      continue;
    if(m_vchecks[i].getMaxTimeDelta() > 0) {
      double latest_allowed = m_vchecks[i].getTimeStamp();
      latest_allowed += m_vchecks[i].getMaxTimeDelta();

      if(m_elapsed_time > latest_allowed) {
	m_vchecks[i].setEvaluated(true);
	m_vchecks[i].setSatisfied(false);
	m_total_failed++;		  
	augFailReport(m_vchecks[i], -1, m_elapsed_time);
      }
    }
  }  
}
  


//---------------------------------------------------------
// Procecure: augFailReport()

void VCheckSet::augFailReport(string var, double eval, double etime,
			      double aval, double atime)
{
  if(m_fail_report != "")
    m_fail_report += " # ";

  m_fail_report += "var=" + var;
  m_fail_report += ", eval=" + doubleToStringX(eval,2);
  m_fail_report += ", etime=" + doubleToStringX(etime,2);
  m_fail_report += ", aval=" + doubleToStringX(aval,2);
  m_fail_report += ", atime=" + doubleToStringX(atime,2);
}

//---------------------------------------------------------
// Procecure: augFailReport()

void VCheckSet::augFailReport(VCheck vcheck, double aval, double atime)
{
  if(m_fail_report != "")
    m_fail_report += " # ";
  
  string eval = vcheck.getValString();
  if(eval == "")
    eval = doubleToStringX(vcheck.getValDouble(),2);
  string etime = doubleToStringX(vcheck.getTimeStamp(),3);
  
  m_fail_report += "var=" + vcheck.getVarName();
  m_fail_report += ", eval=" + eval;
  m_fail_report += ", etime=" + etime;
  m_fail_report += ", aval=" + doubleToStringX(aval,2);
  m_fail_report += ", atime=" + doubleToStringX(atime,2);
}

//---------------------------------------------------------
// Procecure: getReport()

list<string> VCheckSet::getReport() const
{
  list<string> rep;
  
  string vsize = uintToString(m_vchecks.size());
  string vtime = doubleToString(m_elapsed_time,1);
  string status = "(" + vsize + ")  [" + vtime + "]  ";
  status += getStatus();
  rep.push_back("VCheck Status: " + status);
  rep.push_back("============================================");
  ACTable actab(8);
  actab << "    | Targ | Targ | Real | Real | Targ  | Real  | ";
  actab << "Var | Val  | Time | Val  | Time | Delta | Delta | Status";
  actab.addHeaderLines();

  int overage = (int)(m_vchecks.size()) - (int)(m_max_report_size);
  unsigned int amt_out = 0;
  unsigned int hidden = 0;
  
  for(unsigned int i=0; i<m_vchecks.size(); i++) {

    if((overage > 0) && m_vchecks[i].isSatisfied()) {
      hidden++;
      overage--;
      continue;
    }
    amt_out++;
    
    bool  ceval   = m_vchecks[i].isEvaluated();
    bool  cstr    = (m_vchecks[i].getValString() != "");
    bool  vthresh = (m_vchecks[i].getMaxValDelta() > 0);

    string var_name = m_vchecks[i].getVarName();   // col 1
    string targ_val = m_vchecks[i].getValString(); // col 2
    if(targ_val == "")
      targ_val = doubleToStringX(m_vchecks[i].getValDouble());

    string targ_time = doubleToStringX(m_vchecks[i].getTimeStamp()); // col 3
    string real_val = "-";   // col 4
    if(ceval && cstr)
      real_val = m_vchecks[i].getActualValStr();
    if(ceval && !cstr)
      real_val = doubleToStringX(m_vchecks[i].getActualValDbl(),2);

    string real_time = doubleToStringX(m_vchecks[i].getActualTime(),2); // col 5

    string targ_delta = doubleToStringX(m_vchecks[i].getMaxTimeDelta(),2); // col 6
    if(vthresh) 
      targ_delta = doubleToStringX(m_vchecks[i].getMaxValDelta(),2);

    string real_delta = doubleToStringX(m_vchecks[i].getActualDelta(),2); // col 7
    
    string status = "pending";
    if(ceval) {
      if(m_vchecks[i].isSatisfied())
	status = "pass";
      else
	status = "fail";
    }

    if(vthresh) {
      targ_val = "[" + targ_val + "]";
      real_val = "[" + real_val + "]";
      targ_delta = "v:" + targ_delta;
    }
    else {
      targ_time = "[" + targ_time + "]";
      real_time = "[" + real_time + "]";
      targ_delta = "t:" + targ_delta;
    }

    actab << var_name << targ_val << targ_time << real_val << real_time <<
      targ_delta << real_delta << status;
    
    if(amt_out >= m_max_report_size) 
      break;
  }

  vector<string> lines = actab.getTableOutput();
  for(unsigned int i=0; i<lines.size(); i++)
    rep.push_back(lines[i]);

  if(hidden > 0) 
    rep.push_back("  --- " + intToString(hidden) + " additional checks PASSED, not shown ---");
  
  if(m_vchecks.size() > (hidden + amt_out)) {
    int remaining = m_vchecks.size() - (hidden + amt_out);
    rep.push_back("  --- " + intToString(remaining) + " additional checks pending, not shown ---");
  }

  return(rep);
}

