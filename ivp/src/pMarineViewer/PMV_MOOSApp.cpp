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
#include "XYBuildUtils.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

PMV_MOOSApp::PMV_MOOSApp() 
{
  m_left_click_ix  = -1; 
  m_right_click_ix = -1; 

  m_gui     = 0; 
  m_verbose = false;
  m_vp_counter = 0;
}

//----------------------------------------------------------------
// Procedure: OnNewMail

bool PMV_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  NewMail.sort();
  
  bool gui_needs_redraw = false;
  bool gui_clear_trails = false;
  //MOOSMSG_LIST::reverse_iterator p;
  MOOSMSG_LIST::iterator p;

  double curr_time = MOOSTime() - m_start_time;
  m_gui->mviewer->setTime(curr_time);

  //if(NewMail.rbegin() != NewMail.rend()) {
  if(NewMail.begin() != NewMail.end()) {
    double curr_time = MOOSTime() - m_start_time;
    string ctime_str = doubleToString(curr_time, 2);
    MOOSTrace("\n%s > ", ctime_str.c_str());
    if(m_verbose)
      MOOSTrace("\n");
  }

  m_gui->mviewer->mutexLock();

  //for(p = NewMail.rbegin();p!=NewMail.rend();p++) {
  for(p = NewMail.begin();p!=NewMail.end();p++) {
    CMOOSMsg &Msg = *p;

    string key = Msg.m_sKey;

    if(key == "PK_SOL") {
      MOOSTrace("\nProcessing PK_SOL Message\n");
      receivePK_SOL(Msg);
      gui_needs_redraw = true;
    }
    else if((key == "AIS_REPORT_LOCAL") || (key == "AIS_REPORT")) {
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
      cout << "Num points recv'd: " << m_vp_counter++ << endl;
      gui_needs_redraw = true;
    }
    else if(key == "VIEW_CIRCLE") { 
      receivePoint(Msg);
      gui_needs_redraw = true;
    }
    else if(key == "TRAIL_RESET") { 
      gui_clear_trails = true;
    }
    else {
      MOOSTrace("Unknown msg [%s]\n",key.c_str());
    }
  }
  if(gui_needs_redraw && m_gui) {
    m_gui->updateXY();
    m_gui->mviewer->redraw();
  }
  if(gui_clear_trails && m_gui)
    m_gui->mviewer->clearTrails();

  m_gui->mviewer->mutexUnLock();

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
  double curr_time = MOOSTime() - m_start_time;

  if(m_gui) {
    m_gui->setCurrTime(curr_time);
    m_gui->updateXY();
  }

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
    string right_click_str = m_gui->mviewer->getRightClick();
    
    if(right_click_str != "") {
      int    index  = m_gui->mviewer->getDataIndex();
      string vname  = m_gui->mviewer->getVehiName(index);
      string postval = right_click_str;
      if(vname != "")
	postval += (",vname=" + vname);
      m_Comms.Notify("MVIEWER_RCLICK", postval);
    }
  }

  return(true);
}


//--------------------------------------------------------
// Procedure: onStartUp()

bool PMV_MOOSApp::OnStartUp()
{
  MOOSTrace("pMarineViewer starting....\n");
  
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  
  string tif_file = "Default.tif";


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


  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine    = *p;
    string sVarName = MOOSChomp(sLine, "=");
    sVarName = toupper(sVarName);
    sLine    = stripBlankEnds(sLine);
    double dval = atof(sLine.c_str());
    
    if(MOOSStrCmp(sVarName, "TIF_FILE"))
      tif_file = sLine;
    if(MOOSStrCmp(sVarName, "HASH_DELTA"))
      m_gui->mviewer->setParam("hash_delta", dval);
    if(MOOSStrCmp(sVarName, "HASH_SHADE"))
      m_gui->mviewer->setParam("hash_shade", dval);
    if(MOOSStrCmp(sVarName, "BACK_SHADE"))
      m_gui->mviewer->setParam("back_shade", dval);
    if(MOOSStrCmp(sVarName, "TRAIL_SIZE"))
      m_gui->mviewer->setParam("trail_size", dval);
    if(MOOSStrCmp(sVarName, "TRAIL_GAP"))
      m_gui->mviewer->setParam("trail_gap",  dval);
    if(MOOSStrCmp(sVarName, "ZOOM"))
      m_gui->mviewer->setParam("zoom",       dval);
    if(MOOSStrCmp(sVarName, "HASH_VIEW"))
      m_gui->mviewer->setParam("hash_view",  sLine);
    if(MOOSStrCmp(sVarName, "TIFF_VIEW"))
      m_gui->mviewer->setParam("tiff_view",  sLine);
    if(MOOSStrCmp(sVarName, "TRAIL_VIEW"))
      m_gui->mviewer->setParam("trail_view", sLine);
    if(MOOSStrCmp(sVarName, "DISPLAY_VNAME"))
      m_gui->mviewer->setParam("display_vname", sLine);

    if(MOOSStrCmp(sVarName, "VERBOSE")) {
      if(tolower(sLine) == "true")
	m_verbose = true;
      if(tolower(sLine) == "false")
	m_verbose = false;
    }
    if(MOOSStrCmp(sVarName, "VEHICOLOR"))
      m_gui->mviewer->setColorMapping(sLine);
  }

  //if((tif_file != "") && (m_gui))
  //  m_gui->readTiff(tif_file.c_str());

  m_start_time = MOOSTime();
  
  if(m_gui && m_gui->mviewer)
    m_gui->mviewer->redraw();
  
  registerVariables();
  return(true);
}


//---------------------------------------------------------------
// Procedure: parseSingleReport

bool PMV_MOOSApp::parseSingleReport(string sReport)
{
  bool returnStatus = false;
  double dfX  = 0;
  double dfY  = 0; 

  double dfSpeed     = 0;
  double dfHeading   = 0;
  double dfDepth     = 0;
  string vessel_name = "";
  string vessel_type = "";

  MOOSTrace("Received Msg = %s\n", sReport.c_str());

  bool bVName = tokParse(sReport, "NAME",   ',', '=', vessel_name);
  bool bVType = tokParse(sReport, "TYPE",   ',', '=', vessel_type);
  bool bX     = tokParse(sReport, "X",      ',', '=', dfX);
  bool bY     = tokParse(sReport, "Y",      ',', '=', dfY);
  bool bSpeed = tokParse(sReport, "SPD",    ',', '=', dfSpeed);
  bool bHeading = tokParse(sReport, "HDG",  ',', '=', dfHeading);
  bool bDepth = tokParse(sReport, "DEPTH",  ',', '=', dfDepth);

  if(m_verbose)
    MOOSTrace("   AIS(%s)\n", vessel_name.c_str());
  else
    MOOSTrace("*");
  
  if(bVName && bVType && bX && bY && bHeading && bSpeed && bDepth) {
    if(m_gui) {
      m_gui->mviewer->updateVehiclePosition(vessel_name, dfX, dfY, 
					    dfHeading, dfSpeed, dfDepth);
      m_gui->mviewer->setVehicleBodyType(vessel_name, vessel_type);
    }
    returnStatus = true;
  }
  else {
    MOOSTrace("Parse Error in ParseSingleReport. \n");
    MOOSTrace("Msg: %s\n", sReport.c_str());
    returnStatus = false;
  }
  return( returnStatus );
}

//---------------------------------------------------------------
// Procedure: receivePK_SOL

bool PMV_MOOSApp::receivePK_SOL(CMOOSMsg &Msg)
{
  bool returnStatus = true;
  bool singleStatus = false;
  vector<string> rvector;

  // REPORTS in PK_SOL message are separated by ";"

  vector<string> svector = parseString(Msg.m_sVal, ';');

  MOOSTrace("Received PK_SOL Msg\n");

  // cycle through all reports

  for(unsigned int i=0;i<svector.size()-1;i++) {
    // Remove first item of string - it is the type of report
    // (e.g. REPORT_TYPE = AIS_REPORT) so strip that off and 
    // proceed as before with the single AIS_REPORT messages
    
    rvector = chompString(svector[i], ',');
    if(rvector.size() > 1 ) {
      singleStatus = parseSingleReport( rvector[1].c_str());
      if(singleStatus == false) {
	returnStatus = false;
      }
    }
  }
  return(returnStatus);
}

//---------------------------------------------------------------
// Procedure: receiveAIS_REPORT

bool PMV_MOOSApp::receiveAIS_REPORT(CMOOSMsg &Msg)
{
  MOOSTrace("Received AIS_REPORT Msg\n");
  return( parseSingleReport( Msg.m_sVal ) );
}

//--------------------------------------------------------------
// Procedure: receiveGRID_CONFIG

bool PMV_MOOSApp::receiveGRID_CONFIG(CMOOSMsg &Msg)
{
  if(m_verbose)
    MOOSTrace("   Grid-Config\n");
  else
    MOOSTrace("X");

  XYGrid search_grid;
  
  bool ok = search_grid.initialize(Msg.m_sVal);
  if(ok) {
    m_gui->addGrid(search_grid);
    return(true);
  }
  else {
    MOOSTrace("Parse Error in receiveGridConfig. \n");
    MOOSTrace("Msg: %s\n", Msg.m_sVal.c_str());
    return(false);
  }
}

//----------------------------------------------------------
// Procedure: receivePolygon

bool PMV_MOOSApp::receivePolygon(CMOOSMsg &Msg)
{
  XYPolygon new_poly = stringToPoly(Msg.m_sVal);

  string label = "ERR";
  bool ok = (new_poly.size() != 0);
  if(ok)
    label = new_poly.get_label();
  
  if(m_verbose)
    MOOSTrace("   Poly(%s)\n",label.c_str());
  else
    MOOSTrace("P");

  if(ok) {
    m_gui->addPoly(new_poly);
    return(true);
  }
  else {
    MOOSTrace("Parse Error in receivePolygon. \n");
    MOOSTrace("Msg: %s\n", Msg.m_sVal.c_str());
    return(false);
  }
}

//----------------------------------------------------------
// Procedure: receiveSegList

bool PMV_MOOSApp::receiveSegList(CMOOSMsg &Msg)
{
  XYSegList new_seglist = stringToSegList(Msg.m_sVal);

  bool ok = (new_seglist.size() > 0);

  string label = "ERR";
  if(ok)
    label = new_seglist.get_label();
  
  if(m_verbose)
    MOOSTrace("   SegList(%s)\n", label.c_str());
  else
    MOOSTrace("S");

  if(ok) {
    m_gui->mviewer->addSegList(new_seglist);
    return(true);
  }
  else {
    MOOSTrace("Parse Error in receiveSegList. \n");
    MOOSTrace("Msg: %s\n", Msg.m_sVal.c_str());
    return(false);
  }
}

//----------------------------------------------------------
// Procedure: receivePoint

bool PMV_MOOSApp::receivePoint(CMOOSMsg &Msg)
{
  XYCircle new_circ;
  
  bool ok = new_circ.initialize(Msg.m_sVal);
  
  string label = "ERR";
  if(ok)
    label = new_circ.getLabel();
  
  if(m_verbose)
    MOOSTrace("   Point(%s)\n", label.c_str());
  else
    MOOSTrace(".");

  if(ok) {
    m_gui->addCircle(new_circ);
    return(true);
  }
  else {
    MOOSTrace("Parse Error in receivePoint. \n");
    MOOSTrace("Msg: %s\n", Msg.m_sVal.c_str());
    return(false);
  }
}

//----------------------------------------------------------
// Procedure: receiveGRID_DELTA

void PMV_MOOSApp::receiveGRID_DELTA(CMOOSMsg &Msg)
{
  string msg_community = Msg.GetCommunity();
  
  if(m_verbose)
    MOOSTrace("   GDelta(%s)\n", msg_community.c_str());
  else
    MOOSTrace("G");

  m_gui->mviewer->updateGrid(Msg.m_sVal);
}


//------------------------------------------------------------
// Procedure: registerVariables

void PMV_MOOSApp::registerVariables()
{
//  m_Comms.Register("AIS_REPORT", 0);
  m_Comms.Register("PK_SOL", 0);
  m_Comms.Register("AIS_REPORT_LOCAL", 0);
  m_Comms.Register("AIS_REPORT",       0);
  m_Comms.Register("GRID_CONFIG",      0);
  m_Comms.Register("GRID_DELTA",       0);
  m_Comms.Register("VIEW_POLYGON",     0);
  m_Comms.Register("VIEW_POINT",       0);
  m_Comms.Register("VIEW_SEGLIST",     0);
  m_Comms.Register("TRAIL_RESET",      0);
}


