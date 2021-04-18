/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardSensor_MOOSApp.cpp                             */
/*    DATE: Jan 28th 2012                                        */
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
  m_ignore_resemblances      = false;
  m_max_vehicle_speed        = 2.05;

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
  m_color_hazard = "red";
  m_color_benign = "light_blue";
  m_shape_hazard = "triangle";
  m_shape_benign = "triangle";
  m_width_hazard = 8;
  m_width_benign = 8;
  m_swath_len    = 5;
  m_swath_transparency = 0.2;

  m_ac.setMaxEvents(20);
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
  
    if(!handled) 
      reportRunWarning("Unhandled mail: " + key);
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

  Register("NODE_REPORT", 0);
  Register("UHZ_SENSOR_REQUEST", 0);
  Register("UHZ_CLASSIFY_REQUEST", 0);
  Register("UHZ_CONFIG_REQUEST", 0);
  Register("PMV_CONNECT", 0);
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

  processSensorRequests();
  processClassifyQueue();

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
    else if((param == "max_vehicle_speed") && isNumber(value)) {
      m_max_vehicle_speed = atof(value.c_str());
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
    else if(param == "ignore_resemblances")
      handled = setBooleanOnString(m_ignore_resemblances, value);
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
  string vname;  
  vector<string> svector = parseString(request, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "vname")
      vname = value;
  }

  if(vname == "") {
    reportRunWarning("Sensor detect request with null node name");
    return(false);
  }

  m_map_sensor_reqs[vname]++;
  m_map_last_sensor_request[vname] = MOOSTime();
  return(true);
}


//---------------------------------------------------------
// Procedure: processSensorRequests()

bool HazardSensor_MOOSApp::processSensorRequests()
{
  map<string,double>::iterator p;
  for(p=m_map_last_sensor_request.begin(); p!=m_map_last_sensor_request.end(); p++) {
    string vname = p->first;
    double tstamp = p->second;
    double elapsed = MOOSTime() - tstamp;
    if(elapsed < 600)
      processSensorRequest(vname);
  }
  return(true);
}


//---------------------------------------------------------
// Procedure: processSensorRequest(vname)

bool HazardSensor_MOOSApp::processSensorRequest(string vname)
{
  // Part 2: Determine requesting vehicle's index in the node_record vector
  unsigned int j, jsize = m_node_records.size();
  unsigned int vix = jsize;
  for(j=0; j<jsize; j++) {
    if(vname == m_node_records[j].getName())
      vix = j;
  }

  Notify("PSR", vname);

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

  double vehicle_speed = m_node_records[vix].getSpeed();
  if(vehicle_speed > m_max_vehicle_speed)
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
    XYHazard hazard = p->second;
    bool is_hazard = (hazard.getType() == "hazard");

    bool dice_roll_needed = updateVehicleHazardStatus(vix, hlabel);
    if(dice_roll_needed) {
      if(is_hazard)
	m_map_haz_detect_chances[vname]++;
      else
	m_map_ben_detect_chances[vname]++;
      
      int    rand_int  = rand() % 10000;
      double dice_roll = (double)(rand_int) / 10000;

      bool detect_result_normal = rollDetectionDiceNormal(vix, hlabel, dice_roll);
      bool detect_result_aspect = rollDetectionDiceAspect(vix, hlabel, dice_roll);
      if(detect_result_normal) {
	if(is_hazard) 
	  m_map_haz_detect_reports[vname]++;
	else
	  m_map_ben_detect_reports[vname]++;
	postHazardDetectionReport(hlabel, vix); 
      }
      else if(detect_result_aspect) 
	postHazardDetectionAspect(hlabel);
    }
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
  string vname, haz_label, action, priority = "50";

  // Part 1: Parse the string request and announce the request in an event
  vector<string> svector = parseString(request, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "vname")
      vname = value;
    else if(param == "label")
      haz_label = value;
    else if(param == "priority")
      priority = value;
    else if(param == "action")
      action = value;
  }

  if((action == "clear") && (m_map_classify_queue.count(vname) != 0)) {
    m_map_classify_queue[vname].clear();
    reportEvent("Classify Queue cleared for " + vname);
    return(true);
  }
  
  m_map_classify_reqs[vname]++;

  // Part 2: Sanity checks
  if(vname == "") 
    return(reportRunWarning("Classify request with null vehicle name"));
  if(haz_label == "") 
    return(reportRunWarning("Classify request with null label"));
  if(!isNumber(priority))
    return(reportRunWarning("Classify request with bad priority:" + priority));
  if(m_map_hazards.count(haz_label) == 0) 
    return(reportRunWarning(vname+" class req for unknown haz label:"+haz_label));
  if(m_map_prob_classify.count(vname) == 0)
    return(reportRunWarning("Error: No classifier config info for: " + vname));

  // Part 3: check if a new classify req may be made. A new class req may be 
  //         accepted each time a pass has been made over the hazard
  if((m_map_hazard_passes.count(vname)==0) || (m_map_hazard_passes[vname].size()==0))
    return(reportRunWarning("Sensor classify request saturated: " + vname));
  double vehicle_heading = m_map_hazard_passes[vname].front();
  m_map_hazard_passes[vname].pop_front();

  // Part 4: Get hazard, and true type info
  XYHazard true_hazard = m_map_hazards[haz_label];
  bool     is_hazard   = (true_hazard.getType() == "hazard");

  // Part 5: Perhaps modify the PC based on resemblance factor
  double prob_classify = m_map_prob_classify[vname];
  double pc_prime      = prob_classify;

  if((!is_hazard) && true_hazard.hasResemblance() && !m_ignore_resemblances) {
    double resemblance = true_hazard.getResemblance();
    pc_prime += (1-prob_classify) * (1-resemblance);
  }

  // Part 6: Perhaps modify the PC based on aspect angle
  bool use_aspect_info = true_hazard.hasAspect();
  if(!true_hazard.hasAspectRangeMin() || !true_hazard.hasAspectRangeMax())
    use_aspect_info = false;
  
  double pc_dbl_prime = pc_prime;
  if(use_aspect_info) {
    double hazard_aspect_opt = true_hazard.getAspect();
    double aspect_diff = aspectDiff(vehicle_heading, hazard_aspect_opt);
    
    double range_min = true_hazard.getAspectRangeMin();
    double range_max = true_hazard.getAspectRangeMax();
    
    double degrade_pct = 0;
    if(aspect_diff <= range_min)
      degrade_pct = 0;
    else if(aspect_diff >= range_max)
      degrade_pct = 1;
    else
      degrade_pct = (aspect_diff - range_min) / (range_max - range_min);
    degrade_pct = vclip(degrade_pct, 0, 1);
  
    pc_dbl_prime = 0.5 + ((1-degrade_pct) * (pc_prime-0.5));
  }

  // Part 7: Roll the classification dice. 
  double dice_roll = randomDouble(0, 1);

  // Example:              | dice_roll
  //   pc = 0.9            |   0.50 --> proper classifcation
  //   pc_prime = 0.94     |   0.98 --> incorrect classification
  //   pc_dbl_prime = 0.75 |   0.85 --> incorrect classification, aspect_affected

  bool aspect_affected = false;
  if(dice_roll > pc_dbl_prime) {
    is_hazard = !is_hazard;     // apply incorrect classification
    if(dice_roll <= pc_prime)
      aspect_affected = true;   // Declare aspect_affected
  }

  string msg = "pc=" + doubleToString(prob_classify);
  msg += ", pcp=" + doubleToString(pc_prime);
  msg += ", pcdb=" + doubleToString(pc_dbl_prime);
  msg += ", dice=" + doubleToString(dice_roll);
  msg += ", aff=" + boolToString(aspect_affected);
  reportEvent(msg);

  // Part 8: Build the report to be added to the classify queue
  XYHazard post_hazard = true_hazard;
  if(is_hazard) 
    post_hazard.setType("hazard");
  else 
    post_hazard.setType("benign");
  string hazard_spec = post_hazard.getSpec("hr,color,shape,width,xpos,ypos");

  // Part 9: Create an entry for the classify queue and push it on...
  ClassifyEntry entry;
  entry.setHazard(post_hazard);
  entry.setAspectAffected(aspect_affected);
  entry.setPriority(atof(priority.c_str()));

  bool clock_reset = false;
  if(m_map_classify_queue[vname].size() == 0)
    clock_reset = true;

  if(action == "top") {
    m_map_classify_queue[vname].pushTop(entry);
    clock_reset = true;
  }
  else
    m_map_classify_queue[vname].push(entry);  

  if(clock_reset)
    m_map_classify_last_time[vname] = m_curr_time;

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

//------------------------------------------------------------
// Procedure: processClassifyQueue

void HazardSensor_MOOSApp::processClassifyQueue()
{
  map<string, ClassifyQueue>::iterator p;
  for(p=m_map_classify_queue.begin(); p!=m_map_classify_queue.end(); p++) {
    string vname = p->first;

    //  Part 1: Determine if an entry is to be popped for this vehicle.
    if(m_map_classify_last_time[vname] == 0)
      m_map_classify_last_time[vname] = m_curr_time;

    double elapsed = m_curr_time - m_map_classify_last_time[vname];
    if(elapsed < m_min_queue_msg_interval) 
      continue;
    if(m_map_classify_queue[vname].size() == 0) 
      continue;
    m_map_classify_last_time[vname] = m_curr_time;

    // Part 2: Get the entry
    ClassifyEntry entry = m_map_classify_queue[vname].pop();

    bool aspect_affected = entry.getAspectAffected();

    // Part 3: Build and post the vehicle specific hazard report
    string spec = entry.getHazard().getSpec("hr");
    Notify("UHZ_HAZARD_REPORT_"+toupper(vname), spec);
    m_map_classify_answ[vname]++;
    
    // Part 4: Build and post the general hazard report
    string full_spec = "vname=" + vname + "," + spec;
    Notify("UHZ_HAZARD_REPORT", full_spec);

    // Part 5: Build/Post visual artifacts using VIEW_CIRCLE
    if((m_circle_duration == -1) || (m_circle_duration > 0)) {
      XYHazard post_hazard = entry.getHazard();
      bool is_hazard = (tolower(post_hazard.getType()) == "hazard");
      XYCircle circ(post_hazard.getX(), post_hazard.getY(), 10);
      if(is_hazard) {
	circ.set_color("edge", "red");
	circ.set_color("fill", "red");
      }
      else {
	circ.set_color("edge", "white");
	circ.set_color("fill", "white");
      }
#if 0
      if(is_hazard) {
	circ.set_color("edge", "yellow");
	circ.set_color("fill", "yellow");
      }
      else {
	circ.set_color("edge", "green");
	circ.set_color("fill", "green");
      }
#endif
      //circ.set_label(label);
      circ.setDrawVertices(5);
      circ.set_vertex_size(0);
      circ.set_edge_size(1);
      circ.set_transparency(0.3);
      circ.set_time(m_curr_time);
      // circ.setDuration(m_circle_duration);

      if(aspect_affected) {
      	circ.set_color("fill", "pink");
	circ.set_edge_size(2);
	circ.set_transparency(0.5);
      }

      Notify("VIEW_CIRCLE", circ.get_spec());      
    }
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
    double hr    = 0.5;
    double hx    = hazard.getX();
    double hy    = hazard.getY();
    if(hazard.hasResemblance())
      hr = hazard.getResemblance();
    
    double aspect = -1;
    if(hazard.hasAspect())
      aspect = hazard.getAspect();

    double aspect_line_len = 15;
    if(hazard.hasAspectRangeMin() && hazard.hasAspectRangeMax()) {
      double range_min = hazard.getAspectRangeMin();
      double range_max = hazard.getAspectRangeMax();
      double range_avg = (range_min + range_max) / 2.0;
      double range_pct = range_avg / 90;
      aspect_line_len = 5 + (range_pct * 25);
    }

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
    marker.set_vx(hx);
    marker.set_vy(hy);
    marker.set_label(hazard.getLabel());
    marker.set_width(width);
    marker.set_type(shape);
    if(hazard.getType() != "hazard")
      marker.set_transparency(1-hr);
    marker.set_active(true);
    marker.set_color("primary_color", color);
    string spec = marker.get_spec(); 
    Notify("VIEW_MARKER", spec);

    if(aspect > 0) {
      double x1,y1,x2,y2;
      projectPoint(aspect, aspect_line_len, hx, hy, x1, y1);
      projectPoint(aspect+180, aspect_line_len, hx, hy, x2, y2);
      XYSegList segl;
      segl.add_vertex(x1, y1);
      segl.add_vertex(x2, y2);
      segl.set_vertex_size(0);
      segl.set_edge_size(1);
      segl.set_color("edge", "gray70");
      
      Notify("VIEW_SEGLIST", segl.get_spec());
    }
  }
}

//------------------------------------------------------------
// Procedure: updateVehicleHazardStatus

bool HazardSensor_MOOSApp::updateVehicleHazardStatus(unsigned int vix, 
						     string hazard_label)
{
  // Part 1: Sanity checking
  if((m_map_hazards.count(hazard_label)==0) || (vix >= m_node_records.size()))
    return(false);
  
  string vname = m_node_records[vix].getName();

  if(m_map_swath_width.count(vname) == 0) {
    reportRunWarning("No sensor setting for: " + vname);
    return(false);
  }

  // Part 2: Determine the hazard status (whether or not the hazard is in the
  //         sensor swath of the vehicle given by vix.
  XYHazard hazard = m_map_hazards[hazard_label];

  string tag    = vname + "_" + hazard_label;
  double haz_x  = hazard.getX();
  double haz_y  = hazard.getY();
  bool   new_contained_status = m_node_polygons[vix].contains(haz_x, haz_y);

  // Part 3: Update the new status and determine if the hazard has just now
  // entered the swath
  bool previous_contained_status = m_map_hv_status[tag];

  m_map_hv_status[tag] = new_contained_status;

  if((new_contained_status == true) && (previous_contained_status == false))
    return(true);

  return(false);
}

//------------------------------------------------------------
// Procedure: rollDetectionDiceNormal

bool HazardSensor_MOOSApp::rollDetectionDiceNormal(unsigned int vix, 
						   string hazard_label,
						   double dice_roll)
{
  // Part 1: Sanity checking
  if((m_map_hazards.count(hazard_label)==0) || (vix >= m_node_records.size()))
    return(false);
  
  // Part 2: Get the vehicle and hazard information we need
  XYHazard hazard    = m_map_hazards[hazard_label];
  string vname       = m_node_records[vix].getName();
  double prob_detect = m_map_prob_detect[vname];
  double prob_falarm = m_map_prob_false_alarm[vname];

  // Part 3: Apply the roll of the dice, given in the dice_roll range of [0,1).
  if(hazard.getType() == "hazard")
    return(dice_roll < prob_detect);
  
  string info = "DetectionNormal: pd: ";
  info += doubleToString(prob_detect) + " pfa:" + doubleToString(prob_falarm);

  reportEvent(info);

  double dice_thresh = prob_falarm;
  if(!m_ignore_resemblances && hazard.isSetHR()) 
    dice_thresh = (prob_falarm + hazard.getResemblance()) / 2;
  
  return(dice_roll < dice_thresh);
}

//------------------------------------------------------------
// Procedure: rollDetectionDiceAspect

bool HazardSensor_MOOSApp::rollDetectionDiceAspect(unsigned int vix, 
						   string hazard_label,
						   double dice_roll)
{
  // Part 1: Sanity checking
  if((m_map_hazards.count(hazard_label)==0) || (vix >= m_node_records.size()))
    return(false);
  
  // Part 2: Get the hazard information, check if there is an aspect associated
  XYHazard hazard = m_map_hazards[hazard_label];
  bool use_aspect_info = true;
  if(!hazard.hasAspect() || !hazard.hasAspectRangeMin() || !hazard.hasAspectRangeMax())
    use_aspect_info = false;
  if(!use_aspect_info)
    return(rollDetectionDiceNormal(vix, hazard_label, dice_roll));
  
  // Part 3: Hazard has aspect, so now get the veicle info we need
  string vname       = m_node_records[vix].getName();
  double prob_detect = m_map_prob_detect[vname];
  double prob_falarm = m_map_prob_false_alarm[vname];
  
  // Part 4: Determine a new Probability of False Alarm base on degradation 
  //         of sensor performance due to deviation from optimal aspect angle.
  // Part A: Determine the how far off the present vehcle aspect is compared
  //         to the optimal aspect for this particular hazard.
  double vehicle_heading   = m_node_records[vix].getHeading();
  double hazard_aspect_opt = hazard.getAspect();
  double aspect_diff = aspectDiff(vehicle_heading, hazard_aspect_opt);

  // Part B: Determine the sensor degradation due to the difference between
  //         the present aspect and optimal aspect for this object

  double range_min = hazard.getAspectRangeMin();
  double range_max = hazard.getAspectRangeMax();
  
  double degrade_pct = 0;
  if(aspect_diff <= range_min)
    degrade_pct = 0;
  else if(aspect_diff >= range_max)
    degrade_pct = 1;
  else
    degrade_pct = (aspect_diff - range_min) / (range_max - range_min);
  degrade_pct = vclip(degrade_pct, 0, 1);
  

  // Part C: Determine the new ROC curve exponent considering degradation
  //         e.g., If cur_exp=5, and degrade_pct=0.25, then new_exp=4, since 4
  //         is a 25% degradation between 5 and 1 (1 is worst ROC curve, linear)
  double cur_exp = m_map_swath_roc_exp[vname];
  double new_exp = cur_exp - (degrade_pct * (cur_exp-1));
  
  string goo = "cur_exp=" + doubleToStringX(cur_exp);
  goo += "new_exp=" + doubleToStringX(new_exp);
  reportEvent(goo);

  // Part D: Calculate the new PFA given the PD and new EXP values
  prob_falarm = pow(prob_detect, new_exp);

  string foo = "aspect_diff=" + doubleToString(aspect_diff);
  foo += " degrade_pct=" + doubleToString(degrade_pct);
  reportEvent(foo);

  string info = "DetectionAspect: pd: ";
  info += doubleToString(prob_detect) + " pfa:" + doubleToString(prob_falarm);

  reportEvent(info);

  // Part 5: Apply the roll of the dice, given in the dice_roll range of [0,1).
  if(hazard.getType() == "hazard")
    return(dice_roll < prob_detect);
  
  double dice_thresh = prob_falarm;
  if(!m_ignore_resemblances && hazard.isSetHR()) 
    dice_thresh = (prob_falarm + hazard.getResemblance()) / 2;
  return(dice_roll < dice_thresh);
}

//------------------------------------------------------------
// Procedure: postHazardDetectionReport()
//     Notes: Example postings:
//            UHZ_DETECTION_REPORT_GILDA = "x=-125,y=-143,label=517"

void HazardSensor_MOOSApp::postHazardDetectionReport(string hazard_label, 
						     unsigned int vix)
{

  // Part 1: Sanity checking
  if((m_map_hazards.count(hazard_label)==0) || (vix >= m_node_records.size()))
    return;
  
  // Part 2: Prepare the hazard detection report
  XYHazard hazard  = m_map_hazards[hazard_label];
  string   vname   = m_node_records[vix].getName();
  double   heading = m_node_records[vix].getHeading();
  string   spec    = hazard.getSpec("hr,type,color,width,shape");
  
  Notify("UHZ_DETECTION_REPORT", (spec + ",vname=" + vname));
  Notify("UHZ_DETECTION_REPORT_"+toupper(vname), spec);

  // Part 3: Note the detection as another piece of data collected on this
  //         hazard. Then possible for another classify query to be answered
  m_map_hazard_passes[vname].push_back(heading); 

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
    circ.setDrawVertices(12);
    circ.setDuration(m_circle_duration);
    Notify("VIEW_CIRCLE", circ.get_spec());
  }
}

//------------------------------------------------------------
// Procedure: postHazardDetectionAspect()

void HazardSensor_MOOSApp::postHazardDetectionAspect(string hazard_label)
{
  // Part 1: Sanity checking
  if(m_map_hazards.count(hazard_label) == 0)
    return;
  
  // Part 2: Prepare the hazard detection report
  XYHazard hazard = m_map_hazards[hazard_label];
  
  // Part 5: Build/Post some visual artifacts using VIEW_CIRCLE
  if((m_circle_duration == -1) || (m_circle_duration > 0)) {
    double haz_x = hazard.getX();
    double haz_y = hazard.getY();
    XYCircle circ(haz_x, haz_y, 10);
    circ.set_color("edge", "pink");
    circ.set_color("fill", "pink");
    circ.set_vertex_size(0);
    circ.set_edge_size(1);
    circ.set_transparency(0.3);
    circ.set_time(m_curr_time);
    circ.setDrawVertices(12);
    circ.setDuration(m_circle_duration);
    Notify("VIEW_CIRCLE", circ.get_spec());
  }
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
  vector<unsigned int> new_sensor_prop_max;

  unsigned int i, j, vsize = m_sensor_prop_width.size();
  vector<bool> v_hit(vsize, false);

  for(i=0; i<vsize; i++) {
    double min_width = -1;
    unsigned int min_index = 0;
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
    new_sensor_prop_max.push_back(m_sensor_prop_max[min_index]);
  }

  m_sensor_prop_width = new_sensor_prop_width;
  m_sensor_prop_exp   = new_sensor_prop_exp;
  m_sensor_prop_class = new_sensor_prop_class;
  m_sensor_prop_max   = new_sensor_prop_max;
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

  ACTable actab(4);
  
  actab << "Width | Exp |  Classify | Max";
  actab.addHeaderLines();

  unsigned int i, vsize = m_sensor_prop_width.size(); 
  for(i=0; i<vsize; i++) {
    string s_width = doubleToString(m_sensor_prop_width[i],1);
    string s_exp   = doubleToString(m_sensor_prop_exp[i],1);
    string s_class = doubleToString(m_sensor_prop_class[i],3);
    string s_max   = uintToString(m_sensor_prop_max[i]);
    actab << s_width << s_exp << s_class << s_max;
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

  actab = ACTable(7);
  actab << "Vehicle | Sensor |            | FA   | Det  | Classify | Classify";
  actab << "Name    | Reqs   | Detections | Rate | Rate | Requests | Answers ";
  actab.addHeaderLines();

  for(p=m_map_swath_width.begin(); p!=m_map_swath_width.end(); p++) {
    string vname     = p->first;

    string sensor_reqs   = uintToString(m_map_sensor_reqs[vname]);
    string detects       = uintToString(m_map_detections[vname]);

    string s_d_rate = "n/a";
    string s_fa_rate = "n/a";
    
    if(m_map_haz_detect_chances[vname] > 0) {
      double d_rate = ((double)(m_map_haz_detect_reports[vname])) /
	((double)(m_map_haz_detect_chances[vname]));
      s_d_rate = doubleToString(d_rate,2);
    }
    if(m_map_ben_detect_chances[vname] > 0) {
      double fa_rate = ((double)(m_map_ben_detect_reports[vname])) /
	((double)(m_map_ben_detect_chances[vname]));
      s_fa_rate = doubleToString(fa_rate,2);
    }

    string classify_reqs = uintToString(m_map_classify_reqs[vname]);
    string classify_answ = uintToString(m_map_classify_answ[vname]);
    
    actab << vname << sensor_reqs << detects << s_fa_rate << s_d_rate <<
      classify_reqs << classify_answ;
  }
  m_msgs << actab.getFormattedString();

  return(true);
}









