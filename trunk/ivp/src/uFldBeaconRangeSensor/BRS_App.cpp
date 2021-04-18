/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BRS_App.cpp                                          */
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
#include "ACTable.h"
#include "BRS_App.h"
#include "MBUtils.h"
#include "XYRangePulse.h"
#include "XYMarker.h"
#include "NodeRecordUtils.h"

using namespace std;

//------------------------------------------------------------
// Constructor

BRS_App::BRS_App()
{
  // Configuration variables
  m_default_node_push_dist = 100;    // meters
  m_default_node_pull_dist = 100;    // meters
  m_default_node_ping_wait = 30;     // seconds

  m_default_beacon_buoy.setPushDist("50");
  m_default_beacon_buoy.setPullDist("50");

  m_ping_payments = "upon_response";
  m_report_vars   = "short";
  m_ground_truth  = true;
  m_verbose       = false;

  // If uniformly random noise used, (m_rn_algorithm = "uniform")
  // this variable reflects the range of the uniform variable in 
  // terms of percentage of the true (pre-noise) range.
  m_rn_uniform_pct = 0;

  m_ac.setMaxEvents(20);
}


//---------------------------------------------------------
// Procedure: OnNewMail

bool BRS_App::OnNewMail(MOOSMSG_LIST &NewMail)
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
    else if(key == "BRS_RANGE_REQUEST")
      handled = handleRangeRequest(sval);
    
    if(!handled)
      reportRunWarning("Unhandled Mail: " + key);
  }
  return(true);
}


//---------------------------------------------------------
// Procedure: OnStartUp()

bool BRS_App::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  map<string, bool> handled;
  
  STRING_LIST sParams;
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams)) 
    reportConfigWarning("No config block found for " + GetAppName());
  
  for(unsigned int pass=0; pass<4; pass++) {
    
    if(pass==3)
      handled["allow_retractions"] = true;

    STRING_LIST::iterator p;
    for(p = sParams.begin(); p!=sParams.end(); p++) {
      string orig  = *p;
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;

      if(pass == 0)
	handled[param] = false;

      if(pass == 1) {
	if(param == "default_beacon_freq")
	  handled[param] = m_default_beacon_buoy.setFrequencyRange(value);
	else if(param == "default_beacon_shape")
	  handled[param] = m_default_beacon_buoy.setShape(value);
	else if(param == "default_beacon_color")
	  handled[param] = m_default_beacon_buoy.setBuoyColor(value);
	else if(param == "default_beacon_width")
	  handled[param] = m_default_beacon_buoy.setWidth(value);
	else if(param == "default_beacon_push_dist")
	  handled[param] = m_default_beacon_buoy.setPushDist(value);	
	else if(param == "default_beacon_pull_dist")
	  handled[param] = m_default_beacon_buoy.setPullDist(value);	
	else if(param == "node_push_dist")
	  handled[param] = setNodePushDistance(value);
	else if(param == "node_pull_dist")
	  handled[param] = setNodePullDistance(value);
	else if(param == "ping_wait")
	  handled[param] = setPingWait(value);
	else if(param == "ping_payments")
	  handled[param] = setPingPayments(value);
	else if(param == "report_vars")
	  handled[param] = setReportVars(value);
	else if(param == "rn_algorithm") 
	  handled[param] = setRandomNoiseAlgorithm(value);
	else if((param == "rn_uniform_pct") && isNumber(value)) {
	  m_rn_uniform_pct = vclip(atof(value.c_str()), 0, 1);
	  handled[param] = true;
	}
	else if(param == "ground_truth")
	  handled[param] = setBooleanOnString(m_ground_truth, value);
	else if(param == "verbose")
	  handled[param] = setBooleanOnString(m_verbose, value);
      }
      else if(pass == 2) {
	if(param == "beacon")
	  handled[param] = addBeaconBuoy(value);
      }
      else if(pass == 3) {
	if(!handled[param])
	  reportUnhandledConfigWarning(orig);
      }
    }
  }
  
  registerVariables();

  postVisuals(); 
  return(true);
}

//------------------------------------------------------------
// Procedure: Iterate()

bool BRS_App::Iterate()
{
  AppCastingMOOSApp::Iterate();

  unsigned int bix, vsize = m_beacons.size();

  // If the first iteration, mark the timestamp for all beacons.
  if(m_iteration == 1) {
    for(bix=0; bix<vsize; bix++)
      m_beacons[bix].setTimeStamp(m_curr_time);
  }

  // Determine which beacons are ready to broadcast a range msg
  // based on their regular interval setting
  for(bix=0; bix<vsize; bix++) {
    // Determine how much time since the beacon last broadcast
    double elapsed_time = m_curr_time - m_beacons[bix].getTimeStamp();
    double freq = m_beacons[bix].getFrequency();
    if((freq > 0) && (elapsed_time > freq)) {
      m_beacons[bix].setTimeStamp(m_curr_time);
      
      // Generate a RangePulse (strictly for visualization)
      double x = m_beacons[bix].getX();
      double y = m_beacons[bix].getY();
      string label = m_beacons[bix].getLabel();
      postRangePulse(x, y, "pink", label, 15, 40);

      // Find the beacon_push_dist for use with each vehicle calculation
      double beacon_push_dist = m_beacons[bix].getPushDist();

      // Regardless if ping is heard from anyone, increment the count of
      // unsolicited pings generated by this beacon.
      m_beacons[bix].incPingsUnsol();

      // For each vehicle, if valid, if in range, post report
      map<string, NodeRecord>::iterator p;
      for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
	string vname = p->first;
	string VNAME = toupper(vname);
	NodeRecord node_record = p->second;
	if(node_record.valid()) {
	  double actual_range = getTrueBeaconNodeRange(bix, vname);
	  double vehicle_pull_dist = m_map_node_pull_dist[vname];

	  if(pingTrans(beacon_push_dist, vehicle_pull_dist, actual_range)) {
	    m_map_node_pings_usol[vname]++;
	    reportEvent("Beacon["+label+ "]  ))-->  Range Broadcast to " + VNAME);
	    postBeaconRangeReport(bix, actual_range, vname);
	  }
	  else {
	    if(m_verbose)
	      reportEvent("Beacon["+label+ "]  XX-->  Failed Range Brdcast to " + VNAME);
	  }	      
	}
      }
    }
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool BRS_App::OnConnectToServer()
{
  registerVariables();  
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void BRS_App::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("NODE_REPORT", 0);
  Register("NODE_REPORT_LOCAL", 0);
  Register("BRS_RANGE_REQUEST", 0);
}

//------------------------------------------------------------
// Procedure: addBeaconBuoy

bool BRS_App::addBeaconBuoy(const string& line)
{
  vector<string> svector = parseString(line, ',');
  unsigned int i, vsize = svector.size();

  BeaconBuoy beacon = m_default_beacon_buoy;
  for(i=0; i<vsize; i++) {
    string left = biteStringX(svector[i], '=');
    string rest = svector[i];
    if((left == "x") && isNumber(rest))
      beacon.setX(atof(rest.c_str()));
    else if((left == "y") && isNumber(rest))
      beacon.setY(atof(rest.c_str()));
    else if(left == "freq")
      beacon.setFrequencyRange(rest);
    else if(left == "push_dist")
      beacon.setPushDist(rest);
    else if(left == "pull_dist")
      beacon.setPullDist(rest);
    else if(left == "label")
      beacon.setLabel(rest);
    else if(left == "color")
      beacon.setBuoyColor(rest);
    else if(left == "shape")
      beacon.setShape(rest);
    else if(left == "width")
      beacon.setWidth(rest);
    else 
      reportConfigWarning("Unknown Beacon Spec Param: " + left);
  }
  
  if(beacon.getLabel() == "")
    beacon.setLabel(uintToString(m_beacons.size()));
  
  // Determine if a beacon with non-null label already exists.
  unsigned int j, jsize = m_beacons.size();
  for(j=0; j<jsize; j++) {
    if(m_beacons[j].getLabel() == beacon.getLabel()) {
      m_beacons[j] = beacon;
      return(true);
    }
  }
  
  m_beacons.push_back(beacon);
  return(true);
}

//---------------------------------------------------------
// Procedure: handleNodeReport
//   Example: NAME=alpha,TYPE=KAYAK,UTC_TIME=1267294386.51,
//            X=29.66,Y=-23.49,LAT=43.825089, LON=-70.330030, 
//            SPD=2.00, HDG=119.06,YAW=119.05677,DEPTH=0.00,     
//            LENGTH=4.0,MODE=ENGAGED

bool BRS_App::handleNodeReport(const string& node_report_str)
{
  NodeRecord new_node_record = string2NodeRecord(node_report_str);

  if(!new_node_record.valid())
    return(false);

  string vname = new_node_record.getName();
  if(vname == "")
    return(false);

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
//   Example: name=alpha

bool BRS_App::handleRangeRequest(const string& request)
{
  string vname = tokStringParse(request, "name", ',', '=');
  string VNAME = toupper(vname);
  
  if((vname == "")  || (m_map_node_records.count(vname) == 0)) {
    reportRunWarning("Failed Range Request: Unknown vehicle["+vname+"]");
    reportEvent("Failed Range Request: Unknown vehicle["+vname+"]");
    return(false);
  }

  m_map_node_pings_gend[vname]++;

  double v_x = m_map_node_records[vname].getX();
  double v_y = m_map_node_records[vname].getY();

  // Determine if this vehicle is allowed to make a range request
  double elapsed_time  = m_curr_time - m_map_node_last_ping[vname];
  double query_freq    = m_map_node_ping_wait[vname];
  bool   query_allowed = (elapsed_time >= query_freq);

  if(m_ping_payments == "upon_request")
    m_map_node_last_ping[vname] = m_curr_time;
  
  if(!query_allowed) {
    reportRunWarning("Failed Range Request from "+VNAME+": exceeds query freq.");
    if(m_verbose) {
      reportEvent("Failed Range Request from "+VNAME+": exceeds query frequency.");
      reportEvent("  Elapsed time for "+VNAME+": " + doubleToString(elapsed_time));
    }
    m_map_node_xping_freq[vname]++;
    return(true);
  }
  
  reportEvent("Valid Range Request from "+VNAME+". Checking beacon ranges...");

  if(m_ping_payments == "upon_accept") {
    if(m_verbose)
      reportEvent("  Range Request resets the clock for vehicle " +VNAME);
    m_map_node_last_ping[vname] = m_curr_time;
  }
  
  // Build the RangePulse for the requesting vehicle
  string vlabel = vname + "_range_req";
  postRangePulse(v_x, v_y, "white", vlabel, 6, 40);

  // Phase 1: Handle range reports to fixed beacons
  double node_push_dist = m_map_node_push_dist[vname];
  double node_pull_dist = m_map_node_pull_dist[vname];
  
  // For each beacon, determine if the beacon is within range to the
  // requesting vehicle to have received the request.
  unsigned int bix, bsize = m_beacons.size();
  for(bix=0; bix<bsize; bix++) {
    double actual_range_now = getTrueBeaconNodeRange(bix, vname);
    bool   ping_replied     = false;
    double beacon_pull_dist = m_beacons[bix].getPullDist();

    if(pingTrans(node_push_dist, beacon_pull_dist, actual_range_now)) {
      m_beacons[bix].incPingsReceived();
      string label = m_beacons[bix].getLabel();
      reportEvent("Beacon["+label+ "]  <----  Ping from " + VNAME);

      // Then determine if the vehicle is close enough to hear the beacon
      double beacon_push_dist = m_beacons[bix].getPushDist();
      if(pingTrans(beacon_push_dist, node_pull_dist, actual_range_now)) {
	m_beacons[bix].incPingsReplied();
	m_map_node_pings_repl[vname]++;
	double x = m_beacons[bix].getX();
	double y = m_beacons[bix].getY();
	if(m_ping_payments == "upon_response")
	  m_map_node_last_ping[vname] = m_curr_time;
	reportEvent("Beacon["+label+ "]    -->  Range Reply to " + VNAME);
	postBeaconRangeReport(bix, actual_range_now, vname);
	postRangePulse(x, y, "pink", label, 15, 40);
	m_beacons[bix].setTimeStamp(m_curr_time);
	ping_replied = true;
      }
    }
    if(!ping_replied)
      m_map_node_xping_dist[vname]++;
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: postVisuals()

void BRS_App::postVisuals() 
{
  unsigned int i, vsize = m_beacons.size();
  for(i=0; i<vsize; i++) {
    XYMarker marker;
    marker.set_vx(m_beacons[i].getX());
    marker.set_vy(m_beacons[i].getY());
    marker.set_width(m_beacons[i].getWidth());
    marker.set_type(m_beacons[i].getShape());
    marker.set_active(true);
    marker.set_color("primary_color", m_beacons[i].getBuoyColor());
    string spec = marker.get_spec(); 
    Notify("VIEW_MARKER", spec);
  }
}


//------------------------------------------------------------
// Procedure: postBeaconRangePulse

void BRS_App::postBeaconRangePulse(unsigned int ix, string color)
{
  if(ix >= m_beacons.size())
    return;

  XYRangePulse pulse;
  pulse.set_x(m_beacons[ix].getX());
  pulse.set_y(m_beacons[ix].getY());
  pulse.set_label(m_beacons[ix].getLabel());
  pulse.set_rad(40);  
  pulse.set_duration(15);
  pulse.set_time(m_curr_time);
  if(color != "") {
    pulse.set_color("edge", color);
    pulse.set_color("fill", color);
  }
  string spec = pulse.get_spec();
  Notify("VIEW_RANGE_PULSE", spec);
}

//------------------------------------------------------------
// Procedure: postBeaconRangeReport()

void BRS_App::postBeaconRangeReport(unsigned int beacon_ix, 
				      double actual_range, 
				      string vname)
{
  if(beacon_ix >= m_beacons.size())
    return;
  
  string label = m_beacons[beacon_ix].getLabel();

  double report_range = actual_range;
  if(m_rn_algorithm != "")
    report_range  = getNoisyBeaconNodeRange(actual_range);

  // Phase 1: Post the "non-ground-truth" reports
  string str = "range=" + doubleToStringX(report_range, 4);
  str += ",id=" + label;
  str += ",time=" + doubleToStringX(m_curr_time,3);

  if((m_report_vars == "short") || (m_report_vars == "both")) {
    string full_str = "vname=" + vname + "," + str;
    Notify("BRS_RANGE_REPORT", full_str);
  }

  if((m_report_vars == "long") || (m_report_vars == "both")) {
    vname = toupper(vname);
    Notify("BRS_RANGE_REPORT_"+vname, str);
  }

  // Phase 2: Possibly Post the "ground-truth" reports
  if((m_rn_algorithm != "") && m_ground_truth) {
    string str = "range=" + doubleToStringX(actual_range, 4);
    str += ",id=" + label;
    str += ",time=" + doubleToStringX(m_curr_time,3);
    
    if((m_report_vars == "short") || (m_report_vars == "both")) {
      string full_str = "vname=" + vname + "," + str;
      Notify("BRS_RANGE_REPORT_GT", full_str);
    }
    
    if((m_report_vars == "long") || (m_report_vars == "both")) {
      vname = toupper(vname);
      Notify("BRS_RANGE_REPORT_GT_"+vname, str);
    }
  }

}


//------------------------------------------------------------
// Procedure: getTrueBeaconNodeRange()

double BRS_App::getTrueBeaconNodeRange(unsigned int beacon_ix, 
				       string vname) 
{
  if(beacon_ix >= m_beacons.size())
    return(-1);
  if(m_map_node_records.count(vname) == 0)
    return(-1);

  double beacon_x = m_beacons[beacon_ix].getX();
  double beacon_y = m_beacons[beacon_ix].getY();
  double vnode_x  = m_map_node_records[vname].getX();
  double vnode_y  = m_map_node_records[vname].getY();
  double range    = hypot((beacon_x-vnode_x), (beacon_y-vnode_y));

  return(range);
}


//------------------------------------------------------------
// Procedure: getNoisyBeaconNodeRange()

double BRS_App::getNoisyBeaconNodeRange(double true_range) const
{
  if(m_rn_algorithm == "uniform") {
    // Generate a random double in the range [-1, 1]
    int    rand_int = rand() % 10000;
    double rand_pct = ((double)(rand_int) / 5000) - 1;
    
    double noise = ((rand_pct * m_rn_uniform_pct) * true_range);
    double noisy_range = true_range + noise;
    return(noisy_range);
  }
    
  return(true_range);
}


//------------------------------------------------------------
// Procedure: setNodePushDistance
//      Note: Negative values indicate infinity

bool BRS_App::setNodePushDistance(string str)
{
  string vname = biteStringX(str, '=');
  string right = tolower(str); 

  double distance = 0;
  if((right == "nolimit") || (right == "unlimited"))
    distance = -1;
  else if(isNumber(right))
    distance = atof(right.c_str());
  else
    return(false);
  
  if(tolower(vname) == "default")
    m_default_node_push_dist = distance;
  else
    m_map_node_push_dist[vname] = distance;
  
  return(true);
}

//------------------------------------------------------------
// Procedure: setNodePullDistance
//      Note: Negative values indicate infinity

bool BRS_App::setNodePullDistance(string str)
{
  string vname = biteStringX(str, '=');
  string right = tolower(str); 

  double distance = 0;
  if((right == "nolimit") || (right == "unlimited"))
    distance = -1;
  else if(isNumber(right))
    distance = atof(right.c_str());
  else
    return(false);
  
  if(tolower(vname) == "default")
    m_default_node_pull_dist = distance;
  else
    m_map_node_pull_dist[vname] = distance;
  
  return(true);
}


//------------------------------------------------------------
// Procedure: setPingWait
//      Note: Negative values no wait time mandated

bool BRS_App::setPingWait(string str)
{
  string left  = biteStringX(str, '=');
  string right = tolower(str); 

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
// Procedure: setPingPayments

bool BRS_App::setPingPayments(string str)
{
  string val = tolower(str);
  if((val == "upon_response") || 
     (val == "upon_request") ||
     (val == "upon_accept")) {
    m_ping_payments = val;
    return(true);
  }
  else
    return(false);
}

//------------------------------------------------------------
// Procedure: setReportVars

bool BRS_App::setReportVars(string str)
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

bool BRS_App::setRandomNoiseAlgorithm(string str)
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
// Procedure: postRangePulse

void BRS_App::postRangePulse(double x, double y, string color,
			       string label, double duration,
			       double radius)
{
  XYRangePulse pulse;
  pulse.set_x(x);
  pulse.set_y(y);
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
  Notify("VIEW_RANGE_PULSE", spec);
}

//------------------------------------------------------------
// Procedure: pingTrans
//      Note: A negative push/pull distance means infinity

bool BRS_App::pingTrans(double push, double pull, double threshold)
{
  if((push < 0) || (pull < 0))
    return(true);

  return((push + pull) >= threshold);
}

//------------------------------------------------------------
// Procedure: buildReport()
//      Note: A virtual function of the AppCastingMOOSApp superclass, 
//            conditionally invoked if either a terminal or appcast 
//            report is needed.
//  
//   Beacons(3):
//                                 Pings  Pings    
//   ID      X       Y      Freq   Recvd  Gen'd  Noise  Range
//   -----   -----   -----  -----  -----  -----  -----  -----
//   01      45      -1123  n/a    124    87     3%     250    
//   02      115     -850   30     n/a    112    11%    314    
//   03      82      -123   20:45  n/a    43     2%     280    
//   
//   Contacts(4): 
//              Push   Pull   Unsol  Pings  Pings  Too   Too
//   Name       Range  Range  Rec'd  Gen'd  Rep'd  Freq  Far
//   ------     -----  -----  -----  -----  -----  ----  ---
//   archie     100    300     4     23     14     9     0
//   betty      any    any     0     13     11     1     1
//   charlie    90     244     9     16     3      12    1  
//   davis      250    195     2     33     17     6     10
//
//   Events (Last 8):
//   --------------------
//   201.0   Beacon 02  ))--> Range Broadcast to BETTY
//   201.0   Beacon 02  ))--> Range Broadcast to DAVIS
//   201.0   Beacon 02  ))--> Range Broadcast to ARCHIE

//   191.1   Beacon 03   -->  Range Reply to CHARLIE
//   198.2   Beacon 01  <---- Ping from CHARLIE
//   198.2   Beacon 03  <---- Ping from CHARLIE

bool BRS_App::buildReport()
{
  // Part 1: Build a report on the Beacons
  m_msgs << "Beacons(" << (unsigned int)(m_beacons.size()) << "):" << endl;

  ACTable actab(10);
  actab.setColumnJustify(1, "right");
  actab.setColumnJustify(2, "right");
  actab << "   |   |   |      | Pings | Pings | Pings |       | Push | Pull ";
  actab << "ID | X | Y | Freq | Unsol | Recvd | Gen'd | Noise | Dist | Dist";
  actab.addHeaderLines();

  unsigned int i, vsize = m_beacons.size();
  for(i=0; i<vsize; i++) {
    string label = m_beacons[i].getLabel();
    string sxpos = doubleToStringX(m_beacons[i].getX(), 1);
    string sypos = doubleToStringX(m_beacons[i].getY(), 1);
    string sfreq = m_beacons[i].getFreqSetting();
    string upngs = uintToString(m_beacons[i].getPingsUnsol());
    string rpngs = uintToString(m_beacons[i].getPingsReceived());
    string gpngs = uintToString(m_beacons[i].getPingsReplied());
    string noise = "0";
    string pushd = doubleToStringX(m_beacons[i].getPushDist(), 1);
    string pulld = doubleToStringX(m_beacons[i].getPullDist(), 1);
    actab << label << sxpos << sypos << sfreq << upngs << rpngs << gpngs;
    actab << noise << pushd << pulld;
  }
  m_msgs << actab.getFormattedString();
  m_msgs << endl << endl;

  // Part 2: Build a report on the Vehicles
  m_msgs << "Vehicles(" << m_map_node_records.size() << "):" << endl;

  actab = ACTable(8);
  actab << "      | Push | Pull | Unsol | Pings | Pings | Too  | Too ";
  actab << "VName | Dist | Dist | Rec'd | Gen'd | Rep'd | Freq | Far ";
  actab.addHeaderLines();

  map<string, NodeRecord>::iterator p;

  for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
    string vname = p->first;

    double push_dist = m_map_node_push_dist[vname];
    string spush = "inf";
    if(push_dist >= 0)
      spush = doubleToStringX(push_dist,1);

    double pull_dist = m_map_node_pull_dist[vname];
    string spull = "inf";
    if(pull_dist >= 0)
      spull = doubleToStringX(pull_dist,1);
    
    string pusol = uintToString(m_map_node_pings_usol[vname]);
    string pgend = uintToString(m_map_node_pings_gend[vname]);
    string prepl = uintToString(m_map_node_pings_repl[vname]);
    string toofr = uintToString(m_map_node_xping_freq[vname]);
    string toofa = uintToString(m_map_node_xping_dist[vname]);
    actab << vname << spush << spull << pusol << pgend << prepl << toofr << toofa;
  }
  m_msgs << actab.getFormattedString();

  return(true);
}









