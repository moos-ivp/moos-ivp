/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SIMAS_Model.cpp                                      */
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
#include "SIMAS_Model.h"
#include "MBUtils.h"
#include "NodeRecordUtils.h"
#include "XYRangePulse.h"

using namespace std;

//------------------------------------------------------------
// Constructor

SIMAS_Model::SIMAS_Model()
{
  // Configuration variables
  m_default_reach_dist = 100;    // meters
  m_default_reply_dist = 100;    // meters
  m_default_ping_wait  = 30;    // seconds

  m_ping_color   = "white";
  m_reply_color  = "chartreuse";
  m_report_vars  = "short";
  m_verbose      = false;
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

bool SIMAS_Model::setParam(string param, string value)
{
  param = tolower(param);

  if(param == "verbose")
    return(setBooleanOnString(m_verbose, value));
  else if((param == "reach_dist") || (param == "reach_distance"))
    return(setReachDistance(value));
  else if((param == "reply_dist") || (param == "reply_distance"))
    return(setReplyDistance(value));
  else if(param == "ping_wait")
    return(setPingWait(value));
  else if(param == "report_vars")
    return(setReportVars(value));
  else if((param == "ping_color") && isColor(value))
    m_ping_color = value;
  else if((param == "reply_color") && isColor(value))
    m_reply_color = value;
  else if(param == "rn_algorithm")
    return(setRandomNoiseAlgorithm(value));
  else if(param == "ground_truth")
    return(setBooleanOnString(m_ground_truth, value));
  else if((param == "rn_uniform_pct") && isNumber(value))
    m_rn_uniform_pct = vclip(atof(value.c_str()), 0, 1);
  else
    return(false);
  return(true);
}


//------------------------------------------------------------
// Procedure: handleMsg

bool SIMAS_Model::handleMsg(string key, double dval, string sval,
			  double mtime, bool isdouble, bool isstring,
			  string source)
{
  if((key == "NODE_REPORT") || (key == "NODE_REPORT_LOCAL"))
    return(handleNodeReport(sval));

  if(key == "SIMAS_RANGE_REQUEST")
    return(handleRangeRequest(sval));
  
  return(true);
}

//------------------------------------------------------------
// Procedure: setCurrTime

void SIMAS_Model::setCurrTime(double new_time)
{
  if(new_time > m_curr_time)
    m_curr_time = new_time;
}

//------------------------------------------------------------
// Procedure: iterate()

void SIMAS_Model::iterate()
{
  m_iterations++;
}

//------------------------------------------------------------
// Procedure: getMessages

vector<VarDataPair> SIMAS_Model::getMessages(bool clear)
{
  if(!clear)
    return(m_messages);
  
  vector<VarDataPair> rval = m_messages;
  m_messages.clear();
  return(rval);
}

//------------------------------------------------------------
// Procedure: print()

void SIMAS_Model::print() const
{
  cout << termColor("blue");
  cout << "==========================================" << endl;
  cout << "Acoustic Sonar Simulator Model: " << endl;
  cout << "==========================================" << endl;

  cout << "Default Reach Distance:   " << m_default_reach_dist << endl;
  cout << "Default Reply Distance:   " << m_default_reply_dist << endl;
  cout << "Default Ping Wait:        " << m_default_ping_wait << endl;
  cout << "Random Noise Algorithm:   " << m_rn_algorithm << endl;
  cout << "Random Noise Uniform Pct: " << m_rn_uniform_pct << endl;
  cout << "Ping Color:               " << m_ping_color << endl;
  cout << "Reply Color:              " << m_reply_color << endl;
  cout << "Ground Truth Reporting:   " << boolToString(m_ground_truth) << endl;

  cout << "ReachMap:" << endl;
  map<string, double>::const_iterator p;
  for(p=m_reach_map.begin(); p!=m_reach_map.end(); p++) {
    cout << "  VName: " << p->first;
    cout << "  Dist: " << p->second << endl;
  }

  cout << "ReplyMap:" << endl;
  for(p=m_reply_map.begin(); p!=m_reply_map.end(); p++) {
    cout << "  VName: " << p->first;
    cout << "  Dist: "  << p->second << endl;
  }

  cout << "PingWaitMap:" << endl;
  for(p=m_pingw_map.begin(); p!=m_pingw_map.end(); p++) {
    cout << "  VName: " << p->first;
    cout << "  PingWait: " << p->second << endl;
  }

  unsigned int j, jsize = m_node_records.size();
  cout << "Contact Records: " << jsize << endl;
  for(j=0; j<jsize; j++)
    cout << m_node_records[j].getSpec() << endl;
  cout << "==========================================" << endl;
  cout << termColor();
}

//---------------------------------------------------------
// Procedure: handleNodeReport
//   Example: NAME=alpha,TYPE=KAYAK,UTC_TIME=1267294386.51,
//            X=29.66,Y=-23.49,LAT=43.825089, LON=-70.330030, 
//            SPD=2.00, HDG=119.06,YAW=119.05677,DEPTH=0.00,     
//            LENGTH=4.0,MODE=ENGAGED

bool SIMAS_Model::handleNodeReport(const string& node_report_str)
{
  NodeRecord new_node_record = string2NodeRecord(node_report_str);

  if(!new_node_record.valid()) {
    addMessage("SIMAS_DEBUG", "Invalid incoming node report");
    return(false);
  }

  bool new_node_added = updateNodeRecords(new_node_record);
  if(!new_node_added)
    report("*", "green");
  else
    report("%", "magenta");

  return(true);
}

//---------------------------------------------------------
// Procedure: handleRangeRequest
//   Example: vname=alpha

bool SIMAS_Model::handleRangeRequest(const string& request)
{
  string vname;
  
  // Phase 1: Parse the incoming range request string
  vector<string> svector = parseString(request, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = toupper(stripBlankEnds(biteString(svector[i], '=')));
    string value = stripBlankEnds(svector[i]);
    if(param == "NAME")
      vname = value;
  }
  if(vname == "") {
    addMessage("SIMAS_DEBUG", "Range Request with null node name");
    return(false);
  }
  if(m_verbose) 
    cout << endl << "Range Request received from: " << vname << endl;
  
  // Phase 2: Determine if we know anything about this vehicle. If 
  // we have received any prior node reports. We must know the position
  // of a vehicle in order honor a range request.
  unsigned int j, jsize = m_node_records.size();
  unsigned int vix = jsize;
  for(j=0; j<jsize; j++) {
    if(vname == m_node_records[j].getName())
      vix = j;
  }
  if(vix == jsize) {
    report("   Denied: Nothing known about this vehicle.\n", "red");
    return(false);
  }

  // Phase 3: Determine if this vehicle is allowed to generate a ping
  // based on the last time it made a ping request and the ping wait 
  // time associated with the vehicle.
  
  double elapsed_time = m_node_records[vix].getElapsedTime(m_curr_time);
  bool   ping_allowed = false;
  double ping_wait    = m_default_ping_wait;
  if(m_pingw_map.count(vname))
    ping_wait = m_pingw_map[vname];

  if(elapsed_time > ping_wait)
    ping_allowed = true;
  report("   Elapsed time: " + doubleToString(elapsed_time) + "\n");

  if(!ping_allowed) {
    report("   Denied: Range Request exceeds maximum ping frequency.\n", "red");
    return(true);
  }
  else
    report("   Range Request accepted by uSimActiveSonar.\n", "green");

  report("   Range Request resets the clock for vehicle.\n");
  m_node_records[vix].setTimeStamp(m_curr_time);

  
  // Phase 4: Post the RangePulse for the requesting vehicle. This is
  // purely a visual artifact.
  postRangePulse(vix, m_ping_color, vname+"_ping", 6, 50);


  // Phase 5: Handle range reports to target nodes. For each target, 
  // determine if the target is within range to the vehicle making the
  // range request. If so generate the range report and visual pulse.
  unsigned int tix, tsize = m_node_records.size();
  for(tix=0; tix<tsize; tix++) {
    if(tix != vix) { // (no reports back to the requesting vehicle!)
      double actual_range_now = getTrueNodeNodeRange(vix, tix);
      string targ_name  = m_node_records[tix].getName();

      double reach_dist = m_default_reach_dist;
      if(m_reach_map.count(vname))
	reach_dist = m_reach_map[vname];

      double reply_dist = m_default_reply_dist;
      if(m_reply_map.count(targ_name))
	reply_dist = m_reply_map[targ_name];

      bool sonar_hit = false;
      if((reach_dist < 0) || (reply_dist < 0))
	sonar_hit = true;

      if((reach_dist + reply_dist) > actual_range_now)
	sonar_hit = true;

      if(sonar_hit) {
	postNodeRangeReport(vix, tix, actual_range_now);
	string label = targ_name + "_ping_reply";
	postRangePulse(tix, m_reply_color, label, 15, 40);
      }
    }
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: addMessage()

void SIMAS_Model::addMessage(const string& varname,
			     const string& value)
{
  VarDataPair pair(varname, value);
  m_messages.push_back(pair);
}

//---------------------------------------------------------
// Procedure: addMessage()

void SIMAS_Model::addMessage(const string& varname, double value)
{
  VarDataPair pair(varname, value);
  m_messages.push_back(pair);
}

//------------------------------------------------------------
// Procedure: postRangePulse

void SIMAS_Model::postRangePulse(unsigned int ix, string color,
				 string label, double duration,
				 double radius)
{
  if(ix >= m_node_records.size())
    return;

  XYRangePulse pulse;
  pulse.set_x(m_node_records[ix].getX());
  pulse.set_y(m_node_records[ix].getY());
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

//------------------------------------------------------------
// Procedure: postNodeRangeReport()

void SIMAS_Model::postNodeRangeReport(unsigned int rec_ix, 
				      unsigned int tar_ix, 
				      double actual_range)
{
  unsigned num_records = m_node_records.size();
  if((rec_ix >= num_records) || (tar_ix >= num_records))
    return;

  string rec_name = m_node_records[rec_ix].getName();
  string tar_name = m_node_records[tar_ix].getName();

  report("   Range Report sent from targ vehicle[" + tar_name + "] to ");
  report("receiver vehicle[" + rec_name + "]\n");


  double report_range = actual_range;
  if(m_rn_algorithm != "")
    report_range = getNoisyNodeNodeRange(actual_range);


  // Phase 1: Post the "non-ground-truth" reports
  string str = "range=" + doubleToStringX(report_range, 4);
  str += ",target=" + tar_name;
  str += ",time=" + doubleToStringX(m_curr_time,3);

  if((m_report_vars == "short") || (m_report_vars == "both")) {
    string full_str = "vname=" + rec_name + "," + str;
    addMessage("SIMAS_RANGE_REPORT", full_str);
  }

  if((m_report_vars == "long") || (m_report_vars == "both"))
    addMessage("SIMAS_RANGE_REPORT_" + toupper(rec_name), str);

  // Phase 2: Possibly Post the "ground-truth" reports
  if((m_rn_algorithm != "") && m_ground_truth) {
    string str = "range=" + doubleToStringX(actual_range, 4);
    str += ",target=" + tar_name;
    str += ",time=" + doubleToStringX(m_curr_time,3);
    
    if((m_report_vars == "short") || (m_report_vars == "both")) {
      string full_str = "vname=" + rec_name + "," + str;
      addMessage("SIMAS_RANGE_REPORT_GT", full_str);
    }
    
    if((m_report_vars == "long") || (m_report_vars == "both"))
      addMessage("SIMAS_RANGE_REPORT_GT_" + toupper(rec_name), str);
  }

}

//------------------------------------------------------------
// Procedure: getTrueNodeNodeRange()

double SIMAS_Model::getTrueNodeNodeRange(unsigned int src_ix, 
					 unsigned int tar_ix) const
{
  if((src_ix >= m_node_records.size()) || 
     (tar_ix >= m_node_records.size()))
    return(-1);

  double rnode_x = m_node_records[src_ix].getX();
  double rnode_y = m_node_records[src_ix].getY();
  double tnode_x = m_node_records[tar_ix].getX();
  double tnode_y = m_node_records[tar_ix].getY();
  double range = hypot((rnode_x-tnode_x), (rnode_y-tnode_y));

  return(range);
}

//------------------------------------------------------------
// Procedure: getNoisyNodeNodeRange()

double SIMAS_Model::getNoisyNodeNodeRange(double true_range) const
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

void SIMAS_Model::report(string msg, string term_color)
{
  if(m_verbose)
    cout << termColor(term_color) << msg << termColor() << flush;
}

//------------------------------------------------------------
// Procedure: printNodeRecords

void SIMAS_Model::printNodeRecords()
{
  unsigned int i, vsize = m_node_records.size();
  for(i=0; i<vsize; i++) {
    cout << "[" << i << "]:" << m_node_records[i].getSpec() << endl;
  }
}


//------------------------------------------------------------
// Procedure: updateNodeRecords

bool SIMAS_Model::updateNodeRecords(const NodeRecord& new_record)
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

bool SIMAS_Model::setReachDistance(string str)
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
// Procedure: setReplyDistance
//      Note: Negative values indicate infinity

bool SIMAS_Model::setReplyDistance(string str)
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
    m_default_reply_dist = distance;
  else
    m_reply_map[left] = distance;
  
  return(true);
}


//------------------------------------------------------------
// Procedure: setPingWait
//      Note: Negative values no wait time mandated

bool SIMAS_Model::setPingWait(string str)
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
    m_default_ping_wait = wait_time;
  else
    m_pingw_map[left] = wait_time;
  
  return(true);
}

//------------------------------------------------------------
// Procedure: setReportVars

bool SIMAS_Model::setReportVars(string str)
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

bool SIMAS_Model::setRandomNoiseAlgorithm(string str)
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
