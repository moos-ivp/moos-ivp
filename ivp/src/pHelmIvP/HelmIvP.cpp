/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HelmIvP.cpp                                          */
/*    DATE: Oct 12th 2004                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iterator>
#include <iostream>
#include <cstdlib>
#include <math.h>
#include "HelmIvP.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "MBTimer.h" 
#include "FunctionEncoder.h" 
#include "IvPProblem.h"
#include "HelmReport.h"
#include "Populator_BehaviorSet.h"
#include "LifeEvent.h"

using namespace std;

//--------------------------------------------------------------------
// Procedure: Constructor

HelmIvP::HelmIvP()
{
  m_allstop_msg    = "clear";
  m_bhv_set        = 0;
  m_hengine        = 0;
  m_info_buffer    = new InfoBuffer;
  m_verbose        = "terse";
  m_helm_iteration = 0;
  m_ok_skew        = 60; 
  m_skews_matter   = true;
  m_warning_count  = 0;
  m_last_heartbeat = 0;
  m_curr_time      = 0;
  m_start_time     = 0;

  // The m_has_control correlates to helm ENGAGEMENT mode
  m_has_control    = false;

  m_allow_override = true;
  m_disengage_on_allstop = false;

  m_curr_time_updated = false;

  m_init_vars_ready  = false;
  m_init_vars_done   = false;

  // The refresh vars handle the occasional clearing of the m_outgoing
  // maps. These maps will be cleared when MOOS mail is received for the
  // variable given by m_refresh_var. The user can set minimum interval
  // between refreshes so the helm retains some control over refresh rate.
  // Motivated by the need for a viewer handling geometric postings from
  // behaviors. The new arrival of a viewer into the MOOS community can 
  // request a refresh and then get new geometry mail to process.
  m_refresh_var      = "HELM_MAP_CLEAR";
  m_refresh_interval = 2.0;
  m_refresh_pending  = false;
  m_refresh_time     = 0;

  m_node_report_vars.push_back("AIS_REPORT");
  m_node_report_vars.push_back("NODE_REPORT");
  m_node_report_vars.push_back("AIS_REPORT_LOCAL");
  m_node_report_vars.push_back("NODE_REPORT_LOCAL");
}

//--------------------------------------------------------------------
// Procedure: Destructor

HelmIvP::~HelmIvP()
{
  cleanup();
}

//--------------------------------------------------------------------
// Procedure: cleanup
//      Note: Called by the destructor - also upon helm re-build

void HelmIvP::cleanup()
{
  delete(m_info_buffer);
  m_info_buffer = 0;
  m_curr_time_updated = false;

  delete(m_bhv_set);
  m_bhv_set = 0;

  delete(m_hengine);
  m_hengine = 0;

  m_ivp_domain = IvPDomain();
}

//--------------------------------------------------------------------
// Procedure: OnNewMail()

bool HelmIvP::OnNewMail(MOOSMSG_LIST &NewMail)
{
  // The curr_time is set in *both* the OnNewMail and Iterate functions.
  // In the OnNewMail function so the most up-to-date time is available
  // when processing mail.
  // In the Iterate method to ensure behaviors are not iterated with an
  // un-initialized timestamp on startup, and in case there is no Mail 
  m_curr_time = MOOSTime();
  m_info_buffer->setCurrTime(m_curr_time);
  m_curr_time_updated = true;

  MOOSMSG_LIST::iterator p;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string moosvar = msg.GetKey();
    string sval    = msg.GetString();

    double skew_time;
    msg.IsSkewed(m_curr_time, &skew_time);
    
    if(!m_skews_matter || (fabs(skew_time) < m_ok_skew)) {
      if((moosvar =="MOOS_MANUAL_OVERIDE") || 
	 (moosvar =="MOOS_MANUAL_OVERRIDE") ||
	 ((moosvar == m_additional_override) && (moosvar != ""))) {
	if(toupper(sval) == "FALSE") {
	  m_has_control = true;
	  postAllStop("clear");
	}
	else if(toupper(sval) == "TRUE") {
	  if(m_allow_override) {
	    m_has_control = false;
	    postAllStop("ManualOverride");
	  }
	}
      }
      else if(moosvar == "IVPHELM_ENGAGED")
	m_init_vars_ready = true;
      else if(moosvar == "HELM_VERBOSE") {
	if((sval == "verbose") || (sval == "quiet") || (sval == "terse"))
	  m_verbose = sval;
      }
      else if(moosvar == "RESTART_HELM") {
	OnStartUp();
	MOOSTrace("\n");
	MOOSDebugWrite("pHelmIvP Has Been Re-Started");
      }
      else if(vectorContains(m_node_report_vars, moosvar)) {
	bool ok = processNodeReport(sval);
	if(!ok) {
	  m_Comms.Notify("BHV_WARNING", "Unhandled NODE REPORT");
	  m_warning_count++;
	}
      }
      else if(moosvar == m_refresh_var) {
	m_refresh_pending = true;
      }
      else
	updateInfoBuffer(msg);
    }
    else {
      // Add the variable name inthe out put?
      MOOSTrace("pHelmIvP ignores MOOS msg due to skew");
      MOOSDebugWrite("pHelmIvP ignores MOOS msg due to skew");
    }
  }

  return(true);
}

//--------------------------------------------------------------------
// Procedure: Iterate()
//      Note: happens AppTick times per second
//            

bool HelmIvP::Iterate()
{
  postEngagedStatus();
  postCharStatus();
  
  if(m_init_vars_ready && !m_init_vars_done)
    handleInitialVarsPhase2();

  // If the curr_time is not set in the OnNewMail function (possibly 
  // because there was no mail in the queue), set the current time now.
  if(!m_curr_time_updated) {
    m_curr_time = MOOSTime();
    m_info_buffer->setCurrTime(m_curr_time);
  }
  if(m_start_time == 0)
    m_start_time = m_curr_time;

  // Now we're done addressing whether the curr_time is updated on this
  // iteration. It was done either in this function or in onNewMail().
  // Now set m_curr_time_updated=false to reflect the ingoing state for
  // the next iteration.
  m_curr_time_updated = false;

  if(!m_has_control) {
    postAllStop("ManualOverride");
    return(false);
  }

  HelmReport helm_report;
  helm_report = m_hengine->determineNextDecision(m_bhv_set, m_curr_time);

  m_helm_iteration = helm_report.getIteration();
  if(m_verbose == "verbose") {
    MOOSTrace("\n\n\n\n");
    MOOSTrace("Iteration: %d", m_helm_iteration);
    MOOSTrace("  ******************************************\n");
    MOOSTrace("Helm Summary  ---------------------------\n");
    vector<string> svector = helm_report.getMsgs();
    for(unsigned int i=0; i<svector.size(); i++)
      MOOSTrace("%s\n", svector[i].c_str());
  }
 
  // Check if refresh conditions are met - perhaps clear outgoing maps.
  // This will result in all behavior postings being posted to the MOOSDB
  // on the current iteration.
  if(m_refresh_pending && 
     ((m_curr_time-m_refresh_time) > m_refresh_interval)) {
    m_outgoing_strings.clear();
    m_outgoing_doubles.clear();
    m_refresh_time = m_curr_time;
    m_refresh_pending = false;
  }

  registerNewVariables();
  postBehaviorMessages();
  postLifeEvents();
  postModeMessages();
  postDefaultVariables();

  // Should be called after postBehaviorMessages() where warnings
  // are detected and the count is incremented.
  helm_report.setWarningCount(m_warning_count);

  m_Comms.Notify("HELM_IPF_COUNT", helm_report.getOFNUM());
  m_Comms.Notify("IVPHELM_SUMMARY", helm_report.getReportAsString());
 
  if(m_verbose == "verbose") {
    MOOSTrace("(End) Iteration: %d", m_helm_iteration);
    MOOSTrace("  ******************************************\n");
  }
  
  string allstop_msg = "clear";

  if(helm_report.getHalted())
    allstop_msg = "BehaviorError";
  else if(helm_report.getOFNUM() == 0)
    allstop_msg = "NoIvPFunctions";
  
  // First make sure the HelmEngine has made a decision for all 
  // non-optional variables - otherwise declare an incomplete decision.
  if(allstop_msg == "clear") {
    bool   complete_decision = true;
    string missing_dec_vars;
    unsigned int i, dsize = m_ivp_domain.size();
    for(i=0; i<dsize; i++) {
      string domain_var = m_ivp_domain.getVarName(i);
      if(!helm_report.hasDecision(domain_var)) {
	if(m_optional_var[domain_var] == false) {
	  complete_decision = false;
	  if(missing_dec_vars != "")
	    missing_dec_vars += ",";
	  missing_dec_vars += domain_var;
	}
      }
    }
    if(!complete_decision) {
      allstop_msg = "MissingDecVars:" + missing_dec_vars;
      m_Comms.Notify("BHV_ERROR", allstop_msg); 
    }
  }

  if(allstop_msg != "clear")
    postAllStop(allstop_msg);
  else {  // Post all the Decision Variable Results
    postAllStop("clear");
    unsigned int j, dsize = m_ivp_domain.size();
    for(j=0; j<dsize; j++) {
      string domain_var = m_ivp_domain.getVarName(j);
      string post_alias = "DESIRED_"+ toupper(domain_var);
      if(post_alias == "DESIRED_COURSE")
	post_alias = "DESIRED_HEADING";
      double domain_val = helm_report.getDecision(domain_var);
      m_Comms.Notify(post_alias, domain_val);
    }
  }
  
  m_Comms.Notify("CREATE_CPU", helm_report.getCreateTime());
  m_Comms.Notify("LOOP_CPU", helm_report.getLoopTime());

  if(allstop_msg != "clear")
    if(m_allow_override && m_disengage_on_allstop)
      m_has_control = false;

  // Clear the delta vectors now that all behavior have had the 
  // chance to consume delta info.
  m_info_buffer->clearDeltaVectors();

  return(true);
}
  

//------------------------------------------------------------
// Procedure: postBehaviorMessages()
//      Note: Run once after every iteration of control loop.
//            Each behavior has the chance to produce their 
//            own message to posted in both the info_buffer
//            and in the MOOSDB. 
//            By posting to the MOOSDB and using a mechanism
//            for sharing vars between communities, i.e, 
//            pMOOSBridge, or MOOSLink, inter-vehicle coordin.
//            can be accomplished.

void HelmIvP::postBehaviorMessages()
{
  if(!m_bhv_set) 
    return;
  
  if(m_verbose == "verbose") {
    MOOSTrace("Behavior Report ---------------------------------\n");
  }

  unsigned int i, bhv_cnt = m_bhv_set->size();
  m_Comms.Notify("BCOUNT", bhv_cnt);
  m_Comms.Notify("TBCOUNT", m_bhv_set->getTCount());
  m_Comms.Notify("HITER", m_helm_iteration);
  for(i=0; i < bhv_cnt; i++) {
    string bhv_descriptor = m_bhv_set->getDescriptor(i);
    vector<VarDataPair> mvector = m_bhv_set->getMessages(i);
    unsigned int j, msize = mvector.size();

    string bhv_postings_summary;
  
    for(j=0; j<msize; j++) {
      VarDataPair msg = mvector[j];

      string var   = msg.get_var();
      string sdata = msg.get_sdata();
      double ddata = msg.get_ddata();
      string mkey  = msg.get_key();
      bool   msg_is_string = msg.is_string();

      bool key_change = true;
      bool key_repeat = detectRepeatOnKey(var);
      if(sdata == "")
	key_change = detectChangeOnKey(mkey, ddata);
      else
	key_change = detectChangeOnKey(mkey, sdata);
 
      // Keep track of warnings count for inclusion in IVPHELM_SUMMARY
      if(var == "BHV_WARNING")
	m_warning_count++;

      // Possibly augment the postings_summary
      if(key_change) {
	// If first var-data tuple then tack header info on front
	if(bhv_postings_summary == "") {
	  bhv_postings_summary += bhv_descriptor;
	  bhv_postings_summary += "$@!$";
	  bhv_postings_summary += intToString(m_helm_iteration);
	}
	// Handle BHV_IPF tuples separately below
	if(var != "BHV_IPF") {
	  string pair_printable = msg.getPrintable();
	  bhv_postings_summary += "$@!$";
	  bhv_postings_summary += pair_printable;
	}
      }

      // If posting an IvP Function, mux first and post the parts.
      if(var == "BHV_IPF") { // mikerb
	string id = bhv_descriptor + intToString(m_helm_iteration);
	vector<string> svector = IvPFunctionToVector(sdata, id, 2000);
	for(unsigned int k=0; k<svector.size(); k++)
	  m_Comms.Notify("BHV_IPF", svector[k], bhv_descriptor);
      }
      // Otherwise just post to the DB directly.
      else {
	if(msg_is_string) {
	  m_info_buffer->setValue(var, sdata);
	  if(key_change || key_repeat) {
	    string aux = intToString(m_helm_iteration) + ":" + 
	      bhv_descriptor;
	    m_Comms.Notify(var, sdata, aux);
	    m_outgoing_timestamp[var] = m_curr_time;
	  }
	}
	else {
	  m_info_buffer->setValue(var, ddata);
	  if(key_change || key_repeat) {
	    string aux = intToString(m_helm_iteration) + ":" + 
	      bhv_descriptor;
	    m_Comms.Notify(var, ddata, aux);
	    m_outgoing_timestamp[var] = m_curr_time;
	  }
	}
      }
    }
    if(bhv_postings_summary != "")
      m_Comms.Notify("IVPHELM_POSTINGS", bhv_postings_summary, bhv_descriptor);
  }
  // Determine if the list of state-space related variables for
  // the behavior-set has changed and post the new set if so.

  bool changed = m_bhv_set->updateStateSpaceVars();
  if(changed) {
    string state_vars = m_bhv_set->getStateSpaceVars();
    m_Comms.Notify("IVPHELM_STATEVARS", state_vars);
  }
  m_bhv_set->removeCompletedBehaviors();
}

//------------------------------------------------------------
// Procedure: postLifeEvents()
//      Note: Run once after every iteration of control loop.

void HelmIvP::postLifeEvents()
{
  if(!m_bhv_set) 
    return;
  
  vector<LifeEvent> events = m_bhv_set->getLifeEvents();
  unsigned int i, vsize = events.size();
  for(i=0; i<vsize; i++) {
    double htime = m_curr_time - m_start_time;
    string str = "time=" + doubleToString(htime, 2);
    str += ", iter="  + intToString(m_helm_iteration);
    str += ", bname=" + events[i].getBehaviorName();
    str += ", btype=" + events[i].getBehaviorType();
    str += ", event=" + events[i].getEventType();
    str += ", seed="  + events[i].getSpawnString();
    m_Comms.Notify("IVPHELM_LIFE_EVENT", str);
  }
  if(vsize > 0)
    m_bhv_set->clearLifeEvents();
}


//------------------------------------------------------------
// Procedure: postModeMessages()
//      Note: Run once after every iteration of control loop.

void HelmIvP::postModeMessages()
{
  if(!m_bhv_set) 
    return;
  
  vector<VarDataPair> mvector = m_bhv_set->getModeVarDataPairs();
  unsigned int j, msize = mvector.size();

  for(j=0; j<msize; j++) {
    VarDataPair msg = mvector[j];
    
    string var   = msg.get_var();
    string sdata = msg.get_sdata();
    double ddata = msg.get_ddata();
    string mkey  = msg.get_key();

    bool key_change = true;
    if(msg.is_string()) {
      key_change = detectChangeOnKey(mkey, sdata);
      if(key_change)
	m_Comms.Notify(var, sdata);
    }
    else {
      key_change = detectChangeOnKey(mkey, ddata);
      if(key_change)
	m_Comms.Notify(var, ddata);
    }
  }
}

//------------------------------------------------------------
// Procedure: handleInitialVarsPhase1()

void HelmIvP::handleInitialVarsPhase1()
{
  if(!m_bhv_set) 
    return;

  vector<VarDataPair> mvector = m_bhv_set->getInitialVariables();
  unsigned int j, msize = mvector.size();
  for(j=0; j<msize; j++) {
    VarDataPair msg = mvector[j];

    string var   = stripBlankEnds(msg.get_var());
    string sdata = stripBlankEnds(msg.get_sdata());
    double ddata = msg.get_ddata();
    string key   = tolower(msg.get_key());
    // key should be set to either "post" or "defer"

    if(!strContainsWhite(var)) {
      // If this is an init with deferment, just register.
      if(key == "defer")
	registerSingleVariable(var);
      // If this is an init with overwrite, post and update the 
      // info_buffer now.
      if(key == "post") {
	if(sdata != "") {
	  m_info_buffer->setValue(var, sdata);
	  m_Comms.Notify(var, sdata, "HELM_VAR_INIT");
	}
	else {
	  m_info_buffer->setValue(var, ddata);
	  m_Comms.Notify(var, ddata, "HELM_VAR_INIT");
	}
      }
    }

  }
  registerSingleVariable("IVPHELM_ENGAGED");
}

//------------------------------------------------------------
// Procedure: handleInitialVarsPhase2()
//     Notes: Handles only initial variables that use deferment.
//            e.g. initialize_  FOO = "bar"
//            All other initialize lines were handled in phase1.

void HelmIvP::handleInitialVarsPhase2()
{
  if(!m_bhv_set) 
    return;

  m_init_vars_done = true;
  vector<VarDataPair> mvector = m_bhv_set->getInitialVariables();
  unsigned int j, msize = mvector.size();
  for(j=0; j<msize; j++) {
    VarDataPair msg = mvector[j];
    
    string var   = stripBlankEnds(msg.get_var());
    string sdata = stripBlankEnds(msg.get_sdata());
    double ddata = msg.get_ddata();

    string key   = tolower(msg.get_key());
    // key should be set to either "post" or "defer"

    if((key == "defer") && !m_info_buffer->isKnown(var)) {
      if(sdata != "") {
	m_info_buffer->setValue(var, sdata);
	m_Comms.Notify(var, sdata, "HELM_VAR_INIT");
      }
      else {
	m_info_buffer->setValue(var, ddata);
	m_Comms.Notify(var, ddata, "HELM_VAR_INIT");
      }
    }
  }
}

//------------------------------------------------------------
// Procedure: postDefaultVariables()
//
//   Post the "default variables". These variable/value pairs are 
//   provided in the behavior file. The post is made on each helm 
//   iteration if the variable involved was not written to by any
//   of the behaviors on this iteration. Thus all the behavior
//   messages must be collected first (phase1) and the default
//   values posted once compared against the list (phase 2).

  
void HelmIvP::postDefaultVariables()
{
  if(!m_bhv_set) return;

  // Phase 1 - determine what variables were written to by the 
  // behaviors during the last iteration.
  vector<string> message_vars;
  int bhv_cnt = m_bhv_set->getCount();
  for(int i=0; i < bhv_cnt; i++) {
    vector<VarDataPair> mvector = m_bhv_set->getMessages(i);
    unsigned int j, vsize = mvector.size();
    for(j=0; j<vsize; j++) {
      VarDataPair msg = mvector[j];
      message_vars.push_back(msg.get_var());
    }
  }

  // Phase 2 - Examine each of the default_messages and determine
  // for each, if the variable was contained in one of the behavior
  // messages for this iteration. If not, then post the default.
  vector<VarDataPair> dvector = m_bhv_set->getDefaultVariables();
  unsigned int j, dsize = dvector.size();
  for(j=0; j<dsize; j++) {
    VarDataPair msg = dvector[j];
    string var = msg.get_var();
    if(!vectorContains(message_vars, var)) {
      if(msg.is_string()) {
	string sdata  = msg.get_sdata();
	m_info_buffer->setValue(var, sdata);
	m_Comms.Notify(var, sdata);
      }
      else {
	double ddata  = msg.get_ddata();
	m_info_buffer->setValue(var, ddata);
	m_Comms.Notify(var, ddata);
      }
    }
  }
}

//------------------------------------------------------------
// Procedure: postEngagedStatus()

void HelmIvP::postEngagedStatus()
{
  string engaged_status = "ENGAGED";
  if(!m_has_control)
    engaged_status = "DISENGAGED";

  double heartbeat_interval = 1.0;  // seconds
  bool time_for_a_new_heartbeat = false;
  if((m_curr_time-m_last_heartbeat) >= heartbeat_interval) {
      time_for_a_new_heartbeat = true;
      m_last_heartbeat = m_curr_time;
  }

  bool changed = detectChangeOnKey("IVPHELM_ENGAGED", engaged_status);
  if(changed || time_for_a_new_heartbeat)
    m_Comms.Notify("IVPHELM_ENGAGED", engaged_status);
}

//------------------------------------------------------------
// Procedure: postCharStatus()

void HelmIvP::postCharStatus()
{
  if(m_has_control) {
    if(m_verbose == "terse")
      MOOSTrace("$");
  }
  else
    MOOSTrace("*");
}

//------------------------------------------------------------
// Procedure: updateInfoBuffer()
// Note: A src_aux=HELM_VAR_INIT means that this messages was posted
//       by the helm itself and does not need to be applied again to 
//       the info_buffer. Handles the rare but possible situation 
//       where another app posted to the same variable on the same 
//       batch of incoming mail. In this case we want to defer to 
//       value posted by the other application.

bool HelmIvP::updateInfoBuffer(CMOOSMsg &msg)
{
  string moosvar = msg.m_sKey;
  string src_aux = msg.GetSourceAux();
  if(src_aux == "HELM_VAR_INIT")
    return(false);
    
  if(msg.IsDataType(MOOS_DOUBLE)) {
    return(m_info_buffer->setValue(moosvar, msg.GetDouble()));
  }
  else if(msg.IsDataType(MOOS_STRING)) {
    return(m_info_buffer->setValue(moosvar, msg.GetString()));
  }
  return(false);
}

//------------------------------------------------------------
// Procedure: OnConnectToServer()

bool HelmIvP::OnConnectToServer()
{
  registerVariables();
  requestBehaviorLogging();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void HelmIvP::registerVariables()
{
  registerSingleVariable("DB_CLIENTS");
  registerSingleVariable("MOOS_MANUAL_OVERIDE");
  registerSingleVariable("MOOS_MANUAL_OVERRIDE");
  registerSingleVariable("RESTART_HELM");
  registerSingleVariable("HELM_VERBOSE");
  
  registerSingleVariable("NAV_SPEED");
  registerSingleVariable("NAV_HEADING");
  registerSingleVariable("NAV_DEPTH");
  registerSingleVariable(m_refresh_var);

  if(m_additional_override != "")
    registerSingleVariable(m_additional_override);

  // Register for node report variables, e.g., AIS_REPORT, NODE_REPORT
  unsigned int i, vsize = m_node_report_vars.size();
  for(i=0; i<vsize; i++) 
    registerSingleVariable(m_node_report_vars[i]);
  
  if(m_bhv_set) {
    vector<string> info_vars = m_bhv_set->getInfoVars();
    unsigned int j, jsize = info_vars.size();
    for(j=0; j<jsize; j++)
      registerSingleVariable(info_vars[j]);
    vector<string> update_vars = m_bhv_set->getSpecUpdateVars();
    unsigned int i, isize = update_vars.size();
    for(i=0; i<isize; i++) 
      registerSingleVariable(update_vars[i]);
  }
}

//------------------------------------------------------------
// Procedure: registerNewVariables

void HelmIvP::registerNewVariables()
{
  if(m_bhv_set) {
    vector<string> info_vars = m_bhv_set->getNewInfoVars();
    unsigned int j, jsize = info_vars.size();
    for(j=0; j<jsize; j++) 
      registerSingleVariable(info_vars[j]);
  }
}


//------------------------------------------------------------
// Procedure: registerSingleVariable

void HelmIvP::registerSingleVariable(string varname, double frequency)
{
  if(frequency < 0)
    frequency = 0;
  if(m_verbose == "verbose")
    cout << "Registering for: " << varname.c_str() << endl;
  m_Comms.Register(varname, frequency);
}


//------------------------------------------------------------
// Procedure: requestBehaviorLogging

void HelmIvP::requestBehaviorLogging()
{
  // Request that the Logger make a copy of the Behavior file
  set<string>::const_iterator p;
  for(p=m_bhv_files.begin(); p!=m_bhv_files.end(); p++) {
    string filename = *p;
    string command  = "COPY_FILE_REQUEST = " + filename;
    m_Comms.Notify("PLOGGER_CMD", command);
  }
}

//--------------------------------------------------------
// Procedure: onStartUp()

bool HelmIvP::OnStartUp()
{
  cleanup();
  if(!m_info_buffer)
    m_info_buffer = new InfoBuffer;

  MOOSTrace("The IvP Helm (pHelmIvP) is starting....\n");

  // Look for ownship name first - a "global" variable in the 
  // Configuration file. 
  m_ownship = "unknown";
  if(!m_MissionReader.GetValue("Community", m_ownship)) {
    MOOSTrace("Vehicle (Community) Name not provided\n");
    return(false);
  }
  
  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
    
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string line  = *p;
    string param = stripBlankEnds(toupper(biteString(line, '=')));
    string value = stripBlankEnds(line);

    if(param == "BEHAVIORS")
      addBehaviorFile(value);
    else if(param == "OK_SKEW") {
      if(toupper(value) == "ANY") {
	m_skews_matter = false;
	m_ok_skew = -1;
      }
      else {
	double dval = atof(value.c_str());
	if(isNumber(value) && (dval >= 0)) {
	  m_skews_matter = true;
	  m_ok_skew = dval;
	}
	else {
	  MOOSTrace("Improper OK_SKEW value: %s\n", line.c_str());
	  return(false);
	}
      }
    }
    else if(param == "VERBOSE") {
      value = tolower(value);
      if(value == "true")  value = "verbose";
      if(value == "false") value = "terse";
      if((value!="verbose") && (value!="terse") && (value!="quiet")) {
	MOOSTrace("Improper VERBOSE value: %s\n", value.c_str());
	return(false);
      }
      m_verbose = value;
    }
    else if((param == "ACTIVE_START") || (param == "START_ENGAGED"))
      setBooleanOnString(m_has_control, value);
    else if(param == "ALLOW_DISENGAGE") 
      setBooleanOnString(m_allow_override, value);
    else if(param == "DISENGAGE_ON_ALLSTOP")
      setBooleanOnString(m_disengage_on_allstop, value);

    else if(param == "NODE_SKEW") {
      string vname = stripBlankEnds(biteString(value, ','));
      string skew  = stripBlankEnds(value);
      double dskew = atof(skew.c_str());
      if((vname != "") && (skew != "") && isNumber(skew))
	m_node_skews[vname] = dskew;
    }
    else if(param == "DOMAIN") {
      bool ok = handleDomainEntry(value);
      if(!ok) {
	MOOSTrace("Improper Domain Spec: %s\n", value.c_str());
	return(false);
      }
    }

    else if(param == "OTHER_OVERRIDE_VAR") {
      if(!strContainsWhite(value))
	m_additional_override = value;
    }
  }
    
  m_hengine = new HelmEngine(m_ivp_domain, m_info_buffer);

  Populator_BehaviorSet *p_bset;
  p_bset = new Populator_BehaviorSet(m_ivp_domain, m_info_buffer);
  p_bset->loadEnvVarDirectories("IVP_BEHAVIOR_DIRS", true);
  m_bhv_set = p_bset->populate(m_bhv_files);

#if 0
  cout << "==================================================" << endl;
  p_bset->printBehaviorSpecs();
  cout << "==================================================" << endl;
#endif

  if(m_bhv_set == 0) {
    MOOSTrace("NULL Behavior Set \n");
    return(false);
  }

  // Set the "ownship" parameter for all behaviors
  for(int i=0; i<m_bhv_set->getCount(); i++) {
    m_bhv_set->getBehavior(i)->IvPBehavior::setParam("us", m_ownship);
    m_bhv_set->getBehavior(i)->onSetParamComplete();
  }
  if(m_verbose == "verbose")
    m_bhv_set->print();
  
  string mode_set_string_description = m_bhv_set->getModeSetDefinition();

  handleInitialVarsPhase1();
  registerVariables();
  requestBehaviorLogging();

  m_Comms.Notify("IVPHELM_DOMAIN",  domainToString(m_ivp_domain));
  m_Comms.Notify("IVPHELM_MODESET", mode_set_string_description);

  return(true);
}

//--------------------------------------------------------------------
// Procedure: addBehaviorFile
//     Notes: More then one behavior file can be used to 
//            configure the helm. Perhaps if there is a
//            common set of behaviors like safety behaviors.

void HelmIvP::addBehaviorFile(string filename)
{
  m_bhv_files.insert(filename);
}

//--------------------------------------------------------------------
// Procedure: handleDomainEntry

bool HelmIvP::handleDomainEntry(const string& g_entry)
{
  string entry = stripBlankEnds(g_entry);
  entry = findReplace(entry, ',', ':');

  vector<string> svector = parseString(entry, ':');
  unsigned int vsize = svector.size();
  if((vsize < 4) || (vsize > 5))
    return(false);

  string dname = svector[0];
  double dlow  = atof(svector[1].c_str());
  double dhgh  = atof(svector[2].c_str());
  int    dcnt  = atoi(svector[3].c_str());
  
  if(vsize == 5) {
    svector[4] = tolower(svector[4]);
    if(svector[4] == "optional")
      m_optional_var[dname] = true;
    else
      m_optional_var[dname] = false;
  }

  bool ok = m_ivp_domain.addDomain(dname.c_str(), dlow, dhgh, dcnt);
  return(ok);
}


//--------------------------------------------------------------------
// Procedure: detectRepeatOnKey
// Notes: When the helm posts a VarDataPair it may be posted with the
//        understanding that subsequent posts are disallowed if the
//        value has not changed. To indicate this, the VarDataPair sets
//        its KEY field. 
//        The helm may be configured to allow subsequent posts be made
//        even if the value has not changed, even if the VarDataPair has
//        set its key field. 
//        This process determines if a given variable should be allowed
//        to re-post, given the values in m_outgoing_timestamp, and 
//        m_outgoing_repinterval.

bool HelmIvP::detectRepeatOnKey(const string& key)
{
  // If no interval has been declared for this variable, repeat fails.
  double interval = 0;
  map<string, double>::iterator p;
  p = m_outgoing_repinterval.find(key);
  if(p==m_outgoing_repinterval.end())
    return(false);
  else
    interval = p->second;
  
  if(interval <= 0)
    return(false);

  // If no interval has been declared for this variable, repeat fails.
  double timestamp = 0;
  p = m_outgoing_timestamp.find(key);
  if(p == m_outgoing_timestamp.end())
    return(false);
  else
    timestamp = p->second;

  int    random_range = 100;
  double elapsed_time = m_curr_time - timestamp;
  if(elapsed_time < interval) {
    int rand_int = rand() % random_range;
    double threshold = (1 / interval) * random_range; 
    if(rand_int < threshold)
      return(true);
    else
      return(false);
  }
  return(false);
}

//--------------------------------------------------------------------
// Procedure: detectChangeOnKey
//   Purpose: To determine if the given key-value pair is unique 
//            against the last key-value posting.

bool HelmIvP::detectChangeOnKey(const string& key, const string& value)
{
  if(key == "")
    return(true);
  map<string, string>::iterator p;
  p = m_outgoing_strings.find(key);
  if(p == m_outgoing_strings.end()) {
    m_outgoing_strings[key] = value;
    return(true);
  }
  else {
    string old_value = p->second;
    if(old_value == value)
      return(false);
    else {
      m_outgoing_strings[key] = value;
      return(true);
    }
  }
}

//--------------------------------------------------------------------
// Procedure: detectChangeOnKey
//   Purpose: To determine if the given key-value pair is unique 
//            against the last key-value posting.

bool HelmIvP::detectChangeOnKey(const string& key, double value)
{
  if(key == "")
    return(true);
  map<string, double>::iterator p;
  p = m_outgoing_doubles.find(key);
  if(p == m_outgoing_doubles.end()) {
    m_outgoing_doubles[key] = value;
    return(true);
  }
  else {
    double old_value = p->second;
    if(old_value == value)
      return(false);
    else {
      m_outgoing_doubles[key] = value;
      return(true);
    }
  }
}

//--------------------------------------------------------------------
// Procedure: postAllStop
//   Purpose: Post zero-values to all decision variables. 
//  
//   CLEAR
//   ALLSTOP_MESSAGE
//       MANUAL_OVERRIDE
//       Some other allstop message triggered from a helm report
//       Ditto

void HelmIvP::postAllStop(string msg)
{
  if(msg == "")
    msg = "ManualOverride";

  // If nothing has changed, no need to do anything.
  if(msg == m_allstop_msg)
    return;

  // If the incoming message is not "ManualOverride" make this the
  // current allstop message no matter what.
  if(msg != "ManualOverride")
    m_allstop_msg = msg;
  // If it is indeed a manual override message, we only overwrite 
  // the current message if the current message was the "clear" message.
  else {
    if(tolower(m_allstop_msg == "clear"))
      m_allstop_msg = msg;
    else
      return;
  }

  MOOSDebugWrite("pHelmIvP AllStop: " + m_allstop_msg);
  m_Comms.Notify("IVPHELM_ALLSTOP", m_allstop_msg);

  if(tolower(m_allstop_msg) == "clear")
    return;

  // Post all the Decision Variable Results
  unsigned int j, dsize = m_ivp_domain.size();
  for(j=0; j<dsize; j++) {
    string domain_var = m_ivp_domain.getVarName(j);
    string post_alias = "DESIRED_"+ toupper(domain_var);
    if(post_alias == "DESIRED_COURSE")
      post_alias = "DESIRED_HEADING";    
    m_Comms.Notify(post_alias, 0.0);
  }
}

//--------------------------------------------------------------------
// Procedure: processNodeReport
//   Purpose: 

bool HelmIvP::processNodeReport(const string& report)
{
  string x_val, dep_val, spd_val, hdg_val, vname, raw_vname;
  string y_val, utc_val, lat_val, long_val;
  
  vector<string> svector = parseString(report, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string left  = stripBlankEnds(biteString(svector[i], '='));
    string right = stripBlankEnds(svector[i]);
    bool right_isnum = isNumber(right);

    if(left=="NAME") {
      raw_vname = right;
      vname = toupper(right);
    }
    else if((left == "UTC_TIME") && right_isnum)
      utc_val = right;
    else if((left == "MOOS_TIME") && right_isnum) 
      utc_val = right;
    else if((left == "X") && right_isnum)
      x_val = right;
    else if((left == "Y") && right_isnum)
      y_val = right;
    else if((left == "LAT") && right_isnum)
      lat_val = right;
    else if((left == "LON") && right_isnum)
      long_val = right;
    else if((left == "SPD") && right_isnum)
      spd_val = right;
    else if((left == "HDG") && right_isnum)
      hdg_val = right;
    else if((left == "DEPTH") && right_isnum) 
      dep_val = right;
  }

  if((x_val=="") || (spd_val=="") || (utc_val=="") || (vname=="") ||
     (y_val=="") || (hdg_val=="") || (dep_val==""))
    return(false);

  m_info_buffer->setValue(vname+"_NAV_X", atof(x_val.c_str()));
  m_info_buffer->setValue(vname+"_NAV_Y", atof(y_val.c_str()));
  m_info_buffer->setValue(vname+"_NAV_SPEED", atof(spd_val.c_str()));
  m_info_buffer->setValue(vname+"_NAV_HEADING", atof(hdg_val.c_str()));
  m_info_buffer->setValue(vname+"_NAV_DEPTH", atof(dep_val.c_str()));
  m_info_buffer->setValue(vname+"_NAV_LAT", atof(lat_val.c_str()));
  m_info_buffer->setValue(vname+"_NAV_LONG", atof(long_val.c_str()));

  double timestamp = atof(utc_val.c_str());
  
  // Apply a skew if one is declared for this vehicle
  map<string, double>::iterator p = m_node_skews.find(vname);
  if(p == m_node_skews.end())
    p = m_node_skews.find(raw_vname);
  if(p != m_node_skews.end()) 
    timestamp += p->second;
  // Done applying the skew

  m_info_buffer->setValue(vname+"_NAV_UTC", timestamp);

  return(true);
}
