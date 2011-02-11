/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: NodeReporter.cpp                                     */
/*    DATE: Feb 13th 2006 (TransponderAIS)                       */
/*    DATE: Jun  8th 2009 (NodeReporter)                         */
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

#include <list>
#include <iterator>
#include <algorithm>
#include "NodeReporter.h"
#include "MBUtils.h"

#define USE_UTM

using namespace std;

//-----------------------------------------------------------------
// Procedure: Constructor

NodeReporter::NodeReporter()
{
  m_nav_x        = 0;
  m_nav_y        = 0;
  m_nav_speed    = 0;
  m_nav_heading  = 0;
  m_nav_yaw      = 0;
  m_nav_depth    = 0;
  m_vessel_name  = "unknown";
  m_vessel_type  = "unknown";
  m_vessel_len   = "0"; // Zero indicates unspecified length
  m_helm_engaged = false;
  m_helm_lastmsg = -1;
  m_nohelm_thresh = 3;
  m_helm_mode    = "none";
  m_helm_allstop_mode = "unknown";

  m_blackout_interval = 0;
  m_blackout_baseval  = 0;
  m_blackout_variance = 0;
  m_last_post_time    = -1;
  m_db_uptime         = 0;
  m_time_updated      = false;

  m_crossfill_policy = "literal";

  m_navx_recd = 0;
  m_navy_recd = 0;
  m_navlat_recd = 0;
  m_navlon_recd = 0;

  m_node_report_var = "NODE_REPORT_LOCAL";
  m_plat_report_var = "PLATFORM_REPORT_LOCAL";
}

//-----------------------------------------------------------------
// Procedure: OnNewMail

bool NodeReporter::OnNewMail(MOOSMSG_LIST &NewMail)
{
  m_moos_utc_time = MOOSTime();
  m_time_updated = true;

  MOOSMSG_LIST::iterator p;
  
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key   = msg.m_sKey;
    string sdata = msg.m_sVal;
    double ddata = msg.m_dfVal;
    //double mtime = msg.m_dfTime;

    if(key == "DB_UPTIME")
      m_db_uptime = ddata;
    else if(key == "NAV_X") {
      m_nav_x = ddata;
      m_navx_recd = m_moos_utc_time;
    }
    else if(key == "NAV_Y") {
      m_nav_y = ddata;
      m_navy_recd = m_moos_utc_time;
    }
    else if(key == "NAV_LAT") {
      m_nav_lat = ddata;
      m_navlat_recd = m_moos_utc_time;
    }
    else if(key == "NAV_LONG") {
      m_nav_lon = ddata;
      m_navlon_recd = m_moos_utc_time;
    }
    else if(key == "NAV_SPEED")
      m_nav_speed = ddata;
    else if(key == "NAV_HEADING") {
      m_nav_heading = ddata;
      while(m_nav_heading >=360)
	m_nav_heading -= 360;
      while(m_nav_heading < 0)
	m_nav_heading += 360;
    }
    else if(key == "NAV_YAW")
      m_nav_yaw = (ddata*-180.0)/3.1415926;
    else if(key == "NAV_DEPTH")
      m_nav_depth = ddata;
    else if(key == "IVPHELM_SUMMARY") {
      m_helm_lastmsg = m_db_uptime;
      handleLocalHelmSummary(sdata);
    }
    else if(key == "IVPHELM_ALLSTOP") {
      m_helm_lastmsg = m_db_uptime;
      m_helm_allstop_mode = sdata;
    }
    else if(key == "IVPHELM_ENGAGED") {
      m_helm_lastmsg = m_db_uptime;
      string value = tolower(stripBlankEnds(sdata));
      m_helm_engaged = (value == "engaged");
    }

    if(vectorContains(m_plat_vars, key)) {
      string value = sdata;
      if(msg.IsDouble())
	value = dstringCompact(doubleToString(ddata, 8));
      updatePlatformVar(key, value);
    }
  }
  return(true);
}

//-----------------------------------------------------------------
// Procedure: OnConnectoToServer()

bool NodeReporter::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//-----------------------------------------------------------------
// Procedure: registerVariables()

void NodeReporter::registerVariables()
{
  m_Comms.Register("NAV_X", 0);
  m_Comms.Register("NAV_Y", 0);
  m_Comms.Register("NAV_LAT", 0);
  m_Comms.Register("NAV_LONG", 0);
  m_Comms.Register("NAV_SPEED", 0);
  m_Comms.Register("NAV_HEADING", 0);
  m_Comms.Register("NAV_YAW", 0);
  m_Comms.Register("NAV_DEPTH", 0);
  m_Comms.Register("DB_UPTIME", 0);
  m_Comms.Register("IVPHELM_SUMMARY", 0);
  m_Comms.Register("IVPHELM_ENGAGED", 0);
  m_Comms.Register("IVPHELM_ALLSTOP", 0);
}

//-----------------------------------------------------------------
// Procedure: Iterate()
// Note: The "blackout_interval" is a way to have a vehicle simulate 
//       a vehicle that is only making period reports available to 
//       other vehicles. Even if this effect in the real world is due
//       more to transmission loss or latency.
//    

bool NodeReporter::Iterate()
{
  if(!m_time_updated)
    m_moos_utc_time = MOOSTime();
  m_time_updated = false;

  // If (a) this is the first chance to post, or (b) there is no
  // blackout interval being implemented, or (c) the time since last
  // post exceeds the blackout interval, then perform a posting.
  if((m_last_post_time == -1) || (m_blackout_interval <= 0) ||
     ((m_moos_utc_time - m_last_post_time) > m_blackout_interval)) {

    if(m_crossfill_policy != "literal")
      crossFillCoords();
      
    string report = assembleNodeReport();
    m_Comms.Notify(m_node_report_var, report);
    
    // Note the post time and apply it to the blackout calculation
    m_last_post_time = m_moos_utc_time;
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

  string platform_report = assemblePlatformReport();
  if(platform_report != "")
    m_Comms.Notify(m_plat_report_var, platform_report);

  return(true);
}


//-----------------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool NodeReporter::OnStartUp()
{
  CMOOSApp::OnStartUp();
  MOOSTrace("pNodeReporter starting....\n");

  // Ownship_name == Community must be provided
  if(!m_MissionReader.GetValue("Community", m_vessel_name))
    return(MOOSFail("Vehicle (Community) Name not provided\n"));

   // look for latitude, longitude global variables
  double lat_origin, lon_origin;
  bool ok1 = m_MissionReader.GetValue("LatOrigin", lat_origin);
  bool ok2 = m_MissionReader.GetValue("LongOrigin", lon_origin);
  if(!ok1 || !ok2)
    return(MOOSFail("Lat or Lon Origin not set in *.moos file.\n"));

  // initialize m_geodesy
  if(!m_geodesy.Initialise(lat_origin, lon_origin))
    return(MOOSFail("Geodesy init failed.\n"));

  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);
      double dval  = atof(value.c_str());

      if((param == "PLATFORM_TYPE") ||
	 (param == "VESSEL_TYPE"))       // Depricated
	m_vessel_type = value;
      else if((param =="PLATFORM_LENGTH") ||  
	      (param =="VESSEL_LENGTH")) {  // Depricated
	if(isNumber(value) && (dval >= 0))
	  m_vessel_len = value;
      }
      else if(param =="PLATFORM_LENGTH_SRC") {
	if(!strContainsWhite(value))
	  m_vessel_len_src = value;
      }
      else if(param =="PLATFORM_TYPE_SRC") {
	if(!strContainsWhite(value))
	  m_vessel_type_src = value;
      }
      else if(param == "BLACKOUT_INTERVAL") {
	if(isNumber(value) && (dval >= 0)) {
	  m_blackout_baseval  = dval;
	  m_blackout_interval = dval;
	}
      }
      else if(param == "BLACKOUT_VARIANCE") {
	if(isNumber(value) && (dval >= 0))
	  m_blackout_variance = dval;
      }
      else if(param == "PLATFORM_VARIABLE")
	addPlatformVar(value);
      else if((param == "NODE_REPORT_SUMMARY") ||   // Depricated
	      (param == "NODE_REPORT_VARIABLE")) {
	if(!strContainsWhite(value))
	  m_node_report_var = value;
      }      
      else if(param == "PLATFORM_REPORT_SUMMARY") {
	if(!strContainsWhite(value))
	  m_plat_report_var = value;
      }      
      else if(param == "NOHELM_THRESHOLD") {
	if(isNumber(value) && (dval > 0))
	  m_nohelm_thresh = dval;
      }      
      else if(param =="CROSSFILL_POLICY")
	setCrossFillPolicy(value);
    }
  }

  registerVariables();
  unsigned int k, ksize = m_plat_vars.size();
  for(k=0; k<ksize; k++)
    m_Comms.Register(m_plat_vars[k], 0);
  if(m_vessel_type_src != "")
    m_Comms.Register(m_vessel_type_src, 0);
  if(m_vessel_len_src != "")
    m_Comms.Register(m_vessel_len_src, 0);

  return(true);
}

//-----------------------------------------------------------------
// Procedure: handleLocalHelmSummary()
//     Notes: Handles the incoming summary variable posted by the 
//            helm. This routine grabs the "modes" field of that
//            summary for inclusion in the NODE_REPORT summary.

void NodeReporter::handleLocalHelmSummary(const string& sdata)
{
  vector<string> svector = parseString(sdata, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string left  = stripBlankEnds(biteString(svector[i], '='));
    string right = stripBlankEnds(svector[i]);
    if(left == "modes")
      m_helm_mode = right;
  }
}

//------------------------------------------------------------------
// Procedure: addPlatformVar
//   Purpose: Add components to the Platform Report

bool NodeReporter::addPlatformVar(string entry)
{
  string varname;
  string alias;
  double postgap=0;

  vector<string> svector = parseString(entry, ',');
  unsigned int i, isize = svector.size();
  for(i=0; i<isize; i++) {
    string left  = stripBlankEnds(biteString(svector[i],'='));
    string right = stripBlankEnds(svector[i]);
    if(right == "")
      varname = left;
    else {
      if(tolower(left) == "gap") {
	postgap = atof(right.c_str());
	if(postgap < 0)
	  postgap = 0;
      }
      else if(tolower(left == "alias")) {
	if(!strContainsWhite(right))
	  alias = right;
      }
    }
  }

  if(strContainsWhite(varname))
    return(false);
  if(vectorContains(m_plat_vars, varname))
    return(false);

  m_plat_vars.push_back(varname);
  m_plat_vals.push_back("");
  m_plat_alias.push_back(alias);
  m_plat_recv_tstamp.push_back(0);
  m_plat_post_tstamp.push_back(0);
  m_plat_post_gap.push_back(postgap);

  return(true);

}

//------------------------------------------------------------------
// Procedure: updatePlatformVar()
//   Purpose: 

void NodeReporter::updatePlatformVar(string variable, string value)
{
  unsigned int i, isize = m_plat_vars.size();
  for(i=0; i<isize; i++) {
    if(m_plat_vars[i] == variable) {
      m_plat_vals[i] = value;
      m_plat_recv_tstamp[i]  = m_moos_utc_time;
    }
  }
}


//------------------------------------------------------------------
// Procedure: assemblePlatformReport
//   Purpose: 

string NodeReporter::assemblePlatformReport()
{
  string return_str;
  unsigned int i, isize = m_plat_vars.size();
  for(i=0; i<isize; i++) {
    if(m_plat_recv_tstamp[i] != 0) {
      double time_since_last_post = m_moos_utc_time - m_plat_post_tstamp[i];
      bool   unposted_change = false;
      if((m_plat_recv_tstamp[i] - m_plat_post_tstamp[i]) > 0)
	unposted_change = true;
      if(unposted_change && (time_since_last_post > m_plat_post_gap[i])) {
	if(return_str != "")
	  return_str += ",";
	if(m_plat_alias[i] == "")
	  return_str += (m_plat_vars[i] + "=" + m_plat_vals[i]);
	else
	  return_str += (m_plat_alias[i] + "=" + m_plat_vals[i]);
	m_plat_post_tstamp[i] = m_moos_utc_time;
      }
    }
  }
  if(return_str != "") {
    string augment_str = ("platform=" + m_vessel_name + ",");
    augment_str += "utc_time=" + doubleToString(m_moos_utc_time) + ",";
    return_str = augment_str + return_str;
  }
  return(return_str);
}


//------------------------------------------------------------------
// Procedure: assembleNodeReport
//   Purpose: Assemble the node report from member variables.

string NodeReporter::assembleNodeReport()
{
  // If the length is unknown, put in some good guesses
  if(m_vessel_len == "0") {
    if(tolower(m_vessel_type) == "kayak")
      m_vessel_len = "4.0"; // meters;
    if(tolower(m_vessel_type) == "auv")
      m_vessel_len = "4.0"; // meters;
    if(tolower(m_vessel_type) == "uuv")
      m_vessel_len = "4.0"; // meters;
    if(tolower(m_vessel_type) == "ship")
      m_vessel_len = "18.0"; // meters
    if(tolower(m_vessel_type) == "glider")
      m_vessel_len = "3.0"; // meters
  }
  
  string summary = "NAME=" + m_vessel_name;
  summary += ",TYPE=" + m_vessel_type;
  summary += ",MOOSDB_TIME=" + doubleToString(m_db_uptime,2);
  summary += ",UTC_TIME=" + doubleToString(m_moos_utc_time,2);

  if((m_navx_recd > 0) && (m_navy_recd > 0)) {
    summary += ",X="   + doubleToString(m_nav_x,2);
    summary += ",Y="   + doubleToString(m_nav_y,2);
  }
  if((m_navlat_recd > 0) && (m_navlon_recd > 0)) {
    summary += ",LAT=" + doubleToString(m_nav_lat,6);
    summary += ",LON=" + doubleToString(m_nav_lon,6);
  }
  summary += ",SPD=" + doubleToString(m_nav_speed,2);
  summary += ",HDG=" + doubleToString(m_nav_heading,2);
  summary += ",YAW=" + doubleToString(m_nav_yaw,5);
  summary += ",DEPTH=" + doubleToString(m_nav_depth,2);
  summary += ",LENGTH=" + m_vessel_len;
  
  string  mode_str = ",MODE=";
  if((m_db_uptime-m_helm_lastmsg) > m_nohelm_thresh) {
    string awol_time = "EVER";
    if(m_helm_lastmsg > 0)
      awol_time = doubleToString((m_db_uptime-m_helm_lastmsg), 0);
    mode_str += "NOHELM-" + awol_time;
  }
  else if(m_helm_engaged == false)
    mode_str += "DISENGAGED";
  else {
    if(m_helm_mode != "none")
      mode_str += m_helm_mode;
    else
      mode_str += "ENGAGED";
  }
  summary += mode_str;
  summary += ",ALLSTOP=" + m_helm_allstop_mode;

  return(summary);
}


//------------------------------------------------------------------
// Procedure: setCrossFillPolicy
//      Note: Determines how or whether the local and global coords
//            are updated from one another.

void NodeReporter::setCrossFillPolicy(string policy)
{
  policy = tolower(policy);
  policy = findReplace(policy, '_', '-');
  if((policy=="literal")||(policy=="fill-empty")||(policy=="use-latest"))
    m_crossfill_policy = policy;
}


//------------------------------------------------------------------
// Procedure: crossFillCoords
//   Purpose: Potentially update NAV_X/Y from NAV_LAT/LON or v.versa.

void NodeReporter::crossFillCoords()
{
  // The "fill-empty" policy will fill the other coordinates only if 
  // the other coordinates have NEVER been written to.
  if(m_crossfill_policy == "fill-empty") {
    if((m_navx_recd == 0) && (m_navy_recd == 0) &&
       (m_navlat_recd > 0) && (m_navlon_recd > 0))
      crossFillGlobalToLocal();
    if((m_navx_recd > 0) && (m_navy_recd > 0) &&
       (m_navlat_recd == 0) && (m_navlon_recd == 0)) {
      crossFillLocalToGlobal();
    }
  }

  // The "use-latest" policy will fill the other coordinates only if 
  // the other coordinates are more stale than their counterpart
  if(m_crossfill_policy == "use-latest") {
    double t_local  = min(m_navx_recd, m_navy_recd);
    double t_global = min(m_navlat_recd, m_navlon_recd);
    if(t_local == t_global)
      return;

    if(t_local < t_global)       // Local coords older
      crossFillGlobalToLocal();
    else                         // Global coords older
      crossFillLocalToGlobal();
  }
}


//------------------------------------------------------------------
// Procedure: crossFillLocalToGlobal
//   Purpose: Update the Global coordinates based on the Local coords.

void NodeReporter::crossFillLocalToGlobal()
{
#ifdef USE_UTM
  m_geodesy.LocalGrid2LatLong(m_nav_x, m_nav_y, m_nav_lat, m_nav_lon);
#else
  m_geodesy.UTM2LatLong(m_nav_x, m_nav_y, m_nav_lat, m_nav_lon);
#endif      
  m_navlat_recd = m_navy_recd;
  m_navlon_recd = m_navx_recd;
}

//------------------------------------------------------------------
// Procedure: crossFillGlobalToLocal
//   Purpose: Update the Local coordinates based on the Global coords.

void NodeReporter::crossFillGlobalToLocal()
{
  cout << "In GlobalToLocal!!!!" << endl;
  cout << "Before: " << endl;
  cout << "  m_nav_lat: " << m_nav_lat << endl;
  cout << "  m_nav_lon: " << m_nav_lon << endl;
  cout << "  m_nav_x: " << m_nav_x << endl;
  cout << "  m_nav_y: " << m_nav_y << endl;
#ifdef USE_UTM
  cout << "Using UTM" << endl;
  m_geodesy.LatLong2LocalUTM(m_nav_lat, m_nav_lon, m_nav_y, m_nav_x);
#else
  cout << "Using LOCAL" << endl;
  m_geodesy.LatLong2LocalGrid(m_nav_lat, m_nav_lon, m_nav_y, m_nav_x);
#endif      


  cout << "AFTER: " << endl;
  cout << "  m_nav_lat: " << m_nav_lat << endl;
  cout << "  m_nav_lon: " << m_nav_lon << endl;
  cout << "  m_nav_x: " << m_nav_x << endl;
  cout << "  m_nav_y: " << m_nav_y << endl;

  m_navx_recd = m_navlon_recd;
  m_navy_recd = m_navlat_recd;
}
