/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BRS_Model.cpp                                        */
/*    DATE: Feb 2nd, 2011                                        */
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

#include <cstdlib>
#include <iostream>
#include <cmath>
#include "BRS_Model.h"
#include "MBUtils.h"
#include "XYRangePulse.h"
#include "XYMarker.h"
#include "NodeRecordUtils.h"

using namespace std;

//------------------------------------------------------------
// Constructor

BRS_Model::BRS_Model()
{
  // Configuration variables
  m_default_reach_dist  = 200;    // meters
  m_default_ping_wait   = 30;     // seconds

  m_ping_payments = "upon_response";
  m_report_vars = "short";
  m_verbose = true;
  m_ground_truth = true;

  // State Variables
  m_curr_time  = 0;
  m_iterations = 0;

  // If uniformly random noise used, (m_rn_algorithm = "uniform")
  // this variable reflects the range of the uniform variable in 
  // terms of percentage of the true (pre-noise) range.
  m_rn_uniform_pct = 0;
}

//------------------------------------------------------------
// Procedure: setParam

bool BRS_Model::setParam(string param, string value, unsigned int pass)
{
  param = tolower(param);

  if(pass == 0) {
    if(param == "verbose")
      return(setBooleanOnString(m_verbose, value));
    else
      return(false);
  }
  else if(pass == 1) {
    if(param == "default_beacon_freq")
      return(m_default_beacon_buoy.setFrequencyRange(value));
    else if(param == "default_beacon_shape")
      return(m_default_beacon_buoy.setShape(value));
    else if(param == "default_beacon_color")
      return(m_default_beacon_buoy.setBuoyColor(value));
    else if(param == "default_beacon_width")
      return(m_default_beacon_buoy.setWidth(value));
    else if(param == "default_beacon_report_range")
      return(m_default_beacon_buoy.setReportRange(value));

    else if((param == "reach_distance") || (param == "reach_dist"))
      return(setReachDistance(value));
    else if(param == "ping_wait")
      return(setPingWait(value));
    else if(param == "ping_payments")
      return(setPingPayments(value));
    else if(param == "report_vars")
      return(setReportVars(value));
    else if(param == "rn_algorithm") 
      return(setRandomNoiseAlgorithm(value));
    else if((param == "rn_uniform_pct") && isNumber(value))
      m_rn_uniform_pct = vclip(atof(value.c_str()), 0, 1);

    else if(param == "ground_truth")
      return(setBooleanOnString(m_ground_truth, value));

    else
      return(false);
  }
  else if(pass == 2) {
    if(param == "beacon")
      return(addBeaconBuoy(value));
    else
      return(false);
  }
  return(true);
}


//------------------------------------------------------------
// Procedure: handleMsg

bool BRS_Model::handleMsg(string key, double dval, string sval,
			  double mtime, bool isdouble, bool isstring,
			  string source)
{
  if((key == "NODE_REPORT") || (key == "NODE_REPORT_LOCAL"))
    return(handleNodeReport(sval));

  if(key == "BRS_RANGE_REQUEST")
    return(handleRangeRequest(sval));
  
  return(true);
}

//------------------------------------------------------------
// Procedure: setCurrTIme

void BRS_Model::setCurrTime(double new_time)
{
  if(new_time > m_curr_time)
    m_curr_time = new_time;
}

//------------------------------------------------------------
// Procedure: iterate()

void BRS_Model::iterate()
{
  unsigned int bix, vsize = m_beacons.size();

  // If the first iteration, mark the timestamp for all beacons.
  if(m_iterations == 0) {
    for(bix=0; bix<vsize; bix++)
      m_beacons[bix].setTimeStamp(m_curr_time);
  }

  // Determine which beacons are ready to broadcast a range msg
  // based on their regular interval setting
  unsigned int total_reports = 0;
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

      // For each vehicle, if valid, if in range, post report
      unsigned int vix, jsize = m_node_records.size();
      for(vix=0; vix<jsize; vix++) {
	if(m_node_records[vix].valid()) {
	  double actual_range = getTrueBeaconNodeRange(bix, vix);
	  double report_range = m_beacons[bix].getReportRange();
	  if((report_range < 0) || (actual_range < report_range)) {
	    if(total_reports == 0) {
	      report("\nUnsolicited beacon reports:\n", "blue");
	    }
	    total_reports++;
	    string vname = m_node_records[vix].getName(); 
	    postBeaconRangeReport(bix, actual_range, vname);
	  }
	}
      }
    }
  }
  m_iterations++;
}

//------------------------------------------------------------
// Procedure: getMessages

vector<VarDataPair> BRS_Model::getMessages(bool clear)
{
  if(!clear)
    return(m_messages);
  
  vector<VarDataPair> rval = m_messages;
  m_messages.clear();
  return(rval);
}


//------------------------------------------------------------
// Procedure: print()

void BRS_Model::print() const
{
  unsigned int i, vsize = m_beacons.size();

  cout << "BRS Model - # Beacons: " << vsize << endl;
  for(i=0; i<vsize; i++)
    cout << "  [" << i << "]:" << m_beacons[i].getSpec() << endl;

  unsigned int j, jsize = m_node_records.size();
  cout << " NodeRecords:: " << jsize << endl;
  for(j=0; j<jsize; j++)
    cout << m_node_records[j].getSpec() << endl;
}

//------------------------------------------------------------
// Procedure: addBeaconBuoy

bool BRS_Model::addBeaconBuoy(const string& line)
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
    else if(left == "report_range")
      beacon.setReportRange(rest);
    else if(left == "label")
      beacon.setLabel(rest);
    else if(left == "color")
      beacon.setBuoyColor(rest);
    else if(left == "shape")
      beacon.setShape(rest);
    else if(left == "width")
      beacon.setWidth(rest);
    else 
      m_figlog.addWarning("Unknown param in beacon spec:" + left);
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

bool BRS_Model::handleNodeReport(const string& node_report_str)
{
  NodeRecord new_node_record = string2NodeRecord(node_report_str);

  if(!new_node_record.valid()) {
    addMessage("BRS_DEBUG", "Invalid incoming node report");
    return(false);
  }

  bool new_record_added = updateNodeRecords(new_node_record);
  if(!new_record_added)
    report("*", "green");
  else
    report("%", "magenta");

  return(true);
}

//---------------------------------------------------------
// Procedure: handleRangeRequest
//   Example: vname=alpha

bool BRS_Model::handleRangeRequest(const string& request)
{
  string vname;

  vector<string> svector = parseString(request, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = toupper(stripBlankEnds(biteString(svector[i], '=')));
    string value = stripBlankEnds(svector[i]);
    if(param == "NAME")
      vname = value;
  }

  if(vname == "") {
    addMessage("BRS_DEBUG", "Range request with null node name");
    return(false);
  }
  
  // Determine the requesting vehicle's x,y position
  unsigned int j, jsize = m_node_records.size();
  unsigned int vix = jsize;
  for(j=0; j<jsize; j++) {
    if(vname == m_node_records[j].getName())
      vix = j;
  }

  if(m_verbose) 
    cout << endl << "Range request received from: " << vname << endl;

  // If nothing is known about this vehicle, we don't know its 
  // position, so just return false.
  if(vix == jsize) {
    report("   Denied: Nothing known about this vehicle.\n", "red");
    return(false);
  }

  double v_x = m_node_records[vix].getX();
  double v_y = m_node_records[vix].getY();

  // Determine if this vehicle is allowed to make a range request
  double elapsed_time  = m_node_records[vix].getElapsedTime(m_curr_time);
  bool   query_allowed = false;
  string query_freq_s  = m_node_records[vix].getProperty("query_freq");
  double query_freq    = atof(query_freq_s.c_str());
  if(elapsed_time > query_freq)
    query_allowed = true;
  report("   Elapsed time: " + doubleToString(elapsed_time) + "\n");

  if(m_ping_payments == "upon_request")
    m_node_records[vix].setTimeStamp(m_curr_time);

  if(!query_allowed) {
    report("   Denied: Query exceeds maximum query frequency.\n", "red");
    return(true);
  }
  else
    report("   Query accepted by uSimRangeSensor.\n", "green");

  if(m_ping_payments == "upon_accept") {
    report("   Query resets the clock for vehicle.\n");
    m_node_records[vix].setTimeStamp(m_curr_time);
  }
  
  // Build the RangePulse for the requesting vehicle
  string label = vname + "_range_req";
  postRangePulse(v_x, v_y, "white", label, 6, 40);

  // Phase 1: Handle range reports to fixed beacons
  //
  // For each beacon, determine if the beacon is within range to the
  // requesting vehicle to have received the request.
  unsigned int bix, bsize = m_beacons.size();
  for(bix=0; bix<bsize; bix++) {
    double actual_range_now = getTrueBeaconNodeRange(bix, vix);
    double reach_dist = m_default_reach_dist;
    if(m_reach_map.count(vname))
      reach_dist = m_reach_map[vname];

    if((reach_dist < 0) || (actual_range_now <= reach_dist)) {
      // Then determine if the vehicle is within range of the beacon to
      // hear the range report.
      double rpt_range = m_beacons[bix].getReportRange();
      if((rpt_range < 0) || (actual_range_now <= rpt_range)) {
	double x = m_beacons[bix].getX();
	double y = m_beacons[bix].getY();
	string label = m_beacons[bix].getLabel();
	if(m_ping_payments == "upon_response")
	  m_node_records[vix].setTimeStamp(m_curr_time);
	postBeaconRangeReport(bix, actual_range_now, 
			      m_node_records[vix].getName());
	postRangePulse(x, y, "pink", label, 15, 40);
	m_beacons[bix].setTimeStamp(m_curr_time);
      }
    }
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: addMessage()

void BRS_Model::addMessage(const string& varname,
			   const string& value)
{
  VarDataPair pair(varname, value);
  m_messages.push_back(pair);
}

//---------------------------------------------------------
// Procedure: addMessage()

void BRS_Model::addMessage(const string& varname, double value)
{
  VarDataPair pair(varname, value);
  m_messages.push_back(pair);
}

//---------------------------------------------------------
// Procedure: getVisuals()

vector<VarDataPair> BRS_Model::getVisuals()
{
  vector<VarDataPair> visuals;

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
    VarDataPair pair("VIEW_MARKER", spec);
    visuals.push_back(pair);    
  }
  return(visuals);
}


//------------------------------------------------------------
// Procedure: postBeaconRangePulse

void BRS_Model::postBeaconRangePulse(unsigned int ix, string color)
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
  addMessage("VIEW_RANGE_PULSE", spec);
}

//------------------------------------------------------------
// Procedure: postBeaconRangeReport()

void BRS_Model::postBeaconRangeReport(unsigned int beacon_ix, 
				      double actual_range, 
				      string vname)
{
  if(beacon_ix >= m_beacons.size())
    return;
  
  string label = m_beacons[beacon_ix].getLabel();

  report("   Range report sent from beacon[" + label + "] to ");
  report("vehicle[" + vname + "]\n");

  double report_range = actual_range;
  if(m_rn_algorithm != "")
    report_range  = getNoisyBeaconNodeRange(actual_range);

  // Phase 1: Post the "non-ground-truth" reports
  string str = "range=" + doubleToStringX(report_range, 4);
  str += ",id=" + label;
  str += ",time=" + doubleToStringX(m_curr_time,3);

  if((m_report_vars == "short") || (m_report_vars == "both")) {
    string full_str = "vname=" + vname + "," + str;
    addMessage("BRS_RANGE_REPORT", full_str);
  }

  if((m_report_vars == "long") || (m_report_vars == "both")) {
    vname = toupper(vname);
    addMessage("BRS_RANGE_REPORT_"+vname, str);
  }

  // Phase 2: Possibly Post the "ground-truth" reports
  if((m_rn_algorithm != "") && m_ground_truth) {
    string str = "range=" + doubleToStringX(actual_range, 4);
    str += ",id=" + label;
    str += ",time=" + doubleToStringX(m_curr_time,3);
    
    if((m_report_vars == "short") || (m_report_vars == "both")) {
      string full_str = "vname=" + vname + "," + str;
      addMessage("BRS_RANGE_REPORT_GT", full_str);
    }
    
    if((m_report_vars == "long") || (m_report_vars == "both")) {
      vname = toupper(vname);
      addMessage("BRS_RANGE_REPORT_GT_"+vname, str);
    }
  }

}


//------------------------------------------------------------
// Procedure: getTrueBeaconNodeRange()

double BRS_Model::getTrueBeaconNodeRange(unsigned int beacon_ix, 
					 unsigned int vnode_ix) const
{
  if((beacon_ix >= m_beacons.size()) || (vnode_ix >= m_node_records.size()))
    return(-1);

  double beacon_x = m_beacons[beacon_ix].getX();
  double beacon_y = m_beacons[beacon_ix].getY();
  double vnode_x  = m_node_records[vnode_ix].getX();
  double vnode_y  = m_node_records[vnode_ix].getY();
  double range    = hypot((beacon_x-vnode_x), (beacon_y-vnode_y));

  return(range);
}


//------------------------------------------------------------
// Procedure: getNoisyBeaconNodeRange()

double BRS_Model::getNoisyBeaconNodeRange(double true_range) const
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
// Procedure: report()    

void BRS_Model::report(string msg, string term_color)
{
  if(m_verbose)
    cout << termColor(term_color) << msg << termColor() << flush;
}

//------------------------------------------------------------
// Procedure: updateNodeRecords

bool BRS_Model::updateNodeRecords(NodeRecord new_record)
{
  bool add_new_record = true;
  unsigned int i, vsize = m_node_records.size();
  for(i=0; i<vsize; i++) {
    if(new_record.getName() == m_node_records[i].getName()) {
      add_new_record = false;
      // Make sure the timestamp is carried over to the updated 
      // record. It represents the last time a ping was made.
      double tstamp = m_node_records[i].getTimeStamp();
      m_node_records[i] = new_record;
      m_node_records[i].setTimeStamp(tstamp);
    }
  }
  if(add_new_record)
    m_node_records.push_back(new_record);

  return(add_new_record);
}


//------------------------------------------------------------
// Procedure: setReachDistance
//      Note: Negative values indicate infinity

bool BRS_Model::setReachDistance(string str)
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
    m_default_reach_dist = distance;
  else
    m_reach_map[left] = distance;
  
  return(true);
}


//------------------------------------------------------------
// Procedure: setPingWait
//      Note: Negative values no wait time mandated

bool BRS_Model::setPingWait(string str)
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
    m_default_ping_wait = wait_time;
  else
    m_pingw_map[left] = wait_time;
  
  return(true);
}

//------------------------------------------------------------
// Procedure: setPingPayments

bool BRS_Model::setPingPayments(string str)
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

bool BRS_Model::setReportVars(string str)
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

bool BRS_Model::setRandomNoiseAlgorithm(string str)
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

void BRS_Model::postRangePulse(double x, double y, string color,
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
  addMessage("VIEW_RANGE_PULSE", spec);
}

