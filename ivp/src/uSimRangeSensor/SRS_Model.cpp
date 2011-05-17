/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: MIT Dept. of Mechanical Engineering             */
/*    FILE: SRS_Model.cpp                                   */
/*    DATE: Feb 2nd, 2011                                   */
/************************************************************/

#include <cstdlib>
#include <iostream>
#include <cmath>
#include "SRS_Model.h"
#include "MBUtils.h"
#include "XYRangePulse.h"

using namespace std;

//------------------------------------------------------------
// Constructor

SRS_Model::SRS_Model()
{
  // Configuration variables
  m_default_node_query_range  = 100;    // meters
  m_default_node_query_freq   = 30;     // seconds

  m_default_beacon_report_range = 100;  // meters
  m_default_beacon_width = 4;           // meters
  m_default_beacon_color = "orange";
  m_default_beacon_shape = "circle";
  m_default_beacon_freq  = "never";     // seconds

  m_query_payments = "upon_response";
  m_report_vars = "short";
  m_verbose = false;

  // State Variables
  m_curr_time  = 0;
  m_iterations = 0;

}

//------------------------------------------------------------
// Procedure: setParam

bool SRS_Model::setParam(string param, string value, unsigned int pass)
{
  param = tolower(param);

  if((param == "beacon") && (pass == 2))
    return(addBeaconBuoy(value));
  else if(param == "verbose")
    return(setBooleanOnString(m_verbose, value));
  else if(param == "default_beacon_freq")
    m_default_beacon_freq = value;
  else if(param == "default_beacon_shape")
    m_default_beacon_shape = value;
  else if(param == "default_beacon_color")
    m_default_beacon_color = value;
  else if((param == "default_beacon_width") && isNumber(value)) {
    double dval = atof(value.c_str());
    if(dval > 0)
      m_default_beacon_width = dval;
  }
  else if((param == "default_node_query_range") && isNumber(value)) {
    double dval = atof(value.c_str());
    if(dval > 0)
      m_default_node_query_range = dval;
  }
  else if((param == "default_node_query_range") && 
	  (tolower(value) == "nolimit")) {
    m_default_node_query_range = -1;
  }
  else if((param == "default_node_query_freq") && isNumber(value)) {
    double dval = atof(value.c_str());
    if(dval >= 0)
      m_default_node_query_freq = dval;
  }
  else if((param == "default_beacon_report_range") && isNumber(value)) {
    double dval = atof(value.c_str());
    if(dval > 0)
      m_default_beacon_report_range = dval;
  }
  else if((param == "default_beacon_report_range") && 
	  (tolower(value) == "nolimit")) {
    m_default_beacon_report_range = -1;
  }
  else if(param == "query_payments") {
    string val = tolower(value);
    if((val == "upon_response") || (val == "upon_request") ||
       (val == "upon_accept"))
      m_query_payments = val;
  }
  else if(param == "report_vars") {
    string val = tolower(value);
    if((val == "both") || (val == "short") || (val == "long"))
      m_report_vars = val;
  }
  else
    return(false);
  return(true);
}


//------------------------------------------------------------
// Procedure: handleMsg

bool SRS_Model::handleMsg(string key, double dval, string sval,
			  double mtime, bool isdouble, bool isstring,
			  string source)
{
  if((key == "NODE_REPORT") || (key == "NODE_REPORT_LOCAL"))
    return(handleNodeReport(sval));
  if(key == "RANGE_REQUEST")
    return(handleRangeRequest(sval));
  
  return(true);
}

//------------------------------------------------------------
// Procedure: setCurrTIme

void SRS_Model::setCurrTime(double new_time)
{
  if(new_time > m_curr_time)
    m_curr_time = new_time;
}

//------------------------------------------------------------
// Procedure: iterate()

void SRS_Model::iterate()
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
      postBeaconRangePulse(bix);

      // For each vehicle, if valid, if in range, post report
      unsigned int vix, jsize = m_vnodes.size();
      for(vix=0; vix<jsize; vix++) {
	if(m_vnodes[vix].valid()) {
	  double actual_range = getBeaconNodeRange(bix, vix);
	  double report_range = m_beacons[bix].getReportRange();
	  if((report_range < 0) || (actual_range < report_range)) {
	    if(total_reports == 0) {
	      report("\nUnsolicited beacon reports:\n", "blue");
	    }
	    total_reports++;
	    postRangeReport(bix, actual_range, m_vnodes[vix].getName());
	  }
	}
      }
    }
  }
  m_iterations++;
}

//------------------------------------------------------------
// Procedure: getMessages

vector<VarDataPair> SRS_Model::getMessages(bool clear)
{
  if(!clear)
    return(m_messages);
  
  vector<VarDataPair> rval = m_messages;
  m_messages.clear();
  return(rval);
}


//------------------------------------------------------------
// Procedure: print()

void SRS_Model::print() const
{
  unsigned int i, vsize = m_beacons.size();

  cout << "SRS Model - # Beacons: " << vsize << endl;
  for(i=0; i<vsize; i++)
    cout << "  [" << i << "]:" << m_beacons[i].getSpec() << endl;

  cout << "Default Beacon Color: " << m_default_beacon_color << endl;
  cout << "Default Beacon Shape: " << m_default_beacon_shape << endl;
  cout << "Default Beacon Width: " << m_default_beacon_width << endl;

  unsigned int j, jsize = m_vnodes.size();
  cout << " NodeRecords:: " << jsize << endl;
  for(j=0; j<jsize; j++)
    cout << m_vnodes[j].getSpec() << endl;
}

//------------------------------------------------------------
// Procedure: addBeaconBuoy

bool SRS_Model::addBeaconBuoy(const string& line)
{
  vector<string> svector = parseString(line, ',');
  unsigned int i, vsize = svector.size();

  BeaconBuoy beacon;
  beacon.setFrequencyRange(m_default_beacon_freq);
  beacon.setBuoyColor(m_default_beacon_color);
  beacon.setShape(m_default_beacon_shape);
  beacon.setWidth(m_default_beacon_width);
  beacon.setReportRange(m_default_beacon_report_range);
  for(i=0; i<vsize; i++) {
    string left = biteStringX(svector[i], '=');
    string rest = svector[i];
    if((left == "x") && isNumber(rest))
      beacon.setX(atof(rest.c_str()));
    else if((left == "y") && isNumber(rest))
      beacon.setY(atof(rest.c_str()));
    else if(left == "freq")
      beacon.setFrequencyRange(rest);
    else if((left == "report_range") && isNumber(rest))
      beacon.setReportRange(atof(rest.c_str()));
    else if(left == "label")
      beacon.setLabel(rest);
    else if(left == "color")
      beacon.setBuoyColor(rest);
    else if(left == "shape")
      beacon.setShape(rest);
    else if((left == "width") && isNumber(rest))
      beacon.setWidth(atof(rest.c_str()));
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

bool SRS_Model::handleNodeReport(const string& node_report)
{
  string vname, vtype, mode;
  double x, y, lat, lon, depth, speed, heading;
  double yaw, length;
  double utime = 0;

  vector<string> svector = parseString(node_report, ',');
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

  if(vname == "") {
    addMessage("SRS_DEBUG", "Incoming node report with null node name");
    return(false);
  }

  if(utime == 0) {
    addMessage("SRS_DEBUG", "Incoming node report with null timestamp");
    return(false);
  }

  // Find out if the node report corresponds to a contact already
  // in the records
  unsigned int j, jsize = m_vnodes.size();
  unsigned int ix = jsize;
  for(j=0; j<jsize; j++) {
    if(vname == m_vnodes[j].getName())
      ix = j;
  }

  // If the record does not already exist, make one
  if(ix == jsize) {
    NodeRecord vnode;
    vnode.setName(vname);
    vnode.setX(x);
    vnode.setY(y);
    vnode.setSpeed(speed);
    vnode.setValid(true);
    vnode.setTimeStamp(m_curr_time);
    vnode.setQueryFreq(m_default_node_query_freq);
    vnode.setQueryRange(m_default_node_query_range);
    m_vnodes.push_back(vnode);
    report("\nReceived first NODE_REPORT for: " + vname +"\n", "blue");
  }
  else {
    m_vnodes[ix].setX(x);
    m_vnodes[ix].setY(y);
    m_vnodes[ix].setSpeed(speed);
    report("*", "magenta");
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: handleRangeRequest
//   Example: vname=alpha

bool SRS_Model::handleRangeRequest(const string& request)
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
    addMessage("SRS_DEBUG", "Range request with null node name");
    return(false);
  }
  
  // Determine the requesting vehicle's x,y position
  unsigned int j, jsize = m_vnodes.size();
  unsigned int vix = jsize;
  for(j=0; j<jsize; j++) {
    if(vname == m_vnodes[j].getName())
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

  double v_x = m_vnodes[vix].getX();
  double v_y = m_vnodes[vix].getY();

  // Determine if this vehicle is allowed to make a range request
  double elapsed_time = m_vnodes[vix].getElapsedTime(m_curr_time);
  report("   Elapsed time: " + doubleToString(elapsed_time) + "\n");

  bool query_allowed = m_vnodes[vix].queryAllowed(m_curr_time);
  if(m_query_payments == "upon_request")
    m_vnodes[vix].setTimeStamp(m_curr_time);

  if(!query_allowed) {
    report("   Denied: Query exceeds maximum query frequency.\n", "red");
    return(true);
  }
  else
    report("   Query accepted by pSimRangeSensor.\n", "green");

  if(m_query_payments == "upon_accept") {
    report("   Query resets the clock for vehicle.\n");
    m_vnodes[vix].setTimeStamp(m_curr_time);
  }
  
  // Build the RangePulse for the requesting vehicle
  XYRangePulse pulse(v_x, v_y);
  pulse.set_rad(50);
  pulse.set_duration(6);
  pulse.set_fill(0.9);
  pulse.set_color("fill", "white");
  pulse.set_color("edge", "white");
  pulse.set_time(m_curr_time);
  pulse.set_label(vname+"_range_req");
  addMessage("VIEW_RANGE_PULSE", pulse.get_spec());

  // For each beacon, determine if the beacon is within range to the
  // requesting vehicle to have received the request.
  unsigned int bix, bsize = m_beacons.size();
  for(bix=0; bix<bsize; bix++) {
    double actual_range_now = getBeaconNodeRange(bix, vix);
    double query_range = m_vnodes[vix].getQueryRange();
    if((query_range < 0) || (actual_range_now <= query_range)) {
      // Then determine if the vehicle is within range of the beacon to
      // hear the range report.
      double rpt_range = m_beacons[bix].getReportRange();
      if((rpt_range < 0) || (actual_range_now <= rpt_range)) {
	if(m_query_payments == "upon_response")
	  m_vnodes[vix].setTimeStamp(m_curr_time);
	postRangeReport(bix, actual_range_now, m_vnodes[vix].getName());
      }
      postBeaconRangePulse(bix, "pink");
      m_beacons[bix].setTimeStamp(m_curr_time);
    }
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: addMessage()

void SRS_Model::addMessage(const string& varname,
			   const string& value)
{
  VarDataPair pair(varname, value);
  m_messages.push_back(pair);
}

//---------------------------------------------------------
// Procedure: addMessage()

void SRS_Model::addMessage(const string& varname, double value)
{
  VarDataPair pair(varname, value);
  m_messages.push_back(pair);
}

//---------------------------------------------------------
// Procedure: getVisuals()

vector<VarDataPair> SRS_Model::getVisuals()
{
  vector<VarDataPair> visuals;

  unsigned int i, vsize = m_beacons.size();
  for(i=0; i<vsize; i++) {
    string spec = m_beacons[i].getSpec();
    
    if(!strContains(spec, "color"))
      spec += ",color=" + m_default_beacon_color;
    if(!strContains(spec, "type"))
      spec += ",type=" + m_default_beacon_shape;
    if(!strContains(spec, "width"))
      spec += ",width=" + doubleToStringX(m_default_beacon_width);

    VarDataPair pair("VIEW_MARKER", spec);
    visuals.push_back(pair);    
  }
  return(visuals);
}


//------------------------------------------------------------
// Procedure: postBeaconRangePulse

void SRS_Model::postBeaconRangePulse(unsigned int ix, string color)
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
// Procedure: postRangeReport()

void SRS_Model::postRangeReport(unsigned int beacon_ix, 
				double range, string vname)
{
  if(beacon_ix >= m_beacons.size())
    return;

  string label = m_beacons[beacon_ix].getLabel();

  report("   Range report sent from beacon[" + label + "] to ");
  report("vehicle[" + vname + "]\n");

  string str = "range=" + doubleToStringX(range, 4);
  str += ",id=" + label;
  str += ",time=" + doubleToStringX(m_curr_time,3);

  cout << "Range Report:" << str << endl;

  if((m_report_vars == "short") || (m_report_vars == "both")) {
    string full_str = "vname=" + vname + "," + str;
    addMessage("RANGE_REPORT", full_str);
  }

  if((m_report_vars == "long") || (m_report_vars == "both")) {
    vname = toupper(vname);
    addMessage("RANGE_REPORT_"+vname, str);
  }
}


//------------------------------------------------------------
// Procedure: getBeaconNodeRange()

double SRS_Model::getBeaconNodeRange(unsigned int beacon_ix, 
				     unsigned int vnode_ix) const
{
  if((beacon_ix >= m_beacons.size()) || (vnode_ix > m_vnodes.size()))
    return(-1);

  double beacon_x = m_beacons[beacon_ix].getX();
  double beacon_y = m_beacons[beacon_ix].getY();
  double vnode_x  = m_vnodes[vnode_ix].getX();
  double vnode_y  = m_vnodes[vnode_ix].getY();
  double range = hypot((beacon_x-vnode_x), (beacon_y-vnode_y));
  return(range);
}


//------------------------------------------------------------
// Procedure: report()    

void SRS_Model::report(string msg, string term_color)
{
  if(m_verbose)
    cout << termColor(term_color) << msg << termColor() << flush;
}

