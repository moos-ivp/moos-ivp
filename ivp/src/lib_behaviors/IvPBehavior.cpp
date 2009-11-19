/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IvPBehavior.cpp                                      */
/*    DATE: Oct 21, 2003 5 days after Grady's Gaffe              */
/*                                                               */
/* (IvPHelm) The IvP autonomous control Helm is a set of         */
/* classes and algorithms for a behavior-based autonomous        */
/* control architecture with IvP action selection.               */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <vector>
#include <stdlib.h>
#include "IvPBehavior.h"
#include "MBUtils.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

IvPBehavior::IvPBehavior(IvPDomain g_domain)
{
  m_domain       = g_domain;
  m_info_buffer  = 0;
  m_priority_wt  = 100.0;  // Default Priority Weight
  m_descriptor   = "???";  // Default descriptor
  m_state_ok     = true;
  m_completed    = false;
  m_good_updates = 0;
  m_bad_updates  = 0;
  m_perpetual    = false;
  m_filter_level = 0;

  m_duration     = -1;
  m_duration_started         =  false;
  m_duration_start_time      = -1;
  m_duration_reset_timestamp = -1;
  m_duration_running_time    = 0;
  m_duration_idle_time       = 0;
  m_duration_prev_timestamp  = 0;
  m_duration_prev_state      = "";
  m_duration_idle_decay      = true;
  m_duration_reset_on_transition = false;
}
  
//-----------------------------------------------------------
// Procedure: setParamCommon

bool IvPBehavior::setParamCommon(string g_param, string g_val) 
{
  return(IvPBehavior::setParam(g_param, g_val));
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
  else if((g_param == "name") || (g_param == "descriptor")) {
    m_descriptor = g_val;
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
    string var = stripBlankEnds(biteString(g_val, '='));
    string val = stripBlankEnds(g_val);
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
    string left  = stripBlankEnds(biteString(g_val, ','));
    string right = stripBlankEnds(g_val);
    if(!strContainsWhite(left) && !strContainsWhite(right)) {
      m_remap_vars[left] = right;
      return(true);
    }
  }
  else if(g_param == "runflag") {
    vector<string> svector = parseString(g_val, '=');
    if(svector.size() != 2)
      return(false);
    string var = stripBlankEnds(svector[0]);
    string val = stripBlankEnds(svector[1]);
    VarDataPair pair(var, val, "auto");
    m_run_flags.push_back(pair);
    return(true);
  }
  else if(g_param == "activeflag") {
    vector<string> svector = parseString(g_val, '=');
    if(svector.size() != 2)
      return(false);
    string var = stripBlankEnds(svector[0]);
    string val = stripBlankEnds(svector[1]);
    VarDataPair pair(var, val, "auto");
    m_active_flags.push_back(pair);
    return(true);
  }
  else if(g_param == "inactiveflag") {
    vector<string> svector = parseString(g_val, '=');
    if(svector.size() != 2)
      return(false);
    string var = stripBlankEnds(svector[0]);
    string val = stripBlankEnds(svector[1]);
    VarDataPair pair(var, val, "auto");
    m_inactive_flags.push_back(pair);
    return(true);
  }
  else if(g_param == "idleflag") {
    g_val = findReplace(g_val, ',', '=');
    vector<string> svector = parseString(g_val, '=');
    if(svector.size() != 2)
      return(false);
    string var = stripBlankEnds(svector[0]);
    string val = stripBlankEnds(svector[1]);
    VarDataPair pair(var, val, "auto");
    m_idle_flags.push_back(pair);
    return(true);
  }
  else if(g_param == "endflag") {
    g_val = findReplace(g_val, ',', '=');
    vector<string> svector = parseString(g_val, '=');
    if(svector.size() != 2)
      return(false);
    string var = stripBlankEnds(svector[0]);
    string val = stripBlankEnds(svector[1]);
    VarDataPair pair(var, val, "auto");
    m_end_flags.push_back(pair);
    return(true);
  }
  else if((g_param == "no_starve") || (g_param == "nostarve")) {
    vector<string> svector = parseString(g_val, ',');
    int vsize = svector.size();
    // must have at least one var,time pair.
    if(vsize < 2)
      return(false);
    double stime = atof(svector[vsize-1].c_str());
    if(stime <= 0)
      return(false);

    for(int i=0; i<vsize-1; i++) {
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
// Procedure: isCompleted()

bool IvPBehavior::isCompleted()
{
  // If completed indicat that the pre-check failed due to this
  // reason and simply return false. End-flags are posted elsewhere
  if(m_completed) {
    postPCMessage(" -- completed -- ");
    return(true);
  }
  return(false);
}

//-----------------------------------------------------------
// Procedure: isRunnable()

bool IvPBehavior::isRunnable()
{
  if(!checkConditions())
    return(false);

  // Important that this be called after checkConditions so 
  // the duration clock doesn't start until the conditions
  // are met.
  if(durationExceeded()) {
    postPCMessage(" -- completed (duration exceeded) -- ");
    setComplete();
    if(m_perpetual)
      durationReset();
    if(!m_perpetual)
      return(false);
  }
  
  if(m_domain.size() == 0) {
    m_state_ok = false;
    postEMessage("Null IvPDomain given to Behavior");
    postPCMessage("----- Null IvPDomain ----");
    return(false);
  }
  
  if(!checkNoStarve()) {
    //m_state_ok = false;
    postPCMessage("----- STARVED VARIABLE ----");
    return(false);
  }
  
  postPCMessage(" -- ok -- ");
  return(true);
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

  if(key != "repeatable") {
    key = (m_descriptor + var + key);
    pair.set_key(key);
  }

  m_messages.push_back(pair);
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
  
  if(key != "repeatable") {
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
  postFlags("inactiveflags");
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
  m_state_ok = false;
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
// Procedure: postVMessage

void IvPBehavior::postVMessage(string g_msg)
{
  postMessage("PRIVATE_INFO", g_msg);
}


//-----------------------------------------------------------
// Procedure: postPCMessage

void IvPBehavior::postPCMessage(string g_msg)
{
  string mvar = "PC_" + m_descriptor;
  string mval = g_msg;
  
  postMessage(mvar, mval);
}


//-----------------------------------------------------------
// Procedure: checkConditions()

bool IvPBehavior::checkConditions()
{
  if(!m_info_buffer) 
    return(false);

  int i, j, vsize, csize;

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
      postPCMessage(failed_condition);
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

  int i, j, vsize, csize;

  // Phase 1: get the value of the duration_reset_var from info_buffer
  string varname = m_duration_reset_var;
  bool   ok_s, ok_d;
  string s_result = m_info_buffer->sQuery(varname, ok_s);
  double d_result = m_info_buffer->dQuery(varname, ok_d);

  bool reset_triggered = false;
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
  m_duration_started      = false;
  m_duration_start_time   = -1;
  m_duration_idle_time    = 0;
  m_duration_running_time = 0;
  if(m_duration_status != "")
    postMessage(m_duration_status, m_duration);
}

//-----------------------------------------------------------
// Procedure: addInfoVars

void IvPBehavior::addInfoVars(string var_string)
{
  vector<string> svector = parseString(var_string, ',');
  int vsize = svector.size();
  for(int i=0; i<vsize; i++) {
    string varname = stripBlankEnds(svector[i]);
    if(!vectorContains(m_info_vars, varname))
      m_info_vars.push_back(varname);
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
// Procedure: getUpdateSummary()

string IvPBehavior::getUpdateSummary()
{
  string rval;
  if(m_update_var == "")
    rval = "n/a";
  else {
    rval = intToString(m_good_updates);
    rval += "/";
    rval += intToString(m_good_updates + m_bad_updates);
  }
  return(rval);
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
      postPCMessage(msg);
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
  if(m_update_var == "")
    return(false);

  bool ok;
  vector<string> new_update_strs = getBufferStringVector(m_update_var, ok);
  
  bool update_made = false;
  int vsize = new_update_strs.size();
  for(int i=0; i<vsize; i++) {
    string new_update_str = new_update_strs[i];
    
    if((new_update_str != "") && (new_update_str != m_prev_update_str)) {
    
      vector<string> uvector = parseString(new_update_str, '#');
      int usize = uvector.size();
    
      string bad_params;
      ok = true;
      for(int j=0; j<usize; j++) {
	string an_update_str = uvector[j];
	vector<string> pvector = chompString(an_update_str, '=');
	if(pvector.size() == 2) {
	  string param = stripBlankEnds(pvector[0]);
	  string value = stripBlankEnds(pvector[1]);
	  bool  result = setParam(param, value);
	  if(!result)
	    result = IvPBehavior::setParam(param, value);
	  if(!result) {
	    ok = false;
	    if(bad_params != "")
	      bad_params += ",";
	    bad_params += param;
	  }
	}
	else 
	  ok = false;
      }
      
      if(!ok) {
	m_bad_updates++;
	string wmsg = "Faulty update for behavior: " + m_descriptor;
	wmsg += (". Bad parameter(s): " + bad_params);
	postMessage("BHV_WARNING", wmsg);
      }
      else {
	update_made = true;
	m_good_updates++;
	m_prev_update_str = new_update_str;
      }
    }
  }

  if((m_good_updates + m_bad_updates) > 0) {
    string varname = "UH_" + m_descriptor;
    string gstr = intToString(m_good_updates) + " update(s), and ";
    string bstr = intToString(m_bad_updates) + " failure(s)";
    postMessage(varname, gstr+bstr);
  }
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

  if(!m_duration_started) {
    m_duration_started = true;
    m_duration_start_time = curr_time;
  }

  double elapsed_time = (curr_time - m_duration_start_time);

  // If time spent in the idle state is not counted toward the 
  // duration timer, ADD it back here.
  if(!m_duration_idle_decay)
    elapsed_time -= m_duration_idle_time;
  
#if 0
  double remaining_time = m_duration - elapsed_time;


  if(remaining_time < 0)
    remaining_time = 0;
  if(m_duration_status != "") {
    if(remaining_time <= 10)
      postMessage(m_duration_status, remaining_time);
    else
      postIntMessage(m_duration_status, remaining_time);
  }
#endif

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

void IvPBehavior::postFlags(const string& str)
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

  // The endflags are treated as a special case in that they are 
  // posted as "repeatable" - that is they will be posted to the 
  // MOOSDB regardless of whether the "outgoing" cache of postings
  // maintained by the helm indicates that the posting is the same
  // as the previous posting to that MOOS variable. 
  bool endflags = (str == "endflags");

  int vsize = flags.size();
  for(int i=0; i<vsize; i++) {
    string var   = flags[i].get_var();
    string sdata = flags[i].get_sdata();
    double ddata = flags[i].get_ddata();
    //if(sdata != "") {
    if(flags[i].is_string()) {
      if(endflags) 
	postRepeatableMessage(var, sdata);
      else
	postMessage(var, sdata);
    }
    else {
      if(endflags)
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
// Procedure: getBufferTimeVal()

double IvPBehavior::getBufferTimeVal(string varname)
{
  if(!m_info_buffer)
    return(0);
  return(m_info_buffer->tQuery(varname));
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
  if(!ok) 
    postWMessage(varname+" info not found in helm info_buffer");
  return(value);
}

//-----------------------------------------------------------
// Procedure: getBufferStringVal()

string IvPBehavior::getBufferStringVal(string varname, bool& ok)
{
  if(!m_info_buffer) {
    ok = false;
    return(0);
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
  if(!ok) 
    postWMessage(varname+" info not found in helm info_buffer");
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




