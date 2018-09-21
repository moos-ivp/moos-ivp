/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
  AppCastingMOOSApp::OnStartUp();

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
    m_gui->updateNodes();

  // Update the Proc entries if new appcasts for current Node
  if(new_cast_count_n > old_cast_count_n)
    m_gui->updateProcs();

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

    // Handle all appcast attributes, e.g. "refresh_mode", "true"
    bool handled = m_gui->setRadioCastAttrib(param, value);

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
 
  string key = GetAppName() + "startup";
  postAppCastRequest("all", "all", key, "any", 10);
 
  m_gui->updateRadios();
  registerVariables();
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
  if(!m_appcast_repo) {
    m_msgs << "Null AppCastRepo !!!" << endl;
    return(true);
  }

  m_msgs << "Refresh Mode: " << m_appcast_repo->getRefreshMode() << endl;
  return(true);
}








