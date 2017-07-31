/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CRS_App.cpp                                          */
/*    DATE: Feb 2nd, 2011                                        */
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

#include <cstdlib>
#include <iostream>
#include <cmath>
#include "AngleUtils.h"
#include "CRS_App_HP.h"
#include "MBUtils.h"
#include "ACTable.h"
#include "NodeRecordUtils.h"


using namespace std;

//------------------------------------------------------------
// Constructor

CRS_App::CRS_App()
{
  // Configuration variables
  m_default_node_push_dist = 100;    // meters
  m_default_node_pull_dist = 100;    // meters
  m_default_node_ping_wait = 30;    // seconds

  m_ping_color   = "white";
  m_echo_color   = "chartreuse";
  m_report_vars  = "short";
  m_ground_truth = true;

  // If uniformly random noise used, (m_rn_algorithm = "uniform")
  // this variable reflects the range of the uniform variable in 
  // terms of percentage of the true (pre-noise) range.
  m_rn_uniform_pct = 0;

  // If gaussian random noise used, (m_rn_algorithm = "gauss")
  // this variable reflects the sigma in terms of absolute meters.
  m_rn_gaussian_sigma = 0;

  // Added by ALon Yaari Jan 2013
  m_display_range_pulse = true;
}
//---------------------------------------------------------
// Procedure: OnNewMail

bool CRS_App::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    
    string key  = msg.GetKey();
    string sval = msg.GetString();

    bool handled = false;
    if((key == "NODE_REPORT") || (key == "NODE_REPORT_LOCAL"))
      handled = handleNodeReport(sval);
    else if(key == "CRS_RANGE_REQUEST")
      handled = handleRangeRequest(sval);
    else if(key == "APPCAST_REQ")
      handled = true;

    if(!handled)
      reportRunWarning("Unhandled Mail: " + key);
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool CRS_App::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  
  STRING_LIST::iterator p;
  for(p = sParams.begin(); p!=sParams.end(); p++) {
    string sLine  = *p;
    string origl  = *p;
    string param  = tolower(biteStringX(sLine, '='));
    string value  = sLine;
    
    bool handled = true;
    if((param == "push_dist") || (param == "push_distance"))
      handled = setPushDistance(value);
    else if((param == "pull_dist") || (param == "pull_distance"))
      handled = setPullDistance(value);
    else if(param == "ping_wait")
      handled = setPingWait(value);
    else if(param == "report_vars")
      handled = setReportVars(value);
    else if(param == "allow_echo_types")
      handled = setAllowableEchoTypes(value);
    else if((param == "ping_color") && isColor(value))
      m_ping_color = value;
    else if((param == "echo_color") && isColor(value))
      m_echo_color = value;
    else if(param == "reply_color") {
      reportConfigWarning("reply_color deprecated. Use echo_color instead.");
      if(isColor(value))
	m_echo_color = value;
    }
    else if(param == "rn_algorithm")
      handled = setRandomNoiseAlgorithm(value);
    else if(param == "display_pulses")
      handled = setBooleanOnString(m_display_range_pulse, value);
    else if(param == "ground_truth")
      handled = setBooleanOnString(m_ground_truth, value);
    else if((param == "rn_uniform_pct") && isNumber(value))
      m_rn_uniform_pct = vclip(atof(value.c_str()), 0, 1);
    else if((param == "rn_gaussian_sigma") && isNumber(value))
      m_rn_gaussian_sigma = vclip_min(atof(value.c_str()), 0);
    else if(param == "sensor_arc") 
      handled = setSensorArc(value);
    else if((param != "apptick") && (param != "commstick"))
      handled = false;

    if(!handled)
      reportConfigWarning("Unhandled config: " + origl);
  }

  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: Iterate()

bool CRS_App::Iterate()
{
  AppCastingMOOSApp::Iterate();
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool CRS_App::OnConnectToServer()
{
  registerVariables();  
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void CRS_App::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  m_Comms.Register("NODE_REPORT", 0);
  m_Comms.Register("NODE_REPORT_LOCAL", 0);
  m_Comms.Register("CRS_RANGE_REQUEST", 0);
}

//---------------------------------------------------------
// Procedure: handleNodeReport
//   Example: NAME=alpha,TYPE=KAYAK,UTC_TIME=1267294386.51,
//            X=29.66,Y=-23.49,LAT=43.825089, LON=-70.330030,
//            SPD=2.00, HDG=119.06,YAW=119.05677,DEPTH=0.00,   
//            LENGTH=4.0,MODE=ENGAGED

bool CRS_App::handleNodeReport(const string& node_report_str)
{
  NodeRecord new_node_record = string2NodeRecord(node_report_str);

  if(!new_node_record.valid())
    return(false);

  string vname = new_node_record.getName();
  if(vname == "") {
    reportRunWarning("Unhandled NODE_REPORT. Missing Vehicle/Node name.");
    return(false);
  }

  m_map_node_records[vname] = new_node_record;
  m_map_node_reps_recd[vname]++;

  // If node push_dist not pre-configured, set to node default
  if(m_map_node_push_dist.count(vname) == 0)
    m_map_node_push_dist[vname] = m_default_node_push_dist;
  // If node pull_dist not pre-configured, set to node default
  if(m_map_node_pull_dist.count(vname) == 0)
    m_map_node_pull_dist[vname] = m_default_node_pull_dist;
  // If node ping_wait time not pre-configured, set to node default
  if(m_map_node_ping_wait.count(vname) == 0)
    m_map_node_ping_wait[vname] = m_default_node_ping_wait;

  return(true);
}

//---------------------------------------------------------
// Procedure: handleRangeRequest
//   Example: vname=alpha

bool CRS_App::handleRangeRequest(const string& request)
{
  string vname = tokStringParse(request, "name", ',', '=');
  string VNAME = toupper(vname);
  
  // Phase 1: Confirm this request is coming from a known vehicle.
  if((vname == "")  || (m_map_node_records.count(vname) == 0)) {
    reportRunWarning("Failed Range Request: Unknown vehicle["+vname+"]");
    reportEvent("Failed Range Request: Unknown vehicle["+vname+"]");
    return(false);
  }

  m_map_node_pings_gend[vname]++;

  // Phase 2: Determine if this vehicle is allowed to generate a ping
  // based on the last time it made a ping request and the ping wait 
  // time associated with the vehicle.
  
  double elapsed_time  = m_curr_time - m_map_node_last_ping[vname];
  double query_freq    = m_map_node_ping_wait[vname];
  bool   query_allowed = (elapsed_time >= query_freq);

  if(!query_allowed) {
    string msg = VNAME + "  ----))  XXX too frequent. ";
    msg += "(" + doubleToString(elapsed_time, 2) + ")";
    reportEvent(msg);
    m_map_node_xping_freq[vname]++;
    return(true);
  }
  reportEvent(VNAME + "  ----))  ");
  
  m_map_node_last_ping[vname] = m_curr_time;
  
  // Phase 3: Post the RangePulse for the requesting vehicle. This is
  // purely a visual artifact.
  postRangePulse(vname, m_ping_color, vname+"_ping", 6, 50);
  XYArc Arc = XYArc(m_map_node_records[vname].getX(),
		    m_map_node_records[vname].getY(),6,45,135);

  // Phase 4: Handle range reports to target nodes. For each target, 
  // determine if the target is within range to the vehicle making the
  // range request. If so generate the range report and visual pulse.
  
  double push_dist = m_map_node_push_dist[vname];
  map<string, NodeRecord>::iterator p;
  for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
    string contact_name = p->first;
    string contact_type = tolower(p->second.getType());
    
    // Check if the contact type is allowed to be pinged on.
    bool allowed_type = allowableEchoType(contact_type);
    if(allowed_type && (vname != contact_name)) {
    
      double actual_range_now = getTrueNodeNodeRange(vname, contact_name);
      double actual_heading_now = getTrueNodeHeading(vname);
      double actual_bearing_now =  getTrueNodeNodeBearing(vname, contact_name);
      double pull_dist = m_map_node_pull_dist[contact_name];

      // Distance should be less the pull_dist + push_dist
      bool ping_dist = false;
      double max_range = push_dist + pull_dist;
      if((push_dist < 0) || (pull_dist < 0))
	ping_dist = true;
      else if((max_range) > actual_range_now)
	ping_dist = true;
      else {
	// Generate a random double in the range [0, 1]
	int    rand_int = rand() % 10000;
	double rand_pct = ((double)(rand_int) / 10000);
	// Probabilty of detection beyond max sensor range diminishes
	// exponentially
	double prob_detection = exp((max_range - actual_range_now)*3/max_range);
	if(rand_pct <= prob_detection)
	  ping_dist = true;
      }

      // Bearing should only be between allowable bearings set by user
      bool ping_bearing = false;
      double rel_bearing = actual_bearing_now - actual_heading_now;
      if (rel_bearing >= 360) rel_bearing -= 360;
      if (rel_bearing < 0) rel_bearing += 360;

      if (m_left_arcs.size() == 0)
	ping_bearing = true; 
      else
	for (unsigned int i=0; i<m_left_arcs.size(); i++)
	  if (m_left_arcs[i] < m_right_arcs[i]) {
	    if (rel_bearing >= m_left_arcs[i] && rel_bearing <= m_right_arcs[i])   
	      ping_bearing = true;
	  }
	  else
	    if (rel_bearing >= m_left_arcs[i] || rel_bearing <= m_right_arcs[i])
	      ping_bearing = true;
      
      if(ping_dist && ping_bearing) {
	m_map_node_echos_recd[vname]++;
	m_map_node_echos_sent[contact_name]++;
	postNodeRangeReport(vname, contact_name, actual_range_now);
	string label = contact_name + "_echo";
	postRangePulse(contact_name, m_echo_color, label, 15, 40);
      }
      else
	m_map_node_xping_dist[vname]++;
    }
  }

  return(true);
}

//------------------------------------------------------------
// Procedure: postRangePulse

void CRS_App::postRangePulse(const string& node, const string& color,
			     const string& label, double duration,
			     double radius)
{
  if(m_map_node_records.count(node) == 0)
    return;
  if(!m_display_range_pulse)
	return;

  XYRangePulse pulse;
  pulse.set_x(m_map_node_records[node].getX());
  pulse.set_y(m_map_node_records[node].getY());
  pulse.set_label(label);
  pulse.set_rad(radius);  
  pulse.set_fill(0.9);
  pulse.set_duration(duration);
  pulse.set_time(m_curr_time);
  if(color != "") {
    pulse.set_color("edge", color);
    pulse.set_color("fill", color);
  }
  string spec = pulse.get_spec();
  m_Comms.Notify("VIEW_RANGE_PULSE", spec);
}

//------------------------------------------------------------
// Procedure: postNodeRangeReport()

void CRS_App::postNodeRangeReport(const string& rec_name,
				    const string& tar_name, 
				    double actual_range)
{
  if((m_map_node_records.count(rec_name) == 0) ||
     (m_map_node_records.count(tar_name) == 0))
    return;

  
  reportEvent(toupper(rec_name) + "  <--   " + toupper(tar_name));

  double report_range = actual_range;
  if(m_rn_algorithm != "")
    report_range = getNoisyNodeNodeRange(actual_range);

  // Phase 1: Post the "non-ground-truth" reports
  string str = "range=" + doubleToStringX(report_range, 4);
  str += ",target=" + tar_name;
  str += ",time=" + doubleToStringX(m_curr_time,3);

  if((m_report_vars == "short") || (m_report_vars == "both")) {
    string full_str = "vname=" + rec_name + "," + str;
    m_Comms.Notify("CRS_RANGE_REPORT", full_str);
  }

  if((m_report_vars == "long") || (m_report_vars == "both"))
    m_Comms.Notify("CRS_RANGE_REPORT_" + toupper(rec_name), str);

  // Phase 2: Possibly Post the "ground-truth" reports
  if((m_rn_algorithm != "") && m_ground_truth) {
    string str = "range=" + doubleToStringX(actual_range, 4);
    str += ",target=" + tar_name;
    str += ",time=" + doubleToStringX(m_curr_time,3);
    
    if((m_report_vars == "short") || (m_report_vars == "both")) {
      string full_str = "vname=" + rec_name + "," + str;
      m_Comms.Notify("CRS_RANGE_REPORT_GT", full_str);
    }
    
    if((m_report_vars == "long") || (m_report_vars == "both"))
      m_Comms.Notify("CRS_RANGE_REPORT_GT_" + toupper(rec_name), str);
  }

}

//------------------------------------------------------------
// Procedure: getTrueNodeNodeRange()

double CRS_App::getTrueNodeNodeRange(const string& node_a,
				       const string& node_b)
{
  if((m_map_node_records.count(node_a) == 0) ||
     (m_map_node_records.count(node_b) == 0))
    return(-1);

  double anode_x = m_map_node_records[node_a].getX();
  double anode_y = m_map_node_records[node_a].getY();
  double bnode_x = m_map_node_records[node_b].getX();
  double bnode_y = m_map_node_records[node_b].getY();
  double range = hypot((anode_x-bnode_x), (anode_y-bnode_y));

  return(range);
}

//------------------------------------------------------------
// Procedure: getTrueNodeHeading()

double CRS_App::getTrueNodeHeading(const string& vname)
{
  if(m_map_node_records.count(vname) == 0)
    return(-1);

  double heading = m_map_node_records[vname].getHeading();

  return(heading);
}

//------------------------------------------------------------
// Procedure: getTrueNodeNodeBearing()

double CRS_App::getTrueNodeNodeBearing(const string& node_a,
				       const string& node_b)
{
  if((m_map_node_records.count(node_a) == 0) ||
     (m_map_node_records.count(node_b) == 0))
    return(-1);

  double anode_x = m_map_node_records[node_a].getX();
  double anode_y = m_map_node_records[node_a].getY();
  double bnode_x = m_map_node_records[node_b].getX();
  double bnode_y = m_map_node_records[node_b].getY();
  double bearing = relAng(anode_x,anode_y,bnode_x,bnode_y);

  return(bearing);
}

//------------------------------------------------------------
// Procedure: getNoisyNodeNodeRange()

double CRS_App::getNoisyNodeNodeRange(double true_range) const
{
  if(m_rn_algorithm == "uniform") {
    // Generate a random double in the range [-1, 1]
    int    rand_int = rand() % 10000;
    double rand_pct = ((double)(rand_int) / 5000) - 1;
    
    double noise = ((rand_pct * m_rn_uniform_pct) * true_range);
    double noisy_range = true_range + noise;
    return(noisy_range);
  }
  else if(m_rn_algorithm == "gaussian") {
    double noise = MOOSWhiteNoise(m_rn_gaussian_sigma);
    double noisy_range = true_range + noise;
    return(noisy_range);
  }
  
  return(true_range);
}

//------------------------------------------------------------
// Procedure: setPushDistance
//      Note: Negative values indicate infinity

bool CRS_App::setPushDistance(string str)
{
  string left  = biteStringX(str, '=');
  string right = tolower(str); 

  double distance = 0;
  if((right == "nolimit") || (right == "unlimited"))
    distance = -1;
  else if(isNumber(right))
    distance = atof(right.c_str());
  else
    return(false);

  if(tolower(left) == "default")
    m_default_node_push_dist = distance;
  else
    m_map_node_push_dist[left] = distance;
  
  return(true);
}

//------------------------------------------------------------
// Procedure: setPullDistance
//      Note: Negative values indicate infinity

bool CRS_App::setPullDistance(string str)
{
  string left  = biteStringX(str, '=');
  string right = tolower(str); 

  double distance = 0;
  if((right == "nolimit") || (right == "unlimited"))
    distance = -1;
  else if(isNumber(right))
    distance = atof(right.c_str());
  else
    return(false);

  if(tolower(left) == "default")
    m_default_node_pull_dist = distance;
  else
    m_map_node_pull_dist[left] = distance;
  
  return(true);
}

//------------------------------------------------------------
// Procedure: setPingWait
//      Note: Negative values no wait time mandated

bool CRS_App::setPingWait(string str)
{
  string left  = biteStringX(str, '=');
  string right = stripQuotes(tolower(str));

  double wait_time = 0;
  if((right == "nolimit") || (right == "unlimited"))
    wait_time = 0;
  else if(isNumber(right))
    wait_time = atof(right.c_str());
  else
    return(false);

  if(tolower(left) == "default")
    m_default_node_ping_wait = wait_time;
  else
    m_map_node_ping_wait[left] = wait_time;
  
  return(true);
}

//------------------------------------------------------------
// Procedure: setReportVars

bool CRS_App::setReportVars(string str)
{
  string val = tolower(str);
  if((val == "both") || (val == "short") || (val == "long")) {
    m_report_vars = val;
    return(true);
  }
  else
    return(false);
}

//------------------------------------------------------------
// Procedure: setRandomNoiseAlgorithm

bool CRS_App::setRandomNoiseAlgorithm(string str)
{
  string algorithm = biteStringX(str, ',');
  string parameters = str;

  if(algorithm == "uniform") {
    vector<string> svector = parseString(parameters, ',');
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++) {
      string param = biteStringX(svector[i], '=');
      string value = svector[i];
      if(param == "pct") 
	m_rn_uniform_pct = vclip(atof(value.c_str()), 0, 1);
    }
  }
  else
    return(false);
  
  m_rn_algorithm = algorithm;
  return(true);
}

//------------------------------------------------------------
// Procedure: setAllowableEchoTypes

bool CRS_App::setAllowableEchoTypes(string str)
{
  vector<string> svector = parseString(str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string vtype = tolower(stripBlankEnds(svector[i]));

    // Unknown types *are* allowed to be added, but warning is gen'ed.
    if(!isKnownVehicleType(vtype))
      reportConfigWarning("Allowable-Echo-Type, unknown type: " + vtype);

    // Just don't add if it's already been added.
    if(vectorContains(m_allow_echo_types, vtype))
      reportConfigWarning("Allowable-Echo-Type declare twice: " + vtype);
    else
      m_allow_echo_types.push_back(vtype);
  }      

  return(true);
}

//------------------------------------------------------------
// Procedure: allowableEchoType
//   Purpose: check if the contact type is allowed to be pinged on.

bool CRS_App::allowableEchoType(string vehicle_type)
{
  string vtype = tolower(vehicle_type);

  // If no list is provided, then all types are allowed
  if(m_allow_echo_types.size() == 0)
    return(true);

  // If there is indeed a non-empty list of allowed types, check if
  // the given type is on the list.
  if(vectorContains(m_allow_echo_types, vtype))
    return(true);

  // Otherwise....
  return(false);
}

//------------------------------------------------------------
// Procedure: setSensorArc
//   Purpose: set the sensor arc positions
//      Note: the default if no value is provided is a full sensor arc

bool CRS_App::setSensorArc(string str)
{

  vector<string> svector = parseString(str, ',');

  if (str != "360")
    for(unsigned int i=0; i<svector.size(); i++) {
      string left  = biteString(svector[i], ':');
      string right = svector[i];

      if (isNumber(left) && isNumber(right)) {
	m_left_arcs.push_back(atof(left.c_str()));
	m_right_arcs.push_back(atof(right.c_str()));
      }
      else
	return(false);
    }
  
  return(true);
}

  
//------------------------------------------------------------
// Procedure: buildReport()

//   Contacts(4): 
//              Ping   Push   Pull   Pings  Echos  Too   Too  Echos
//   Name       Wait   Range  Range  Gen'd  Rec'd  Freq  Far  Sent
//   ------     -----  -----  -----  -----  ----   ----  ---  -----
//   archie     25     100    300    23     14     9     0    12
//   betty      0      100    any    13     11     1     1    9
//   charlie    60     100    244    16     3      12    1    12
//   davis      60     100    195    33     17     6     10   16
//
//   Events (Last 8):
//   --------------------
//   201.0   CHARLIE ----))
//   201.0   CHARLIE  <--   DAVIS
//   201.0   CHARLIE  <--   BETTY
//   .....
//   198.2   BETTY   ----)) 
//   198.2   DAVIS   <--     BETTY
//   198.2   CHARLIE <--     BETTY


bool CRS_App::buildReport()
{
  m_msgs << "Default Ping Wait:        " << m_default_node_ping_wait << endl;
  m_msgs << "Random Noise Algorithm:   " << m_rn_algorithm           << endl;
  m_msgs << "Random Noise Uniform Pct: " << m_rn_uniform_pct         << endl;
  m_msgs << "Gaussian Noise:           " << m_rn_gaussian_sigma      << endl;
  m_msgs << "Ping Color:               " << m_ping_color             << endl;
  m_msgs << "Echo Color:               " << m_echo_color             << endl;
  m_msgs << "Ground Truth Reporting:   " << boolToString(m_ground_truth) << endl;

  // Part 2: Build a report on the Vehicles
  m_msgs << endl;
  m_msgs << "Vehicles(" << m_map_node_records.size() << "):" << endl;

  ACTable actab(9);
  actab << "      | Ping | Push | Pull | Pings | Echos | Too  | Too | Echos";
  actab << "VName | Wait | Dist | Dist | Gen'd | Rec'd | Freq | Far | Sent";
  actab.addHeaderLines();

  map<string, NodeRecord>::iterator p;
  for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
    string vname = p->first;
    
    string pwait = doubleToStringX(m_map_node_ping_wait[vname],1);
    
    double push_dist = m_map_node_push_dist[vname];
    string spush = "inf";
    if(push_dist >= 0)
      spush = doubleToStringX(push_dist,1);

    double pull_dist = m_map_node_pull_dist[vname];
    string spull = "inf";
    if(pull_dist >= 0)
      spull = doubleToStringX(pull_dist,1);
    
    string pgend = uintToString(m_map_node_pings_gend[vname]);
    string erecd = uintToString(m_map_node_echos_recd[vname]);
    string toofr = uintToString(m_map_node_xping_freq[vname]);
    string toofa = uintToString(m_map_node_xping_dist[vname]);
    string esent = uintToString(m_map_node_echos_sent[vname]);
    actab << vname << pwait << spush << spull  << pgend << erecd;
    actab << toofr << toofa << esent;
  }
  m_msgs << actab.getFormattedString();
  return(true);
}







