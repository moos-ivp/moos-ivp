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
#include "AngleUtils.h"
#include "IvPProblem.h"
#include "HelmReport.h"
#include "stringutil.h"
#include "Populator_BehaviorSet.h"

using namespace std;

//--------------------------------------------------------------------
// Procedure: Constructor

HelmIvP::HelmIvP()
{
  m_has_control    = false;
  m_allstop_posted = false;
  m_bhv_set        = 0;
  m_hengine        = 0;
  m_info_buffer    = new InfoBuffer;
  m_iteration      = 0;
  m_last_heartbeat = 0;
  m_curr_time_updated = false;

  m_node_report_vars.push_back("NODE_REPORT");
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

    if((msg.m_sKey == "MOOS_MANUAL_OVERIDE") || 
       (msg.m_sKey == "MOOS_MANUAL_OVERRIDE")) {
      if(MOOSStrCmp(msg.m_sVal, "FALSE")) {
	m_has_control = true;
	MOOSTrace("\n");
	MOOSDebugWrite("pHelmIvP Control Is On");
      }
    }
    else if(msg.m_sKey == "RESTART_HELM") {
      OnStartUp();
      MOOSTrace("\n");
      MOOSDebugWrite("pHelmIvP Has Been Re-Started");
    }
    else if(vectorContains(m_node_report_vars, msg.m_sKey)) {
      bool ok = processNodeReport(msg.m_sVal);
      if(!ok)
	m_Comms.Notify("BHV_WARNING", "Unhandled NODE REPORT");
    }
    else
      updateInfoBuffer(msg);
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
  
  // If the curr_time is not set in the OnNewMail function (possibly 
  // because there was no mail in the queue), set the current time now.
  if(!m_curr_time_updated) {
    m_curr_time = MOOSTime();
    m_info_buffer->setCurrTime(m_curr_time);
  }

  // Now we're done addressing whether the curr_time is updated on this
  // iteration. It was done either in this function or in onNewMail().
  // Now set m_curr_time_updated=false to reflect the ingoing state for
  // the next iteration.
  m_curr_time_updated = false;

  if(!m_has_control) {
    postAllStop();
    return(false);
  }

  HelmReport helm_report;
  helm_report = m_hengine->determineNextDecision(m_bhv_set, m_curr_time);

  m_iteration = helm_report.getIteration();
 

  postBehaviorMessages();
  //postModeMessages();
  //postDefaultVariables();

  m_Comms.Notify("HELM_IPF_COUNT", helm_report.getOFNUM());
  m_Comms.Notify("IVPHELM_SUMMARY", helm_report.getReportAsString());
 
  if(helm_report.getHalted()) {
    m_has_control = false;
    bool ok;
    string bhv_error_str = m_info_buffer->sQuery("BHV_ERROR", ok);
    if(!ok)
      bhv_error_str = " - unknown - ";
    MOOSDebugWrite("BHV_ERROR: " + bhv_error_str);
    MOOSDebugWrite("pHelmIvP Control Is Off: All Dec-Vars set to ZERO");
  }
  
  int dsize = m_ivp_domain.size();

  // First make sure the HelmEngine has made a decision for all 
  // non-optional variables - otherwise declare an incomplete decision.
  // If a complete decision is not generated, this does not mean the 
  // helm relinquishes control, only that an all-stop is posted on 
  // this iteration.
  bool complete_decision = false;
  if(m_has_control && helm_report.getOFNUM() > 0) {
    complete_decision = true;
    for(int i=0; i<dsize; i++) {
      string domain_var = m_ivp_domain.getVarName(i);
      if(!helm_report.hasDecision(domain_var))
	if(m_optional_var[domain_var] == false) {
	  complete_decision = false;
	  string s1 = "ERROR! No decision for mandatory var - " + domain_var;
	  string s2 = "pHelmIvP Control is Off: All Dec-Vars set to ZERO";
	  MOOSDebugWrite(s1);
	  MOOSDebugWrite(s2);
	}
    }
  }
  
  if(!m_has_control || !complete_decision)
    postAllStop();
  else {  // Post all the Decision Variable Results
    m_allstop_posted = false;
    for(int j=0; j<dsize; j++) {
      string domain_var = m_ivp_domain.getVarName(j);
      string post_alias = "DESIRED_"+ toupper(domain_var);
      if(post_alias == "DESIRED_COURSE")
	post_alias = "DESIRED_HEADING";
      double domain_val = helm_report.getDecision(domain_var);
      m_Comms.Notify(post_alias, domain_val);
    }
  }
  
  double create_cpu  = helm_report.getCreateTime();
  m_Comms.Notify("CREATE_CPU", create_cpu);

  double loop_cpu  = helm_report.getLoopTime();
  m_Comms.Notify("LOOP_CPU", loop_cpu);

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
  
  unsigned int i, bhv_cnt = m_bhv_set->size();
  m_Comms.Notify("BCOUNT", bhv_cnt);
  m_Comms.Notify("TBCOUNT", m_bhv_set->getTCount());
  m_Comms.Notify("HITER", m_iteration);
  for(i=0; i < bhv_cnt; i++) {
    string bhv_descriptor = m_bhv_set->getDescriptor(i);
    vector<VarDataPair> mvector = m_bhv_set->getMessages(i);
    unsigned int msize = mvector.size();

    string bhv_postings_summary;
  
    //for(int j=0; j<msize; j++) {
    for(int j=0; (j<2)&&(j<msize); j++) {
      VarDataPair msg = mvector[j];

      string var   = msg.get_var();
      string sdata = msg.get_sdata();
      double ddata = msg.get_ddata();
      string mkey  = msg.get_key();
      bool   msg_is_string = msg.is_string();

      bool key_change = true;
      bool key_repeat = true;

#if 0
      bool key_repeat = detectRepeatOnKey(var);
      if(sdata == "")
	key_change = detectChangeOnKey(mkey, ddata);
      else
	key_change = detectChangeOnKey(mkey, sdata);
#endif     
 
#if 0     
      // Possibly augment the postings_summary
      if(key_change) {
	// If first var-data tuple then tack header info on front
	if(bhv_postings_summary == "") {
	  bhv_postings_summary += bhv_descriptor;
	  bhv_postings_summary += "$@!$";
	  bhv_postings_summary += intToString(m_iteration);
	}
	// Handle BHV_IPF tuples separately below
	if(var != "BHV_IPF") {
	  string pair_printable = msg.getPrintable();
	  bhv_postings_summary += "$@!$";
	  bhv_postings_summary += pair_printable;
	}
      }
#endif
      // If posting an IvP Function, mux first and post the parts.
      // if(var == "BHV_IPF") { // mikerb
      if(0) {
	string id = bhv_descriptor + intToString(m_iteration);
	vector<string> svector = IvPFunctionToVector(sdata, id, 2000);
	for(unsigned int k=0; k<svector.size(); k++)
	  m_Comms.Notify("BHV_IPF", svector[k], bhv_descriptor);
      }
      // Otherwise just post to the DB directly.
      else {
	if(msg_is_string) {
	  //m_info_buffer->setValue(var, sdata); // mikerb
	  if(key_change || key_repeat) {
	    //string aux = intToString(m_iteration) + ":" + bhv_descriptor;
	    //m_Comms.Notify(var, sdata, aux);

	    m_Comms.Notify("LEN", sdata.length());
	    m_Comms.Notify(var, sdata);
	  }
	}
	else {
	  //m_info_buffer->setValue(var, ddata); // mikerb
	  if(key_change || key_repeat) {
	    //string aux = intToString(m_iteration) + ":" + bhv_descriptor;
	    m_Comms.Notify(var, ddata);
	    //m_Comms.Notify(var, ddata, aux);
	  }
	}
      }
    }
    //mikerb
    //    if(bhv_postings_summary != "")
    //  m_Comms.Notify("IVPHELM_POSTINGS", bhv_postings_summary, bhv_descriptor);
  }
  // Determine if the list of state-space related variables for
  // the behavior-set has changed and post the new set if so.

  return;  //mikerb
  bool changed = m_bhv_set->updateStateSpaceVars();
  if(changed) {
    string state_vars = m_bhv_set->getStateSpaceVars();
    m_Comms.Notify("IVPHELM_STATEVARS", state_vars);
  }
}

//------------------------------------------------------------
// Procedure: postModeMessages()
//      Note: Run once after every iteration of control loop.

void HelmIvP::postModeMessages()
{
  if(!m_bhv_set) 
    return;
  
  vector<VarDataPair> mvector = m_bhv_set->getModeVarDataPairs();
  int msize = mvector.size();

  for(int j=0; j<msize; j++) {
    VarDataPair msg = mvector[j];
    
    string var   = msg.get_var();
    string sdata = msg.get_sdata();
    double ddata = msg.get_ddata();
    string mkey  = msg.get_key();

    bool key_change = true;
    if(msg.is_string()) {
      //key_change = detectChangeOnKey(mkey, sdata);
      if(key_change)
	m_Comms.Notify(var, sdata);
    }
    else {
      //key_change = detectChangeOnKey(mkey, ddata);
      if(key_change)
	m_Comms.Notify(var, ddata);
    }
  }
}

//------------------------------------------------------------
// Procedure: postIntialVariables()

void HelmIvP::postInitialVariables()
{
  if(!m_bhv_set) return;

  vector<VarDataPair> mvector = m_bhv_set->getInitialVariables();
  int msize = mvector.size();
  for(int j=0; j<msize; j++) {
    VarDataPair msg = mvector[j];
    
    string var   = stripBlankEnds(msg.get_var());
    string sdata = stripBlankEnds(msg.get_sdata());
    double ddata = msg.get_ddata();
    
    if(sdata != "") {
      m_info_buffer->setValue(var, sdata);
      m_Comms.Notify(var, sdata);
    }
    else {
      m_info_buffer->setValue(var, ddata);
      m_Comms.Notify(var, ddata);
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
    for(unsigned int j=0; j<mvector.size(); j++) {
      VarDataPair msg = mvector[j];
      message_vars.push_back(msg.get_var());
    }
  }

  // Phase 2 - Examine each of the default_messages and determine
  // for each, if the variable was contained in one of the behavior
  // messages for this iteration. If not, then post the default.
  vector<VarDataPair> dvector = m_bhv_set->getDefaultVariables();
  int dsize = dvector.size();
  for(int j=0; j<dsize; j++) {
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

  bool changed = true;
  // changed = detectChangeOnKey("IVPHELM_ENGAGED", engaged_status);
  if(changed || time_for_a_new_heartbeat)
    m_Comms.Notify("IVPHELM_ENGAGED", engaged_status);
}

//------------------------------------------------------------
// Procedure: postCharStatus()

void HelmIvP::postCharStatus()
{
  if(m_has_control)
    MOOSTrace("$");
  else
    MOOSTrace("*");
}

//------------------------------------------------------------
// Procedure: updateInfoBuffer()

bool HelmIvP::updateInfoBuffer(CMOOSMsg &msg)
{
  string key = msg.m_sKey;
  string community = msg.m_sOriginatingCommunity;
  string sdata = msg.m_sVal;
  double ddata = msg.m_dfVal;
  char   mtype = msg.m_cDataType;

  // Special Case: We get Yaw/Heading from pNav, but pNav 
  // doesn't post HEADING, so assume for now its same as YAW
  if(key == "NAV_YAW") {
    double heading = (ddata*-180.0)/3.1415926;
    m_info_buffer->setValue("NAV_HEADING", heading);
    m_info_buffer->setValue("NAV_YAW", ddata);
    return(true);
  }

  if(mtype == MOOS_DOUBLE) {
    return(m_info_buffer->setValue(key, ddata));
  }
  else if(mtype == MOOS_STRING) {
    return(m_info_buffer->setValue(key, sdata));
  }
  return(false);
}

//------------------------------------------------------------
// Procedure: OnConnectToServer()

bool HelmIvP::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void HelmIvP::registerVariables()
{
  m_Comms.Register("MOOS_MANUAL_OVERIDE", 0);
  m_Comms.Register("MOOS_MANUAL_OVERRIDE", 0);
  m_Comms.Register("RESTART_HELM", 0);

  m_Comms.Register("NAV_SPEED", 0);
  m_Comms.Register("NAV_HEADING", 0);
  m_Comms.Register("NAV_PITCH", 0);
  m_Comms.Register("NAV_DEPTH", 0);

  // Register for node report variables, e.g., AIS_REPORT, NODE_REPORT
  unsigned int i, vsize = m_node_report_vars.size();
  for(i=0; i<vsize; i++) 
    m_Comms.Register(m_node_report_vars[i], 0);
  
  if(m_bhv_set) {
    vector<string> info_vars = m_bhv_set->getInfoVars();
    for(unsigned int j=0; j<info_vars.size(); j++)
      m_Comms.Register(info_vars[j], 0.0);
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
    else if(param == "DOMAIN")
      handleDomainEntry(value);
  }
    
  m_hengine = new HelmEngineBeta(m_ivp_domain, m_info_buffer);

  Populator_BehaviorSet *p_bset;
  p_bset = new Populator_BehaviorSet(m_ivp_domain, m_info_buffer);
  p_bset->loadEnvVarDirectories("IVP_BEHAVIOR_DIRS", true);
  m_bhv_set = p_bset->populate(m_bhv_files);

  if(m_bhv_set == 0)
    return(false);

  // Set the "ownship" parameter for all behaviors
  for(int i=0; i<m_bhv_set->getCount(); i++) {
    m_bhv_set->getBehavior(i)->IvPBehavior::setParam("us", m_ownship);
    m_bhv_set->getBehavior(i)->onSetParamComplete();
  }
  
  string mode_set_string_description = m_bhv_set->getModeSetDefinition();

  postInitialVariables();
  registerVariables();

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
  int vsize = svector.size();
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
// Procedure: postAllStop
//   Purpose: Post zero-values to all decision variables. 

void HelmIvP::postAllStop()
{
  if(m_allstop_posted)
    return;

  // Post all the Decision Variable Results
  int dsize = m_ivp_domain.size();
  for(int j=0; j<dsize; j++) {
    string domain_var = m_ivp_domain.getVarName(j);
    string post_alias = "DESIRED_"+ toupper(domain_var);
    if(post_alias == "DESIRED_COURSE")
      post_alias = "DESIRED_HEADING";    
    m_Comms.Notify(post_alias, 0.0);
  }

  m_allstop_posted = true;
}

//--------------------------------------------------------------------
// Procedure: processNodeReport
//   Purpose: 

bool HelmIvP::processNodeReport(const string& report)
{
  string x_val, dep_val, spd_val, hdg_val, vname;
  string y_val, utc_val, lat_val, long_val;
  
  vector<string> svector = parseString(report, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string left  = stripBlankEnds(biteString(svector[i], '='));
    string right = stripBlankEnds(svector[i]);
    bool right_isnum = isNumber(right);

    if(left=="NAME")
      vname = toupper(right);
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

  m_info_buffer->setValue(vname+"_NAV_UTC", atof(utc_val.c_str()));
  m_info_buffer->setValue(vname+"_NAV_X", atof(x_val.c_str()));
  m_info_buffer->setValue(vname+"_NAV_Y", atof(y_val.c_str()));
  m_info_buffer->setValue(vname+"_NAV_SPEED", atof(spd_val.c_str()));
  m_info_buffer->setValue(vname+"_NAV_HEADING", atof(hdg_val.c_str()));
  m_info_buffer->setValue(vname+"_NAV_DEPTH", atof(dep_val.c_str()));
  m_info_buffer->setValue(vname+"_NAV_LAT", atof(lat_val.c_str()));
  m_info_buffer->setValue(vname+"_NAV_LONG", atof(long_val.c_str()));
  
  return(true);
}

