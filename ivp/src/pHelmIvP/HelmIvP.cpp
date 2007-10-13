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
#include <math.h>
#include "HelmIvP.h"
#include "MBUtils.h"
#include "MBTimer.h" 
#include "FunctionEncoder.h" 
#include "AngleUtils.h"
#include "IvPProblem.h"
#include "Populator_BehaviorSet.h"
#include "HelmReport.h"

using namespace std;

//--------------------------------------------------------------------
// Procedure: Constructor

HelmIvP::HelmIvP()
{
  has_control   = false;
  allow_overide = true;
  bhv_set       = 0;
  hengine       = 0;
  info_buffer   = new InfoBuffer;
  ylog_flag     = true;
  verbose       = "terse";
  iteration     = 0;
  simulate      = false;
  ok_skew       = 360; 
  skews_matter  = true;
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
  if(info_buffer) {
    delete(info_buffer);
    info_buffer = 0;
  }
  if(bhv_set) {
    delete(bhv_set);
    bhv_set = 0;
  }
  if(hengine) {
    delete(hengine);
    hengine = 0;
  }
  IvPDomain new_domain;
  ivp_domain = new_domain;
}

//--------------------------------------------------------------------
// Procedure: OnNewMail()

bool HelmIvP::OnNewMail(MOOSMSG_LIST &NewMail)
{
  double curr_time = MOOSTime() - GetAppStartTime();
  info_buffer->setCurrTime(curr_time);

  MOOSMSG_LIST::reverse_iterator p;
  
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;
    double dfT;

    msg.IsSkewed(MOOSTime(),&dfT);
    
    if(!skews_matter || (fabs(dfT) < ok_skew)) {
      if(msg.m_sKey == "MOOS_MANUAL_OVERIDE") {
	if(MOOSStrCmp(msg.m_sVal, "FALSE")) {
	  has_control = true;
	  double start_time = GetAppStartTime();
	  MOOSTrace("\n");
	  MOOSDebugWrite("pHelmIvP Control Is On");
	  //info_buffer->set_val(ownship, "start_time", curr_time);
	  info_buffer->setCurrTime(curr_time);
	}
	else {
	  if(allow_overide) {
	    has_control = false;
	    MOOSTrace("\n");
	    MOOSDebugWrite("pHelmIvP Control Is Off");
	  }
	}
      }
      else if(msg.m_sKey == "HELM_VERBOSE") {
	if(msg.m_sVal == "verbose")
	  verbose = "verbose";
	else if(msg.m_sVal == "quiet")
	  verbose = "quiet";
	else
	  verbose = "terse";
      }
      else if(msg.m_sKey == "RESTART_HELM") {
	OnStartUp();
	MOOSTrace("\n");
	MOOSDebugWrite("pHelmIvP Has Been Re-Started");
      }
      else {
	bool result = updateInfoBuffer(msg);
      }
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

  if(!has_control)
    return(false);

  double curr_time = MOOSTime() - GetAppStartTime();

  //  info_buffer->setCurrTime(curr_time);

  HelmReport helm_report = hengine->determineNextDecision(bhv_set, curr_time);
  iteration = helm_report.getIteration();
  if(verbose == "verbose") {
    MOOSTrace("\n\n\n\n");
    MOOSTrace("Iteration: %d", iteration);
    MOOSTrace("  ******************************************\n");
    MOOSTrace("Helm Summary  ---------------------------\n");
    vector<string> svector = helm_report.getMsgs();
    for(int i=0; i<svector.size(); i++)
      MOOSTrace("%s\n", svector[i].c_str());
  }
  
  postBehaviorMessages();
  postDefaultVariables();

  m_Comms.Notify("HELM_IPF_COUNT", helm_report.getOFNUM());
  m_Comms.Notify("HELM_ACTIVE_BHV", helm_report.getActiveBehaviors());
  m_Comms.Notify("HELM_NONIDLE_BHV", helm_report.getNonIdleBehaviors());
 
  if(verbose == "verbose") {
    MOOSTrace("(End) Iteration: %d", iteration);
    MOOSTrace("  ******************************************\n");
  }

  if(helm_report.getHalted()) {
    has_control = false;
    bool ok;
    string bhv_error_str = info_buffer->sQuery("BHV_ERROR", ok);
    if(!ok)
      bhv_error_str = " - unknown - ";
    MOOSDebugWrite("BHV_ERROR: " + bhv_error_str);
    MOOSDebugWrite("pHelmIvP Control Is Off: All Dec-Vars set to ZERO");
  }
  
  int dsize = ivp_domain.size();

  // First make sure the HelmEngine has made a decision for all 
  // non-optional variables - otherwise declare has_control = false.
  // If HelmEngine hasn't made *any* decisions, indicated by OFNUM
  // of zero, then a lack of decision on a non-optional var is ok.
  if(has_control && helm_report.getOFNUM() > 0) {
    for(int i=0; i<dsize; i++) {
      string domain_var = ivp_domain.getVarName(i);
      if(!helm_report.hasDecision(domain_var))
	if(optional_var[domain_var] == false) {
	  has_control = false;
	  string s1 = "ERROR! No decision for mandatory var - " + domain_var;
	  string s2 = "pHelmIvP Control is Off: All Dec-Vars set to ZERO";
	  MOOSDebugWrite(s1);
	  MOOSDebugWrite(s2);
	}
    }
  }

  // Post all the Decision Variable Results
  for(int j=0; j<dsize; j++) {
    string domain_var = ivp_domain.getVarName(j);
    string post_alias = "DESIRED_"+ toupper(domain_var);
    if(post_alias == "DESIRED_COURSE")
      post_alias = "DESIRED_HEADING";
    if(has_control) {
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

  if(!allow_overide)
    has_control = true;

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
  if(!bhv_set) return;
  
  if(verbose == "verbose") {
    MOOSTrace("Behavior Report ---------------------------------\n");
  }
  
  int bhv_cnt = bhv_set->getCount();
  for(int i=0; i < bhv_cnt; i++) {
    vector<VarDataPair> mvector = bhv_set->getMessages(i);
    string bhv_descriptor = bhv_set->getDescriptor(i);
    int msize = mvector.size();
    for(int j=0; j<msize; j++) {
      VarDataPair msg = mvector[j];
      
      string var   = msg.get_var();
      string sdata = msg.get_sdata();
      double ddata = msg.get_ddata();

      // If error message indicate time of error. If ylog_flag
      // is true, also post info to MOOS_SYSTEM variable which
      // pLogger will write to the ylog file
      if((var == "BHV_ERROR") || (var == "BHV_WARNING")) {
	double now_time = MOOSTime() - GetAppStartTime();
	string str_time = doubleToString(now_time, 1);
	sdata = "(" + str_time + ")" + sdata;
	if(ylog_flag) {
	  string output = var + ": " + sdata;
	  m_Comms.Notify("MOOS_SYSTEM", output);
	}
      }

      // If posting an IvP Function, mux first and post the parts.
      if(var == "BHV_IPF") {
	string id = bhv_descriptor + intToString(iteration);
	vector<string> svector = IvPFunctionToVector(sdata, id, 2000);
	for(int i=0; i<svector.size(); i++)
	  m_Comms.Notify("BHV_IPF", svector[i]);
      }
      // Otherwise just post to the DB directly.
      else {
	string padvar = padString(var, 22, false);
	if(sdata != "") {
	  info_buffer->set_val(var, sdata);
	  m_Comms.Notify(var, sdata);
	  if(verbose == "verbose")
	    MOOSTrace("  %s %s \n", padvar.c_str(), sdata.c_str());
	}
	else {
	  info_buffer->set_val(var, ddata);
	  m_Comms.Notify(var, ddata);
	  if(verbose == "verbose") {
	    string dstr = dstringCompact(doubleToString(ddata));
	    MOOSTrace("  %s %s \n", padvar.c_str(), dstr.c_str());
	  }
	}
      }

    }
  }
}

//------------------------------------------------------------
// Procedure: postIntialVariables()

void HelmIvP::postInitialVariables()
{
  if(!bhv_set) return;

  vector<VarDataPair> mvector = bhv_set->getInitialVariables();
  int msize = mvector.size();
  for(int j=0; j<msize; j++) {
    VarDataPair msg = mvector[j];
    
    string var   = stripBlankEnds(msg.get_var());
    string sdata = stripBlankEnds(msg.get_sdata());
    double ddata = msg.get_ddata();
    
    if(sdata != "") {
      info_buffer->set_val(var, sdata);
      m_Comms.Notify(var, sdata);
    }
    else {
      info_buffer->set_val(var, ddata);
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
  if(!bhv_set) return;

  // Phase 1 - determine what variables were written to by the 
  // behaviors during the last iteration.
  vector<string> message_vars;
  int bhv_cnt = bhv_set->getCount();
  for(int i=0; i < bhv_cnt; i++) {
    vector<VarDataPair> mvector = bhv_set->getMessages(i);
    for(int j=0; j<mvector.size(); j++) {
      VarDataPair msg = mvector[j];
      message_vars.push_back(msg.get_var());
    }
  }

  // Phase 2 - Examine each of the default_messages and determine
  // for each, if the variable was contained in one of the behavior
  // messages for this iteration. If not, then post the default.
  vector<VarDataPair> dvector = bhv_set->getDefaultVariables();
  int dsize = dvector.size();
  for(int j=0; j<dsize; j++) {
    VarDataPair msg = dvector[j];
    string var = msg.get_var();
    if(!vectorContains(message_vars, var)) {
      if(msg.is_string()) {
	string sdata  = msg.get_sdata();
	info_buffer->set_val(var, sdata);
	m_Comms.Notify(var, sdata);
      }
      else {
	double ddata  = msg.get_ddata();
	info_buffer->set_val(var, ddata);
	m_Comms.Notify(var, ddata);
      }
    }
  }
}

//------------------------------------------------------------
// Procedure: postCharStatus()

void HelmIvP::postCharStatus()
{
  if(has_control) {
    if(verbose == "terse")
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
    info_buffer->set_val("NAV_HEADING", heading);
    info_buffer->set_val("NAV_YAW", ddata);
    return(true);
  }

  if(mtype == MOOS_DOUBLE) {
    return(info_buffer->set_val(key, ddata));
  }
  else if(mtype == MOOS_STRING) {
    return(info_buffer->set_val(key, sdata));
  }
  return(false);
}

//------------------------------------------------------------
// Procedure: OnConnectToServer()

bool HelmIvP::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));
  
  registerVariables();

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

  if(bhv_set) {
    vector<string> info_vars = bhv_set->getInfoVars();
    for(int j=0; j<info_vars.size(); j++) {
      if(verbose == "verbose")
	MOOSTrace("Registering for: %s\n", info_vars[j].c_str());
      m_Comms.Register(info_vars[j], 0.0);
    }
  }
}

//--------------------------------------------------------
// Procedure: onStartUp()

bool HelmIvP::OnStartUp()
{
  cleanup();
  if(!info_buffer)
    info_buffer = new InfoBuffer;

  MOOSTrace("pHelmIvP starting....\n");

  // Look for ownship name first - a "global" variable in the 
  // Configuration file. 
  ownship = "unknown";
  if(!m_MissionReader.GetValue("Community", ownship)) {
    MOOSTrace("Vehicle (Community) Name not provided\n");
    return(false);
  }
  
  string simulator;
  if(!m_MissionReader.GetValue("Simulator",simulator)) {
    MOOSTrace("Simulation flag not provided\n");
    //return(false);
  }
  if(MOOSStrCmp(simulator, "true")){
    simulate = true;
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
	skews_matter = false;
	ok_skew = -1;
      }
      else {
	double dval = atof(str.c_str());
	if(isNumber(str) && (dval >= 0)) {
	  skews_matter = true;
	  ok_skew = dval;
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
      verbose = sLine;
      if((verbose!="verbose")&&(verbose!="terse")&&(verbose!="quiet")) {
	MOOSTrace("Improper VERBOSE value: %s\n", sLine.c_str());
	return(false);
      }
    }

    else if(MOOSStrCmp(sVarName, "ACTIVE_START")) {
      sLine = tolower(sLine);
      if(sLine == "true") {
	has_control = true;
	allow_overide = false;
      }
    }

    else if(MOOSStrCmp(sVarName, "YLOG"))
      ylog_flag = (sLine == "true");
    
    else if(MOOSStrCmp(sVarName, "DOMAIN")) {
      bool ok = handleDomainEntry(sLine);
      if(!ok) {
	MOOSTrace("Improper Domain Spec: %s\n", sLine.c_str());
	return(false);
      }
    }
  }
    
  hengine = new HelmEngine(ivp_domain);

  Populator_BehaviorSet p_bset(ivp_domain, info_buffer);
  bhv_set = p_bset.populate(bhv_files);
  
  if(bhv_set == 0) {
    MOOSTrace("NULL Behavior Set \n");
    return(false);
  }

  // Set the "ownship" parameter for all behaviors
  for(int i=0; i<bhv_set->getCount(); i++)
    bhv_set->getBehavior(i)->setParam("us", ownship);
  if(verbose == "verbose")
    bhv_set->print();
  
  postInitialVariables();
  registerVariables();

  return(true);
}

//--------------------------------------------------------------------
// Procedure: addBehaviorFile
//     Notes: More then one behavior file can be used to 
//            configure the helm. Perhaps if there is a
//            common set of behaviors like safety behaviors.

void HelmIvP::addBehaviorFile(string filename)
{
  bhv_files.insert(filename);
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
      optional_var[dname] = true;
    else
      optional_var[dname] = false;
  }

  bool   ok = ivp_domain.addDomain(dname.c_str(), dlow, dhgh, dcnt);
  return(ok);
}

