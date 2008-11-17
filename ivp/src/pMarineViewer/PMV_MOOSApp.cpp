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
  m_left_click_str  = "null"; 
  m_right_click_str = "null"; 

  m_gui     = 0; 
  m_verbose = false;
}

//----------------------------------------------------------------
// Procedure: OnNewMail

bool PMV_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  if(!m_gui) 
    return(true);

  NewMail.sort();

  m_gui->mviewer->setParam("curr_time", MOOSTime());

  int handled_msgs = 0;

  MOOSMSG_LIST::iterator p;
  for(p = NewMail.begin();p!=NewMail.end();p++) {
    CMOOSMsg &Msg = *p;
    
    string key  = Msg.m_sKey;
    string sval = Msg.m_sVal;
    
    bool handled = m_gui->mviewer->setParam(key, sval);
    if(!handled && (key == "PK_SOL")) {
      MOOSTrace("\nProcessing PK_SOL Message\n");
      receivePK_SOL(Msg);
    }
    if(!handled) {
      MOOSTrace("pMarineViewer OnNewMail Unhandled msg: \n");
      MOOSTrace("  [key:%s val:%s]\n", key.c_str(), sval.c_str());
    }
    
    if(key == "VIEW_POLYGON")          MOOSTrace("P");
    else if(key == "VIEW_SEGLIST")     MOOSTrace("S");
    else if(key == "VIEW_POINT")       MOOSTrace(".");
    else if(key == "GRID_CONFIG")      MOOSTrace("X");
    else if(key == "AIS_REPORT")       MOOSTrace("*");
    else if(key == "AIS_REPORT_LOCAL") MOOSTrace("*");
    else if(key == "GRID_CONFIG")      MOOSTrace("X");
    else if(key == "GRID_DELTA")       MOOSTrace("G");
    else if(key == "VIEW_MARKER")      MOOSTrace("M");
    else MOOSTrace("?");

    if(handled)
      handled_msgs++;
  }

  if(handled_msgs > 0) {
    m_gui->updateXY();
    m_gui->mviewer->redraw();
  }

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
  if(!m_gui)
    return(false);

  double curr_time = MOOSTime() - m_start_time;
  m_gui->setCurrTime(curr_time);
  m_gui->updateXY();

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

  return(true);
}


//--------------------------------------------------------
// Procedure: onStartUp()

bool PMV_MOOSApp::OnStartUp()
{
  MOOSTrace("pMarineViewer starting....\n");
  
  //Initialize m_Geodesy from lat lon origin in .moos file
  string str;
  double lat_origin = 0;
  double lon_origin = 0;

  // First get the Latitude Origine from the MOOS file
  if(m_MissionReader.GetValue("LatOrigin", str))
    lat_origin = atof(str.c_str());
  else {
    MOOSTrace("LatOrigin not specified in mission file - FAIL\n");
    return(false);
  }

  // First get the Londitude Origine from the MOOS file
  if(m_MissionReader.GetValue("LongOrigin",str))
    lon_origin = atof(str.c_str());
  else {
    MOOSTrace("LongOrigin not specified in mission file - FAIL\n");
    return(false);
  }
  
  // If both lat and lon origin ok - then initialize the Geodesy.
  if(!m_gui->mviewer->initGeodesy(lat_origin, lon_origin)) {
    MOOSTrace("Geodesy Init inside pMarineViewer failed - FAIL\n");
    return(false);
  }

  // Keep track of whether the back images were user configured.
  // If not, we'll use the default image afterwards.
  bool tiff_a_set = false;
  bool tiff_b_set = false;

  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  STRING_LIST::reverse_iterator p;
  for(p = sParams.rbegin();p!=sParams.rend();p++) {
    string sLine    = *p;
    string param = tolower(MOOSChomp(sLine, "="));
    string value = stripBlankEnds(sLine);
    
    if(param == "verbose")
      m_verbose = (tolower(value) == "true");
    else { 
      bool handled = m_gui->mviewer->setParam(param, value);
      if(!handled)
	handled = m_gui->mviewer->setParam(param, atof(value.c_str()));
      if(handled && (param == "tiff_file"))
	tiff_a_set = true;
      if(handled && (param == "tiff_file_b"))
	tiff_b_set = true;	
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
  return(true);
}


//---------------------------------------------------------------
// Procedure: receivePK_SOL

bool PMV_MOOSApp::receivePK_SOL(CMOOSMsg &Msg)
{
  bool return_status = true;
  vector<string> rvector;

  // REPORTS in PK_SOL message are separated by ";"

  vector<string> svector = parseString(Msg.m_sVal, ';');

  // Cycle through all reports
  for(unsigned int i=0;i<svector.size()-1;i++) {
    // Remove first item of string - it is the type of report
    // (e.g. REPORT_TYPE = AIS_REPORT) so strip that off and 
    // proceed as before with the single AIS_REPORT messages
    
    rvector = chompString(svector[i], ',');
    if(rvector.size() > 1 ) {
      return_status = m_gui->mviewer->setParam("ais_report", rvector[1].c_str());
    }
  }
  return(return_status);
}

//------------------------------------------------------------
// Procedure: registerVariables

void PMV_MOOSApp::registerVariables()
{
  m_Comms.Register("PK_SOL", 0);
  m_Comms.Register("AIS_REPORT_LOCAL", 0);
  m_Comms.Register("AIS_REPORT",       0);
  m_Comms.Register("GRID_CONFIG",      0);
  m_Comms.Register("GRID_DELTA",       0);
  m_Comms.Register("VIEW_POLYGON",     0);
  m_Comms.Register("VIEW_POINT",       0);
  m_Comms.Register("VIEW_SEGLIST",     0);
  m_Comms.Register("TRAIL_RESET",      0);
  m_Comms.Register("VIEW_MARKER",      0);
}


