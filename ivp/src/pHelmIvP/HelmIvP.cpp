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

// Trivial change

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

#if 1
  #define USE_NEW_POPULATOR
#endif

#ifdef USE_NEW_POPULATOR
   #include "Populator_BehaviorSet2.h"
#else
   #include "Populator_BehaviorSet.h"
#endif


using namespace std;

//--------------------------------------------------------------------
// Procedure: Constructor

HelmIvP::HelmIvP()
{
  m_has_control   = false;
  m_allow_overide = true;
  m_bhv_set       = 0;
  m_hengine       = 0;
  m_info_buffer   = new InfoBuffer;
  m_verbose       = "terse";
  m_iteration     = 0;
  m_ok_skew       = 360; 
  m_skews_matter  = true;
  m_warning_count = 0;

  // The refresh vars handle the occasional clearing of the m_outgoing
  // maps. These maps will be cleared when MOOS mail is received for the
  // variable given by m_refresh_var. The user can set minimum interval
  // between refreshes so the helm retains some control over refresh rate.
  // Motivated by the need for a viewer handling geometric postings from
  // behaviors. The new arrival of a viewer into the MOOS community can 
  // request a refresh and then get new geometry mail to process.
  m_refresh_var      = "HELM_MAP_CLEAR";
  m_refresh_interval = 10.0;
  m_refresh_pending  = false;
  m_refresh_time     = 0;
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
  if(m_info_buffer) {
    delete(m_info_buffer);
    m_info_buffer = 0;
  }
  if(m_bhv_set) {
    delete(m_bhv_set);
    m_bhv_set = 0;
  }
  if(m_hengine) {
    delete(m_hengine);
    m_hengine = 0;
  }
  IvPDomain new_domain;
  m_ivp_domain = new_domain;
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
  double curr_time = MOOSTime();
  m_info_buffer->setCurrTime(curr_time);

  // Clear the delta vectors in the info_buffer here, before any
  // new MOOS Mail is applied to the info buffer.
  m_info_buffer->clearDeltaVectors();

  MOOSMSG_LIST::reverse_iterator p;
  
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;
    double dfT;

    msg.IsSkewed(MOOSTime(),&dfT);
    
    if(!m_skews_matter || (fabs(dfT) < m_ok_skew)) {
      if(msg.m_sKey == "MOOS_MANUAL_OVERIDE") {
	if(MOOSStrCmp(msg.m_sVal, "FALSE")) {
	  m_has_control = true;
	  MOOSTrace("\n");
	  MOOSDebugWrite("pHelmIvP Control Is On");
	  m_info_buffer->setCurrTime(curr_time);
	}
	else {
	  if(m_allow_overide) {
	    m_has_control = false;
	    MOOSTrace("\n");
	    MOOSDebugWrite("pHelmIvP Control Is Off");
	  }
	}
      }
      else if(msg.m_sKey == "HELM_VERBOSE") {
	if(msg.m_sVal == "verbose")
	  m_verbose = "verbose";
	else if(msg.m_sVal == "quiet")
	  m_verbose = "quiet";
	else
	  m_verbose = "terse";
      }
      else if(msg.m_sKey == "RESTART_HELM") {
	OnStartUp();
	MOOSTrace("\n");
	MOOSDebugWrite("pHelmIvP Has Been Re-Started");
      }
      else if(msg.m_sKey == m_refresh_var) {
	m_refresh_pending = true;
      }
      else
	updateInfoBuffer(msg);
    }
    else {
      // Add the variable name inthe out put?
      MOOSTrace("\n pHelmIvP ignores MOOS msg due to skew");
      MOOSDebugWrite("\n pHelmIvP ignores MOOS msg due to skew");
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
  postCharStatus();

  if(!m_has_control)
    return(false);

  // The curr_time is set in *both* the OnNewMail and Iterate functions.
  // In the OnNewMail function so the most up-to-date time is available
  // when processing mail.
  // In the Iterate method to ensure behaviors are not iterated with an
  // un-initialized timestamp on startup, and in case there is no Mail 
  double curr_time = MOOSTime();
  m_info_buffer->setCurrTime(curr_time);

  HelmReport helm_report;
  helm_report = m_hengine->determineNextDecision(m_bhv_set, curr_time);

  m_iteration = helm_report.getIteration();
  if(m_verbose == "verbose") {
    MOOSTrace("\n\n\n\n");
    MOOSTrace("Iteration: %d", m_iteration);
    MOOSTrace("  ******************************************\n");
    MOOSTrace("Helm Summary  ---------------------------\n");
    vector<string> svector = helm_report.getMsgs();
    for(unsigned int i=0; i<svector.size(); i++)
      MOOSTrace("%s\n", svector[i].c_str());
  }
  
  // Check if refresh conditions are met - potentiall clear outgoing maps.
  // This will result in all behavior postings being posted to the MOOSDB
  // on the current iteration.
  if(m_refresh_pending && ((curr_time-m_refresh_time) > m_refresh_interval)) {
    m_outgoing_strings.clear();
    m_outgoing_doubles.clear();
    m_refresh_time = curr_time;
    m_refresh_pending = false;
  }

  registerNewVariables();
  postBehaviorMessages();
  postDefaultVariables();

  // Should be called after postBehaviorMessages() where warnings
  // are detected and the count is incremented.
  helm_report.setWarningCount(m_warning_count);

  m_Comms.Notify("HELM_IPF_COUNT", helm_report.getOFNUM());
  m_Comms.Notify("IVPHELM_SUMMARY", helm_report.getReportAsString());
 
  if(m_verbose == "verbose") {
    MOOSTrace("(End) Iteration: %d", m_iteration);
    MOOSTrace("  ******************************************\n");
  }

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
  // non-optional variables - otherwise declare has_control = false.
  // If HelmEngine hasn't made *any* decisions, indicated by OFNUM
  // of zero, then a lack of decision on a non-optional var is ok.
  if(m_has_control && helm_report.getOFNUM() > 0) {
    for(int i=0; i<dsize; i++) {
      string domain_var = m_ivp_domain.getVarName(i);
      if(!helm_report.hasDecision(domain_var))
	if(m_optional_var[domain_var] == false) {
	  m_has_control = false;
	  string s1 = "ERROR! No decision for mandatory var - " + domain_var;
	  string s2 = "pHelmIvP Control is Off: All Dec-Vars set to ZERO";
	  MOOSDebugWrite(s1);
	  MOOSDebugWrite(s2);
	}
    }
  }

  // Post all the Decision Variable Results
  for(int j=0; j<dsize; j++) {
    string domain_var = m_ivp_domain.getVarName(j);
    string post_alias = "DESIRED_"+ toupper(domain_var);
    if(post_alias == "DESIRED_COURSE")
      post_alias = "DESIRED_HEADING";
    if(m_has_control) {
      double domain_val = helm_report.getDecision(domain_var);
      m_Comms.Notify(post_alias, domain_val);
    }
    else
      m_Comms.Notify(post_alias, 0.0);
  }

  double create_cpu  = helm_report.getCreateTime();
  m_Comms.Notify("CREATE_CPU", create_cpu);

  double loop_cpu  = helm_report.getLoopTime();
  m_Comms.Notify("LOOP_CPU", loop_cpu);

  if(!m_allow_overide)
    m_has_control = true;

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
  if(!m_bhv_set) return;
  
  if(m_verbose == "verbose") {
    MOOSTrace("Behavior Report ---------------------------------\n");
  }

  int bhv_cnt = m_bhv_set->getCount();
  for(int i=0; i < bhv_cnt; i++) {
    vector<VarDataPair> mvector = m_bhv_set->getMessages(i);
    string bhv_descriptor = m_bhv_set->getDescriptor(i);
    int msize = mvector.size();

    string bhv_postings_summary;
  
    for(int j=0; j<msize; j++) {
      VarDataPair msg = mvector[j];

      string var   = msg.get_var();
      string sdata = msg.get_sdata();
      double ddata = msg.get_ddata();
      string mkey  = msg.get_key();

      bool key_change = true;
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
	  bhv_postings_summary += intToString(m_iteration);
	}
	// Handle BHV_IPF tuples separately below
	if(var != "BHV_IPF") {
	  string pair_printable = msg.getPrintable();
	  bhv_postings_summary += "$@!$";
	  bhv_postings_summary += pair_printable;
	}
      }


      // If posting an IvP Function, mux first and post the parts.
      if(var == "BHV_IPF") {
	string id = bhv_descriptor + intToString(m_iteration);
	vector<string> svector = IvPFunctionToVector(sdata, id, 2000);
	for(unsigned int k=0; k<svector.size(); k++)
	  m_Comms.Notify("BHV_IPF", svector[k]);
      }
      // Otherwise just post to the DB directly.
      else {
	if(sdata != "") {
	  m_info_buffer->setValue(var, sdata);
	  if(key_change)
	    m_Comms.Notify(var, sdata);
	}
	else {
	  m_info_buffer->setValue(var, ddata);
	  if(key_change)
	    m_Comms.Notify(var, ddata);
	}
      }
    }

    if(bhv_postings_summary != "")
      m_Comms.Notify("IVPHELM_POSTINGS", bhv_postings_summary);
  }

  // Determine if the list of state-space related variables for
  // the behavior-set has changed and post the new set if so.

  bool changed = m_bhv_set->updateStateSpaceVars();
  if(changed) {
    string state_vars = m_bhv_set->getStateSpaceVars();
    m_Comms.Notify("IVPHELM_STATEVARS", state_vars);
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
  requestBehaviorLogging();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void HelmIvP::registerVariables()
{
  m_Comms.Register("MOOS_MANUAL_OVERIDE", 0);
  m_Comms.Register("RESTART_HELM", 0);
  m_Comms.Register("HELM_VERBOSE", 0);

  m_Comms.Register("NAV_YAW", 0);
  m_Comms.Register("NAV_SPEED", 0);
  m_Comms.Register("NAV_HEADING", 0);
  m_Comms.Register("NAV_PITCH", 0);
  m_Comms.Register("NAV_DEPTH", 0);
  m_Comms.Register(m_refresh_var, 0);

  if(m_bhv_set) {
    vector<string> info_vars = m_bhv_set->getInfoVars();
    for(unsigned int j=0; j<info_vars.size(); j++) {
      if(m_verbose == "verbose")
	MOOSTrace("Registering for: %s\n", info_vars[j].c_str());
      m_Comms.Register(info_vars[j], 0.0);
    }
  }
}

//------------------------------------------------------------
// Procedure: registerNewVariables

void HelmIvP::registerNewVariables()
{
  if(m_bhv_set) {
    vector<string> info_vars = m_bhv_set->getNewInfoVars();
    for(unsigned int j=0; j<info_vars.size(); j++) {
      if(m_verbose == "verbose")
	MOOSTrace("Registering for: %s\n", info_vars[j].c_str());
      m_Comms.Register(info_vars[j], 0.0);
    }
  }
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
  cerr << "HelmIvP::OnStartUp()" << endl;

  cleanup();
  if(!m_info_buffer)
    m_info_buffer = new InfoBuffer;

  MOOSTrace("pHelmIvP starting....\n");

  // Look for ownship name first - a "global" variable in the 
  // Configuration file. 
  m_ownship = "unknown";
  if(!m_MissionReader.GetValue("Community", m_ownship)) {
    MOOSTrace("Vehicle (Community) Name not provided\n");
    return(false);
  }
  
  string simulator;
  if(!m_MissionReader.GetValue("Simulator",simulator)) {
    MOOSTrace("Simulation flag not provided\n");
    //return(false);
  }
  vector<string> must_bhvs;
  must_bhvs.push_back("BHV_OpRegion");


  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
    
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine     = *p;
    string sVarName  = MOOSChomp(sLine, "=");
    sVarName = toupper(sVarName);
    sLine    = stripBlankEnds(sLine);
    
    if(MOOSStrCmp(sVarName, "BEHAVIORS"))
      addBehaviorFile(sLine);
    
    if(MOOSStrCmp(sVarName, "OK_SKEW")) {
      string str = toupper(sLine);
      if(str == "ANY") {
	m_skews_matter = false;
	m_ok_skew = -1;
      }
      else {
	double dval = atof(str.c_str());
	if(isNumber(str) && (dval >= 0)) {
	  m_skews_matter = true;
	  m_ok_skew = dval;
	}
	else {
	  MOOSTrace("Improper OK_SKEW value: %s\n", sLine.c_str());
	  return(false);
	}
      }
    }
    
    else if(MOOSStrCmp(sVarName, "VERBOSE")) {
      sLine = tolower(sLine);
      if(sLine == "true")  sLine = "verbose";
      if(sLine == "false") sLine = "terse";
      m_verbose = sLine;
      if((m_verbose!="verbose")&&(m_verbose!="terse")&&(m_verbose!="quiet")) {
	MOOSTrace("Improper VERBOSE value: %s\n", sLine.c_str());
	return(false);
      }
    }

    else if(MOOSStrCmp(sVarName, "ACTIVE_START")) {
      sLine = tolower(sLine);
      if(sLine == "true") {
	m_has_control = true;
	m_allow_overide = false;
      }
    }

    else if(MOOSStrCmp(sVarName, "DOMAIN")) {
      bool ok = handleDomainEntry(sLine);
      if(!ok) {
	MOOSTrace("Improper Domain Spec: %s\n", sLine.c_str());
	return(false);
      }
    }
  }
    
  m_hengine = new HelmEngine(m_ivp_domain);

#ifdef USE_NEW_POPULATOR
  Populator_BehaviorSet2 p_bset(m_ivp_domain, m_info_buffer);
  p_bset.loadEnvVarDirectories("IVP_BEHAVIOR_DIRS", true);

#else
  Populator_BehaviorSet p_bset(m_ivp_domain, m_info_buffer);
#endif

  m_bhv_set = p_bset.populate(m_bhv_files);
  
  if(m_bhv_set == 0) {
    MOOSTrace("NULL Behavior Set \n");
    return(false);
  }

  // Set the "ownship" parameter for all behaviors
  for(int i=0; i<m_bhv_set->getCount(); i++)
    m_bhv_set->getBehavior(i)->setParam("us", m_ownship);
  if(m_verbose == "verbose")
    m_bhv_set->print();
  
  postInitialVariables();
  registerVariables();
  requestBehaviorLogging();

  m_Comms.Notify("IVPHELM_DOMAIN", domainToString(m_ivp_domain));

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
