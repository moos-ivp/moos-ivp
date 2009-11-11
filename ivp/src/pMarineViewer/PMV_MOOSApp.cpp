/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PMV_MOOSApp.cpp                                      */
/*    DATE:                                                      */
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

#include <iostream>
#include "PMV_MOOSApp.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

PMV_MOOSApp::PMV_MOOSApp() 
{
  m_left_click_str  = "null"; 
  m_right_click_str = "null"; 

  m_pending_moos_events = 0;
  m_verbose         = false;
  m_gui             = 0; 
  m_start_time      = 0;
  m_lastredraw_time = 0;
  m_node_report_vars.push_back("AIS_REPORT");
  m_node_report_vars.push_back("AIS_REPORT_LOCAL");
  m_node_report_vars.push_back("NODE_REPORT");
  m_node_report_vars.push_back("NODE_REPORT_LOCAL");
}

//----------------------------------------------------------------
// Procedure: OnNewMail

bool PMV_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  if((!m_gui) || (!m_pending_moos_events))
    return(true);

  NewMail.sort();
  
  MOOS_event e;
  e.type="OnNewMail";
  e.moos_time = MOOSTime();

  MOOSMSG_LIST::iterator p;
  for(p = NewMail.begin();p!=NewMail.end();p++) {
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

  m_Comms.Notify("HELM_MAP_CLEAR", 0);
  return(true);
}

//-------------------------------------------------------------
// Procedure: Iterate
//      Note: virtual overide of base class CMOOSApp member.

bool PMV_MOOSApp::Iterate()
{
  if((!m_gui) || (!m_pending_moos_events))
    return(true);
  
  MOOS_event e;
  e.type="Iterate";
  e.moos_time = MOOSTime();

  m_pending_moos_events->enqueue(e);
  Fl::awake();
  
  return(true);
}


//--------------------------------------------------------
// Procedure: onStartUp()

bool PMV_MOOSApp::OnStartUp()
{
  MOOSTrace("pMarineViewer starting....\n");
  
  // look for latitude, longitude global variables
  double lat_origin, lon_origin;
  bool ok1 = m_MissionReader.GetValue("LatOrigin", lat_origin);
  bool ok2 = m_MissionReader.GetValue("LongOrigin", lon_origin);
  if(!ok1 || !ok2)
    return(MOOSFail("Lat or Lon Origin not set in *.moos file.\n"));

  // If both lat and lon origin ok - then initialize the Geodesy.
  if(m_gui && !m_gui->mviewer->initGeodesy(lat_origin, lon_origin))
    return(MOOSFail("Geodesy Init in pMarineViewer failed - FAIL\n"));

  if((!m_gui) || (!m_pending_moos_events))
    return(true);
  
  MOOS_event e;
  e.type="OnStartUp";
  e.moos_time = MOOSTime();

  m_pending_moos_events->enqueue(e);
  Fl::awake();

  return(true);
}


//---------------------------------------------------------------
// Procedure: receivePK_SOL

bool PMV_MOOSApp::receivePK_SOL(string sval)
{
  bool return_status = true;
  vector<string> rvector;

  // REPORTS in PK_SOL message are separated by ";"

  vector<string> svector = parseString(sval, ';');

  // Cycle through all reports
  for(unsigned int i=0;i<svector.size()-1;i++) {
    // Remove first item of string - it is the type of report
    // (e.g. REPORT_TYPE = AIS_REPORT) so strip that off and 
    // proceed as before with the single AIS_REPORT messages
    
    rvector = chompString(svector[i], ',');
    if(rvector.size() > 1 ) {
      return_status = m_gui->mviewer->setParam("node_report", 
					       rvector[1].c_str());
    }
  }
  return(return_status);
}

//------------------------------------------------------------
// Procedure: registerVariables

void PMV_MOOSApp::registerVariables()
{
  m_Comms.Register("PK_SOL", 0);
  m_Comms.Register("GRID_CONFIG",      0);
  m_Comms.Register("GRID_DELTA",       0);
  m_Comms.Register("VIEW_POLYGON",     0);
  m_Comms.Register("VIEW_POINT",       0);
  m_Comms.Register("VIEW_CIRCLE",      0);
  m_Comms.Register("VIEW_SEGLIST",     0);
  m_Comms.Register("TRAIL_RESET",      0);
  m_Comms.Register("VIEW_MARKER",      0);

  unsigned int i, vsize = m_scope_vars.size();
  for(i=0; i<vsize; i++)
    m_Comms.Register(m_scope_vars[i], 0);

  vsize = m_node_report_vars.size();
  for(i=0; i<vsize; i++)
    m_Comms.Register(m_node_report_vars[i], 0);
}

//----------------------------------------------------------------------
// Procedure: handlePendingGUI

void PMV_MOOSApp::handlePendingGUI()
{
  if(!m_gui)
    return;
  
  int pendingSize = m_gui->getPendingSize();

  for(int i=0; i<pendingSize; i++) {
    string var  = m_gui->getPendingVar(i);
    string val  = m_gui->getPendingVal(i);
    double dval = 0;

    if(var == "scope_register") {
      m_Comms.Register(val, 0);
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
      
      //cout << "Notifying - var: " << var << "  val:" << val << endl;
      if(val_type == "string")
	m_Comms.Notify(var, val);
      else
	m_Comms.Notify(var, dval);
    }
  }

  m_gui->clearPending();
}



//----------------------------------------------------------------------
// Procedure: handleNewMail

void PMV_MOOSApp::handleNewMail(const MOOS_event & e)
{
  m_gui->mviewer->setParam("curr_time", e.moos_time);
  
  int handled_msgs = 0;
  for (size_t i = 0; i < e.mail.size(); ++i) {
    CMOOSMsg msg = e.mail[i].msg;
    string key   = msg.m_sKey;
    string sval  = msg.m_sVal;
    
    bool scope_handled = false;
    unsigned int j, vsize = m_scope_vars.size();
    for(j=0; j<vsize; j++) {
      if(key == m_scope_vars[j]) {
	string mtime = doubleToString((msg.m_dfTime-m_start_time),2);
	string source = msg.m_sSrc;
	if(msg.IsDouble())
	  sval = dstringCompact(doubleToString(msg.m_dfVal, 8));
	m_gui->mviewer->updateScopeVariable(key, sval, mtime, source);
	scope_handled = true;
      }
	}

    bool handled = m_gui->mviewer->setParam(key, sval);
    if(!handled && (key == "PK_SOL")) {
      MOOSTrace("\nProcessing PK_SOL Message\n");
      receivePK_SOL(sval);
      handled = true;
    }
    if(!handled && !scope_handled) {
      MOOSTrace("pMarineViewer OnNewMail Unhandled msg: \n");
      MOOSTrace("  [key:%s val:%s]\n", key.c_str(), sval.c_str());
      MOOSTrace("?");
    }
    
    if(key == "VIEW_POLYGON")           MOOSTrace("P");
    else if(key == "VIEW_SEGLIST")      MOOSTrace("S");
    else if(key == "VIEW_POINT")        MOOSTrace(".");
    else if(key == "GRID_CONFIG")       MOOSTrace("X");
    else if(key == "NODE_REPORT")       MOOSTrace("*");
    else if(key == "NODE_REPORT_LOCAL") MOOSTrace("*");
    else if(key == "GRID_CONFIG")       MOOSTrace("X");
    else if(key == "GRID_DELTA")        MOOSTrace("G");
    else if(key == "VIEW_MARKER")       MOOSTrace("M");

    if(handled)
      handled_msgs++;
  }

  if(handled_msgs > 0) {
    m_lastredraw_time = e.moos_time;
    m_gui->updateXY();
    m_gui->mviewer->redraw();
  }
}


//----------------------------------------------------------------------
// Procedure: handleIterate

void PMV_MOOSApp::handleIterate(const MOOS_event & e) {
  double curr_time = e.moos_time - m_start_time;
  double time_diff = (e.moos_time - m_lastredraw_time);
  if(time_diff > 1) {
    m_gui->mviewer->redraw();
    m_lastredraw_time = e.moos_time;
  }
  m_gui->updateXY();
  m_gui->mviewer->setParam("curr_time", e.moos_time);
  m_gui->setCurrTime(curr_time);

  string left_click_str = m_gui->mviewer->getStringInfo("left_click_info");
  if(left_click_str != m_left_click_str) {
    m_left_click_str = left_click_str;
    
    if(m_left_click_str != "") {
      string vname   = m_gui->mviewer->getStringInfo("active_vehicle_name");
      string postval = left_click_str;
      if(vname != "")
        postval += (",vname=" + vname);
      m_Comms.Notify("MVIEWER_LCLICK", postval);
    }
  }

  string right_click_str = m_gui->mviewer->getStringInfo("right_click_info");
  if(m_right_click_str != m_right_click_str) {
    m_right_click_str = right_click_str;
    
    if(m_right_click_str != "") {
      string vname   = m_gui->mviewer->getStringInfo("active_vehicle_name");
      string postval = right_click_str;
      if(vname != "")
        postval += (",vname=" + vname);
      m_Comms.Notify("MVIEWER_RCLICK", postval);
    }
  }

  handlePendingGUI();
}


//----------------------------------------------------------------------
// Procedure: handleStartUp

void PMV_MOOSApp::handleStartUp(const MOOS_event & e) {
  // Keep track of whether the back images were user configured.
  // If not, we'll use the default image afterwards.
  bool tiff_a_set = false;
  bool tiff_b_set = false;

  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  STRING_LIST::reverse_iterator p;
  for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
    string line  = *p;
    string param = stripBlankEnds(tolower(biteString(line, '=')));
    string value = stripBlankEnds(line);
    
    if(param == "verbose")
      m_verbose = (tolower(value) == "true");
    else if(param == "button_one")
      m_gui->addButton(param, value);
    else if(param == "button_two")
      m_gui->addButton(param, value);
    else if(param == "button_three")
      m_gui->addButton(param, value);
    else if(param == "button_four")
      m_gui->addButton(param, value);
    else if(param == "action")
      m_gui->addAction(value);
    else if(param == "action+")
      m_gui->addAction(value, true);
    else if((param == "center_vehicle") || (param == "reference_vehicle"))
      m_gui->addReferenceVehicle(value);
    else if(param == "left_context")
      m_gui->addContext("left", value);
    else if(param == "right_context")
      m_gui->addContext("right", value);
    else if(param == "tiff_file") {
      if(!tiff_a_set)
	tiff_a_set = m_gui->mviewer->setParam(param, value);
    }
    else if(param == "tiff_file_b") {
      if(!tiff_b_set) 
	tiff_b_set = m_gui->mviewer->setParam(param, value);
    }
    else if(param == "node_report_variable") {
      if(!strContainsWhite(value)) {
	m_gui->mviewer->setParam(param, value);
	m_node_report_vars.push_back(value);
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
	}
      }
    }
    else { 
      bool handled = m_gui->mviewer->setParam(param, value);
      if(!handled)
        handled = m_gui->mviewer->setParam(param, atof(value.c_str()));
    }
  }
  
  // If no images were specified, use the default images.
  if(!tiff_a_set && !tiff_b_set) {
    m_gui->mviewer->setParam("tiff_file", "Default.tif");
    m_gui->mviewer->setParam("tiff_file_b", "DefaultB.tif");
  }

  m_start_time = MOOSTime();
  m_gui->mviewer->redraw();
  
  registerVariables();
}
