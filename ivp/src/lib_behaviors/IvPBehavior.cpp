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
  domain            = g_domain;
  info_buffer       = 0;
  priority_wt       = 100.0;  // Default Priority Weight
  descriptor        = "???";  // Default descriptor
  grid_box          = 0;
  unif_box          = 0;
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
// Procedure: Destructor

IvPBehavior::~IvPBehavior()
{
  if(grid_box) delete(grid_box);
  if(unif_box) delete(unif_box);
}
  
//-----------------------------------------------------------
// Procedure: setParam

bool IvPBehavior::setParam(string g_param, string g_val) 
{
  return(setParamCommon(g_param, g_val));
}

//-----------------------------------------------------------
// Procedure: setParamCommon

bool IvPBehavior::setParamCommon(string g_param, string g_val) 
{
  g_val   = stripBlankEnds(g_val);
  g_param = tolower(g_param);

  // Allow parameters to be prefaced by the '_' and be treated
  // as if that character were not present.
  if(g_param[0] == '_')
    g_param = g_param.c_str()+1;

  if(g_param == "us") { 
    us_name = g_val;
    return(true);
  }
  else if((g_param == "name") || (g_param == "descriptor")) {
    descriptor = g_val;
    return(true);
  }
  else if(g_param == "duration_status") {
    duration_status = g_val;
    return(true);
  }
  else if((g_param == "pwt") || 
	  (g_param == "priwt") || 
	  (g_param == "priority")) {   // Alt way of setting priority
    double pwt = atof(g_val.c_str());
    if((pwt < 0) || (!isNumber(g_val)))
      return(false);
    priority_wt = pwt;
    return(true);
  }
  else if(g_param == "gridbox") {  // Alt way of setting gridbox
    if(g_val == "default")
      return(grid_box != 0);

    IvPBox ptbox = stringToPointBox(g_val, domain);
    if(!ptbox.isPtBox())
      return(false);

    IvPBox *new_box = ptbox.copy(); 
    if(grid_box)
      delete(grid_box);
    grid_box = new_box;
    if(unif_box == 0)               // grid_box makes a good
      unif_box = grid_box->copy();  // default unif_box too!
    else
      delete(new_box);
    return(true);
  }
  else if(g_param == "unifbox") { // Alt way of setting unifbox
    if(g_val == "default")
      return(unif_box != 0);

    IvPBox ptbox = stringToPointBox(g_val, domain);
    if(!ptbox.isPtBox())
      return(false);

    IvPBox *new_box = ptbox.copy();
    if(unif_box)
      delete(unif_box);
    unif_box = new_box;
    if(grid_box == 0)               // unif_box makes a good
      grid_box = unif_box->copy();  // default grid_box too!
    else
      delete(new_box);
    return(true);
  }

  else if(g_param == "condition") {
    g_val = findReplace(g_val, ',', '=');
    g_val = findReplace(g_val, "==", "=");
    bool ok = true;
    LogicCondition new_condition;
    ok = new_condition.setCondition(g_val);
    if(ok)
      logic_conditions.push_back(new_condition);
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
    run_flags.push_back(pair);
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
    idle_flags.push_back(pair);
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
    end_flags.push_back(pair);
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
      starve_vars[var] = stime;
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

  else if(g_param == "updates") {
    m_update_var = g_val;
    if(m_update_var != "")
      info_vars.push_back(g_val);
    return(true);
  }
  
  else if(g_param == "precision") {
    if((g_val != "high") && (g_val != "normal") && (g_val != "low"))
      return(false);
    if((g_val == "high") && (unif_box)) {
      int dim = unif_box->getDim();
      for(int d=0; d<dim; d++) {
	unif_box->pt(d,0) = unif_box->pt(d,0) / 2;
	unif_box->pt(d,1) = unif_box->pt(d,1) / 2;
	if(unif_box->pt(d,0) == 0)
	  unif_box->pt(d,0) = 1;
	if(unif_box->pt(d,1) == 0)
	  unif_box->pt(d,1) = 1;
      }
    }
    if((g_val == "low") && (unif_box)) {
      int dim = unif_box->getDim();
      for(int d=0; d<dim; d++) {
	unif_box->pt(d,0) = unif_box->pt(d,0) * 2;
	unif_box->pt(d,1) = unif_box->pt(d,1) * 2;
      }
    }
    return(true);
  }

  return(false);
}

//-----------------------------------------------------------
// Procedure: preCheck

bool IvPBehavior::preCheck()
{
  if(m_completed) {
    postPCMessage(" -- completed -- ");
    //postFlags(end_flags);
    return(false);
  }

  if(!checkConditions())
    return(false);

  // Important that this be called after checkConditions so 
  // the duration clock doesn't start until the the conditions
  // are met.
  if(durationExceeded()) {
    postPCMessage(" -- completed (duration exceeded) -- ");
    setComplete();
    if(m_perpetual)
      m_started = false;
    return(false);
  }
  
  if(domain.size() == 0) {
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
  postFlags(run_flags);
  return(true);
}
  
  //-----------------------------------------------------------
  // Procedure: setInfoBuffer

void IvPBehavior::setInfoBuffer(const InfoBuffer *ib)
{
  info_buffer = ib;
}

//-----------------------------------------------------------
// Procedure: postMessage

void IvPBehavior::postMessage(string var, string sdata)
{
  messages.push_back(VarDataPair(var, sdata));
}

//-----------------------------------------------------------
// Procedure: postMessage

void IvPBehavior::postMessage(string var, double ddata)
{
  messages.push_back(VarDataPair(var, ddata));
}

//-----------------------------------------------------------
// Procedure: setComplete

void IvPBehavior::setComplete()
{
  postFlags(end_flags);
  if(!m_perpetual)
    m_completed = true;
}


//-----------------------------------------------------------
// Procedure: postEMessage

void IvPBehavior::postEMessage(string g_emsg)
{
  string emsg = "";
  if(descriptor != "")
    emsg += descriptor + ": ";
  emsg += g_emsg;

  VarDataPair msg("BHV_ERROR", emsg);
  messages.push_back(msg);

  m_state_ok = false;
}


//-----------------------------------------------------------
// Procedure: postWMessage

void IvPBehavior::postWMessage(string g_wmsg)
{
  string wmsg = "";
  if(descriptor != "")
    wmsg += descriptor + ": ";
  wmsg += g_wmsg;

  VarDataPair msg("BHV_WARNING", wmsg);
  messages.push_back(msg);
}


//-----------------------------------------------------------
// Procedure: postPCMessage

void IvPBehavior::postPCMessage(string g_msg)
{
  string mvar = "PC_" + descriptor;
  
  double curr_time = info_buffer->getCurrTime();
  string str_time = "(" + doubleToString(curr_time, 1) + ")";
  string mval = str_time + g_msg;
  
  VarDataPair msg(mvar, mval);
  messages.push_back(msg);
}


//-----------------------------------------------------------
// Procedure: checkConditions()

bool IvPBehavior::checkConditions()
{
  if(!info_buffer) 
    return(false);

  int i, j, vsize, csize;

  // Phase 1: get all the variable names from all present conditions.
  vector<string> all_vars;
  csize = logic_conditions.size();
  for(i=0; i<csize; i++) {
    vector<string> svector = logic_conditions[i].getVarNames();
    all_vars = mergeVectors(all_vars, svector);
  }
  all_vars = removeDuplicates(all_vars);

  // Phase 2: get values of all variables from the info_buffer and 
  // propogate these values down to all the logic conditions.
  vsize = all_vars.size();
  for(i=0; i<vsize; i++) {
    string varname = all_vars[i];
    string community = us_name;
    bool   ok_s, ok_d;
    string s_result = info_buffer->sQuery(varname, ok_s);
    double d_result = info_buffer->dQuery(varname, ok_d);

    for(j=0; (j<csize)&&(ok_s); j++)
      logic_conditions[j].setVarVal(varname, s_result);
    for(j=0; (j<csize)&&(ok_d); j++)
      logic_conditions[j].setVarVal(varname, d_result);
  }

  // Phase 3: evaluate all logic conditions. Return true only if all
  // conditions evaluate to be true.
  for(i=0; i<csize; i++) {
    bool satisfied = logic_conditions[i].eval();
    if(!satisfied) {
      string failed_condition = logic_conditions[i].getRawCondition();
      postPCMessage(failed_condition);
      return(false);
    }
  }
  return(true);

}

//-----------------------------------------------------------
// Procedure: getInfoVars()

vector<string> IvPBehavior::getInfoVars()
{
  vector<string> svector;
  for(int i=0; i<logic_conditions.size(); i++)
    svector = mergeVectors(svector, logic_conditions[i].getVarNames());

  svector = mergeVectors(svector, info_vars);
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
  if(!info_buffer) 
    return(false);

  map<string,double>::const_iterator p;
  for(p=starve_vars.begin(); p!=starve_vars.end(); p++) {
    string var   = p->first;
    double stime = p->second;
    double itime = info_buffer->tQuery(var);

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
// Procedure: checkForUpdates

void IvPBehavior::checkForUpdates()
{
  if(m_update_var == "")
    return;

  int    i;
  bool   ok;
  string new_update_str;
  double curr_update_age;
  
  new_update_str  = info_buffer->sQuery(m_update_var, ok);
  new_update_str  = stripBlankEnds(new_update_str);
  curr_update_age = info_buffer->tQuery(m_update_var);
  
  bool fresh = false;
  if((curr_update_age < m_last_update_age) || (m_last_update_age == -1))
    fresh = true;
  
  if((fresh) && (new_update_str != "") && 
     (new_update_str != m_curr_update_str)) {
    
    vector<string> uvector = parseString(new_update_str, '#');
    int usize = uvector.size();
    
    ok = true;
    for(i=0; i<usize; i++) {
      string an_update_str = uvector[i];
      vector<string> pvector = chompString(an_update_str, '=');
      if(pvector.size() == 2) {
	string param = stripBlankEnds(pvector[0]);
	string value = stripBlankEnds(pvector[1]);
	bool  result = setParam(param, value);
	ok = ok && result;
      }
      else 
	ok = false;
    }
    
    if(!ok) {
      m_bad_updates++;
      string wmsg = "Faulty Behavior (" + descriptor + ") Update - INCOMPLETE!";
      postMessage("BHV_WARNING", wmsg);
    }
    else {
      m_good_updates++;
      m_curr_update_str = new_update_str;
    }
  }

  m_last_update_age = curr_update_age;

  if((m_good_updates + m_bad_updates)>0) {
    string varname = "UH_" + descriptor;
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

  double curr_time = info_buffer->getCurrTime();

  if(!m_started) {
    m_started = true;
    m_start_time = info_buffer->getCurrTime();
  }

  double elapsed_time = (curr_time - m_start_time);
  double remaining_time = m_duration - elapsed_time;
  if(remaining_time < 0)
    remaining_time = 0;
  if(duration_status != "")
    postMessage(duration_status, remaining_time);
  
  if(elapsed_time >= m_duration)
    return(true);

  return(false);
}


//-----------------------------------------------------------
// Procedure: postFlags()

void IvPBehavior::postFlags(const vector<VarDataPair>& flags)
{
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




