/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: SSV_MOOSApp.cpp                                      */
/*    DATE: September 11th, 2007                                 */
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
#include "SSV_MOOSApp.h"
#include "MBUtils.h"
#include "XYBuildUtils.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: Constructor

SSV_MOOSApp::SSV_MOOSApp() 
{
  m_gui = 0;
  
  m_left_click_str  = "null"; 
  m_right_click_str = "null";
}

//-----------------------------------------------------------------
// Procedure: OnNewMail

bool SSV_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  NewMail.sort();
  
  bool gui_needs_redraw = false;
  bool gui_clear_trails = false;
  MOOSMSG_LIST::reverse_iterator p;

  m_gui->mviewer->mutexLock();
  double curr_time = MOOSTime();

  m_gui->mviewer->setParam("curr_time", curr_time);

  for(p = NewMail.rbegin();p!=NewMail.rend();p++) {
    CMOOSMsg &Msg = *p;

    string key = Msg.m_sKey;

    if((key == "AIS_REPORT") || (key == "AIS_REPORT_LOCAL")) {
      if(m_gui) {
	bool ok = m_gui->mviewer->setParam("ais_report", Msg.m_sVal);
	if(!ok)
	  cout << "Problem with parsing AIS_REPORT" << endl;
	else
	  gui_needs_redraw = true;
	cout << "*" << flush;
      }
    }
    else if(key == "GRID_CONFIG") { 
      receiveGRID_CONFIG(Msg);
      gui_needs_redraw = true;
    }
    else if(key == "GRID_DELTA") { 
      receiveGRID_DELTA(Msg);
      gui_needs_redraw = true;
    }
    else if(key == "VIEW_POLYGON") { 
      receivePolygon(Msg);
      gui_needs_redraw = true;
    }
    else if(key == "VIEW_SEGLIST") { 
      receiveSegList(Msg);
      gui_needs_redraw = true;
    }
    else if(key == "VIEW_POINT") { 
      receivePoint(Msg);
      gui_needs_redraw = true;
    }
    else if(key == "VIEW_CIRCLE") { 
      receivePoint(Msg);
      gui_needs_redraw = true;
    }
    else if(key == "STATION_CIRCLE") { 
      receiveStationCircle(Msg);
      gui_needs_redraw = true;
    }
    else if(key == "RTATION_CIRCLE") { 
      receiveStationCircle(Msg);
      gui_needs_redraw = true;
    }
    else if(key == "TRAIL_RESET") { 
      gui_clear_trails = true;
    }
    else {
      MOOSTrace("Unknown msg [%s]\n",key.c_str());
    }
    //cout << "Originating community: " << Msg.m_sOriginatingCommunity << endl;
  }

  if(gui_clear_trails && m_gui)
    m_gui->mviewer->setParam("clear_trails");

  m_gui->mviewer->mutexUnLock();

  if(gui_needs_redraw && m_gui) {
    m_gui->updateXY();
    m_gui->mviewer->redraw();
  }

  return(true);
}


//----------------------------------------------------------------------
// Procedure: OnConnectToServer()
//      Note: virtual overide of base class CMOOSApp member.

bool SSV_MOOSApp::OnConnectToServer()
{
  m_Comms.Register("AIS_REPORT", 0);
  m_Comms.Register("AIS_REPORT_LOCAL", 0);
  m_Comms.Register("GRID_CONFIG", 0);
  m_Comms.Register("GRID_DELTA", 0);
  m_Comms.Register("VIEW_POLYGON", 0);
  m_Comms.Register("VIEW_SEGLIST", 0);
  m_Comms.Register("VIEW_POINT", 0);
  m_Comms.Register("TRAIL_RESET", 0);
  m_Comms.Register("STATION_CIRCLE", 0);
  m_Comms.Register("RSTATION_CIRCLE", 0);
  return(true);
}

//----------------------------------------------------------------------
// Procedure: Iterate
//      Note: virtual overide of base class CMOOSApp member.

bool SSV_MOOSApp::Iterate()
{
  cout << "." << flush;

  double curr_time = MOOSTime() - m_start_time;
  if(m_gui) {
    m_gui->setCurrTime(curr_time);
    m_gui->updateXY();
  }

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

  return(true);
}


//--------------------------------------------------------
// Procedure: onStartUp()

bool SSV_MOOSApp::OnStartUp()
{
  MOOSTrace("pShipsideViewer starting....\n");
  
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  
  //Initialize m_Geodesy from lat lon origin in .moos file
  string sVal;
  double dfLatOrigin;
  double dfLongOrigin;

  if(m_MissionReader.GetValue("LatOrigin",sVal)) {
    dfLatOrigin = atof(sVal.c_str());
    MOOSTrace("  LatOrigin  = %10.5f deg.\n",dfLatOrigin);
  }
  else {
    MOOSTrace("LatOrigin not specified in mission file - FAIL\n");
    return(false);
  }

  if(m_MissionReader.GetValue("LongOrigin",sVal)) {
    dfLongOrigin = atof(sVal.c_str());
    MOOSTrace("  LongOrigin = %10.5f deg.\n",dfLongOrigin);
  }
  else {
    MOOSTrace("LongOrigin not specified in mission file - FAIL\n");
    return(false);
  }
  
  if(!m_gui->mviewer->initGeodesy(dfLatOrigin, dfLongOrigin)) {
    MOOSTrace("Geodesy Init inside pShipSideViewer failed - FAIL\n");
    return(false);
  }

  
  m_gui->mviewer->mutexLock();
  STRING_LIST::reverse_iterator p;
  for(p = sParams.rbegin();p!=sParams.rend();p++) {
    string sLine = *p;
    string param = toupper(MOOSChomp(sLine, "="));
    string value = stripBlankEnds(sLine);
    
    if(param == "CONTACTS")
      handleContactList(sLine);
    else { 
      bool handled = m_gui->mviewer->setParam(param, value);
      if(!handled)
	m_gui->mviewer->setParam(param, atof(value.c_str()));
    } 
  }
  m_gui->mviewer->mutexUnLock();

  m_start_time = MOOSTime();
  m_gui->mviewer->redraw();
  
  return(true);
}


//----------------------------------------------------------------------
// Procedure: receiveGRID_CONFIG

bool SSV_MOOSApp::receiveGRID_CONFIG(CMOOSMsg &Msg)
{
  //cout << "RECEIVED GRID CONFIGURATION!!!!" << endl << flush;
  //cout << "   Msg.m_sVal:" << Msg.m_sVal << endl << flush;
  XYGrid search_grid;
  
  bool ok = search_grid.initialize(Msg.m_sVal);
  if(ok) {
    m_gui->mviewer->addGrid(search_grid);
    return(true);
  }
  else {
    cout << "Parse Error in receiveGRID_CONFIG" << endl;
    return(false);
  }
}

//----------------------------------------------------------------------
// Procedure: receivePolygon

bool SSV_MOOSApp::receivePolygon(CMOOSMsg &Msg)
{
  XYPolygon new_poly = stringToPoly(Msg.m_sVal);
  
  if(new_poly.size() != 0) {
    m_gui->mviewer->addPoly(new_poly);
    return(true);
  }
  else {
    cout << "Parse Error in receivePolygon" << endl;
    cout << "Msg: " << Msg.m_sVal << endl;
    return(false);
  }
}

//----------------------------------------------------------------------
// Procedure: receiveSegList

bool SSV_MOOSApp::receiveSegList(CMOOSMsg &Msg)
{
  XYSegList new_seglist = stringToSegList(Msg.m_sVal);
  
  bool ok = (new_seglist.size() > 0);
  if(ok) {
    m_gui->mviewer->addSegList(new_seglist);
    return(true);
  }
  else {
    cout << "Parse Error in receiveSegList" << endl;
    cout << "Msg: " << Msg.m_sVal << endl;
    return(false);
  }
}

//----------------------------------------------------------------------
// Procedure: receivePoint

bool SSV_MOOSApp::receivePoint(CMOOSMsg &Msg)
{
  XYPoint new_point = stringToPoint(Msg.m_sVal);

  if(new_point.valid()) {
    m_gui->mviewer->addPoint(new_point);
    return(true);
  }
  else {
    MOOSTrace("Parse Error in receivePoint. \n");
    MOOSTrace(" String: %s \n", Msg.m_sVal.c_str());
    return(false);
  }
}

//----------------------------------------------------------------------
// Procedure: receiveStationCircle

bool SSV_MOOSApp::receiveStationCircle(CMOOSMsg &Msg)
{
  XYCircle new_circ;
  
  bool ok = new_circ.initialize(Msg.m_sVal);
  if(ok) {
    m_gui->mviewer->addStationCircle(new_circ);
    return(true);
  }
  else {
    cout << "Parse Error in receiveStationCircle" << endl;
    return(false);
  }
}

//----------------------------------------------------------------------
// Procedure: receiveGRID_DELTA

void SSV_MOOSApp::receiveGRID_DELTA(CMOOSMsg &Msg)
{
  //cout << "RECEIVED GRID ----------- DELTA   !!!!" << endl << flush;
  //cout << "   Msg.m_sVal:" << Msg.m_sVal << endl << flush;
  m_gui->mviewer->updateGrid(Msg.m_sVal);
}

//----------------------------------------------------------------------
// Procedure: handlePendingGUI

void SSV_MOOSApp::handlePendingGUI()
{
  if(!m_gui)
    return;
  
  int pendingSize = m_gui->getPendingSize();

  for(int i=0; i<pendingSize; i++) {
    string var = m_gui->getPendingVar(i);
    string val = m_gui->getPendingVal(i);
    m_Comms.Notify(var, val);
  }
  m_gui->clearPending();
}


//----------------------------------------------------------------------
// Procedure: handleContactList

bool SSV_MOOSApp::handleContactList(string clist)
{
  vector<string> svector = parseString(clist, ',');
  int vsize = svector.size();

  for(int i=0; i<vsize; i++) {
    cout << "[" << i << "]: " << svector[i] << endl;
    m_gui->addContactButton(i, svector[i]);
  }
  return(true);
}


