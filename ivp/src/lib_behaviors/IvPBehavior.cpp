/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IvPBehavior.cpp                                      */
/*    DATE: Oct 21, 2003 5 days after Grady's Gaffe              */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <iostream>
#include <vector>
#include <cstdlib>
#include "IvPBehavior.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BehaviorReport.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

IvPBehavior::IvPBehavior(IvPDomain g_domain)
{
  m_domain       = g_domain;
  m_info_buffer  = 0;
  m_priority_wt  = 100.0;  // Default Priority Weight
  m_descriptor   = "???";  // Default descriptor
  m_bhv_state_ok = true;
  m_completed    = false;
  m_good_updates = 0;
  m_bad_updates  = 0;
  m_perpetual    = false;
  m_filter_level = 0;

  m_last_runcheck_post = false;
  m_last_runcheck_time = 0;
  
  m_duration     = -1;
  m_duration_started         =  false;
  m_duration_start_time      = -1;
  m_duration_reset_timestamp = -1;
  m_duration_reset_pending   = false;
  m_duration_running_time    = 0;
  m_duration_idle_time       = 0;
  m_duration_prev_timestamp  = 0;
  m_duration_prev_state      = "";
  m_duration_idle_decay      = true;
  m_duration_reset_on_transition = false;

  m_helm_iter = 0;
}

//-----------------------------------------------------------
// Procedure: onRunState(string)

BehaviorReport IvPBehavior::onRunState(string)
{
  BehaviorReport empty_report;
  return(empty_report);
}

//-----------------------------------------------------------
// Procedure: setParamCommon

bool IvPBehavior::setParamCommon(string g_param, string g_val) 
{
  return(IvPBehavior::setParam(g_param, g_val));
}

//-----------------------------------------------------------
// Procedure: setBehaviorName(string)

bool IvPBehavior::setBehaviorName(string bhv_name)
{
  if(!isAlphaNum(bhv_name, "_-[]()#@+"))
    return(false);
  
  m_descriptor = bhv_name;
  m_status_info = "name=" + m_descriptor;
  return(true);
}


//-----------------------------------------------------------
// Procedure: augBehaviorName(string)

bool IvPBehavior::augBehaviorName(string aug_name)
{
  if(!isAlphaNum(aug_name, "_-[]()#@+"))
    return(false);
  
  m_descriptor += aug_name;
  m_status_info = "name=" + m_descriptor;
  return(true);
}

//-----------------------------------------------------------
// Procedure: setPriorityWt()

void IvPBehavior::setPriorityWt(double val)
{
  if(val < 0)
    val = 0;
  m_priority_wt = val;
}


//-----------------------------------------------------------
// Procedure: setParam

bool IvPBehavior::setParam(string g_param, string g_val) 
{
  g_val   = stripBlankEnds(g_val);
  g_param = tolower(g_param);

  // Allow parameters to be prefaced by the '_' and be treated
  // as if that character were not present.
  if(g_param[0] == '_')
    g_param = g_param.c_str()+1;

  if(g_param == "us") { 
    m_us_name = g_val;
    return(true);
  }
  else if((g_param == "pwt") || 
	  (g_param == "priwt") || 
	  (g_param == "priority")) {   // Alt way of setting priority
    double pwt = atof(g_val.c_str());
    if((pwt < 0) || (!isNumber(g_val)))
      return(false);
    m_priority_wt = pwt;
    return(true);
  }
  else if(g_param == "condition") {
    g_val = findReplace(g_val, ',', '=');
    bool ok = true;
    LogicCondition new_condition;
    ok = new_condition.setCondition(g_val);
    if(ok)
      m_logic_conditions.push_back(new_condition);
    return(ok);
  }
  else if(g_param == "duration_status") {
    m_duration_status = g_val;
    return(true);
  }
  else if(g_param == "duration_reset") {
    string var = biteStringX(g_val, '=');
    string val = g_val;
    if(var == "")
      return(false);
    m_duration_reset_var = var;
    m_duration_reset_val = val;
    return(true);
  }
  else if(g_param == "duration_idle_decay") {
    bool ok = setBooleanOnString(m_duration_idle_decay, g_val);
    return(ok);
  }
  else if(g_param == "post_mapping") {
    string left  = biteStringX(g_val, ',');
    string right = g_val;
    if(!strContainsWhite(left) && !strContainsWhite(right)) {
      m_remap_vars[left] = right;
      return(true);
    }
  }
  else if(g_param == "spawnflag") {
    string var = biteStringX(g_val, '=');
    string val = g_val;
    if(strContainsWhite(var) || (val == ""))
      return(false);
    VarDataPair pair(var, val, "auto");
    m_spawn_flags.push_back(pair);
    return(true);
  }
  else if(g_param == "runflag") {
    string var = biteStringX(g_val, '=');
    string val = g_val;
    if(strContainsWhite(var) || (val == ""))
      return(false);
    VarDataPair pair(var, val, "auto");
    m_run_flags.push_back(pair);
    return(true);
  }
  else if(g_param == "activeflag") {
    string var = biteStringX(g_val, '=');
    string val = g_val;
    if(strContainsWhite(var) || (val == ""))
      return(false);
    VarDataPair pair(var, val, "auto");
    m_active_flags.push_back(pair);
    return(true);
  }
  else if(g_param == "inactiveflag") {
    string var = biteStringX(g_val, '=');
    string val = g_val;
    if(strContainsWhite(var) || (val == ""))
      return(false);
    VarDataPair pair(var, val, "auto");
    m_inactive_flags.push_back(pair);
    return(true);
  }
  else if(g_param == "idleflag") {
    string var = biteStringX(g_val, '=');
    string val = g_val;
    if(strContainsWhite(var) || (val == ""))
      return(false);
    VarDataPair pair(var, val, "auto");
    m_idle_flags.push_back(pair);
    return(true);
  }
  else if(g_param == "endflag") {
    string var = biteStringX(g_val, '=');
    string val = g_val;
    if(strContainsWhite(var) || (val == ""))
      return(false);
    VarDataPair pair(var, val, "auto");
    m_end_flags.push_back(pair);
    return(true);
  }
  else if((g_param == "no_starve") || (g_param == "nostarve")) {
    vector<string> svector = parseString(g_val, ',');
    unsigned int i, vsize = svector.size();
    // must have at least one var,time pair.
    if(vsize < 2)
      return(false);
    double stime = atof(svector[vsize-1].c_str());
    if(stime <= 0)
      return(false);

    for(i=0; i<vsize-1; i++) {
      string var = stripBlankEnds(svector[i]);
      m_starve_vars[var] = stime;
    }
    return(true);
  }

  else if(g_param == "perpetual")  {
    string modval = tolower(g_val);
    m_perpetual = (modval == "true");
    return(true);
  }
  
  // Accept duration parameter (in seconds)
  else if(g_param == "duration") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      if((g_val != "no-time-limit") && (g_val != "-1"))
	return(false);
    
    if((g_val == "no-time-limit") || (g_val == "-1"))
      m_duration = -1;
    else
      m_duration = dval;
    return(true);
  }

  else if(g_param == "build_info") {
    g_val = tolower(g_val);
    if(g_val == "clear") {
      m_build_info = "";
      return(true);
    }
    // Add new info on to the *front* of the string
    if(m_build_info != "")
      m_build_info = " # " + m_build_info;
    m_build_info = g_val + m_build_info;
    return(true);
  }
  
  else if(g_param == "updates") {
    m_update_var = g_val;
    if(m_update_var != "")
      m_info_vars.push_back(g_val);
    return(true);
  }
  
  else if(g_param == "precision") {
    string precision_info = "precision=" + tolower(g_val);
    return(setParam("build_info", precision_info)); 
  }

  return(false);
}

//-----------------------------------------------------------
// Procedure: isRunnable()

string IvPBehavior::isRunnable()
{
#if 0
  if(!m_duration_started) {
    double curr_time = m_info_buffer->getCurrTime();
    m_duration_started = true;
    m_duration_start_time = curr_time;
  }
#endif

  if(m_completed)
    return("completed");

  if(!checkConditions()) {
    //if(m_duration_idle_decay && m_duration_reset_pending)
    //  durationReset();
    return("idle");
  }

  // Important that this be called after checkConditions so 
  // the duration clock doesn't start until the conditions
  // are met.
  if(m_duration_reset_pending)
    durationReset();
  if(durationExceeded()) {
    statusInfoAdd("pc", "completed-byduration");
    setComplete();
    if(m_perpetual) 
      m_duration_reset_pending = true;
    else
      return("completed");
  }
  
  if(m_domain.size() == 0) {
    postEMessage("Null IvPDomain given to Behavior");
    statusInfoAdd("pc", "null-domain");
    return("idle");
  }
  
  if(!checkNoStarve())
    return("idle");
  
  return("running");
}
  
//-----------------------------------------------------------
// Procedure: setInfoBuffer

void IvPBehavior::setInfoBuffer(const InfoBuffer *ib)
{
  m_info_buffer = ib;
}

//-----------------------------------------------------------
// Procedure: postMessage
//     Notes: If the key is set to be "repeatable" then in effect 
//            there is no key is associated with this variable-value 
//            pair and it will NOT be filtered.

void IvPBehavior::postMessage(string var, string sdata, string key)
{
  // First check if posted var has been remapped to another variable.
  map<string,string>::iterator p;
  p = m_remap_vars.find(var);
  if(p != m_remap_vars.end()) {
    var = p->second;
    // If the variable has been mapped to "silent", this indicates
    // the posting is not to be made at all.
    if(tolower(var) == "silent")
      return;
  }
  VarDataPair pair(var, sdata);

  if(tolower(key) != "repeatable") {
    key = (m_descriptor + var + key);
    pair.set_key(key);
  }

  m_messages.push_back(pair);
}

//-----------------------------------------------------------
// Procedure: noteLastRunCheck()

void IvPBehavior::noteLastRunCheck(bool post, double timestamp)
{
  m_last_runcheck_post = post;
  m_last_runcheck_time = timestamp;
}

//-----------------------------------------------------------
// Procedure: postMessage
//     Notes: If the key is set to be "repeatable" then in effect 
//            there is no key is associated with this variable-value 
//            pair and it will NOT be filtered.

void IvPBehavior::postMessage(string var, double ddata, string key)
{
  // First check if posted var has been remapped to another variable.
  map<string,string>::iterator p;
  p = m_remap_vars.find(var);
  if(p != m_remap_vars.end())  {
    var = p->second;
    // If the variable has been mapped to "silent", this indicates
    // the posting is not to be made at all.
    if(tolower(var) == "silent")
      return;
  }
  
  VarDataPair pair(var, ddata);
  
  if(tolower(key) != "repeatable") {
    key = (m_descriptor + var + key);
    pair.set_key(key);
  }
  m_messages.push_back(pair);
}


//-----------------------------------------------------------
// Procedure: postBoolMessage

void IvPBehavior::postBoolMessage(string var, bool bdata, string key)
{
  if(bdata)
    postMessage(var, "true", key);
  else
    postMessage(var, "false", key);
}

//-----------------------------------------------------------
// Procedure: postIntMessage
//      Note: A convenience method for posting the double data rounded
//            to the nearest integer. This may reduce the number of 
//            posts out to the DB, since the posts are done by default
//            on change detection. For doubles representing for example
//            meters, seconds, or heading, this function is useful.

void IvPBehavior::postIntMessage(string var, double ddata, string key)
{
  int idata = (int)(ddata + 0.5);
  postMessage(var, (double)(idata), key);
}

//-----------------------------------------------------------
// Procedure: postRepeatableMessage

void IvPBehavior::postRepeatableMessage(string var, string sdata)
{
  postMessage(var, sdata, "repeatable");
}

//-----------------------------------------------------------
// Procedure: postRepeatableMessage

void IvPBehavior::postRepeatableMessage(string var, double ddata)
{
  postMessage(var, ddata, "repeatable");
}

//-----------------------------------------------------------
// Procedure: setComplete

void IvPBehavior::setComplete()
{
  postFlags("endflags");
  // Removed by mikerb jun2213 to prevent double posting
  // postFlags("inactiveflags");  
  if(!m_perpetual)
    m_completed = true;
}


//-----------------------------------------------------------
// Procedure: postEMessage

void IvPBehavior::postEMessage(string g_emsg)
{
  if(m_descriptor != "")
    g_emsg = (m_descriptor + ": " + g_emsg);

  postMessage("BHV_ERROR", g_emsg, "repeatable");
  m_bhv_state_ok = false;
}

//-----------------------------------------------------------
// Procedure: postBadConfig

void IvPBehavior::postBadConfig(string message)
{
  if(m_descriptor != "")
    message = (m_descriptor + ": " + message);

  postMessage("BHV_BAD_CONFIG", message, "repeatable");
}


//-----------------------------------------------------------
// Procedure: postWMessage

void IvPBehavior::postWMessage(string g_msg)
{
  if(g_msg == "")
    return;

  if(m_descriptor != "")
    g_msg = (m_descriptor + ": " + g_msg);
  
  postMessage("BHV_WARNING", g_msg, "repeatable");
}

//-----------------------------------------------------------
// Procedure: postRetractWMessage

void IvPBehavior::postRetractWMessage(string g_msg)
{
  if(g_msg == "")
    return;

  if(m_descriptor != "")
    g_msg = (m_descriptor + ": " + g_msg);
  
  postMessage("BHV_WARNING", g_msg, "retract");
}

//-----------------------------------------------------------
// Procedure: statusInfoAdd

void IvPBehavior::statusInfoAdd(string param, string value)
{
  m_status_info += ("," + param + "=" + value);
}

//-----------------------------------------------------------
// Procedure: statusInfoPost()

void IvPBehavior::statusInfoPost()
{
  statusInfoAdd("updates", m_update_summary);
  postMessage("BHV_STATUS", m_status_info, m_descriptor);
  
  // Reset the status_info contents after each post.
  m_status_info = "name=" + m_descriptor;
}

//-----------------------------------------------------------
// Procedure: checkConditions()

bool IvPBehavior::checkConditions()
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
    if(!satisfied) {
      string failed_condition = m_logic_conditions[i].getRawCondition();
      statusInfoAdd("pc", failed_condition);
      return(false);
    }
  }
  return(true);

}


//-----------------------------------------------------------
// Procedure: checkForDurationReset()

bool IvPBehavior::checkForDurationReset()
{
  if(!m_info_buffer) 
    return(false);

  // Phase 1: get the value of the duration_reset_var from info_buffer
  string varname = m_duration_reset_var;
  bool   ok_s, ok_d;
  string s_result = m_info_buffer->sQuery(varname, ok_s);
  double d_result = m_info_buffer->dQuery(varname, ok_d);

  bool reset_triggered = false;
  //if(m_duration_reset_val == "")
  //  reset_triggered = true;
  if(ok_s && (m_duration_reset_val == s_result))
    reset_triggered = true;
  if(ok_d && (atof(m_duration_reset_val.c_str()) == d_result))
    reset_triggered = true;

  if(!reset_triggered)
    return(false);

  // Get the absolute, not relative, timestamp from the info_buffer
  double curr_reset_timestamp = m_info_buffer->tQuery(varname, false);

  if((m_duration_reset_timestamp == -1) || 
     (curr_reset_timestamp > m_duration_reset_timestamp)) {
    // Note the timestamp so we wont reset unless a fresh post detected.
    m_duration_reset_timestamp = curr_reset_timestamp;
    durationReset();
    return(true);
  }
  else
    return(false);
}



//-----------------------------------------------------------
// Procedure: durationReset()

void IvPBehavior::durationReset()
{
  m_duration_reset_pending = false;
  m_duration_started       = false;
  m_duration_start_time    = -1;
  m_duration_idle_time     = 0;
  m_duration_running_time  = 0;
  if(m_duration_status != "")
    postMessage(m_duration_status, m_duration);
}

//-----------------------------------------------------------
// Procedure: addInfoVars
//      Note: If post_warning is false, then the 

void IvPBehavior::addInfoVars(string var_string, string warning)
{
  bool no_warning = false;
  warning = tolower(warning);
  if((warning == "nowarning") || (warning == "no_warning"))
    no_warning = true;

  vector<string> svector = parseString(var_string, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string varname = stripBlankEnds(svector[i]);
    if(!vectorContains(m_info_vars, varname))
      m_info_vars.push_back(varname);
    if(!vectorContains(m_info_vars_no_warning, varname) && no_warning)
      m_info_vars_no_warning.push_back(varname);
  }
}

//-----------------------------------------------------------
// Procedure: getInfoVars()

vector<string> IvPBehavior::getInfoVars()
{
  unsigned int i;
  vector<string> svector;
  for(i=0; i<m_logic_conditions.size(); i++)
    svector = mergeVectors(svector, m_logic_conditions[i].getVarNames());

  if(m_duration_reset_var != "")
    svector.push_back(m_duration_reset_var);

  svector = mergeVectors(svector, m_info_vars);
  svector = removeDuplicates(svector);

  return(svector);
}

//-----------------------------------------------------------
// Procedure: checkNoStarve()
//     Notes: Check all "starve_vars" against the info_buffer
//              to ensure no starve conditions.
//            starve_vars is a map: var_name to time.

bool IvPBehavior::checkNoStarve()
{
  if(!m_info_buffer) 
    return(false);

  map<string,double>::const_iterator p;
  for(p=m_starve_vars.begin(); p!=m_starve_vars.end(); p++) {
    string var   = p->first;
    double stime = p->second;
    double itime = m_info_buffer->tQuery(var);

    if((itime == -1) || (itime > stime)) {
      string msg = "VarStarve: " + var + "(stime:" + doubleToString(stime);
      msg += ") (itime:" + doubleToString(itime) + ")";
      statusInfoAdd("pc", msg);
      return(false);
    }
  }
  return(true);
}


//-----------------------------------------------------------
// Procedure: checkUpdates

// (1) We want to be efficient and avoid applying an update string 
//     if it hasn't change since the last application - in case some
//     process talking to the helm is inadvertently repeating the 
//     same requested update over and over. So we store the value
//     of the previous update string in m_curr_update_str, and only
//     apply the current string if it is different.
//
// (2) We want to keep track of successful and failed updates, and
//     increment m_bad_updates and m_good_updates accordingly.
//
// (3) A write to the update_var with an empty string is just ignored.

bool IvPBehavior::checkUpdates()
{
  if(m_update_var == "") {
    m_update_summary = "n/a";
    return(false);
  }

  m_update_results.clear();
  
  bool ok;
  vector<string> new_update_strs = getBufferStringVector(m_update_var, ok);
  
  bool update_made = false;
  unsigned int i, vsize = new_update_strs.size();
  for(i=0; i<vsize; i++) {
    string new_update_str = new_update_strs[i];

    string update_result = "bhv=" + getDescriptor();
    update_result += ",var=" + m_update_var;
    update_result += ",time=" + doubleToString(getBufferLocalTime(),2);

    // Added Mar 7th 2014, allow successive dupl updates with word toggle in it.
    if(strContains(tolower(new_update_str), "toggle") ||
       ((new_update_str != "") && (new_update_str != m_prev_update_str))) {
    
      vector<string> uvector = parseString(new_update_str, '#');
      unsigned int j, usize = uvector.size();
      
      // First make sure that, if there is a behavior name specified, 
      // it must match the name of this behavior. Note that a name
      // mismatch is not considered failed update nor is it considered
      // a successful update. If no name parameter provided, this is
      // not considered a mismatch.
      bool name_mismatch = false;
      for(j=0; j<usize; j++) {
	string pair  = uvector[j];
	string param = biteStringX(pair, '=');
	string value = pair;
	if((param=="name") && (value!=m_descriptor))
	  name_mismatch = true;
      }

      if(name_mismatch)
	update_result += ",result=name_mismatch";
      else {
    	string bad_params;
	bool ok_params = true;
	for(j=0; j<usize; j++) {
	  string pair  = uvector[j];
	  string param = biteStringX(pair, '=');
	  string value = pair;
	  bool  result = true;
	  // Aug 28, 2015, Don't try to update "name" parameter. We don't want to
	  // since "name" should only be set at creation time. We want to totally
	  // ignore it since we dont want it flagged as an unhandled update.
	  if(param != "name") { 
	    result = setParam(param, value);
	    if(!result)
	      result = IvPBehavior::setParam(param, value);
	  }
	  if(!result) {
	    ok_params = false;
	    if(bad_params != "")
	      bad_params += ",";
	    bad_params += param;
	  }
	}
	
	if(!ok_params) {
	  m_bad_updates++;
	  string wmsg = "Faulty update for behavior: " + m_descriptor;
	  wmsg += (". Bad parameter(s): " + bad_params);
	  postMessage("BHV_WARNING", wmsg);
	  postMessage("BHV_CONFIG_WARNING", wmsg);
	  update_result += ",result=param_error";
	}
	else {
	  update_made = true;
	  m_good_updates++;
	  m_prev_update_str = new_update_str;
	  update_result += ",result=success";
	}
      }      
    }
    else {
      update_result += ",result=duplicate";
    }
    
    update_result += ",val=" + new_update_str;
    m_update_results.push_back(update_result);

  }

  if(update_made)
    postConfigStatus();

  m_update_summary  = uintToString(m_good_updates) + "/";
  m_update_summary += uintToString(m_good_updates + m_bad_updates);
  return(update_made);
}

//-----------------------------------------------------------
// Procedure: durationExceeded()
//   Purpose: Check to see if the duration time limit has been 
//            exceeded. If so, simply return true. No action is
//            taken other than to possibly post the time remaining
//            if the duration_status parameter is set.

bool IvPBehavior::durationExceeded()
{
  // If a duration was never specified, just return false
  if(m_duration == -1)
    return(false);

  double curr_time = m_info_buffer->getCurrTime();
  double elapsed_time = 0;
  // If we're starting the duration clock we don't give credit for 
  // time spent in the idle state.
  if(!m_duration_started) {
    m_duration_started = true;
    m_duration_start_time = curr_time;
    m_duration_idle_time = 0;
  }
  else
    elapsed_time = (curr_time - m_duration_start_time);

  // If time spent in the idle state is not counted toward the 
  // duration timer, ADD it back here.
  if(!m_duration_idle_decay)
    elapsed_time -= m_duration_idle_time;
  
  double remaining_time = m_duration - elapsed_time;

#if 0
  string msg1 = "ELAPSED_" + toupper(m_descriptor);
  postMessage(msg1, elapsed_time);

  string msg2 = "DUR_IDLE_TIME_" + toupper(m_descriptor);
  postMessage(msg2, m_duration_idle_time);

  string msg3 = "REMAINING_TIME_" + toupper(m_descriptor);
  postMessage(msg3, remaining_time);
#endif

  if(remaining_time < 0)
    remaining_time = 0;
  if(m_duration_status != "") {
    if(remaining_time <= 10)
      postMessage(m_duration_status, remaining_time);
    else
      postIntMessage(m_duration_status, remaining_time);
  }

  if(elapsed_time >= m_duration)
    return(true);

  return(false);
}


//-----------------------------------------------------------
// Procedure: postDurationStatus
//      Note: 

void IvPBehavior::postDurationStatus()
{
  double elapsed_time = 0;
  if(m_duration_started) {
    double curr_time = m_info_buffer->getCurrTime();
    elapsed_time = (curr_time - m_duration_start_time);
  }
  
  double remaining_time = m_duration - elapsed_time;

  // If time spent in the idle state is not counted toward the 
  // duration timer, ADD it back here.
  if(!m_duration_idle_decay)
    remaining_time += m_duration_idle_time;
  
  if(remaining_time < 0)
    remaining_time = 0;
  if(m_duration_status != "") {
    if(remaining_time <= 10)
      postMessage(m_duration_status, remaining_time);
    else
      postIntMessage(m_duration_status, remaining_time);
  }
}



//-----------------------------------------------------------
// Procedure: updateStateDurations
//      Note: Update the two variables representing how long
//            the behavior has been in the "idle" state and 
//            how long in the "running" state.

void IvPBehavior::updateStateDurations(string bhv_state) 
{
  double curr_time = m_info_buffer->getCurrTime();

  if(m_duration_prev_state == "") {
    m_duration_prev_state     = bhv_state;
    m_duration_prev_timestamp = curr_time;
    return;
  }

  if(bhv_state == "idle") {
    double delta_time = curr_time - m_duration_prev_timestamp;
    m_duration_idle_time += delta_time;
  }
  else if(bhv_state == "running") {
    double delta_time = curr_time - m_duration_prev_timestamp;
    m_duration_running_time += delta_time;
  }
  
  m_duration_prev_timestamp = curr_time;
  m_duration_prev_state = bhv_state;
}

//-----------------------------------------------------------
// Procedure: postFlags()
//     Notes: The repeat argument indicates that the posting should
//            be made as postRepeatable. This means the helm's
//            duplication filter will let it through absolutely.

void IvPBehavior::postFlags(const string& str, bool repeatable)
{
  vector<VarDataPair> flags;
  if(str == "runflags")
    flags = m_run_flags;
  else if(str == "endflags")
    flags = m_end_flags;
  else if(str == "idleflags")
    flags = m_idle_flags;
  else if(str == "activeflags")
    flags = m_active_flags;
  else if(str == "inactiveflags")
    flags = m_inactive_flags;
  else if(str == "spawnflags")
    flags = m_spawn_flags;
  
  // The endflags are treated as a special case in that they are 
  // posted as "repeatable" - that is they will be posted to the 
  // MOOSDB regardless of whether the "outgoing" cache of postings
  // maintained by the helm indicates that the posting is the same
  // as the previous posting to that MOOS variable. 
  bool endflags = (str == "endflags");

  unsigned int i, vsize = flags.size();
  for(i=0; i<vsize; i++) {
    string var = flags[i].get_var();
    
    if(flags[i].is_string()) {
      string sdata = flags[i].get_sdata();
      sdata = findReplace(sdata, "$[OWNSHIP]", m_us_name);
      sdata = findReplace(sdata, "$[BHVNAME]", m_descriptor);
      sdata = findReplace(sdata, "$[BHVTYPE]", m_behavior_type);
      sdata = findReplace(sdata, "$[CONTACT]", m_contact);
      if(endflags || repeatable) 
	postRepeatableMessage(var, sdata);
      else
	postMessage(var, sdata);
    }
    else {
      double ddata = flags[i].get_ddata();
      if(endflags || repeatable)
	postRepeatableMessage(var, ddata);
      else
	postMessage(var, ddata);
    }	
  }    
}


//-----------------------------------------------------------
// Procedure: getBufferCurrTime()

double IvPBehavior::getBufferCurrTime()
{
  if(!m_info_buffer)
    return(0);
  return(m_info_buffer->getCurrTime());
}


//-----------------------------------------------------------
// Procedure: getBufferLocalTime()

double IvPBehavior::getBufferLocalTime()
{
  if(!m_info_buffer)
    return(0);
  return(m_info_buffer->getLocalTime());
}


//-----------------------------------------------------------
// Procedure: getBufferTimeVal()
//   Purpose: Return the amount of time since this variable was last
//            updated in the info buffer
//   Returns: 0 if for some reason the info buffer is null
//            -1 if the info buffer knows nothing about this variable.
//            N otherwise the time since last updated.
//      Note: If updated on the current helm iteration, will be zero.

double IvPBehavior::getBufferTimeVal(string varname)
{
  if(!m_info_buffer)
    return(0);
  return(m_info_buffer->tQuery(varname));
}

//-----------------------------------------------------------
// Procedure: getBufferMsgTimeVal()

double IvPBehavior::getBufferMsgTimeVal(string varname)
{
  if(!m_info_buffer)
    return(0);
  return(m_info_buffer->mtQuery(varname));
}

//-----------------------------------------------------------
// Procedure: getBufferDoubleVal()

double IvPBehavior::getBufferDoubleVal(string varname, bool& ok)
{
  if(!m_info_buffer) {
    ok = false;
    return(0);
  }

  double value = m_info_buffer->dQuery(varname, ok);
  if(!ok) {
    bool result;
    string sval = m_info_buffer->sQuery(varname, result);
    if(result && isNumber(sval)) {
      value = atof(sval.c_str());
      ok = true;
    }
  }
  if((!ok) && !vectorContains(m_info_vars_no_warning, varname)) 
    postWMessage(varname + " dbl info not found in helm info_buffer");
  return(value);
}

//-----------------------------------------------------------
// Procedure: getBufferStringVal()

string IvPBehavior::getBufferStringVal(string varname, bool& ok)
{
  if(!m_info_buffer) {
    ok = false;
    return("");
  }

  string value = m_info_buffer->sQuery(varname, ok);
  if(!ok) {
    bool result;
    double dval = m_info_buffer->dQuery(varname, result);
    if(result) {
      value = doubleToString(dval, 6);
      ok = true;
    }
  }
  if((!ok) && !vectorContains(m_info_vars_no_warning, varname)) 
    postWMessage(varname + " str info not found in helm info_buffer");
  return(value);
}


//-----------------------------------------------------------
// Procedure: getBufferStringVal()
//      Note: Same as the other getBufferStringVal but we don't bother
//            returing a status indicator.

string IvPBehavior::getBufferStringVal(string varname)
{
  if(!m_info_buffer) 
    return("");

  bool ok = false;
  string value = m_info_buffer->sQuery(varname, ok);
  if(!ok) {
    bool result;
    double dval = m_info_buffer->dQuery(varname, result);
    if(result) {
      value = doubleToString(dval, 6);
      ok = true;
    }
  }
  if((!ok) && !vectorContains(m_info_vars_no_warning, varname)) 
    postWMessage(varname + " info not found in helm info_buffer");
  return(value);
}


//-----------------------------------------------------------
// Procedure: getBufferDoubleVector()

vector<double> IvPBehavior::getBufferDoubleVector(string varname, bool& ok)
{
  vector<double> empty_vector;
  if(!m_info_buffer) {
    ok = false;
    return(empty_vector);
  }
  return(m_info_buffer->dQueryDeltas(varname, ok));
}

//-----------------------------------------------------------
// Procedure: getBufferStringVector()

vector<string> IvPBehavior::getBufferStringVector(string varname, bool& ok)
{
  vector<string> empty_vector;
  if(!m_info_buffer) {
    ok = false;
    return(empty_vector);
  }
  return(m_info_buffer->sQueryDeltas(varname, ok));
}


//-----------------------------------------------------------
// Procedure: getStateSpaceVars
//   Purpose: Get a vector of all the variables involved in the 
//            state space of a behavior. This includes conditions, 
//            idle_flags, end_flags and run_flags.

vector<string> IvPBehavior::getStateSpaceVars()
{
  vector<string> rvector;

  int i, vsize = m_logic_conditions.size();
  for(i=0; i<vsize; i++) 
    rvector = mergeVectors(rvector, m_logic_conditions[i].getVarNames());

  vsize = m_run_flags.size();
  for(i=0; i<vsize; i++)
    rvector.push_back(m_run_flags[i].get_var());
  
  vsize = m_idle_flags.size();
  for(i=0; i<vsize; i++)
    rvector.push_back(m_idle_flags[i].get_var());
  
  vsize = m_end_flags.size();
  for(i=0; i<vsize; i++)
    rvector.push_back(m_end_flags[i].get_var());

  vsize = m_active_flags.size();
  for(i=0; i<vsize; i++)
    rvector.push_back(m_active_flags[i].get_var());
  
  vsize = m_inactive_flags.size();
  for(i=0; i<vsize; i++)
    rvector.push_back(m_inactive_flags[i].get_var());
  
  return(rvector);
}
