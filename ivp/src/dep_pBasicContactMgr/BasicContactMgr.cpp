/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BasicContactMgr.cpp                                  */
/*    DATE: Feb 24th 2010                                        */
/*    DATE: Sep 26th 2017 Added Alert Region Support / mikerb    */
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

#ifdef _WIN32
  #define _USE_MATH_DEFINES
#endif
#include <unistd.h>
#include <cmath>
#include <iterator>
#include <vector>
#include "LinearExtrapolator.h"
#include "BasicContactMgr.h"
#include "MBUtils.h"
#include "ColorParse.h"
#include "CPAEngine.h"
#include "NodeRecordUtils.h"
#include "XYCircle.h"
#include "XYFormatUtilsPoly.h"
#include "ACTable.h"

using namespace std;

//---------------------------------------------------------
// Constructor

BasicContactMgr::BasicContactMgr()
{
  // Configuration Variables
  m_default_alert_rng           = 1000;
  m_default_alert_rng_cpa       = 1000;
  m_default_alert_rng_color     = "gray65";
  m_default_alert_rng_cpa_color = "gray35";

  m_display_radii      = false;
  m_post_closest_range = false;
  m_contact_max_age    = 600;      // units in seconds 600 = 10 mins
  m_contacts_recap_interval = 1;

  // verbatim, lazy_lat_lon, or force_lat_lon
  m_contact_local_coords = "verbatim"; 
  m_alert_verbose = false;
  m_decay_start = 15;
  m_decay_end   = 30;

  m_eval_range_far  = 20;
  m_eval_range_near = 10;
  m_prev_closest_contact_val = 0;

  m_max_retired_hist   = 5;
  m_use_geodesy = false;

  m_range_report_timeout = 10;
  
  m_reject_range = 2000;
  
  // State Variables
  m_nav_x   = 0;
  m_nav_y   = 0;
  m_nav_hdg = 0;
  m_nav_spd = 0;

  m_contacts_recap_posted = 0;
  m_prev_contacts_count   = 0;
  m_prev_closest_range    = 0;

  m_alert_requests_received = 0;

  // This app is deprecated. Suggest an alternative
  m_deprecated_alt = "pContactMgrV20";
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool BasicContactMgr::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
	
    string key   = msg.GetKey();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    
    if(key == "NAV_X")
      m_nav_x = dval;
    else if(key == "NAV_Y")
      m_nav_y = dval;
    else if(key == "NAV_HEADING")
      m_nav_hdg = dval;
    else if(key == "NAV_SPEED") 
      m_nav_spd = dval;
    else if(key == "NODE_REPORT") 
      handleMailNodeReport(sval);
    else if(key == "BCM_REPORT_REQUEST") 
      handleMailReportRequest(sval);
    else if(key == "BCM_DISPLAY_RADII")
      handleMailDisplayRadii(sval);      
    else if(key == "BCM_ALERT_REQUEST")
      handleMailAlertRequest(sval);      
    else if(key == "CONTACT_RESOLVED")
      handleMailResolved(sval);

    else
      reportRunWarning("Unhandled Mail: " + key);
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool BasicContactMgr::OnConnectToServer()
{
  registerVariables();  
  return(true);
}


//---------------------------------------------------------
// Procedure: Iterate()
//      Note: Happens AppTick times per second

bool BasicContactMgr::Iterate()
{
  AppCastingMOOSApp::Iterate();

  updateRanges();
  checkForNewRetiredContacts();

  postRangeReports();
  postSummaries();
  checkForAlerts();
  checkForCloseInReports();
  
  if(m_display_radii)
    postRadii();

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool BasicContactMgr::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  // PID published to support uMemWatch or similar oversight
  Notify("PBASICCONTACTMGR_PID", getpid());

  m_ownship = m_host_community;
  if(m_ownship == "") {
    cout << "Vehicle Name (MOOS community) not provided" << endl;
    return(false);
  }
  
  // Part 1: Set the basic configuration parameters.
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  
  STRING_LIST::reverse_iterator p;
  for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
    string orig  = *p;
    string line  = stripBlankEnds(*p);
    string param = tolower(biteStringX(line, '='));
    string value = line;
    double dval  = atof(value.c_str());
    
    if(param == "alert") {
      bool ok = handleConfigAlert(value);
      if(!ok) 
	reportConfigWarning("Failed alert config: " + value);
    }


    else if(param == "ignore_group") {
      bool handled = handleConfigIgnoreGroup(value);
      if(!handled)
	reportConfigWarning("Unhandled ignore_group: " + value);
    }

    else if(param == "decay") {
      string left  = biteStringX(value, ',');
      string right = value;
      bool   handled = false;
      if(isNumber(left) && isNumber(right)) {
	double start = atof(left.c_str());
	double end   = atof(right.c_str());
	if((start >= 0) && (start <= end)) {
	  m_decay_start = start;
	  m_decay_end   = end;
	  handled = true;
	}
      }
      if(!handled)
	reportConfigWarning("Unhandled decay config: " + orig);	
    }  


    else if(param == "range_report_timeout") {
      bool ok = setNonNegDoubleOnString(m_range_report_timeout, value);
      if(!ok) {
	string msg = "range_report_timeout cannot be negative val";
	reportConfigWarning(msg);
      }
    }
    else if(param == "display_radii")
      setBooleanOnString(m_display_radii, value);
    else if(param == "post_closest_range") {
      bool ok = setBooleanOnString(m_post_closest_range, value);
      if(!ok) {
	string msg = "post_closest_range must be a Boolean";
	reportConfigWarning(msg);
      }
    }
    else if(param == "max_retired_history") {
      bool ok = setUIntOnString(m_max_retired_hist, value);
      if(!ok || (m_max_retired_hist > 50) || (m_max_retired_hist < 1)) {
	string msg = "max_retired_hist should be in range [1,50]";
	reportConfigWarning(msg);
      }
      if(m_max_retired_hist > 50)
	m_max_retired_hist = 50;
      if(m_max_retired_hist < 1)
	m_max_retired_hist = 1;
    }
    else if(param == "contact_local_coords") {
      string lval = tolower(value);
      if((lval== "verbatim") || (lval=="lazy_lat_lon") ||
	 (lval=="force_lat_lon"))
	m_contact_local_coords = lval;
      else {
	string msg = "Illegal contact_local_coords configuration";
	reportConfigWarning(msg);
      }
    }
    else if((param=="alert_range") || (param=="default_alert_range")) {
      bool ok = setPosDoubleOnString(m_default_alert_rng, value);
      if(!ok)
	reportConfigWarning("alert_range must be > zero: " + value);
    }
    
    else if((param=="alert_cpa_range") || (param=="default_cpa_range")) {
      bool ok = setPosDoubleOnString(m_default_alert_rng_cpa, value);
      if(!ok) {
	string msg = "default_alert_range_cpa must be >0: " + value;
	reportConfigWarning(msg);
      }
    }
    
    else if(param == "contacts_recap_interval") {
      if(tolower(value) == "off")
	m_contacts_recap_interval = -1;
      else if(dval >= 0)
	m_contacts_recap_interval = dval;
      else {
	string msg = "contacts_recap_interval must be >=0: " + value;
	reportConfigWarning(msg);
      }
    }
    
    else if(param == "reject_range") {
      if(tolower(value) == "off")
	m_reject_range = -1;
      else if(dval >= 0)
	m_reject_range = dval;
      else {
	string msg = "reject_range must be >=0: " + value;
	reportConfigWarning(msg);
      }
    }
    
    else if(param=="alert_cpa_time") {
      string msg = "alert_cpa_time parameter has been deprecated.";
      reportConfigWarning(msg);
    }
    
    else if(param=="alert_verbose") {
      bool ok = setBooleanOnString(m_alert_verbose, value);
      if(!ok)
	reportConfigWarning("alert_verbose must be either true or false");
    }
    
    else if(param == "default_alert_range_color") {
      if(isColor(value))
	m_default_alert_rng_color = value;
      else {
	string msg = "default_alert_range_color, " + value;
	msg += ", not a color";
	reportConfigWarning(msg);
      }
    }
    
    else if(param == "default_cpa_range_color") {
      if(isColor(value))
	m_default_alert_rng_cpa_color = value;
      else {
	string msg = "default_alert_range_cpa_color, " +value;
	msg += ", not a color";
	reportConfigWarning(msg);
      }
    }
    
    else if(param == "contact_max_age") {
      if(dval > 0)
	m_contact_max_age = dval;
      else
	reportConfigWarning("contact_max_age must be > zero: " + value);
    }
    
    else if(param == "eval_range_far") {
      if(dval >= 0) {
	m_eval_range_far = dval;
	if(m_eval_range_near > m_eval_range_far)
	  m_eval_range_near = m_eval_range_far;
      }
      else {
	string msg = "eval_range_far must be >= zero: ";
	msg += value;
	reportConfigWarning(msg);
      }
    }
    
    else if(param == "eval_range_near") {
      if(dval >= 0) {
	m_eval_range_near = dval;
	if(m_eval_range_far < m_eval_range_near)
	  m_eval_range_far = m_eval_range_near;
      }
      else {
	string msg = "eval_range_near must be >= zero: ";
	msg += value;
	reportConfigWarning(msg);
      }
    }
    
    else 
      reportUnhandledConfigWarning(orig);
  }

  // Part 2: If we may possibly want to set our incoming X/Y report
  // values based on Lat/Lon values, then we must check for and
  // initialized the MOOSGeodesy.
  if(m_contact_local_coords != "verbatim") {
    // look for latitude, longitude global variables
    double lat_origin, lon_origin;
    if(!m_MissionReader.GetValue("LatOrigin", lat_origin)) {
      reportConfigWarning("No LatOrigin in *.moos file");
      reportConfigWarning("Wont derive x/y from lat/lon in node reports.");
    }
    else if (!m_MissionReader.GetValue("LongOrigin", lon_origin)) {
      reportConfigWarning("No LongOrigin set in *.moos file");
      reportConfigWarning("Wont derive x/y from lat/lon in node reports.");
    }
    else if(!m_geodesy.Initialise(lat_origin, lon_origin)) {
      reportConfigWarning("Lat/Lon Origin found but Geodesy init failed.");
      reportConfigWarning("Wont derive x/y from lat/lon in node reports.");
    }
    else {
      m_use_geodesy = true;
      string msg = "Geodesy init ok: ";
      msg += "Will derive x/y from lat/lon in node reports."; 
      reportEvent(msg);
    }
  }
  
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void BasicContactMgr::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  Register("NODE_REPORT", 0);
  Register("CONTACT_RESOLVED", 0);
  Register("BCM_DISPLAY_RADII", 0);
  Register("BCM_ALERT_REQUEST", 0);
  Register("BCM_REPORT_REQUEST", 0);
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("NAV_SPEED", 0);
  Register("NAV_HEADING", 0);
}

//---------------------------------------------------------
// Procedure: handleMailNodeReport
//   Example: NAME=alpha,TYPE=KAYAK,UTC_TIME=1267294386.51,
//            X=29.66,Y=-23.49, LAT=43.825089,LON=-70.330030, 
//            SPD=2.00,HDG=119.06,YAW=119.05677,DEPTH=0.00,     
//            LENGTH=4.0,MODE=DRIVE

void BasicContactMgr::handleMailNodeReport(string report)
{
  NodeRecord new_node_record = string2NodeRecord(report, true);

  string group = tolower(new_node_record.getGroup());
  if(vectorContains(m_ignore_groups, group))
    return;

  // Part 1: Decide if we want to override X/Y with Lat/Lon based on 
  // user configuration and state of the node record.
  bool override_xy_with_latlon = true;
  if(m_contact_local_coords == "verbatim")
    override_xy_with_latlon = false;
  if(!m_use_geodesy)
    override_xy_with_latlon = false;
  if(m_contact_local_coords == "lazy_lat_lon") {
    if(new_node_record.isSetX() && new_node_record.isSetY())
      override_xy_with_latlon = false;
  }
  if(!new_node_record.isSetLatitude() || !new_node_record.isSetLongitude())
    override_xy_with_latlon = false;
 
  // Part 2: If we can override xy with latlon and configured to do so
  // then find the X/Y from MOOSGeodesy and Lat/Lon and replace.
  if(override_xy_with_latlon) {
    double nav_x, nav_y;
    double lat = new_node_record.getLat();
    double lon = new_node_record.getLon();
    
#ifdef USE_UTM
    m_geodesy.LatLong2LocalUTM(lat, lon, nav_y, nav_x);
#else
    m_geodesy.LatLong2LocalGrid(lat, lon, nav_y, nav_x);
#endif      
    new_node_record.setX(nav_x);
    new_node_record.setY(nav_y);
  }

  string vname = new_node_record.getName();

  // If incoming node name matches ownship, just ignore the node report
  // but don't return false which would indicate an error.
  if(vname == m_ownship)
    return;

  bool newly_known_vehicle = false;
  if(m_map_node_records.count(vname) == 0)
    newly_known_vehicle = true;
   
  // If we are (a) not currently tracking the given vehicle, and (b)
  // a reject_range is enabled, and (c) the contact is outside the
  // reject_range, then ignore this contact.
  if(newly_known_vehicle && (m_reject_range > 0)) {
    double cnx = new_node_record.getX();
    double cny = new_node_record.getY();
    double range = hypot(m_nav_x - cnx, m_nav_y -cny);
    if(range > m_reject_range)
      return;
  }
  
  //  if(!new_node_record.valid()) {
  //  Notify("CONTACT_MGR_WARNING", "Bad Node Report Received");
  //  reportRunWarning("Bad Node Report Received");
  //  return;
  //}
  
  m_map_node_records[vname] = new_node_record;
  
  if(newly_known_vehicle) {
    m_map_node_alerts_total[vname]    = 0;
    m_map_node_alerts_active[vname]   = 0;
    m_map_node_alerts_resolved[vname] = 0;

    m_map_node_ranges_actual[vname] = 0;
    m_map_node_ranges_extrap[vname] = 0;
    m_map_node_ranges_cpa[vname]    = 0;

    m_par.addVehicle(vname);

    if(m_alert_verbose) 
      Notify("ALERT_VERBOSE", "new_contact="+vname);
  }
  
  // Check if the contact had been on the retired list (due to age)
  // and if so, resurrect it and remove it from the retired list.
  list<string>::iterator p;
  for(p=m_contacts_retired.begin(); p!=m_contacts_retired.end(); ) {
    if(vname == *p)
      p = m_contacts_retired.erase(p);
    else
      ++p;
  }
}


//---------------------------------------------------------
// Procedure: handleMailResolved
//      Note: This MOOS process is registering for the variable
//            CONTACT_RESOLVED, which simply contains the string
//            value of a contact name and an alert id. 
//      Note: The notion of "resolved" means that this contact mgr
//            needs to post another alert when or if the alert
//            criteria is met again.

void BasicContactMgr::handleMailResolved(string str)
{
  string resolution = tolower(str);
  string vehicle = biteStringX(resolution, ',');
  string alertid = resolution;

  if(alertid == "")
    alertid = "all_alerts";

  reportEvent("TryResolve: (" + vehicle + "," + alertid + ")");

  // Error Check #1 - Make sure we know about the vehicle
  if(!m_par.containsVehicle(vehicle)) {
    string msg = "Resolution failed. Nothing known about vehicle: " + vehicle;
    Notify("CONTACT_MGR_WARNING", msg);
    reportRunWarning(msg);
    return;
  }
  
  // Error Check #2 - Make sure we know about the alert id
  if((alertid != "all_alerts") && !m_par.containsAlertID(alertid)) {
    string msg = "Resolution failed. Nothing known about alertid: " + alertid;
    Notify("CONTACT_MGR_WARNING", msg);
    reportRunWarning(msg);
    return;
  }
  
  m_map_node_alerts_active[vehicle]--;
  m_map_node_alerts_resolved[vehicle]++;
  
  m_par.setAlertedValue(vehicle, alertid, false);
  reportEvent("Resolved: (" + vehicle + "," + alertid + ")");
}

//---------------------------------------------------------
// Procedure: handleMailDisplayRadii

void BasicContactMgr::handleMailDisplayRadii(string value)
{
  bool ok = setBooleanOnString(m_display_radii, value);
  if(ok) {
    string msg = "DISPLAY_RADII set to " + boolToString(m_display_radii);
    reportEvent(msg);
  }
  else {
    string msg = "Failed attempt so set DISPLAY_RADII to: " + value;
    reportEvent(msg);
    reportConfigWarning(msg);
  }

  if(m_display_radii == false)
    postRadii(false);
}

//---------------------------------------------------------
// Procedure: handleMailReportRequest()
//   Example: BCM_REPORT_REQUEST = var=BCM_CONTACTS_85, range=85

void BasicContactMgr::handleMailReportRequest(string str)
{
  bool ok = true;
  
  string moos_var;
  double range = -1;
  string group;
  string vtype;
  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if((param == "var") && !strContainsWhite(value) && (value != ""))
      moos_var = value;
    else if((param == "range") && isNumber(value))
      range = atof(value.c_str());
    else if(param == "group")
      group = value;
    else if(param == "type")
      vtype = value;
    else
      ok = false;
  }

  if(!ok || (moos_var == "") || (range < 0)) {
    string msg = "Failed REPORT_REQUEST: " + str;
    reportEvent(msg);
    reportRunWarning(msg);
  }    
  else {
    string msg = "New REPORT_REQUEST: " + str;
    reportEvent(msg);
    // If this is a new request, make new map entry for all fields
    if(!m_map_rep_range.count(moos_var) ||
       m_map_rep_range[moos_var] != range) {
      m_map_rep_range[moos_var] = range;
      m_map_rep_group[moos_var] = group;
      m_map_rep_vtype[moos_var] = vtype;
      m_map_rep_contacts[moos_var] = "";
    }
    // If this is a repeat request (same moos_var and range) then
    // just update the current time.
    m_map_rep_reqtime[moos_var] = m_curr_time;
  }
}

//---------------------------------------------------------
// Procedure: handleMailAlertRequest
//    Format: BCM_ALERT_REQUEST = 
//            var=CONTACT_INFO, val="name=avd_$[VNAME] # contact=$[VNAME]"
//            alert_range=80, cpa_range=95

                     
void BasicContactMgr::handleMailAlertRequest(string value)
{
  m_alert_requests_received++;
  bool ok = handleConfigAlert(value);
  if(!ok)
    reportRunWarning("Unhandled Alert Request: " + value);   
}

//---------------------------------------------------------
// Procedure: handleConfigIgnoreGroup
//      Note: The ignore_group can also be set in the individual
//            alert registration, but this is applied at the
//            global level, right with a node report is received 


bool BasicContactMgr::handleConfigIgnoreGroup(string grp_str)
{
  // All group names are treated case insensitive
  grp_str = tolower(grp_str);
  
  if(vectorContains(m_ignore_groups, grp_str))
    return(false);

  m_ignore_groups.push_back(grp_str);
  return(true);
}

//---------------------------------------------------------
// Procedure: handleConfigAlert

bool BasicContactMgr::handleConfigAlert(string alert_str)
{
  // Part 1: Get the alert id. Allow for an "empty" alert,
  // but call it "no_id".
  string alert_id = tokStringParse(alert_str, "id", ',', '=');
  if(alert_id == "")
    alert_id = "no_id";

  // Added mikerb July 2019
  //if(m_par.containsAlertID(alert_id))
  //  return(false);
  

  m_par.addAlertID(alert_id);

  if(m_map_alerts.count(alert_id) == 0) {
    m_map_alerts[alert_id].setAlertRange(m_default_alert_rng);
    m_map_alerts[alert_id].setAlertRangeFar(m_default_alert_rng_cpa);
    m_map_alerts[alert_id].setAlertRangeColor(m_default_alert_rng_color);
    m_map_alerts[alert_id].setAlertRangeFarColor(m_default_alert_rng_cpa_color);
  }
  
  string var, pattern;
  vector<string> svector = parseStringQ(alert_str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string left  = tolower(biteStringX(svector[i], '='));
    string right = svector[i];
    double dval  = atof(right.c_str());

    if(isQuoted(right))
      right = stripQuotes(right);

    bool ok = false;
    
    if(left == "var") {
      var = right;
      ok = true;
    }
    else if((left == "val") || (left == "pattern")) {
      pattern = right;
      ok = true;
    }
    else if((left == "on_flag") || (left == "onflag")) 
      ok = m_map_alerts[alert_id].addAlertOnFlag(right);
    else if((left == "off_flag") || (left == "offflag"))
      ok = m_map_alerts[alert_id].addAlertOffFlag(right);
    else if(left == "region")
      ok = m_map_alerts[alert_id].setAlertRegion(right);
    else if((left == "alert_range") && isNumber(right))
      ok = m_map_alerts[alert_id].setAlertRange(dval);
    else if((left == "alert_range_color") && isColor(right))
      ok = m_map_alerts[alert_id].setAlertRangeColor(right);

    else if(left == "match_type")
      ok = m_map_alerts[alert_id].addMatchType(right);
    else if(left == "ignore_type")
      ok = m_map_alerts[alert_id].addIgnoreType(right);

    else if(left == "match_group")
      ok = m_map_alerts[alert_id].addMatchGroup(right);
    else if(left == "ignore_group")
      ok = m_map_alerts[alert_id].addIgnoreGroup(right);

    else if((left == "cpa_range") && isNumber(right))
      ok = m_map_alerts[alert_id].setAlertRangeFar(dval);
    else if((left == "cpa_range_color") && isColor(right))
      ok = m_map_alerts[alert_id].setAlertRangeFarColor(right);
    
    if(!ok && (left != "id")) {
      reportConfigWarning("unhandled alert config component: " + left);
      return(false);
    }
  }

  // For backward compatibility sake we allow the user to specify an on-flag
  // with separate var,pattern fields.  
  if((var != "") && (pattern != ""))
    m_map_alerts[alert_id].addAlertOnFlag(var + "=" + pattern);
  
  return(true);
}


//---------------------------------------------------------
// Procedure: postRangeReports()
//   Purpose: 

void BasicContactMgr::postRangeReports()
{
  // Part 1: Check timestamps for all report requests and see if they
  // should be retired and deleted from memory. Build a list of
  // reports to retire. Recall that requested customized range reports
  // must be periodically refreshed with a new request as a guard
  // against unbounded memory and cpu growth.
  vector<string> to_retire;
  map<string, double>::iterator p;
  for(p=m_map_rep_reqtime.begin(); p!=m_map_rep_reqtime.end(); p++) {
    string varname = p->first;
    double reqtime = p->second;

    double age = m_curr_time - reqtime;
    if(age > m_range_report_timeout)
      to_retire.push_back(varname);
  }

  // Part 2: Go through the list of reports to retire and actually
  // remove them from memory.
  for(unsigned int i=0; i<to_retire.size(); i++) {
    string varname = to_retire[i];
    m_map_rep_range.erase(varname);
    m_map_rep_reqtime.erase(varname);
    m_map_rep_contacts.erase(varname);
  }

  // Part 3: For each report (varname), figure out which contacts
  //         satisfy the range threshold for that report
  //         Also check contact group name if report specifies
  //         Also check contact vehicle type if report specifies
  for(p=m_map_rep_range.begin(); p!=m_map_rep_range.end(); p++) {
    string varname = p->first;
    double rthresh = p->second;

    // Part 3A: Get the list of contacts for this report
    string contacts;   
    vector<string> vcontacts;
    map<string, double>::iterator q;
    for(q=m_map_node_ranges_extrap.begin();
	q!=m_map_node_ranges_extrap.end(); q++) {
      string vname = q->first;

      // Part 3AA: If report specifies group, check contact for match
      bool group_match = true;
      if(m_map_rep_group[varname] != "") { 
	if(tolower(m_map_rep_group[varname]) !=
	   tolower(m_map_node_records[vname].getGroup()))
	  group_match = false;
      }

      // Part 3AB: If report specifies vtype, check contact for match
      bool vtype_match = true;
      if(m_map_rep_vtype[varname] != "") {
	if(tolower(m_map_rep_vtype[varname]) !=
	   tolower(m_map_node_records[vname].getType()))
	  vtype_match = false;
      }

      // Part 3AC: Check if the range is satisfied
      bool range_sat = false;
      double now_range = q->second;
      if(now_range < rthresh) 
	range_sat = true;

      if(group_match && vtype_match && range_sat) {
	if(contacts != "")
	  contacts += ",";
	contacts += vname;
	vcontacts.push_back(vname);
      }
    }
    // Part 3B: If the report is different post it!
    if(contacts != m_map_rep_contacts[varname]) {
      Notify(varname, contacts);
      m_map_rep_contacts[varname] = contacts;
    }
  }
}

//---------------------------------------------------------
// Procedure: postSummaries

void BasicContactMgr::postSummaries()
{
  // ==========================================================
  // Part 1: Handle the contacts_retired list
  // ==========================================================
  string contacts_retired;
  list<string>::iterator q;
  for(q=m_contacts_retired.begin(); q!=m_contacts_retired.end(); q++) {
    string this_contact = *q;
    if(contacts_retired != "")
      contacts_retired += ",";
    contacts_retired += this_contact;
  }

  if(m_prev_contacts_retired != contacts_retired) {
    Notify("CONTACTS_RETIRED", contacts_retired);
    m_prev_contacts_retired = contacts_retired;
  }

  // Limit the size of the list so this doesn't also grow unbounded.
  // Prune after posting so all newly retired get >=1  posting
  while(m_contacts_retired.size() > m_max_retired_hist)
    m_contacts_retired.pop_back();

  // ==========================================================
  // Part 2: Everything else
  // ==========================================================
  string contacts_list;
  string contacts_alerted;
  string contacts_unalerted;
  string contacts_recap;

  string closest_contact;
  double closest_range = 0;
  
  map<string, NodeRecord>::const_iterator p;
  for(p=m_map_node_records.begin(); p!= m_map_node_records.end(); p++) {
    string     contact_name = p->first;
    NodeRecord node_record  = p->second;

    if(contacts_list != "")
      contacts_list += ",";
    contacts_list += contact_name;

    double age   = m_curr_time - node_record.getTimeStamp();
    double range = m_map_node_ranges_actual[contact_name];

    // Update who is the closest contact and it's range
    if((closest_contact == "") || (range < closest_range)) {
      closest_contact = contact_name;
      closest_range   = range;
    }
	
    if(contacts_recap != "")
      contacts_recap += " # ";
    contacts_recap += "vname=" + contact_name;
    contacts_recap += ",range=" + doubleToString(range, 2);
    contacts_recap += ",age=" + doubleToString(age, 2);
  }

  if((closest_contact != "") && m_post_closest_range) {
    // Round to integer and only post when changed, to reduce postings
    long int closest_range_int = closest_range;
    closest_range = (double)(closest_range_int);
    if(closest_range != m_prev_closest_range) {
      Notify("CONTACT_CLOSEST_RANGE", closest_range);
      m_prev_closest_range = closest_range;
    }
  }
  if((m_prev_contact_closest != closest_contact) &&
     (closest_contact != "")){
    Notify("CONTACT_CLOSEST", closest_contact);
    Notify("CONTACT_CLOSEST_TIME", m_curr_time);
    m_prev_contact_closest = closest_contact;
  }

  
  unsigned int contacts_count = m_par.getAlertedGroupCount(true);
  if(contacts_count != m_prev_contacts_count) {
    Notify("CONTACTS_COUNT", (double)(contacts_count));
    m_prev_contacts_count = contacts_count;
  }

  if(m_prev_contacts_list != contacts_list) {
    Notify("CONTACTS_LIST", contacts_list);
    m_prev_contacts_list = contacts_list;
  }

  contacts_alerted = m_par.getAlertedGroup(true);
  if(m_prev_contacts_alerted != contacts_alerted) {
    Notify("CONTACTS_ALERTED", contacts_alerted);
    m_prev_contacts_alerted = contacts_alerted;
  }

  contacts_unalerted = m_par.getAlertedGroup(false);
  if(m_prev_contacts_unalerted != contacts_unalerted) {
    Notify("CONTACTS_UNALERTED", contacts_unalerted);
    m_prev_contacts_unalerted = contacts_unalerted;
  }

  double time_since_last_recap = m_curr_time - m_contacts_recap_posted;

  // recaps may be configured "off" and the interval would be -1.
  if((m_contacts_recap_interval > 0) &&
     (time_since_last_recap > m_contacts_recap_interval)) {
    m_contacts_recap_posted = m_curr_time;
    Notify("CONTACTS_RECAP", contacts_recap);
    m_prev_contacts_recap = contacts_recap;
  }
}


//---------------------------------------------------------
// Procedure: checkForAlerts
//   Purpose: Check each contact/alert pair and handle if the
//            alert condition changes.

void BasicContactMgr::checkForAlerts()
{
  //==============================================================
  // For each contact, check all alerts
  //==============================================================
  map<string, NodeRecord>::iterator p;
  for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
    string     contact = p->first;
    NodeRecord record  = p->second;
    string     cntype  = record.getType();
    
    //==============================================================
    // For each alert_id, check if alert should be posted for this contact
    //==============================================================
    map<string, CMAlert>::iterator q;
    for(q=m_map_alerts.begin(); q!=m_map_alerts.end(); q++) {
      string id = q->first;

      bool alert_applies = checkAlertApplies(contact, id);

      // If alert applies and currently not alerted, handle
      string transition;
      if(alert_applies && !m_par.getAlertedValue(contact, id)) {
        postOnAlerts(record, id);
	m_par.setAlertedValue(contact, id, true);
	transition = "off-->alerted";
	m_map_node_alerts_total[contact]++;
	m_map_node_alerts_active[contact]++;
      }
      else if(!alert_applies && m_par.getAlertedValue(contact, id)) {
        postOffAlerts(record, id);
	m_par.setAlertedValue(contact, id, false);
	transition = "alerted-->off";
      }
            
      if((m_alert_verbose) && (transition != "")) {
	string mvar = "ALERT_VERBOSE";
	string mval = "contact=" + contact;
	mval += ",alert_id=" + id;
	mval += "," + transition; 
	mval += ", alerted=";
	mval += boolToString(m_par.getAlertedValue(contact,id));
	//mval += ",alert_range=" + doubleToString(alert_range,1);
	//mval += ",alert_range_cpa=" + doubleToString(alert_range_cpa,1);
	
	//double range_actual = m_map_node_ranges_actual[contact];
	//double range_cpa = m_map_node_ranges_cpa[contact];
	//mval += ",range_actual=" + doubleToString(range_actual,1);	
	//mval += ",range_cpa=" + doubleToString(range_cpa,1);
	
	Notify(mvar, mval);
      }
    }
  }
}

//---------------------------------------------------------
// Procedure: checkForCloseInReports()
//   Purpose: Report a single number {0, 1, 2} indicating the severity of
//            range of the closest contact. The idea is to have a single
//            number to plot in a timeline as in alogview to help the user
//            jump to the interesting (miss or near-miss) points in time.
//
//    Report: 0 if closest contact is far away
//            1 if closest contact is near (< m_eval_range_far)
//            2 if closest contact is very near(< m_eval_range_near)
//
//   Example: m_eval_range_far = 12 (meters) near miss
//            m_eval_range_near = 5  (meters) collision
//      Note: Typically these two values will match what uFldCollisionDetect
//            regards as near misses and hits.

void BasicContactMgr::checkForCloseInReports()
{
  double report_val = 0;

  //==============================================================
  // For each contact, check all the absolute (no extrap) ranges
  //==============================================================
  map<string, NodeRecord>::iterator p;
  for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
    string    contact_name = p->first;
    NodeRecord node_record = p->second;

    // Skip this contact if age of node record exceeds max age
    double age = m_curr_time - node_record.getTimeStamp();
    if(age > m_contact_max_age)
      continue;

    double contact_range_abs = m_map_node_ranges_actual[contact_name]; 

    double this_report_val = 0;
    if(contact_range_abs < m_eval_range_far)
      this_report_val = 1;
    else if(contact_range_abs < m_eval_range_near)
      this_report_val = 2;

    // Overall report_val is max of the report_vals for each contact.
    if(this_report_val > report_val)
      report_val = this_report_val;
  }    
    
  if(report_val != m_prev_closest_contact_val) { 
    Notify("CONTACT_CLOSEST_EVAL", report_val);
    m_prev_closest_contact_val = report_val;
  }
}

//---------------------------------------------------------
// Procedure: checkForNewRetiredContacts()
//   Purpose: Check all contacts and see if any of them should be
//            moved onto the retired list based on contact_max_age.

void BasicContactMgr::checkForNewRetiredContacts()
{
  //==============================================================
  // Part 1: Find new contacts that need to be retired.
  //==============================================================
  list<string> newly_retired;
  map<string, NodeRecord>::iterator p;
  for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
    string     contact  = p->first;
    NodeRecord record   = p->second;

    // Possibly drop due to age
    double age = m_curr_time - record.getTimeStamp();
    if(age > m_contact_max_age) 
      newly_retired.push_front(contact);

    // Possibly drop due to reject_range. Note the range thresh has
    // 10 meters added, to help prevent thrashing.
    else if(m_map_node_ranges_actual.count(contact) &&
	    (m_reject_range > 0) &&
	    (m_map_node_ranges_actual[contact] > m_reject_range+10)) {
      newly_retired.push_front(contact);
    }
  }
  if(newly_retired.size() == 0)
    return;
  
  //==============================================================
  // Part 2: Go through the list of newly retired contacts and do
  //         (a) free up any memory associated with this contact
  //         (b) Merge newly retired contacts onto the front of 
  //         the list of previously retired contacts, since we
  //         always delete retired contacts from back of this list.
  //==============================================================
  list<string>::iterator q;
  for(q=newly_retired.begin(); q!=newly_retired.end(); q++) {
    string contact = *q;
    // (a) Free up any memory associated with this contact
    m_map_node_records.erase(contact);
    m_map_node_alerts_total.erase(contact);
    m_map_node_alerts_active.erase(contact);
    m_map_node_alerts_resolved.erase(contact);
    m_map_node_ranges_actual.erase(contact);
    m_map_node_ranges_extrap.erase(contact);
    m_map_node_ranges_cpa.erase(contact);
    m_par.removeVehicle(contact);

    // (b) Add to front of list of retired contacts
    m_contacts_retired.push_front(contact);
  }
}

//----------------------------------------------------------------
// Procedure: postOnAlerts

void BasicContactMgr::postOnAlerts(NodeRecord record, string id)
{
  if(!knownAlert(id))
    return;

  vector<VarDataPair> pairs = getAlertOnFlags(id);
  for(unsigned int i=0; i<pairs.size(); i++)
    postAlert(record, pairs[i]);
  
}

//----------------------------------------------------------------
// Procedure: postOffAlerts

void BasicContactMgr::postOffAlerts(NodeRecord record, string id)
{
  if(!knownAlert(id))
    return;

  vector<VarDataPair> pairs = getAlertOffFlags(id);
  for(unsigned int i=0; i<pairs.size(); i++)
    postAlert(record, pairs[i]);
  
}

//----------------------------------------------------------------
// Procedure: postAlert

void BasicContactMgr::postAlert(NodeRecord record, VarDataPair pair)
{
  // Step 1: Get all potential macro info
  string x_str    = record.getStringValue("x");
  string y_str    = record.getStringValue("y");
  string lat_str  = record.getStringValue("lat");
  string lon_str  = record.getStringValue("lon");
  string spd_str  = record.getStringValue("speed");
  string hdg_str  = record.getStringValue("heading");
  string dep_str  = record.getStringValue("depth");
  string time_str = record.getStringValue("time");
  string name_str = record.getName();
  string type_str = record.getType();

  // Step 2: Get var to post, and expand macros if any
  string var = pair.get_var();

  var = findReplace(var, "$[X]", x_str);
  var = findReplace(var, "$[Y]", y_str);
  var = findReplace(var, "$[LAT]", lat_str);
  var = findReplace(var, "$[LON]", lon_str);
  var = findReplace(var, "$[SPD}", spd_str);
  var = findReplace(var, "$[HDG]", hdg_str);
  var = findReplace(var, "$[DEP]", dep_str);
  var = findReplace(var, "$[VNAME]", name_str);
  var = findReplace(var, "$[VTYPE]", type_str);
  var = findReplace(var, "$[UTIME]", time_str);
  var = findReplace(var, "%[VNAME]", tolower(name_str));
  var = findReplace(var, "%[VTYPE]", tolower(type_str));

  // Step 3: If posting is numerical just do it and be done!
  if(!pair.is_string()) {
    double dval = pair.get_ddata();
    Notify(var, dval);
    reportEvent(var + "=" + doubleToStringX(dval, 3));
    return;
  }

  // Step 4: Otherwise handle a string posting with macros
  
  string msg = pair.get_sdata();
  
  msg = findReplace(msg, "$[X]", x_str);
  msg = findReplace(msg, "$[Y]", y_str);
  msg = findReplace(msg, "$[LAT]", lat_str);
  msg = findReplace(msg, "$[LON]", lon_str);
  msg = findReplace(msg, "$[SPD}", spd_str);
  msg = findReplace(msg, "$[HDG]", hdg_str);
  msg = findReplace(msg, "$[DEP]", dep_str);
  msg = findReplace(msg, "$[VNAME]", name_str);
  msg = findReplace(msg, "$[VTYPE]", type_str);
  msg = findReplace(msg, "$[UTIME]", time_str);
  msg = findReplace(msg, "%[VNAME]", tolower(name_str));
  msg = findReplace(msg, "%[VTYPE]", tolower(type_str));

  Notify(var, msg);
  reportEvent(var + "=" + msg);
}

//---------------------------------------------------------
// Procedure: updateRanges

void BasicContactMgr::updateRanges()
{
  double alert_range_cpa_time = 36000; // 10 hours

  map<string, NodeRecord>::iterator p;
  for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
    string     vname = p->first;
    NodeRecord node_record = p->second;

    // First figure out the raw range to the contact
    double cnx = node_record.getX();
    double cny = node_record.getY();
    double cnh = node_record.getHeading();
    double cns = node_record.getSpeed();
    double cnt = node_record.getTimeStamp();

    // #1 Determine and store the actual point-to-point range between
    // ownship and the last absolute known position of the contact
    double range_actual = hypot((m_nav_x - cnx), (m_nav_y - cny));
    m_map_node_ranges_actual[vname] = range_actual;

    // #2 Determine and store the extrapolated range between ownship
    // and the contact position determined by its last known range and
    // extrapolation.
    LinearExtrapolator linex;
    linex.setDecay(m_decay_start, m_decay_end);
    linex.setPosition(cnx, cny, cns, cnh, cnt);

    double extrap_x = cnx;
    double extrap_y = cny;
    double range_extrap = range_actual;

    bool ok = linex.getPosition(extrap_x, extrap_y, m_curr_time);
    if(ok) {
      cnx = extrap_x;
      cny = extrap_y;
      range_extrap = hypot((m_nav_x - cnx), (m_nav_y - cny));
    }
    m_map_node_ranges_extrap[vname] = range_extrap;

    // #3 Determine and store the cpa range between ownship and the
    // contact position determined by the contact's extrapolated position
    // and it's last known heading and speed.
    CPAEngine engine(cny, cnx, cnh, cns, m_nav_y, m_nav_x);      
    double range_cpa = engine.evalCPA(m_nav_hdg, m_nav_spd,
				      alert_range_cpa_time);
    m_map_node_ranges_cpa[vname] = range_cpa;
  }
}

//---------------------------------------------------------
// Procedure: postRadii

void BasicContactMgr::postRadii(bool active)
{
  map<string, CMAlert>::const_iterator p;
  for(p=m_map_alerts.begin(); p!=m_map_alerts.end(); p++) {
    string alert_id = p->first;
    double alert_range = getAlertRange(alert_id);
    string alert_range_color = getAlertRangeColor(alert_id);

    if(!active || (alert_range_color == "invisible") ||
       (alert_range_color == "empty") || (alert_range_color == ""))
      active = false;
    
    XYCircle circle(m_nav_x, m_nav_y, alert_range);
    circle.set_label(alert_id + "_in");
    circle.set_color("edge", alert_range_color);
    circle.set_vertex_size(0);
    circle.set_edge_size(1);
    circle.set_active(active);
    string s1 = circle.get_spec();
    Notify("VIEW_CIRCLE", s1);

    double alert_range_cpa = getAlertRangeCPA(alert_id);
    if(alert_range_cpa > alert_range) {

      string alert_range_cpa_color = getAlertRangeCPAColor(alert_id);

      if(!active || (alert_range_cpa_color == "invisible") ||
	 (alert_range_cpa_color == "empty") ||
	 (alert_range_cpa_color == ""))
	active = false;

      XYCircle circ(m_nav_x, m_nav_y, alert_range_cpa);
      circ.set_label(alert_id + "_out");
      circ.set_color("edge", alert_range_cpa_color);
      circ.set_vertex_size(0);
      circ.set_edge_size(1);
      circ.set_active(active);
      string s2 = circ.get_spec();

      Notify("VIEW_CIRCLE", s2);
    }
  }
}

//---------------------------------------------------------
// Procedure: checkAlertApplies()

bool BasicContactMgr::checkAlertApplies(string contact, string id) 
{
  //=========================================================
  // Part 1: Sanity checks
  //=========================================================

  // Return false immediately if alert or contact are unknown
  if(!knownAlert(id))
    return(false);

  if(m_map_node_records.count(contact) == 0)
    return(false);

  // Return false immediately if age of node record exceeds max age
  NodeRecord record = m_map_node_records.at(contact);
  double age = m_curr_time - record.getTimeStamp();
  if(age > m_contact_max_age)
    return(false);


  //=========================================================
  // Part 2: Check if the contact has a known type that is
  //         either excluded or mandated by alert spec
  //=========================================================

  string cntype = record.getType();
  if(cntype != "") {
    vector<string> match_types = getAlertMatchTypes(id);
    // If this alert has any match types, must match at least one
    if(match_types.size() != 0) {
      if(!vectorContains(match_types, cntype))
	return(false);
    }
    vector<string> ignore_types = getAlertIgnoreTypes(id);
    // If this alert has any ignore types, must not match any
    if(ignore_types.size() != 0) {
      if(vectorContains(ignore_types, cntype))
	return(false);
    }
  }

  //=========================================================
  // Part 3: Check if the contact has a known group that is
  //         either excluded or mandated by alert spec
  //=========================================================

  string cngroup = record.getGroup();
  if(cngroup != "") {
    vector<string> match_groups = getAlertMatchGroups(id);
    // If this alert has any match groups, must match at least one
    if(match_groups.size() != 0) {
      if(!vectorContains(match_groups, cngroup))
	return(false);
    }
    vector<string> ignore_groups = getAlertIgnoreGroups(id);
    // If this alert has any ignore groups, must not match any
    if(ignore_groups.size() != 0) {
      if(vectorContains(ignore_groups, cngroup))
	return(false);
    }
  }

  //=========================================================
  // Part 4: Checks based on range of ownship to contact
  //=========================================================

  double alert_range = getAlertRange(id);
  double alert_range_cpa = getAlertRangeCPA(id);

  // If alert range is zero, this is regarded as having the range
  // criteria OFF. Likely this alert depends only on the region.

  if(alert_range > 0) {
    double contact_range_abs = m_map_node_ranges_actual[contact]; 
    double contact_range_cpa = m_map_node_ranges_cpa[contact];    
    
    if(contact_range_abs > alert_range_cpa)
      return(false);
    
    if((contact_range_abs > alert_range) && (contact_range_cpa > alert_range))
      return(false);
  }
  
  //=========================================================
  // Part 5: Checks based on Alert Region
  //=========================================================
  
  // Skip if this alertid has valid convex region and contact not in region
  if(hasAlertRegion(id)) {
    double cnx = record.getX();
    double cny = record.getY();
    XYPolygon region = getAlertRegion(id);
    if(region.is_convex() && !region.contains(cnx, cny))
      return(false);
  }

  // If none of the above no-apply conditions hold, return true!
  return(true);  
}

//---------------------------------------------------------
// Procedure: knownAlert()

bool BasicContactMgr::knownAlert(string alert_id) const
{
  map<string, CMAlert>::const_iterator p;
  p = m_map_alerts.find(alert_id);
  if(p == m_map_alerts.end())
    return(false);
  return(true);
}

//---------------------------------------------------------
// Procedure: getAlertRange
//      Note: Use this method to access map, not map directly. This 
//            allows intelligent defaults to be applied if missing key.

double BasicContactMgr::getAlertRange(string alert_id) const
{
  if(!knownAlert(alert_id))
    return(m_default_alert_rng);
  return(m_map_alerts.at(alert_id).getAlertRange());
}

//---------------------------------------------------------
// Procedure: getAlertRangeCPA
//      Note: Use this method to access map, not map directly. This 
//            allows intelligent defaults to be applied if missing key.

double BasicContactMgr::getAlertRangeCPA(string alert_id) const
{
  if(!knownAlert(alert_id))
    return(m_default_alert_rng_cpa);
  return(m_map_alerts.at(alert_id).getAlertRangeFar());
}

//---------------------------------------------------------
// Procedure: getAlertRangeColor
//      Note: Use this method to access map, not map directly. This 
//            allows intelligent defaults to be applied if missing key.

string BasicContactMgr::getAlertRangeColor(string alert_id) const
{
  if(!knownAlert(alert_id))
    return(m_default_alert_rng_color);
  return(m_map_alerts.at(alert_id).getAlertRangeColor());
}

//---------------------------------------------------------
// Procedure: getAlertRangeCPAColor
//      Note: Use this method to access map, not map directly. This 
//            allows intelligent defaults to be applied if missing key.

string BasicContactMgr::getAlertRangeCPAColor(string alert_id) const
{
  if(!knownAlert(alert_id))
    return(m_default_alert_rng_cpa_color);
  return(m_map_alerts.at(alert_id).getAlertRangeFarColor());
}


//---------------------------------------------------------
// Procedure: hasAlertRegion
//      Note: Use this method to access map, not map directly. This 
//            allows intelligent defaults to be applied if missing key.

bool BasicContactMgr::hasAlertRegion(string alert_id) const
{
  if(!knownAlert(alert_id)) {
    return(false);
  }
  return(m_map_alerts.at(alert_id).hasAlertRegion());
}

//---------------------------------------------------------
// Procedure: getAlertRegion
//      Note: Use this method to access map, not map directly. This 
//            allows intelligent defaults to be applied if missing key.

XYPolygon BasicContactMgr::getAlertRegion(string alert_id) const
{
  if(!knownAlert(alert_id)) {
    XYPolygon null_poly;
    return(null_poly);
  }
  return(m_map_alerts.at(alert_id).getAlertRegion());
}

//---------------------------------------------------------
// Procedure: getAlertMatchTypes()

vector<string> BasicContactMgr::getAlertMatchTypes(string alert_id) const
{
  vector<string> rvector;
  if(!knownAlert(alert_id)) 
    return(rvector);

  return(m_map_alerts.at(alert_id).getMatchTypes());
}

//---------------------------------------------------------
// Procedure: getAlertIgnoreTypes()

vector<string> BasicContactMgr::getAlertIgnoreTypes(string alert_id) const
{
  vector<string> rvector;
  if(!knownAlert(alert_id)) 
    return(rvector);

  return(m_map_alerts.at(alert_id).getIgnoreTypes());
}

//---------------------------------------------------------
// Procedure: getAlertMatchGroups()

vector<string> BasicContactMgr::getAlertMatchGroups(string alert_id) const
{
  vector<string> rvector;
  if(!knownAlert(alert_id)) 
    return(rvector);

  return(m_map_alerts.at(alert_id).getMatchGroups());
}

//---------------------------------------------------------
// Procedure: getAlertIgnoreGroups()

vector<string> BasicContactMgr::getAlertIgnoreGroups(string alert_id) const
{
  vector<string> rvector;
  if(!knownAlert(alert_id)) 
    return(rvector);

  return(m_map_alerts.at(alert_id).getIgnoreGroups());
}

//---------------------------------------------------------
// Procedure: hasAlertOnFlag

bool BasicContactMgr::hasAlertOnFlag(string alert_id) const
{
  if(!knownAlert(alert_id))
    return(false);
  return(m_map_alerts.at(alert_id).hasAlertOnFlag());
}

//---------------------------------------------------------
// Procedure: hasAlertOffFlag

bool BasicContactMgr::hasAlertOffFlag(string alert_id) const
{
  if(!knownAlert(alert_id))
    return(false);
  return(m_map_alerts.at(alert_id).hasAlertOffFlag());
}

//---------------------------------------------------------
// Procedure: getAlertOnFlags

vector<VarDataPair> BasicContactMgr::getAlertOnFlags(string alert_id) const
{
  vector<VarDataPair> empty;
  if(!knownAlert(alert_id))
    return(empty);
  return(m_map_alerts.at(alert_id).getAlertOnFlags());
}

//---------------------------------------------------------
// Procedure: getAlertOffFlags

vector<VarDataPair> BasicContactMgr::getAlertOffFlags(string alert_id) const
{
  vector<VarDataPair> empty;
  if(!knownAlert(alert_id))
    return(empty);
  return(m_map_alerts.at(alert_id).getAlertOffFlags());
}

//---------------------------------------------------------
// Procedure: buildReport
//      Note: A virtual function of the AppCastingMOOSApp superclass, 
//            conditionally invoked if either a terminal or appcast 
//            report is needed.
//
// Alert Configurations (2):
// ---------------------
// Alert ID = avd
//   VARNAME   = CONTACT_INFO
//   PATTERN   = name=$[VNAME] # contact=$[VNAME]
//   RANGE     = 1000, lightblue
//   CPA_RANGE = 1200, green
//
// Alert ID = trail
//   VARNAME   = TRAIL_INFO
//   PATTERN   = name=trail_$[VNAME] # contact=$[VNAME]
//   RANGE     = 600, white
//   CPA_RANGE = 750, grey45
//
// Alert Status Summary
// ----------------------
//        List: henry
//     Alerted: 
//   UnAlerted: (henry,avd)
//     Retired: 
//       Recap: vname=henry,range=105.85,age=1.75
//  
// Contact Status Summary:
// ---------------------- 
//   Contact   Range    Alerts   Alerts  Alerts    Alerts
//                      Total    Active  Resolved  Active
//   -------   -----    -------  ------  --------  ------
//   gilda     188.2    4        3       1         (a,b)
//   henry     19.0     0        0       0         
//   ike       65.9     23       12      11        (a)
// 
//
// Events (Last 5):
// ---------------------
// 203.1  CONTACT_INFO = name=gilda # contact=gilda
// 192.0  CONTACT_RESOLVED = contact=gilda, alert=CONTACT_INFO 
// 143.2  CONTACT_INFO = name=gilda # contact=gilda
// 111.9  CONTACT_RESOLVED = contact=gilda, alert=CONTACT_INFO 
//  43.2  CONTACT_INFO = name=gilda # contact=gilda

bool BasicContactMgr::buildReport()
{
  //=================================================================
  // Part 1: Header Content
  //=================================================================
  string alert_count = uintToString(m_map_alerts.size());
  string bcm_req_received = uintToString(m_alert_requests_received);
  string max_age = doubleToStringX(m_contact_max_age,2);
  string reject_range = "off";
  if(m_reject_range > 0)
    reject_range = doubleToStringX(m_reject_range,2);
  m_msgs << "DisplayRadii:              " << boolToString(m_display_radii) << endl;
  m_msgs << "Deriving X/Y from Lat/Lon: " << boolToString(m_use_geodesy)   << endl;
  m_msgs << "Contact Max Age: " << max_age << endl;
  m_msgs << "Reject Range: " << reject_range << endl;
  m_msgs << "BCM_ALERT_REQUEST count: " << bcm_req_received << endl << endl; 
  

  //=================================================================
  // Part 2: Alert Configurations
  //=================================================================
  m_msgs << "Alert Configurations (" << alert_count << "):"  << endl;
  m_msgs << "---------------------" << endl;
  map<string, CMAlert>::iterator p;
  for(p=m_map_alerts.begin(); p!=m_map_alerts.end(); p++) {
    string alert_id = p->first;
    string alert_rng     = doubleToStringX(getAlertRange(alert_id));
    string alert_rng_cpa = doubleToStringX(getAlertRangeCPA(alert_id));
    string alert_rng_color  = getAlertRangeColor(alert_id);
    string alert_rng_cpa_color = getAlertRangeCPAColor(alert_id);
    string alert_region  = "n/a";
    XYPolygon region = getAlertRegion(alert_id);
    if(region.is_convex())
      alert_region = region.get_spec();
    m_msgs << "Alert ID = " << alert_id         << endl;
    m_msgs << "  RANGE     = " << alert_rng     << ", " << alert_rng_color  << endl;
    m_msgs << "  CPA_RANGE = " << alert_rng_cpa << ", " << alert_rng_cpa_color << endl;
    m_msgs << "  ALERT_REG = " << alert_region << endl;
    vector<VarDataPair> pairs = getAlertOnFlags(alert_id);
    for(unsigned int i=0; i<pairs.size(); i++) {
      VarDataPair pair = pairs[i];
      string var = pair.get_var();
      string val = pair.get_sdata();
      if(!pair.is_string())
	val = doubleToStringX(pair.get_ddata(), 3);
      m_msgs << "  ON_FLAG = " << var << "=" << val << endl;
    }

    vector<VarDataPair> xpairs = getAlertOffFlags(alert_id);
    for(unsigned int i=0; i<xpairs.size(); i++) {
      VarDataPair pair = xpairs[i];
      string var = pair.get_var();
      string val = pair.get_sdata();
      if(!pair.is_string())
	val = doubleToStringX(pair.get_ddata(), 3);
      m_msgs << "  OFF_FLAG = " << var << "=" << val << endl;
    }
  }
  m_msgs << endl;
  //=================================================================
  // Part 3: Alert Status Summary
  //=================================================================
  m_msgs << "Alert Status Summary: " << endl;
  m_msgs << "----------------------" << endl;
  m_msgs << "       List: " << m_prev_contacts_list         << endl;
  m_msgs << "    Alerted: " << m_prev_contacts_alerted      << endl;
  m_msgs << "  UnAlerted: " << m_prev_contacts_unalerted    << endl;
  m_msgs << "    Retired: " << m_prev_contacts_retired      << endl;
  m_msgs << "      Recap: " << m_prev_contacts_recap        << endl;
  m_msgs << endl;

  //=================================================================
  // Part 4: Contact Status Summary
  //=================================================================
  m_msgs << "Contact Status Summary:" << endl;
  m_msgs << "-----------------------" << endl;

  ACTable actab(5,5);
  actab.setColumnJustify(1, "right");
  actab << "Contact | Range  | Alerts  | Alerts | Alerts   ";
  actab << "        | Actual | Total   | Active | Resolved ";
  actab.addHeaderLines();

  map<string, NodeRecord>::iterator q;
  for(q=m_map_node_records.begin(); q!=m_map_node_records.end(); q++) {
    string vname = q->first;
    string range = doubleToString(m_map_node_ranges_actual[vname], 1);
    string alerts_total  = uintToString(m_map_node_alerts_total[vname]);
    string alerts_active = uintToString(m_map_node_alerts_active[vname]);
    string alerts_resolved = uintToString(m_map_node_alerts_resolved[vname]);
    actab << vname << range << alerts_total << alerts_active << alerts_resolved;
  }
  m_msgs << actab.getFormattedString();

  //=================================================================
  // Part 5: Custom Contact Reports
  //=================================================================
  m_msgs << endl << endl;
  m_msgs << "Custom Contact Reports:" << endl;
  m_msgs << "-----------------------" << endl;

  ACTable actab2(5,5);
  actab2.setColumnJustify(1, "right");
  actab2 << "VarName | Range  | Group  | VType | Contacts   ";
  actab2.addHeaderLines();

  for(map<string,double>::iterator p=m_map_rep_range.begin();
      p!=m_map_rep_range.end(); p++) {
    string varname = p->first;
    string range = doubleToStringX(p->second,2);
    string group = m_map_rep_group[varname];
    string vtype = m_map_rep_vtype[varname];
    vector<string> svector = parseString(m_map_rep_contacts[varname], ',');
    string contacts = uintToString(svector.size());   
    actab2 << varname << range << group << vtype << contacts;
  }
  m_msgs << actab2.getFormattedString();
  
  return(true);
}

