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

bool PMV_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  NewMail.sort();
  
  bool gui_needs_redraw = false;
  MOOSMSG_LIST::reverse_iterator p;

  //cout  << NewMail.size() << "," << flush;
  for(p = NewMail.rbegin();p!=NewMail.rend();p++) {
    CMOOSMsg &Msg = *p;
    
    if(Msg.m_sKey == "AIS_REPORT") {
      cout << "*" << flush;
      receiveAIS_REPORT(Msg);
      gui_needs_redraw = true;
    }
    else if(Msg.m_sKey == "AIS_REPORT_LOCAL") {
      cout << "*" << flush;
      receiveAIS_REPORT(Msg);
      gui_needs_redraw = true;
    }
    else if(Msg.m_sKey == "GRID_CONFIG") { 
      receiveGRID_CONFIG(Msg);
      gui_needs_redraw = true;
    }
    else if(Msg.m_sKey == "GRID_DELTA") { 
      receiveGRID_DELTA(Msg);
      gui_needs_redraw = true;
    }
    else if(Msg.m_sKey == "VIEW_POLYGON") { 
      receivePolygon(Msg);
      gui_needs_redraw = true;
    }
    else if(Msg.m_sKey == "VIEW_POINT") { 
      receivePoint(Msg);
      gui_needs_redraw = true;
    }
    else if(Msg.m_sKey == "INFO_POLYGON") { 
      receivePolygon(Msg);
      gui_needs_redraw = true;
    }
    else {
      MOOSTrace("Unknown msg [%s]\n",Msg.m_sKey.c_str());
    }
    //cout << "Originating community: " << Msg.m_sOriginatingCommunity << endl;
  }
  if(gui_needs_redraw && gui) {
    gui->updateXY();
    gui->mviewer->redraw();
  }
  return(true);
}

bool PMV_MOOSApp::OnConnectToServer()
{
  m_Comms.Register("AIS_REPORT", 0);
  m_Comms.Register("AIS_REPORT_LOCAL", 0);
  m_Comms.Register("GRID_CONFIG", 0);
  m_Comms.Register("GRID_DELTA", 0);
  m_Comms.Register("VIEW_POLYGON", 0);
  m_Comms.Register("VIEW_POINT", 0);
  m_Comms.Register("INFO_POLYGON", 0);
  return(true);
}

//----------------------------------------------------------------------
// Procedure: Iterate
//      Note: virtual overide of base class CMOOSApp member.

bool PMV_MOOSApp::Iterate()
{
  cout << "." << flush;

  double curr_time = MOOSTime() - start_time;
  gui->setCurrTime(curr_time);

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

  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine    = *p;
    string sVarName = MOOSChomp(sLine, "=");
    sVarName = toupper(sVarName);
    sLine    = stripBlankEnds(sLine);
    
    if(MOOSStrCmp(sVarName, "TIF_FILE"))
      tif_file = sLine;
  }

  if(tif_file != "")
    gui->readTiff(tif_file.c_str());

  start_time = MOOSTime();
  
  return(true);
}


//----------------------------------------------------------------------
// Procedure: receiveAIS_REPORT

bool PMV_MOOSApp::receiveAIS_REPORT(CMOOSMsg &Msg)
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

  if(community != vessel_name) {
    cout << "Community - Vessel_Name mismatch in receiveAIS_REPORT" << endl;
    return(false);
  }

  if(bX && bY && bHeading && bSpeed && bDepth) {
    if(gui) {
      gui->mviewer->updateVehiclePosition(community, dfX, dfY, 
					  dfHeading, dfSpeed, dfDepth);
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

bool PMV_MOOSApp::receiveGRID_CONFIG(CMOOSMsg &Msg)
{
  cout << "RECEIVED GRID CONFIGURATION!!!!" << endl << flush;
  cout << "   Msg.m_sVal:" << Msg.m_sVal << endl << flush;
  XYGrid search_grid;
  
  bool ok = search_grid.initialize(Msg.m_sVal);
  if(ok) {
    gui->addGrid(search_grid);
    return(true);
  }
  else {
    cout << "Parse Error in receiveGRID_CONFIG" << endl;
    return(false);
  }
}

//----------------------------------------------------------------------
// Procedure: receivePolygon

bool PMV_MOOSApp::receivePolygon(CMOOSMsg &Msg)
{
  //cout << "RECEIVED POLYGON!!!!" << endl << flush;
  //cout << "   Msg.m_sVal:" << Msg.m_sVal << endl << flush;
  XYPolygon new_poly;
  
  bool ok = new_poly.initialize(Msg.m_sVal);
  if(ok) {
    gui->addPoly(new_poly);
    return(true);
  }
  else {
    cout << "Parse Error in receivePolygon" << endl;
    return(false);
  }
}

//----------------------------------------------------------------------
// Procedure: receivePoint

bool PMV_MOOSApp::receivePoint(CMOOSMsg &Msg)
{
  cout << "Received VIEW_POINT" << endl;
  XYCircle new_circ;
  
  bool ok = new_circ.initialize(Msg.m_sVal);
  if(ok) {
    gui->addCircle(new_circ);
    return(true);
  }
  else {
    cout << "Parse Error in receivePoint" << endl;
    return(false);
  }
}

//----------------------------------------------------------------------
// Procedure: receiveGRID_DELTA

void PMV_MOOSApp::receiveGRID_DELTA(CMOOSMsg &Msg)
{
  cout << "RECEIVED GRID ----------- DELTA   !!!!" << endl << flush;
  cout << "   Msg.m_sVal:" << Msg.m_sVal << endl << flush;
  gui->mviewer->updateGrid(Msg.m_sVal);
}








