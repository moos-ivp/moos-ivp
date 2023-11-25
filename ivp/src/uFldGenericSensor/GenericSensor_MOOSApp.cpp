/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: GenericSensor_MOOSApp.cpp                            */
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
#include "GenericSensor_MOOSApp.h"
#include "ColorParse.h"
#include "XYCircle.h"
#include "NodeRecordUtils.h"
#include "GeomUtils.h"
#include "ACTable.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

GenericSensor_MOOSApp::GenericSensor_MOOSApp()
{
  // State Variables
  // -------------------------------------------------------------
  m_reports           = 0;
  m_last_summary_time = 0;    // last time settings options summary posted

  // Configuration variables
  // -------------------------------------------------------------
  m_min_sensor_interval  = 30;       // seconds
  m_min_reset_interval   = 300;      // seconds
  m_term_report_interval = 0.8;      // realtime (non-timewarp) seconds
  m_sensor_transparency  = 0.75;     // visual preference
  m_options_summary_interval = 10;   // in timewarped seconds
  
  // Visual preferences
  m_show_source_pts    = true;
  m_color_source_pts   = "green";
  m_scope_transparency = 0.25;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool GenericSensor_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    
    string key  = msg.GetKey();
    string sval = msg.GetString();
    string src_app = msg.GetSource();
    string src_community = msg.GetCommunity();
    
    if(key == "NODE_REPORT")
      handleMailNodeReport(sval);
    
    else if(key == "UGS_SENSOR_REQUEST")
      handleMailSensorRequest(sval);
    
    else if(key == "UGS_CONFIG_REQUEST")
      handleMailSensorConfig(sval, src_community);
    
    else 
      reportRunWarning("Unhandled Mail: " + key);    
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool GenericSensor_MOOSApp::OnConnectToServer()
{
  registerVariables();  
  return(true);
}


//------------------------------------------------------------
// Procedure: registerVariables

void GenericSensor_MOOSApp::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("NODE_REPORT", 0);
  Register("UGS_SENSOR_REQUEST", 0);
  Register("UGS_CONFIG_REQUEST", 0);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool GenericSensor_MOOSApp::Iterate()
{
  AppCastingMOOSApp::Iterate();

  // Consider posting a settings-options summary
  double elapsed_time = m_curr_time - m_last_summary_time;
  if(elapsed_time >= m_options_summary_interval) {
    Notify("UGS_OPTIONS_SUMMARY", m_sensor_prop_summary);
    m_last_summary_time = m_curr_time;
  }
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool GenericSensor_MOOSApp::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams)) 
    reportConfigWarning("No config block found for " + GetAppName());
  
  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;
    
    bool handled = false;
    if((param == "color_source_pts") && isColor(value)) {
      m_color_source_pts = value;
      handled = true;
    }
    else if(param == "show_source_pts")
      handled = setBooleanOnString(m_show_source_pts, value);
    else if(param == "source_point")
      handled = handleConfigSourcePoint(value);
    else if(param == "min_reset_interval")
      handled = handleConfigMinResetInterval(value);
    else if(param == "min_sensor_interval")
      handled = handleConfigMinSensorInterval(value);
    else if(param == "sensor_config")
      handled = handleConfigSensorOption(value);
    else if((param == "scope_transparency") && isNumber(value)) {
      m_scope_transparency = vclip(atof(value.c_str()), 0, 1);
      handled = true;
    }
    
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  
  registerVariables();
  
  postVisuals();
  
  srand(time(NULL));
  return(true);
}


//------------------------------------------------------------
// Procedure: handleConfigSourcePoint  "x=20,y=30,label=01"

bool GenericSensor_MOOSApp::handleConfigSourcePoint(string line)
{
  double xval, yval;
  string label;
  bool   xval_set = false;
  bool   yval_set = false;

  vector<string> svector = parseString(line, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string left  = biteStringX(svector[i], '=');
    string right = svector[i];
    if((left == "x") && isNumber(right)) {
      xval     = atof(right.c_str());
      xval_set = true;
    }    
    else if((left == "y") && isNumber(right)) {
      yval     = atof(right.c_str());
      yval_set = true;
    }
    else if(left == "label")
      label = right;
  }

  if(xval_set && yval_set) {
    m_source_pt_x.push_back(xval);
    m_source_pt_y.push_back(yval);
    m_source_pt_label.push_back(label);
  }

  return(true);
}

//------------------------------------------------------------
// Procedure: handleConfigSensorOption  
//   Example: range=40

bool GenericSensor_MOOSApp::handleConfigSensorOption(string line)
{
  string sensor_range_str;

  vector<string> svector = parseString(line, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "range") 
      sensor_range_str = value;
  }
  if(!isNumber(sensor_range_str))
    return(false);
  
  // Don't take any further action if this setting choice already exists.
  if(vectorContains(m_sensor_ranges_str, sensor_range_str))
    return(false);

  // Now create a numerical version of the range and store a copy of the
  // numerical version and the string version.
  double sensor_range_dbl = atof(sensor_range_str.c_str());
  
  m_sensor_ranges_dbl.push_back(sensor_range_dbl);
  m_sensor_ranges_str.push_back(sensor_range_str);

  // Augment the locally stored summary of available sensor configurations
  if(m_sensor_prop_summary != "")
    m_sensor_prop_summary += ",";
  m_sensor_prop_summary += "range=" + sensor_range_str;
  
  return(true);
}
    
//---------------------------------------------------------
// Procedure: handleMailNodeReport
//   Example: NAME=alpha,TYPE=KAYAK,UTC_TIME=1267294386.51,
//            X=29.66,Y=-23.49,LAT=43.825089, LON=-70.330030, 
//            SPD=2.00, HDG=119.06,YAW=119.05677,DEPTH=0.00,     
//            LENGTH=4.0,MODE=ENGAGED

bool GenericSensor_MOOSApp::handleMailNodeReport(const string& node_report_str)
{
  NodeRecord node_record = string2NodeRecord(node_report_str);

  if(!node_record.valid()) {
    reportRunWarning("Unhandled node record");
    return(false);
  }
  
  string vname = node_record.getName();
  
  m_map_node_records[vname]     = node_record;
  m_map_last_node_update[vname] = m_curr_time;
  
  return(true);
}

//---------------------------------------------------------
// Procedure: handleMailSensorRequest
//   Example: vname=alpha

bool GenericSensor_MOOSApp::handleMailSensorRequest(const string& request)
{
  // Part 1: Parse the string request
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
    reportRunWarning("Sensor request received with null vehicle name");
    return(false);
  }

  m_map_requests_total[vname]++;

  // Part 2: Find out the latest node report info for the given vname;
  if(m_map_node_records.count(vname) == 0) {
    reportRunWarning("Cant handle sensor request: no node reports rec'd from "+vname);
    return(false);
  }
  
  double vx = m_map_node_records[vname].getX();
  double vy = m_map_node_records[vname].getY();

  // Part 3: If this vehicle has not initialized its sensor setting, then 
  //         perform a default setting.
  if(m_map_vehicle_sensor_range.count(vname) == 0) {
    bool ok = setVehicleSensorSetting(vname, 0);
    if(ok)
      reportRunWarning("Warning: no sensor setting for " + vname + ". Default chosen");
    if(!ok) {
      reportRunWarning("No sensor setting for vehicle " + vname);
      return(false);
    }
  }
  double sensor_range = m_map_vehicle_sensor_range[vname];

  // Part 4: Update the sensor region/polygon based on new position 
  //         and perhaps new sensor setting.
  // updateNodePolygon(vix);

  // Part 5: For each source point, determine if the point is within range to
  //         the requesting vehicle.
  unsigned int pt_index, psize = m_source_pt_x.size();
  for(pt_index=0; pt_index<psize; pt_index++) {
    double ptx = m_source_pt_x[pt_index];
    double pty = m_source_pt_y[pt_index];
    double range = distPointToPoint(vx, vy, ptx, pty);
    if(range <= sensor_range)
      postSensorReport(ptx, pty, vname);  // rolling dice inside
  }

  // Part 6: Build/Post some visual artifacts using VIEW_CIRCLE
  XYCircle circ(vx, vy, sensor_range);
  circ.set_color("edge", "white");
  circ.set_color("fill", "white");
  circ.set_label("gs_scope_"+vname);
  circ.set_msg("gs_scope_"+vname);
  circ.set_vertex_size(0);
  circ.set_edge_size(1);
  circ.set_transparency(m_scope_transparency);
  circ.set_time(m_curr_time);
  circ.setDuration(10);
  Notify("VIEW_CIRCLE", circ.get_spec());
  
  return(true);
}

//---------------------------------------------------------
// Procedure: handleMailSensorConfig
//   Example: vname=alpha,range=40
//     Notes: For those simulated sensors that have different possible 
//            sensor configurations, we check for mail here in case the
//            vehicle has requested to dynamically reconfigure its setting.
//  Consider: How to handle the case where the vehicle requests a setting
//            not available? Find the "nearest" legal setting? Reject the
//            request outright? Here we take the nearest legal setting.
//  Consider: Posting an acknowledgement. We do that here: UGS_CONFIG_ACK 

bool GenericSensor_MOOSApp::handleMailSensorConfig(const string& config,
						   const string& msg_src)
{
  // Part 1: Parse the incoming configuration request
  string vname, str_range;
  vector<string> svector = parseString(config, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "vname")
      vname = value;
    else if(param == "range")
      str_range = value;
  }

  // Part 2: Check for correctness of incoming request. The vname and src should 
  // match so that one vehicle cannot change the sensor setting of another.
  if((vname == "") || (vname != msg_src)) {
    reportRunWarning("Bad sensor config request from: " +vname+ " src:" + msg_src);
    return(false);
  }
  
  if(!isNumber(str_range)) {
    reportRunWarning("Bad sensor config request from: " + vname);
    return(false);
  }
  
  // Part 3: Clip the numerical values if necessary
  double dbl_range = atof(str_range.c_str());
  if(dbl_range < 0)
    dbl_range = 0;
  
  // Part 4: Fit the request to one of the allowable sensor settings.
  return(setVehicleSensorSetting(vname, dbl_range));
}

//---------------------------------------------------------
// Procedure: postVisuals()
//   Purpose: Post visual artifacts relevant to the initial sensor
//            configuration or simulated field of objects. Here this
//            is just the set of "points" in the op area.

void GenericSensor_MOOSApp::postVisuals()
{
  if(!m_show_source_pts)
    return;
  
  unsigned int i, vsize = m_source_pt_x.size();
  for(i=0; i<vsize; i++) {
    XYPoint point;
    point.set_vx(m_source_pt_x[i]);
    point.set_vy(m_source_pt_y[i]);
    point.set_label(m_source_pt_label[i]);
    point.set_active(true);
    point.set_vertex_size(4);
    point.set_color("vertex", m_color_source_pts);
    string spec = point.get_spec(); 
    Notify("VIEW_POINT", spec);
  }
}


//------------------------------------------------------------
// Procedure: postSensorReport()
//   Purpose: Generate the MOOS var posting that will constitute the "sensor
//            output" from the vehicle's perspective. For example, a temperature
//            sensor would grab the temperature and wrap it up into a MOOS
//            message here.
//      Note: Visual artifcacts are also typically generated. These are usually
//            not bridged out to the vehicle, but rather consumed on the 
//            shoreside for visual feedback.

void GenericSensor_MOOSApp::postSensorReport(double ptx, double pty, string vname)
{
  // Get the sensor range
  double range = m_map_vehicle_sensor_range[vname];
  
  int    bearing = rand() % 360; 
  double dist    = MOOSWhiteNoise(range/4);
  
  double rx, ry;
  projectPoint(bearing, dist, ptx, pty, rx, ry);
  
  string report = "vname=" + vname + ",x=" + doubleToString(rx,1) + 
    ",y=" + doubleToString(ry,1);
  Notify("UGS_SENSOR_REPORT_" + toupper(vname), report);
  
  m_map_reports_total[vname]++;

  XYPoint point;
  point.set_vx(rx);
  point.set_vy(ry);
  point.set_active(true);
  point.set_vertex_size(4);
  point.set_color("vertex", "white");
  string spec = point.get_spec(); 
  Notify("VIEW_POINT", spec);
}

//------------------------------------------------------------
// Procedure: handleConfigSensorTransparency
//      Note: For sensors that have a "range" or "swath", we often generate
//            a polygon to render that field. Often it is also good to make
//            that field semi transparent. This function just allows one to
//            configure that transparency.

bool GenericSensor_MOOSApp::handleConfigSensorTransparency(string str)
{
  // No check for [0,1] range. Handled in polygon set_transparency()
  m_sensor_transparency = atof(str.c_str());
  return(true);
}

//------------------------------------------------------------
// Procedure: handleConfigTermReportInterval
//      Note: Reports sent to the terminal are not usually generated every
//            AppTick, but rather based on a time interval more in line with
//            how often a user can visually consume continuously updated 
//            information. The time interval is "real" time, that is, time
//            warp information will be taken into consideration.

bool GenericSensor_MOOSApp::handleConfigTermReportInterval(string str)
{
  if(!isNumber(str)) 
    return(false);

  m_term_report_interval = atof(str.c_str());
  m_term_report_interval = vclip(m_term_report_interval, 0, 10);
  return(true);
}

//------------------------------------------------------------
// Procedure: handleConfigOptionsSummaryInterval
//   Purpose: Periodically the simulated sensor will post configuration options
//            and bridge this to the vehicle. This way the vehicle knows which
//            configuration options are available. This may not be relevant for
//            say a temperature sensor, but for say a simulated sonar this 
//            could mean the available swath widths and frequencies available.

bool GenericSensor_MOOSApp::handleConfigOptionsSummaryInterval(string str)
{
  if(!isNumber(str)) 
    return(false);
  
  m_options_summary_interval = atof(str.c_str());
  m_options_summary_interval = vclip(m_options_summary_interval, 0, 120);
  return(true);
}

//------------------------------------------------------------
// Procedure: handleConfigMinResetInterval
//   Purpose: Allow the simulated sensor to enforce a minimum time between
//            sensor re-configurations.

bool GenericSensor_MOOSApp::handleConfigMinResetInterval(string str)
{
  if(!isNumber(str)) 
    return(false);
  
  m_min_reset_interval = atof(str.c_str());
  if(m_min_reset_interval < 0)
    m_min_reset_interval  = 0;
  return(true);
}

//------------------------------------------------------------
// Procedure: handleConfigMinSensorInterval

bool GenericSensor_MOOSApp::handleConfigMinSensorInterval(string str)
{
  if(!isNumber(str))
    return(false);
  
  m_min_sensor_interval = atof(str.c_str());
  if(m_min_sensor_interval < 0)
    m_min_sensor_interval  = 0;
  return(true);
}

//------------------------------------------------------------
// Procedure: setVehicleSensorSetting
//   Purpose: Handle a request to set the sensor range for a given
//            vehicle and range.
//      Note: A UGS_CONFIG_ACK_<VNAME> acknowledgement is posted
//            and presumably bridged out to the vehicle.

bool GenericSensor_MOOSApp::setVehicleSensorSetting(string req_vname,
						    double req_range)
{
  reportEvent("Setting sensor settings for: " + req_vname + ":" + 
	      doubleToString(req_range,0));

  // Part 1: Sanity check: If no sensor properties have been configured, 
  //         there is nothing to choose from! Just return false. 

  unsigned int i, psize = m_sensor_ranges_dbl.size();
  if(psize == 0) {
    reportRunWarning("Error. No selectable settings for uFldGenericSensor!");
    return(false);
  }
  
  // Part 2: Determine if this request is allowed, based on frequency
  double elapsed = m_curr_time - m_map_reset_time[req_vname];
  if(elapsed < m_min_reset_interval) {
    reportEvent("Sensor reset denied (too soon) for vehicle: " + req_vname);
    return(false);
  }

  // Part 3: Fit the request to one of the allowable sensor settings.
  //         Select the range closest to the requested range;
  
  double select_range = m_sensor_ranges_dbl[0];
  double lowest_delta = req_range - m_sensor_ranges_dbl[0];
  if(lowest_delta < 0)
    lowest_delta = -lowest_delta;
  for(i=1; i<psize; i++) {
    double irange = m_sensor_ranges_dbl[i];
    double idelta = req_range - m_sensor_ranges_dbl[i];
    if(idelta < 0)
      idelta = -idelta;
    if(idelta < lowest_delta) {
      select_range = irange;
      lowest_delta = idelta;
    }
  }    
    
  // Part 4: Good. We have new selected sensor settings for this vehicle
  //         Note it locally, and send confirmation to the vehicle.

  // Part 4a: Store selection locally.
  m_map_vehicle_sensor_range[req_vname] = select_range;

  // Part 4b: Build and post confirmation
  string var = "UGS_CONFIG_ACK_" + toupper(req_vname);
  string msg = "vname=" + req_vname;
  msg += ",range="  + doubleToStringX(select_range, 1);
  Notify(var, msg);

  // Part 5: Update our local stats reflecting the number of updates
  //         for this vehicle and the timestamp of this update
  m_map_reset_total[req_vname]++;
  m_map_reset_time[req_vname] = m_curr_time;
	       
  return(true);
}

//------------------------------------------------------------
// Procedure: buildReport
//      Note: A virtual function of the AppCastingMOOSApp superclass, 
//            conditionally invoked if either a terminal or appcast 
//            report is needed.
//
bool GenericSensor_MOOSApp::buildReport()
{

  m_msgs << "Sensor Configuration Options"              << endl;
  m_msgs << "=========================================" << endl;

  ACTable actab(1);
  actab << "Range";
  actab.addHeaderLines();

  unsigned int i, vsize = m_sensor_ranges_str.size();
  for(i=0; i<vsize; i++) 
    actab << m_sensor_ranges_str[i];
  m_msgs << actab.getFormattedString();

  string curr_loc_time = doubleToString((m_curr_time - m_start_time), 2);
  m_msgs << endl;
  m_msgs << "Sensor Settings for known vehicles: (" << curr_loc_time << ")" << endl;
  m_msgs << "=================================================" << endl;

  actab = ACTable(5);
  actab << "Vehicle | Sensor | Config | Sensor   | Sensor ";
  actab << "Name    | Range  | Resets | Requests | Reports";
  actab.addHeaderLines();

  map<string, NodeRecord>::iterator p;
  for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
    string vname    = p->first;

    double drange   = m_map_vehicle_sensor_range[vname];
    string range    = doubleToStringX(drange,1);
    string resets   = uintToString(m_map_reset_total[vname]);
    string requests = uintToString(m_map_requests_total[vname]);
    string reports  = uintToString(m_map_reports_total[vname]);
    if(m_map_reset_total[vname] > 0) {
      double timestamp = m_map_reset_time[vname] - m_start_time;
      resets += "(" + doubleToString(timestamp, 1) + ")";
    }
    actab << vname << range << resets << requests << reports;
  }
  m_msgs << actab.getFormattedString();

  return(true);
}









