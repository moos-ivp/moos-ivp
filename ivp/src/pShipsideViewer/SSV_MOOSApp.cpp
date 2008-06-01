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

bool SSV_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  NewMail.sort();
  
  bool gui_needs_redraw = false;
  bool gui_clear_trails = false;
  MOOSMSG_LIST::reverse_iterator p;

  double curr_time = MOOSTime() - m_start_time;
  m_gui->mviewer->setTime(curr_time);

  //cout  << NewMail.size() << "," << flush;
  for(p = NewMail.rbegin();p!=NewMail.rend();p++) {
    CMOOSMsg &Msg = *p;

    string key = Msg.m_sKey;

    if(key == "AIS_REPORT") {
      cout << "*" << flush;
      receiveAIS_REPORT(Msg);
      gui_needs_redraw = true;
    }
    else if(key == "AIS_REPORT_LOCAL") {
      cout << "*" << flush;
      receiveAIS_REPORT(Msg);
      gui_needs_redraw = true;
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
  if(gui_needs_redraw && m_gui) {
    m_gui->updateXY();
    m_gui->mviewer->redraw();
  }
  if(gui_clear_trails && m_gui)
    m_gui->mviewer->clearTrails();

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
  m_gui->mviewer->setTime(curr_time);
  m_gui->updateXY();
  
  int left_click_ix = m_gui->mviewer->getLeftClickIX();
  if(left_click_ix > m_left_click_ix) {
    m_left_click_ix = left_click_ix;
    string left_click_str = m_gui->mviewer->getLeftClick();
    
    if(left_click_str != "") {
      int    index   = m_gui->mviewer->getDataIndex();
      string vname   = m_gui->mviewer->getVehiName(index);
      string postval = left_click_str;
      if(vname != "")
	postval += (",vname=" + vname);
      m_Comms.Notify("MVIEWER_LCLICK", postval);
    }
  }

  int right_click_ix = m_gui->mviewer->getRightClickIX();
  if(right_click_ix > m_right_click_ix) {
    m_right_click_ix = right_click_ix;
    string right_click_str    = m_gui->mviewer->getRightClick();
    string right_click_rp_str = m_gui->mviewer->getRightClickRP();
    
    if(right_click_str != "") {
      int    index  = m_gui->mviewer->getDataIndex();
      string vname  = m_gui->mviewer->getVehiName(index);
      if(vname != "") {
	right_click_str += (",vname=" + vname);
	right_click_rp_str += (",vname=" + vname);
      }
      m_Comms.Notify("MVIEWER_RCLICK", right_click_str);
      m_Comms.Notify("MVIEWER_RCLICK_RP", right_click_rp_str);
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
  string op_area;

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


  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine    = *p;
    string sVarName = MOOSChomp(sLine, "=");
    sVarName = toupper(sVarName);
    sLine    = stripBlankEnds(sLine);
    
    if(MOOSStrCmp(sVarName, "OP_AREA"))
      op_area = tolower(sLine);
    if(MOOSStrCmp(sVarName, "VEHICOLOR"))
      m_gui->mviewer->colorMapping(sLine);
    if(MOOSStrCmp(sVarName, "OWNSHIP_NAME"))
      m_gui->mviewer->setOwnShipName(sLine);
    if(MOOSStrCmp(sVarName, "CONTACTS"))
      handleContactList(sLine);
    if(MOOSStrCmp(sVarName, "PORTABLE_RANGE_SENSOR"))
      handleMarker("portable_range_sensor", sLine);
    if(MOOSStrCmp(sVarName, "GATEWAYA"))
      handleMarker("gateway_a", sLine);
    if(MOOSStrCmp(sVarName, "GATEWAYB"))
      handleMarker("gateway_b", sLine);
    if(MOOSStrCmp(sVarName, "EFIELD"))
      handleMarker("efield", sLine);
  }

  m_start_time = MOOSTime();
  
  if(m_gui && m_gui->mviewer)
    if(op_area != "")
      m_gui->mviewer->setParam("op_area", op_area);
    m_gui->mviewer->redraw();
  
  return(true);
}


//----------------------------------------------------------------------
// Procedure: receiveAIS_REPORT

bool SSV_MOOSApp::receiveAIS_REPORT(CMOOSMsg &Msg)
{
  string sVal = Msg.m_sVal;
  double dfX  = 0;
  double dfY  = 0; 

  double dfSpeed     = 0;
  double dfHeading   = 0;
  double dfDepth     = 0;
  string vessel_name = "";
  string vessel_type = "";

  string community(Msg.m_sOriginatingCommunity);

  bool bVName = tokParse(sVal, "NAME",   ',', '=', vessel_name);
  bool bVType = tokParse(sVal, "TYPE",   ',', '=', vessel_type);
  bool bX     = tokParse(sVal, "X",      ',', '=', dfX);
  bool bY     = tokParse(sVal, "Y",      ',', '=', dfY);
  bool bSpeed = tokParse(sVal, "SPD",    ',', '=', dfSpeed);
  bool bHeading = tokParse(sVal, "HDG",  ',', '=', dfHeading);
  bool bDepth = tokParse(sVal, "DEPTH",  ',', '=', dfDepth);

  if(bVName && bVType && bX && bY && bHeading && bSpeed && bDepth) {
    if(m_gui) {
      m_gui->mviewer->updateVehiclePosition(vessel_name, dfX, dfY, 
					    dfHeading, dfSpeed, dfDepth);
      m_gui->mviewer->setVehicleBodyType(vessel_name, vessel_type);
    }
    return(true);
  }
  else {
    cout << "Parse Error in receiveAIS_REPORT" << endl;
    return(false);
  }
}

//----------------------------------------------------------------------
// Procedure: receiveGRID_CONFIG

bool SSV_MOOSApp::receiveGRID_CONFIG(CMOOSMsg &Msg)
{
  cout << "RECEIVED GRID CONFIGURATION!!!!" << endl << flush;
  cout << "   Msg.m_sVal:" << Msg.m_sVal << endl << flush;
  XYGrid search_grid;
  
  bool ok = search_grid.initialize(Msg.m_sVal);
  if(ok) {
    m_gui->addGrid(search_grid);
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
    cout << "Receieved OK poly of size: " << new_poly.size() << endl;
    m_gui->addPoly(new_poly);
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
  cout << "In SSV_MOOSApp::receiveSegList()" << endl;
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
  XYCircle new_circ;
  
  bool ok = new_circ.initialize(Msg.m_sVal);
  if(ok) {
    m_gui->addCircle(new_circ);
    return(true);
  }
  else {
    cout << "Parse Error in receivePoint" << endl;
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
  cout << "RECEIVED GRID ----------- DELTA   !!!!" << endl << flush;
  cout << "   Msg.m_sVal:" << Msg.m_sVal << endl << flush;
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


//----------------------------------------------------------------------
// Procedure: handleMarker

bool SSV_MOOSApp::handleMarker(string marker_type, string clist)
{
  vector<string> svector = parseString(clist, ',');
  int vsize = svector.size();

  if(vsize != 3) {
    cout << "Error processing clist: " << clist  << "(" << vsize << ")" << endl;
    return(false);
  }

  for(int i=0; i<vsize; i++)
    svector[i] = stripBlankEnds(svector[i]);

  double x = atof(svector[0].c_str());
  double y = atof(svector[1].c_str());
  double s = atof(svector[2].c_str());
  
  cout << "Adding " << marker_type << ": " << endl;
  cout << "  X: " << x << " y: " << y << " S:" << s << endl;


  if(marker_type == "gateway_a")
    m_gui->mviewer->addGatewayA(x, y, s);

  if(marker_type == "gateway_b")
    m_gui->mviewer->addGatewayB(x, y, s);

  if(marker_type == "efield")
    m_gui->mviewer->addEField(x, y, s);

  if(marker_type == "portable_range_sensor")
    m_gui->mviewer->addRangeSensor(x, y, s);
  else
    return(false);

  return(true);
}


