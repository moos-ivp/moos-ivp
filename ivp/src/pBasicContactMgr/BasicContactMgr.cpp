/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BasicContactMgr.cpp                                  */
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
#include "ACTable.h"

// As of Release 15.4 this is now set in CMake, defaulting to be defined
// #define USE_UTM 

using namespace std;

//---------------------------------------------------------
// Constructor

BasicContactMgr::BasicContactMgr()
{
  m_nav_x   = 0;
  m_nav_y   = 0;
  m_nav_hdg = 0;
  m_nav_spd = 0;

  m_alert_verbose = false;

  m_contact_max_age = 600;   // units in seconds 600 = 10 mins
  m_display_radii   = false;

  m_default_alert_rng           = 1000;
  m_default_alert_rng_cpa       = 1000;
  m_default_alert_rng_color     = "gray65";
  m_default_alert_rng_cpa_color = "gray35";

  m_use_geodesy = false;

  m_decay_start = 15;
  m_decay_end   = 30;

  m_contact_local_coords = "verbatim"; // Or lazy_lat_lon, or force_lat_lon

  m_contacts_recap_interval = 0;
  m_contacts_recap_posted = 0;
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
  postAlerts();

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


    else if(param == "display_radii")
      setBooleanOnString(m_display_radii, value);
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

  m_Comms.Register("NODE_REPORT", 0);
  m_Comms.Register("CONTACT_RESOLVED", 0);
  m_Comms.Register("BCM_DISPLAY_RADII", 0);
  m_Comms.Register("BCM_ALERT_REQUEST", 0);
  m_Comms.Register("NAV_X", 0);
  m_Comms.Register("NAV_Y", 0);
  m_Comms.Register("NAV_SPEED", 0);
  m_Comms.Register("NAV_HEADING", 0);
}

//---------------------------------------------------------
// Procedure: handleMailNodeReport
//   Example: NAME=alpha,TYPE=KAYAK,UTC_TIME=1267294386.51,
//            X=29.66,Y=-23.49, LAT=43.825089,LON=-70.330030, 
//            SPD=2.00,HDG=119.06,YAW=119.05677,DEPTH=0.00,     
//            LENGTH=4.0,MODE=DRIVE

void BasicContactMgr::handleMailNodeReport(const string& report)
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
    m_map_node_ranges[vname] = 0;
    m_map_node_alerts_total[vname]    = 0;
    m_map_node_alerts_active[vname]   = 0;
    m_map_node_alerts_resolved[vname] = 0;

    m_map_node_ranges_actual[vname] = 0;
    m_map_node_ranges_extrap[vname] = 0;
    m_map_node_ranges_cpa[vname]    = 0;

    m_par.addVehicle(vname);
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

void BasicContactMgr::handleMailResolved(const string& str)
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
  
  m_par.setValue(vehicle, alertid, false);
  reportEvent("Resolved: (" + vehicle + "," + alertid + ")");
}

//---------------------------------------------------------
// Procedure: handleMailDisplayRadii

void BasicContactMgr::handleMailDisplayRadii(const string& value)
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

                     
void BasicContactMgr::handleMailAlertRequest(const string& value)
{
  bool ok = handleConfigAlert(value);
  if(!ok)
    reportRunWarning("Unhandled Alert Request: " + value);   
}

//---------------------------------------------------------
// Procedure: handleConfigAlert

bool BasicContactMgr::handleConfigAlert(const string& alert_str)
{
  string alert_id = tokStringParse(alert_str, "id", ',', '=');
  if(alert_id == "")
    alert_id = "no_id";
  m_par.addAlertID(alert_id);

  vector<string> svector = parseStringQ(alert_str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string left  = tolower(biteStringX(svector[i], '='));
    string right = svector[i];
    double dval  = atof(right.c_str());

    if(isQuoted(right))
      right = stripQuotes(right);

    if(left == "var")
      m_map_alert_varname[alert_id] = right;
    else if((left == "val") || (left == "pattern"))
      m_map_alert_pattern[alert_id] = right;
    else if((left == "alert_range") && isNumber(right) && (dval >= 0))
      m_map_alert_rng[alert_id] = dval;
    else if((left == "cpa_range") && isNumber(right) && (dval >= 0))
      m_map_alert_rng_cpa[alert_id] = dval;
    else if((left == "alert_range_color") && isColor(right))
      m_map_alert_rng_color[alert_id] = right;
    else if((left == "cpa_range_color") && isColor(right))
      m_map_alert_rng_cpa_color[alert_id] = right;
    else {
      if(left != "id") {
	reportConfigWarning("unhandled alert config component: " + left);
	return(false);
      }
    }
  }

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
      double range = m_map_node_ranges[contact_name];
      if(contacts_recap != "")
	contacts_recap += " # ";
      contacts_recap += "vname=" + contact_name;
      contacts_recap += ",range=" + doubleToString(range, 2);
      contacts_recap += ",age=" + doubleToString(age, 2);
    }
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
// Procedure: postAlerts

bool BasicContactMgr::postAlerts()
{
  bool new_alerts = false;

  map<string, NodeRecord>::iterator p;
  for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
    string     contact_name  = p->first;
    NodeRecord node_record   = p->second;
    double     contact_range = m_map_node_ranges[contact_name];
    double     age = m_curr_time - node_record.getTimeStamp();

    // For each alert_id
    map<string,string>::iterator q;
    for(q=m_map_alert_varname.begin(); q!=m_map_alert_varname.end(); q++) {
      string alert_id = q->first;
      bool alerted = m_par.getValue(contact_name, alert_id);
      if(!alerted) {
	double alert_range     = getAlertRange(alert_id);
	double alert_range_cpa = getAlertRangeCPA(alert_id);
	if((age <= m_contact_max_age) && (contact_range <= alert_range)) {
	  new_alerts = true;

	  string x_str    = node_record.getStringValue("x");
	  string y_str    = node_record.getStringValue("y");
	  string lat_str  = node_record.getStringValue("lat");
	  string lon_str  = node_record.getStringValue("lon");
	  string spd_str  = node_record.getStringValue("speed");
	  string hdg_str  = node_record.getStringValue("heading");
	  string dep_str  = node_record.getStringValue("depth");
	  string time_str = node_record.getStringValue("time");
	  string name_str = node_record.getName();
	  string type_str = node_record.getType();
	  
	  string var = m_map_alert_varname[alert_id];
	  string msg = m_map_alert_pattern[alert_id];
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
	  m_par.setValue(contact_name, alert_id, true);
	  reportEvent(var + "=" + msg);


	  if(m_alert_verbose) {
	    string mvar = "ALERT_VERBOSE";
	    string mval = "contact=" + contact_name;
	    mval += ",config_alert_range=" + doubleToString(alert_range,1);

	    mval += ",config_alert_range_cpa=" + doubleToString(alert_range_cpa,1);

	    mval += ",range_used=" + doubleToString(contact_range,1);

	    double range_actual = m_map_node_ranges_actual[contact_name];
	    mval += ",range_actual=" + doubleToString(range_actual,1);

	    double range_extrap = m_map_node_ranges_actual[contact_name];
	    mval += ",range_extrap=" + doubleToString(range_extrap,1);

	    double range_cpa = m_map_node_ranges_cpa[contact_name];
	    mval += ",range_cpa=" + doubleToString(range_cpa,1);

	    Notify(mvar, mval);
	  }

	  m_map_node_alerts_total[contact_name]++;
	  m_map_node_alerts_active[contact_name]++;
	}
      }
    }
  }

  return(new_alerts);
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

    //XYPoint point_mb(cnx, cny);
    //point_mb.set_label(node_record.getName() + "xpt_mb");
    //m_Comms.Notify("VIEW_POINT", point_mb.get_spec());

    // #3 Determine and store the cpa range between ownship and the
    // contact position determined by the contact's extrapolated position
    // and it's last known heading and speed.
    CPAEngine engine(cny, cnx, cnh, cns, m_nav_y, m_nav_x);      
    double range_cpa = engine.evalCPA(m_nav_hdg, m_nav_spd, alert_range_cpa_time);
    m_map_node_ranges_cpa[vname] = range_cpa;
    
    // Now we have set:
    //   1. m_map_node_ranges_actual[vname]  (range_actual)
    //   2. m_map_node_ranges_extrap[vname]  (range_extrap)
    //   3. m_map_node_ranges_cpa[vname]     (range_cpa)
    // We just need to decide which one to use for the purposes of 
    // determining whether an alert should be generated.

    // If the extrapolated (non-cpa) range exceeds the minimum threshold, 
    // but is less than the alert_cpa threshold, calculate the CPA and 
    // uses this as the range instead. 

    double alert_range = getAlertRange(vname);          // min threshold
    double alert_range_cpa = getAlertRangeCPA(vname);   // cpa threshold

    m_map_node_ranges[vname] = range_extrap;
    if((range_extrap > alert_range) && (range_extrap < alert_range_cpa)) 
      m_map_node_ranges[vname] = range_cpa;
  }
}

//---------------------------------------------------------
// Procedure: postRadii

void BasicContactMgr::postRadii(bool active)
{
  map<string, double>::const_iterator p;
  for(p=m_map_alert_rng.begin(); p!=m_map_alert_rng.end(); p++) {
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
  string alert_count = uintToString(m_map_alert_varname.size());
  m_msgs << "DisplayRadii:              " << m_display_radii << endl;
  m_msgs << "Deriving X/Y from Lat/Lon: " << m_use_geodesy   << endl << endl;
  m_msgs << "Alert Configurations (" << alert_count << "):"  << endl;
  m_msgs << "---------------------";
  map<string,string>::iterator p;
  for(p=m_map_alert_varname.begin(); p!=m_map_alert_varname.end(); p++) {
    string alert_id = p->first;
    string alert_var = p->second;
    string alert_pattern = m_map_alert_pattern[alert_id];
    string alert_rng     = doubleToStringX(getAlertRange(alert_id));
    string alert_rng_cpa = doubleToStringX(getAlertRangeCPA(alert_id));
    string alert_rng_color     = getAlertRangeColor(alert_id);
    string alert_rng_cpa_color = getAlertRangeCPAColor(alert_id);

    m_msgs << "Alert ID = " << alert_id         << endl;
    m_msgs << "  VARNAME   = " << alert_var     << endl;
    m_msgs << "  PATTERN   = " << alert_pattern << endl;
    m_msgs << "  RANGE     = " << alert_rng     << ", " << alert_rng_color     << endl;
    m_msgs << "  CPA_RANGE = " << alert_rng_cpa << ", " << alert_rng_cpa_color << endl;
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
  actab << "Contact | Range | Alerts  | Alerts | Alerts   ";
  actab << "        |       | Total   | Active | Resolved ";
  actab.addHeaderLines();

  map<string, NodeRecord>::iterator q;
  for(q=m_map_node_records.begin(); q!=m_map_node_records.end(); q++) {
    string vname = q->first;
    string range = doubleToString(m_map_node_ranges[vname], 1);
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
// Procedure: getAlertRange
//      Note: Use this method to access map, not map directly. This 
//            allows intelligent defaults to be applied if missing key.

double BasicContactMgr::getAlertRange(const string& alert_id) const
{
  map<string, double>::const_iterator p;
  p = m_map_alert_rng.find(alert_id);
  if(p == m_map_alert_rng.end())
    return(m_default_alert_rng);
  else
    return(p->second);
}

//---------------------------------------------------------
// Procedure: getAlertRangeCPA
//      Note: Use this method to access map, not map directly. This 
//            allows intelligent defaults to be applied if missing key.

double BasicContactMgr::getAlertRangeCPA(const string& alert_id) const
{
  map<string, double>::const_iterator p;
  p = m_map_alert_rng_cpa.find(alert_id);
  if(p == m_map_alert_rng_cpa.end())
    return(m_default_alert_rng_cpa);
  else
    return(p->second);
}

//---------------------------------------------------------
// Procedure: getAlertRangeColor
//      Note: Use this method to access map, not map directly. This 
//            allows intelligent defaults to be applied if missing key.

string BasicContactMgr::getAlertRangeColor(const string& alert_id) const
{
  map<string, string>::const_iterator p;
  p = m_map_alert_rng_color.find(alert_id);
  if(p == m_map_alert_rng_color.end())
    return(m_default_alert_rng_color);
  else
    return(p->second);
}

//---------------------------------------------------------
// Procedure: getAlertRangeCPAColor
//      Note: Use this method to access map, not map directly. This 
//            allows intelligent defaults to be applied if missing key.

string BasicContactMgr::getAlertRangeCPAColor(const string& alert_id) const
{
  map<string, string>::const_iterator p;
  p = m_map_alert_rng_cpa_color.find(alert_id);
  if(p == m_map_alert_rng_cpa_color.end())
    return(m_default_alert_rng_cpa_color);
  else
    return(p->second);
}



