/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TransponderAIS.cpp                                   */
/*    DATE: Feb 13th 2006                                        */
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

#include <string>
#include <list>
#include <iterator>
#include <iostream>
#include "TransponderAIS.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: Constructor

TransponderAIS::TransponderAIS()
{
  nav_x = 0;
  nav_y = 0;
  nav_speed = 0;
  nav_heading = 0;
  nav_depth = 0;
  vessel_name = "UNKNOWN_VESSEL_NAME";
  vessel_type = "UNKNOWN_VESSEL_TYPE";
}

//-----------------------------------------------------------------
// Procedure: OnNewMail

bool TransponderAIS::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::reverse_iterator p;
  
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.m_sKey;
    string community = msg.m_sOriginatingCommunity;
    string sdata = msg.m_sVal;
    double ddata = msg.m_dfVal;
    char   mtype = msg.m_cDataType;

    if(key == "NAV_X")
      nav_x = ddata;
    else if(key == "NAV_Y")
      nav_y = ddata;
    else if(key == "NAV_SPEED")
      nav_speed = ddata;
    else if(key == "NAV_HEADING")
      nav_heading = ddata;
    else if(key == "NAV_YAW")
      nav_heading = (ddata*-180.0)/3.1415926;
    else if(key == "NAV_DEPTH")
      nav_depth = ddata;
    else if(key == "AIS_REPORT") {
      bool ok = handleIncomingAISReport(sdata);
      if(!ok) 
	MOOSTrace("TransponderAIS: Un-Parsed AIS-Report.\n");
    }
    else if(key == "CONTACT_STAT") {
      bool ok = handleIncomingCSReport(sdata);
      if(!ok) 
	MOOSTrace("TransponderAIS: Un-Parsed CONTACT_STAT Report.\n");
    }
    else {
      MOOSTrace("TransponderAIS: Unknown msg [%s]\n",msg.m_sKey.c_str());
    }
  }
  return(true);
}

//-----------------------------------------------------------------
// Procedure: handleIncomingAISReport()

bool TransponderAIS::handleIncomingAISReport(const string& sdata)
{
  string vname;         bool vname_set   = false;
  double nav_x_val;     bool nav_x_set   = false;
  double nav_y_val;     bool nav_y_set   = false;
  double nav_spd_val;   bool nav_spd_set = false;
  double nav_hdg_val;   bool nav_hdg_set = false;
  double nav_dep_val;   bool nav_dep_set = false;

  vector<string> svector = parseString(sdata, ',');
  int vsize = svector.size();
  for(int i=0; i<vsize; i++) {
    vector<string> svector2 = parseString(svector[i], '=');
    if(svector2.size() == 2) {
      string left = stripBlankEnds(svector2[0]);
      string right = stripBlankEnds(svector2[1]);
      
      if(left=="NAME") {
	vname = right;
	vname_set = true;
      }
      if(left == "X") {
	nav_x_val = atof(right.c_str());
	nav_x_set = true;
      }
      if(left == "Y") {
	nav_y_val = atof(right.c_str());
	nav_y_set = true;
      }
      if(left == "SPD") {
	nav_spd_val = atof(right.c_str());
	nav_spd_set = true;
      }
      if(left == "HDG") {
	nav_hdg_val = atof(right.c_str());
	nav_hdg_set = true;
      }
      if(left == "DEPTH") {
	nav_dep_val = atof(right.c_str());
	nav_dep_set = true;
      }
    }
  }

  if(!vname_set || !nav_x_set || !nav_y_set ||
     !nav_spd_set || !nav_hdg_set || !nav_dep_set)
    return(false);


  vname = toupper(vname);
  
  m_Comms.Notify(vname+"_NAV_X", nav_x_val);
  m_Comms.Notify(vname+"_NAV_Y", nav_y_val);
  m_Comms.Notify(vname+"_NAV_SPEED", nav_spd_val);
  m_Comms.Notify(vname+"_NAV_HEADING", nav_hdg_val);
  m_Comms.Notify(vname+"_NAV_DEPTH", nav_hdg_val);

  return(true);
}

//-----------------------------------------------------------------
// Procedure: handleIncomingCSReport()

bool TransponderAIS::handleIncomingCSReport(const string& sdata)
{
  string vname;         bool vname_set   = false;
  double nav_x_val;     bool nav_x_set   = false;
  double nav_y_val;     bool nav_y_set   = false;
  double nav_spd_val;   bool nav_spd_set = false;
  double nav_hdg_val;   bool nav_hdg_set = false;
  double nav_dep_val;   bool nav_dep_set = false;

  vector<string> svector = parseString(sdata, ',');
  int vsize = svector.size();
  for(int i=0; i<vsize; i++) {
    vector<string> svector2 = parseString(svector[i], '=');
    if(svector2.size() == 2) {
      string left = tolower(stripBlankEnds(svector2[0]));
      string right = stripBlankEnds(svector2[1]);
      
      if(left=="node") {
	vname = right;
	vname_set = true;
      }
      if(left == "x") {
	nav_x_val = atof(right.c_str());
	nav_x_set = true;
      }
      if(left == "y") {
	nav_y_val = atof(right.c_str());
	nav_y_set = true;
      }
      if(left == "speed") {
	nav_spd_val = atof(right.c_str());
	nav_spd_set = true;
      }
      if(left == "heading") {
	nav_hdg_val = atof(right.c_str());
	nav_hdg_set = true;
      }
      if(left == "depth") {
	nav_dep_val = atof(right.c_str());
	nav_dep_set = true;
      }
    }
  }

  if(!vname_set || !nav_x_set || !nav_y_set ||
     !nav_spd_set || !nav_hdg_set)
    return(false);
  

  vname = toupper(vname);
  
  m_Comms.Notify(vname+"_NAV_X", nav_x_val);
  m_Comms.Notify(vname+"_NAV_Y", nav_y_val);
  m_Comms.Notify(vname+"_NAV_SPEED", nav_spd_val);
  m_Comms.Notify(vname+"_NAV_HEADING", nav_hdg_val);
  if(nav_dep_set)
    m_Comms.Notify(vname+"_NAV_DEPTH", nav_dep_val);

  return(true);
}

//-----------------------------------------------------------------
// Procedure: OnConnectoToServer()

bool TransponderAIS::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));
 
  m_Comms.Register("NAV_X", 0);
  m_Comms.Register("NAV_Y", 0);
  m_Comms.Register("NAV_SPEED", 0);
  m_Comms.Register("NAV_HEADING", 0);
  m_Comms.Register("NAV_YAW", 0);
  m_Comms.Register("NAV_DEPTH", 0);
  m_Comms.Register("AIS_REPORT", 0);
  return(true);
}

//-----------------------------------------------------------------
// Procedure: Iterate()
//      Note: Happens AppTick times per second

bool TransponderAIS::Iterate()
{
  string timeinfo = dstringCompact(doubleToString(MOOSTime()-start_time));
  string summary = "NAME=" + vessel_name;
  summary += ",TYPE=" + vessel_type;
  summary += ",TIME=" + timeinfo;
  summary += ",X="   + dstringCompact(doubleToString(nav_x));
  summary += ",Y="   + dstringCompact(doubleToString(nav_y));
  summary += ",SPD=" + dstringCompact(doubleToString(nav_speed));
  summary += ",HDG=" + dstringCompact(doubleToString(nav_heading));
  summary += ",DEPTH=" + dstringCompact(doubleToString(nav_depth));

  m_Comms.Notify("AIS_REPORT_LOCAL", summary);

  return(true);
}


//-----------------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool TransponderAIS::OnStartUp()
{
  CMOOSApp::OnStartUp();

  MOOSTrace("pTransponderAIS starting....\n");

  start_time = MOOSTime();

  // Look for ownship name first - a "global" variable in the 
  // Configuration file. 
  if(!m_MissionReader.GetValue("Community", vessel_name)) {
    MOOSTrace("Vehicle (Community) Name not provided\n");
    return(false);
  }
  
  list<string> sParams;
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    
    list<string>::iterator p;
    for(p = sParams.begin();p!=sParams.end();p++) {
      string sLine    = *p;
      string sVarName = MOOSChomp(sLine, "=");

      if(MOOSStrCmp(sVarName, "VESSEL_TYPE")) {
	vessel_type = stripBlankEnds(sLine);
      }
    }
  }
  return(true);
}








