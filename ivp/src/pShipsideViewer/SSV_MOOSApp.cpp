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

using namespace std;

//-----------------------------------------------------------------
// Procedure: Constructor

SSV_MOOSApp::SSV_MOOSApp() 
{
  m_gui = 0;

  // We hold a local copy of last click-string to repeatedly compare
  // against the currently generated one and only post when different
  m_left_click_str  = "null"; 
  m_right_click_str = "null";
}

//-----------------------------------------------------------------
// Procedure: OnNewMail

bool SSV_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
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
    if(!handled) {
      MOOSTrace("pMarineViewer OnNewMail Unhandled msg: \n");
      MOOSTrace("  [key:%s val:%s]\n", key.c_str(), sval.c_str());
    }
    else {
      if(key == "VIEW_POLYGON")          MOOSTrace("P");
      else if(key == "VIEW_SEGLIST")     MOOSTrace("S");
      else if(key == "VIEW_POINT")       MOOSTrace(".");
      else if(key == "GRID_CONFIG")      MOOSTrace("X");
      else if(key == "AIS_REPORT")       MOOSTrace("*");
      else if(key == "AIS_REPORT_LOCAL") MOOSTrace("*");
      else if(key == "GRID_CONFIG")      MOOSTrace("X");
      else if(key == "GRID_DELTA")       MOOSTrace("G");
      else MOOSTrace("?");
      handled_msgs++;
    }
  }

  if(handled_msgs > 0) {
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
  registerVariables();
  m_Comms.Notify("HELM_MAP_CLEAR", 0);
  return(true);
}

//----------------------------------------------------------------------
// Procedure: Iterate
//      Note: virtual overide of base class CMOOSApp member.

bool SSV_MOOSApp::Iterate()
{
  if(!m_gui)
    return(false);

  double curr_time = MOOSTime() - m_start_time;
  m_gui->setCurrTime(curr_time);
  m_gui->updateXY();  // do we need this here?

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
  string str;
  double lat_origin;
  double lon_origin;

  // First get the latitude origin from the mission file
  if(m_MissionReader.GetValue("LatOrigin", str))
    lat_origin = atof(str.c_str());
  else {
    MOOSTrace("LatOrigin not specified in mission file - FAIL\n");
    return(false);
  }
  // Next get the longitude origin from the mission file
  if(m_MissionReader.GetValue("LongOrigin", str))
    lon_origin = atof(str.c_str());
  else {
    MOOSTrace("LongOrigin not specified in mission file - FAIL\n");
    return(false);
  }
  // If both lat and lon - then initialize the geodesy
  if(!m_gui->mviewer->initGeodesy(lat_origin, lon_origin)) {
    MOOSTrace("Geodesy Init inside pShipSideViewer failed - FAIL\n");
    return(false);
  }

  // Keep track of whether the back images were user configured.
  // If not, we'll use the default image afterwards.
  bool tiff_a_set = false;
  bool tiff_b_set = false;

  STRING_LIST::reverse_iterator p;
  for(p = sParams.rbegin();p!=sParams.rend();p++) {
    string sLine = *p;
    string param = tolower(MOOSChomp(sLine, "="));
    string value = stripBlankEnds(sLine);
    
    if(param == "contacts") {
      vector<string> svector = parseString(value, ',');
      for(unsigned int i=0; i<svector.size(); i++)
	m_gui->addContactButton(i, svector[i]);
    }
    else { 
      // First try as if the param-value pair had a string value
      bool handled = m_gui->mviewer->setParam(param, value);
      // Then try as if the param-value pair had a double value
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
// Procedure: registerVariables

void SSV_MOOSApp::registerVariables()
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
}
