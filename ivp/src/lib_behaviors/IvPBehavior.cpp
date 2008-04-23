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
#include "IvPBehavior.h"
#include "MBUtils.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

IvPBehavior::IvPBehavior(IvPDomain g_domain)
{
  m_domain          = g_domain;
  m_info_buffer     = 0;
  m_priority_wt     = 100.0;  // Default Priority Weight
  m_descriptor      = "???";  // Default descriptor
  m_state_ok        = true;
  m_started         =  false;
  m_start_time      = -1;
  m_last_update_age = -1;
  m_duration        = -1;
  m_completed       = false;
  m_good_updates    = 0;
  m_bad_updates     = 0;
  m_perpetual       = false;
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
  else if(g_param == "duration_status") {
    m_duration_status = g_val;
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
    g_val = findReplace(g_val, "==", "=");
    bool ok = true;
    LogicCondition new_condition;
    ok = new_condition.setCondition(g_val);
    if(ok)
      m_logic_conditions.push_back(new_condition);
    return(ok);
  }

  else if(g_param == "runflag") {
    //g_val = findReplace(g_val, ',', '=');
    vector<string> svector = parseString(g_val, '=');
    if(svector.size() != 2)
      return(false);
    string var = stripBlankEnds(svector[0]);
    string val = stripBlankEnds(svector[1]);
    VarDataPair pair(var, val, "auto");
    m_run_flags.push_back(pair);
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
  else if(g_param == "nostarve") {
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
    return(setParamCommon("build_info", precision_info)); 
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
      m_started = false;
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
    m_state_ok = false;
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

void IvPBehavior::postMessage(string var, string sdata)
{
  m_messages.push_back(VarDataPair(var, sdata));
}

//-----------------------------------------------------------
// Procedure: postMessage

void IvPBehavior::postMessage(string var, double ddata)
{
  m_messages.push_back(VarDataPair(var, ddata));
}

//-----------------------------------------------------------
// Procedure: setComplete

void IvPBehavior::setComplete()
{
  postFlags("endflags");
  if(!m_perpetual)
    m_completed = true;
}


//-----------------------------------------------------------
// Procedure: postEMessage

void IvPBehavior::postEMessage(string g_emsg)
{
  string emsg = "";
  if(m_descriptor != "")
    emsg += m_descriptor + ": ";
  emsg += g_emsg;

  VarDataPair msg("BHV_ERROR", emsg);
  m_messages.push_back(msg);

  m_state_ok = false;
}


//-----------------------------------------------------------
// Procedure: postWMessage

void IvPBehavior::postWMessage(string g_wmsg)
{
  string wmsg = "";
  if(m_descriptor != "")
    wmsg += m_descriptor + ": ";
  wmsg += g_wmsg;

  VarDataPair msg("BHV_WARNING", wmsg);
  m_messages.push_back(msg);
}


//-----------------------------------------------------------
// Procedure: postPCMessage

void IvPBehavior::postPCMessage(string g_msg)
{
  string mvar = "PC_" + m_descriptor;
  
  //double curr_time = m_info_buffer->getCurrTime();
  //string str_time = "(" + doubleToString(curr_time, 1) + ")";
  //string mval = str_time + g_msg;
  string mval = g_msg;
  
  VarDataPair msg(mvar, mval);
  m_messages.push_back(msg);
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
    string community = m_us_name;
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
  vector<string> svector;
  for(int i=0; i<m_logic_conditions.size(); i++)
    svector = mergeVectors(svector, m_logic_conditions[i].getVarNames());

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
      postPCMessage(msg);
      return(false);
    }
  }
  return(true);
}

//-----------------------------------------------------------
// Procedure: checkUpdates

void IvPBehavior::checkUpdates()
{
  if(m_update_var == "")
    return;

  int    i;
  bool   ok;
  string new_update_str;
  double curr_update_age;
  
  new_update_str  = m_info_buffer->sQuery(m_update_var, ok);
  new_update_str  = stripBlankEnds(new_update_str);
  curr_update_age = m_info_buffer->tQuery(m_update_var);
  
  bool fresh = false;
  if((curr_update_age < m_last_update_age) || (m_last_update_age == -1))
    fresh = true;
  
  if((fresh) && (new_update_str != "") && 
     (new_update_str != m_curr_update_str)) {
    
    vector<string> uvector = parseString(new_update_str, '#');
    int usize = uvector.size();
    
    string bad_params;
    ok = true;
    for(i=0; i<usize; i++) {
      string an_update_str = uvector[i];
      vector<string> pvector = chompString(an_update_str, '=');
      if(pvector.size() == 2) {
	string param = stripBlankEnds(pvector[0]);
	string value = stripBlankEnds(pvector[1]);
	bool  result = setParam(param, value);
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
      m_good_updates++;
      m_curr_update_str = new_update_str;
    }
  }

  m_last_update_age = curr_update_age;

  if((m_good_updates + m_bad_updates)>0) {
    string varname = "UH_" + m_descriptor;
    string gstr = intToString(m_good_updates) + " update(s), and ";
    string bstr = intToString(m_bad_updates) + " failure(s)";
    postMessage(varname, gstr+bstr);
  }
}


//-----------------------------------------------------------
// Procedure: durationExceeded()

bool IvPBehavior::durationExceeded()
{
  // If a duration was never specified, just return false
  if(m_duration == -1)
    return(false);

  double curr_time = m_info_buffer->getCurrTime();

  if(!m_started) {
    m_started = true;
    m_start_time = m_info_buffer->getCurrTime();
  }

  double elapsed_time = (curr_time - m_start_time);
  double remaining_time = m_duration - elapsed_time;
  if(remaining_time < 0)
    remaining_time = 0;
  if(m_duration_status != "")
    postMessage(m_duration_status, remaining_time);
  
  if(elapsed_time >= m_duration)
    return(true);

  return(false);
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

  int vsize = flags.size();
  for(int i=0; i<vsize; i++) {
    string var   = flags[i].get_var();
    string sdata = flags[i].get_sdata();
    double ddata = flags[i].get_ddata();
    if(sdata != "")
      postMessage(var, sdata);
    else
      postMessage(var, ddata);
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




