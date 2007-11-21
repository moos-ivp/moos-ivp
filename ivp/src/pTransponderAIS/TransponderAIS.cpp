/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    NaFCon Functionality: Toby Schneider tes@mit.edu           */
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

#include <iostream>
#include <list>
#include <iterator>
#include "TransponderAIS.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: Constructor

TransponderAIS::TransponderAIS()
{
  m_nav_x       = 0;
  m_nav_y       = 0;
  m_nav_speed   = 0;
  m_nav_heading = 0;
  m_nav_depth   = 0;
  m_vessel_name = "UNKNOWN_VESSEL_NAME";
  m_vessel_type = "UNKNOWN_VESSEL_TYPE";
  m_parseNaFCon = false;

  m_blackout_interval = 0;
  m_blackout_baseval  = 0;
  m_blackout_variance = 0;
  m_last_post_time    = -1;
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
      m_nav_x = ddata;
    else if(key == "NAV_Y")
      m_nav_y = ddata;
    else if(key == "NAV_SPEED")
      m_nav_speed = ddata;
    else if(key == "NAV_HEADING")
      m_nav_heading = ddata;
    else if(key == "NAV_YAW")
      m_nav_heading = (ddata*-180.0)/3.1415926;
    else if(key == "NAV_DEPTH")
      m_nav_depth = ddata;
    else if(key == "AIS_REPORT") {
      bool ok = handleIncomingAISReport(sdata);
      if(!ok) 
	MOOSTrace("TransponderAIS: Un-Parsed AIS-Report.\n");
    }
    // tes 9-12-07 added support for NAFCON_MESSAGES
    else if((key == "NAFCON_MESSAGES") || (key == "NAFCON_MESSAGES_WLAN")) {
      if(m_parseNaFCon) {
	bool ok = handleIncomingNaFConMessage(sdata);
	if (!ok)
	  MOOSTrace("TransponderAIS: Unparsed NaFConMessage.\n");
      }
    }
    // end tes 9-12-07

    else {
      MOOSTrace("TransponderAIS: Unknown msg [%s]\n",msg.m_sKey.c_str());
    }
  }
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
  
  // tes 9-12-07. added NAFCON_MESSAGES registration
  m_Comms.Register("NAFCON_MESSAGES", 0);
  m_Comms.Register("NAFCON_MESSAGES_WLAN", 0);

  return(true);
}

//-----------------------------------------------------------------
// Procedure: Iterate()
//
// Note: Happens AppTick times per second
// Note: The "blackout_interval" is a way to have a vehicle simulate 
//       a vehicle that is only making period reports available to 
//       other vehicles. Even if this effect in the real world is due
//       more to transmission loss or latency.
//    

bool TransponderAIS::Iterate()
{
  double moos_time = MOOSTime();
  
  // If (a) this is the first chance to post, or (b) there is no
  // blackout interval being implemented, or (c) the time since last
  // post exceeds the blackout interval, then perform a posting.

  if((m_last_post_time == -1) || (m_blackout_interval <= 0) ||
     ((moos_time-m_last_post_time) > m_blackout_interval)) {

    string timeinfo = dstringCompact(doubleToString(MOOSTime() - m_start_time));
    string summary = "NAME=" + m_vessel_name;
    summary += ",TYPE=" + m_vessel_type;
    summary += ",TIME=" + timeinfo;
    summary += ",X="   + dstringCompact(doubleToString(m_nav_x));
    summary += ",Y="   + dstringCompact(doubleToString(m_nav_y));
    summary += ",SPD=" + dstringCompact(doubleToString(m_nav_speed));
    summary += ",HDG=" + dstringCompact(doubleToString(m_nav_heading));
    summary += ",DEPTH=" + dstringCompact(doubleToString(m_nav_depth));
    
    m_Comms.Notify("AIS_REPORT_LOCAL", summary);
    m_last_post_time = moos_time;

    int    range = (int)(m_blackout_variance * 100.0);
    int    val   = rand() % range;
    double delta = (double)((double)val / 100.0);


    m_blackout_interval  = m_blackout_baseval;
    m_blackout_interval -= m_blackout_variance;
    m_blackout_interval += (delta * 2.0);
    if(m_blackout_interval < 0)
      m_blackout_interval = 0;

#if 0
    interval_history += ",";
    interval_history += doubleToString(m_blackout_interval, 2);
    m_Comms.Notify("AIS_HISTORY", interval_history);
#endif

  }

  postContactList();

  return(true);
}


//-----------------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool TransponderAIS::OnStartUp()
{
  CMOOSApp::OnStartUp();

  MOOSTrace("pTransponderAIS starting....\n");

  m_start_time = MOOSTime();

  // Look for ownship name first - a "global" variable in the 
  // Configuration file. 
  if(!m_MissionReader.GetValue("Community", m_vessel_name)) {
    MOOSTrace("Vehicle (Community) Name not provided\n");
    return(false);
  }
  
  // tes 9-12-07
  // look for latitude, longitude global variables
  double latOrigin, longOrigin;
  if (!m_MissionReader.GetValue("LatOrigin", latOrigin))
    return MOOSFail("LatOrigin not set in *.moos file.\n");
      
  if (!m_MissionReader.GetValue("LongOrigin", longOrigin))
    return MOOSFail("LongOrigin not set in *.moos file\n");
  
  // initialize m_Geodesy
  if (!m_Geodesy.Initialise(latOrigin, longOrigin))
    return MOOSFail("Geodesy init failed.\n");

  // initialize naFConPublishForID to all false before adding
  //  config entries. we will add true values for NaFCoN
  //  ids we wish to publish for
  naFConPublishForID.assign(32, false);
  bool publishingSpecified = false;

  list<string> sParams;
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    
    list<string>::iterator p;
    for(p = sParams.begin();p!=sParams.end();p++) {

      string sLine    = stripBlankEnds(*p);
      string sVarName = MOOSChomp(sLine, "=");

      if(MOOSStrCmp(sVarName, "VESSEL_TYPE"))
	m_vessel_type = sLine;
      
      if(MOOSStrCmp(sVarName, "BLACKOUT_INTERVAL")) {
	double dval = atof(sLine.c_str());
	if(dval >= 0)
	  m_blackout_baseval  = dval;
	  m_blackout_interval = dval;
      }
      
      if(MOOSStrCmp(sVarName, "BLACKOUT_VARIANCE")) {
	double dval = atof(sLine.c_str());
	if(dval >= 0)
	  m_blackout_variance = dval;
      }
      
      if(MOOSStrCmp(sVarName, "PARSE_NAFCON")) 
	m_parseNaFCon = MOOSStrCmp(sLine, "true"); 
      
      // for each publish_for_nafcon_id config value
      // insert a true in the appropriate entry of the 
      // naFConPublishForID vector.
      if(MOOSStrCmp(sVarName, "PUBLISH_FOR_NAFCON_ID")) {
	int id = atoi(sLine.c_str());
	naFConPublishForID[id] = true;
	// since the user specified id(s), do not publish
	// AIS_REPORT for all ids
	publishingSpecified = true;
      }
    }
    // if no IDs to publish are specified, publish for ALL ids
    // (true for all 32 entries of naFConPublishForID)
    if (!publishingSpecified)
      naFConPublishForID.assign(32,true);
  }
  return(true);
  // end tes 9-12-07
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
  
  updateContactList(vname);

  m_Comms.Notify(vname+"_NAV_X", nav_x_val);
  m_Comms.Notify(vname+"_NAV_Y", nav_y_val);
  m_Comms.Notify(vname+"_NAV_SPEED", nav_spd_val);
  m_Comms.Notify(vname+"_NAV_HEADING", nav_hdg_val);
  m_Comms.Notify(vname+"_NAV_DEPTH", nav_dep_val);

  return(true);
}


//-----------------------------------------------------------------
// Procedure: handleIncomingNaFConMessage()
//
//  tes 9-12-97
//  process a NaFCon Status Report and publish
//  an AIS_REPORT.
//

bool TransponderAIS::handleIncomingNaFConMessage(const string& rMsg)
{    
  //check message type                                                                                 
  string messageType = "unknown";
  
  if (!(MOOSValFromString(messageType, rMsg, "MessageType"))) {
    MOOSTrace("pTransponderAIS: Bad message. No message type found.\n");
    return false;
  } 

  MOOSTrace("Began parsing NaFCon message to AIS_REPORT:\n");
  MOOSTrace(rMsg);
  MOOSTrace("\n\n");
  
  //we have a status message                                                                           
  if(MOOSStrCmp(messageType, "SENSOR_STATUS")) {
    string sourceID;
    MOOSValFromString(sourceID, rMsg, "SourcePlatformId");
    
    // limit to vehicles specified in config file
    // and now stored in naFConPublishForID
    if(naFConPublishForID[atoi(sourceID.c_str())]) {
      MOOSTrace("Will publish for this ID. \n");
      double navX, navY, navLat, navLong, navHeading, navSpeed, navDepth, navTime;
      if(!MOOSValFromString(navLong, rMsg, "NodeLongitude"))
	return MOOSFail("No NodeLongitude\n");
      
      if (!MOOSValFromString(navLat, rMsg, "NodeLatitude"))
	return MOOSFail("No NodeLatitude\n");
      
      if (!MOOSValFromString(navHeading, rMsg, "NodeHeading"))
	return MOOSFail("No NodeHeading\n");
      
      if (!MOOSValFromString(navSpeed, rMsg, "NodeSpeed"))
	return MOOSFail("No NodeSpeed\n");
      
      if(!MOOSValFromString(navDepth, rMsg, "NodeDepth"))
	return MOOSFail("No NodeDepth\n");      

      if(!MOOSValFromString(navTime, rMsg, "Timestamp"))
	return MOOSFail("No Timestamp\n");      

      // convert lat, long into x, y. 60 nautical miles per minute
      if(!m_Geodesy.LatLong2LocalGrid(navLat, navLong, navY, navX))
	return MOOSFail("Geodesy conversion failed\n");
      

      string vtype = "GLIDER";
      string vname = sourceID;
      if(sourceID == "1") {
	vtype = "AUV";
	vname = "(1)Sea-Horse";
      }
      if(sourceID == "2") {
	vtype = "KAYAK";
	vname = "(2)Bobby";
      }
      if(sourceID == "3") {
	vtype = "AUV";
	vname = "(3)Unicorn";
      }
      if(sourceID == "4") {
	vtype = "AUV";
	vname = "(4)Macrura";
      }
      if(sourceID == "5") {
	vtype = "KELP";
	vname = "(5)PN2";
      }
      if(sourceID == "7") {
	vtype = "GLIDER";
	vname = "(7)X-RAY";
      }
      if(sourceID == "9") { 
	vtype = "KAYAK";
	vname = "(9)DEE";
      }
      if(sourceID == "10") {
	vtype = "KAYAK";
	vname = "(10)Elanor";
      }
      if(sourceID == "11") {
	vtype = "KAYAK";
	vname = "(11)Frankie";
      }
      if(sourceID == "14") {
	vtype = "GLIDER";
	vname = "(14)SLOCUM-GTAS";
      }
      if(sourceID == "15") {
	vtype = "KELP";
	vname = "(15)KELP-OBCI";
      }
      if(sourceID == "18") {
	vtype = "VSA";
	vname = "(18)VSA-1";
      }
      if(sourceID == "19") {
	vtype = "VSA";
	vname = "(19)VSA-2";
      }
      if(sourceID == "20") {
	vtype = "GLIDER";
	vname = "(20)SeaGlider-106";
      }
      if(sourceID == "21") {
	vtype = "GLIDER";
	vname = "(21)SeaGlider-107";
      }
      if(sourceID == "22") {
	vtype = "GLIDER";
	vname = "(22)SeaGlider-116";
      }
      if(sourceID == "24") {
	vtype = "GLIDER";
	vname = "(24)SeaGlider-118";
      }

      // publish it at AIS_REPORT
      string summary = "NAME=" + vname;
      summary += ",TYPE=";
      summary += vtype;
      summary += ",TIME=" + dstringCompact(doubleToString(navTime));
      summary += ",X="   + dstringCompact(doubleToString(navX));
      summary += ",Y="   + dstringCompact(doubleToString(navY));
      summary += ",SPD=" + dstringCompact(doubleToString(navSpeed));
      summary += ",HDG=" + dstringCompact(doubleToString(navHeading));
      summary += ",DEPTH=" + dstringCompact(doubleToString(navDepth));
      m_Comms.Notify("AIS_REPORT", summary);
      m_Comms.Notify("TRANSPONDER_NAFCON_REPORT", summary);

      MOOSTrace("Transponder NaFCon Report:\n");
      MOOSTrace(summary);
      MOOSTrace("\n");
     } 
  }
  return true;
}


//-----------------------------------------------------------------
// Procedure: updateContactList
//   Purpose: Maintain a list of known unique contact names, and the
//            times they were last written to.

void TransponderAIS::updateContactList(string contact_name)
{
  contact_name = toupper(stripBlankEnds(contact_name));

  double moos_time = MOOSTime();

  int i;
  
  for(i=0; i<m_contact_list.size(); i++) {
    if(m_contact_list[i] == contact_name) {
      m_contact_time[i] = moos_time;
      return;
    }
  }
  
  m_contact_list.push_back(contact_name);
  m_contact_time.push_back(moos_time);
}



//-----------------------------------------------------------------
// Procedure: postContactList
//   Purpose: 

void TransponderAIS::postContactList()
{
  double moos_time = MOOSTime();

  int vsize = m_contact_list.size();

 
  string contact_list_string;
  
  for(int i=0; i<vsize; i++) {
    double time_since_update = moos_time - m_contact_time[i];
    if(i!=0)
      contact_list_string += ",";
    contact_list_string += m_contact_list[i];
    contact_list_string += "(";
    contact_list_string += doubleToString(time_since_update, 2);
    contact_list_string += ")";    
  }

  m_Comms.Notify("CONTACT_LIST", contact_list_string);
}
