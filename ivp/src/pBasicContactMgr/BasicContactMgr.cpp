/************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J.Leonard                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: BasicContactMgr.cpp                             */
/*    DATE: Feb 24th 2010                                   */
/************************************************************/

#ifdef _WIN32
  #define _USE_MATH_DEFINES
#endif

#include <cmath>
#include <iterator>
#include <vector>
#include "BasicContactMgr.h"
#include "MBUtils.h"
#include "ColorParse.h"
#include "CPAEngine.h"

using namespace std;

//---------------------------------------------------------
// Constructor

BasicContactMgr::BasicContactMgr()
{
  m_nav_x   = 0;
  m_nav_y   = 0;
  m_nav_lat = 0;
  m_nav_lon = 0;
  m_nav_hdg = 0;
  m_nav_spd = 0;
  m_nav_dep = 0;

  m_pos_timestamp = 0;

  m_contact_max_age = 3600;   // units in seconds 3600 = 1 hour
  m_alert_range     = 1000;   // units in meters
  m_alert_cpa_range = 1000;   // units in meters
  m_alert_cpa_time  = 0;      // units in seconds
  m_verbose         = true;
  m_display_radii   = false;

  m_start_time = -1;
  m_curr_time  = -1;

  m_iteration = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool BasicContactMgr::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
	
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
	
    string key   = msg.GetKey();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    double mtime = msg.GetTime();
    string msrc  = msg.GetSource();

    if(key == "NODE_REPORT") {
      bool ok = handleNodeReport(sval);
      if(!ok)
	m_Comms.Notify("CONTACT_MGR_WARNING", "Bad Node Report Rcvd");
    }
    else if(key == "NAV_X") {
      m_nav_x = dval;
      m_pos_timestamp = mtime;
    }
    else if(key == "NAV_Y") {
      m_nav_y = dval;
      m_pos_timestamp = mtime;
    }
    else if(key == "NAV_LAT") {
      m_nav_lat = dval;
      m_pos_timestamp = mtime;
    }
    else if(key == "NAV_LON") {
      m_nav_lon = dval;
      m_pos_timestamp = mtime;
    }
    else if(key == "NAV_HEADING")
      m_nav_hdg = dval;
    else if(key == "NAV_SPEED") 
      m_nav_spd = dval;
    else if(key == "NAV_DEPTH")
      m_nav_dep = dval;
    else if(key == "CONTACT_RESOLVED") {
      bool ok = handleResolved(sval);
      if(!ok) {
	string msg = "Unresolvable contact: " + sval;
	m_Comms.Notify("CONTACT_MGR_WARNING", msg);
      }
    }
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool BasicContactMgr::OnConnectToServer()
{
  RegisterVariables();  
  return(true);
}


//------------------------------------------------------------
// Procedure: RegisterVariables

void BasicContactMgr::RegisterVariables()
{
  m_Comms.Register("NODE_REPORT", 0);
  m_Comms.Register("CONTACT_RESOLVED", 0);
  m_Comms.Register("NAV_X", 0);
  m_Comms.Register("NAV_Y", 0);
  m_Comms.Register("NAV_LAT", 0);
  m_Comms.Register("NAV_LON", 0);
  m_Comms.Register("NAV_SPEED", 0);
  m_Comms.Register("NAV_HEADING", 0);
  m_Comms.Register("NAV_DEPTH", 0);
}


//---------------------------------------------------------
// Procedure: Iterate()
//      Note: Happens AppTick times per second

bool BasicContactMgr::Iterate()
{
  m_iteration++;

  m_curr_time = MOOSTime();
  if(m_start_time == -1)
    m_start_time = m_curr_time;

  updateRanges();
  postSummaries();

  bool new_alerts = postAlerts();

  if(m_verbose)
    postConsoleSummary();
  else {
    if(new_alerts)
      cout << "A" << flush;
    else
      cout << "*" << flush;
  }

  
  if(m_display_radii)
    postRadii();

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool BasicContactMgr::OnStartUp()
{
  CMOOSApp::OnStartUp();
  MOOSTrace("pBasicContactMgr starting....\n");

  // Look for ownship name, a global variable in the config file.
   m_ownship = "unknown";
  if(!m_MissionReader.GetValue("Community", m_ownship)) {
    MOOSTrace("Vehicle (Community) Name not provided\n");
    return(false);
  }
  
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::reverse_iterator p;
    for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
      string line  = stripBlankEnds(*p);
      string param = tolower(stripBlankEnds(biteString(line, '=')));
      string value = stripBlankEnds(line);
      double dval  = atof(value.c_str());

      if(param == "alert") {
	bool ok = addNewAlert(value);
	if(!ok) 
	  postErrorMsg("Failed alert config: " + value);
      }
      else if(param == "display_radii")
	setBooleanOnString(m_display_radii, value);
      else if(param == "verbose")
	setBooleanOnString(m_verbose, value);
      else if(param == "alert_range") {
	if(dval > 0)
	  m_alert_range = dval;
	else
	  postErrorMsg("alert_range must be > zero: " + value);
      }
      else if(param == "alert_cpa_range") {
	if(dval > 0)
	  m_alert_cpa_range = dval;
	else
	  postErrorMsg("alert_cpa_range must be > zero: " + value);
      }
      else if(param == "alert_cpa_time") {
	if(dval > 0)
	  m_alert_cpa_time = dval;
	else
	  postErrorMsg("alert_cpa_time must be > zero: " + value);
      }
      else if(param == "contact_max_age") {
	if(dval > 0)
	  m_contact_max_age = dval;
	else
	  postErrorMsg("contact_max_age must be > zero: " + value);
      }
      else {
	if((param != "apptick") && (param != "commstick")) 
	  postErrorMsg("Unrecognized parameter: " + param);
      }
    }
  }

  if(m_verbose) {
    cout << termColor("blue") << "pBasicContactMgr Alerts: ";
    unsigned int i, vsize = m_alert_var.size();
    for(i=0; i<vsize; i++) {
      cout << "[" << i << "]: VAR=" << m_alert_var[i];
      cout << "  VAL=" << m_alert_val[i] << endl;
    }
    cout << termColor() << endl;
  }  

  RegisterVariables();
  return(true);
}


//---------------------------------------------------------
// Procedure: postErrorMsg()

void BasicContactMgr::postErrorMsg(const string& msg)
{
  cout << termColor("red");
  cout << msg << termColor() << endl;
}


//---------------------------------------------------------
// Procedure: handleNodeReport
//   Example: NAME=alpha,TYPE=KAYAK,MOOSDB_TIME=30.07, 
//            UTC_TIME=1267294386.51,X=29.66,Y=-23.49, 
//            LAT=43.825089, LON=-70.330030, SPD=2.00, 
//            HDG=119.06,YAW=119.05677,DEPTH=0.00,     
//            LENGTH=4.0,MODE=ENGAGED

bool BasicContactMgr::handleNodeReport(const string& report)
{
  string vname, vtype, mode;
  double x, y, lat, lon, utime, mtime, depth, speed, heading;
  double yaw, length;

  vector<string> svector = parseString(report, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = toupper(stripBlankEnds(biteString(svector[i], '=')));
    string value = stripBlankEnds(svector[i]);
    
    if(param == "NAME")
      vname = value;
    else if(param == "TYPE")
      vtype = value;
    else if(param == "MODE")
      mode = value;
    else if(param == "MOOSDB_TIME")
      mtime = atof(value.c_str());
    else if(param == "UTC_TIME")
      utime = atof(value.c_str());
    else if(param == "X")
      x = atof(value.c_str());
    else if(param == "Y")
      y = atof(value.c_str());
    else if(param == "LAT")
      lat = atof(value.c_str());
    else if(param == "LON")
      lon = atof(value.c_str());
    else if(param == "SPD")
      speed = atof(value.c_str());
    else if(param == "HDG")
      heading = atof(value.c_str());
    else if(param == "YAW")
      yaw = atof(value.c_str());
    else if(param == "DEPTH")
      depth = atof(value.c_str());
    else if(param == "LENGTH")
      length = atof(value.c_str());
  }

  // If incoming node name matches ownship, just ignore the node report
  // but don't return false which would indicate an error.
  if(vname == m_ownship) {
    return(true);
  }

  // Find out if the node report corresponds to a contact already
  // in the records
  unsigned int j, jsize = m_records.size();
  unsigned int ix = jsize;
  for(j=0; j<jsize; j++)
    if(vname == m_records[j].getName())
      ix = j;

  // If the record does not already exist, make one
  if(ix == jsize) {
    ContactRecord new_record(vname);
    m_records.push_back(new_record);
    m_par.addVehicle(vname);
    m_ranges.push_back(0);
  }

  // Now push the info into the (either new or old) record.
  m_records[ix].setX(x);
  m_records[ix].setY(y);
  m_records[ix].setLat(lat);
  m_records[ix].setLon(lon);
  m_records[ix].setHeading(heading);
  m_records[ix].setSpeed(speed);
  m_records[ix].setDepth(depth);
  m_records[ix].setTimeStamp(utime);
  m_records[ix].setType(vtype);

  return(true);
}

//---------------------------------------------------------
// Procedure: handleResolved
//   Returns: true if the contact name is found, false otherwise
//      Note: This MOOS process is registering for the variable
//            CONTACT_RESOLVED, which simply contains the string
//            value of a contact name. 
//      Note: The notion of "resolved" means that this contact mgr
//            needs to post another alert when or if the alert
//            criteria is met again.

bool BasicContactMgr::handleResolved(string resolution)
{
  string vehicle = stripBlankEnds(biteString(resolution, ','));
  string alertid = stripBlankEnds(resolution);

  if(alertid == "")
    alertid = "all_alerts";

  augmentAlertHistory("TryResolve: (" + vehicle + "," + alertid + ")");

  if(!m_par.containsVehicle(vehicle))
    return(false);
  if((alertid != "all_alerts") && !m_par.containsAlertID(alertid))
    return(false);
  
  m_par.setValue(vehicle, alertid, false);
  augmentAlertHistory("Resolved: (" + vehicle + "," + alertid + ")");
  return(true);
}


//---------------------------------------------------------
// Procedure: addNewAlert

bool BasicContactMgr::addNewAlert(const string& alert_str)
{
  string varname;
  string value;
  string alert_id = "no_id";

  vector<string> svector = parseStringQ(alert_str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string left  = tolower(stripBlankEnds(biteString(svector[i], '=')));
    string right = stripBlankEnds(svector[i]);
    
    if(isQuoted(right))
      right = stripQuotes(right);

    if(left == "var")
      varname = right;
    else if(left == "val")
      value = right;
    else if(left == "id")
      alert_id = right;
    else {
      cout << "pBasicContactMgr: Bad Alert component: " << left << endl;
      return(false);
    }
  }

  if((varname == "") || (value == "") || strContainsWhite(varname)) {
    cout << "pBasicContactMgr: Faulty Alert configuration: " << endl;
    cout << alert_str << endl;
    return(false);
  }

  m_alert_var.push_back(varname);
  m_alert_val.push_back(value);
  m_alert_id.push_back(alert_id);
  
  if(alert_id != "")
    m_par.addAlertID(alert_id);

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

  unsigned int i, vsize = m_records.size();
  for(i=0; i<vsize; i++) {
    string contact_name = m_records[i].getName();
    if(contacts_list != "")
      contacts_list += ",";
    contacts_list += contact_name;

    double age = m_curr_time - m_records[i].getTimeStamp();
    if(age > m_contact_max_age) {
      if(contacts_retired == "")
	contacts_retired += ",";
      contacts_retired += contact_name;
    }

    double range = m_ranges[i];

    if(contacts_recap != "")
      contacts_recap += " # ";
    contacts_recap += "vname=" + contact_name;
    contacts_recap += ",range=" + doubleToString(range, 2);
    contacts_recap += ",age=" + doubleToString(age, 2);
  }

  if(m_prev_contacts_list != contacts_list) {
    m_Comms.Notify("CONTACTS_LIST", contacts_list);
    m_prev_contacts_list = contacts_list;
  }

  if(m_prev_contacts_alerted != contacts_alerted) {
    contacts_alerted = m_par.getAlertedGroup(true);
    m_Comms.Notify("CONTACTS_ALERTED", contacts_alerted);
    m_prev_contacts_alerted = contacts_alerted;
  }

  if(m_prev_contacts_unalerted != contacts_unalerted) {
    contacts_unalerted = m_par.getAlertedGroup(false);
    m_Comms.Notify("CONTACTS_UNALERTED", contacts_unalerted);
    m_prev_contacts_unalerted = contacts_unalerted;
  }

  if(m_prev_contacts_recap != contacts_recap) {
    m_Comms.Notify("CONTACTS_RECAP", contacts_recap);
    m_prev_contacts_recap = contacts_recap;
  }
  
  if(m_prev_contacts_retired != contacts_retired) {
    m_Comms.Notify("CONTACTS_RETIRED", contacts_retired);
    m_prev_contacts_retired = contacts_retired;
  }
}


//---------------------------------------------------------
// Procedure: postAlerts

bool BasicContactMgr::postAlerts()
{
#if 0
  // Quick check to see if there are any posts needed, otherwise
  // we can return immediately.
  bool alerts_pending = m_par.alertsPending();
  if(!alerts_pending)
    return(false);
#endif  

  bool new_alerts = false;
  unsigned int i, isize = m_records.size();
  unsigned int j, jsize = m_alert_var.size();
  for(i=0; i<isize; i++) {
    for(j=0; j<jsize; j++) {
      string ivname = m_records[i].getName();
      string jalert = m_alert_id[j];;
      bool alerted = m_par.getValue(ivname, jalert);
      if(!alerted) {
	double range = m_ranges[i];
	double age = m_curr_time - m_records[i].getTimeStamp();
	if((age <= m_contact_max_age) && (range <= m_alert_range)) {
	  string x_str   = m_records[i].getValue("x", m_curr_time);
	  string y_str   = m_records[i].getValue("y", m_curr_time);
	  string lat_str = m_records[i].getValue("lat", m_curr_time);
	  string lon_str = m_records[i].getValue("lon", m_curr_time);
	  string spd_str = m_records[i].getValue("speed");
	  string hdg_str = m_records[i].getValue("heading");
	  string dep_str = m_records[i].getValue("depth");
	  string time_str = m_records[i].getValue("time");
	  string name_str = m_records[i].getName();
	  string type_str = m_records[i].getType();
	  
	  new_alerts = true;

	  string var = m_alert_var[j];
	  string msg = m_alert_val[j];
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

	  m_Comms.Notify(var, msg);
	  m_par.setValue(ivname, jalert, true);
	  augmentAlertHistory(var + "=" + msg);
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
  unsigned int i, vsize = m_records.size();
  for(i=0; i<vsize; i++) {

    // First figure out the raw range to the contact
    double cnx = m_records[i].getX(m_curr_time);
    double cny = m_records[i].getY(m_curr_time);
    // Extrapolation from last node report added by henrik
    double cnh = m_records[i].getHeading();
    double cns = m_records[i].getSpeed();
    double cnt = m_records[i].getTimeStamp();
    double ang = (90.0-cnh)*M_PI;
    cnx += cns*(m_curr_time - cnt)*cos(ang);
    cny += cns*(m_curr_time - cnt)*sin(ang);

    double range = hypot((m_nav_x - cnx), (m_nav_y - cny));
    m_ranges[i] = range;

    // If the raw range exceeds the minimum threshold, but is less
    // than the alert_cpa threshold, calculate the CPA and uses this
    // as the range instead.
    if((range > m_alert_range) && (range <= m_alert_cpa_range)) {
      CPAEngine engine(cny, cnx, cnh, cns, m_nav_y, m_nav_x);
      
      double cpa = engine.evalCPA(m_nav_hdg, m_nav_spd, m_alert_cpa_time);
      m_ranges[i] = cpa;
    }
  }
}

//---------------------------------------------------------
// Procedure: postRadii

void BasicContactMgr::postRadii()
{
  string s1 = "radial:: pts=60, snap=1, label=in";
  s1 += ", source=" + m_ownship;
  s1 += ", x=" + doubleToString(m_nav_x,1);
  s1 += ", y=" + doubleToString(m_nav_y,1);
  s1 += ", radius=" + doubleToString(m_alert_range,1);
  s1 += ", edge_color=gray65, vertex_size=0";
    
  m_Comms.Notify("VIEW_POLYGON", s1);

  string s2 = "radial:: pts=60, snap=1, label=out";
  s2 += ", source=" + m_ownship;
  s2 += ", x=" + doubleToString(m_nav_x,1);
  s2 += ", y=" + doubleToString(m_nav_y,1);
  s2 += ", radius=" + doubleToString(m_alert_cpa_range,1);
  s2 += ", edge_color=gray35, vertex_size=0";
    
  m_Comms.Notify("VIEW_POLYGON", s2);
}


//---------------------------------------------------------
// Procedure: postConsoleSummary

void BasicContactMgr::postConsoleSummary()
{
  cout << endl;
  cout << "--------------------- Iteration: " << m_iteration << endl;
  cout << " Time: " << (m_curr_time - m_start_time) << endl;
  cout << "       List: " << m_prev_contacts_list << endl;
  cout << "    Alerted: " << m_prev_contacts_alerted << endl;
  cout << "  UnAlerted: " << m_prev_contacts_unalerted << endl;
  cout << "    Retired: " << m_prev_contacts_retired << endl;
  cout << "      Recap: " << m_prev_contacts_recap << endl;
  cout << endl;

  cout << "Recent Alerts: (" << m_alert_history.size() << ")" << endl;
  list<string>::iterator p;
  for(p=m_alert_history.begin(); p!=m_alert_history.end(); p++) {
    string str = *p;
    cout << str << endl;
  }

  m_par.print();

}

//---------------------------------------------------------
// Procedure: augmentAlertHistory

void BasicContactMgr::augmentAlertHistory(const string& alert) 
{
  double timestamp = m_curr_time - m_start_time;
  string timestr   = "  [" + doubleToString(timestamp,2) + "]: ";
  
  m_alert_history.push_back(timestr+alert);
  if(m_alert_history.size() > 10)
    m_alert_history.pop_front();
}

