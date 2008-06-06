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
  m_db_uptime         = 0;
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
  m_Comms.Register("AIS_REPORT", 0);
  m_Comms.Register("DB_UPTIME", 0);

  
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


    //all strings: assembleAIS(name,type,db_time,utc_time,x,y,lat,lon,spd,hdg,depth)
    string summary = assembleAIS(m_vessel_name, m_vessel_type, moosdb_time, utc_time,\
				 dstringCompact(doubleToString(m_nav_x, 2)),\
				 dstringCompact(doubleToString(m_nav_y, 2)),\
				 dstringCompact(doubleToString(lat, 6)),\
				 dstringCompact(doubleToString(lon, 6)),\
				 dstringCompact(doubleToString(m_nav_speed, 2)),\
				 dstringCompact(doubleToString(m_nav_heading, 2)),\
				 dstringCompact(doubleToString(m_nav_depth, 2)));
    
    m_Comms.Notify("AIS_REPORT_LOCAL", summary);
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
  double nav_utc_val;   bool nav_utc_set = false;
  double nav_lat_val; 
  double nav_long_val;
  
  
  vector<string> svector = parseString(sdata, ',');
  int vsize = svector.size();
  for(int i=0; i<vsize; i++) {
    vector<string> svector2 = parseString(svector[i], '=');
    if(svector2.size() == 2) {
      string left = stripBlankEnds(svector2[0]);
      string right = stripBlankEnds(svector2[1]);
      bool right_isnum = isNumber(right);

      if(left=="NAME") {
	vname = right;
	vname_set = true;
      }
      else if((left == "UTC_TIME") && right_isnum) {
	nav_utc_val = atof(right.c_str());
	nav_utc_set = true;
      }
      else if((left == "X") && right_isnum) {
	nav_x_val = atof(right.c_str());
	nav_x_set = true;
      }
      else if((left == "Y") && right_isnum) {
	nav_y_val = atof(right.c_str());
	nav_y_set = true;
      }
      else if((left == "LAT") && right_isnum) {
	nav_lat_val = atof(right.c_str());
      }
      else if((left == "LON") && right_isnum) {
	nav_long_val = atof(right.c_str());
      }
      else if((left == "SPD") && right_isnum) {
	nav_spd_val = atof(right.c_str());
	nav_spd_set = true;
      }
      else if((left == "HDG") && right_isnum) {
	nav_hdg_val = atof(right.c_str());
	if (nav_hdg_val < 0.0)
	  nav_hdg_val += 360.0;
	if (nav_hdg_val >= 360.0)
	  nav_hdg_val -= 360.0;
	nav_hdg_set = true;
      }
      else if((left == "DEPTH") && right_isnum) {
	nav_dep_val = atof(right.c_str());
	nav_dep_set = true;
      }
    }
  }

  if(!vname_set || !nav_x_set || !nav_y_set || !nav_utc_set ||
     !nav_spd_set || !nav_hdg_set || !nav_dep_set)
    return(false);


  vname = toupper(vname);
  
  updateContactList(vname,
		    nav_utc_val,
		    nav_x_val,
		    nav_y_val,
		    nav_spd_val,
		    nav_hdg_val,
		    nav_dep_val);

  m_Comms.Notify(vname+"_NAV_UTC", nav_utc_val);
  m_Comms.Notify(vname+"_NAV_X", nav_x_val);
  m_Comms.Notify(vname+"_NAV_Y", nav_y_val);
  m_Comms.Notify(vname+"_NAV_SPEED", nav_spd_val);
  m_Comms.Notify(vname+"_NAV_HEADING", nav_hdg_val);
  m_Comms.Notify(vname+"_NAV_DEPTH", nav_dep_val);
  m_Comms.Notify(vname+"_NAV_LAT", nav_lat_val);
  m_Comms.Notify(vname+"_NAV_LONG", nav_long_val);
  
  return(true);
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

  MOOSTrace("Began parsing NaFCon message to AIS_REPORT:\n");
  MOOSTrace(rMsg);
  MOOSTrace("\n\n");
  
  //we have a status message                                                                           
  if(MOOSStrCmp(messageType, "SENSOR_STATUS") || MOOSStrCmp(messageType, "SENSOR_CONTACT")) {
    string sourceID;
    MOOSValFromString(sourceID, rMsg, "SourcePlatformId");
    
    // limit to vehicles specified in config file
    // and now stored in naFConPublishForID
    if(naFConPublishForID[atoi(sourceID.c_str())]) {
      MOOSTrace("Will publish for this ID. \n");
      double navX, navY, navLat, navLong, navHeading, navSpeed, navDepth, navTime;

      string vtype = "GLIDER";
      string vname = sourceID;
      if(sourceID == "1" ) {vtype = "AUV";    vname = "Sea-Horse";}
      if(sourceID == "2")  {vtype = "KAYAK";  vname = "Bobby";}
      if(sourceID == "3")  {vtype = "AUV";    vname = "Unicorn";}
      if(sourceID == "4")  {vtype = "AUV";    vname = "Macrura";}
      if(sourceID == "5")  {vtype = "KELP";   vname = "PN2";}
      if(sourceID == "7")  {vtype = "GLIDER"; vname = "X-RAY";}
      if(sourceID == "9")  {vtype = "KAYAK";  vname = "DEE";}
      if(sourceID == "10") {vtype = "AUV";    vname = "OEX";}
      if(sourceID == "11") {vtype = "KAYAK";  vname = "Frankie";}
      if(sourceID == "14") {vtype = "GLIDER"; vname = "SLOCUM-GTAS";}
      if(sourceID == "15") {vtype = "KELP";   vname = "KELP-OBCI";}
      if(sourceID == "18") {vtype = "VSA";    vname = "VSA-1";}
      if(sourceID == "19") {vtype = "VSA";    vname = "VSA-2";}
      if(sourceID == "20") {vtype = "GLIDER"; vname = "SeaGlider-106";}
      if(sourceID == "21") {vtype = "GLIDER"; vname = "SeaGlider-107";}
      if(sourceID == "22") {vtype = "GLIDER"; vname = "SeaGlider-116";}
      if(sourceID == "24") {vtype = "GLIDER"; vname = "SeaGlider-118";}
      if(sourceID == "25") {vtype = "AUV";    vname = "NUWC-Iver2-Mako";}
      if(sourceID == "26") {vtype = "AUV";    vname = "NUWC-Iver2-Whitetip";}
      if(sourceID == "27") {vtype = "AUV";    vname = "NUWC-Iver2-Hammerhead";}
      if(sourceID == "29") {vtype = "GLIDER";    vname = "Fologa";}

      if(MOOSStrCmp(messageType, "SENSOR_STATUS"))
      {
          
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

      if(MOOSStrCmp(messageType, "SENSOR_CONTACT"))
      {
          
          if(!MOOSValFromString(navLong, rMsg, "SensorLongitude"))
              return MOOSFail("No SensorLongitude\n");
          
          if (!MOOSValFromString(navLat, rMsg, "SensorLatitude"))
              return MOOSFail("No SensorLatitude\n");
          
          if (!MOOSValFromString(navHeading, rMsg, "SensorHeading"))
              return MOOSFail("No SensorHeading\n");
          
          if(!MOOSValFromString(navDepth, rMsg, "SensorDepth"))
              return MOOSFail("No SensorDepth\n");      

          if(!MOOSValFromString(navTime, rMsg, "ContactTimestamp"))
              return MOOSFail("No ContactTimestamp\n");      

	  // Use previous speed for CONTACT_REPORT
	  double node_utc,node_x,node_y,node_spd,node_hdg,node_dep;      

	  if (prevContactInfo(vname,&node_utc,&node_x,&node_y,&node_spd,&node_hdg,&node_dep))
	    navSpeed = node_spd;

	  MOOSTrace("Status Info: Time = %f, Speed = %f, Heading = %f \n",node_utc,node_spd,node_hdg);
      }
      
      // convert lat, long into x, y. 60 nautical miles per minute
      if(!m_geodesy.LatLong2LocalGrid(navLat, navLong, navY, navX))
	return MOOSFail("Geodesy conversion failed\n");
      

      //string vtype = "GLIDER";
      //string vname = sourceID;
      if(sourceID == "1") {
	vtype = "AUV";
	vname = "Sea-Horse";
      }
      if(sourceID == "2") {
	vtype = "KAYAK";
	vname = "Bobby";
      }
      if(sourceID == "3") {
	vtype = "AUV";
	vname = "Unicorn";
      }
      if(sourceID == "4") {
	vtype = "AUV";
	vname = "Macrura";
      }
      if(sourceID == "5") {
	vtype = "KELP";
	vname = "PN2";
      }
      if(sourceID == "7") {
	vtype = "GLIDER";
	vname = "X-RAY";
      }
      if(sourceID == "9") { 
	vtype = "KAYAK";
	vname = "DEE";
      }
      if(sourceID == "10") {
	vtype = "AUV";
	vname = "OEX";
      }
      if(sourceID == "11") {
	vtype = "KAYAK";
	vname = "Frankie";
      }
      if(sourceID == "14") {
	vtype = "GLIDER";
	vname = "SLOCUM-GTAS";
      }
      if(sourceID == "15") {
	vtype = "KELP";
	vname = "KELP-OBCI";
      }
      if(sourceID == "18") {
	vtype = "VSA";
	vname = "VSA-1";
      }
      if(sourceID == "19") {
	vtype = "VSA";
	vname = "VSA-2";
      }
      if(sourceID == "20") {
	vtype = "GLIDER";
	vname = "SeaGlider-106";
      }
      if(sourceID == "21") {
	vtype = "GLIDER";
	vname = "SeaGlider-107";
      }
      if(sourceID == "22") {
	vtype = "GLIDER";
	vname = "SeaGlider-116";
      }
      if(sourceID == "24") {
	vtype = "GLIDER";
	vname = "SeaGlider-118";
      }
      if(sourceID == "26") {
	vtype = "AUV";
	vname = "WHITETIP";
      }//NUWC Iver
      if(sourceID == "27") {
	vtype = "AUV";
	vname = "HAMMERHEAD";
      }//NUWC Iver
      if(sourceID == "28") {
	vtype = "AUV";
	vname = "MAKO";
      }//NUWC Iver

      // temporary hack until we know OEXs real ID
      if(sourceID == "30") {vtype = "AUV";    vname = "OEX";}

      // publish it at AIS_REPORT
      // all strings: assembleAIS(name,type,db_time,utc_time,x,y,lat,lon,spd,hdg,depth)
      string summary = assembleAIS(vname, vtype, "-1",\
				   dstringCompact(doubleToString(navTime)),\
				   dstringCompact(doubleToString(navX, 2)),\
				   dstringCompact(doubleToString(navY, 2)),\
				   dstringCompact(doubleToString(navLat, 6)),\
				   dstringCompact(doubleToString(navLong, 6)),\
				   dstringCompact(doubleToString(navSpeed, 2)),\
				   dstringCompact(doubleToString(navHeading, 2)),\
				   dstringCompact(doubleToString(navDepth, 2)));
      
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

string TransponderAIS::assembleAIS(string name, string type, string db_time, string utc_time,
				   string x, string y, string lat, string lon, string spd,
				   string hdg, string depth)
{

    string summary = "NAME=" + name;
    summary += ",TYPE=" + type;
    summary += ",MOOSDB_TIME=" + db_time;
    summary += ",UTC_TIME=" + utc_time;
    summary += ",X="   + x;
    summary += ",Y="   + y;
    summary += ",LAT=" + lat;
    summary += ",LON=" + lon;
    summary += ",SPD=" + spd;
    summary += ",HDG=" + hdg;
    summary += ",DEPTH=" + depth;
    
    return summary;

}
