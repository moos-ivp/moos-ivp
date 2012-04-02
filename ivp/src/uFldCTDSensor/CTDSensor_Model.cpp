/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CTDSensor_Model.cpp                               */
/*    DATE: Jan 28th, 2012                                       */
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
#include <ctime>
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "CTDSensor_Model.h"
#include "NodeRecordUtils.h"
#include "XYCircle.h"

using namespace std;

//------------------------------------------------------------
// Constructor

CTDSensor_Model::CTDSensor_Model()
{
  // State Variables
  // -------------------------------------------------------------
  m_curr_time  = 0;
  m_time_warp  = 1;
  m_iterations = 0;
  m_reports    = 0;
  m_last_report_time  = 0;
  m_last_summary_time = 0;    // last time settings options summary posted

  // Configuration variables
  m_term_report_interval = 0.8;      // realtime (non-timewarp) seconds
  m_options_summary_interval = 10;   // in timewarped seconds
  // Front model parameters
  //-------------------------------------------------------------
  m_xmin  = 0;       
  m_xmax  = 500;       
  m_ymin  = 0;       
  m_ymax  = 400;       
  m_offset = 200;
  m_amplitude = 50;
  m_period = 30;
  m_wavelength = 300;
  m_alpha = 100;
  m_T_N = 20;
  m_T_S = 25;
  m_sigma = 0.1;
}

//------------------------------------------------------------
// Procedure: setParam

bool CTDSensor_Model::setParam(string param, string value)
{
  bool handled = false;

  if((param == "xmin") && isNumber(value))
    {
      m_xmin = atof(value.c_str());
      handled = true;
    }
  else if((param == "xmax") && isNumber(value))
    {
      m_xmax = atof(value.c_str());
      handled = true;
    }
  else if((param == "ymin") && isNumber(value))
    {
      m_ymin = atof(value.c_str());
      handled = true;
    }
  else if((param == "ymax") && isNumber(value))
    {
      m_ymax = atof(value.c_str());
      handled = true;
    }
  else if((param == "m_offset") && isNumber(value))
    {
      m_offset = atof(value.c_str());
      handled = true;
    }
  else if((param == "amplitude") && isNumber(value))
    {
      m_amplitude = atof(value.c_str());
      handled = true;
    }
  else if((param == "period") && isNumber(value))
    {
      m_period = atof(value.c_str());
      handled = true;
    }
  else if((param == "wavelength") && isNumber(value))
    {
      m_wavelength = atof(value.c_str());
      handled = true;
    }
  else if((param == "alpha") && isNumber(value))
    {
      m_alpha = atof(value.c_str());
      handled = true;
    }
  else if((param == "temperature_north") && isNumber(value))
    {
      m_T_N = atof(value.c_str());
      handled = true;
    }
  else if((param == "temperature_south") && isNumber(value))
    {
      m_T_S = atof(value.c_str());
      handled = true;
    }
  else if((param == "sigma") && isNumber(value))
    {
      m_sigma = atof(value.c_str());
      handled = true;
    }


  if(!handled)
    memoErr("Unhandled config, param="+param+", value="+value);

  return(handled);
}


//------------------------------------------------------------
// Procedure: handleMsg

bool CTDSensor_Model::handleMsg(string key, double dval, string sval,
				    double mtime, bool isdouble, bool isstring,
				    string src_app, string src_community)
{
  bool handled = false;
  if((key == "NODE_REPORT") || (key == "NODE_REPORT_LOCAL"))
    handled = handleNodeReport(sval);

  if(key == "UGS_SENSOR_REQUEST")
    handled = handleSensorRequest(sval);
  
  if(key == "UGS_CONFIG_REQUEST")
    handled = handleSensorConfig(sval, src_community);
  
  if(!handled) {
    string msg = "Uhandled msg: " + key;
    if(isstring) 
      msg += ", sval=" + sval;
    else if(isdouble)
      msg += ", dval=" + doubleToStringX(dval,6);
    else
      msg += ", unknown_type";
    msg += ",[" + src_app + "],[" + src_community + "]";
    memoErr(msg);
  }
  
  return(handled);
}

//------------------------------------------------------------
// Procedure: setCurrTIme

void CTDSensor_Model::setCurrTime(double new_time)
{
  if(new_time > m_curr_time)
    m_curr_time = new_time;
}

//------------------------------------------------------------
// Procedure: iterate()

void CTDSensor_Model::iterate()
{
  m_iterations++;

  // Part 1: Consider printing a report to the terminal
  double warp_elapsed_time = m_curr_time - m_last_report_time;
  double real_elapsed_time = warp_elapsed_time;
  if(m_time_warp > 0)
    real_elapsed_time = warp_elapsed_time / m_time_warp;
  if(real_elapsed_time > m_term_report_interval) {
    printReport();
    m_last_report_time = m_curr_time;
  }

  // Part 2: Consider posting an settings options summary
  double elapsed_time = m_curr_time - m_last_summary_time;
  if(elapsed_time >= m_options_summary_interval) {
    addMessage("UGS_OPTIONS_SUMMARY", m_sensor_prop_summary);
    m_last_summary_time = m_curr_time;
  }

}

//------------------------------------------------------------
// Procedure: getMessages

vector<VarDataPair> CTDSensor_Model::getMessages(bool clear)
{
  if(!clear)
    return(m_messages);
  
  vector<VarDataPair> rval = m_messages;
  m_messages.clear();
  return(rval);
}


//------------------------------------------------------------
// Procedure: FieldModelConfig  

bool CTDSensor_Model::FieldModelConfig()
{

  front.setVars(m_amplitude , m_offset, m_wavelength, 
		m_period, m_alpha, m_T_N, m_T_S);
  front.setRegion(m_xmin, m_xmax,
		  m_ymin, m_ymax); 
  
  return(true);
}
    
//------------------------------------------------------------
// Procedure: SensorConfig  

bool CTDSensor_Model::FieldModelConfig()
{

  front.setVars(m_amplitude , m_offset, m_wavelength, 
		m_period, m_alpha, m_T_N, m_T_S);
  front.setRegion(m_xmin, m_xmax,
		  m_ymin, m_ymax); 
  
  return(true);
}
    
//---------------------------------------------------------
// Procedure: handleNodeReport
//   Example: NAME=alpha,TYPE=KAYAK,UTC_TIME=1267294386.51,
//            X=29.66,Y=-23.49,LAT=43.825089, LON=-70.330030, 
//            SPD=2.00, HDG=119.06,YAW=119.05677,DEPTH=0.00,     
//            LENGTH=4.0,MODE=ENGAGED

bool CTDSensor_Model::handleNodeReport(const string& node_report_str)
{
  NodeRecord node_record = string2NodeRecord(node_report_str);

  if(!node_record.valid()) {
    memoErr("Unhandled node record");
    return(false);
  }
  
  string vname = node_record.getName();
  
  m_map_node_records[vname]     = node_record;
  m_map_last_node_update[vname] = m_curr_time;

  memo("Node report received");

  return(true);
}

//---------------------------------------------------------
// Procedure: handleSensorRequest
//   Example: vname=alpha

bool CTDSensor_Model::handleSensorRequest(const string& request)
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
    memoErr("Sensor request received with null vehicle name");
    return(false);
  }
  memo("Sensor request received from " + vname);

  // Part 2: Find out the latest node report info for the given vname;
  if(m_map_node_records.count(vname) == 0) {
    memoErr("Cannot handle sensor request: no node reports received from " +vname);
    return(false);
  }
  
  double vx = m_map_node_records[vname].getX();
  double vy = m_map_node_records[vname].getY();

  // Part 3: If this vehicle has not initialized its sensor setting, then 
  //         perform a default setting.
  if(m_map_vehicle_sensor_temperature.count(vname) == 0) {
    bool ok = setVehicleSensorSetting(vname, 0);
    if(ok)
      memo("Warning: no sensor setting for vehicle " + vname + ". Default chosen");
    if(!ok) {
      memoErr("No sensor setting for vehicle " + vname);
      return(false);
    }
  }

  double sensor_temperature = m_map_vehicle_sensor_temperature[vname];

  // Part 4: Update the sensor region/polygon based on new position 
  //         and perhaps new sensor setting.
  //updateNodePolygon(vix);

  // Part 5: For each source point, determine if the point is within range to
  //         the requesting vehicle.
  unsigned int pt_index, psize = m_source_pt_x.size();
  for(pt_index=0; pt_index<psize; pt_index++) {
    double ptx = m_source_pt_x[pt_index];
    double pty = m_source_pt_y[pt_index];
    double range = distPointToPoint(vx, vy, ptx, pty);
    if(range <= sensor_temperature)
      postSensorReport(ptx, pty, vname);  // rolling dice inside
  }

  // Part 6: Build/Post some visual artifacts using VIEW_CIRCLE
  XYCircle circ(vx, vy, sensor_temperature);
  circ.set_color("edge", "white");
  circ.set_color("fill", "white");
  circ.set_label("gs_scope_"+vname);
  circ.set_msg("gs_scope_"+vname);
  circ.set_source("gs_scope_"+vname);
  circ.set_vertex_size(0);
  circ.set_edge_size(1);
  circ.set_transparency(m_scope_transparency);
  circ.set_time(m_curr_time);
  circ.setDuration(10);
  addMessage("VIEW_CIRCLE", circ.get_spec());

  return(true);
}

//---------------------------------------------------------
// Procedure: handleSensorConfig
//   Example: vname=alpha,width=50,pd=0.9

bool CTDSensor_Model::handleSensorConfig(const string& config,
					     const string& msg_src)
{
  // Part 1: Parse the incoming configuration request
  string vname, str_temperature;
  vector<string> svector = parseString(config, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "vname")
      vname = value;
    else if(param == "range")
      str_temperature = value;
  }

  // Part 2: Check for correctness of incoming request. The vname and src should 
  // match so that one vehicle cannot change the sensor setting of another.
  if((vname == "") || (vname != msg_src)) {
    memoErr("Bad sensor config request from: " +vname+ " src:" + msg_src);
    return(false);
  }
  
  if(!isNumber(str_temperature)) {
    memoErr("Bad sensor config request from: " + vname);
    return(false);
  }
  
  // Part 3: Clip the numerical values if necessary
  double dbl_temperature = atof(str_temperature.c_str());
  if(dbl_temperature < 0)
    dbl_temperature = 0;
  
  // Part 4: Fit the request to one of the allowable sensor settings.
  return(setVehicleSensorSetting(vname, dbl_temperature));
}

//---------------------------------------------------------
// Procedure: addMessage()

void CTDSensor_Model::addMessage(const string& varname,
			   const string& value)
{
  VarDataPair pair(varname, value);
  m_messages.push_back(pair);
}

//---------------------------------------------------------
// Procedure: addMessage()

void CTDSensor_Model::addMessage(const string& varname, double value)
{
  VarDataPair pair(varname, value);
  m_messages.push_back(pair);
}

//---------------------------------------------------------
// Procedure: getVisuals()

vector<VarDataPair> CTDSensor_Model::getVisuals()
{
  vector<VarDataPair> visuals;

  if(!m_show_source_pts)
    return(visuals);

  unsigned int i, vsize = m_source_pt_x.size();
  for(i=0; i<vsize; i++) {

    XYPoint point;
    point.set_vx(m_source_pt_x[i]);
    point.set_vy(m_source_pt_y[i]);
    point.set_label(m_source_pt_label[i]);
    point.set_active(true);
    point.set_vertex_size(4);
    point.set_color("vertex", "blue");
    string spec = point.get_spec(); 
    VarDataPair pair("VIEW_POINT", spec);
    visuals.push_back(pair);    
  }

  return(visuals);
}


//------------------------------------------------------------
// Procedure: postSensorReport()

void CTDSensor_Model::postSensorReport(double ptx, double pty, string vname)
{
  // Get the sensor range
  double range = m_map_vehicle_sensor_temperature[vname];

  int    bearing = rand() % 360; 
  double dist    = rand() % (int)(range);

  double rx, ry;
  projectPoint(bearing, dist, ptx, pty, rx, ry);

  string report = "vname=" + vname + ",x=" + doubleToString(rx,1) + 
    ",y=" + doubleToString(ry,1);
  addMessage("UGS_HAZARD_REPORT_" + toupper(vname), report);
  
  XYPoint point;
  point.set_vx(rx);
  point.set_vy(ry);
  point.set_active(true);
  point.set_vertex_size(4);
  point.set_color("vertex", "white");
  string spec = point.get_spec(); 
  addMessage("VIEW_POINT", spec);
}

//------------------------------------------------------------
// Procedure: setSensorTransparency

bool CTDSensor_Model::setSensorTransparency(string str)
{
  // No check for [0,1] range. Handled in polygon set_transparency()
  m_sensor_transparency = atof(str.c_str());
  return(true);
}

//------------------------------------------------------------
// Procedure: setTermReportInterval

bool CTDSensor_Model::setTermReportInterval(string str)
{
  if(!isNumber(str)) {
    memoErr("Improper term_report_interval: " + str);
    return(false);
  }

  m_term_report_interval = atof(str.c_str());
  m_term_report_interval = vclip(m_term_report_interval, 0, 10);
  return(true);
}

//------------------------------------------------------------
// Procedure: setOptionsSummaryInterval

bool CTDSensor_Model::setOptionsSummaryInterval(string str)
{
  if(!isNumber(str)) {
    memoErr("Improper options_summary_interval: " + str);
    return(false);
  }
  
  m_options_summary_interval = atof(str.c_str());
  m_options_summary_interval = vclip(m_options_summary_interval, 0, 120);
  return(true);
}

//------------------------------------------------------------
// Procedure: setMinResetInterval

bool CTDSensor_Model::setMinResetInterval(string str)
{
  if(!isNumber(str)) {
    memoErr("Improper min_reset_interval: " + str);
    return(false);
  }
  
  m_min_reset_interval = atof(str.c_str());
  if(m_min_reset_interval < 0)
    m_min_reset_interval  = 0;
  return(true);
}

//------------------------------------------------------------
// Procedure: setMinSensorInterval

bool CTDSensor_Model::setMinSensorInterval(string str)
{
  if(!isNumber(str)) {
    memoErr("Improper min_sensor_interval: " + str);
    return(false);
  }
  
  m_min_sensor_interval = atof(str.c_str());
  if(m_min_sensor_interval < 0)
    m_min_sensor_interval  = 0;
  return(true);
}

//------------------------------------------------------------
// Procedure: perhapsSeedRandom

void CTDSensor_Model::perhapsSeedRandom()
{
  if(m_seed_random)
    srand(time(NULL));
}


//------------------------------------------------------------
// Procedure: setVehicleSensorSetting
//   Purpose: Handle a request to set the sensor range for a given
//            vehicle and range.
//      Note: A UGS_CONFIG_ACK_<VNAME> acknowledgement is posted
//            and presumably bridged out to the vehicle.

bool CTDSensor_Model::setVehicleSensorSetting(string req_vname,
						  double req_temperature)
{
  memo("Setting sensor settings for:" + req_vname);

  // Part 1: Sanity check: If no sensor properties have been configured, 
  //         there is nothing to choose from! Just return false. 

  unsigned int i, psize = m_sensor_temperatures_dbl.size();
  if(psize == 0) {
    memoErr("Error. No selectable settings for uFldCTDSensor!");
    return(false);
  }
  
  // Part 2: Determine if this request is allowed, based on frequency

  double elapsed = m_curr_time - m_map_reset_time[req_vname];
  if(elapsed < m_min_reset_interval) {
    memo("Sensor reset denied (too soon) for vehicle: " + req_vname);
    return(false);
  }

  // Part 3: Fit the request to one of the allowable sensor settings.
  //         Select the range closest to the requested range;
  
  double select_temperature = m_sensor_temperatures_dbl[0];
  double lowest_delta = req_temperature - m_sensor_temperatures_dbl[0];
  if(lowest_delta < 0)
    lowest_delta = -lowest_delta;
  for(i=1; i<psize; i++) {
    double irange = m_sensor_temperatures_dbl[i];
    double idelta = req_temperature - m_sensor_temperatures_dbl[i];
    if(idelta < 0)
      idelta = -idelta;
    if(idelta < lowest_delta) {
      select_temperature = irange;
      lowest_delta = idelta;
    }
  }    
    
  // Part 4: Good. We have new selected sensor settings for this vehicle
  //         Note it locally, and send confirmation to the vehicle.

  // Part 4a: Store selection locally.
  m_map_vehicle_sensor_temperature[req_vname] = select_temperature;

  // Part 4b: Build and post confirmation
  string var = "UGS_CONFIG_ACK_" + toupper(req_vname);
  string msg = "vname=" + req_vname;
  msg += ",range="  + doubleToStringX(select_temperature, 1);
  addMessage(var, msg);

  // Part 5: Update our local stats reflecting the number of updates
  //         for this vehicle and the timestamp of this update
  m_map_reset_total[req_vname]++;
  m_map_reset_time[req_vname] = m_curr_time;
	       
  return(true);
}


//------------------------------------------------------------
// Procedure: memo()
//   Purpose: Collect output meant for the terminal on each iteration.
//            Duplicate memos simply have their counter increased so the
//               screen is not flooded with a line for each occurance. 
//            Some thought must be given to the memo content such that 
//            memos posted regularly do not differ in content on trivial
//            differences; to avoid terminal reports that run off the 
//            screen.

void CTDSensor_Model::memo(const string& memo_str)
{
  m_map_memos[memo_str]++;
}

//------------------------------------------------------------
// Procedure: memoErr()
//   Purpose: Same as the memo() function, but...
//            when these memos are sent to the terminal, they may be
//            in a different color indicating the error nature.

void CTDSensor_Model::memoErr(const string& memo_str)
{
  m_map_err_memos[memo_str]++;
}

//------------------------------------------------------------
// Procedure: printReport
//   Purpose: 

void CTDSensor_Model::printReport()
{
  m_reports++;
  unsigned int colpad = 2;

  cout << endl << endl <<endl << endl << endl << endl;
  cout << "*****************************************"      << endl;
  cout << "uFldCTDSensor Summary:(" << m_reports << ")" << endl;
  cout << "*****************************************"      << endl;
  cout << "                                         "      << endl;
  cout << "Sensor Configuration Options             "      << endl;
  cout << "========================================="      << endl;

  unsigned int i, vsize = m_sensor_temperatures_str.size();
  cout << "   Range  " << endl;
  cout << "  ------  " << endl;
  for(i=0; i<vsize; i++) {
    string s_temperature = m_sensor_temperatures_str[i];
    s_temperature = padString(s_temperature, 6+colpad);
    cout << s_temperature << endl;
  }

  cout << endl;
  cout << "Sensor Settings for known vehicles: ";
  cout << "(" << m_map_node_records.size() << ")" << endl;
  cout << "======================================   " << endl;

  cout << "VName      Range        Resets  Reports" << endl;
  cout << "-------    -----     ---------  -------" << endl;

  map<string, NodeRecord>::iterator p;
  for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
    string vname      = p->first;

    double dbl_temperature   = m_map_vehicle_sensor_temperature[vname];
    string str_temperature   = doubleToStringX(dbl_temperature,1);
    string str_resets  = uintToString(m_map_reset_total[vname]);
    if(m_map_reset_total[vname] > 0) {
      double elapsed_since_reset = m_curr_time - m_map_reset_time[vname];
      str_resets        += "(" + doubleToString(elapsed_since_reset,0) + ")";
    }
    string str_reports = uintToString(m_map_reports_total[vname]);
    
    string pad_vname   = padString(vname,       6+colpad, false);
    string pad_temperature   = padString(str_temperature,   6+colpad);
    string pad_resets  = padString(str_resets,  11+colpad);
    string pad_reports = padString(str_reports, 7+colpad);

    cout << pad_vname << pad_temperature << pad_resets << pad_reports << endl;
  }

  cout << endl;
  cout << "Internal Memos:                            " << endl;
  cout << "======================================     " << endl;
  map<string, unsigned int>::iterator p1;
  for(p1=m_map_memos.begin(); p1!=m_map_memos.end(); p1++) {
    string       msg = p1->first;
    unsigned int amt = p1->second;
    cout << " (" << amt << ") " << msg << endl;
  }

  for(p1=m_map_err_memos.begin(); p1!=m_map_err_memos.end(); p1++) {
    string msg = p1->first;
    int    amt = p1->second;
    cout << termColor("magenta");
    cout << " (" << amt << ") " << msg  << termColor() << endl;
  }
}
