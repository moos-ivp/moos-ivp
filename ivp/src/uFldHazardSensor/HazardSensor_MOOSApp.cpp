/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardSensor_MOOSApp.cpp                             */
/*    DATE: Jan 28th 2012                                        */
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

#include <iterator>
#include <cmath>
#include "HazardSensor_MOOSApp.h"
#include "XYFormatUtilsHazard.h"
#include "NodeRecordUtils.h"
#include "FileBuffer.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "ColorParse.h"
#include "XYMarker.h"
#include "XYCircle.h"
#include "MBUtils.h"
#include "ACTable.h"

using namespace std;

//---------------------------------------------------------
// Constructor

HazardSensor_MOOSApp::HazardSensor_MOOSApp()
{
  // State Variables
  // -------------------------------------------------------------
  m_last_summary_time = 0;    // last time settings options summary posted

  // Configuration variables
  // -------------------------------------------------------------
  m_min_reset_interval       = 300;  // seconds
  m_options_summary_interval = 10;   // in timewarped seconds
  m_min_queue_msg_interval   = 30;   // seconds
  m_sensor_max_turn_rate     = 1.5;  // degrees per second

  m_hazard_file_hazard_cnt = 0;
  m_hazard_file_benign_cnt = 0;
  m_hazard_file = "None provided.";

  // If uniformly random noise used, (m_rn_algorithm = "uniform")
  // this variable reflects the range of the uniform variable in 
  // terms of percentage of the true (pre-noise) range.
  m_rn_uniform_pct = 0;

  m_seed_random  = true;
  
  // Visual preferences
  m_circle_duration = -1;
  m_show_hazards = true;
  m_show_swath   = true;
  m_show_pd      = true;
  m_show_pfa     = true;
  m_color_hazard = "green";
  m_color_benign = "light_blue";
  m_shape_hazard = "triangle";
  m_shape_benign = "triangle";
  m_width_hazard = 8;
  m_width_benign = 8;
  m_swath_len    = 5;
  m_swath_transparency = 0.2;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool HazardSensor_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    //p->Trace();
    CMOOSMsg msg = *p;
	
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 
    string comm  = msg.GetCommunity();

    bool handled = false;
    if(key == "NODE_REPORT")
      handled = handleNodeReport(sval);
    else if(key == "UHZ_SENSOR_REQUEST")
      handled = handleSensorRequest(sval);
    else if(key == "UHZ_SENSOR_CLEAR")
      handled = handleSensorClear(sval);
    else if(key == "UHZ_CLASSIFY_REQUEST")
      handled = handleClassifyRequest(sval);   
    else if(key == "UHZ_CONFIG_REQUEST")
      handled = handleSensorConfig(sval, comm);
    else if(key == "PMV_CONNECT") {
      postVisuals();
      handled = true;
    }
  
    //if(!handled) 
    //  reportRunWarning("Unhandled mail: " + key);
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool HazardSensor_MOOSApp::OnConnectToServer()
{
  registerVariables();  
  return(true);
}


//------------------------------------------------------------
// Procedure: registerVariables

void HazardSensor_MOOSApp::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  m_Comms.Register("NODE_REPORT", 0);
  m_Comms.Register("UHZ_SENSOR_REQUEST", 0);
  m_Comms.Register("UHZ_SENSOR_CLEAR", 0);
  m_Comms.Register("UHZ_CLASSIFY_REQUEST", 0);
  m_Comms.Register("UHZ_CONFIG_REQUEST", 0);
  m_Comms.Register("PMV_CONNECT", 0);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool HazardSensor_MOOSApp::Iterate()
{
  AppCastingMOOSApp::Iterate();

  // Part 1: Consider posting a settings options summary
  double elapsed_time = m_curr_time - m_last_summary_time;
  if(elapsed_time >= m_options_summary_interval) {
    Notify("UHZ_OPTIONS_SUMMARY", m_sensor_prop_summary);
    m_last_summary_time = m_curr_time;
  }

  postQueueMessages();

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool HazardSensor_MOOSApp::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  
  STRING_LIST sParams;
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams)) 
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig   = *p;
    string line   = *p;
    string param  = biteStringX(line, '=');
    string value  = line;

    bool handled = false;
    if(param == "default_hazard_shape") {
      m_shape_hazard = value;
      handled = true;
    }
    else if((param == "default_hazard_color") && isColor(value)) {
      m_color_hazard = value;
      handled = true;
    }
    else if((param == "default_hazard_width") && isNumber(value)) {
      m_width_hazard = atof(value.c_str());
      handled = true;
    }
    else if((param == "min_reset_interval") && isNumber(value)) {
      m_min_reset_interval = atof(value.c_str());
      handled = true;
    }
    else if((param == "max_turn_rate") && isNumber(value)) {
      m_sensor_max_turn_rate = atof(value.c_str());
      handled = true;
    }
    else if((param == "classify_period") || (param == "min_classify_interval")) {
      if(isNumber(value)) {
	m_min_queue_msg_interval = atof(value.c_str());
	handled = true;
      }
    }
    else if(param == "default_benign_shape") {
      m_shape_benign = value;
      handled = true;
    }
    else if(param == "default_benign_color") {
      m_color_benign = value;
      handled = true;
    }
    else if((param == "default_benign_width") && isNumber(value)) {
      m_width_benign = atof(value.c_str());
      handled = true;
    }
    else if(param == "show_hazards")
      handled = setBooleanOnString(m_show_hazards, value);
    else if(param == "show_pd")
      handled = setBooleanOnString(m_show_pd, value);
    else if(param == "show_pfa")
      handled = setBooleanOnString(m_show_pfa, value);
    else if(param == "show_swath")
      handled = setBooleanOnString(m_show_swath, value);
    else if(((param == "show_reports") || (param == "show_detections")) 
	    && isNumber(value)) {
      m_circle_duration = atof(value.c_str());
      handled = true;
    }
    else if((param == "show_reports") || (param == "show_detections")) {
      value = tolower(value);
      if((value == "unlimited") || (value == "nolimit")) {
	m_circle_duration = -1;
	handled = true;
      }
    }
    else if(param == "options_summary_interval")
      handled = setOptionsSummaryInterval(value);
    else if(param == "swath_length")
      handled = setSwathLength(value);
    else if(param == "show_swath")
      handled = setBooleanOnString(m_show_swath, value);
    else if(param == "show_hazards")
      handled = setBooleanOnString(m_show_hazards, value);
    else if(param == "seed_random")
      handled = setBooleanOnString(m_seed_random, value);
    else if((param == "swath_transparency") && isNumber(value))
      handled = setSwathTransparency(value);
    else if(param == "rn_algorithm") 
      handled = setRandomNoiseAlgorithm(value);
    else if(param == "sensor_config") 
      handled = addSensorConfig(value);
    else if((param == "rn_uniform_pct") && isNumber(value)) {
      m_rn_uniform_pct = vclip(atof(value.c_str()), 0, 1);
      handled = true;
    }
    else if(param == "hazard_file")
      handled = processHazardFile(value);

    else if(param == "hazard")   // Fix me! Needs to be done on PASS2
      handled = addHazard(value);

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  registerVariables();
  postVisuals();
  perhapsSeedRandom();
  sortSensorProperties();

  if(m_map_hazards.size() == 0)
    reportConfigWarning("No Hazard Field Laydown Provided.");

  if(m_sensor_prop_width.size() == 0)
    reportConfigWarning("Sensor was configured with NO sensor_config properties!");

  return(true);
}

//------------------------------------------------------------
// Procedure: perhapsSeedRandom

void HazardSensor_MOOSApp::perhapsSeedRandom()
{
  if(m_seed_random)
    srand(time(NULL));
}

//------------------------------------------------------------
// Procedure: addHazard

bool HazardSensor_MOOSApp::addHazard(string line)
{
  XYHazard hazard = string2Hazard(line);
  if(!hazard.isSetX() || !hazard.isSetY())
    return(false);
  if(!hazard.isSetType()) {
    reportConfigWarning("Hazard specified with no type: " + line);
    return(false);
  }
  if(hazard.getLabel() == "") {
    reportConfigWarning("Hazard specified with no label: " + line);
    return(false);
  }

  string label = hazard.getLabel();
  if(label == "") {
    label = uintToString(m_map_hazards.size());    
    hazard.setLabel(label);
  }

  if(!hazard.hasResemblance())
    hazard.setResemblance(1);

  if(m_map_hazards.count(label) != 0) {
    reportConfigWarning("Duplicated hazard label detected:" + label);
    return(false);
  }

  m_map_hazards[label] = hazard;

  // Keep a running count of object type for later in appcast report
  if(hazard.getType() == "hazard")
    m_hazard_file_hazard_cnt++;
  else
    m_hazard_file_benign_cnt++;

  return(true);
}

//------------------------------------------------------------
// Procedure: setSwathLength()

bool HazardSensor_MOOSApp::setSwathLength(string line)
{
  if(!isNumber(line))
    return(false);
  
  m_swath_len = atof(line.c_str());
  if(m_swath_len < 1)
    m_swath_len = 1;

  return(true);
}
    
//------------------------------------------------------------
// Procedure: addSensorConfig

bool HazardSensor_MOOSApp::addSensorConfig(string line)
{
  string str_width, str_exp, str_class, str_max="0";

  vector<string> svector = parseString(line, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "width") 
      str_width = value;
    else if(param == "exp") 
      str_exp = value;
    else if((param == "class") || (param == "pclass"))
      str_class = value;
    else if(param == "max")
      str_max = value;
  }
  if(!isNumber(str_width) || !isNumber(str_exp) || 
     !isNumber(str_class) || !isNumber(str_max))
    return(false);
  
  double d_width = atof(str_width.c_str());
  double d_exp   = atof(str_exp.c_str());
  double d_class = atof(str_class.c_str());
  int    i_max   = atoi(str_max.c_str());

  d_width = vclip_min(d_width, 0);
  d_exp   = vclip_min(d_exp, 1);
  d_class = vclip(d_class, 0, 1);
  if(i_max < 0)
    i_max = 0;

  m_sensor_prop_width.push_back(d_width);
  m_sensor_prop_exp.push_back(d_exp);
  m_sensor_prop_class.push_back(d_class);
  m_sensor_prop_max.push_back((unsigned int)(i_max));

  // Build up the sensor property summary, published occassionally
  // "width=25,exp=4,class=0.8 : width=10,exp=8,class=0.93"

  if(m_sensor_prop_summary != "")
    m_sensor_prop_summary += ":";
  m_sensor_prop_summary += "width=" + str_width;
  m_sensor_prop_summary += ",exp="   + str_exp;
  m_sensor_prop_summary += ",class=" + str_class;

  if(i_max > 0)
    m_sensor_prop_summary += ",max=" + intToString(i_max);

  return(true);
}


//---------------------------------------------------------
// Procedure: handleNodeReport
//   Example: NAME=alpha,TYPE=KAYAK,UTC_TIME=1267294386.51,
//            X=29.66,Y=-23.49,LAT=43.825089, LON=-70.330030, 
//            SPD=2.00, HDG=119.06,YAW=119.05677,DEPTH=0.00,     
//            LENGTH=4.0,MODE=ENGAGED

bool HazardSensor_MOOSApp::handleNodeReport(const string& node_report_str)
{
  NodeRecord new_node_record = string2NodeRecord(node_report_str);

  if(!new_node_record.valid()) {
    Notify("UHZ_DEBUG", "Invalid incoming node report");
    reportRunWarning("ERROR: Unhandled node record");
    return(false);
  }

  // In case there is an outstanding RunWarning indicating the lack
  // of a node report for a given vehicle, retract it here. This is 
  // mostly a startup timing issue. Sometimes a sensor request is 
  // received before a node report. Only a problem if the node report
  // never comes. Once we get one, it's no longer a problem.
  string vname = new_node_record.getName();
  retractRunWarning("No NODE_REPORT received for " + vname);


  updateNodeRecords(new_node_record);
  return(true);
}

//---------------------------------------------------------
// Procedure: handleSensorRequest
//   Example: vname=alpha

bool HazardSensor_MOOSApp::handleSensorRequest(const string& request)
{
  // Part 1: Parse the string request
  string vname;  
  vector<string> svector = parseString(request, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = toupper(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "VNAME")
      vname = value;
  }

  if(vname == "") {
    reportRunWarning("Sensor detect request with null node name");
    return(false);
  }

  m_map_sensor_reqs[vname]++;
  
  // Part 2: Determine requesting vehicle's index in the node_record vector
  unsigned int j, jsize = m_node_records.size();
  unsigned int vix = jsize;
  for(j=0; j<jsize; j++) {
    if(vname == m_node_records[j].getName())
      vix = j;
  }

  // If nothing is known about this vehicle, we don't know its 
  // position, so just return false.
  if(vix == jsize) {
    reportRunWarning("No NODE_REPORT received for " + vname);
    return(true); // return true - don't want to report another warning
  }

  // Part 3: If this vehicle has not initialized the sensor setting, 
  //         then perform a default setting.
  if(m_map_swath_width.count(vname) == 0)
    setVehicleSensorSetting(vname, 0, 0, true);


  // Part 4: Determine the vehicle turn rate and if the sensor is "on"
  double turn_rate = m_node_hdg_hist[vix].getTurnRate(2);
  bool sensor_on = true;
  if(turn_rate > m_sensor_max_turn_rate)
    sensor_on = false;

  // Part 5: Update the sensor region/polygon based on new position 
  //         and perhaps new sensor setting.
  updateNodePolygon(vix, sensor_on);
  // Possibly draw the swath
  if(m_show_swath) {
    if(m_show_pd || m_show_pfa) {
      string msg;
      string pd_str  = doubleToString(m_map_prob_detect[vname],2);
      string pfa_str = doubleToString(m_map_prob_false_alarm[vname],2);
      if(m_show_pd && m_show_pfa) 
	msg = pd_str + "/" + pfa_str;
      else if(m_show_pd && !m_show_pfa)
	msg = pd_str;
      else if(!m_show_pd && m_show_pfa)
	msg = "pfa=" + pfa_str;
      m_node_polygons[vix].set_msg(msg);
    }

    string poly_spec = m_node_polygons[vix].get_spec();
    Notify("VIEW_POLYGON", poly_spec);
  }

  // If the sensor is effectively off, then we're done
  if(!sensor_on)
    return(true);

  // Part 6: For each hazard, determine if the hazard is newly within
  // the sensor swath of the requesting vehicle.
    map<string, XYHazard>::iterator p;
  for(p=m_map_hazards.begin(); p!=m_map_hazards.end(); p++) {
    string hlabel = p->first;
    bool new_report = updateVehicleHazardStatus(vix, hlabel);  // detection dice
    if(new_report) 
      postHazardDetectionReport(hlabel, vname); 
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: handleSensorClear
//   Example: vname=alpha

bool HazardSensor_MOOSApp::handleSensorClear(const string& request)
{
  string vname;
  
  // Part 1: Parse the string request
  vector<string> svector = parseString(request, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "vname")
      vname = value;
  }

  if(vname == "") {
    reportRunWarning("Error. Sensor clear request with null node name");
    return(false);
  }

  reportEvent("Sensor clear request for vehicle " + vname);

  return(true);
}

//---------------------------------------------------------
// Procedure: handleClassifyRequest
//   Example: vname=alpha, label=421

bool HazardSensor_MOOSApp::handleClassifyRequest(const string& request)
{
  string vname, hlabel;
  
  // Part 1: Parse the string request
  vector<string> svector = parseString(request, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = tolower(stripBlankEnds(biteString(svector[i], '=')));
    string value = stripBlankEnds(svector[i]);
    if(param == "vname")
      vname = value;
    else if(param == "label")
      hlabel = value;
  }

  // Part 2: Sanity check
  if(vname == "") {
    reportRunWarning("Sensor classify request with null vehicle name");
    return(false);
  }

  if(hlabel == "") {
    reportRunWarning("Sensor classify request with null label");
    return(false);
  }

  // Note we dont include the label number to avoid bloating the event list
  reportEvent("Sensor classify request received from: " + vname);

  // Part 3: check whether there is a new classification result that may
  //         possibly be made. A new class result may be generated each 
  //         time a pass has been made over the hazard
  unsigned int queries = m_map_hazard_class_queries[hlabel];
  unsigned int hits    = m_map_hazard_hits[hlabel];
  unsigned int unclassified_hits = 0;
  if(hits > queries)
    unclassified_hits = hits - queries;
  if(unclassified_hits == 0) {
    reportRunWarning("Sensor classify request saturated: " + vname);
    return(false);
  }

  // Part 4: go ahead and post the classify report
  m_map_classify_reqs[vname]++;
  postHazardClassifyReport(hlabel, vname);  // classify dice inside

  return(true);
}

//---------------------------------------------------------
// Procedure: handleSensorConfig
//   Example: vname=alpha,width=50,pd=0.9

bool HazardSensor_MOOSApp::handleSensorConfig(const string& config,
					      const string& msg_src)
{
  // Part 1: Parse the incoming configuration request
  string vname, str_width, str_pd;
  vector<string> svector = parseString(config, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "vname")
      vname = value;
    else if(param == "width")
      str_width = value;
    else if(param == "pd")
      str_pd = value;
  }

  // Part 2: Check for correctness of incoming request 
  if((vname == "") || (vname != msg_src)) {
    reportConfigWarning("Bad sensor config request from: "+vname + " src: "+msg_src);
    return(false);
  }

  if(((str_width != "") && !isNumber(str_width)) || !isNumber(str_pd)) {
    reportConfigWarning("Bad sensor config request from: " + vname);
    return(false);
  }    
  
  // Part 3: Clip the numerical values if necessary
  double d_width = atof(str_width.c_str());
  double d_pd    = atof(str_pd.c_str());
  
  d_width = vclip_min(d_width, 0);
  d_pd    = vclip(d_pd, 0, 1);


    
  // Part 4: Process the sensor setting request
  if(str_width == "")
    return(setVehicleSensorSettingPD(vname, d_pd));
  else
    return(setVehicleSensorSetting(vname, d_width, d_pd));
}



//---------------------------------------------------------
// Procedure: addQueueMessage()

void HazardSensor_MOOSApp::addQueueMessage(const string& vname,
					   const string& varname,
					   const string& value)
{
  // If this is the first entry in the queue for this vehicle, set the timestamp
  // for this vehicle to the present time. To ensure even the first element in 
  // the queue has the proper delay.
  if(m_map_msgs_queued[vname].size() == 0)
    m_map_msg_last_queue_time[vname] = m_curr_time;

  VarDataPair pair(varname, value);
  m_map_msgs_queued[vname].push_back(pair);
}


//------------------------------------------------------------
// Procedure: postQueueMessages

void HazardSensor_MOOSApp::postQueueMessages()
{
  map<string, list<VarDataPair> >::iterator p;
  for(p=m_map_msgs_queued.begin(); p!=m_map_msgs_queued.end(); p++) {
    string vname = p->first;

    //  Calculate the last time since a msg was popped for this vehicle.
    double elapsed = m_curr_time - m_map_msg_last_queue_time[vname];
    if(elapsed < m_min_queue_msg_interval) 
      continue;
    
    if(m_map_msgs_queued[vname].size() == 0) 
      continue;

    // Ok, this vehicle is due for a message, so we're going to pop N=3 of them
    // We assume each classify report consists of three messages:
    // UHZ_CLASSIFY_REPORT, UHZ_CLASSIFY_REPORT_VNAME, VIEW_CIRCLE
    bool message_popped = false;
    for(unsigned int i=0; i<3; i++) {
      if(m_map_msgs_queued[vname].size() != 0) {
	message_popped = true;
	
	VarDataPair message = m_map_msgs_queued[vname].front();
	m_map_msgs_queued[vname].pop_front();
	
	string varname = message.get_var();
	if(message.is_string())
	  m_Comms.Notify(varname, message.get_sdata());
	else
	  m_Comms.Notify(varname, message.get_ddata());      
	if(strBegins(varname, "UHZ_HAZARD_REPORT_")) {
	  m_map_classify_answ[vname]++;
	  reportEvent(varname + " posted");
	}
      }
    }    

    // Only update timestamp if there was actually something in the queue
    if(message_popped)
      m_map_msg_last_queue_time[vname] = m_curr_time;
  }
}


//---------------------------------------------------------
// Procedure: postVisuals()

void HazardSensor_MOOSApp::postVisuals()
{
  if(m_show_hazards == false)
    return;

  map<string, XYHazard>::iterator p;
  for(p=m_map_hazards.begin(); p!=m_map_hazards.end(); p++) {
    
    XYHazard hazard = p->second;

    string color = m_color_hazard;
    string shape = m_shape_hazard;
    double width = m_width_hazard;
    double hr    = 0;
    if(hazard.hasResemblance())
      hr = hazard.getResemblance();
    
    if(hazard.getType() != "hazard") {
      color = m_color_benign;
      shape = m_shape_benign;
      width = m_width_benign;
    }    

    if(hazard.getColor() != "")
      color = hazard.getColor();
    if(hazard.getShape() != "")
      shape = hazard.getShape();
    if(hazard.getWidth() >= 0)
      width = hazard.getWidth();

    XYMarker marker;
    marker.set_vx(hazard.getX());
    marker.set_vy(hazard.getY());
    marker.set_label(hazard.getLabel());
    marker.set_width(width);
    marker.set_type(shape);
    if(hazard.getType() != "hazard")
      marker.set_transparency(1-hr);
    marker.set_active(true);
    marker.set_color("primary_color", color);
    string spec = marker.get_spec(); 
    Notify("VIEW_MARKER", spec);
  }
}

//------------------------------------------------------------
// Procedure: postHazardDetectionReport()
//     Notes: Example postings:
//            UHZ_DETECTION_REPORT_GILDA = "x=-125,y=-143,label=517"

void HazardSensor_MOOSApp::postHazardDetectionReport(string hazard_label, 
						     string vname)
{
  // Part 1: Sanity checking
  if(m_map_hazards.count(hazard_label) == 0) {
    reportRunWarning("Error: Unknown hazard label");
    return;
  }
  
  // Part 2: Prepare the hazard detection report
  XYHazard hazard = m_map_hazards[hazard_label];
  string   spec   = hazard.getSpec("hr,type,color,width,shape");
  
  Notify("UHZ_DETECTION_REPORT", (spec + ",vname=" + vname));
  Notify("UHZ_DETECTION_REPORT_"+toupper(vname), spec);

  // Part 3: Note the detection as another piece of data collected on this
  //         hazard. Then possible for another classify query to be answered
  m_map_hazard_hits[hazard_label]++;
  
  // Part 4: Build some local memo/debug messages.
  reportEvent("Detection report sent to vehicle: " + vname);
  m_map_detections[vname]++;

  // Part 5: Build/Post some visual artifacts using VIEW_CIRCLE
  if((m_circle_duration == -1) || (m_circle_duration > 0)) {
    double haz_x = hazard.getX();
    double haz_y = hazard.getY();
    //string label = hazard.getLabel();
    XYCircle circ(haz_x, haz_y, 10);
    circ.set_color("edge", "white");
    //circ.set_label(label);
    circ.set_color("fill", "white");
    circ.set_vertex_size(0);
    circ.set_edge_size(1);
    circ.set_transparency(0.3);
    circ.set_time(m_curr_time);
    circ.setDuration(m_circle_duration);
    Notify("VIEW_CIRCLE", circ.get_spec());
  }
}




//------------------------------------------------------------
// Procedure: postHazardClassifyReport()
//     Notes: Example postings:
//            UHZ_HAZARD_REPORT_GILDA = "x=-125,y=-143,type=benign,
//                               label=517,hazard=false"

void HazardSensor_MOOSApp::postHazardClassifyReport(string hazard_label, 
						    string vname)
{
  // Part 1: Sanity checking
  if(m_map_hazards.count(hazard_label) == 0) {
    reportRunWarning("Error: Unknown hazard label");
    return;
  }
  
  if(m_map_prob_classify.count(vname) == 0) {
    reportRunWarning("Error: No classifier info for: " + vname);
    return;
  }
    
  // Part 2: Get the hazard type info and do classification
  XYHazard raw_hazard = m_map_hazards[hazard_label];
  string   htype      = raw_hazard.getType();
  bool     is_hazard  = (htype == "hazard");

  double prob_classify = m_map_prob_classify[vname];
  double pc_prime      = prob_classify;

  if((!is_hazard) && raw_hazard.hasResemblance()) {
    double resemblance = raw_hazard.getResemblance();
    pc_prime += (1-prob_classify) * (1-resemblance);
  }
    
  // Here's where we role the classification dice. 
  int rand_int = rand() % 10000;
  int thresh   = (pc_prime * 10000) + 1;
  if(rand_int > thresh)
    is_hazard = !is_hazard;
  // Done rolling the dice and applying the consequences


  // Part 3: Build the report to be posted locally and eventually
  //         out to the source vehicle.

  XYHazard post_hazard = raw_hazard;
  if(is_hazard) 
    post_hazard.setType("hazard");
  else
    post_hazard.setType("benign");
  string hazard_spec = post_hazard.getSpec("hr,color,shape,width,xpos,ypos");

  string full_str = "vname=" + vname + "," + hazard_spec;


  addQueueMessage(vname, "UHZ_HAZARD_REPORT", full_str);
  addQueueMessage(vname, "UHZ_HAZARD_REPORT_"+toupper(vname), hazard_spec);

  // Part 4: Build some event messages/info.
  reportEvent("Classify report queued to vehicle: " + vname);
  m_map_detections[vname]++;

  // Part 5: Build/Post some visual artifacts using VIEW_CIRCLE
  if((m_circle_duration == -1) || (m_circle_duration > 0)) {
    double haz_x = raw_hazard.getX();
    double haz_y = raw_hazard.getY();
    //string label = raw_hazard.getLabel();
    XYCircle circ(haz_x, haz_y, 10);
    if(is_hazard) {
      circ.set_color("edge", "yellow");
      circ.set_color("fill", "yellow");
    }
    else {
      circ.set_color("edge", "green");
      circ.set_color("fill", "green");
    }
    //circ.set_label(label);
    circ.set_vertex_size(0);
    circ.set_edge_size(1);
    circ.set_transparency(0.3);
    circ.set_time(m_curr_time);
    //    circ.setDuration(m_circle_duration);
    addQueueMessage(vname, "VIEW_CIRCLE", circ.get_spec());
  }
}

//------------------------------------------------------------
// Procedure: updateVehicleHazardStatus

bool HazardSensor_MOOSApp::updateVehicleHazardStatus(unsigned int vix, 
						   string hazard_label)
{
  // Part 1: Sanity checking
  if(m_map_hazards.count(hazard_label)==0)
    return(false);
  
  if(vix >= m_node_records.size())
    return(false);
  
  string vname = m_node_records[vix].getName();
  if(m_map_swath_width.count(vname) == 0) {
    reportRunWarning("No sensor setting for: " + vname);
    return(false);
  }

  // Part 2:
  XYHazard hazard = m_map_hazards[hazard_label];

  double prob_detect      = m_map_prob_detect[vname];
  double prob_false_alarm = m_map_prob_false_alarm[vname];

  string tag = vname + "_" + hazard_label;

  bool previous_contained_status = m_map_hv_status[tag];

  // Now figure out what the new status should be
  double haz_x  = hazard.getX();
  double haz_y  = hazard.getY();

  bool new_contained_status = m_node_polygons[vix].contains(haz_x, haz_y);
  
  m_map_hv_status[tag] = new_contained_status;

  bool is_hazard = true;
  if(hazard.getType() != "hazard")
    is_hazard = false;

  if((new_contained_status == true) && (previous_contained_status == false)) {
    // Here's where we roll the detection dice!
    int rand_int = rand() % 10000;
    int thresh;
    if(is_hazard)
      thresh = (prob_detect * 10000) + 1;
    else {
      double coeff = 1;
      if(prob_false_alarm < 1) {
	if(hazard.isSetHR()) {
	  double haz_hr = hazard.getResemblance();
	  coeff = (prob_false_alarm + haz_hr) / 2;
	}
	else
	  coeff = prob_false_alarm;
      }
      thresh = (coeff * 10000) + 1;
    }
    if(rand_int < thresh)
      return(true);
  }

  return(false);
}

//------------------------------------------------------------
// Procedure: updateNodeRecords

bool HazardSensor_MOOSApp::updateNodeRecords(NodeRecord new_record)
{
  bool add_new_record = true;

  //new_record.setName(tolower(new_record.getName()));

  double heading = new_record.getHeading();

  unsigned int i, vsize = m_node_records.size();
  for(i=0; i<vsize; i++) {
    if(new_record.getName() == m_node_records[i].getName()) {
      add_new_record = false;
      // Make sure the timestamp is carried over to the updated 
      // record. It represents the last time a ping was made.
      double tstamp = m_node_records[i].getTimeStamp();
      m_node_records[i] = new_record;
      m_node_records[i].setTimeStamp(tstamp);
      m_node_hdg_hist[i].addHeadingTime(heading, m_curr_time);
    }
  }
  if(add_new_record) {
    XYPolygon null_poly;
    HeadingHistory hdg_hist;
    hdg_hist.addHeadingTime(heading, m_curr_time);
    m_node_records.push_back(new_record);
    m_node_hdg_hist.push_back(hdg_hist);
    m_node_polygons.push_back(null_poly);
  }

  return(add_new_record);
}

//------------------------------------------------------------
// Procedure: updateNodePolygon

bool HazardSensor_MOOSApp::updateNodePolygon(unsigned int ix, bool sensor_on)
{
  if((ix >= m_node_records.size()) || (ix >= m_node_polygons.size()))
    return(false);

  double x     = m_node_records[ix].getX();
  double y     = m_node_records[ix].getY();
  double hdg   = m_node_records[ix].getHeading();
  string vname = m_node_records[ix].getName();

  if(m_map_swath_width.count(vname) == 0) {
    reportRunWarning("Cant update poly for: " + vname + ". swath_wid unknown"); 
    return(false);
  }
  double swath_width = m_map_swath_width[vname];

  double phi, hypot;
  calcSwathGeometry(swath_width, phi, hypot);

  double x1,y1, hdg1 = angle360(hdg + (90-phi));
  double x2,y2, hdg2 = angle360(hdg + (90+phi));
  double x3,y3, hdg3 = angle360(hdg + (90-phi) + 180);
  double x4,y4, hdg4 = angle360(hdg + (90+phi) + 180);

  projectPoint(hdg1, hypot, x, y, x1, y1); 
  projectPoint(hdg2, hypot, x, y, x2, y2); 
  projectPoint(hdg3, hypot, x, y, x3, y3); 
  projectPoint(hdg4, hypot, x, y, x4, y4); 
  
  XYPolygon poly;
  poly.add_vertex(x1, y1);
  poly.add_vertex(x2, y2);
  poly.add_vertex(x3, y3);
  poly.add_vertex(x4, y4);

  poly.set_color("edge", "green");
  poly.set_vertex_size(0);
  poly.set_edge_size(0);

  if(sensor_on)
    poly.set_color("fill", "white");
  else
    poly.set_color("fill", "red");

  string label = "sensor_swath_" + vname;
  poly.set_label(label);
  poly.set_msg("_null_");
  
  poly.set_transparency(m_swath_transparency);

  m_node_polygons[ix] = poly;
  return(true);
}

//------------------------------------------------------------
// Procedure: setRandomNoiseAlgorithm

bool HazardSensor_MOOSApp::setRandomNoiseAlgorithm(string str)
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
// Procedure: setSwathTransparency

bool HazardSensor_MOOSApp::setSwathTransparency(string str)
{
  // No check for [0,1] range. Handled in polygon set_transparency()
  m_swath_transparency = atof(str.c_str());
  return(true);
}

//------------------------------------------------------------
// Procedure: setOptionsSummaryInterval

bool HazardSensor_MOOSApp::setOptionsSummaryInterval(string str)
{
  if(!isNumber(str))
    return(false);
  
  m_options_summary_interval = atof(str.c_str());
  m_options_summary_interval = vclip(m_options_summary_interval, 0, 120);
  return(true);
}

//------------------------------------------------------------
// Procedure: processHazardFile

bool HazardSensor_MOOSApp::processHazardFile(string filename)
{
  vector<string> lines = fileBuffer(filename);
  unsigned int i, vsize = lines.size();
  if(vsize == 0) {
    m_hazard_file = "Error reading: " + filename;
    return(false);
  }
  else
    m_hazard_file = filename;

  for(i=0; i<vsize; i++) {
    string left  = biteStringX(lines[i], '=');
    string right = lines[i];
    if(left == "hazard") {
      bool ok = addHazard(right);
      if(!ok)
	reportConfigWarning("Poorly specified hazard: " + right);
    }
  }
  return(true);
}

//------------------------------------------------------------
// Procedure: updateSwathGeometry
//
//   tan(phi) = len/wid
//   hypot    = wid/cos(phi) 
// 
//                             |----  swath_wid  -----|
//                                      
//      +---------------------------------------------+      ---
//      |   \                                     /   |       |
//      |       \                             /       |       |
//      |           \                     hypot       |       |  m_
//      |               \             /               |       |  swath_
//      |                   \     /    Phi            |       |  len
//      |----------------------o----------------------|       |
//      |                   /     \                   |       |
//      |               /             \               |       |
//      |           /                     \           |       |
//      |       /                             \       |       |
//      |   /                                     \   |       |
//      +---------------------------------------------+      ---
//
//

void HazardSensor_MOOSApp::calcSwathGeometry(double swath_wid,
					   double& phi, double& hypot)
{
  phi   = atan(m_swath_len / swath_wid);
  hypot = swath_wid / cos(phi);
  phi   = radToDegrees(phi);
}

//------------------------------------------------------------
// Procedure: sortSensorProperties
//   Purpose: Sort the three aligned vectors, 
//            m_sensor_prop_width, exp, and class
//            from lowest to highest based on the width
//
//      Note: The three vectors represent the basic properties of the
//            sensor. Each index in the array corresponds to one 
//            configuration choice. By choosing the width, one is also
//            choosing a ROC curve defined by Pfa = Pd^exp. Each index
//            also contains a value for the classifier algorithm. This
//            is a number between [0,1] representing the probability 
//            that the sensor will properly classify the object as either
//            a hazard or benign object, once it has been detected.

void HazardSensor_MOOSApp::sortSensorProperties()
{
  vector<double> new_sensor_prop_width;
  vector<double> new_sensor_prop_exp;
  vector<double> new_sensor_prop_class;

  unsigned int i, j, vsize = m_sensor_prop_width.size();
  vector<bool> v_hit(vsize, false);

  for(i=0; i<vsize; i++) {
    double min_width = -1;
    unsigned int min_index;
    for(j=0; j<vsize; j++) {
      double jwid = m_sensor_prop_width[j];
      if(!v_hit[j] && ((min_width == -1) || (jwid < min_width))) {
	min_index = j;
	min_width = jwid;
      }
    }
    v_hit[min_index] = true;
    
    new_sensor_prop_width.push_back(m_sensor_prop_width[min_index]);
    new_sensor_prop_exp.push_back(m_sensor_prop_exp[min_index]);
    new_sensor_prop_class.push_back(m_sensor_prop_class[min_index]);
  }

  m_sensor_prop_width = new_sensor_prop_width;
  m_sensor_prop_exp   = new_sensor_prop_exp;
  m_sensor_prop_class = new_sensor_prop_class;
}


//------------------------------------------------------------
// Procedure: setVehicleSensorSetting
//   Purpose: Handle a request to set the sensor setting for a given
//            vehicle. The setting is based on the requested swath
//            width of the sensor. Based on the width, a ROC curve is
//            implied by the "exp" value associated with the given width.
//            The user also chooses the probability of detection (Pd), 
//            and the probability of false-alarm, (Pfa) is determined 
//            based on the ROC curve, Pfa = Pd ^ exp.
//      Note: If guess is true, then a sensor setting is automatically  
//            chosen based on the avg width of possible sensor settings.
//            This may be useful if a vehicle starts using the sensor 
//            w/out ever requesting a particular sensor setting.
//      Note: A UHZ_CONFIG_ACK_<VNAME> acknowledgement is posted
//            and presumably bridged out to the vehicle.

bool HazardSensor_MOOSApp::setVehicleSensorSetting(string vname,
						   double width,
						   double pd,
						   bool guess)
{
  reportEvent("Setting sensor settings for: " + vname);

  // Part 1: Fit the request to one of the allowable sensor settings.

  // If no sensor properties have been configured, there is nothing to
  // choose from! Just return false. 
  unsigned int psize = m_sensor_prop_width.size();
  if(psize == 0) {
    reportEvent("No sensor properties to set settings for: " + vname);
    return(false);
  }

  // Handle the case where this function is being used to set the sensor
  // setting to *anything* reasonable.
  if(guess == true) {
    double wid_min = m_sensor_prop_width[0];
    double wid_max = m_sensor_prop_width[psize-1];
    double wid_avg = (wid_min + wid_max) / 2;
    width = wid_avg;
    pd    = 0.9;
  }

  // Ensure the requested sensor width is nonzero and pd is in [0,1]
  width = vclip_min(width, 0);
  pd    = vclip(pd, 0, 1);
    
  m_map_reset_swath_req[vname]++;
  // Part 2: Determine if this request is allowed, based on frequency
  double elapsed = m_curr_time - m_map_reset_swath_time[vname];
  if(elapsed < m_min_reset_interval) {
    reportEvent("Sensor reset denied (too soon) for vehicle: " + vname);
    return(false);
  }

  // Part 3: Get the set of properties based on the request.
  // Find the property index (pix) with closest match to request. If no
  // exact width request, MATCH TO THE NEXT LOWEST. If requested width is
  // smaller than the smallest property width, take the smallest.
  unsigned int pix = 0;
  double       smallest_delta = 0;
  bool         smallest_delta_set = false;

  unsigned int j, jsize = m_sensor_prop_width.size();
  for(j=0; j<jsize; j++) {
    double jwidth = m_sensor_prop_width[j];

    // first determine if this width is limited and at its limit
    unsigned int smax = m_sensor_prop_max[j];
    if((smax == 0) || (sensorSwathCount(jwidth,vname) < smax)) {
      double delta = abs(width - jwidth);
      if(!smallest_delta_set || (delta < smallest_delta)) {
	smallest_delta_set = true;
	smallest_delta = delta;
	pix = j;
      }
    }
  }
  
  double selected_width = m_sensor_prop_width[pix];
  double selected_exp   = m_sensor_prop_exp[pix];
  double selected_class = m_sensor_prop_class[pix];
  double selected_pd    = pd;
  double implied_pfa    = pow(selected_pd, selected_exp);
  
  // Part 4: Good. We have new selected sensor settings for this vehicle
  //         Now store selection locally.
  m_map_swath_width[vname]      = selected_width;
  m_map_swath_roc_exp[vname]    = selected_exp;
  m_map_prob_detect[vname]      = selected_pd;
  m_map_prob_false_alarm[vname] = implied_pfa;
  m_map_prob_classify[vname]    = selected_class;

  postConfigurationAck(vname);

  // Part 5: Update our local stats reflecting the number of updates
  //         for this vehicle and the timestamp of this update
  m_map_reset_swath_total[vname]++;
  m_map_reset_swath_time[vname] = m_curr_time;
	       
  return(true);
}


//------------------------------------------------------------
// Procedure: setVehicleSensorSettingPD

bool HazardSensor_MOOSApp::setVehicleSensorSettingPD(string vname, double new_pd)
{
  reportEvent("Setting sensor PD settings for: " + vname);

  // First check if the primary sensor setting (swath width) has been chosen.
  // If not a default swath setting it chosen
  if(m_map_swath_roc_exp.count(vname) == 0)
    setVehicleSensorSetting(vname, 0, 0, true);

  // Ensure the requested sensor width is nonzero and pd is in [0,1]
  new_pd = vclip(new_pd, 0, 1);
    
  double current_exp = m_map_swath_roc_exp[vname];
  double new_pfa     = pow(new_pd, current_exp);
  
  m_map_prob_detect[vname]      = new_pd;
  m_map_prob_false_alarm[vname] = new_pfa;

  m_map_reset_pd_total[vname]++;
  m_map_reset_pd_time[vname] = m_curr_time;

  postConfigurationAck(vname);
	       
  return(true);
}

//------------------------------------------------------------
// Procedure: postConfigurationAck

void HazardSensor_MOOSApp::postConfigurationAck(string vname)
{
  if(m_map_swath_width.count(vname) == 0) {
    reportRunWarning("Failed Configuration Ack for Unknown Vehicle: " + vname);
    return;
  }

  double current_width  = m_map_swath_width[vname];
  double current_pd     = m_map_prob_detect[vname];
  double current_pfa    = m_map_prob_false_alarm[vname];
  double current_class  = m_map_prob_classify[vname];

  string var = "UHZ_CONFIG_ACK_" + toupper(vname);
  string msg = "vname=" + vname;
  msg += ",width="  + doubleToStringX(current_width,1);
  msg += ",pd="     + doubleToStringX(current_pd,3);
  msg += ",pfa="    + doubleToStringX(current_pfa,3);
  msg += ",pclass=" + doubleToStringX(current_class,3);
  Notify(var, msg);
}

//------------------------------------------------------------
// Procedure: sensorSwathCount()
//   Purpose: Return the number of *other* vehicles known to this simulator 
//            that have the given swath width setting.

unsigned int HazardSensor_MOOSApp::sensorSwathCount(double swath, string vname)
{
  unsigned int count = 0;

  map<string, double>::iterator p;
  for(p=m_map_swath_width.begin(); p!=m_map_swath_width.end(); p++) {
    string this_vname = p->first;
    double this_swath = p->second;
    if((swath == this_swath) && (vname != this_vname))
      count++;
  }
  
  return(count);
}
  

//------------------------------------------------------------
// Procedure: buildReport
//      Note: A virtual function of the AppCastingMOOSApp superclass, 
//            conditionally invoked if either a terminal or appcast 
//            report is needed.

bool HazardSensor_MOOSApp::buildReport()
{
  m_msgs << "Hazard File: (" + m_hazard_file + ") "        << endl;
  m_msgs << "     Hazard: " << m_hazard_file_hazard_cnt << endl;
  m_msgs << "     Benign: " << m_hazard_file_benign_cnt << endl;
  m_msgs << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "Sensor Configuration Options"                 << endl;
  m_msgs << "============================================" << endl;

  ACTable actab(3);
  
  actab << "Width | Exp |  Classify";
  actab.addHeaderLines();

  unsigned int i, vsize = m_sensor_prop_width.size(); 
  for(i=0; i<vsize; i++) {
    string s_width = doubleToString(m_sensor_prop_width[i],1);
    string s_exp   = doubleToString(m_sensor_prop_exp[i],1);
    string s_class = doubleToString(m_sensor_prop_class[i],3);
    actab << s_width << s_exp << s_class;
  }
  m_msgs << actab.getFormattedString();

  m_msgs << endl << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "Sensor Settings / Stats for known vehicles: " << endl;
  m_msgs << "============================================" << endl;

  actab = ACTable(8);
  actab << "Vehicle | Swath |    |     |        | Try Swath | Swath  | Pd     ";
  actab << "Name    | Width | Pd | Pfa | Pclass | Resets    | Resets | Resets ";
  actab.addHeaderLines();

  map<string, double>::iterator p;
  for(p=m_map_swath_width.begin(); p!=m_map_swath_width.end(); p++) {
    string vname = p->first;

    double last_sw_reset_dbl = 0;
    if(m_map_reset_swath_time.count(vname) != 0)
      last_sw_reset_dbl = m_map_reset_swath_time[vname] - m_start_time;
    string last_sw_reset_str = doubleToString(last_sw_reset_dbl,0);

    double last_pd_reset_dbl = 0;
    if(m_map_reset_pd_time.count(vname) != 0) 
      last_pd_reset_dbl = m_map_reset_pd_time[vname] - m_start_time;
    string last_pd_reset_str = doubleToString(last_pd_reset_dbl,0);


    string width     = doubleToString(m_map_swath_width[vname],1);
    string pd        = doubleToString(m_map_prob_detect[vname],3);
    string pfa       = doubleToString(m_map_prob_false_alarm[vname],3);
    string pclass    = doubleToString(m_map_prob_classify[vname],3);
    string sw_resets_req = uintToString(m_map_reset_swath_req[vname]);
    string sw_resets = uintToString(m_map_reset_swath_total[vname]) 
      + "(" + last_sw_reset_str + ")";
    string pd_resets = uintToString(m_map_reset_pd_total[vname]) 
      + "(" + last_pd_reset_str + ")";

    actab << vname << width << pd << pfa << pclass << sw_resets_req <<
      sw_resets << pd_resets;
  }
  m_msgs << actab.getFormattedString() << endl << endl << endl;

  actab = ACTable(5);
  actab << "Vehicle | Sensor   |            | Classify | Classify ";
  actab << "Name    | Requests | Detections | Requests | Answers  ";
  actab.addHeaderLines();

  for(p=m_map_swath_width.begin(); p!=m_map_swath_width.end(); p++) {
    string vname     = p->first;

    string sensor_reqs   = uintToString(m_map_sensor_reqs[vname]);
    string detects       = uintToString(m_map_detections[vname]);
    string classify_reqs = uintToString(m_map_classify_reqs[vname]);
    string classify_answ = uintToString(m_map_classify_answ[vname]);
    
    actab << vname << sensor_reqs << detects << classify_reqs << classify_answ;
  }
  m_msgs << actab.getFormattedString();
  return(true);
}


