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

#include <iostream>
#include <list>
#include <iterator>
#include "TransponderAIS.h"
#include "MBUtils.h"

#define MAX_NAFCON_ID 64

using namespace std;

//-----------------------------------------------------------------
// Procedure: Constructor

TransponderAIS::TransponderAIS()
{
  m_nav_x        = 0;
  m_nav_y        = 0;
  m_nav_speed    = 0;
  m_nav_heading  = 0;
  m_nav_yaw      = 0;
  m_nav_depth    = 0;
  m_vessel_name  = "UNKNOWN_VESSEL_NAME";
  m_vessel_type  = "UNKNOWN_VESSEL_TYPE";
  m_vessel_len   = 0; // Zero indicates unspecified length
  m_parseNaFCon  = false;
  m_helm_mode    = "none";
  m_helm_engaged = false;
  m_helm_lastmsg = -1;

  m_blackout_interval = 0;
  m_blackout_baseval  = 0;
  m_blackout_variance = 0;
  m_last_post_time    = -1;
  m_db_uptime         = 0;

  m_contact_report_var = "AIS_REPORT";
}

//-----------------------------------------------------------------
// Procedure: OnNewMail

bool TransponderAIS::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.m_sKey;
    string community = msg.m_sOriginatingCommunity;
    string sdata = msg.m_sVal;
    double ddata = msg.m_dfVal;
    double mtime = msg.m_dfTime;
    //char   mtype = msg.m_cDataType;

    if(key == "DB_UPTIME")
      m_db_uptime = ddata;
    else if(key == "NAV_X")
      m_nav_x = ddata;
    else if(key == "NAV_Y")
      m_nav_y = ddata;
    else if(key == "NAV_SPEED")
      m_nav_speed = ddata;
    else if(key == "NAV_HEADING")
      m_nav_heading = ddata;
    else if(key == "NAV_YAW")
      m_nav_yaw = (ddata*-180.0)/3.1415926;
    else if(key == "NAV_DEPTH")
      m_nav_depth = ddata;
    else if(key == "IVPHELM_SUMMARY") {
      m_helm_lastmsg = m_db_uptime;
      handleLocalHelmSummary(sdata, mtime);
    }
    else if(key == "IVPHELM_ENGAGED") {
      m_helm_lastmsg = m_db_uptime;
      string value = tolower(stripBlankEnds(sdata));
      m_helm_engaged = (value == "engaged");
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

    else
      MOOSTrace("TransponderAIS: Unknown msg [%s]\n",msg.m_sKey.c_str());
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
  m_Comms.Register("DB_UPTIME", 0);
  m_Comms.Register("IVPHELM_SUMMARY", 0);
  m_Comms.Register("IVPHELM_ENGAGED", 0);

  
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


    double lat, lon;
    m_geodesy.LocalGrid2LatLong(m_nav_x, m_nav_y, lat, lon);

    string moosdb_time = dstringCompact(doubleToString(m_db_uptime,3));
    string utc_time = dstringCompact(doubleToString(moos_time,3));

    while(m_nav_heading >=360)
      m_nav_heading -= 360;
    while(m_nav_heading < 0)
      m_nav_heading += 360;

    //all strings: assembleAIS(name,type,db_time,utc_time,x,y,lat,lon,spd,hdg,depth)
    string summary = assembleAIS(m_vessel_name, m_vessel_type, moosdb_time, utc_time, 
				 dstringCompact(doubleToString(m_nav_x, 2)), 
				 dstringCompact(doubleToString(m_nav_y, 2)),
				 dstringCompact(doubleToString(lat, 6)), 
				 dstringCompact(doubleToString(lon, 6)),
				 dstringCompact(doubleToString(m_nav_speed, 2)),
				 dstringCompact(doubleToString(m_nav_heading, 2)),
				 dstringCompact(doubleToString(m_nav_depth, 2)),
				 dstringCompact(doubleToString(m_vessel_len, 2)),
				 dstringCompact(doubleToString(m_nav_yaw,5)),
				 m_helm_mode);

    string local_var = m_contact_report_var + "_LOCAL";
    
    m_Comms.Notify(local_var, summary);
    m_last_post_time = moos_time;

    m_blackout_interval  = m_blackout_baseval;

    if(m_blackout_variance > 0) {
      int    range = (int)(m_blackout_variance * 100.0);
      int    val   = rand() % range;
      double delta = (double)((double)val / 100.0);
      m_blackout_interval -= m_blackout_variance;
      m_blackout_interval += (delta * 2.0);
    }

    if(m_blackout_interval < 0)
      m_blackout_interval = 0;
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
  
  // initialize m_geodesy
  if (!m_geodesy.Initialise(latOrigin, longOrigin))
    return MOOSFail("Geodesy init failed.\n");

  // initialize naFConPublishForID to all false before adding
  //  config entries. we will add true values for NaFCoN
  //  ids we wish to publish for
  naFConPublishForID.assign(MAX_NAFCON_ID, false);
  bool publishingSpecified = false;

  list<string> sParams;
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    
    list<string>::iterator p;
    for(p = sParams.begin();p!=sParams.end();p++) {

      string sLine    = stripBlankEnds(*p);
      string sVarName = MOOSChomp(sLine, "=");

      if(MOOSStrCmp(sVarName, "VESSEL_TYPE"))
	m_vessel_type = sLine;
      else if(MOOSStrCmp(sVarName, "VESSEL_LENGTH"))
	m_vessel_len = atof(sLine.c_str());
      else if(MOOSStrCmp(sVarName, "BLACKOUT_INTERVAL")) {
	double dval = atof(sLine.c_str());
	if(dval >= 0)
	  m_blackout_baseval  = dval;
	  m_blackout_interval = dval;
      }
      else if(MOOSStrCmp(sVarName, "BLACKOUT_VARIANCE")) {
	double dval = atof(sLine.c_str());
	if(dval >= 0)
	  m_blackout_variance = dval;
      }
      else if(MOOSStrCmp(sVarName, "PARSE_NAFCON")) 
	m_parseNaFCon = MOOSStrCmp(sLine, "true"); 
      else if(MOOSStrCmp(sVarName, "CONTACT_REPORT_VARIABLE"))
	m_contact_report_var = sLine;
            
      // for each publish_for_nafcon_id config value
      // insert a true in the appropriate entry of the 
      // naFConPublishForID vector.
      else if(MOOSStrCmp(sVarName, "PUBLISH_FOR_NAFCON_ID")) {
	int id = atoi(sLine.c_str());
	naFConPublishForID[id] = true;
	// since the user specified id(s), do not publish
	// AIS_REPORT for all ids
	publishingSpecified = true;
      }
    }
    // if no IDs to publish are specified, publish for ALL ids
    // (true for all entries of naFConPublishForID)
    if (!publishingSpecified)
      naFConPublishForID.assign(MAX_NAFCON_ID,true);
  }
  // end tes 9-12-07

  
  // find the lookup table file and parse it
  if(m_parseNaFCon) {
    string path;
    if(!m_MissionReader.GetConfigurationParam("MODEM_ID_LOOKUP_PATH", path)) {
      MOOSTrace("nafcon message parsing enabled but MODEM_ID_LOOKUP_PATH specified!\n");
      exit(1);
    }
    ifstream fin;
    fin.open(path.c_str(), ifstream::in);
    if(fin.fail()) {
      MOOSTrace("cannot open %s for reading!\n", path.c_str());
      exit(1);   
    }

    MOOSTrace("reading in modem id lookup table:\n");
      
    while(fin) {
      char line[5000];
      fin.getline(line, 5000);
      
      string sline = line;
      
      // strip the spaces and comments out
      int pos = sline.find(' ');
      while(pos != string::npos) {
	sline.erase(pos, 1);
	pos = sline.find(' ');
      }
      pos = sline.find('#');
      while(pos != string::npos) {
	sline.erase(pos);
	pos = sline.find('#');
      }
      pos = sline.find("//");
      while(pos != string::npos) {
	sline.erase(pos);
	pos = sline.find("//");
      }
      
      // ignore blank lines
      if(sline != "") {
	vector<string> line_parsed = parseString(sline, ",");
	if(line_parsed.size() < 3)
	  MOOSTrace("invalid line: %s\n", line);
	else {
	  cout << "modem id [" << line_parsed[0] << "], name [" << line_parsed[1] << "], type [" << line_parsed[2] << "]" << endl;
	  
	  vehicle_nametype new_vehicle =
	    {line_parsed[1], line_parsed[2]};
	  
	  //add the entry to our lookup map
	  nafcon_lookup[atoi(line_parsed[0].c_str())] = new_vehicle;
	}
      }
    }
    fin.close();
  }

  MOOSTrace("Using %s for contact report variable and %s_LOCAL for local contact report variable\n", m_contact_report_var.c_str(), m_contact_report_var.c_str());
  return(true);
}


//-----------------------------------------------------------------
// Procedure: handleLocalHelmSummary()

void TransponderAIS::handleLocalHelmSummary(const string& sdata, 
					    double stime)
{
  vector<string> svector = parseString(sdata, ',');
  unsigned int i, vsize = svector.size();
  string helm_summary;
  for(i=0; i<vsize; i++) {
    svector[i] = stripBlankEnds(svector[i]);
    string left = biteString(svector[i], '=');
    string right = stripBlankEnds(svector[i]);
    if(left == "modes")
      m_helm_mode = right;
  }
}


//-----------------------------------------------------------------
// Procedure: handleIncomingNaFConMessage()
//
//  tes 9-12-07
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
  
  MOOSTrace("Began parsing NaFCon message to %s:\n", m_contact_report_var.c_str());
  MOOSTrace(rMsg);
  MOOSTrace("\n\n");
  
  //we have a status message                                                                           
  if(MOOSStrCmp(messageType, "SENSOR_STATUS") || MOOSStrCmp(messageType, "SENSOR_CONTACT") || MOOSStrCmp(messageType, "SENSOR_TRACK") ) {
    int sourceID;
    MOOSValFromString(sourceID, rMsg, "SourcePlatformId");
    
    // limit to vehicles specified in config file
    // and now stored in naFConPublishForID
    if(naFConPublishForID[sourceID])  {
      MOOSTrace("Will publish for this ID. \n");
      double navX, navY, navLat, navLong, navHeading, navSpeed, navDepth, navTime;            
      string vname = nafcon_lookup[sourceID].name;
      string vtype = nafcon_lookup[sourceID].type;
      
      if (vname == "")
	vname = intToString(sourceID);
      
      
      if(MOOSStrCmp(messageType, "SENSOR_STATUS")) {
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
      }
      
      else if(MOOSStrCmp(messageType, "SENSOR_CONTACT")) {
	if(!MOOSValFromString(navLong, rMsg, "SensorLongitude"))
	  return MOOSFail("No SensorLongitude\n");
	
	if (!MOOSValFromString(navLat, rMsg, "SensorLatitude"))
	  return MOOSFail("No SensorLatitude\n");
	
	if (!MOOSValFromString(navHeading, rMsg, "SensorHeading"))
	  return MOOSFail("No SensorHeading\n");
	
	if(!MOOSValFromString(navDepth, rMsg, "SensorDepth"))
	  return MOOSFail("No SensorDepth\n");      
	
	if(!MOOSValFromString(navTime, rMsg, "Timestamp"))
	  return MOOSFail("No ContactTimestamp\n");      
	
	// Use previous speed for CONTACT_REPORT
	double node_utc,node_x,node_y,node_spd,node_hdg,node_dep;      
	
	if (prevContactInfo(vname,&node_utc,&node_x,&node_y,&node_spd,&node_hdg,&node_dep))
	  navSpeed = node_spd;
	
	MOOSTrace("Status Info: Time = %f, Speed = %f, Heading = %f \n",node_utc,node_spd,node_hdg);
      }
      else if(MOOSStrCmp(messageType, "SENSOR_TRACK")) {
	
	if(!MOOSValFromString(navLong, rMsg, "TrackLongitude"))
	  return MOOSFail("No TrackLongitude\n");
	
	if (!MOOSValFromString(navLat, rMsg, "TrackLatitude"))
	  return MOOSFail("No TrackLatitude\n");
	
	if (!MOOSValFromString(navHeading, rMsg, "TrackHeading"))
	  return MOOSFail("No TrackHeading\n");
	
	if(!MOOSValFromString(navDepth, rMsg, "TrackDepth"))
	  return MOOSFail("No TrackDepth\n");      
	
	if (!MOOSValFromString(navSpeed, rMsg, "TrackSpeed"))
	  return MOOSFail("No TrackSpeed\n");
	
	if(!MOOSValFromString(navTime, rMsg, "Timestamp"))
	  return MOOSFail("No TrackTimestamp\n");      
	
	string trackID;
	MOOSValFromString(trackID, rMsg, "TrackNumber");
	int track_id = atoi(trackID.c_str());
	MOOSTrace("Track report. trackID = %d\n",track_id);
	
	// set track type to ship
	vtype = "TRACK";
	vname = "TRK_"+trackID+"_"+vname;
      }
      
      // convert lat, long into x, y. 60 nautical miles per minute
      //            if(!m_geodesy.LatLong2LocalGrid(navLat, navLong, navY, navX))
      //Changed to UTM for consistency
      if(!m_geodesy.LatLong2LocalUTM(navLat, navLong, navY, navX))
	return MOOSFail("Geodesy conversion failed\n");
      
      
      
      // publish it at AIS_REPORT
      // all strings: assembleAIS(name,type,db_time,utc_time,x,y,lat,lon,spd,hdg,depth)
      string summary = assembleAIS(vname, vtype, "-1",
				   dstringCompact(doubleToString(navTime)), 
				   dstringCompact(doubleToString(navX, 2)),
				   dstringCompact(doubleToString(navY, 2)), 
				   dstringCompact(doubleToString(navLat, 6)), 
				   dstringCompact(doubleToString(navLong, 6)), 
				   dstringCompact(doubleToString(navSpeed, 2)),
				   dstringCompact(doubleToString(navHeading, 2)),
				   dstringCompact(doubleToString(navDepth, 2)), 
				   "0", "0", "unknown-mode");
      
      m_Comms.Notify(m_contact_report_var, summary);
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
//   HS 032108 Modified to full contact nav info and UTC of report to allow inheritance
//             and extrapolation of unreported varibles in NaFCon messages 

void TransponderAIS::updateContactList(string contact_name,
                                       double cn_utc,
                                       double cn_x,
                                       double cn_y,
                                       double cn_spd,
                                       double cn_hdg,
                                       double cn_dep)
{
  contact_name = toupper(stripBlankEnds(contact_name));
  
  double moos_time = MOOSTime();
  unsigned int i;
  
  for(i=0; i<m_contact_list.size(); i++) {
    if(m_contact_list[i] == contact_name) {
      m_contact_time[i] = moos_time;
      m_contact_utc[i] = cn_utc;
      m_contact_x[i] = cn_x;
      m_contact_y[i] = cn_y;
      m_contact_spd[i] = cn_spd;
      m_contact_hdg[i] = cn_hdg;
      m_contact_dep[i] = cn_dep;
      MOOSTrace("Existing Contact %d, Time = %f, Speed = %f, Heading = %f \n",
		i+1,cn_utc,cn_spd,cn_hdg);
      return;
    }
  }
  
  m_contact_list.push_back(contact_name);
  m_contact_time.push_back(moos_time);
  m_contact_utc.push_back(cn_utc);
  m_contact_x.push_back(cn_x);
  m_contact_y.push_back(cn_y);
  m_contact_spd.push_back(cn_spd);
  m_contact_hdg.push_back(cn_hdg);
  m_contact_dep.push_back(cn_dep);
  
  MOOSTrace("New Contact %d, Time = %f, Speed = %f, Heading = %f \n",
	    m_contact_list.size(),cn_utc,cn_spd,cn_hdg);
}

//-----------------------------------------------------------------
// Procedure: prevContactInfo
//   Purpose: 

bool TransponderAIS::prevContactInfo(string contact_name,
                                     double* cn_utc,
                                     double* cn_x,
                                     double* cn_y,
                                     double* cn_spd,
                                     double* cn_hdg,
                                     double* cn_dep)
  
{
  contact_name = toupper(stripBlankEnds(contact_name));
  
  //  i=m_contact_list.size();
  // MOOSTrace("Last Contact %d, Time = %f, Speed = %f, 
  //    Heading = %f \n",i,m_contact_utc[i-1],m_contact_spd[i-1],
  //    m_contact_hdg[i-1]);
  
  for(unsigned int i=0; i<m_contact_list.size(); i++) {
    if(m_contact_list[i] == contact_name) {
      *cn_utc = m_contact_utc[i];
      *cn_x    = m_contact_x[i];
      *cn_y   = m_contact_y[i];
      *cn_spd  = m_contact_spd[i];
      *cn_hdg  = m_contact_hdg[i];
      *cn_dep  = m_contact_dep[i];
      //  MOOSTrace("Old Contact %d, Time = %f, Speed = %f, Heading = %f \n",i+1,cn_utc,cn_spd,cn_hdg);
      return(true);
    }
  }
  return(false);
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

//------------------------------------------------------------------
// Procedure: assembleAIS
// Purpose: builds the string used for AIS_REPORT
// tes 11.19.07

string TransponderAIS::assembleAIS(string vname, string vtype, 
				   string db_time, string utc_time, 
				   string x, string y, string lat, 
				   string lon, string spd, string hdg, 
				   string depth, string vlen, 
				   string navyaw, string mode)
{
  // If the length is unknown, put in some good guesses
  if(atof(vlen.c_str()) == 0) {
    if(tolower(vtype) == "kayak")
      vlen = "4.0"; // meters;
    if(tolower(vtype) == "auv")
      vlen = "4.0"; // meters;
    if(tolower(vtype) == "ship")
      vlen = "18.0"; // meters
    if(tolower(vtype) == "glider")
      vlen = "3.0"; // meters
  }

  string summary = "NAME=" + vname;
  summary += ",TYPE=" + vtype;
  summary += ",MOOSDB_TIME=" + db_time;
  summary += ",UTC_TIME=" + utc_time;
  summary += ",X="   + x;
  summary += ",Y="   + y;
  summary += ",LAT=" + lat;
  summary += ",LON=" + lon;
  summary += ",SPD=" + spd;
  summary += ",HDG=" + hdg;
  summary += ",YAW=" + navyaw;
  summary += ",DEPTH=" + depth;
  summary += ",LENGTH=" + vlen;

  // We choose a duration of 3 seconds because the helm is 
  // configured to provide a heartbeat once per second.
  double timeout_duration = 3.0; // seconds
  string  mode_str = ",MODE=";
  if((m_db_uptime-m_helm_lastmsg) > timeout_duration) {
    string awol_time = doubleToString((m_db_uptime-m_helm_lastmsg), 0);
    mode_str += "NOHELM-" + awol_time;
  }
  else if(m_helm_engaged == false)
    mode_str += "DISENGAGED";
  else {
    if(mode != "none")
      mode_str += mode;
    else
      mode_str += "ENGAGED";
  }
  summary += mode_str;

  return(summary);
}
