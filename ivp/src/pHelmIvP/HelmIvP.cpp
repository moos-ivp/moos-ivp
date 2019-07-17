/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmIvP.cpp                                          */
/*    DATE: Oct 12th 2004                                        */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <unistd.h>
#include <iterator>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "HelmIvP.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "MBTimer.h" 
#include "FunctionEncoder.h" 
#include "IvPProblem.h"
#include "HelmReport.h"
#include "Populator_BehaviorSet.h"
#include "LifeEvent.h"
#include "NodeRecord.h"
#include "NodeRecordUtils.h"
#include "ACTable.h"

using namespace std;

//--------------------------------------------------------------------
// Procedure: Constructor

HelmIvP::HelmIvP()
{
  m_allstop_msg    = "";
  m_bhv_set        = 0;
  m_hengine        = 0;
  m_info_buffer    = 0;
  m_verbose        = "verbose";
  m_verbose_reset  = false;
  m_helm_iteration = 0;
  m_bhv_count      = 0;
  m_bhv_count_ever = 0;
  m_ok_skew        = 60; 
  m_skews_matter   = true;
  m_helm_start_time = 0;
  m_curr_time      = 0;
  m_start_time     = 0;
  m_no_decisions   = 0;

  // The m_has_control correlates to helm status
  m_has_control     = false;
  m_helm_status     = "PARK";

  // If a vehicle is configured to be a standby helm, having control
  // also requires the standby_mode to be false.
  m_standby_helm           = false;
  m_standby_threshold      = 0;
  m_standby_last_heartbeat = 0;

  m_allow_override  = true;
  m_park_on_allstop = false;

  m_ibuffer_curr_time_updated = false;

  m_rejournal_requested = true;
  m_reset_post_pending  = false;

  m_prev_total_completed = 0;
  
  m_init_vars_ready  = false;
  m_init_vars_done   = false;

  m_helm_start_posted  = false;
  m_hold_apps_all_seen = true; // will init to false if apps specified
  
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
  if(m_info_buffer)  delete(m_info_buffer);
  if(m_bhv_set)      delete(m_bhv_set);
  if(m_hengine)      delete(m_hengine);
}

//--------------------------------------------------------------------
// Procedure: handlePrepareRestart

void HelmIvP::handlePrepareRestart()
{
  m_ibuffer_curr_time_updated = false;

#if 0  // To be implemented (8/30/15)
  if(m_reset_pending == "type2") {
    if(m_info_buffer)
      delete(m_info_buffer);
    m_info_buffer = 0;
  }
#endif

  if(m_bhv_set)
    delete(m_bhv_set);
  m_bhv_set = 0;

  if(m_hengine)
    delete(m_hengine);
  m_hengine = 0;

  m_bhv_files.clear();
  m_outgoing_key_strings.clear();

  m_outgoing_key_doubles.clear();
  m_outgoing_timestamp.clear();
  m_outgoing_sval.clear();
  m_outgoing_dval.clear();
  m_outgoing_iter.clear();
  m_outgoing_bhv.clear();
  m_outgoing_repinterval.clear();

  m_ivp_domain = IvPDomain();

  // Indicate that we to potentially re-post the defer init vars
  m_init_vars_done = false;

  // Indicate we want to post confirmation of the restart after done
  m_reset_post_pending = true;
}

//--------------------------------------------------------------------
// Procedure: OnNewMail()

bool HelmIvP::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);
  // If this helm has been disabled, don't bother processing mail.
  if(helmStatus() == "DISABLED")
    return(true);

  // The info_buffer curr_time is synched to the helm's curr_time  
  // in *both* the OnNewMail and Iterate functions. Because:
  // (1) The OnNewMail function needs the most up-to-date time 
  //     when processing mail.
  // (2) The Iterate method must ensure behaviors aren't iterated w/ an
  //     un-initialized timestamp on startup, in case there is no Mail 
  // (3) The info_buffer should reflect "time since last update" of 
  //     exactly zero for updates made on this iteration. If info_buffer
  //     is synched in *both" OnNewMail and Iterate, the "time since 
  //     last update" will be non-zero.

  m_info_buffer->setCurrTime(m_curr_time);
  m_ibuffer_curr_time_updated = true;

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    
    string moosvar   = msg.GetKey();
    string sval      = msg.GetString();
    string source    = msg.GetSource();
    string community = msg.GetCommunity();
    
    double skew_time;
    msg.IsSkewed(m_curr_time, &skew_time);
    
    
    double reg_skew_time = m_curr_time - m_var_reg_time[moosvar];
    if(reg_skew_time < skew_time)
      skew_time = reg_skew_time;

    if(moosvar=="MOOS_MANUAL_OVERIDE") {
      string skew_info = "var=" + moosvar + ":";
      skew_info += "matter="+boolToString(m_skews_matter);
      skew_info += ", skew=" + doubleToString(skew_time,2);
      reportEvent(skew_info);
    }

    if(m_skews_matter && (fabs(skew_time) > m_ok_skew)) {
      // If mail not posted by this helm from this community, flag the skew.
      bool posted_by_this_helm = true;
      if((source != GetAppName()) || (m_host_community != community))
	posted_by_this_helm = false;
      bool exception = false;
      if(moosvar == "HELM_MAP_CLEAR")
	exception = true;
      if(!posted_by_this_helm && !exception) {
	string skew_msg = "Helm ignores MOOS msg due to skew: " + moosvar;
	skew_msg += " Source=" + source + " Skew=" + doubleToString(skew_time,2);
	reportRunWarning(skew_msg);
	skew_msg += " curr_time: " + doubleToString(m_curr_time,3);
	Notify("IVPHELM_SKEW", skew_msg);
	continue;
      }
    }

    if((moosvar =="MOOS_MANUAL_OVERIDE") || 
       (moosvar =="MOOS_MANUAL_OVERRIDE") ||
       ((moosvar == m_additional_override) && (moosvar != ""))) {
      if(toupper(sval) == "FALSE") {
	m_has_control = true;
	helmStatusUpdate();
      }
      else if(toupper(sval) == "TRUE") {
	if(m_allow_override) {
	  m_has_control = false;
	  helmStatusUpdate();
	}
      }
      updateInfoBuffer(msg);
    }
    else if(moosvar == "IVPHELM_STATE") {
      m_init_vars_ready = true;
      // Check if this heartbeat message is from another helm
      if(msg.GetSource() != GetAppName()) {
	// First handle things considering we are a standby helm
	if(m_standby_helm) {
	  // If we're still in standby mode, just note the hearbeat
	  // of the primary helm. We take over when that gets old.
	  if(helmStatus() == "STANDBY") {
	    m_standby_last_heartbeat = m_curr_time;
	  }
	  // If the standby helm has taken over already, the incoming
	  // report must be from tardy primary helm finally responding.
	  // In this case we just want to republish things that are
	  // normally not published due to being the same as previous
	  // publications.
	  else {
	    m_refresh_pending = true;
	    m_rejournal_requested = true;
	    postAllStop();
	  }
	}
	// If we're a primary helm, then yield to the other helm
	// if their helm status is anything but standby
	else if(!m_standby_helm && (sval != "STANDBY")) {
	  postAllStop("DisabledByStandbyHelm");
	  helmStatusUpdate("DISABLED");
	}
      }
    }
    else if((moosvar == "DB_CLIENTS") && !m_hold_apps_all_seen)
      checkHoldOnApps(sval);
    else if(moosvar == "RESTART_HELM") {
      // type 1: kill and re-spawn new behaviors - don't touch info_buffer
      // tyep 2: kill and re-spawn new behaviors and re-init the info_buffer
      if((sval == "type1") || (sval == "type2")) {
	m_reset_pending = sval;
	handlePrepareRestart();
	OnStartUp();
	reportRunWarning("pHelmIvP has been re-started!!");
	if(m_verbose == "terse") 
	  cout << endl << "pHelmIvP Has Been Re-Started" << endl;
      }
    }
    else if(moosvar == "IVPHELM_REJOURNAL")
      m_rejournal_requested = true;
    else if(vectorContains(m_node_report_vars, moosvar)) {
      bool ok = processNodeReport(sval);
      if(!ok)
	reportRunWarning("Unhandled NODE_REPORT");
      updateInfoBuffer(msg);
    }
    else if(moosvar == m_refresh_var) {
      m_refresh_pending = true;
    }
    else
      updateInfoBuffer(msg);
  }

  if(helmStatus() == "STANDBY")
    checkForTakeOver();
  return(true);
}

//--------------------------------------------------------------------
// Procedure: Iterate()

bool HelmIvP::Iterate()
{
  AppCastingMOOSApp::Iterate();
  handleHelmStartMessages();

  double cpu_load = GetCPULoad();
  Notify("IVPHELM_CPU", cpu_load);

  helmStatusUpdate();
  postHelmStatus();
  if(!helmStatusEnabled()) {
    m_info_buffer->clearDeltaVectors();
    AppCastingMOOSApp::PostReport();
    return(true);
  }
  
  postCharStatus();
  
  if(m_init_vars_ready && !m_init_vars_done)
    handleInitialVarsPhase2();

  // If the info_buffer curr_time is not synched in the OnNewMail function
  //  (possibly because there was no new mail), synch the current time now.
  if(!m_ibuffer_curr_time_updated) 
    m_info_buffer->setCurrTime(m_curr_time);
  if(m_start_time == 0)
    m_start_time = m_curr_time;

  // Now we're done addressing whether the info_buffer curr_time is synched 
  // on this iteration. It was done either in this function or in onNewMail().
  // Now set m_ibuffer_curr_time_updated=false to reflect the ingoing state for
  // the next iteration.
  m_ibuffer_curr_time_updated = false;

  if(!m_has_control) {
    postAllStop("ManualOverride");
    AppCastingMOOSApp::PostReport();
    return(false);
  }

  // We don't try to get a helm decision for the first second upon startup
  // Gives the info_buffer time to receive critical mail, reduce warnings.
  if((m_curr_time - m_helm_start_time) < 1)
    return(true);

  // If the helm has been restarted, make a single post confirming this now
  // that the helm is back in the business of making decisions.
  if(m_reset_post_pending) {
    Notify("IVPHELM_RESTARTED", "true");
    m_reset_post_pending = false;
  }
  
  m_helm_report = m_hengine->determineNextDecision(m_bhv_set, m_curr_time);

#if 0 // mikerb jan 2016 debug
  vector<string> msgs = m_helm_report.getMsgs();
  m_helm_iteration = m_helm_report.getIteration();
  for(unsigned int i=0; i<msgs.size(); i++) {
    Notify("IVPHELM_REPORT_MSG", msgs[i], uintToString(m_helm_iteration));
  }
#endif

  vector<string> update_results = m_helm_report.getUpdateResults();
  for(unsigned int i=0; i<update_results.size(); i++)
    Notify("IVPHELM_UPDATE_RESULT", update_results[i]);

  
  m_helm_iteration = m_helm_report.getIteration();

  // Check if refresh conditions are met - perhaps clear outgoing maps.
  // This will result in all behavior postings being posted to the MOOSDB
  // on the current iteration.
  if(m_refresh_pending && 
     ((m_curr_time-m_refresh_time) > m_refresh_interval)) {
    m_outgoing_key_strings.clear();
    m_outgoing_key_doubles.clear();
    m_refresh_time = m_curr_time;
    m_refresh_pending = false;
  }

  registerNewVariables();
  postModeMessages();
  postBehaviorMessages();
  postLifeEvents();
  postDefaultVariables();

  // Should be called after postBehaviorMessages() where warnings
  // are detected and the count is incremented.
  unsigned int warning_count = getWarningCount("all"); 
  m_helm_report.setWarningCount(warning_count);
  
  string report;
  if(m_rejournal_requested) {    
    report = m_helm_report.getReportAsString();
    m_rejournal_requested = false;
  }
  else
    report = m_helm_report.getReportAsString(m_prev_helm_report); 
  
  Notify("IVPHELM_SUMMARY", report);
  Notify("IVPHELM_IPF_CNT", m_helm_report.getOFNUM());
  Notify("IVPHELM_TOTAL_PCS_FORMED", m_helm_report.getTotalPcsFormed());
  Notify("IVPHELM_TOTAL_PCS_CACHED", m_helm_report.getTotalPcsCached());

  string bhvs_active_list = m_helm_report.getActiveBehaviors(false);
  if(m_bhvs_active_list != bhvs_active_list) {
    Notify("IVPHELM_BHV_ACTIVE", bhvs_active_list); 
    m_bhvs_active_list = bhvs_active_list;
  }
  string bhvs_running_list = m_helm_report.getRunningBehaviors(false);
  if(m_bhvs_running_list != bhvs_running_list) {
    Notify("IVPHELM_BHV_RUNNING", bhvs_running_list); 
    m_bhvs_running_list = bhvs_running_list;
  }
  string bhvs_idle_list = m_helm_report.getIdleBehaviors(false);
  if(m_bhvs_idle_list != bhvs_idle_list) {
    Notify("IVPHELM_BHV_IDLE", bhvs_idle_list); 
    m_bhvs_idle_list = bhvs_idle_list;
  }

  m_prev_helm_report = m_helm_report;

   string allstop_msg = "clear";

  if(m_helm_report.getHalted())
    allstop_msg = "BehaviorError";
  else if(m_helm_report.getOFNUM() == 0)
    allstop_msg = "NothingToDo";
  
  // First make sure the HelmEngine has made a decision for all 
  // non-optional variables - otherwise declare an incomplete decision.
  if(allstop_msg == "clear") {
    bool   complete_decision = true;
    string missing_dec_vars;
    unsigned int i, dsize = m_ivp_domain.size();
    for(i=0; i<dsize; i++) {
      string domain_var = m_ivp_domain.getVarName(i);
      if(!m_helm_report.hasDecision(domain_var)) {
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
      Notify("BHV_ERROR", allstop_msg); 
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
      if(m_helm_report.hasDecision(domain_var)) {
	double domain_val = m_helm_report.getDecision(domain_var);
	Notify(post_alias, domain_val);
      }
    }
  }
  
  Notify("IVPHELM_CREATE_CPU", m_helm_report.getCreateTime());
  Notify("IVPHELM_LOOP_CPU", m_helm_report.getLoopTime());

  bool changed_update_vars = m_bhv_set->refreshMapUpdateVars();
  if(changed_update_vars)
    Notify("IVPHELM_UPDATEVARS", m_bhv_set->getUpdateVarSummary());
  
  if(allstop_msg != "clear")
    if(m_allow_override && m_park_on_allstop)
      m_has_control = false;

  // Clear the delta vectors now that all behavior have had the 
  // chance to consume delta info.
  m_info_buffer->clearDeltaVectors();

  AppCastingMOOSApp::PostReport();
  return(true);
}
  
//------------------------------------------------------------
// Procedure: postBehaviorMessages()
//      Note: Run once after every iteration of control loop.
//            Each behavior has the chance to produce their 
//            own message to be posted in both the info_buffer
//            and to the MOOSDB. 

void HelmIvP::postBehaviorMessages()
{
  if(!m_bhv_set) 
    return;
  
  // Added Aug 02 2012 to support enhanced warning reporting
  vector<string> config_warnings = m_bhv_set->getWarnings();
  for(unsigned int i=0; i<config_warnings.size(); i++) {
    reportRunWarning(config_warnings[i]);
    Notify("IVPHELM_BHVSET_WARNING", uintToString(i) + ":" + config_warnings[i]);
  }
  m_bhv_set->clearWarnings();
  
  unsigned bhv_count      = m_bhv_set->size();
  if(bhv_count != m_bhv_count) {
    Notify("IVPHELM_BHV_CNT", bhv_count);
    m_bhv_count = bhv_count;
  }

  unsigned bhv_count_ever = m_bhv_set->getTCount();
  if(bhv_count_ever != m_bhv_count_ever) {
    Notify("IVPHELM_BHV_CNT_EVER", bhv_count_ever);
    m_bhv_count_ever = bhv_count_ever;
  }
    
  Notify("IVPHELM_ITER", m_helm_iteration);
  for(unsigned i=0; i < bhv_count; i++) {
    string bhv_descriptor = m_bhv_set->getDescriptor(i);
    vector<VarDataPair> mvector = m_bhv_set->getMessages(i);

    unsigned int j, msize = mvector.size();
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
 
      // Include warnings in this application's appcast
      if(var == "BHV_WARNING") {
	//if(mkey != "retract")
	if(!strEnds(mkey, "retract"))
	  reportRunWarning("BHV_WARNING: " + sdata);
	else 
	  retractRunWarning("BHV_WARNING: " + sdata);
      }
      else if(var == "BHV_ERROR")
	reportRunWarning("BHV_ERROR: " + sdata);

      // If posting an IvP Function, mux first and post the parts.
      if(var == "BHV_IPF") { // mikerb

#if 0
	Notify("BHV_IPF", sdata);
#endif
#if 1
	string id = bhv_descriptor + "^" + intToString(m_helm_iteration);
	vector<string> svector = IvPFunctionToVector(sdata, id, 2000);
	for(unsigned int k=0; k<svector.size(); k++)
	  Notify("BHV_IPF", svector[k], bhv_descriptor);
#endif
      }
      // Otherwise just post to the DB directly.
      else {
	if(msg_is_string) {
	  m_info_buffer->setValue(var, sdata);
	  if(key_change || key_repeat) {
	    string aux = intToString(m_helm_iteration) + ":" + 
	      bhv_descriptor;
	    Notify(var, sdata, aux);
	    m_outgoing_timestamp[var] = m_curr_time;
	    m_outgoing_iter[var] = m_helm_iteration;
	    m_outgoing_sval[var] = sdata;
	    m_outgoing_bhv[var]  = bhv_descriptor;
	  }
	}
	else {
	  m_info_buffer->setValue(var, ddata);
	  if(key_change || key_repeat) {
	    string aux = intToString(m_helm_iteration) + ":" + 
	      bhv_descriptor;
	    Notify(var, ddata, aux);
	    m_outgoing_timestamp[var] = m_curr_time;
	    m_outgoing_iter[var] = m_helm_iteration;
	    m_outgoing_dval[var] = ddata;
	    m_outgoing_bhv[var]  = bhv_descriptor;
	  }
	}
      }
    }
  }
  // Determine if the list of state-space related variables for
  // the behavior-set has changed and post the new set if so.

  bool changed = m_bhv_set->updateStateSpaceVars();
  if(changed) {
    string state_vars = m_bhv_set->getStateSpaceVars();
    Notify("IVPHELM_STATEVARS", state_vars);
  }

  string helm_iter     = uintToString(m_helm_iteration);

  string compl_pending = boolToString(m_bhv_set->getCompletedPending());
  if(compl_pending != m_prev_compl_pending)
    Notify("IVPHELM_COMPLETED_PENDING", compl_pending, helm_iter); 
  m_prev_compl_pending = compl_pending;
  
  unsigned int total_completed = m_bhv_set->removeCompletedBehaviors();
  if(total_completed != m_prev_total_completed)
    Notify("IVPHELM_COMPLETED_COUNT", total_completed, helm_iter);
  m_prev_total_completed = total_completed;
}

//------------------------------------------------------------
// Procedure: buildReport
//      Note: A virtual function of the AppCastingMOOSApp superclass, conditionally 
//            invoked if either a terminal or appcast report is needed.

bool HelmIvP::buildReport()
{
  if(m_helm_status == "MALCONFIG") {
    // Only need to send this message to the terminal once. After 
    // doing so, turn off further reports to the terminal.
    if(m_iteration > 1)
      m_term_reporting = false;
    m_msgs << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    m_msgs << "!!                                             !!" << endl;
    m_msgs << "!!  The helm is in the MALCONFIG state due to  !!" << endl;
    m_msgs << "!!  unresolved configuration warnings.         !!" << endl;
    m_msgs << "!!                                             !!" << endl;
    m_msgs << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    return(true);
  }

  list<string> summary = m_helm_report.formattedSummary(m_curr_time);
  list<string>::iterator p;
  for(p=summary.begin(); p!=summary.end(); p++)
    m_msgs << *p << endl;

  string hold_on_status = "none";
  if(m_hold_on_app_name.size() > 0) {
    if(m_hold_apps_all_seen)
      hold_on_status = "satisfied";
    else
      hold_on_status = "waiting";
  }  
  m_msgs << "Hold-On-Apps: " << hold_on_status << endl;
  
  ACTable actab(5);
  actab << "Variable | Behavior | Time | Iter | Value";
  actab.addHeaderLines();
  actab.setColumnMaxWidth(4,55);
  actab.setColumnNoPad(4);
  map<string, double>::iterator q;
  for(q=m_outgoing_timestamp.begin(); q!=m_outgoing_timestamp.end(); q++) {
    string varname = q->first;
    string value = "???";
    if(m_outgoing_sval.count(varname))
      value = m_outgoing_sval[varname];
    else if(m_outgoing_dval.count(varname))
      value = doubleToStringX(m_outgoing_dval[varname]);
    double db_time = q->second - m_start_time;
    string timestamp = doubleToString(db_time,2);
    string bhv  = m_outgoing_bhv[varname];
    string iter = uintToString(m_outgoing_iter[varname]);
    actab << varname << bhv << timestamp << iter << value;
  }
  m_msgs << endl << endl;
  m_msgs << actab.getFormattedString();

  return(true);
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
    Notify("IVPHELM_LIFE_EVENT", str);
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
	Notify(var, sdata, "HELM_MODE");
    }
    else {
      key_change = detectChangeOnKey(mkey, ddata);
      if(key_change)
	Notify(var, ddata, "HELM_MODE");
    }
  }
}

//------------------------------------------------------------
// Procedure: handleHelmStartMessages
//      Note: In release 17.7.x this was only executed upon startup.
//            In later releases this is executed on startup and in
//            each iteration of the iterate loop, but it is also
//            conditioned on m_hold_apps_all_seen=true and
//            helm_start_posted=false. Therefore behavior startup
//            messages will only be posted once, and only after
//            any hold_on_apps have been detected in db_clients.

void HelmIvP::handleHelmStartMessages()
{
  if(!m_bhv_set) 
    return;

  // If posted already, don't post again
  if(m_helm_start_posted)
    return;

  // Make sure we have seen all apps identified as hold apps
  if(!m_hold_apps_all_seen)
    return;

  retractRunWarning("Not all HoldOnApps have been detected");
  m_helm_start_posted = true;
  
  vector<VarDataPair> helm_start_msgs = m_bhv_set->getHelmStartMessages();

  unsigned int j, msize = helm_start_msgs.size();
  for(j=0; j<msize; j++) {
    VarDataPair msg = helm_start_msgs[j];

    string var   = stripBlankEnds(msg.get_var());
    string sdata = stripBlankEnds(msg.get_sdata());
    double ddata = msg.get_ddata();

    if(!strContainsWhite(var)) {
      if(sdata != "") {
	m_info_buffer->setValue(var, sdata);
	Notify(var, sdata, "HELM_STARTUP_MSG");
      }
      else {
	m_info_buffer->setValue(var, ddata);
	Notify(var, ddata, "HELM_STARTUP_MSG");
      }
    }
  }
}

//------------------------------------------------------------
// Procedure: handleInitialVarsPhase1()
//   Purpose: Publish all the initial variables provided in the bhv
//            file, but not the ones marked "defer".

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
	  Notify(var, sdata, "HELM_VAR_INIT");
	}
	else {
	  m_info_buffer->setValue(var, ddata);
	  Notify(var, ddata, "HELM_VAR_INIT");
	}
      }
    }

  }
  registerSingleVariable("IVPHELM_STATE");
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
	Notify(var, sdata, "HELM_VAR_INIT");
      }
      else {
	m_info_buffer->setValue(var, ddata);
	Notify(var, ddata, "HELM_VAR_INIT");
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
  unsigned int i, bhv_cnt = m_bhv_set->size();
  for(i=0; i < bhv_cnt; i++) {
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
	Notify(var, sdata);
      }
      else {
	double ddata  = msg.get_ddata();
	m_info_buffer->setValue(var, ddata);
	Notify(var, ddata);
      }
    }
  }
}

//------------------------------------------------------------
// Procedure: setVerbosity()
//    Values: verbose values: "verbose", "terse", "quiet"
//
//            verbose: appcasts printed to the terminal
//                     AppCastingMOOSApp::m_term_reporting = true
//            terse:   Startup in plus heartbeat to terminal
//                     AppCastingMOOSApp::m_term_reporting = false
//            quiet:   Nothing to terminal
//                     AppCastingMOOSApp::m_term_reporting = false
//
//   pHelmIvP --noterm ~ --verbose=terse
//
//   command-line specs trump VERBOSE setting in MOOS block

bool HelmIvP::setVerbosity(const string& str)
{
  if(m_verbose_reset)
    return(true);

  string lstr = tolower(str);
  
  if((lstr == "verbose") || (lstr == "true"))
    return(true);
  else if((lstr == "terse") || (lstr == "false"))
    m_term_reporting = false;
  else if(lstr == "quiet")
    m_term_reporting = false;
  else
    return(false);

  m_verbose = lstr;
  m_verbose_reset = true;
  return(true);
}

//------------------------------------------------------------
// Procedure: postHelmStatus()
//    Values: Possible helm status values: STANDBY
//                                         PARK     (ENABLED)
//                                         DRIVE    (ENABLED)
//                                         DISABLED

void HelmIvP::postHelmStatus()
{
  string helm_status = helmStatus();

  // Was this originally a standby helm? If so indicate with '+'
  if(helmStatusEnabled() && m_standby_helm)
    helm_status += "+";

  // Post the helm status even if the value hasn't changed, 
  // unless the status is DISABLED and has been posted previously.
  if(helm_status != "DISABLED")
    Notify("IVPHELM_STATE", helm_status);
  else {
    bool changed = detectChangeOnKey("IVPHELM_STATE", helm_status);
    if(changed)
      Notify("IVPHELM_STATE", helm_status);
  }
}

//------------------------------------------------------------
// Procedure: postCharStatus()

void HelmIvP::postCharStatus()
{
  if((m_verbose == "quiet") || (m_verbose == "verbose"))
    return;

  if(helmStatus() == "DRIVE")
    cout << "$" << flush;
  else if(helmStatus() == "PARK")
    cout << "*" << flush;
  else if(helmStatus() == "STANDBY")
    cout << "#" << flush;
  else if(helmStatus() == "MALCONFIG")
    cout << "!" << flush;
  else if(helmStatus() == "DISABLED")
    cout << "X" << flush;
  else
    cout << "?" << flush;
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
  string moosvar  = msg.m_sKey;
  string src_aux  = msg.GetSourceAux();
  double msg_time = msg.GetTime();
  if(src_aux == "HELM_VAR_INIT")
    return(false);
    
  if(msg.IsDouble()) {
    return(m_info_buffer->setValue(moosvar, msg.GetDouble(), msg_time));
  }
  else if(msg.IsString()) {
    return(m_info_buffer->setValue(moosvar, msg.GetString(), msg_time));
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
  AppCastingMOOSApp::RegisterVariables();
  registerSingleVariable("DB_CLIENTS");
  registerSingleVariable("MOOS_MANUAL_OVERIDE");
  registerSingleVariable("MOOS_MANUAL_OVERRIDE");
  registerSingleVariable("RESTART_HELM");
  registerSingleVariable("IVPHELM_REJOURNAL");
  
  registerSingleVariable("NAV_SPEED");
  registerSingleVariable("NAV_HEADING");
  registerSingleVariable("NAV_DEPTH");
  registerSingleVariable(m_refresh_var);

  if(m_additional_override != "")
    registerSingleVariable(m_additional_override);

  // Register for node report variables, e.g., AIS_REPORT, NODE_REPORT
  unsigned int vsize = m_node_report_vars.size();
  for(unsigned int i=0; i<vsize; i++) 
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
  Register(varname, frequency);

  Notify("IVPHELM_REGISTER", varname);
  if(m_var_reg_time.count(varname) == 0)
    m_var_reg_time[varname] = m_curr_time;
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
    Notify("PLOGGER_CMD", command);
  }
}

//------------------------------------------------------------
// Procedure: checkForTakeOver

void HelmIvP::checkForTakeOver()
{
  if(helmStatus() != "STANDBY")
    return;
  
  if(m_standby_last_heartbeat == 0)
    return;

  double elapsed_time = m_curr_time - m_standby_last_heartbeat;
  if(elapsed_time > m_standby_threshold)
    helmStatusUpdate("ENABLED");
}

//--------------------------------------------------------
// Procedure: OnStartUp()

bool HelmIvP::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  Notify("PHELMIVP_PID", getpid());
    
  m_helm_start_time = m_curr_time;
  if(!m_info_buffer) {
    m_info_buffer = new InfoBuffer;
    m_info_buffer->setStartTime(m_helm_start_time);
  }
    
  bool bhv_dir_not_found_ok = false;
  // ownship xis name of MOOS community, set in AppCastingMOOSApp::OnStartUp()
  m_ownship = m_host_community;

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
    
  vector<string> behavior_dirs;

  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "BEHAVIORS")
      handled = addBehaviorFile(value);
    else if(param == "OK_SKEW") 
      handled = handleConfigSkewAny(value);
    else if(param == "VERBOSE") 
      handled = setVerbosity(value);
    else if(param == "ACTIVE_START")
      handled = setBooleanOnString(m_has_control, value);
    else if(param == "START_ENGAGED")
      handled = setBooleanOnString(m_has_control, value);
    else if((param == "START_INDRIVE") || (param == "START_IN_DRIVE"))
      handled = setBooleanOnString(m_has_control, value);
    else if(param == "HELM_ALIAS") 
      handled = setNonWhiteVarOnString(m_helm_alias, value);
    else if(param == "STANDBY") 
      handled = handleConfigStandBy(value);
    else if(param == "ALLOW_PARK") 
      handled = setBooleanOnString(m_allow_override, value);
    else if(param == "PARK_ON_ALLSTOP")
      handled = setBooleanOnString(m_park_on_allstop, value);
    else if(param == "NODE_SKEW") 
      handled = handleConfigNodeSkew(value);
    else if((param == "HOLD_ON_APP") || (param == "HOLD_ON_APPS"))
      handled = handleConfigHoldOnApp(value);
    else if(param == "DOMAIN")
      handled = handleConfigDomain(value);
    else if((param == "BHV_DIR_NOT_FOUND_OK") || (param == "BHV_DIRS_NOT_FOUND_OK"))
      handled = setBooleanOnString(bhv_dir_not_found_ok, value);
    else if((param == "IVP_BEHAVIOR_DIR") || (param == "IVP_BEHAVIOR_DIRS")) {
      behavior_dirs.push_back(value);
      handled = true;
    }
    else if(param == "OTHER_OVERRIDE_VAR") 
      handled = setNonWhiteVarOnString(m_additional_override, value);

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  // Check for Config Warnings first here after reading pHelmIvP block.
  if(getWarningCount("config") > 0) {
    helmStatusUpdate("MALCONFIG");
    m_has_control = false;
    return(true);
  }

  m_hengine = new HelmEngine(m_ivp_domain, m_info_buffer);

  Populator_BehaviorSet *p_bset;
  p_bset = new Populator_BehaviorSet(m_ivp_domain, m_info_buffer);
  p_bset->setBHVDirNotFoundOK(bhv_dir_not_found_ok);
  p_bset->setOwnship(m_ownship);
  unsigned int ksize = behavior_dirs.size();
  for(unsigned int k=0; k<ksize; k++)
    p_bset->addBehaviorDir(behavior_dirs[k]);
  
  m_bhv_set = p_bset->populate(m_bhv_files);

  vector<string> config_warnings = p_bset->getConfigWarnings();
  for(unsigned int k=0; k<config_warnings.size(); k++) 
    reportConfigWarning(config_warnings[k]);

  if(getWarningCount("config") > 0) {
    helmStatusUpdate("MALCONFIG");
    m_has_control = false;
    return(true);
  }

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
  unsigned int i, bsize = m_bhv_set->size();
  for(i=0; i<bsize; i++) {
    m_bhv_set->getBehavior(i)->IvPBehavior::setParam("us", m_ownship);
    m_bhv_set->getBehavior(i)->onSetParamComplete();
  }
  
  string mode_set_string_description = m_bhv_set->getModeSetDefinition();

  if((m_reset_pending == "") || (m_reset_pending == "type2"))
    handleInitialVarsPhase1();
  m_reset_pending = "";
  
  handleHelmStartMessages();
  registerVariables();
  requestBehaviorLogging();

  Notify("IVPHELM_DOMAIN",  domainToString(m_ivp_domain));
  Notify("IVPHELM_MODESET", mode_set_string_description);
  if(m_helm_alias != "")
    Notify("IVPHELM_ALIAS", m_helm_alias);

  return(true);
}

//--------------------------------------------------------------------
// Procedure: handleConfigNodeSkew
//   Example: NODE_SKEW = macrura,43

bool HelmIvP::handleConfigNodeSkew(const string& given_value)
{
  string value = given_value;
  string vname = biteStringX(value, ',');
  string skew  = value;
  double dskew = atof(skew.c_str());
  if((vname != "") && (skew != "") && isNumber(skew)) {
    // If we've set it already, we should disallow and flag this.
    if(m_node_skews.count(vname) == 0) {
      m_node_skews[vname] = dskew;
      return(true);
    }
  }
  return(false);
}

//--------------------------------------------------------------------
// Procedure: handleConfigSkewAny

bool HelmIvP::handleConfigSkewAny(const string& value)
{
  bool handled = true;
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
    else
      handled = false;
  }
  return(handled);
}

//--------------------------------------------------------------------
// Procedure: handleConfigStandBy

bool HelmIvP::handleConfigStandBy(const string& value)
{
  bool handled = setPosDoubleOnString(m_standby_threshold, value);
  if(handled) {
    helmStatusUpdate("STANDBY");
    m_standby_helm = true;
  }

  return(handled);
}

//--------------------------------------------------------------------
// Procedure: handleConfigDomain

bool HelmIvP::handleConfigDomain(const string& g_entry)
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
// Procedure: handleConfigHoldOnApp()
//   Returns: true if all given apps are unique, and no app has a white
//            space character in its name.

bool HelmIvP::handleConfigHoldOnApp(string applist)
{
  vector<string> svector = parseString(applist, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string appname = stripBlankEnds(svector[i]);
    if(!vectorContains(m_hold_on_app_name, appname) &&
       !strContainsWhite(appname) &&
       (appname != "")) {
      m_hold_on_app_name.push_back(svector[i]);
      m_hold_on_app_seen.push_back(false);
      m_hold_apps_all_seen = false;
    }
    else
      return(false);
  }
  
  return(true);
}

//--------------------------------------------------------------------
// Procedure: checkHoldOnApps()
//     Notes: If any hold_on_apps have been specified, check DB_CLIENTS
//            for each app and note accordingly

void HelmIvP::checkHoldOnApps(string db_clients)
{
  if(m_hold_apps_all_seen)
    return;

  // parse the string of clients and ensure no white space on ends
  vector<string> clients = parseString(db_clients,',');
  for(unsigned int i=0; i<clients.size(); i++)
    clients[i] = stripBlankEnds(clients[i]);

  // For each hold app, check against the vector of db clients
  m_hold_apps_all_seen = true;
  for(unsigned int j=0; j<m_hold_on_app_name.size(); j++) {
    if(!m_hold_on_app_seen[j]) {
      if(vectorContains(clients, m_hold_on_app_name[j]))
	m_hold_on_app_seen[j] = true;
    }
    if(!m_hold_on_app_seen[j])
      m_hold_apps_all_seen = false;
  }

  reportRunWarning("Not all HoldOnApps have been detected");
}

//--------------------------------------------------------------------
// Procedure: addBehaviorFile
//     Notes: More then one behavior file can be used to 
//            configure the helm. Perhaps if there is a
//            common set of behaviors like safety behaviors.

bool HelmIvP::addBehaviorFile(string filename)
{
  if(m_bhv_files.count(filename) != 0)
    return(false);
  
  m_bhv_files.insert(filename);
  return(true);
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
//      Note: The assumption is that if a change is detected, the caller
//            will go ahead and make the posting. This assumption is 
//            reflected in the fact that the m_outgoing_key_strings map
//            is updated when a changed is detected.

bool HelmIvP::detectChangeOnKey(const string& key, const string& value)
{
  if(key == "")
    return(true);
  map<string, string>::iterator p;
  p = m_outgoing_key_strings.find(key);
  if(p == m_outgoing_key_strings.end()) {
    m_outgoing_key_strings[key] = value;
    return(true);
  }
  else {
    string old_value = p->second;
    if(old_value == value)
      return(false);
    else {
      // map is updated if changed detected.
      m_outgoing_key_strings[key] = value;
      return(true);
    }
  }
}

//--------------------------------------------------------------------
// Procedure: detectChangeOnKey
//   Purpose: To determine if the given key-value pair is unique 
//            against the last key-value posting.
//      Note: The assumption is that if a change is detected, the caller
//            will go ahead and make the posting. This assumption is 
//            reflected in the fact that the m_outgoing_key_strings map is
//            updated when a changed is detected.

bool HelmIvP::detectChangeOnKey(const string& key, double value)
{
  if(key == "")
    return(true);
  map<string, double>::iterator p;
  p = m_outgoing_key_doubles.find(key);
  if(p == m_outgoing_key_doubles.end()) {
    m_outgoing_key_doubles[key] = value;
    return(true);
  }
  else {
    double old_value = p->second;
    if(old_value == value)
      return(false);
    else {
      // map is updated if changed detected.
      m_outgoing_key_doubles[key] = value;
      return(true);
    }
  }
}

//--------------------------------------------------------------------
// Procedure: postAllStop
//   Purpose: Post zero-values to all decision variables. 
//  
//   clear
//   ALLSTOP_MESSAGE
//       MANUAL_OVERRIDE
//       Some other allstop message triggered from a helm report
//       Ditto

void HelmIvP::postAllStop(string msg)
{
  Notify("IVPHELM_ALLSTOP_DEBUG", msg);

  // Don't post all-stop info if the helm is on standby or disabled.
  if(!helmStatusEnabled())
    return;

  if(msg == m_allstop_msg) 
    return;

  // Interpret empty message as request to re-post the current status
  if(msg != "")
    m_allstop_msg = msg;

  if((msg == "NothingToDo") || strBegins(msg, "MissingDecVars"))
    m_no_decisions++;
  else
    m_no_decisions = 0;

  MOOSDebugWrite("pHelmIvP AllStop: " + m_allstop_msg);
  Notify("IVPHELM_ALLSTOP", m_allstop_msg);

  if(tolower(m_allstop_msg) == "clear")
    return;

  // Willing to hold off one iteration if simply no decision. To give helm
  // chance to transition between modes.
  if(m_no_decisions == 1) {
    m_allstop_msg = "IncompleteOrEmptyDecision";
    return;
  }

  // Post all the Decision Variable Results
  unsigned int j, dsize = m_ivp_domain.size();
  for(j=0; j<dsize; j++) {
    string domain_var = m_ivp_domain.getVarName(j);
    string post_alias = "DESIRED_"+ toupper(domain_var);
    if(post_alias == "DESIRED_COURSE")
      post_alias = "DESIRED_HEADING";    
    Notify(post_alias, 0.0);
  }
}

//--------------------------------------------------------------------
// Procedure: processNodeReport
//   Purpose: 

bool HelmIvP::processNodeReport(const string& report)
{
  NodeRecord new_record = string2NodeRecord(report);
  //if(!new_record.valid("name,x,y,time"))
  //  return(false);

  string raw_vname = new_record.getName();
  string vname = toupper(raw_vname);
  
  m_info_buffer->setValue(vname+"_NAV_X", new_record.getX());
  m_info_buffer->setValue(vname+"_NAV_Y", new_record.getY());
  m_info_buffer->setValue(vname+"_NAV_SPEED", new_record.getSpeed());
  m_info_buffer->setValue(vname+"_NAV_HEADING", new_record.getHeading());
  m_info_buffer->setValue(vname+"_NAV_DEPTH", new_record.getDepth());
  m_info_buffer->setValue(vname+"_NAV_LAT", new_record.getLat());
  m_info_buffer->setValue(vname+"_NAV_LONG", new_record.getLon());
  m_info_buffer->setValue(vname+"_NAV_GROUP", new_record.getGroup());
  m_info_buffer->setValue(vname+"_NAV_TYPE", new_record.getType());

  double timestamp = new_record.getTimeStamp();
  
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

//--------------------------------------------------------------------
// Procedure: helmStatusUpdate()
//   Purpose: 

void HelmIvP::helmStatusUpdate(const string& new_status)
{
  if((new_status == "DRIVE") || (new_status == "STANDBY")  ||
     (new_status == "PARK")  || (new_status == "DISABLED") ||
     (new_status == "MALCONFIG")) {
    m_helm_status = new_status;
  }
  
  // If otherwise just specified generally as "ENABLED", then 
  // set it, for now, to PARK and we'll figure out below
  // whether it should actually be PARK based on m_has_control
  else if(new_status == "ENABLED")
    m_helm_status = "PARK";
  
  // Now apply the knowledge expressed in m_has_control
  if((m_helm_status == "DRIVE") && !m_has_control)
    m_helm_status = "PARK";
  else if((m_helm_status == "PARK") && m_has_control)
    m_helm_status = "DRIVE";
}
  

//--------------------------------------------------------------------
// Procedure: helmStatusEnabled
//   Purpose: Convenience function, and just to be clear that the two
//            states, PARK and DRIVE represent an "enabled" 
//            helm, a helm that is neither on standby or disabled.

bool HelmIvP::helmStatusEnabled() const
{
  if((m_helm_status == "DRIVE") || (m_helm_status == "PARK"))
    return(true);
  return(false);
}

