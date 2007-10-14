/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TransponderNMEA.cpp                                  */
/*    DATE: October 8th 2007                                     */
/*****************************************************************/

#include <iostream>
#include <list>
#include <iterator>
#include "TransponderNMEA.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: Constructor

TransponderNMEA::TransponderNMEA()
{
  m_nav_x       = 0;
  m_nav_y       = 0;
  m_nav_speed   = 0;
  m_nav_heading = 0;
  m_nav_depth   = 0;
  m_vessel_name = "UNKNOWN_NAME";
  m_vessel_type = "UNKNOWN_TYPE";

  m_nav_x_set   = false;
  m_nav_y_set   = false;
  m_nav_h_set   = false;
}

//-----------------------------------------------------------------
// Procedure: OnNewMail

bool TransponderNMEA::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::reverse_iterator p;
  
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.m_sKey;
    string community = msg.m_sOriginatingCommunity;
    string sdata = msg.m_sVal;
    double ddata = msg.m_dfVal;
    char   mtype = msg.m_cDataType;
    
    if(key == "OUR_SHIP_NMEA_1") {
      bool ok = handleIncomingNMEA(sdata);
    }
    else if(key == "OUR_SHIP_NMEA_2") {
      bool ok = handleIncomingNMEA(sdata);
    }
    else {
      MOOSTrace("TransponderNMEA: Unknown msg [%s]\n",msg.m_sKey.c_str());
    }
  }
  return(true);
}

//-----------------------------------------------------------------
// Procedure: OnConnectoToServer()

bool TransponderNMEA::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));
 
  m_Comms.Register("OUR_SHIP_NMEA_1", 0);
  m_Comms.Register("OUR_SHIP_NMEA_2", 0);

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

bool TransponderNMEA::Iterate()
{
  if((!m_nav_x_set || !m_nav_y_set || !m_nav_h_set))
    return(false);

  
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
  
  return(true);
}


//-----------------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool TransponderNMEA::OnStartUp()
{
  CMOOSApp::OnStartUp();

  MOOSTrace("pTransponderNMEA starting....\n");

  m_start_time = MOOSTime();

  double latOrigin, longOrigin;
  if (!m_MissionReader.GetValue("LatOrigin", latOrigin))
    return MOOSFail("LatOrigin not set in *.moos file.\n");
  if (!m_MissionReader.GetValue("LongOrigin", longOrigin))
    return MOOSFail("LongOrigin not set in *.moos file\n");
  
  // initialize m_Geodesy
  if (!m_Geodesy.Initialise(latOrigin, longOrigin))
    return MOOSFail("Geodesy init failed.\n");

  list<string> sParams;
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    
    list<string>::iterator p;
    for(p = sParams.begin();p!=sParams.end();p++) {

      string sLine    = stripBlankEnds(*p);
      string sVarName = MOOSChomp(sLine, "=");

      if(MOOSStrCmp(sVarName, "VESSEL_TYPE"))
	m_vessel_type = sLine;
      
      if(MOOSStrCmp(sVarName, "VESSEL_NAME"))
	m_vessel_name = sLine;
      
      }
  }
  return(true);
}


//-----------------------------------------------------------------
// Procedure: handleIncomingNMEA()

bool TransponderNMEA::handleIncomingNMEA(const string& rMsg)
{    
  string message = stripBlankEnds(rMsg);
  vector<string> svector = parseString(message, ',');
  int vsize = svector.size();

  if(svector[0] == "$GPGGA") {
    bool   latpos = (svector[3] == "N");
    bool   lonpos = (svector[5] == "E");
    string latstr = svector[2];
    string lonstr = svector[4];
    
    int lat_decimal_position = 0;
    for(int i=0; i<latstr.size(); i++)
      if(latstr.at(i) == '.')
	lat_decimal_position = i;
    
    int lon_decimal_position = 0;
    for(int i=0; i<lonstr.size(); i++)
      if(lonstr.at(i) == '.')
	lon_decimal_position = i;
    
    char buffer[100];
    
    strcpy(buffer, latstr.c_str());
    double latmins = atof(buffer + lat_decimal_position -2);
    buffer[lat_decimal_position-2] = '\0';
    double latdegs = atof(buffer);
    if(!latpos)
      latdegs *= -1;
    
    strcpy(buffer, lonstr.c_str());
    double lonmins = atof(buffer + lon_decimal_position -2);
    buffer[lon_decimal_position-2] = '\0';
    double londegs = atof(buffer);
    
    double navLat = latdegs + (latmins/60);
    double navLon = londegs + (lonmins/60);
    if(!latpos)
      navLat *= -1;
    if(!lonpos)
      navLon *= -1;
    
    
    double navX, navY;
    // convert lat, long into x, y. 60 nautical miles per minute
    if(!m_Geodesy.LatLong2LocalGrid(navLat, navLon, navY, navX))
      return MOOSFail("Geodesy conversion failed\n");
    
    m_nav_x = navX;
    m_nav_y = navY;
    m_nav_x_set = true;
    m_nav_y_set = true;
  }
  if(svector[0] == "$HEHDT") {
    m_nav_heading = atof(svector[1].c_str());
    m_nav_h_set = true;
  }


  return(true);
}


