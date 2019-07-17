/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: NodeReporter.cpp                                     */
/*    DATE: Feb 13th 2006 (TransponderAIS)                       */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <unistd.h>
#include <list>
#include <iterator>
#include <algorithm>
#include "NodeReporter.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "NodeRecord.h"
#include "ACBlock.h"

// As of Release 15.4 this is now set in CMake, defaulting to be defined
// #define USE_UTM 

using namespace std;

//-----------------------------------------------------------------
// Constructor

NodeReporter::NodeReporter()
{
  m_vessel_name  = "unknown";

  m_reports_posted         = 0;
  m_reports_posted_alt_nav = 0;
  m_thrust_mode_reverse    = false;

  m_record.setType("unknown");
  m_record.setLength(0);

  m_helm_status_primary = "unknown";
  m_helm_status_standby = "";
  m_helm_lastmsg      = -1;
  m_nohelm_thresh     = 5;
  m_helm_mode         = "none";
  m_helm_allstop_mode = "unknown";
  m_helm_switch_noted = false;
  m_terse_reports     = false;

  m_blackout_interval = 0;
  m_blackout_baseval  = 0;
  m_blackout_variance = 0;
  m_last_post_time    = -1;
  m_paused            = false;
  m_node_info_posted  = false;
  
  // Below is a good alt_nav prefix suggestion,  but left blank to keep
  // feature off unless another app is generating the alt nav solution.
  // m_alt_nav_prefix    = "NAV_GT";
  m_alt_nav_name      = "_GT";

  m_crossfill_policy = "literal";

  // Timestamps used for executing the "use-latest" crossfill policy 
  m_nav_xy_updated        = 0;
  m_nav_latlon_updated    = 0;
  m_nav_xy_updated_gt     = 0;
  m_nav_latlon_updated_gt = 0;

  m_record_gt_updated     = 0;

  m_node_report_var = "NODE_REPORT_LOCAL";
  m_plat_report_var = "PLATFORM_REPORT_LOCAL";
}

//-----------------------------------------------------------------
// Procedure: OnNewMail

bool NodeReporter::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key   = msg.m_sKey;
    string sdata = msg.m_sVal;
    double ddata = msg.m_dfVal;

    if(key == "NAV_X") {
      m_record.setX(ddata);
      m_nav_xy_updated = m_curr_time;
    }
    else if(key == "NAV_Y") {
      m_record.setY(ddata);
      m_nav_xy_updated = m_curr_time;
    }
    else if(key == "NAV_LAT") {
      m_record.setLat(ddata);
      m_nav_latlon_updated = m_curr_time;
    }
    else if(key == "NAV_LONG") {
      m_record.setLon(ddata);
      m_nav_latlon_updated = m_curr_time;
    }
    else if(key == "NAV_SPEED")
      m_record.setSpeed(ddata);
    else if(key == "NAV_HEADING")
      m_record.setHeading(angle360(ddata));
    else if(key == "NAV_DEPTH")
      m_record.setDepth(ddata);
    else if(key == "NAV_YAW") 
      m_record.setYaw(ddata);
    else if(key == "NAV_TRAJECTORY")
      m_record.setTrajectory(sdata);
    else if(key == "THRUST_MODE_REVERSE") {
      bool reverse = (tolower(sdata)=="true");
      m_record.setThrustModeReverse(reverse);
    }
    // BEGIN logic for checking for alternative nav reporting
    if(m_alt_nav_prefix != "") {
      bool record_gt_updated = true;
      if(key == (m_alt_nav_prefix + "X")) {
	m_record_gt.setX(ddata);
	m_nav_xy_updated_gt = m_curr_time;
      }
      else if(key == (m_alt_nav_prefix + "Y")) {
	m_record_gt.setY(ddata);
	m_nav_xy_updated_gt = m_curr_time;
      }
      else if(key == (m_alt_nav_prefix + "LAT")) {
	m_record_gt.setLat(ddata);
	m_nav_latlon_updated_gt = m_curr_time;
      }
      else if(key == (m_alt_nav_prefix + "LONG")) {
	m_record_gt.setLon(ddata);
	m_nav_latlon_updated_gt = m_curr_time;
      }
      else if(key == (m_alt_nav_prefix + "SPEED"))
	m_record_gt.setSpeed(ddata);
      else if(key == (m_alt_nav_prefix + "HEADING"))
	m_record_gt.setHeading(angle360(ddata));
      else if(key == (m_alt_nav_prefix + "DEPTH"))
	m_record_gt.setDepth(ddata);
      else if(key == (m_alt_nav_prefix + "YAW"))
	m_record_gt.setYaw(ddata);
      else 
	record_gt_updated = false;
      
      if(record_gt_updated)
	m_record_gt_updated = m_curr_time;
    }
    // END logic for checking for alternative nav reporting
    
    if(key == "AUX_MODE") 
      m_record.setModeAux(sdata);

    else if(key == "PNR_PAUSE") 
      setBooleanOnString(m_paused, sdata);

    else if(key == "LOAD_WARNING") {
      string app = tokStringParse(sdata, "app", ',', '=');
      string gap = tokStringParse(sdata, "maxgap", ',', '=');
      if((app != "") && (gap != ""))
	m_record.setLoadWarning(app + ":" + gap);
    }

    else if(key == "IVPHELM_SUMMARY") {
      m_helm_lastmsg = m_curr_time;
      handleLocalHelmSummary(sdata);
    }
    else if(key == "IVPHELM_ALLSTOP") {
      m_helm_lastmsg = m_curr_time;
      m_helm_allstop_mode = sdata;
    }
    else if(key == "IVPHELM_STATE") {
      m_helm_lastmsg = m_curr_time;
      string helm_status = stripBlankEnds(sdata);
      if((helm_status == "DRIVE")     ||
	 (helm_status == "PARK")      ||
	 (helm_status == "MALCONFIG") ||
	 (helm_status == "DISABLED"))
	m_helm_status_primary = helm_status;
      else if((helm_status == "DRIVE+")     ||
	      (helm_status == "PARK+")      ||
	      (helm_status == "MALCONFIG+") ||
	      (helm_status == "STANDBY"))
	m_helm_status_standby = helm_status;
      
      if(strEnds(helm_status, "+"))
	handleHelmSwitch();
      if(strBegins(helm_status, "MALCONFIG"))
	m_helm_allstop_mode = "n/a";
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
  AppCastingMOOSApp::RegisterVariables();

  Register("NAV_*", "*", 0);

  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("NAV_LAT", 0);
  Register("NAV_LONG", 0);
  Register("NAV_SPEED", 0);
  Register("NAV_HEADING", 0);
  Register("NAV_YAW", 0);
  Register("NAV_DEPTH", 0);
  Register("NAV_TRAJECTORY", 0);

  if(m_alt_nav_prefix != "") {
    if(!strEnds(m_alt_nav_prefix, "_"))
      m_alt_nav_prefix += "_";
    Register(m_alt_nav_prefix + "X", 0);
    Register(m_alt_nav_prefix + "Y", 0);
    Register(m_alt_nav_prefix + "LAT", 0);
    Register(m_alt_nav_prefix + "LONG", 0);
    Register(m_alt_nav_prefix + "SPEED", 0);
    Register(m_alt_nav_prefix + "HEADING", 0);
    Register(m_alt_nav_prefix + "YAW", 0);
    Register(m_alt_nav_prefix + "DEPTH", 0);
  }  

  Register("IVPHELM_SUMMARY", 0);
  Register("IVPHELM_STATE", 0);
  Register("IVPHELM_ALLSTOP", 0);
  Register("AUX_MODE", 0);
  Register("LOAD_WARNING", 0);
  Register("THRUST_MODE_REVERSE", 0);
  
  Register("PNR_PAUSE", 0);

}

//-----------------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool NodeReporter::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  Notify("PNODEREPORTER_PID", getpid());
    
  // m_host_community set in superclass OnStartUp()
  m_vessel_name = m_host_community;

   // look for latitude, longitude global variables
  double lat_origin, lon_origin;
  bool ok1 = m_MissionReader.GetValue("LatOrigin", lat_origin);
  bool ok2 = m_MissionReader.GetValue("LongOrigin", lon_origin);
  if(!ok1 || !ok2)
    reportConfigWarning("Lat or Lon Origin not set in *.moos file.");

  // initialize m_geodesy
  if(!m_geodesy.Initialise(lat_origin, lon_origin))
    reportConfigWarning("Geodesy init failed.");
  
  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams)) 
    reportConfigWarning("No config block found for " + GetAppName());
  
  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;
    double dval  = atof(value.c_str());

    bool handled = false;
    if((param == "platform_type") ||           // Preferred
       (param == "vessel_type")) {             // Deprecated
      m_record.setType(value);
      handled = true;
    }
    if((param == "platform_color") && isColor(value)) { 
      m_record.setColor(value);
      handled = true;
    }
    else if((param =="platform_length") ||     // Preferred
	    (param =="vessel_length")) {       // Deprecated
      if(isNumber(value) && (dval >= 0)) {
	m_record.setLength(dval);
	handled = true;
      }
    }
    else if(param == "terse_reports") 
      handled = setBooleanOnString(m_terse_reports, value);
    else if(param == "blackout_interval") 
      handled = setNonNegDoubleOnString(m_blackout_interval, value);
    else if(param == "blackout_variance") 
      handled = setNonNegDoubleOnString(m_blackout_variance, value);
    else if((param == "plat_report_input") ||     // Preferred
	    (param == "plat_report_var")   ||     // Deprecated
	    (param == "platform_variable")) {     // Deprecated
      addPlatformVar(value);
      handled = true;
    }
    else if((param == "node_report_output")  ||   // Preferred
	    (param == "node_report_summary") ||   // Deprecated
	    (param == "node_report_var")) {       // Deprecated
      if(!strContainsWhite(value)) {
	m_node_report_var = value;
	handled = true;
      }
    }      
    else if(param == "plat_report_output") {
      if(!strContainsWhite(value)) {
	m_plat_report_var = value;
	handled = true;
      }
    }      
    else if(param == "group") {
      m_group_name = value;
      handled = true;
    }
    else if(param == "paused")
      handled = setBooleanOnString(m_paused, value);
    else if(param == "nohelm_threshold") {
      if(isNumber(value) && (dval > 0)) {
	m_nohelm_thresh = dval;
	handled = true;
      }
    }      
    else if(param =="crossfill_policy")
      handled = setCrossFillPolicy(value);
    else if(param =="alt_nav_prefix") {
      m_alt_nav_prefix = value;
      handled = true;
    }
    else if(param =="alt_nav_name") {
      m_alt_nav_name = value;
      handled = true;
    }
    
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  registerVariables();
  unsigned int k, ksize = m_plat_vars.size();
  for(k=0; k<ksize; k++)
    Register(m_plat_vars[k], 0);

  // If the length is unknown, put in some good guesses
  if(m_record.getLength() == 0) {
    string vtype = tolower(m_record.getType());
    if((vtype == "kayak") || (vtype == "mokai"))
      m_record.setLength(4); // meters;
    else if(vtype == "auv")
      m_record.setLength(4); // meters;
    else if(vtype == "uuv")
      m_record.setLength(4); // meters;
    else if(vtype == "ship")
      m_record.setLength(8); // meters
    else if(vtype == "glider")
      m_record.setLength(3); // meters
    else if(vtype == "usv")
      m_record.setLength(6); // meters
    else if(vtype == "kingfisher")
      m_record.setLength(3); // meters
    else
      reportConfigWarning("Unrecognized platform type: " + vtype);
  }
  
  m_record.setName(m_vessel_name);
  m_record.setGroup(m_group_name);
  // To start with m_record_gt is just a copy of m_record.
  m_record_gt = m_record;       

  if(strBegins(m_alt_nav_name, "_"))
    m_record_gt.setName(m_vessel_name + m_alt_nav_name);
  else
    m_record_gt.setName(m_alt_nav_name);
  
  return(true);
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
  AppCastingMOOSApp::Iterate();

  // If (a) this is the first chance to post, or (b) there is no
  // blackout interval being implemented, or (c) the time since last
  // post exceeds the blackout interval, then perform a posting.
  if((m_last_post_time == -1) || (m_blackout_interval <= 0) ||
     ((m_curr_time - m_last_post_time) > m_blackout_interval)) {

    if(m_crossfill_policy != "literal")
      crossFillCoords(m_record, m_nav_xy_updated, m_nav_latlon_updated);
    
    m_record.setIndex(m_reports_posted);
    string report = assembleNodeReport(m_record);    
    if(!m_paused) {
      if(m_reports_posted == 0) 
	Notify(m_node_report_var+"_FIRST", report);
      Notify(m_node_report_var, report);
      m_reports_posted++;
    }

    double elapsed_time = m_curr_time - m_record_gt_updated;
    if(elapsed_time < 5) {
      if(m_crossfill_policy != "literal")
	crossFillCoords(m_record_gt, m_nav_xy_updated_gt, 
			m_nav_latlon_updated_gt);
      
      m_record_gt.setIndex(m_reports_posted);
      string report_gt = assembleNodeReport(m_record_gt);
      if(!m_paused) {
	Notify(m_node_report_var, report_gt);
	m_reports_posted_alt_nav++;
      }
    }

    // Note the post time and apply it to the blackout calculation
    m_last_post_time = m_curr_time;
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
  if((platform_report != "") && !m_paused)
    Notify(m_plat_report_var, platform_report);
  
  m_record.setLoadWarning("");
  AppCastingMOOSApp::PostReport();
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
    string left  = biteStringX(svector[i], '=');
    string right = svector[i];
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
      m_plat_recv_tstamp[i]  = m_curr_time;
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
      double time_since_last_post = m_curr_time - m_plat_post_tstamp[i];
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
	m_plat_post_tstamp[i] = m_curr_time;
      }
    }
  }
  if(return_str != "") {
    string augment_str = ("platform=" + m_vessel_name + ",");
    augment_str += "utc_time=" + doubleToString(m_curr_time) + ",";
    return_str = augment_str + return_str;
  }
  return(return_str);
}


//------------------------------------------------------------------
// Procedure: assembleNodeReport
//   Purpose: Assemble the node report from member variables.

string NodeReporter::assembleNodeReport(NodeRecord record)
{
  record.setTimeStamp(m_curr_time); 

  // First set the mode field

  // (1) If the info has never been received or is stale we want
  // to indicate so.
  string  mode;
  if((m_curr_time-m_helm_lastmsg) > m_nohelm_thresh) {
    string awol_time = "EVER";
    if(m_helm_lastmsg > 0)
      awol_time = doubleToString((m_curr_time-m_helm_lastmsg), 0);
    mode = "NOHELM-" + awol_time;
  }
  // (2) If helm is in DRIVE but has provided mode information, 
  // then use the mode information in the node report.
  else {
    // First determine the helm_status, possibly using info
    // reported by a standby that has stepped in.
    string helm_status = m_helm_status_primary;
    if((m_helm_status_standby == "DRIVE+") ||
       (m_helm_status_standby == "PARK+")) {
      helm_status = m_helm_status_standby;
    }
    mode = helm_status;
    // Now check if the helm is in DRIVE and has a mode defined
    if((helm_status == "DRIVE") || (helm_status == "DRIVE+")) {
      if((m_helm_mode !="none") && (m_helm_mode != ""))
	mode = m_helm_mode;
    }

  }
  
  record.setMode(mode);
  record.setAllStop(m_helm_allstop_mode);

  string summary = record.getSpec(m_terse_reports);
  return(summary);
}

//------------------------------------------------------------------
// Procedure: setCrossFillPolicy
//      Note: Determines how or whether the local and global coords
//            are updated from one another.

bool NodeReporter::setCrossFillPolicy(string policy)
{
  policy = tolower(policy);
  policy = findReplace(policy, '_', '-');
  if((policy=="literal")||(policy=="fill-empty")||(policy=="use-latest")) {
    m_crossfill_policy = policy;
    return(true);
  }
  
  return(false);
}



//------------------------------------------------------------------
// Procedure: crossFillCoords
//   Purpose: Potentially update NAV_X/Y from NAV_LAT/LON or v.versa.

void NodeReporter::crossFillCoords(NodeRecord& record, 
				   double nav_xy_updated,
				   double nav_latlon_updated)
{
  // The "fill-empty" policy will fill the other coordinates only if 
  // the other coordinates have NEVER been written to.
  if(m_crossfill_policy == "fill-empty") {
    if(!record.valid("x") && !record.valid("y") &&
       record.valid("lat") && record.valid("lon"))
      crossFillGlobalToLocal(record);
    if(record.valid("x") && record.valid("y") &&
       !record.valid("lat") && !record.valid("lon")) {
      crossFillLocalToGlobal(record);
    }
  }

  // The "use-latest" policy will fill the other coordinates only if 
  // the other coordinates are more stale than their counterpart
  if(m_crossfill_policy == "use-latest") {
    if(nav_xy_updated == nav_latlon_updated)
      return;

    if(nav_xy_updated < nav_latlon_updated) // Local coords older
      crossFillGlobalToLocal(record);
    else                                    // Global coords older
      crossFillLocalToGlobal(record);
  }
}


//------------------------------------------------------------------
// Procedure: crossFillLocalToGlobal
//   Purpose: Update the Global coordinates based on the Local coords.

void NodeReporter::crossFillLocalToGlobal(NodeRecord& record)
{
  double nav_x = record.getX();
  double nav_y = record.getY();

  double nav_lat, nav_lon;

#ifdef USE_UTM
  m_geodesy.UTM2LatLong(nav_x, nav_y, nav_lat, nav_lon);
#else
  m_geodesy.LocalGrid2LatLong(nav_x, nav_y, nav_lat, nav_lon);
#endif      

  record.setLat(nav_lat);
  record.setLon(nav_lon);  
}

//------------------------------------------------------------------
// Procedure: crossFillGlobalToLocal
//   Purpose: Update the Local coordinates based on the Global coords.

void NodeReporter::crossFillGlobalToLocal(NodeRecord& record)
{
  double nav_lat = record.getLat();
  double nav_lon = record.getLon();

  double nav_x, nav_y;

#ifdef USE_UTM
  m_geodesy.LatLong2LocalUTM(nav_lat, nav_lon, nav_y, nav_x);
#else
  m_geodesy.LatLong2LocalGrid(nav_lat, nav_lon, nav_y, nav_x);
#endif      

  record.setX(nav_x);
  record.setY(nav_y);
}


//------------------------------------------------------------------
// Procedure: handleHelmSwitch
//   Purpose: When or if a helm switch has been noted, reset some
//            locally held information that may have been due to the
//            the disabled helm, and let them be repopulated by the
//            helm that has stepped in.

void NodeReporter::handleHelmSwitch()
{
  if(m_helm_switch_noted) 
    return;
  
  m_helm_mode = "";
  m_helm_allstop_mode = "";
  m_helm_switch_noted = true;
}


//-----------------------------------------------------------------
// Procedure: buildReport()
//      Note: A virtual function of the AppCastingMOOSApp superclass, 
//            conditionally invoked if either a terminal or appcast 
//            report is needed.
// 
// Configuration Summary:
// ---------------------------------
// Vehicle name:    henry
// Platform type:   uuv
// Platform length: 4.2
// 
// BLACKOUT_INTERVAL  = 0
//
// NODE_REPORT_OUTPUT = NODE_REPORT_LOCAL
// NOHELM_THRESHOLD   = 5
// CROSSFILL_POLICY   = literal
// 
// ALT_NAV_PREFIX     = NAV_GT
// ALT_NAV_NAME       = _GT
//
//
//
// Alt Nav Updates: 276 



// Latest posting:
// NODE_REPORT_LOCAL = NAME=alpha,TYPE=UUV,TIME=1252348077.59,   
//                      X=51.71,Y=-35.50, LAT=43.824981,          
//                      LON=-70.329755,SPD=2.0,HDG=118.8,         
//                      YAW=118.8,DEPTH=4.6,LENGTH=3.8,           
//                      MODE=MODE@ACTIVE:LOITERING      

bool NodeReporter::buildReport()
{
  string str_vlength  = doubleToString(m_record.getLength(),1);
  string str_blackout = doubleToString(m_blackout_interval,2);
  string str_bovariance = doubleToString(m_blackout_variance,2);

  m_msgs << "Paused: " << boolToString(m_paused) << endl; 
  m_msgs << "Vehicle Configuration:"                    << endl;
  m_msgs << "----------------------------"              << endl;
  m_msgs << "     Vehicle name: " << m_vessel_name      << endl;
  m_msgs << "     Vehicle type: " << m_record.getType() << endl; 
  m_msgs << "   Vehicle length: " << str_vlength        << endl;
  m_msgs << endl;
  m_msgs << "Blackout Configuration:"                   << endl;
  m_msgs << "----------------------------"              << endl;
  m_msgs << "Blackout interval: " << str_blackout       << endl;
  m_msgs << "Blackout variance: " << str_bovariance     << endl;
  m_msgs  << endl;
  m_msgs << "AltNav Config and Status:"                 << endl;
  m_msgs << "----------------------------"              << endl;
  m_msgs << "   ALT_NAV_PREFIX: " << m_alt_nav_prefix   << endl;
  m_msgs << "     ALT_NAV_NAME: " << m_alt_nav_name     << endl;
  m_msgs << " ALT_NAV_POSTINGS: " << m_reports_posted_alt_nav << endl;
  m_msgs << endl;
  m_msgs << "Node Report Summary:"                 << endl;
  m_msgs << "----------------------------"         << endl;
  m_msgs << "Reports Posted: " << m_reports_posted << endl;
  
  string report = assembleNodeReport(m_record);    
  ACBlock block(" Latest Report: ", report, 50);
  m_msgs << block.getFormattedString();

  if(m_record_gt_updated > 0) {
    string report_gt = assembleNodeReport(m_record_gt);    
    ACBlock block_gt(" Latest GT Report: ", report_gt, 50);
    m_msgs << block_gt.getFormattedString();
  }

  return(true);
}
