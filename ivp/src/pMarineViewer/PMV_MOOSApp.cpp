/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PMV_MOOSApp.cpp                                      */
/*    DATE: Early 2000's. Modified many times since....          */
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

#include <unistd.h>
#include <iostream>
#include "PMV_MOOSApp.h"
#include "MBUtils.h"
#include "NodeRecordUtils.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

PMV_MOOSApp::PMV_MOOSApp() 
{
  m_pending_moos_events = 0;
  m_gui             = 0; 
  m_lastredraw_time = 0;
  m_node_report_vars.push_back("NODE_REPORT");
  m_node_report_vars.push_back("NODE_REPORT_LOCAL");

  VarDataPair pair1("HELM_MAP_CLEAR", 0);
  VarDataPair pair2("PMV_CONNECT", 0);
  m_connection_pairs.push_back(pair1);
  m_connection_pairs.push_back(pair2);
  m_pending_pairs   = true;

  m_appcast_repo             = 0;
  m_appcast_last_req_time    = 0;
  m_appcast_request_interval = 1.0;  // seconds
  m_clear_geoshapes_received = 0;

  m_node_reports_received = 0;
  m_node_report_index     = 0;

  m_log_the_image = false;
}

//----------------------------------------------------------------
// Procedure: setPendingEventsPipe()

void PMV_MOOSApp::setPendingEventsPipe(Threadsafe_pipe<MOOS_event> 
				       *pending_moos_events) 
{
  m_pending_moos_events = pending_moos_events;
}

//----------------------------------------------------------------
// Procedure: OnNewMail

bool PMV_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
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

bool PMV_MOOSApp::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//-------------------------------------------------------------
// Procedure: Iterate
//      Note: virtual overide of base class CMOOSApp member.

bool PMV_MOOSApp::Iterate()
{
  AppCastingMOOSApp::Iterate();
  if(m_pending_pairs)
    postConnectionPairs();

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

//-------------------------------------------------------------
// Procedure: postConnectionPairs
//      Note: 

void PMV_MOOSApp::postConnectionPairs()
{
  m_pending_pairs = false;
  unsigned int i, vsize = m_connection_pairs.size();
  for(i=0; i<vsize; i++) {
    VarDataPair pair = m_connection_pairs[i];
    string var = pair.get_var();
    if(pair.is_string())
      Notify(var, pair.get_sdata());
    else
      Notify(var, pair.get_ddata());
  }
}

//--------------------------------------------------------
// Procedure: onStartUp()

bool PMV_MOOSApp::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  
  Notify("PMARINEVIEWER_PID", getpid());

  // look for datum latitude, longitude global variables
  double lat, lon;
  bool ok1 = m_MissionReader.GetValue("LatOrigin", lat);
  bool ok2 = m_MissionReader.GetValue("LongOrigin", lon);
  if(!ok1 || !ok2) 
    reportConfigWarning("Lat or Lon Origin not set in *.moos file");

  // If both lat and lon origin ok - then initialize the Geodesy.
  if(m_gui && !m_gui->mviewer->initGeodesy(lat, lon))
    reportConfigWarning("Geodesy init failed");

  if(m_gui) {
    cout << "Setting PMV LatOrigin based on the MOOS file. " << endl;
    string datum = doubleToString(lat) + "," + doubleToString(lon);
    m_gui->mviewer->setParam("datum", datum);
  }

  double time_warp;
  bool okw = m_MissionReader.GetValue("MOOSTimeWarp", time_warp);
  if(okw && (time_warp > 0)) {
    if((m_dfFreq * time_warp) > 20) 
      SetAppFreq((20/time_warp));
  }

  m_time_warp = GetMOOSTimeWarp();

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

void PMV_MOOSApp::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  Register("APPCAST", 0);
  Register("VIEW_POLYGON", 0);
  Register("VIEW_WEDGE", 0);
  Register("PHI_HOST_IP",  0);
  Register("VIEW_POINT",   0);
  Register("VIEW_VECTOR",  0);
  Register("VIEW_CIRCLE",  0);
  Register("VIEW_SEGLIST", 0);
  Register("VIEW_SEGLR",   0);
  Register("TRAIL_RESET",  0);
  Register("VIEW_MARKER",  0);
  Register("VIEW_COMMS_PULSE", 0);
  Register("GRID_CONFIG",  0);
  Register("GRID_DELTA",   0);
  Register("VIEW_GRID", 0);
  Register("VIEW_RANGE_PULSE", 0);
  Register("PMV_MENU_CONTEXT", 0);
  Register("PMV_CLEAR", 0);

  unsigned int i, vsize = m_scope_vars.size();
  for(i=0; i<vsize; i++)
    Register(m_scope_vars[i], 0);

  vsize = m_node_report_vars.size();
  for(i=0; i<vsize; i++)
    Register(m_node_report_vars[i], 0);
}

//----------------------------------------------------------------------
// Procedure: handlePendingGUI

void PMV_MOOSApp::handlePendingGUI()
{
  if(!m_gui)
    return;
  
  unsigned int i, pendingSize = m_gui->getPendingSize();

  for(i=0; i<pendingSize; i++) {
    string var  = m_gui->getPendingVar(i);
    string val  = m_gui->getPendingVal(i);
    double dval = 0;

    if(var == "scope_register") {
      Register(val, 0);
      m_scope_vars.push_back(val);
    }
    else {
      string val_type = "string";
      if(isQuoted(val))  
	val = stripQuotes(val);
      else if(isNumber(val)) {
	val_type = "double";
	dval = atof(val.c_str());
      }
      
      if(val_type == "string")
	Notify(var, val);
      else
	Notify(var, dval);
    }
  }

  m_gui->clearPending();
}

//----------------------------------------------------------------------
// Procedure: handleNewMail (OnNewMail)

void PMV_MOOSApp::handleNewMail(const MOOS_event & e)
{
  if(!m_appcast_repo || !m_gui || !m_gui->mviewer)
    return;

  m_gui->mviewer->setParam("curr_time", e.moos_time);
  
  // Begin gather appcast repo info prior to mail handling
  string node = m_appcast_repo->getCurrentNode();
  string proc = m_appcast_repo->getCurrentProc();  

  unsigned int old_node_count    = m_appcast_repo->getNodeCount();
  unsigned int old_proc_count    = m_appcast_repo->getProcCount();
  unsigned int old_cast_count_n  = m_appcast_repo->getAppCastCount(node);
  unsigned int old_cast_count_np = m_appcast_repo->getAppCastCount(node, proc);
  bool         handled_appcast   = false;
  // End gather appcast repo info prior to mail handling

  for(size_t i = 0; i < e.mail.size(); ++i) {
    CMOOSMsg msg   = e.mail[i].msg;
    string   key   = msg.GetKey();
    string   sval  = msg.GetString();
    string   community = msg.GetCommunity();

    bool     handled = false;
    string   why_not;

    if((key == "NODE_REPORT") || (key == "NODE_REPORT_LOCAL")) {
      m_node_reports_received++;
      NodeRecord record = string2NodeRecord(sval);
      m_node_report_index = record.getIndex();
      handled = m_gui->mviewer->handleNodeReport(sval, why_not);
    }

    if(key == "PHI_HOST_IP") {
      m_gui->augmentTitle(sval);
      handled = true;
    }
    
    else if(key == "PMV_CLEAR") {
      handled = handleMailClear(sval);
    }
      
    // PMV_MENU_CONTEXT = 
    // side=left, menukey=polyvert, post="POLY_VERT=x=$(XPOS),y=$(YPOS)"
    else if(key == "PMV_MENU_CONTEXT") {
      vector<string> svector = parseStringQ(sval, ',');
      unsigned int k, ksize = svector.size(); 
      string side, menukey, post;
      for(k=0; k<ksize; k++) {
	string param = biteStringX(svector[k], '=');
	string value = svector[k];
	if(param == "side")
	  side = value;
	else if(param == "menukey")
	  menukey = value;
	else if(param == "post")
	  post = value;
      }
      if((side != "") && (menukey != "") && (post != ""))
	m_gui->addMousePoke(side, menukey, post);
      
      handled = true;
      cout << "Handling PMV_MENUS_CONTEXT" << endl;
    }
      

    // Handler part 1: check for geometry messages
    m_gui->addFilterVehicle(community);

    bool handled_scope   = false;
    unsigned int j, vsize = m_scope_vars.size();

    for(j=0; j<vsize; j++) {
      if(key == m_scope_vars[j]) {
	double tstamp = msg.GetTime();
	string mtime  = doubleToString((tstamp - m_start_time),2);
	string source = msg.GetSource();
	if(msg.IsDouble())
	  sval = doubleToStringX(msg.GetDouble(), 8);
	m_gui->mviewer->updateScopeVariable(key, sval, mtime, source);
	handled_scope = true;
      }
    }

    if(!handled)
      handled = m_gui->mviewer->addGeoShape(key, sval, community, MOOSTime());
    if(!handled)
      handled = m_gui->mviewer->setParam(key, sval);

    if(!handled && (key == "APPCAST")) {
      handled = m_appcast_repo->addAppCast(sval);
      handled_appcast = true;
    }

    if(!handled && !handled_scope) {
      string warning = "Unhandled Mail: " + key + " ";
      if(why_not != "")
	warning += why_not;
      else {
	warning += "=[" + sval + "] src=" + msg.GetSource();
	if(msg.GetSourceAux() != "")
	  warning += " aux=" + msg.GetSourceAux();
      }
      Notify("MVIEWER_UNHANDLED_MAIL", key + "=" + sval);
      reportRunWarning(warning);
    }
  }

  // Part II: Handle Appcasting updates and possible new appcast requests
  unsigned int new_node_count    = m_appcast_repo->getNodeCount();
  unsigned int new_proc_count    = m_appcast_repo->getProcCount();
  unsigned int new_cast_count_n  = m_appcast_repo->getAppCastCount(node);
  unsigned int new_cast_count_np = m_appcast_repo->getAppCastCount(node, proc);
  
  // If we've just discovered a new node, send out a request to ALL nodes
  // everywhere.
  if(new_node_count > old_node_count) {
    string key = GetAppName() + "startup";
    postAppCastRequest("all", "all", key, "any", (0.1*m_time_warp));
  }
  // If we've just discovered a new app on the current node, send out a 
  // request to ALL apps on the current node.
  else if(new_proc_count > old_proc_count) {
    string key = GetAppName() + "newproc";
    postAppCastRequest("all", "all", key, "any", (1.1*m_time_warp));
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

void PMV_MOOSApp::handleIterate(const MOOS_event & e) 
{
  m_gui->clearStaleVehicles();
  
  double curr_time = e.moos_time - m_start_time;
  cout << "." << flush;

  m_gui->mviewer->PMV_Viewer::draw();
  m_gui->mviewer->redraw();
  m_gui->updateXY();
  m_gui->mviewer->setParam("curr_time", e.moos_time);
  m_gui->setCurrTime(curr_time);
  m_lastredraw_time = e.moos_time;

  // We want to detect when a vehicle has been cleared that may still be
  // active. Sent appcast requests to everyone/global when a vehicle has
  // ben recently cleared. Just to give everyone a chance to report back.
  double clear_stale_timestamp = m_gui->getClearStaleTimeStamp();
  double elapsed = curr_time - clear_stale_timestamp;
  double force = false;
  if(elapsed < 2)
    force = true;
  handleAppCastRequesting(force);
  
  string vname = m_gui->mviewer->getStringInfo("active_vehicle_name");

  vector<VarDataPair> left_pairs = m_gui->mviewer->getLeftMousePairs();
  unsigned int i, vsize = left_pairs.size();
  for(i=0; i<vsize; i++) {
    VarDataPair pair = left_pairs[i];
    string var = pair.get_var();
    if(!pair.is_string())
      Notify(var, pair.get_ddata());
    else
      Notify(var, pair.get_sdata());
  }

  vector<VarDataPair> right_pairs = m_gui->mviewer->getRightMousePairs();
  vsize = right_pairs.size();
  for(i=0; i<vsize; i++) {
    VarDataPair pair = right_pairs[i];
    string var = pair.get_var();
    if(!pair.is_string())
      Notify(var, pair.get_ddata());
    else
      Notify(var, pair.get_sdata());
  }

  vector<VarDataPair> non_mouse_pairs = m_gui->mviewer->getNonMousePairs();
  vsize = non_mouse_pairs.size();
  for(i=0; i<vsize; i++) {
    VarDataPair pair = non_mouse_pairs[i];
    string var = pair.get_var();
    if(!pair.is_string())
      Notify(var, pair.get_ddata());
    else
      Notify(var, pair.get_sdata());
  }
  
  handlePendingGUI();
  handlePendingPostsFromGUI();
  handlePendingCommandSummary();

  unsigned int window_val = 1;
  if(!m_gui || !m_gui->getCmdGUI())
    window_val = 0;
  else
    window_val = m_gui->getCmdGUI()->isVisible();

  //cout << "window_val: " << uintToString(window_val) << endl;

  if(window_val == 0)
    m_gui->closeCmdGUI();
  else {
    if(m_gui->getCmdGUI()->getConcedeTop())
      m_gui->show();
  }


  
}

//-------------------------------------------------------------
// Procedure: handleAppCastRequesting()

void PMV_MOOSApp::handleAppCastRequesting(bool force)
{
  // If appcasts are not being viewed dont request refreshes 
  if(m_gui && (m_gui->showingAppCasts() == false))
    return;

  if(force) {
    string key = GetAppName() + ":" + m_host_community;      
    postAppCastRequest("all", "all", key, "any", 3);
    return;
  }

  // Consider how long its been since our appcast request.
  // Want to request less frequently if using a higher time warp.
  double moos_elapsed_time = m_curr_time - m_appcast_last_req_time;
  double real_elapsed_time = moos_elapsed_time / m_time_warp;
  
  if(real_elapsed_time >= m_appcast_request_interval) {
    m_appcast_last_req_time = m_curr_time;
    string refresh_mode = m_appcast_repo->getRefreshMode();
    string current_node = m_appcast_repo->getCurrentNode();
    string current_proc = m_appcast_repo->getCurrentProc();
    
    if(refresh_mode == "streaming") {
      string key = GetAppName() + ":" + m_host_community;      
      postAppCastRequest("all", "all", key, "any", 3);
    }
    else if(refresh_mode == "events") {
      // Not critical that key names be unique, but good practice to 
      // head off multiple uMAC clients from interfering
      string key_app = GetAppName() + ":" + m_host_community + "app";      
      string key_gen = GetAppName() + ":" + m_host_community + "gen";      
      postAppCastRequest(current_node, current_proc, key_app, "any", 3);
      postAppCastRequest("all", "all", key_gen, "run_warning", 3);
    }
  }
}

//----------------------------------------------------------------------
// Procedure: handleStartUp  (OnStartUp)

void PMV_MOOSApp::handleStartUp(const MOOS_event & e) {
  // Keep track of whether the back images were user configured.
  // If not, we'll use the default image afterwards.
  bool tiff_a_set = false;
  bool tiff_b_set = false;

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams)) 
    reportConfigWarning("No config block found for " + GetAppName());
  
  STRING_LIST::reverse_iterator p;
  for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;
    bool   handled = false;

    if((param == "gui_size") && (tolower(value) == "small")) {
      m_gui->size(1000,750);
      handled = true;
    }
    else if((param == "gui_size") && (tolower(value) == "xsmall")) {
      m_gui->size(800,600);
      handled = true;
    }
    else if(param == "button_one")
      handled = m_gui->addButton(param, value);
    else if(param == "button_two")
      handled = m_gui->addButton(param, value);
    else if(param == "button_three")
      handled = m_gui->addButton(param, value);
    else if(param == "button_four")
      handled = m_gui->addButton(param, value);
    else if(param == "action")
      handled = m_gui->addAction(value);
    else if(param == "action+")
      handled = m_gui->addAction(value, true);
    else if((param == "center_vehicle") || (param == "reference_vehicle"))
      handled = m_gui->addReferenceVehicle(value);

    else if(param == "nodes_font_size") 
      handled = m_gui->setRadioCastAttrib(param, value);
    else if(param == "procs_font_size") 
      handled = m_gui->setRadioCastAttrib(param, value);
    else if(param == "appcast_font_size") 
      handled = m_gui->setRadioCastAttrib(param, value);
    else if(param == "appcast_color_scheme") 
      handled = m_gui->setRadioCastAttrib(param, value);
    else if(param == "appcast_viewable") 
      handled = m_gui->setRadioCastAttrib(param, value);
    else if(param == "appcast_height") 
      handled = m_gui->setRadioCastAttrib(param, value);
    else if(param == "appcast_width") 
      handled = m_gui->setRadioCastAttrib(param, value);
    else if(param == "stale_report_thresh") 
      handled = m_gui->mviewer->setParam(param, value);
    else if(param == "stale_remove_thresh") 
      handled = m_gui->mviewer->setParam(param, value);
    else if(param == "cmd") 
      handled = handleConfigCmd(value);

    else if(param == "log_the_image") 
      handled = setBooleanOnString(m_log_the_image, value);
    
    else if(strBegins(param, "left_context", false)) {
      string key = getContextKey(param);
      handled = m_gui->addMousePoke("left", key, value);
    }
    else if(strBegins(param, "right_context", false)) {
      string key = getContextKey(param);
      handled = m_gui->addMousePoke("right", key, value);
    }
    else if(param == "tiff_file") {
      if(!tiff_a_set) {
	tiff_a_set = m_gui->mviewer->setParam(param, value);
      }
      handled = true;
    }
    else if(param == "tiff_file_b") {
      if(!tiff_b_set) 
	tiff_b_set = m_gui->mviewer->setParam(param, value);
      handled = true;
    }
    else if(param == "node_report_variable") {
      if(!strContainsWhite(value)) {
	m_gui->mviewer->setParam(param, value);
	m_node_report_vars.push_back(value);
	handled = true;
      }
    }
    else if(param == "connection_posting") {
      string var = biteStringX(value, '=');
      string val = value;
      if(!strContainsWhite(var)) {
	m_pending_pairs = true;
	VarDataPair pair(var, val, "auto");
	m_connection_pairs.push_back(pair);
	handled = true;
      }      
    }
    else if(param == "scope") {
      vector<string> svector = parseString(value, ',');
      unsigned int i, vsize = svector.size();
      for(i=0; i<vsize; i++) {
	string new_var = stripBlankEnds(svector[i]);
	bool ok = m_gui->mviewer->addScopeVariable(new_var);
	if(ok)  {
	  m_gui->addScopeVariable(new_var);
	  m_scope_vars.push_back(new_var);
	  handled = true;
	}
      }
    }
    else {
      handled = m_gui->mviewer->setParam(param, value);
      if(!handled)
        handled = m_gui->mviewer->setParam(param, atof(value.c_str()));
    }
    
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  if(tiff_a_set && m_log_the_image) {
    string command = "COPY_FILE_REQUEST=";
    string tiff_file = m_gui->mviewer->getTiffFileA();
    string info_file = m_gui->mviewer->getInfoFileA();
    if((tiff_file != "") && (info_file != "")) {
      Notify("PLOGGER_CMD", command + tiff_file);
      Notify("PLOGGER_CMD", command + info_file);
    }
  }

  if(tiff_b_set && m_log_the_image) {
    string command = "COPY_FILE_REQUEST=";
    string tiff_file = m_gui->mviewer->getTiffFileB();
    string info_file = m_gui->mviewer->getInfoFileB();
    if((tiff_file != "") && (info_file != "")) {
      Notify("PLOGGER_CMD", command + tiff_file);
      Notify("PLOGGER_CMD", command + info_file);
    }
  }

#if 0
  // If no images were specified, use the default images.
  if(!tiff_a_set && !tiff_b_set) {
    m_gui->mviewer->setParam("tiff_file", "Default.tif");
    m_gui->mviewer->setParam("tiff_file_b", "DefaultB.tif");
  }
#endif

  m_gui->mviewer->handleNoTiff();
  m_gui->setCommandFolio(m_cmd_folio);

  m_start_time = MOOSTime();
  m_gui->mviewer->setParam("time_warp", m_time_warp);
  m_gui->mviewer->redraw();
  m_gui->updateRadios();
  m_gui->setMenuItemColors();
  
  registerVariables();
}

//----------------------------------------------------------------------
// Procedure: getContextKey
//   Purpose: To determine the "key" in strings of the following form:
//            "left_context[mode]" or "right_context[mode]". 

string PMV_MOOSApp::getContextKey(string str)
{
  string remainder;
  string null_key = "any_left";
  if(strBegins(str, "left_context", false)) // false means case insens.
    remainder = str.c_str()+12;
  else if(strBegins(str, "right_context", false)) {
    remainder = str.c_str()+13;
    null_key = "any_right";
  }
  else
    return("error");

  unsigned int rsize = remainder.size();
  if(rsize == 0)
    return(null_key);

  if(rsize == 1)
    return("error");
  
  if((remainder.at(0) != '[') || (remainder.at(rsize-1) != ']'))
    return("error");

  string key = remainder.substr(1, rsize-2);
  if(key == "")
    return(null_key);
  else
    return(key);
}

//------------------------------------------------------------
// Procedure: handleMailClear
//   Example: "vname=henry, shape=polygon, stype=hull*"
//   Example: "vname=henry"
//   Example: "shape=polygon"

bool PMV_MOOSApp::handleMailClear(string str)
{
  m_clear_geoshapes_received++;
  vector<string> svector = parseStringQ(str, ',');
  unsigned int k, ksize = svector.size(); 
  string vname, shape, stype;
  for(k=0; k<ksize; k++) {
    string param = tolower(biteStringX(svector[k], '='));
    string value = svector[k];
    if(param == "vname")
      vname = value;
    else if(param == "shape")
      shape = value;
    else if(param == "stype")
      stype = value;
    else
      return(false);
  }
  m_gui->clearGeoShapes(vname, shape, stype);
  return(true);
}

//---------------------------------------------------------
// Procedure: handleConfigCmd
//  Examples:
//   cmd = label=loiter,   var=LOITER,      sval=true
//   cmd = label=go_deep,  var=DEPTH,       dval=400
//   cnd = label=PolyWest, var=LOITER_POLY, sval={60,-40:60,-160:150,-160}

bool PMV_MOOSApp::handleConfigCmd(string cmd)
{
  string label, moosvar, sval, dval, receivers, bcolor;
  
  vector<string> svector = parseStringQ(cmd, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string field = tolower(biteStringX(svector[i], '='));
    string value = svector[i];

    if((field == "label") && (label == ""))
      label = value;
    else if((field == "var") && (moosvar == ""))
      moosvar = value;
    else if((field == "sval") && (sval == ""))
      sval = value;
    else if((field == "dval") && (dval == "") && isNumber(value))
      dval = value;
    else if((field == "receivers") && (receivers == ""))
      receivers = value;
    else if((field == "color") && isColor(value))
      bcolor = value;
    else
      return(false);
  }

  if((label == "") || (moosvar == ""))
    return(false);
  if((sval == "") && (dval == ""))
    return(false);
  if((sval != "") && (dval != ""))
    return(false);

  if(isBraced(sval))
    sval = stripBraces(sval);
  
  // Part 2: Build the Command Item
  CommandItem item;
  bool ok = true;
  item.setCmdLabel(label);

  if(bcolor != "")
    item.setCmdColor(bcolor);

  if(sval != "")
    ok = ok && item.setCmdPostStr(moosvar, sval);
  else 
    ok = ok && item.setCmdPostDbl(moosvar, atof(dval.c_str()));
  
  vector<string> rvector = parseString(receivers, ':');
  for(unsigned int i=0; i<rvector.size(); i++) {
    string receiver = stripBlankEnds(rvector[i]);
    ok = ok && item.addCmdPostReceiver(receiver);
  }

  if(!ok)
    return(false);

  // Part 3: Add the new command item
  cout << "Adding Command item:" << endl;
  bool result = m_cmd_folio.addCmdItem(item);
  cout << "result: " << boolToString(result) << endl;
  
  return(true);
}



//----------------------------------------------------------------------
// Procedure: handlePendingPostsFromGUI

void PMV_MOOSApp::handlePendingPostsFromGUI()
{
  if(!m_gui || !m_gui->getCmdGUI())
    return;

  vector<CommandPost> cmd_posts = m_gui->getCmdGUI()->getPendingCmdPosts();
  
  for(unsigned i=0; i<cmd_posts.size(); i++) {
    CommandItem cmd_item = cmd_posts[i].getCommandItem();
    string      cmd_targ = cmd_posts[i].getCommandTarg();
    bool        cmd_test = cmd_posts[i].getCommandTest();
    string      cmd_pid  = cmd_posts[i].getCommandPID();

    cout << "cmd_targ: " << cmd_targ << endl;

    string moosvar = cmd_item.getCmdPostVar();
    if((cmd_targ != "local") && (cmd_targ != "shore"))
      moosvar += "_" + toupper(cmd_targ);
      
    string valtype = cmd_item.getCmdPostType();

    // Part 1: Make the posting if the posting is real. A posting with a
    // target name beginning with "test:" indicates that a posting should
    // not be made, but the posting should still go into the cmd_summary
    // to show the user what would have been posted.
    if(!cmd_test) {
      string src_aux = "pid=" + cmd_pid;
      if(valtype == "string") 
	Notify(moosvar, cmd_item.getCmdPostStr(), src_aux);
      else 
	Notify(moosvar, cmd_item.getCmdPostDbl(), src_aux);
    }
    
    // Part 2: Build the history entry
    string post_val = cmd_item.getCmdPostStr();
    if(valtype != "string")
      post_val = doubleToStringX(cmd_item.getCmdPostDbl());

    m_cmd_summary.addPosting(moosvar, post_val, cmd_pid, cmd_test);
  }

  m_gui->getCmdGUI()->clearPendingCmdPosts();
}


//----------------------------------------------------------------------
// Procedure: handlePendingCommandSummary()

void PMV_MOOSApp::handlePendingCommandSummary()
{
  if(!m_gui || !m_gui->getCmdGUI() || !m_cmd_summary.reportPending())
    return;

  // Give the main GUI an updated copy of the command summary so it
  // can populate the commain GUI upon creation, from its own stored
  // copy of the command summary.
  m_gui->setCommandSummary(m_cmd_summary);
  
  vector<string> command_report = m_cmd_summary.getCommandReport();
  m_gui->getCmdGUI()->setPostSummary(command_report);
}


//------------------------------------------------------------
// Procedure: postAppCastRequest
//   Example: str = "node=henry,app=pHostInfo,duration=10,key=uMAC_438"

void PMV_MOOSApp::postAppCastRequest(string channel_node, 
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

//------------------------------------------------------------
// Procedure: buildReport
//      Note: A virtual function of the AppCastingMOOSApp superclass, conditionally 
//            invoked if either a terminal or appcast report is needed.

bool PMV_MOOSApp::buildReport()
{
  // Nothing for now. AppCasting mostly to catch configuration warnings.

  string tiff_file_a = m_gui->mviewer->getTiffFileA();
  string info_file_a = m_gui->mviewer->getInfoFileA();
  string tiff_file_b = m_gui->mviewer->getTiffFileB();
  string info_file_b = m_gui->mviewer->getInfoFileB();

  m_msgs << "Tiff File A:      " << tiff_file_a << endl;
  m_msgs << "Info File A:      " << info_file_a << endl;
  m_msgs << "Tiff File B:      " << tiff_file_b << endl;
  m_msgs << "Info File B:      " << info_file_b << endl;
  m_msgs << "------------------" << endl;
  m_msgs << "Total GeoShapes:  " << m_gui->mviewer->shapeCount("total_shapes") << endl;
  m_msgs << "Clear GeoShapes:  " << m_clear_geoshapes_received << endl;
  m_msgs << "NodeReports Recd: " << m_node_reports_received << endl;
  m_msgs << "NodeReport Index: " << m_node_report_index << endl;

  unsigned int drawcount = m_gui->mviewer->getDrawCount();
  
  double real_elapsed = (m_curr_time - m_start_time) / m_time_warp;
  double drawrate = ((double)(drawcount) / real_elapsed);
  
  m_msgs << "Draw Count:       " << drawcount << endl; 
  m_msgs << "Draw Count Rate:  " << drawrate  << endl; 

  double curr_time = m_gui->mviewer->getCurrTime();
  m_msgs << "Curr Time:        " << doubleToString(curr_time,2) << endl;

  double time_warp = m_gui->mviewer->getTimeWarp();
  m_msgs << "Time Warp:        " << doubleToString(time_warp,3) << endl;

  double srep_thresh = m_gui->mviewer->getStaleReportThresh();
  m_msgs << "Stale report thresh: " << doubleToStringX(srep_thresh,2) << endl;

  double srem_thresh = m_gui->mviewer->getStaleRemoveThresh();
  m_msgs << "Stale remove thresh: " << doubleToStringX(srem_thresh,2) << endl;

  double elapsed = m_gui->mviewer->getElapsed();
  m_msgs << "Elapsed:          " << doubleToString(elapsed,5) << endl;

  unsigned int cmd_folio_size = m_cmd_folio.size();
  m_msgs << "CmdFolio size:    " << uintToString(cmd_folio_size) << endl;

  m_msgs << endl;
  m_msgs << "Visual Settings: " << endl;
  string panx_str = doubleToString(m_gui->getMarineViewer()->getPanX(), 2);
  string pany_str = doubleToString(m_gui->getMarineViewer()->getPanY(), 2);
  string zoom_str = doubleToString(m_gui->getMarineViewer()->getZoom(), 2);

  m_msgs << "  pan_x:   " << panx_str << endl;
  m_msgs << "  pan_y:   " << pany_str << endl;
  m_msgs << "  zoom :   " << zoom_str << endl;

  return(true);
}







