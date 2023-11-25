/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: UMV_MOOSApp.cpp                                      */
/*    DATE: Aug 11th, 2012                                       */
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

#include <iostream>
#include "UMV_MOOSApp.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

UMV_MOOSApp::UMV_MOOSApp() 
{
  m_pending_moos_events = 0;
  m_gui                 = 0; 

  m_appcast_repo             = 0;
  m_appcast_last_req_time    = 0;
  m_appcast_request_interval = 1.0;  // seconds

  m_relcast_last_req_time    = 0;
  m_relcast_request_interval = 1.0;  // seconds

  // Build a unique client name for realmcasting
  srand(time(NULL));
  int    rand_int = rand() % 1000;
  string rand_str = intToString(rand_int);

  m_relcast_client_name = "umv_" + rand_str;
}

//----------------------------------------------------------------
// Procedure: setPendingEventsPipe

void UMV_MOOSApp::setPendingEventsPipe(Threadsafe_pipe<MOOS_event>*
				       pending_moos_events)
{
  m_pending_moos_events = pending_moos_events;
}

//----------------------------------------------------------------
// Procedure: OnNewMail

bool UMV_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  if((!m_gui) || (!m_pending_moos_events))
    return(true);

  NewMail.sort();
  
  MOOS_event e;
  e.type="OnNewMail";
  e.moos_time = MOOSTime();

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &Msg = *p;
    e.mail.push_back(MOOS_event::Mail_message(Msg));
  }
  
  m_pending_moos_events->enqueue(e);
  Fl::awake();
  
  return(true);
}


//--------------------------------------------------------------
// Procedure: OnConnectToServer()
//      Note: virtual overide of base class CMOOSApp member.

bool UMV_MOOSApp::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//-------------------------------------------------------------
// Procedure: Iterate
//      Note: virtual overide of base class CMOOSApp member.

bool UMV_MOOSApp::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if((!m_gui) || (!m_pending_moos_events))
    return(true);  
 
  MOOS_event e;
  e.type="Iterate";
  e.moos_time = MOOSTime();

  m_pending_moos_events->enqueue(e);
  Fl::awake();
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//--------------------------------------------------------
// Procedure: OnStartUp()

bool UMV_MOOSApp::OnStartUp()
{
  string directives = "must_have_moosblock=false";
  AppCastingMOOSApp::OnStartUpDirectives(directives);

  if((!m_gui) || (!m_pending_moos_events))
    return(true);
  
  MOOS_event e;
  e.type="OnStartUp";
  e.moos_time = MOOSTime();

  m_pending_moos_events->enqueue(e);
  Fl::awake();

  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void UMV_MOOSApp::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("DB_UPTIME", 0);
  Register("APPCAST", 0);
  Register("REALMCAST", 0);
  Register("WATCHCAST", 0);
  Register("REALMCAST_CHANNELS", 0);
}

//----------------------------------------------------------------------
// Procedure: handleNewMail  (OnNewMail)

void UMV_MOOSApp::handleNewMail(const MOOS_event & e)
{
  if(!m_appcast_repo || !m_gui)
    return;

  string node = m_appcast_repo->getCurrentNode();
  string proc = m_appcast_repo->getCurrentProc();  

  unsigned int old_node_count    = m_appcast_repo->getNodeCount();
  unsigned int old_cast_count_n  = m_appcast_repo->getAppCastCount(node);
  unsigned int old_cast_count_np = m_appcast_repo->getAppCastCount(node, proc);

  bool handled_appcast = false;

  for(size_t i=0; i<e.mail.size(); ++i) {
    CMOOSMsg msg = e.mail[i].msg;
    string key   = msg.GetKey();
    string sval  = msg.GetString();
    //double dval  = msg.GetDouble();
    string community = msg.GetCommunity();
    string source = msg.GetSource();

    if(key == "APPCAST") {
      m_appcast_repo->addAppCast(sval);
      handled_appcast = true;
    }
    else if(key == "REALMCAST") {
      m_realm_repo->addRealmCast(sval);
      m_gui->updateRealmCastNodes();
      m_gui->updateRealmCastProcs();
      m_gui->updateRealmCast();
    }
    else if(key == "WATCHCAST") {
      m_realm_repo->addWatchCast(sval);
      m_gui->updateRealmCastNodes();
      m_gui->updateRealmCastProcs();
      m_gui->updateRealmCast();
    }
    else if(key == "REALMCAST_CHANNELS") {
      RealmSummary summary = string2RealmSummary(sval);

      bool changed_node_or_proc = false;
      bool handled = m_realm_repo->addRealmSummary(summary, changed_node_or_proc);
      if(changed_node_or_proc) {
	m_gui->updateRealmCastNodes(true);
      }
      else {
	m_gui->updateRealmCastNodes();
	m_gui->updateRealmCastProcs();
      }
      
      if(!handled)
	reportRunWarning("Unhandled RealmSummary:" + sval);
    }
  }

  unsigned int new_node_count    = m_appcast_repo->getNodeCount();
  unsigned int new_cast_count_n  = m_appcast_repo->getAppCastCount(node);
  unsigned int new_cast_count_np = m_appcast_repo->getAppCastCount(node, proc);
  
  if(new_node_count > old_node_count) {
    string key = GetAppName() + "startup";
    postAppCastRequest("all", "all", key, "any", 0.1);
  }

  // Update the Node entries if there is ANY new appcast
  if(handled_appcast)
    m_gui->updateAppCastNodes();

  // Update the Proc entries if new appcasts for current Node
  if(new_cast_count_n > old_cast_count_n)
    m_gui->updateAppCastProcs();

  // Update the Appcast content if new appcast for current node/proc
  if(new_cast_count_np > old_cast_count_np)
    m_gui->updateAppCast();

}


//----------------------------------------------------------------------
// Procedure: handleIterate

void UMV_MOOSApp::handleIterate(const MOOS_event & e) 
{
  // Consider how long its been since our appcast request.
  // Want to request less frequently if using a higher time warp.
  double moos_elapsed_time = m_curr_time - m_appcast_last_req_time;
  double real_elapsed_time = moos_elapsed_time / m_time_warp;
  
  if(real_elapsed_time >= m_appcast_request_interval) {
    m_appcast_last_req_time = m_curr_time;
    string refresh_mode = m_appcast_repo->getRefreshMode();
    string current_node = m_appcast_repo->getCurrentNode();
    string current_proc = m_appcast_repo->getCurrentProc();
    
    if(refresh_mode == "events") {
      // Not critical that key names be unique, but good practice to 
      // head off multiple uMAC clients from interfering
      string key_app = GetAppName() + ":" + m_host_community + "app"; 
      string key_gen = GetAppName() + ":" + m_host_community + "gen";  
      postAppCastRequest(current_node, current_proc, key_app, "any", 3);
      postAppCastRequest("all", "all", key_gen, "run_warning", 3);
    }
    else if(refresh_mode == "streaming") {
      string key = GetAppName() + ":" + m_host_community;      
      postAppCastRequest("all", "all", key, "any", 3);
    }
  }

  handleRealmCastRequesting();
}


//----------------------------------------------------------------------
// Procedure: handleStartUp (OnStartUp)

void UMV_MOOSApp::handleStartUp(const MOOS_event & e) 
{
  if(!m_gui)
    return;

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  STRING_LIST::reverse_iterator p;
  for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "watch_cluster")
      handled = handleConfigWatchCluster(value);
    else if(param == "content_mode") 
      handled = m_gui->setRadioCastAttrib(param, value);
    else if(param == "realmcast_channel")
      handled = m_realm_repo->setOnStartPreferences(value);
    else {
      // Handle all infocast attributes, e.g. "refresh_mode", "true"
      handled = m_gui->setRadioCastAttrib(param, value);
    }
    
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  bool changed = m_realm_repo->checkStartCluster();
  if(changed) {
    m_gui->updateRealmCastNodes(true);
    m_gui->updateRealmCastProcs(true);
  }
  
  string key = GetAppName() + "startup";
  postAppCastRequest("all", "all", key, "any", 10);
 
  m_gui->updateRadios();
  m_gui->setMenuItemColors();
  registerVariables();
}

//---------------------------------------------------------
// Procedure: handleConfigWatchCluster()
//  Examples:
//   str = key=helm_state, vars=DEPLOY:RETURN:MODE:STATION_KEEP
//   str = vars=SURVEY_POINTS:SURVEY_REQUESt    (key auto-assigned "cluster2")

bool UMV_MOOSApp::handleConfigWatchCluster(string str)
{
  if(!m_realm_repo)
    return(false);
  
  return(m_realm_repo->addWatchCluster(str));
}


//-------------------------------------------------------------
// Procedure: handleRealmCastRequesting()

void UMV_MOOSApp::handleRealmCastRequesting()
{
  // Sanity Checks
  if(!m_gui || !m_realm_repo)
    return;
  if(m_gui->getInfoCastSettings().getContentMode() != "realmcast")
    return;
  if(m_gui->getInfoCastSettings().getRefreshMode() == "paused")
    return;

  
  // Part 1: Consider how long its been since our realmcast request.
  // Want to request less frequently if using a higher time warp.
  bool post_request = false;
  if(m_realm_repo->getNodeProcChanged())
    post_request = true;
  else {
    double moos_elapsed_time = m_curr_time - m_relcast_last_req_time;
    double real_elapsed_time = moos_elapsed_time / m_time_warp;
    if(real_elapsed_time >= m_relcast_request_interval) 
      post_request = true;
  }
  if(!post_request)
    return;

  // Part 2: Build the general compnents of the request: who is requesting
  // and what kinds of modifiers should be applied.
  string request_str = "client=" + m_relcast_client_name + ",duration=3";
  if(!m_gui->getInfoCastSettings().getShowRealmCastSource())
    request_str += ",nosrc";
  if(!m_gui->getInfoCastSettings().getShowRealmCastCommunity())
    request_str += ",nocom";
  if(!m_gui->getInfoCastSettings().getShowRealmCastSubs())
    request_str += ",nosubs";
  if(m_gui->getInfoCastSettings().getWrapRealmCastContent())
    request_str += ",wrap";
  if(!m_gui->getInfoCastSettings().getShowRealmCastMasked())
    request_str += ",mask";
  if(m_gui->getInfoCastSettings().getTruncRealmCastContent())
    request_str += ",trunc";
  if(m_gui->getInfoCastSettings().getRealmCastTimeFormatUTC())
    request_str += ",utc";

  // Part 3: Content requested depends on whether a cluster_key has
  // been selected. If cluster_key then WATCHCAST_REQ. 

  string curr_cluster_key = m_realm_repo->getCurrClusterKey();
  
  // Part 3A: Generate a REALMCAST_REQ
  if(curr_cluster_key == "") {
    string current_node = m_realm_repo->getCurrentNode();
    string current_proc = m_realm_repo->getCurrentProc();
    
    if((current_node == "") || (current_proc == "")) {
      cout << "REJECTED postRealmCastRequest!!!!" << endl;
      return;
    }
    request_str += ",channel=" + current_proc;
    if(current_node == m_host_community)  
      Notify("REALMCAST_REQ", request_str);
    else
      Notify("REALMCAST_REQ_"+toupper(current_node), request_str);
  }

  // Part 3B: Generate a WATCHCAST_REQ
  else {
    string curr_cluster_var = m_realm_repo->getCurrClusterVar();
    if(curr_cluster_var != "")
      request_str += ",vars=" + curr_cluster_var;
    else {
      string cluster_vars = m_realm_repo->getCurrClusterVars();
      request_str += ",vars=" + cluster_vars;
    }
    if(m_realm_repo->getForceRefreshWC()) {
      request_str += ",force";
      m_realm_repo->setForceRefreshWC(false);
    }
    Notify("REALMCAST_REQ_ALL", request_str);
  }
  
  m_relcast_last_req_time = m_curr_time;

}

//------------------------------------------------------------
// Procedure: postAppCastRequest
//   Example: str = "node=henry,app=pHostInfo,duration=10,key=uMAC_438"

void UMV_MOOSApp::postAppCastRequest(string channel_node, 
				     string channel_proc, 
				     string given_key, 
				     string threshold, 
				     double duration)
{
  string key = given_key;
  if(key == "")
    key = GetAppName();

  if((channel_node == "") || (channel_proc == "")) {
    cout << "REJECTED postAppCastRequest!!!!" << endl;
    return;
  }
  
  if((threshold != "any") && (threshold != "run_warning")) {
    cout << "REJECTED postAppCastRequest: unrecognized threshold type" << endl;
    return;
  }

  // Notify the source of the NEW channel of the switch
  string str = "node=" + channel_node;
  str += ",app=" + channel_proc;
  str += ",duration=" + doubleToString(duration, 1);
  str += ",key=" + key;
  str += ",thresh=" + threshold;

  Notify("APPCAST_REQ", str);
  Notify("APPCAST_REQ_"+toupper(channel_node), str);
}

//---------------------------------------------------------
// Procedure: buildReport()
//      Note: A virtual function of the AppCastingMOOSApp superclass, conditionally 
//            invoked if either a terminal or appcast report is needed.

bool UMV_MOOSApp::buildReport()
{
  return(false);
  if(!m_appcast_repo) {
    m_msgs << "Null AppCastRepo !!!" << endl;
    return(true);
  }

  m_msgs << "Refresh Mode: " << m_appcast_repo->getRefreshMode() << endl;
  return(true);
}









