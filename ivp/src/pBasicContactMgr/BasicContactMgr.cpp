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
  // State Variables
  m_nav_x   = 0;
  m_nav_y   = 0;
  m_nav_hdg = 0;
  m_nav_spd = 0;

  m_prev_contacts_count = 0;

  // Configuration Variables
  m_default_alert_rng           = 1000;
  m_default_alert_rng_cpa       = 1000;
  m_default_alert_rng_color     = "gray65";
  m_default_alert_rng_cpa_color = "gray35";

  m_display_radii      = false;
  m_contact_max_age    = 600;         // units in seconds 600 = 10 mins

  m_contact_local_coords = "verbatim"; // Or lazy_lat_lon, or force_lat_lon
  m_alert_verbose = false;
  m_decay_start = 15;
  m_decay_end   = 30;

  m_closest_contact_rng_one = 20;
  m_closest_contact_rng_two = 10;

  m_use_geodesy = false;

  m_contacts_recap_interval = 0;
  m_contacts_recap_posted = 0;

  m_prev_closest_range = 0;
  m_post_closest_range = false;

  m_prev_closest_contact_val = 0;

  //---------------------------------------------
  m_contact_max_age_hist = 60;  // units in seconds 60 = 1 min

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
  postSummaries();
  checkForAlerts();
  checkForCloseInReports();
  clearOldRetiredContacts();
  
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


    else if(param == "display_radii") {
      bool ok = setBooleanOnString(m_display_radii, value);
      if(!ok)
	reportConfigWarning("display_radii must be a Boolean");
    }
    else if(param == "post_closest_range") {
      bool ok = setBooleanOnString(m_post_closest_range, value);
      if(!ok) {
	string msg = "post_closest_range must be a Boolean";
	reportConfigWarning(msg);
      }
    }
    else if(param == "contact_local_coords") {
      string lval = tolower(value);
      if((lval== "verbatim") || (lval=="lazy_lat_lon") || (lval=="force_lat_lon"))
	m_contact_local_coords = lval;
      else
	reportConfigWarning("Illegal contact_local_coords configuration");
    }
    else if((param == "alert_range") || (param == "default_alert_range")) {
      if(dval > 0)
	m_default_alert_rng = dval;
      else
	reportConfigWarning("alert_range must be > zero: " + value);
    }
    
    else if((param=="alert_cpa_range") || (param=="default_cpa_range")) {
      if(dval > 0)
	m_default_alert_rng_cpa = dval;
      else
	reportConfigWarning("default_alert_range_cpa must be > zero: " + value);
    }
    
    else if(param == "contacts_recap_interval") {
      if(dval >= 0)
	m_contacts_recap_interval = dval;
      else
	reportConfigWarning("contacts_recap_interval must be >= zero: " + value);
    }
    
    else if(param=="alert_cpa_time") {
      reportConfigWarning("alert_cpa_time parameter has been deprecated.");
    }
    
    else if(param=="alert_verbose") {
      bool ok = setBooleanOnString(m_alert_verbose, value);
      if(!ok)
	reportConfigWarning("alert_verbose must be either true or false");
    }
    
    else if(param == "default_alert_range_color") {
      if(isColor(value))
	m_default_alert_rng_color = value;
      else
	reportConfigWarning("default_alert_range_color, " + value +", not a color");
    }
    
    else if(param == "default_cpa_range_color") {
      if(isColor(value))
	m_default_alert_rng_cpa_color = value;
      else
	reportConfigWarning("default_alert_range_cpa_color, " +value+", not a color");
    }
    
    else if(param == "contact_max_age") {
      if(dval > 0)
	m_contact_max_age = dval;
      else
	reportConfigWarning("contact_max_age must be > zero: " + value);
    }
    
    else if(param == "contact_max_age_history") {
      if((dval >= 0) && (dval <= 600))
	m_contact_max_age_hist = dval;
      else {
	string msg = "contact_max_age_history must be [0,600] secs: ";
	msg += value;
	reportConfigWarning(msg);
      }
    }
    
    else if(param == "closest_contact_rng_one") {
      if(dval >= 0)
	m_closest_contact_rng_one = dval;
      else
	reportConfigWarning("closest_contact_rng_one must be >= zero: " + value);
    }
    
    else if(param == "closest_contact_rng_two") {
      if(dval >= 0)
	m_closest_contact_rng_two = dval;
      else
	reportConfigWarning("closest_contact_rng_two must be >= zero: " + value);
    }
    
    else 
      reportUnhandledConfigWarning(orig);
  }

  // Part 2: If we may possibly want to set our incoming X/Y report values based
  // on Lat/Lon values, then we must check for and initialized the MOOSGeodesy.
  if(m_contact_local_coords != "verbatim") {
    // look for latitude, longitude global variables
    double lat_origin, lon_origin;
    if(!m_MissionReader.GetValue("LatOrigin", lat_origin)) {
      reportConfigWarning("No LatOrigin in *.moos file");
      reportConfigWarning("Will not derive x/y from lat/lon in node reports.");
    }
    else if (!m_MissionReader.GetValue("LongOrigin", lon_origin)) {
      reportConfigWarning("No LongOrigin set in *.moos file");
      reportConfigWarning("Will not derive x/y from lat/lon in node reports.");
    }
    else if(!m_geodesy.Initialise(lat_origin, lon_origin)) {
      reportConfigWarning("Lat/Lon Origin found but Geodesy init failed.");
      reportConfigWarning("Will not derive x/y from lat/lon in node reports.");
    }
    else {
      m_use_geodesy = true;
      reportEvent("Geodesy init ok: will derive x/y from lat/lon in node reports.");
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
  
  //  if(!new_node_record.valid()) {
  //  Notify("CONTACT_MGR_WARNING", "Bad Node Report Received");
  //  reportRunWarning("Bad Node Report Received");
  //  return;
  //}
  
  bool newly_known_vehicle = false;
  if(m_map_node_records.count(vname) == 0)
    newly_known_vehicle = true;
  
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
// Procedure: handleMailAlertRequest
//    Format: BCM_ALERT_REQUEST = 
//            var=CONTACT_INFO, val="name=avd_$[VNAME] # contact=$[VNAME]"
//            alert_range=80, cpa_range=95

                     
void BasicContactMgr::handleMailAlertRequest(string value)
{
  bool ok = handleConfigAlert(value);
  if(!ok)
    reportRunWarning("Unhandled Alert Request: " + value);   
}

//---------------------------------------------------------
// Procedure: handleConfigAlert

bool BasicContactMgr::handleConfigAlert(string alert_str)
{
  // Part 1: Get the alert id. Allow for an "empty" alert, but call it "no_id".
  string alert_id = tokStringParse(alert_str, "id", ',', '=');
  if(alert_id == "")
    alert_id = "no_id";
  m_par.addAlertID(alert_id);

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
    else if((left == "on_flag")) 
      ok = m_map_alerts[alert_id].addAlertOnFlag(right);
    else if((left == "off_flag"))
      ok = m_map_alerts[alert_id].addAlertOffFlag(right);
    else if(left == "region")
      ok = m_map_alerts[alert_id].setAlertRegion(right);
    else if((left == "alert_range") && isNumber(right))
      ok = m_map_alerts[alert_id].setAlertRange(dval);
    else if((left == "alert_range_color") && isColor(right))
      ok = m_map_alerts[alert_id].setAlertRangeColor(right);
    else if((left == "cpa_range") && isNumber(right))
      ok = m_map_alerts[alert_id].setAlertRangeFar(dval);
    else if((left == "cpa_range_color") && isColor(right))
      ok = m_map_alerts[alert_id].setAlertRangeFarColor(right);
    
    if(!ok && (left != "id")) {
      reportConfigWarning("unhandled alert config component: " + left);
      return(false);
    }
  }

  // For backward compatibility sake we allow the user to specify an
  // on-flag with separate var,pattern fields.
  if((var != "") && (pattern != ""))
    m_map_alerts[alert_id].addAlertOnFlag(var + "=" + pattern);
  
  return(true);
}


//---------------------------------------------------------
// Procedure: postSummaries

void BasicContactMgr::postSummaries()
{
  string contacts_list;
  string contacts_retired;
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

    double age = m_curr_time - node_record.getTimeStamp();

    // If retired
    if(age > m_contact_max_age) {
      if(contacts_retired != "")
	contacts_retired += ",";
      contacts_retired += contact_name;
    }    
    else { // Else if not retired
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
  }

  if((closest_contact != "") && m_post_closest_range) {
    // Round to integer and only post when changed, to reduce postings
    long int closest_range_int = closest_range;
    closest_range = (double)(closest_range_int);
    if(closest_range != m_prev_closest_range) {
      Notify("CLOSEST_RANGE", closest_range);
      m_prev_closest_range = closest_range;
    }
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

  if((m_prev_contact_closest != closest_contact) && (closest_contact != "")){
    Notify("CONTACT_CLOSEST", closest_contact);
    Notify("CONTACT_CLOSEST_TIME", m_curr_time);
    m_prev_contact_closest = closest_contact;
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

  if(m_prev_contacts_retired != contacts_retired) {
    Notify("CONTACTS_RETIRED", contacts_retired);
    m_prev_contacts_retired = contacts_retired;
  }

  double time_since_last_recap = m_curr_time - m_contacts_recap_posted;

  if(time_since_last_recap > m_contacts_recap_interval) {
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
    string     contact  = p->first;
    NodeRecord record   = p->second;

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
	mval += ", alerted=" + boolToString(m_par.getAlertedValue(contact,id));
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
//            1 if closest contact is near (< m_closest_contact_rng_one)
//            2 if closest contact is very near(< m_closest_contact_rng_two)
//
//   Example: m_closest_contact_rng_one = 12 (meters) near miss
//            m_closest_contact_rng_two = 5  (meters) collision
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
    if(contact_range_abs < m_closest_contact_rng_one)
      this_report_val = 1;
    else if(contact_range_abs < m_closest_contact_rng_two)
      this_report_val = 2;

    // Overall report_val is max of the report_vals for each contact.
    if(this_report_val > report_val)
      report_val = this_report_val;
  }    
    
  if(report_val != m_prev_closest_contact_val) { 
    Notify("CONTACT_MGR_CLOSEST", report_val);
    m_prev_closest_contact_val = report_val;
  }
}

//---------------------------------------------------------
// Procedure: clearOldRetiredContacts()

void BasicContactMgr::clearOldRetiredContacts()
{
  // Part 1: Build a vector of retired contacts that have timed-out
  vector<string> to_remove;
  map<string, NodeRecord>::const_iterator p;
  for(p=m_map_node_records.begin(); p!= m_map_node_records.end(); p++) {
    string     contact_name = p->first;
    NodeRecord node_record  = p->second;
    
    double age = m_curr_time - node_record.getTimeStamp();
    if(age > (m_contact_max_age + m_contact_max_age_hist))
      to_remove.push_back(contact_name);
  }
  for(unsigned int i=0; i<to_remove.size(); i++)
    removeOldContact(to_remove[i]);
}

//---------------------------------------------------------
// Procedure: removeOldContact()
//   Purpose: Remove any trace of a contact from memory.
//            The below actions should be exhaustive in their
//            accounting for memory used by a contact

void BasicContactMgr::removeOldContact(string contact)
{
  m_map_node_records.erase(contact);
  m_map_node_alerts_total.erase(contact);
  m_map_node_alerts_active.erase(contact);
  m_map_node_alerts_resolved.erase(contact);
  m_map_node_ranges_actual.erase(contact);
  m_map_node_ranges_extrap.erase(contact);
  m_map_node_ranges_cpa.erase(contact);
  //m_par.removeVehicle(contact);
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

    // #1 Determine and store the actual point-to-point range between ownship
    // and the last absolute known position of the contact
    double range_actual = hypot((m_nav_x - cnx), (m_nav_y - cny));
    m_map_node_ranges_actual[vname] = range_actual;

    // #2 Determine and store the extrapolated range between ownship and the
    // contact position determined by its last known range and extrapolation.
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
    double range_cpa = engine.evalCPA(m_nav_hdg, m_nav_spd, alert_range_cpa_time);
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

    XYCircle circle(m_nav_x, m_nav_y, alert_range);
    circle.set_label(alert_id + "_in");
    circle.set_color("edge", alert_range_color);
    circle.set_vertex_size(0);
    circle.set_edge_size(1);
    circle.set_active(true);
    string s1 = circle.get_spec();
    Notify("VIEW_CIRCLE", s1);

    double alert_range_cpa = getAlertRangeCPA(alert_id);
    if(alert_range_cpa > alert_range) {

      string alert_range_cpa_color = getAlertRangeCPAColor(alert_id);
      XYCircle circ(m_nav_x, m_nav_y, alert_range_cpa);
      circ.set_label(alert_id + "_out");
      circ.set_color("edge", alert_range_cpa_color);
      circ.set_vertex_size(0);
      circ.set_edge_size(1);
      circ.set_active(true);
      string s2 = circ.get_spec();

      Notify("VIEW_CIRCLE", s2);
    }
  }
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
  string alert_count = uintToString(m_map_alerts.size());
  m_msgs << "DisplayRadii:              " << boolToString(m_display_radii) << endl;
  m_msgs << "Deriving X/Y from Lat/Lon: " << boolToString(m_use_geodesy)   << endl;
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
    m_msgs << "  RANGE     = " << alert_rng     << ", " << alert_rng_color     << endl;
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
  m_msgs << "Alert Status Summary: " << endl;
  m_msgs << "----------------------" << endl;
  m_msgs << "       List: " << m_prev_contacts_list         << endl;
  m_msgs << "    Alerted: " << m_prev_contacts_alerted      << endl;
  m_msgs << "  UnAlerted: " << m_prev_contacts_unalerted    << endl;
  m_msgs << "    Retired: " << m_prev_contacts_retired      << endl;
  m_msgs << "      Recap: " << m_prev_contacts_recap        << endl;
  m_msgs << endl;

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
  m_msgs << "Contact Status Summary:" << endl;
  m_msgs << "-----------------------" << endl;
  m_msgs << actab.getFormattedString();
  return(true);
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
  // Part 2: Checks based on range of ownship to contact
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
  // Part 3: Checks based on Alert Region
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

