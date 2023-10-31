/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: USM_MOOSApp.cpp                                      */
/*    DATE: Oct 25th 2004                                        */
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

#include <iostream>
#include <math.h>
#include "USM_MOOSApp.h"
#include "MBUtils.h"
#include "ACTable.h"
#include "AngleUtils.h"

// As of Release 15.4 this is now set in CMake, defaulting to be defined
// #define USE_UTM 

using namespace std;

//------------------------------------------------------------------------
// Constructor

USM_MOOSApp::USM_MOOSApp() 
{
  m_sim_prefix  = "USM";
  m_reset_count = 0;
  m_geo_ok      = false;
  m_enabled     = true;
  
  buoyancy_requested = false;
  trim_requested = false;
  buoyancy_delay = 5;
  max_trim_delay = 10;
  last_report     = 0;
  report_interval = 5; 
  last_report     = 0;
  report_interval = 5;
  pitch_tolerance = 5;

  m_obstacle_hit  = false;

  m_thrust_mode_reverse = false;
  m_thrust_mode_differential = false;

  // This app is deprecated. Suggest an alternative
  m_deprecated_alt = "uSimMarineV22";
}

//------------------------------------------------------------------------
// Procedure: OnNewMail

bool USM_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);
  
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key = msg.GetKey();
    double dval = msg.GetDouble();
    string sval = msg.GetString();

    if(key == "DESIRED_THRUST") {
      if(m_thrust_mode_differential == false)
	m_model.setThrust(dval);
    }
    else if(key == "DESIRED_RUDDER") {
      if(m_thrust_mode_differential == false)
	m_model.setRudder(dval, MOOSTime());
    }
    else if(key == "DESIRED_THRUST_L") {
      if(m_thrust_mode_differential == true)
	m_model.setThrustLeft(dval);
    }
    else if(key == "DESIRED_THRUST_R") {
      if(m_thrust_mode_differential == true)
	m_model.setThrustRight(dval);
    }
    else if(key == "DESIRED_ELEVATOR")
      m_model.setElevator(dval);
    else if(key == "USM_SIM_PAUSED")
      m_model.setPaused(toupper(sval) == "TRUE");

    else if(key == "THRUST_MODE_REVERSE") {
      m_thrust_mode_reverse = false;
      if(tolower(sval) == "true") 
	m_thrust_mode_reverse = true;
      m_model.setThrustModeReverse(m_thrust_mode_reverse);
    }
    else if(key == "THRUST_MODE_DIFFERENTIAL") {
      setBooleanOnString(m_thrust_mode_differential, sval);
      if(m_thrust_mode_differential)
	m_model.setThrustModeDiff("differential");
      else
	m_model.setThrustModeDiff("normal");
    }
    else if((key == "USM_BUOYANCY_RATE") || // Deprecated
	    (key == "BUOYANCY_RATE"))
      m_model.setParam("buoyancy_rate", dval);

    else if((key == "USM_FORCE_THETA") ||   // Deprecated
	    (key == "ROTATE_SPEED"))
      m_model.setParam("rotate_speed", dval);

    else if((key == "USM_FORCE_X") ||       // Deprecated
	    (key == "CURRENT_X")   || 
	    (key == "DRIFT_X")) {
      m_srcs_drift.insert(msg.GetSource());
      m_model.setParam("drift_x", dval);
    }

    else if((key == "USM_FORCE_Y") ||       // Deprecated
	    (key == "CURRENT_Y")   || 
	    (key == "DRIFT_Y")) {
      m_srcs_drift.insert(msg.GetSource());
      m_model.setParam("drift_y", dval);
    }

    else if((key == "USM_FORCE_VECTOR") ||  // Deprecated
	    (key == "DRIFT_VECTOR"))  {
      m_srcs_drift.insert(msg.GetSource());
      m_model.setDriftVector(sval, false);
    }

    else if((key == "USM_FORCE_VECTOR_ADD") || // Deprecated
	    (key == "DRIFT_VECTOR_ADD"))  {
      m_srcs_drift.insert(msg.GetSource());
      m_model.setDriftVector(sval, true);
    }

    else if((key == "USM_FORCE_VECTOR_MULT") || // Deprecated
	    (key == "DRIFT_VECTOR_MULT"))  {
      m_srcs_drift.insert(msg.GetSource());
      m_model.magDriftVector(dval);
    }

    else if((key == "USM_WATER_DEPTH") ||    // Deprecated
	    (key == "WATER_DEPTH"))
      m_model.setParam("water_depth", dval);

    else if(key == "USM_RESET") {
      m_reset_count++;
      Notify("USM_RESET_COUNT", m_reset_count);
      m_model.initPosition(sval);
    }

    else if(key == "USM_TURN_RATE") 
      m_model.setParam("turn_rate", dval);
    else if(key == "USM_ENABLED")
      setBooleanOnString(m_enabled, sval);
    
    else if(key == "OBSTACLE_HIT") {
      if(dval != 0)
	m_obstacle_hit = true;
    }

    // Added buoyancy and trim control and sonar handshake. HS 2012-07-22
    else if (key == "BUOYANCY_CONTROL") {
      if(MOOSStrCmp(sval,"true")) {
	// Set buoyancy to zero to simulate trim
	m_model.setParam("buoyancy_rate", 0.0);
	buoyancy_request_time = MOOSTime();
	std::string buoyancy_status="status=1,error=0,progressing,buoyancy=0.0";
	Notify("BUOYANCY_REPORT",buoyancy_status);
	buoyancy_requested = true;
	last_report = buoyancy_request_time;
      } 
    }	    
    else if(key == "TRIM_CONTROL") {
      if(MOOSStrCmp(sval,"true")) {
	trim_request_time = MOOSTime();
	std::string trim_status="status=1,error=0,progressing,trim_pitch=0.0,trim_roll=0.0";
	Notify("TRIM_REPORT",trim_status);
	trim_requested = true;
	last_report = trim_request_time;
      }
    }
    


#if 0 // OLD
    // Added buoyancy and trim control and sonar handshake. HS 2012-07-22
    else if(key == "BUOYANCY_CONTROL") {
      if(MOOSStrCmp(sval,"true")) {
	// Set buoyancy to zero to simulate trim
	m_model.setParam("buoyancy_rate", 0.0);
	buoyancy_request_time = MOOSTime();
	std::string buoyancy_status="status=1,error=0,progressing,buoyancy=0.0";
	Notify("BUOYANCY_REPORT",buoyancy_status);
	buoyancy_requested = true;
      } 
    }	    
    else if(key == "TRIM_CONTROL") {
      if(MOOSStrCmp(sval,"true")) {
	trim_request_time = MOOSTime();
	string trim_status="status=1,error=0,progressing,trim_pitch=0.0,trim_roll=0.0";
	Notify("TRIM_REPORT",trim_status);
	trim_requested = true;
      }
    }
    else 
      reportRunWarning("Unhandled mail: " + key);
# endif // OLD
  }
  
  return(true);
}
  
//------------------------------------------------------------------------
// Procedure: OnStartUp()

bool USM_MOOSApp::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  
  m_model.resetTime(m_curr_time);

  STRING_LIST sParams;
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams)) 
    reportConfigWarning("No config block found for " + GetAppName());
    
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = line;
    double dval  = atof(value.c_str());

    // Handle and warn of deprecated configuration parameters.
    param = handleConfigDeprecations(param);

    bool handled = false;
    if((param == "START_X") && isNumber(value))
      handled = m_model.setParam(param, dval);
    else if((param == "START_Y") && isNumber(value))
      handled = m_model.setParam(param, dval);
    else if((param == "START_HEADING") && isNumber(value))
      handled = m_model.setParam(param, dval);
    else if((param == "START_SPEED") && isNumber(value))
      handled = m_model.setParam(param, dval);
    else if((param == "START_DEPTH") && isNumber(value))
      handled = m_model.setParam(param, dval);
    else if((param == "BUOYANCY_RATE") && isNumber(value))
      handled = m_model.setParam(param, dval);
    else if((param == "DRIFT_X") && isNumber(value))
      handled = m_model.setParam("drift_x", dval);
    else if((param == "DRIFT_Y") && isNumber(value))
      handled = m_model.setParam("drift_y", dval);
    else if((param == "ROTATE_SPEED") && isNumber(value))
      handled = m_model.setParam("rotate_speed", dval);
    else if((param == "MAX_ACCELERATION") && isNumber(value))
      handled = m_model.setParam("max_acceleration", dval);
    else if((param == "MAX_DECELERATION") && isNumber(value))
      handled = m_model.setParam("max_deceleration", dval);
    else if((param == "MAX_DEPTH_RATE") && isNumber(value))
      handled = m_model.setParam("max_depth_rate", dval);
    else if((param == "MAX_DEPTH_RATE_SPEED") && isNumber(value))
      handled = m_model.setParam("max_depth_rate_speed", dval);
    else if((param == "MAX_RUDDER_DEGS_PER_SEC") && isNumber(value))
      handled = m_model.setMaxRudderDegreesPerSec(dval);
    else if(param == "PREFIX")
      handled = setNonWhiteVarOnString(m_sim_prefix, value);
    else if(param == "DRIFT_VECTOR")
      handled = m_model.setDriftVector(value);
    else if((param == "SIM_PAUSE") && isBoolean(value)) {
      m_model.setPaused(tolower(value) == "true");
      handled = true;
    }
    else if((param == "DUAL_STATE") && isBoolean(value)) {
      m_model.setDualState(tolower(value) == "true");
      handled = true;
    }
    else if(param == "START_POS")
      handled = m_model.initPosition(value);
    else if((param == "THRUST_REFLECT") && isBoolean(value)) {
      m_model.setThrustReflect(tolower(value)=="true");
      handled = true;
    }
    else if((param == "THRUST_FACTOR") && isNumber(value))
      m_model.setThrustFactor(dval);
    else if(param == "THRUST_MAP")
      handled = handleThrustMapping(value);
    else if((param == "TURN_RATE") && isNumber(value))
      handled = m_model.setParam("turn_rate", dval);
    else if((param == "DEFAULT_WATER_DEPTH") && isNumber(value))
      handled = m_model.setParam("water_depth", dval);
    else if((param == "TRIM_TOLERANCE") && isNumber(value)) {
      pitch_tolerance = dval;
      handled = true;
    }
    else if((param == "MAX_TRIM_DELAY") && isNumber(value)) {
      max_trim_delay = dval; 
      handled = true;
    }
        
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  // look for latitude, longitude global variables
  double latOrigin, longOrigin;
  if(!m_MissionReader.GetValue("LatOrigin", latOrigin)) {
    MOOSTrace("uSimMarine: LatOrigin not set in *.moos file.\n");
    m_geo_ok = false;
  } 
  else if(!m_MissionReader.GetValue("LongOrigin", longOrigin)) {
    MOOSTrace("uSimMarine: LongOrigin not set in *.moos file\n");
    m_geo_ok = false;      
  }
  else {
    m_geo_ok = true;
    // initialize m_geodesy
    if(!m_geodesy.Initialise(latOrigin, longOrigin)) {
      MOOSTrace("uSimMarine: Geodesy init failed.\n");
      m_geo_ok = false;
    }
  }
 
  cacheStartingInfo();
 
  registerVariables();
  MOOSTrace("uSimMarine started \n");
  return(true);
}

//------------------------------------------------------------------------
// Procedure: OnConnectToServer
//      Note: 

bool USM_MOOSApp::OnConnectToServer()
{
  registerVariables();
  MOOSTrace("Sim connected\n");
  
  return(true);
}

//------------------------------------------------------------------------
// Procedure: cacheStartingInfo()

void USM_MOOSApp::cacheStartingInfo()
{
  NodeRecord record = m_model.getNodeRecord();
  double nav_x = record.getX();
  double nav_y = record.getY();
  double depth = m_model.getWaterDepth();
  double nav_depth = record.getDepth();
  double nav_alt = depth - nav_depth;

  double nav_lat = 0;
  double nav_lon = 0;
 if(m_geo_ok) {
#ifdef USE_UTM
    m_geodesy.UTM2LatLong(nav_x, nav_y, nav_lat, nav_lon);
#else
    m_geodesy.LocalGrid2LatLong(nav_x, nav_y, nav_lat, nav_lon);
#endif
 }

  m_start_nav_x     = doubleToStringX(nav_x,2);
  m_start_nav_y     = doubleToStringX(nav_y,2);
  m_start_nav_lat   = doubleToStringX(nav_lat,8);
  m_start_nav_lon   = doubleToStringX(nav_lon,8);
  m_start_nav_spd   = doubleToStringX(record.getSpeed(),2);
  m_start_nav_hdg   = doubleToStringX(record.getHeading(),1);
  m_start_nav_dep   = doubleToStringX(record.getDepth(),2);
  m_start_nav_alt   = doubleToStringX(nav_alt,2);
  m_start_buoyrate  = doubleToStringX(m_model.getBuoyancyRate(),8);
  m_start_drift_x   = doubleToStringX(m_model.getDriftX(),4);
  m_start_drift_y   = doubleToStringX(m_model.getDriftY(),4);
  m_start_drift_mag = doubleToStringX(m_model.getDriftMag(),4);
  m_start_drift_ang = doubleToStringX(m_model.getDriftAng(),4);
  m_start_rotate_spd = doubleToStringX(m_model.getRotateSpd(),4);
}

//------------------------------------------------------------------------
// Procedure: registerVariables

void USM_MOOSApp::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  Register("DESIRED_RUDDER", 0);
  Register("DESIRED_THRUST", 0);
  Register("DESIRED_ELEVATOR", 0);

  Register("DESIRED_THRUST_L", 0);
  Register("DESIRED_THRUST_R", 0);

  //Register("USM_BUOYANCY_RATE", 0);  // Deprecated
  Register("BUOYANCY_RATE", 0);

  //Register("USM_WATER_DEPTH", 0);    // Deprecated
  Register("WATER_DEPTH", 0);

  //Register("USM_FORCE_X", 0);  // Deprecated
  Register("CURRENT_X",0);
  Register("DRIFT_X",0);

  //Register("USM_FORCE_Y", 0);  // Deprecated
  Register("CURRENT_Y",0);
  Register("DRIFT_Y",0);

  //Register("USM_FORCE_VECTOR", 0); // Deprecated
  Register("DRIFT_VECTOR", 0);

  //Register("USM_FORCE_VECTOR_ADD", 0); // Deprecated
  Register("DRIFT_VECTOR_ADD", 0);

  //Register("USM_FORCE_VECTOR_MULT", 0); // Deprecated
  Register("DRIFT_VECTOR_MULT", 0);

  //Register("USM_FORCE_THETA", 0); // Deprecated
  Register("ROTATE_SPEED", 0);

  Register("USM_SIM_PAUSED", 0); 
  Register("USM_RESET", 0);

  Register("USM_TURN_RATE", 0);
  Register("USM_ENABLED", 0);

  Register("OBSTACLE_HIT", 0);

  // Added buoyancy and trim control and sonar handshake
  Register("TRIM_CONTROL",0);
  Register("BUOYANCY_CONTROL",0);

  Register("THRUST_MODE_REVERSE",0);
  Register("THRUST_MODE_DIFFERENTIAL",0);
}

//------------------------------------------------------------------------
// Procedure: Iterate
//      Note: This is where it all happens.

bool USM_MOOSApp::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if(!m_obstacle_hit)
    m_model.propagate(m_curr_time);

  if(!m_enabled) {
    AppCastingMOOSApp::PostReport();
    return(true);
  }
  
  NodeRecord record = m_model.getNodeRecord();

  double pitch_degrees = record.getPitch()*180.0/M_PI;

  if(m_thrust_mode_reverse) {
    record.setHeading(angle360(record.getHeading()+180));
    record.setHeadingOG(angle360(record.getHeadingOG()+180));
    record.setThrustModeReverse(true);

    double pi = 3.1415926;
    double new_yaw = record.getYaw() + pi;
    if(new_yaw > (2* pi))
      new_yaw = new_yaw - (2 * pi);
    record.setYaw(new_yaw);
  }

  // buoyancy and trim control
  if(buoyancy_requested) {
    if(m_curr_time-buoyancy_request_time >= buoyancy_delay) {
      string buoyancy_status="status=2,error=0,completed,buoyancy=0.0";
      Notify("BUOYANCY_REPORT",buoyancy_status);
      buoyancy_requested = false;
    }
    else if(m_curr_time - last_report >= report_interval) {
      string buoyancy_status="status=1,error=0,progressing,buoyancy=0.0";
      Notify("BUOYANCY_REPORT",buoyancy_status);
      last_report = m_curr_time; 
    }
  }
  if(trim_requested) {
    if(((fabs(pitch_degrees) <= pitch_tolerance)
	&& (m_curr_time-trim_request_time >= buoyancy_delay)) 
       || (m_curr_time-trim_request_time) >= max_trim_delay) {
      string trim_status="status=2,error=0,completed,trim_pitch="
	+ doubleToString(pitch_degrees) + ",trim_roll=0.0";
      Notify("TRIM_REPORT",trim_status);
      trim_requested = false;
    }
    else if((m_curr_time - last_report) >= report_interval) {
      string trim_status="status=1,error=0,progressing,trim_pitch="
	+ doubleToString(pitch_degrees) + ",trim_roll=0.0";
      Notify("TRIM_REPORT",trim_status);
      last_report = m_curr_time; 
    }
  }
  
  postNodeRecordUpdate(m_sim_prefix, record);

  if(m_model.usingDualState()) {
    NodeRecord record_gt = m_model.getNodeRecordGT();
    postNodeRecordUpdate(m_sim_prefix+"_GT", record_gt);
  }

  if(m_model.isDriftFresh()) {
    Notify("USM_FSUMMARY", m_model.getDriftSummary());
    Notify("USM_DRIFT_SUMMARY", m_model.getDriftSummary());
    m_model.setDriftFresh(false);
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}

//------------------------------------------------------------------------
// Procedure: postNodeRecordUpdate

void USM_MOOSApp::postNodeRecordUpdate(string prefix, 
				       const NodeRecord &record)
{
  double nav_x = record.getX();
  double nav_y = record.getY();

  Notify(prefix+"_X", nav_x, m_curr_time);
  Notify(prefix+"_Y", nav_y, m_curr_time);

  if(m_geo_ok) {
    double lat, lon;
#ifdef USE_UTM
    m_geodesy.UTM2LatLong(nav_x, nav_y, lat, lon);
#else
    m_geodesy.LocalGrid2LatLong(nav_x, nav_y, lat, lon);
#endif
    Notify(prefix+"_LAT", lat, m_curr_time);
    Notify(prefix+"_LONG", lon, m_curr_time);
  }

  double new_speed = record.getSpeed();
  new_speed = snapToStep(new_speed, 0.01);

  Notify(prefix+"_HEADING", record.getHeading(), m_curr_time);
  Notify(prefix+"_SPEED", new_speed, m_curr_time);
  Notify(prefix+"_DEPTH", record.getDepth(), m_curr_time);

  // Added by HS 120124 to make it work ok with iHuxley
  Notify("SIMULATION_MODE","TRUE", m_curr_time);
  Notify(prefix+"_Z", -record.getDepth(), m_curr_time);
  Notify(prefix+"_PITCH", record.getPitch(), m_curr_time);
  Notify(prefix+"_YAW", record.getYaw(), m_curr_time);
  Notify("TRUE_X", nav_x, m_curr_time);
  Notify("TRUE_Y", nav_y, m_curr_time);


  double hog = angle360(record.getHeadingOG());
  double sog = record.getSpeedOG();

  Notify(prefix+"_HEADING_OVER_GROUND", hog, m_curr_time);
  Notify(prefix+"_SPEED_OVER_GROUND", sog, m_curr_time);
  
  if(record.isSetAltitude()) 
    Notify(prefix+"_ALTITUDE", record.getAltitude(), m_curr_time);
  
}

//--------------------------------------------------------------------
// Procedure: handleThrustMapping
//

bool USM_MOOSApp::handleThrustMapping(string mapping)
{
  if(mapping == "")
    return(false);
  vector<string> svector = parseString(mapping, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string thrust = biteStringX(svector[i], ':');
    string speed  = svector[i];
    if(!isNumber(thrust) || !isNumber(speed))
      return(false);
    double dthrust = atof(thrust.c_str());
    double dspeed  = atof(speed.c_str());
    bool ok = m_model.addThrustMapping(dthrust, dspeed);
    if(!ok)
      return(false);
  }
  return(true);
}


//--------------------------------------------------------------------
// Procedure: handleConfigDeprecations

string USM_MOOSApp::handleConfigDeprecations(string param)
{
  string new_param = param;

  param = toupper(param);  
  if(param == "FORCE_X")
    new_param = "DRIFT_X";
  else if(param == "FORCE_Y")
    new_param = "DRIFT_Y";
  else if(param == "STARTHEADING")
    new_param = "START_HEADING";
  else if(param == "STARTDEPTH")
    new_param = "START_DEPTH";
  else if(param == "STARTSPEED")
    new_param = "START_SPEED";
  else if(param == "FORCE_THETA")
    new_param = "ROTATE_SPEED";
  else if(param == "FORCE_VECTOR")
    new_param = "DRIFT_VECTOR";

  if(param != new_param)
    reportConfigWarning(param + " is deprecated. Use instead: " + new_param);

  return(new_param);
}


//------------------------------------------------------------------------
// Procedure: buildReport
//      Note: A virtual function of the AppCastingMOOSApp superclass, 
//            conditionally invoked if either a terminal or appcast 
//            report is needed.
//
// Datum: 43.825300,  -71.087589, (MIT Sailing Pavilion)
//
//   Starting Pose                Current Pose
//   -------- ----------         -------- -----------
//   Heading: 180                Heading: 134.8
//     Speed: 0                    Speed: 1.2
//     Depth: 0                    Depth: 37.2  
//  Altitude: 58                Altitude: 20.8
//     (X,Y): 0,0                  (X,Y): -4.93,-96.05
//       Lat: 43.8253                Lat: 43.82443465       
//       Lon: -70.3304               Lon: -70.33044214      

//  External Drift  X   Y   |  Mag  Ang  |  Rotate  |  Source(s)  
//  --------------  --  --  |  ---  ---  |  ------  |  -----------
//        Starting  0   0   |  0    0    |  0       |  init_config
//         Present  0   0   |  0    0    |  0       |  n/a        
//
//   Dual state: true
// 
//  DESIRED_THRUST=24 ==> Speed=1.2
//  Using Thrust Factor: false
//  Positive Thrust Map: 0:1, 20:2.4, 50:4.2, 80:4.8, 100:5.0
//  Negative Thrust Map: -100:-3.5, -75:-3.2, -10:-2,
//     Max Acceleration: 0
//     Max Deceleration: 0.5
//
//  DESIRED_ELEVATOR=-12 ==> Depth=37.2
//        Max Depth Rate: 0.5 
//  Max Depth Rate Speed: 2.0
//           Water depth: 58

bool USM_MOOSApp::buildReport()
{
  NodeRecord record = m_model.getNodeRecord();
  double nav_x   = record.getX();
  double nav_y   = record.getY();
  double nav_alt = m_model.getWaterDepth() - record.getDepth();
  if(nav_alt < 0)
    nav_alt = 0;

  NodeRecord record_gt = m_model.getNodeRecordGT();
  double nav_x_gt   = record_gt.getX();
  double nav_y_gt   = record_gt.getY();
  double nav_alt_gt = m_model.getWaterDepth() - record_gt.getDepth();
  if(nav_alt_gt < 0)
    nav_alt_gt = 0;

  bool dual_state = m_model.usingDualState();

  double nav_lat = 0;
  double nav_lon = 0;
  double nav_lat_gt = 0;
  double nav_lon_gt = 0;
  if(m_geo_ok) {
#ifdef USE_UTM
    m_geodesy.UTM2LatLong(nav_x, nav_y, nav_lat, nav_lon);
    m_geodesy.UTM2LatLong(nav_x_gt, nav_y_gt, nav_lat_gt, nav_lon_gt);
#else
    m_geodesy.LocalGrid2LatLong(nav_x_gt, nav_y_gt, nav_lat_gt, nav_lon_gt);
    m_geodesy.LocalGrid2LatLong(nav_x_gt, nav_y_gt, nav_lat_gt, nav_lon_gt);
#endif
 }

  string datum_lat = doubleToStringX(m_geodesy.GetOriginLatitude(),9);
  string datum_lon = doubleToStringX(m_geodesy.GetOriginLongitude(),9);
  
  m_msgs << "Enabled: " + boolToString(m_enabled) << endl;
  m_msgs << "Datum: " + datum_lat + "," + datum_lon;
  m_msgs << endl << endl;
  // Part 1: Pose Information ===========================================
  ACTable actab(6,1);
  actab << "Start | Pose | Current | Pose (NAV) | Current | Pose (GT)";
  actab.addHeaderLines();
  actab.setColumnPadStr(1, "   "); // Pad w/ extra blanks between cols 1&2
  actab.setColumnPadStr(3, "   "); // Pad w/ extra blanks between cols 3&4
  actab.setColumnJustify(0, "right");
  actab.setColumnJustify(2, "right");
  actab.setColumnJustify(4, "right");

  actab << "Headng:" << m_start_nav_hdg;
  actab << "Headng:" << doubleToStringX(record.getHeading(),1);
  if(dual_state)
    actab << "Headng:" << doubleToStringX(record.getHeading(),1);
  else
    actab << "(same)" << "-";

  actab << "Speed:" << m_start_nav_spd;
  actab << "Speed:" << doubleToStringX(record.getSpeed(),2);
  if(dual_state)
    actab << "Speed:" << doubleToStringX(record_gt.getSpeed(),2);
  else
    actab << " " << "-";

  actab << "Depth:" << m_start_nav_dep;
  actab << "Depth:" << doubleToStringX(record.getDepth(),1);
  if(dual_state)
    actab << "Depth:" << doubleToStringX(record_gt.getDepth(),1);
  else
    actab << " " << "-";

  actab << "Alt:" << m_start_nav_alt;
  actab << "Alt:" << doubleToStringX(nav_alt,1);
  if(dual_state)
    actab << "Alt:" << doubleToStringX(nav_alt_gt,1);
  else
    actab << " " << "-";

  actab << "(X,Y):" << m_start_nav_x +","+ m_start_nav_y;
  actab << "(X,Y):" << doubleToStringX(nav_x,2) + "," + doubleToStringX(nav_y,2); 
  if(dual_state)
    actab << "(X,Y):" << doubleToStringX(nav_x_gt,2) +","+ doubleToStringX(nav_y_gt,2); 
  else
    actab << " " << "-";

  actab << "Lat:" << m_start_nav_lat;
  actab << "Lat:" << doubleToStringX(nav_lat,8);
  if(dual_state)
    actab << "Lat:" << doubleToStringX(nav_lat_gt,8);
  else
    actab << " " << "-";

  actab << "Lon:" << m_start_nav_lon;
  actab << "Lon:" << doubleToStringX(nav_lon,8);
  if(dual_state)
    actab << "Lon:" << doubleToStringX(nav_lon_gt,8);
  else
    actab << " " << "-";

  m_msgs << actab.getFormattedString();

  // Part 2: Buoyancy Info ==============================================
  string buoy_rate_now = doubleToStringX(m_model.getBuoyancyRate(),8);
  string buoy_srcs = "initial_config";
  if(m_srcs_buoyrate.size() != 0)
    buoy_srcs = setToString(m_srcs_buoyrate);

  m_msgs <<  endl;
  m_msgs <<  "Present Buoyancy rate: " << buoy_rate_now << endl;
  m_msgs <<  "        Starting rate: " << m_start_buoyrate << endl;
  m_msgs <<  "               Source: " << buoy_srcs << endl;

  // Part 3: External Drift Info =======================================
  m_msgs << endl;
  actab = ACTable(7,2);
  actab << "Ext Drift | X | Y | Mag | Ang | Rot. |Source(s)";
  actab.addHeaderLines();
  actab.setColumnJustify(0, "right");
  actab.setColumnPadStr(2, "  |  ");
  actab.setColumnPadStr(4, "  |  ");
  actab.setColumnPadStr(5, "  |  ");

  actab << "Starting" << m_start_drift_x << m_start_drift_y << m_start_drift_mag;
  actab << m_start_drift_ang << m_start_rotate_spd << "init_config";

  string drift_x   = doubleToStringX(m_model.getDriftX(),3);
  string drift_y   = doubleToStringX(m_model.getDriftY(),3);
  string drift_mag = doubleToStringX(m_model.getDriftMag(),4);
  string drift_ang = doubleToStringX(m_model.getDriftAng(),4);
  string rotate_spd = doubleToStringX(m_model.getRotateSpd(),4);
  if(drift_mag == "0")
    drift_ang = "0";
  string drift_srcs = "n/a";
  if(m_srcs_drift.size() != 0)
    drift_srcs = setToString(m_srcs_drift);
  actab << "Present" << drift_x << drift_y << drift_mag << drift_ang;
  actab << rotate_spd << drift_srcs;
  m_msgs << actab.getFormattedString();
  m_msgs << endl << endl;

  // Part 4: Speed/Thrust Info =======================================
  m_msgs << "Velocity Information: " << endl;
  m_msgs << "--------------------- " << endl;
  m_msgs << "     DESIRED_THRUST=" << doubleToStringX(m_model.getThrust(),1);
  m_msgs << " ==> SPEED=" << doubleToStringX(record.getSpeed(),2) << endl;
  bool using_thrust_factor = m_model.usingThrustFactor();
  m_msgs << "     Using Thrust_Factor: " << boolToString(using_thrust_factor);
  if(using_thrust_factor)
    m_msgs << " (" + doubleToStringX(m_model.getThrustFactor(),4) + ")";
  m_msgs << endl;
  
  string max_acceleration = doubleToStringX(m_model.getMaxAcceleration(),6);
  string max_deceleration = doubleToStringX(m_model.getMaxDeceleration(),6);
  string posmap = m_model.getThrustMapPos();
  string negmap = m_model.getThrustMapNeg();
  string thrust_mode = m_model.getThrustModeDiff();
  string desired_thrust_l = "n/a";
  string desired_thrust_r = "n/a";
  if(thrust_mode == "differential") {
    desired_thrust_l = doubleToStringX(m_model.getThrustLeft(),2);
    desired_thrust_r = doubleToStringX(m_model.getThrustRight(),2);
  }
  string thrust_mode_reverse = boolToString(m_model.getThrustModeReverse());

  if(posmap == "")
    posmap = "n/a";
  if(negmap == "")
    negmap = "n/a";
  m_msgs << "     Positive Thrust Map: " << posmap << endl;
  m_msgs << "     Negative Thrust Map: " << negmap << endl;
  m_msgs << "        Max Accereration: " << max_acceleration << endl;
  m_msgs << "        Max Decereration: " << max_deceleration << endl << endl;

  m_msgs << "             Thrust Mode: " << thrust_mode << endl;
  m_msgs << "     Thrust Mode Reverse: " << thrust_mode_reverse << endl;
  m_msgs << "        DESIRED_THRUST_L: " << desired_thrust_l << endl;
  m_msgs << "        DESIRED_THRUST_R: " << desired_thrust_r << endl;



  // Part 5: Speed/Depth Change Info ===========================
  string max_depth_rate   = doubleToStringX(m_model.getMaxDepthRate(),6);
  string max_depth_rate_v = doubleToStringX(m_model.getMaxDepthRateSpd(),6);
  m_msgs << endl;
  m_msgs << "Depth Information: " << endl;
  m_msgs << "------------------ " << endl;
  m_msgs << "           Max Depth Rate: " << max_depth_rate   << endl;
  m_msgs << "     Max Depth Rate Speed: " << max_depth_rate_v << endl;
  m_msgs << "              Water Depth: " << m_model.getWaterDepth() << endl;

  return(true);
}









