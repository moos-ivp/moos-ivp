/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ContactMgrV20.cpp                                    */
/*    DATE: Feb 24th 2010                                        */
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
#include <cmath>
#include "LinearExtrapolator.h"
#include "ContactMgrV20.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "ColorParse.h"
#include "CPAEngine.h"
#include "NodeRecordUtils.h"
#include "XYCircle.h"
#include "ACTable.h"
#include "RangeMark.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

ContactMgrV20::ContactMgrV20()
{
  // Configuration Variables
  m_alert_rng_color     = "gray65";
  m_alert_rng_cpa_color = "gray35";

  m_display_radii = false;

  m_max_contacts  = 500;
  
  m_post_closest_range  = false;
  m_post_closest_relbng = false;
  m_post_all_ranges    = false;
  m_contact_max_age    = 60;       // units in seconds 60 = 10 mins
  m_contacts_recap_interval = 1;

  // verbatim, lazy_lat_lon, or force_lat_lon
  m_contact_local_coords = "verbatim"; 
  m_alert_verbose = false;
  m_decay_start = 15;
  m_decay_end   = 30;

  m_max_retired_hist = 5;
  m_use_geodesy = false;

  m_range_report_timeout = 10;
  m_range_report_maxsize = 20;
  
  m_reject_range = 2000;
  
  // State Variables
  m_osx = 0;
  m_osy = 0;
  m_osh = 0;
  m_osv = 0;

  m_contacts_recap_posted = 0;
  m_prev_contacts_count   = 0;
  m_prev_closest_range    = 0;
  m_prev_closest_contact_val = 0;

  m_alert_requests_received = 0;

  m_hold_alerts_for_helm = false;
  m_helm_in_drive_noted  = false;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool ContactMgrV20::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key  = msg.GetKey();
    double dval = msg.GetDouble();
    string sval = msg.GetString(); 
    string src  = msg.GetSource(); 
    string aux  = msg.GetSourceAux(); 

    if(aux != "")
      src += ":" + aux;
    
    if(key == "NAV_X")
      m_osx = dval;
    else if(key == "NAV_Y")
      m_osy = dval;
    else if(key == "NAV_HEADING")
      m_osh = dval;
    else if(key == "NAV_SPEED") 
      m_osv = dval;
    else if(key == "NODE_REPORT") 
      handleMailNodeReport(sval);
    else if(key == "BCM_REPORT_REQUEST") 
      handleMailReportRequest(sval, src);
    else if(key == "BCM_DISPLAY_RADII")
      handleMailDisplayRadii(sval);      
    else if(key == "BCM_ALERT_REQUEST")
      handleMailAlertRequest(sval, src);
    else if(key == "IVPHELM_STATE")
      handleMailHelmState(sval);
    else
      reportRunWarning("Unhandled Mail: " + key);
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool ContactMgrV20::OnConnectToServer()
{
  registerVariables();  
  return(true);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool ContactMgrV20::Iterate()
{
  AppCastingMOOSApp::Iterate();

  updateRanges();

  if(!m_hold_alerts_for_helm || m_helm_in_drive_noted) 
    checkForAlerts();

  postRadii();

  checkForNewRetiredContacts();
  postRangeReports();
  postSummaries();

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool ContactMgrV20::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  // PID published to support uMemWatch or similar oversight
  Notify("PCONTACTMGRV20_PID", getpid());

  m_ownship = m_host_community;
  if(m_ownship == "") 
    reportUnhandledConfigWarning("ownship name unknown, host_community unset");
  
  // Part 1: Set the basic configuration parameters.
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  
  STRING_LIST::reverse_iterator p;
  for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
    string orig  = *p;
    string line  = stripBlankEnds(*p);
    string param = tolower(biteStringX(line, '='));
    string value = line;

    param = handleConfigDeprecations(param);
    
    bool handled = false;
    if(param == "alert") 
      handled = handleConfigAlert(value, "general");
    else if(param == "ignore_name") 
      handled = m_filter_set.addIgnoreName(value);
    else if(param == "match_name") 
      handled = m_filter_set.addMatchName(value);
    else if(param == "ignore_group") 
      handled = m_filter_set.addIgnoreGroup(value);
    else if(param == "match_group") 
      handled = m_filter_set.addMatchGroup(value);
    else if(param == "ignore_type") 
      handled = m_filter_set.addIgnoreType(value);
    else if(param == "match_type") 
      handled = m_filter_set.addMatchType(value);
    else if(param == "match_region") 
      handled = m_filter_set.addMatchRegion(value);
    else if(param == "ignore_region") 
      handled = m_filter_set.addIgnoreRegion(value);
    else if(param == "strict_ignore")
      handled = m_filter_set.setStrictIgnore(value);
    else if(param == "max_contacts")
      handled = setUIntOnString(m_max_contacts, value);

    else if(param == "decay")
      handled = handleConfigDecay(value);
    else if(param == "range_report_timeout")
      handled = setNonNegDoubleOnString(m_range_report_timeout, value);
    else if(param == "range_report_maxsize")
      handled = setUIntOnString(m_range_report_maxsize, value);
    else if(param == "display_radii")
      handled = setBooleanOnString(m_display_radii, value);
    else if(param == "display_radii_id")
      handled = setNonWhiteVarOnString(m_display_radii_id, value);
    else if(param == "post_closest_range") 
      handled = setBooleanOnString(m_post_closest_range, value);
    else if(param == "post_closest_relbng") 
      handled = setBooleanOnString(m_post_closest_relbng, value);
    else if(param == "post_all_ranges") 
      handled = setBooleanOnString(m_post_all_ranges, value);
    else if(param == "max_retired_history")
      handled = handleConfigMaxRetHist(value);
    else if(param == "contact_local_coords") 
      handled = handleConfigCoords(value);
    else if(param == "recap_interval")
      handled = handleConfigRecapInterval(value);
    else if(param == "reject_range")
      handled = handleConfigRejectRange(value);
    else if(param=="alert_verbose")
      handled = setBooleanOnString(m_alert_verbose, value);
    else if(param == "contact_max_age")
      handled = setPosDoubleOnString(m_contact_max_age, value);
    else if(param == "alert_range_color")
      handled = setColorOnString(m_alert_rng_color, value);
    else if(param == "cpa_range_color") 
      handled = setColorOnString(m_alert_rng_cpa_color, value);
    else if(param == "hold_alerts_for_helm") 
      handled = setBooleanOnString(m_hold_alerts_for_helm, value);
    else if(param == "ownship_group") { 
      setNonWhiteVarOnString(m_os_group, value);
      handled = m_filter_set.setOwnshipGroup(value);
    }
    else if(param == "ownship_type") {
      setNonWhiteVarOnString(m_os_type, value);
      handled = m_filter_set.setOwnshipType(value);
    }
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  // Part 2: If alerts were configured, ensure they are complete/valid
  map<string, CMAlert>::iterator q;
  for(q=m_map_alerts.begin(); q!=m_map_alerts.end(); q++) {
    string  alert_id = q->first;
    CMAlert alert = q->second;
    if(!alert.valid())
      reportConfigWarning("Invalid alert config:" + alert_id);
  }


  
  // Part 3: If we may possibly want to set our incoming X/Y report
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
// Procedure: registerVariables()

void ContactMgrV20::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  Register("NODE_REPORT", 0);
  Register("BCM_DISPLAY_RADII", 0);
  Register("BCM_ALERT_REQUEST", 0);
  Register("BCM_REPORT_REQUEST", 0);
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("NAV_SPEED", 0);
  Register("NAV_HEADING", 0);
  Register("IVPHELM_STATE", 0);
}

//---------------------------------------------------------
// Procedure: handleMailNodeReport()
//   Example: NAME=alpha,TYPE=KAYAK,UTC_TIME=1267294386.51,
//            X=29.66,Y=-23.49, LAT=43.825089,LON=-70.330030, 
//            SPD=2.00,HDG=119.06,YAW=119.05677,DEPTH=0.00,     
//            LENGTH=4.0,MODE=DRIVE

void ContactMgrV20::handleMailNodeReport(string report)
{
  NodeRecord new_node_record = string2NodeRecord(report, true);
  string vname = new_node_record.getName();
  // If incoming node name matches ownship, just ignore the node report
  if(vname == m_ownship)
    return;


  // ==============================================================
  // Part 1: Settle contact x/y position if overriding with lat/lon
  // ==============================================================
  // Part 1A: Decide if we want to override X/Y with Lat/Lon based 
  //         on user configuration and state of the node record.
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
 
  // ==============================================================
  // Part 1B: If overriding x/y with latlon and configured to do so
  //          then find x/y from MOOSGeodesy and Lat/Lon and replace.
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

  // ==============================================================
  // Part 2: Check against ContactMgr level ExclusionFilterSet 
  // ==============================================================
  // Note: If this contact is filtered, add it to the set of filtered
  // vnames. If for some reason it had passed this check previously,
  // and failed now, we to retire it promptly. This may occur for
  // example if the contact moved in/out of a match/ignore region.

  bool ok_filt = m_filter_set.filterCheck(new_node_record, m_osx, m_osy);
  if(!ok_filt) {
    m_filtered_vnames.insert(new_node_record.getName());
    return;
  }


  // ==============================================================
  // Part 3: Accept the contact, update data structures
  // ==============================================================
  bool newly_known_vehicle = false;
  if(m_map_node_records.count(vname) == 0)
    newly_known_vehicle = true;
   
  m_map_node_records[vname] = new_node_record;
  
  if(newly_known_vehicle) {
    m_map_node_ranges_actual[vname] = 0;
    m_map_node_ranges_extrap[vname] = 0;
    m_map_node_ranges_cpa[vname]    = 0;

    m_par.addVehicle(vname);

    if(m_alert_verbose) 
      Notify("ALERT_VERBOSE", "new_contact="+vname);
  }
  
  // Check if the contact had been on the retired list (due to age)
  // and if so, resurrect it and remove it from the retired list.
  m_contacts_retired.remove(vname);
}


//---------------------------------------------------------
// Procedure: handleMailDisplayRadii()

void ContactMgrV20::handleMailDisplayRadii(string value)
{
  value = tolower(stripBlankEnds(value));

  if((value == "off") || (value == "false")) 
    m_display_radii = false;
  else if((value == "on") || (value == "true")) 
    m_display_radii = true;
  else if(value == "toggle") 
    m_display_radii = !m_display_radii;
  else if(knownAlert(value)) {
    m_display_radii = true;
    m_display_radii_id = value;
    reportEvent("Display_radii turn on and set to: " + value);
  }
  else {
    string msg = "Failed attempt to set Display_radii set to: " + value;
    reportConfigWarning(msg);
  }
}

//---------------------------------------------------------
// Procedure: handleMailReportRequest()
//   Example: BCM_REPORT_REQUEST = var=BCM_CONTACTS_85, range=85, reason=teaming
//      Note: The reason field is allowed but not processed.
//            It is allowed to support apps such as alogmtask that
//            want to grab report variables related to task teaming

void ContactMgrV20::handleMailReportRequest(string str, string src)
{
  bool ok = true;
  
  string moos_var;
  double range = -1;
  string group;
  string vtype;
  string reason;
  bool   refresh = false;
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
    else if(param == "refresh")
      refresh = (tolower(value) == "true");
    else if(param == "reason")
      reason = value;
    else
      ok = false;
  }

  if(!ok || (moos_var == "") || (range < 0)) {
    string msg = "Failed REPORT_REQUEST: " + str;
    reportEvent(msg);
    reportRunWarning(msg);
  }    
  else {
    string msg = "New BCM_REPORT_REQUEST (src=" + src + "): " + str;
    reportEvent(msg);
    // If this is a new request, make new map entry for all fields
    if(!m_map_rep_range.count(moos_var) ||
       m_map_rep_range[moos_var] != range) {
      m_map_rep_range[moos_var] = range;
      m_map_rep_group[moos_var] = group;
      m_map_rep_vtype[moos_var] = vtype;
      m_map_rep_contacts[moos_var] = "";
      m_map_rep_refresh[moos_var] = refresh;
    }
    // If this is a repeat request (same moos_var and range) then
    // just update the current time.
    m_map_rep_reqtime[moos_var] = m_curr_time;
    m_map_rep_refresh[moos_var] = refresh;
  }
}

//---------------------------------------------------------
// Procedure: handleMailAlertRequest()
//    Format: BCM_ALERT_REQUEST = 
//            var=CONTACT_INFO, val="name=avd_$[VNAME] # contact=$[VNAME]"
//            alert_range=80, cpa_range=95

                     
void ContactMgrV20::handleMailAlertRequest(string value, string src)
{
  m_alert_requests_received++;

  string msg = "New BCM_ALERT_REQUEST (src=" + src + "): " + value;
  reportEvent(msg);

  bool ok = handleConfigAlert(value, src);
  if(!ok)
    reportRunWarning("Unhandled Alert Request: " + value);   
}

//---------------------------------------------------------
// Procedure: handleMailHelmState()
                     
void ContactMgrV20::handleMailHelmState(string value)
{
  if(value == "DRIVE")
    m_helm_in_drive_noted = true;
}

//--------------------------------------------------------------------
// Procedure: handleConfigDeprecations

string ContactMgrV20::handleConfigDeprecations(string param)
{
  param = tolower(param);  

  string new_param = param;

  if(param == "alert_range")
    reportConfigWarning(param + " is deprecated");
  else if(param == "alert_cpa_range")
    reportConfigWarning(param + " is deprecated");
  else if(param == "default_alert_range_color")
    new_param = "alert_range_color";
  else if(param == "default_cpa_range_color")
    new_param = "cpa_range_color";
  else if(param == "contacts_recap_interval")
    new_param = "recap_interval";
  
  if(param != new_param)
    reportConfigWarning(param + " is deprecated. Use instead: " + new_param);

  return(new_param);
}


//---------------------------------------------------------
// Procedure: handleConfigDecay()
//   Example: "30,60"

bool ContactMgrV20::handleConfigDecay(string decay_str)
{
  string left  = biteStringX(decay_str, ',');
  string right = decay_str;

  if(!isNumber(left) || !isNumber(right))
    return(false);
  
  double start = atof(left.c_str());
  double end   = atof(right.c_str());

  if((start >= 0) && (start <= end)) {
    m_decay_start = start;
    m_decay_end   = end;
    return(true);
  }  
  return(false);
}

//---------------------------------------------------------
// Procedure: handleConfigMaxRetHist()
//      Note: Enforces the unsigned int to be in range [1,50]

bool ContactMgrV20::handleConfigMaxRetHist(string str)
{
  bool ok = setUIntOnString(m_max_retired_hist, str);
  if(!ok)
    return(false);

  if((m_max_retired_hist > 50) || (m_max_retired_hist < 1)) {
    string msg = "max_retired_hist should be in range [1,50]";
    reportConfigWarning(msg);
  }
  
  if(m_max_retired_hist > 50)
    m_max_retired_hist = 50;
  if(m_max_retired_hist < 1)
    m_max_retired_hist = 1;

  return(true);
}

//---------------------------------------------------------
// Procedure: handleConfigCoords()

bool ContactMgrV20::handleConfigCoords(string str)
{
  str = tolower(str);
  if((str != "verbatim") && (str != "lazy_lat_lon") &&
     (str != "force_lat_lon"))
    return(false);

  m_contact_local_coords = str;

  return(true);
}

//---------------------------------------------------------
// Procedure: handleConfigRecapInterval()

bool ContactMgrV20::handleConfigRecapInterval(string str)
{
  if(tolower(str) == "off") {
    m_contacts_recap_interval = -1;
    return(true);
  }

  return(setPosDoubleOnString(m_contacts_recap_interval, str));
}

//---------------------------------------------------------
// Procedure: handleConfigRejectRange()

bool ContactMgrV20::handleConfigRejectRange(string str)
{
  if(tolower(str) == "off") {
    m_reject_range = -1;
    return(true);
  }
  return(setPosDoubleOnString(m_reject_range, str));
}

//---------------------------------------------------------
// Procedure: handleConfigAlert

bool ContactMgrV20::handleConfigAlert(string alert_str, string source)
{
  // Part 1: Get the alert id. Allow for an "empty" alert,
  // but call it "no_id".
  string alert_id = tokStringParse(alert_str, "id", ',', '=');
  if(alert_id == "")
    alert_id = "no_id";

  // Part 2: Add to the Platform Alert Record. If alert_id is
  // already known, it's just ignored.
  m_par.addAlertID(alert_id);

  // Part 3: Apply the source to the Alert
  if(source != "")
    m_map_alerts[alert_id].setAlertSource(source);
  
  // Part 4: Parse the configuration string
  string var, pattern;
  vector<string> svector = parseStringQ(alert_str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string left  = tolower(biteStringX(svector[i], '='));
    string right = svector[i];
    double dval  = atof(right.c_str());

    if(isQuoted(right))
      right = stripQuotes(right);

    bool ok = true;
    if(left == "var")
      var = right;
    else if((left == "val") || (left == "pattern")) 
      pattern = right;

    else if((left == "on_flag") || (left == "onflag")) 
      ok = m_map_alerts[alert_id].addAlertOnFlag(right);
    else if((left == "off_flag") || (left == "offflag"))
      ok = m_map_alerts[alert_id].addAlertOffFlag(right);
    else if((left == "alert_range") && isNumber(right))
      ok = m_map_alerts[alert_id].setAlertRange(dval);

    else if(left == "match_name")
      ok = m_map_alerts[alert_id].configFilter(left, right);
    else if(left == "ignore_name")
      ok = m_map_alerts[alert_id].configFilter(left, right);
    else if(left == "match_group")
      ok = m_map_alerts[alert_id].configFilter(left, right);
    else if(left == "ignore_group")
      ok = m_map_alerts[alert_id].configFilter(left, right);
    else if(left == "match_type")
      ok = m_map_alerts[alert_id].configFilter(left, right);
    else if(left == "ignore_type")
      ok = m_map_alerts[alert_id].configFilter(left, right);
    
    else if(left == "match_region")
      ok = m_map_alerts[alert_id].configFilter(left, right);
    else if(left == "ignore_region")
      ok = m_map_alerts[alert_id].configFilter(left, right);

    else if(left == "strict_ignore")
      ok = m_map_alerts[alert_id].configFilter(left, right);

    else if((left == "cpa_range") && isNumber(right))
      ok = m_map_alerts[alert_id].setAlertRangeFar(dval);
    else
      ok = false;

    
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
// Procedure: pruneRangeReports()

void ContactMgrV20::pruneRangeReports()
{
  // Part 1: Identify the oldest report
  string oldest_varname;
  double oldest_age = 0;
  
  map<string, double>::iterator p;
  for(p=m_map_rep_reqtime.begin(); p!=m_map_rep_reqtime.end(); p++) {
    string varname = p->first;
    double reqtime = p->second;
    double age = m_curr_time - reqtime;

    if(age < m_range_report_timeout)
      continue;
    
    if((oldest_varname == "") || (age > oldest_age)) {
      oldest_varname = varname;
      oldest_age = age;
    }
  }

  if(oldest_varname == "")
    return;
  
  // Part 2: Remove from memory the oldest report
  m_map_rep_range.erase(oldest_varname);
  m_map_rep_reqtime.erase(oldest_varname);
  m_map_rep_group.erase(oldest_varname);
  m_map_rep_vtype.erase(oldest_varname);
  m_map_rep_contacts.erase(oldest_varname);
  m_map_rep_refresh.erase(oldest_varname);
}


//---------------------------------------------------------
// Procedure: postRangeReports()

void ContactMgrV20::postRangeReports()
{
  // Part 1: Ensure requested range reports don't grow unbounded
  if(m_map_rep_range.size() > m_range_report_maxsize)
    pruneRangeReports();
  
  // Part 2: For each report (varname), figure out which contacts
  //         satisfy the range threshold for that report
  //         Also check contact group name if report specifies
  //         Also check contact vehicle type if report specifies
  map<string, double>::iterator p;
  for(p=m_map_rep_range.begin(); p!=m_map_rep_range.end(); p++) {
    string varname = p->first;
    double rthresh = p->second;

    // Part 2A: Get the list of contacts for this report
    string contacts;   
    vector<string> vcontacts;
    map<string, double>::iterator q;
    for(q=m_map_node_ranges_extrap.begin();
	q!=m_map_node_ranges_extrap.end(); q++) {
      string vname = q->first;

      // Part 2AA: If report specifies group, check contact for match
      bool group_match = true;
      if(m_map_rep_group[varname] != "") { 
	if(tolower(m_map_rep_group[varname]) !=
	   tolower(m_map_node_records[vname].getGroup()))
	  group_match = false;
      }

      // Part 2AB: If report specifies vtype, check contact for match
      bool vtype_match = true;
      if(m_map_rep_vtype[varname] != "") {
	if(tolower(m_map_rep_vtype[varname]) !=
	   tolower(m_map_node_records[vname].getType()))
	  vtype_match = false;
      }

      // Part 2AC: Check if the range is satisfied
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
    // Part 2B: If refresh requested or the report is different, then post it!
    if(m_map_rep_refresh[varname] || (contacts != m_map_rep_contacts[varname])) {
      Notify(varname, contacts);
      m_map_rep_contacts[varname] = contacts;
      m_map_rep_refresh[varname]  = false;
    }
  }
}

//---------------------------------------------------------
// Procedure: postSummaries

void ContactMgrV20::postSummaries()
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
  string contacts_recap;

  string closest_contact;
  double closest_range = 0;
  double closest_relbng = 0;

  list<double> ranges;
  
  map<string, NodeRecord>::const_iterator p;
  for(p=m_map_node_records.begin(); p!= m_map_node_records.end(); p++) {
    string     contact_name = p->first;
    NodeRecord node_record  = p->second;

    if(contacts_list != "")
      contacts_list += ",";
    contacts_list += contact_name;

    double age   = m_curr_time - node_record.getTimeStamp();
    double range = m_map_node_ranges_actual[contact_name];

    ranges.push_front(range);
    
    // Update who is the closest contact and it's range
    if((closest_contact == "") || (range < closest_range)) {
      closest_contact = contact_name;
      closest_range   = range;

      double cnx = m_map_node_records[contact_name].getX();
      double cny = m_map_node_records[contact_name].getY();
      closest_relbng  = relBearing(m_osx, m_osy, m_osh, cnx, cny);
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

  // Added mikerb Oct 15, 2023
  if((closest_contact != "") && m_post_closest_relbng) {
    // Round to integer and only post when changed, to reduce postings
    long int closest_relbng_int = closest_relbng;
    closest_relbng = (double)(closest_relbng_int);
    if(closest_relbng != m_prev_closest_relbng) {
      Notify("CONTACT_CLOSEST_RELBNG", closest_relbng_int);
      m_prev_closest_relbng = closest_relbng;
    }
  }
  // End Added mikerb Oct 15, 2023

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

  double time_since_last_recap = m_curr_time - m_contacts_recap_posted;

  // recaps may be configured "off" and the interval would be -1.
  if((m_contacts_recap_interval > 0) &&
     (time_since_last_recap > m_contacts_recap_interval)) {
    m_contacts_recap_posted = m_curr_time;
    Notify("CONTACTS_RECAP", contacts_recap);
    m_prev_contacts_recap = contacts_recap;
  }

  //============================================================
  // Build the CONTACT_RANGES posting

  if(m_post_all_ranges) {
    string ranges_report;
    ranges.sort();
    list<double>::iterator p2;
    for(p2=ranges.begin(); p2!=ranges.end(); p2++) {
      if(ranges_report != "")
	ranges_report += ",";
      double range = *p2;
      ranges_report += doubleToStringX(range,1);
    }
    Notify("CONTACT_RANGES", ranges_report);
  }

}


//---------------------------------------------------------
// Procedure: checkForAlerts
//   Purpose: Check each contact/alert pair and handle if the
//            alert condition changes.

void ContactMgrV20::checkForAlerts()
{
  //==============================================================
  // For each contact, check all alerts
  //==============================================================
  map<string, NodeRecord>::iterator p;
  for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
    string     contact = p->first;
    NodeRecord record  = p->second;
    
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

	double alert_range = getAlertRange(id);
	double alert_range_cpa = getAlertRangeCPA(id);
	mval += ",alert_range=" + doubleToStringX(alert_range,1);
	mval += ",alert_range_cpa=" + doubleToStringX(alert_range_cpa,1);
	
	double range_actual = m_map_node_ranges_actual[contact];
	mval += ",range_actual=" + doubleToString(range_actual,1);	
	if(m_par.getAlertedValue(contact,id)) {
	  double range_cpa = m_map_node_ranges_cpa[contact];
	  mval += ",range_cpa=" + doubleToString(range_cpa,1);
	}
	
	Notify(mvar, mval);
      }
    }
  }
}


//---------------------------------------------------------
// Procedure: checkForNewRetiredContacts()
//   Purpose: Check all contacts and see if any of them should be
//            moved onto the retired list based on contact_max_age.

void ContactMgrV20::checkForNewRetiredContacts()
{
  unsigned int starting_amt = m_map_node_records.size();
  
  //==============================================================
  // Part 1: Find new contacts that need to be retired.
  //==============================================================
  // To-be-retired set starts with the vnames flagged when receiving
  // node reports, that did not survive one of the filter criteria
  set<string> to_be_retired = m_filtered_vnames;
  map<string, NodeRecord>::iterator p;
  for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
    string     contact  = p->first;
    NodeRecord record   = p->second;

    // Possibly drop due to age
    double age = m_curr_time - record.getTimeStamp();
    if(age > m_contact_max_age) 
      to_be_retired.insert(contact);

    // Possibly drop due to reject_range. 
    else if(m_map_node_ranges_actual.count(contact) && (m_reject_range > 0)) {
      // Reject range adjusted 5pct higher to avoid thrashing
      double adjusted_reject_range = m_reject_range * 1.05;
      if(m_map_node_ranges_actual[contact] > adjusted_reject_range) {
	to_be_retired.insert(contact);
      }
    }
  }

  //==============================================================
  // Part 1A: Contingent Culling based on staleness. If pruning is
  //          is still needed, remove based on lowere stale thresh.
  //==============================================================
  // Cull if needed
  if((to_be_retired.size() + m_max_contacts) < starting_amt) {
    map<string, NodeRecord>::iterator p;
    for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
      string     contact  = p->first;
      NodeRecord record   = p->second;
      
      // Possibly drop due to age
      double age = m_curr_time - record.getTimeStamp();
      double lower_contact_max_age = m_contact_max_age / 2;
      if(age > lower_contact_max_age) 
	to_be_retired.insert(contact);      
    }
  }
  
  //==============================================================
  // Part 1B: Dire Contingent Culling. Keep only the N closest
  //==============================================================
  if((to_be_retired.size() + m_max_contacts) < starting_amt) {
    unsigned int to_cull = starting_amt - (to_be_retired.size() + m_max_contacts);
    list<string> ordered_contacts = getRangeOrderedContacts();

    unsigned int culled_so_far = 0;
    list<string>::reverse_iterator p;
    for(p=ordered_contacts.rbegin(); p!=ordered_contacts.rend(); p++) {
      string contact = *p;
      if(to_be_retired.count(contact) == 0) {
	to_be_retired.insert(contact);
	culled_so_far++;
      }
      if(culled_so_far >= to_cull)
	break;
    }
  }
  
  //==============================================================
  // Part 2: Go through the set of newly retired contacts and do
  //         (a) free up any memory associated with this contact
  //         (b) Merge newly retired contacts onto the front of 
  //         the list of previously retired contacts, since we
  //         always delete retired contacts from back of this list.
  //==============================================================
  set<string>::iterator q;
  for(q=to_be_retired.begin(); q!=to_be_retired.end(); q++) {
    string contact = *q;

    // (a) If contact currently in memory and will be removed,
    // then add to front of list of retired contacts
    if(m_map_node_records.count(contact))
      m_contacts_retired.push_front(contact);

    // (b) Free up any memory associated with this contact
    m_map_node_records.erase(contact);
    m_map_node_ranges_actual.erase(contact);
    m_map_node_ranges_extrap.erase(contact);
    m_map_node_ranges_cpa.erase(contact);
    m_par.removeVehicle(contact);
  }

  // Part 3: Clear the set of filtered_vnames. This list should start
  // each iteration empty, with names added as node report mail is
  // processed. Now that we have used it here in this function to seed
  // the set of names to retire, it can be cleared now.
  m_filtered_vnames.clear();
  
}

//----------------------------------------------------------------
// Procedure: postOnAlerts

void ContactMgrV20::postOnAlerts(NodeRecord record, string id)
{
  if(!knownAlert(id))
    return;

  vector<VarDataPair> pairs = getAlertOnFlags(id);
  for(unsigned int i=0; i<pairs.size(); i++)
    postAlert(record, pairs[i]);
  
}

//----------------------------------------------------------------
// Procedure: postOffAlerts

void ContactMgrV20::postOffAlerts(NodeRecord record, string id)
{
  if(!knownAlert(id))
    return;

  vector<VarDataPair> pairs = getAlertOffFlags(id);
  for(unsigned int i=0; i<pairs.size(); i++)
    postAlert(record, pairs[i]);
  
}

//----------------------------------------------------------------
// Procedure: postAlert

void ContactMgrV20::postAlert(NodeRecord record, VarDataPair pair)
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
  string group_str = record.getGroup();
  
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
  msg = findReplace(msg, "$[GROUP]", group_str);
  msg = findReplace(msg, "$[UTIME]", time_str);
  msg = findReplace(msg, "%[VNAME]", tolower(name_str));
  msg = findReplace(msg, "%[VTYPE]", tolower(type_str));
  msg = findReplace(msg, "%[GROUP]", tolower(group_str));

  Notify(var, msg);
  reportEvent(var + "=" + msg);
}

//---------------------------------------------------------
// Procedure: updateRanges

void ContactMgrV20::updateRanges()
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
    double range_actual = hypot((m_osx - cnx), (m_osy - cny));
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
      range_extrap = hypot((m_osx - cnx), (m_osy - cny));
    }
    m_map_node_ranges_extrap[vname] = range_extrap;

    // #3 Determine and store the cpa range between ownship and the
    // contact position determined by the contact's extrapolated position
    // and it's last known heading and speed.
    CPAEngine engine(cny, cnx, cnh, cns, m_osy, m_osx);      
    double range_cpa = engine.evalCPA(m_osh, m_osv,
				      alert_range_cpa_time);
    m_map_node_ranges_cpa[vname] = range_cpa;
  }
}

//---------------------------------------------------------
// Procedure: postRadii

void ContactMgrV20::postRadii(bool active)
{
  // Sanity check 1: Check if all radii displaying is disabled
  if(!m_display_radii)
    return;
  // Sanity check 2: Check that there is at lease one alert
  if(m_map_alerts.size() == 0)
    return;
  // Sanity check 3: If ID is non-empty it must be a known id  
  if((m_display_radii_id != "") && !knownAlert(m_display_radii_id))
    return;
  // Sanity check 4: If displaying enabled, but ID is empty, then
  // pick one. Note that this setting persists.
  if(m_display_radii && (m_display_radii_id == ""))
    m_display_radii_id = m_map_alerts.begin()->first;

  
  // Part 1: Get the alert information based on ID 
  string alert_id = m_display_radii_id;
  double alert_range = getAlertRange(alert_id);
  double alert_range_cpa = getAlertRangeCPA(alert_id);
  
  if(!active || (alert_range <= 0) || 
     (m_alert_rng_color == "invisible") ||
     (m_alert_rng_color == "empty") ||
     (m_alert_rng_color == ""))
    active = false;
  
  XYCircle circle(m_osx, m_osy, alert_range);
  circle.set_label(alert_id + "_in");
  circle.set_color("edge", m_alert_rng_color);
  circle.set_vertex_size(0);
  circle.set_edge_size(1);
  circle.set_active(active);
  circle.set_duration(3);
  circle.set_time(m_curr_time);
  string s1 = circle.get_spec();
  Notify("VIEW_CIRCLE", s1);

  if(alert_range_cpa > alert_range) {
    
    if(!active || (m_alert_rng_cpa_color == "invisible") ||
       (m_alert_rng_cpa_color == "empty") ||
       (m_alert_rng_cpa_color == ""))
      active = false;
    
    XYCircle circ(m_osx, m_osy, alert_range_cpa);
    circ.set_label(alert_id + "_out");
    circ.set_color("edge", m_alert_rng_cpa_color);
    circ.set_vertex_size(0);
    circ.set_edge_size(1);
    circ.set_active(active);
    circ.set_duration(3);
    circ.set_time(m_curr_time);
    string s2 = circ.get_spec();
    
    Notify("VIEW_CIRCLE", s2);
  }
}

//---------------------------------------------------------
// Procedure: checkAlertApplies()

bool ContactMgrV20::checkAlertApplies(string contact, string id) 
{
  //=========================================================
  // Part 1: Sanity checks
  //=========================================================
  // Return false immediately if alert or contact are unknown
  if(!knownAlert(id))
    return(false);
  if(!m_map_alerts.at(id).valid())
    return(false);
  if(m_map_node_records.count(contact) == 0)
    return(false);

  // Return false immediately if age of node record exceeds max age
  NodeRecord record = m_map_node_records.at(contact);
  double age = m_curr_time - record.getTimeStamp();
  if(age > m_contact_max_age)
    return(false);

  //=========================================================
  // Part 2: Apply the alert's exclusion filter
  //=========================================================
  bool okfilter = m_map_alerts.at(id).filterCheck(record, m_osx, m_osy);
  if(!okfilter)
    return(false);

  //=========================================================
  // Part 3: Check range and cpa_range of ownship to contact
  //=========================================================
  double alert_range     = getAlertRange(id);
  double alert_range_cpa = getAlertRangeCPA(id);

  // If alert range is not positive, regarded as having the range
  // criteria OFF. Likely this alert depends only on the region.
  if(alert_range > 0) {
    double contact_range_abs = m_map_node_ranges_actual[contact]; 
    double contact_range_cpa = m_map_node_ranges_cpa[contact];    
    
    if(contact_range_abs > alert_range_cpa)
      return(false);
    
    if((contact_range_abs > alert_range) && (contact_range_cpa > alert_range))
      return(false);
  }
  
  // If none of the above no-apply conditions hold, return true!
  return(true);  
}

//---------------------------------------------------------
// Procedure: knownAlert()

bool ContactMgrV20::knownAlert(string alert_id) const
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

double ContactMgrV20::getAlertRange(string alert_id) const
{
  if(!knownAlert(alert_id))
    return(-1);
  return(m_map_alerts.at(alert_id).getAlertRange());
}

//---------------------------------------------------------
// Procedure: getAlertRangeCPA
//      Note: Use this method to access map, not map directly. This 
//            allows intelligent defaults to be applied if missing key.

double ContactMgrV20::getAlertRangeCPA(string alert_id) const
{
  if(!knownAlert(alert_id))
    return(-1);
  return(m_map_alerts.at(alert_id).getAlertRangeFar());
}

//---------------------------------------------------------
// Procedure: getAlertOnFlags

vector<VarDataPair> ContactMgrV20::getAlertOnFlags(string alert_id) const
{
  vector<VarDataPair> empty;
  if(!knownAlert(alert_id))
    return(empty);
  return(m_map_alerts.at(alert_id).getAlertOnFlags());
}

//---------------------------------------------------------
// Procedure: getAlertOffFlags

vector<VarDataPair> ContactMgrV20::getAlertOffFlags(string alert_id) const
{
  vector<VarDataPair> empty;
  if(!knownAlert(alert_id))
    return(empty);
  return(m_map_alerts.at(alert_id).getAlertOffFlags());
}

//---------------------------------------------------------
// Procedure: getRangeOrderedContacts()
//   Purpose: Get a list of all contact names, sorted by range,
//            with the closest contact at the front of the list.

list<string> ContactMgrV20::getRangeOrderedContacts() const
{
  list<string> to_be_returned_ordered_list;

  list<RangeMark> working_list;
  map<string,double>::const_iterator p;
  for(p=m_map_node_ranges_actual.begin(); p!=m_map_node_ranges_actual.end(); p++) {
    string vname = p->first;
    double range = p->second;
    RangeMark range_mark(vname, range);
    working_list.push_front(range_mark);
  }

  working_list.sort();

  list<RangeMark>::iterator q;
  for(q=working_list.begin(); q!=working_list.end(); q++) {
    RangeMark range_mark = *q;
    to_be_returned_ordered_list.push_back(range_mark.getContact());
  }

  return(to_be_returned_ordered_list);
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
//   RANGE     = 1000
//   CPA_RANGE = 1200
//
// Alert ID = trail
//   VARNAME   = TRAIL_INFO
//   PATTERN   = name=trail_$[VNAME] # contact=$[VNAME]
//   RANGE     = 600
//   CPA_RANGE = 750
//
// Alert Status Summary
// ----------------------
//        List: henry
//     Alerted: 
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
// 143.2  CONTACT_INFO = name=gilda # contact=gilda
//  43.2  CONTACT_INFO = name=gilda # contact=gilda

bool ContactMgrV20::buildReport()
{
  //=================================================================
  // Part 1: Header Content
  //=================================================================
  string alert_count = uintToString(m_map_alerts.size());
  string bcm_req_received = uintToString(m_alert_requests_received);
  string max_age = doubleToStringX(m_contact_max_age,2);
  string reject_range = "off";

  string os_group = m_os_group;
  if(os_group == "")
    os_group = "not specified";
  string os_type = m_os_type;
  if(os_type == "")
    os_type = "not specified";

  if(m_reject_range > 0)
    reject_range = doubleToStringX(m_reject_range,2);
  m_msgs << "Ownship Group:      " << os_group << endl;
  m_msgs << "Ownship Type:       " << os_type << endl;
  m_msgs << "X/Y from Lat/Lon:   " << boolToString(m_use_geodesy)   << endl;
  m_msgs << "Contact Max Age:    " << max_age << endl;
  m_msgs << "Reject Range:       " << reject_range << endl;
  m_msgs << "BCM_ALERT_REQUESTs: " << bcm_req_received << endl;
  m_msgs << "DisplayRadii:       " << boolToString(m_display_radii) << endl;
  if(m_display_radii_id != "")
    m_msgs << "DisplayRadii ID:    " << m_display_radii_id << endl;
  m_msgs << endl; 

  //=================================================================
  // Part 2: Alert Configurations
  //=================================================================
  m_msgs << "Alert Configurations (" << alert_count << "):"  << endl;
  m_msgs << "---------------------" << endl;
  map<string, CMAlert>::iterator p;
  unsigned int acounter = 1;
  for(p=m_map_alerts.begin(); p!=m_map_alerts.end(); p++) {
    string alert_id = p->first;
    m_msgs << uintToString(acounter) << " ID: " << alert_id << endl;
    vector<string> alert_summary = p->second.getSummary();
    for(unsigned int i=0; i<alert_summary.size(); i++) {
      m_msgs << "  " << alert_summary[i] << endl;;
    }
    acounter++;
  }
  m_msgs << endl;

  //=================================================================
  // Part 3: Alert Status Summary
  //=================================================================
  m_msgs << "Alert Status Summary: " << endl;
  m_msgs << "----------------------" << endl;
  m_msgs << "       List: " << m_prev_contacts_list         << endl;
  m_msgs << "    Alerted: " << m_prev_contacts_alerted      << endl;
  m_msgs << "    Retired: " << m_prev_contacts_retired      << endl;
  m_msgs << "      Recap: " << m_prev_contacts_recap        << endl;
  m_msgs << endl;

  //=================================================================
  // Part 4: Contact Status Summary
  //=================================================================
  m_msgs << "Contact Status Summary:" << endl;
  m_msgs << "-----------------------" << endl;

  ACTable actab(4,4);
  actab.setColumnJustify(1, "right");
  actab << "Contact | Range  | Alerts  | Alerts ";
  actab << "        | Actual | Total   | Active ";
  actab.addHeaderLines();

  unsigned contacts_reported = 0;
  map<string, NodeRecord>::iterator q;
  for(q=m_map_node_records.begin(); q!=m_map_node_records.end(); q++) {
    contacts_reported++;
    if(contacts_reported < 8) {
      string vname = q->first;
      string range = doubleToString(m_map_node_ranges_actual[vname], 1);
      string alerts_total  = uintToString(m_par.getAlertsTotal(vname));
      string alerts_active = uintToString(m_par.getAlertsActive(vname));
      actab << vname << range << alerts_total << alerts_active;
    }
    else
      break;
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


