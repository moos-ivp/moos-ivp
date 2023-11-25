/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: USM_Model.cpp                                        */
/*    DATE: Nov 19th 2006 (as separate class under MVC paradigm  */
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
#include <cmath> 
#include <cstdlib>
#include "USM_Model.h"
#include "SimEngine.h"
#include "MBUtils.h"
#include "AngleUtils.h"

using namespace std;

//------------------------------------------------------------------------
// Constructor

USM_Model::USM_Model() 
{
  // Initalize the configuration variables
  m_dual_state           = false;
  m_paused               = false;
  m_turn_rate            = 70;
  m_max_acceleration     = 0;
  m_max_deceleration     = 0.5;
  m_buoyancy_rate        = 0.025;  // positively buoyant
  m_max_depth_rate       = 0.5;    // meters per second
  m_max_depth_rate_speed = 2.0;    // meters per second

  m_max_rudder_degs_per_sec = 0;
  m_reset_count = 0;
  
  m_thrust_map.setThrustFactor(20);

  // Initalize the state variables
  m_rudder       = 0;
  m_rudder_prev  = 0;
  m_rudder_tstamp = 0;

  m_thrust       = 0;
  m_elevator     = 0;
  m_drift_x      = 0;
  m_drift_y      = 0; 
  m_rotate_speed = 0;
  m_drift_fresh  = true;
  m_water_depth  = 0;    // zero means nothing known, no altitude reported

  m_thrust_mode  = "normal";  // vs. "differential"
  m_thrust_lft   = 0;
  m_thrust_rgt   = 0;

  m_thrust_mode_reverse = false;

  m_geo_ok = false;
  m_obstacle_hit = false;
}

//------------------------------------------------------------------------
// Procedure: resetTime()

void USM_Model::resetTime(double g_curr_time)
{
  m_record.setTimeStamp(g_curr_time);
  m_record_gt.setTimeStamp(g_curr_time);
}

//------------------------------------------------------------------------
// Procedure: setDualState()

bool USM_Model::setDualState(string value)
{
  return(setBooleanOnString(m_dual_state, value));
}

//------------------------------------------------------------------------
// Procedure: setPaused()

bool USM_Model::setPaused(string value)
{
  bool g_paused;
  bool ok = setBooleanOnString(g_paused, value);
  if(!ok)
    return(false);

  if(m_paused == g_paused)
    return(true);

  m_paused = g_paused;
  if(m_paused)
    m_pause_timer.start();
  else
    m_pause_timer.stop();  

  return(true);
}

//------------------------------------------------------------------------
// Procedure: sailingEnabled()

bool USM_Model::sailingEnabled() const
{
  return(m_thrust_mode == "sailing");
}


//------------------------------------------------------------------------
// Procedure: getWindArrowSpec()

string USM_Model::getWindArrowSpec() const
{
  return(m_wind_model.getArrowSpec());
}


//------------------------------------------------------------------------
// Procedure: getPolarPlotSpec()

string USM_Model::getPolarPlotSpec() const
{
  return(m_polar_plot.getSpec());
}


//------------------------------------------------------------------------
// Procedure: getWindModelSpec()

string USM_Model::getWindModelSpec() const
{
  return(m_wind_model.getModelSpec());
}


//------------------------------------------------------------------------
// Procedure: setTSMapFullSpeed()

bool USM_Model::setTSMapFullSpeed(string value)
{
  if(!isNumber(value))
    return(false);

  return(m_turn_speed_map.setFullSpeed(atof(value.c_str())));
}

//------------------------------------------------------------------------
// Procedure: setTSMapNullSpeed()

bool USM_Model::setTSMapNullSpeed(string value)
{
  if(!isNumber(value))
    return(false);

  return(m_turn_speed_map.setNullSpeed(atof(value.c_str())));
}

//------------------------------------------------------------------------
// Procedure: setTSMapFullRate()

bool USM_Model::setTSMapFullRate(string value)
{
  if(!isNumber(value))
    return(false);

  return(m_turn_speed_map.setFullRate(atof(value.c_str())));
}

//------------------------------------------------------------------------
// Procedure: setTSMapNullRate()

bool USM_Model::setTSMapNullRate(string value)
{
  if(!isNumber(value))
    return(false);

  return(m_turn_speed_map.setNullRate(atof(value.c_str())));
}

//------------------------------------------------------------------------
// Procedure: setThrustModeDiff()

bool USM_Model::setThrustModeDiff(string value)
{
  value = tolower(value);
  if((value == "differential") || (value == "true"))
    m_thrust_mode = "differential";
  else if((value == "normal") || (value == "false"))
    m_thrust_mode = "normal";
  else
    return(false);

  return(true);
}


//------------------------------------------------------------------------
// Procedure: setThrustModeReverse()

bool USM_Model::setThrustModeReverse(string value)
{
  value = tolower(value);
  if((value == "reverse") || (value == "true"))
    m_thrust_mode_reverse = true;
  else if((value == "normal") || (value == "false"))
    m_thrust_mode_reverse = false;
  else
    return(false);

  return(true);
}


//------------------------------------------------------------------------
// Procedure: setParam()

bool USM_Model::setParam(string param, double value)
{
  param = stripBlankEnds(tolower(param));
  if(param == "start_x") {
    m_record.setX(value);
    m_record_gt.setX(value);
  }
  else if(param == "start_y") {
    m_record.setY(value);
    m_record_gt.setY(value);
  }
  else if(param == "start_heading") {
    m_record.setHeading(value);
    m_record_gt.setHeading(value);
  }
  else if(param == "start_speed") {
    m_record.setSpeed(value);
    m_record_gt.setSpeed(value);
  }
  else if(param == "start_depth") {
    m_record.setDepth(value);
    m_record_gt.setDepth(value);
    if(value < 0) {
      m_record.setDepth(0);
      m_record_gt.setDepth(0);
      return(false);
    }
  }
  else if(param == "buoyancy_rate")
    m_buoyancy_rate = value;
  else if(param == "turn_rate")
    m_turn_rate = vclip(value, 0, 100);
  else if(param == "rotate_speed")
    m_rotate_speed = value;
  else if(param == "max_acceleration") {
    m_max_acceleration = value;
    if(m_max_acceleration < 0) {
      m_max_acceleration = 0;
      return(false);
    }
  }
  else if(param == "max_deceleration") {
    m_max_deceleration = value;
    if(m_max_deceleration < 0) {
      m_max_deceleration = 0;
      return(false);
    }
  }
  else if(param == "max_depth_rate")
    m_max_depth_rate = value;
  else if(param == "max_depth_rate_speed")
    m_max_depth_rate_speed = value;
  else if(param == "water_depth") {
    if(value >= 0)
      m_water_depth = value;
    else
      return(false);
  }
  else
    return(false);

  return(true);
}

//------------------------------------------------------------------------
// Procedure: setParam()

bool USM_Model::setParam(string param, string value)
{
  param = stripBlankEnds(tolower(param));
  if(param == "wind_conditions") {
    bool ok = m_wind_model.setConditions(value);
    if(!ok)
      return(false);
    m_polar_plot.setWindAngle(m_wind_model.getWindDir());
  }
  else if(param == "polar_plot") {
    bool ok = m_polar_plot.addSetPoints(value);
    if(!ok)
      return(false);
  }
  else
    return(false);

  if(m_wind_model.set() && m_polar_plot.set())
    m_thrust_mode = "sailing";
  
  return(true);
}

//------------------------------------------------------------------------
// Procedure: informX()

void USM_Model::informX(double nav_x)
{
  m_record.setX(nav_x);
  m_record_gt.setX(nav_x);
  m_rudder = 0;
}

//------------------------------------------------------------------------
// Procedure: informY()

void USM_Model::informY(double nav_y)
{
  m_record.setY(nav_y);
  m_record_gt.setY(nav_y);
  m_rudder = 0;
}

//------------------------------------------------------------------------
// Procedure: informHeading()

void USM_Model::informHeading(double nav_hdg)
{
  m_record.setHeading(nav_hdg);
  m_record_gt.setHeading(nav_hdg);
  m_rudder = 0;
}

//------------------------------------------------------------------------
// Procedure: setRudder()

void USM_Model::setRudder(double desired_rudder)
{
  if(m_thrust_mode == "differential")
    return;

  m_rudder = desired_rudder;
}

//------------------------------------------------------------------------
// Procedure: setThrust()

void USM_Model::setThrust(double desired_thrust)
{
  if(m_thrust_mode == "differential")
    return;

  m_thrust = desired_thrust;
}

//------------------------------------------------------------------------
// Procedure: setThrustFan()

void USM_Model::setThrustFan(double desired_thrust_fan)
{
  if(m_thrust_mode != "sailing")
    return;

  m_thrust_fan = desired_thrust_fan;
}

//------------------------------------------------------------------------
// Procedure: setThrustLeft()

void USM_Model::setThrustLeft(double val)
{
  if(m_thrust_mode != "differential")
    return;
      
  if(val < -100)
    val = -100;
  else if(val > 100)
    val = 100;

  m_thrust_mode = "differential";

  if(m_thrust_mode_reverse == false)  // The normal mode
    m_thrust_lft  = val;
  else
    m_thrust_rgt  = -val;
}

//------------------------------------------------------------------------
// Procedure: setThrustRight()

void USM_Model::setThrustRight(double val)
{
  if(m_thrust_mode != "differential")
    return;

  if(val < -100)
    val = -100;
  else if(val > 100)
    val = 100;

  if(m_thrust_mode_reverse == false)  // The normal mode
    m_thrust_rgt  = val;
  else
    m_thrust_lft  = -val;
}

//------------------------------------------------------------------------
// Procedure: setRudder()

void USM_Model::setRudder(double desired_rudder, double tstamp)
{
  if(m_thrust_mode == "differential")
    return;
  
  // Part 0: Copy the current rudder value to "previous" before overwriting
  m_rudder_prev = m_rudder;


  // Part 1: Calculate the maximum change in rudder
  double max_rudder_change = 100;
  if(m_max_rudder_degs_per_sec > 0) {
    double delta_time = tstamp - m_rudder_tstamp;
    max_rudder_change = (delta_time * m_max_rudder_degs_per_sec);
  }

  // Part 2: Handle the change requested
  double change = desired_rudder - m_rudder_prev;
  if(change > 0) { 
    if(change > max_rudder_change)
      change = max_rudder_change;
    m_rudder += change;
  }
  else {
    if(-change > max_rudder_change)
      change = -max_rudder_change;
    m_rudder += change;
  }

  //m_thrust_mode = "normal";
  m_rudder_tstamp = tstamp;
}


//------------------------------------------------------------------------
// Procedure: setGeodesy()

void USM_Model::setGeodesy(CMOOSGeodesy geodesy)
{
  m_geodesy = geodesy;
  m_geo_ok = true;
}

//------------------------------------------------------------------------
// Procedure: propagate()

bool USM_Model::propagate(double g_curr_time)
{
  if(m_paused) {
    cout << "Simulator PAUSED..................." << endl;
    return(true);
  }
  if(m_obstacle_hit)
    return(true);

  
  // Calculate actual current time considering time spent paused.
  double a_curr_time = g_curr_time - m_pause_timer.get_wall_time();
  double delta_time  = a_curr_time - m_record.getTimeStamp();

  if(m_dual_state) {
    propagateNodeRecord(m_record, delta_time, false);
    propagateNodeRecord(m_record_gt, delta_time, true);
  }
  else
    propagateNodeRecord(m_record, delta_time, true);
    
  return(true);
}

//--------------------------------------------------------------------
// Procedure: setDriftX()
//      Note: A null string source indicates a startup condition

bool USM_Model::setDriftX(double val, string source)
{
  m_drift_x = val;
  if((source != "") && !m_drift_sources.count(source)) {
    m_drift_sources.insert(source);
    m_drift_fresh = true;
  }
  return(true);
}

//--------------------------------------------------------------------
// Procedure: setDriftY()
//      Note: A null string source indicates a startup condition

bool USM_Model::setDriftY(double val, string source)
{
  m_drift_y = val;
  if((source != "") && !m_drift_sources.count(source)) {
    m_drift_sources.insert(source);
    m_drift_fresh = true;
  }
  return(true);
}

//--------------------------------------------------------------------
// Procedure: setDriftVector(string, bool)
//   Example: "angle,magnitude", "uTimerScript", true

bool USM_Model::setDriftVector(string str, string source, bool add_new_drift)
{
  string left  = biteStringX(str, ',');
  string right = str;

  if(!isNumber(left) || !isNumber(right))
    return(false);
  
  double ang  = angle360(atof(left.c_str()));
  double mag  = atof(right.c_str());
  double rads = headingToRadians(ang);

  double xmps = cos(rads) * mag;
  double ymps = sin(rads) * mag;

  if(add_new_drift) {
    setDriftX(m_drift_x + xmps, source);
    setDriftY(m_drift_y + ymps, source);
  }
  else {
    setDriftX(xmps, source);
    setDriftY(ymps, source);
  }
  return(true);
}

//--------------------------------------------------------------------
// Procedure: magDriftVector(double, source)
//   Purpose: Grow the existing drift vector by the specified percent.
//            Negative values allowed, but each will flip the direction
//            of the vector.

void USM_Model::magDriftVector(double pct, string source)
{
  double ang = relAng(0, 0, m_drift_x, m_drift_y);
  double mag = hypot(m_drift_x, m_drift_y);

  double new_mag = mag * pct;
  double rads = headingToRadians(ang);

  double xmps = cos(rads) * new_mag;
  double ymps = sin(rads) * new_mag;

  setDriftX(xmps, source);
  setDriftY(ymps, source);
}

//------------------------------------------------------------------------
// Procedure: setMaxRudderDegreesPerSec()

bool USM_Model::setMaxRudderDegreesPerSec(double v)
{
  if(v < 0)
    return(false);

  m_max_rudder_degs_per_sec = v;
  return(true);
}

//------------------------------------------------------------------------
// Procedure: setThrustFactor()

void USM_Model::setThrustFactor(double value)
{
  m_thrust_map.setThrustFactor(value);
}

//------------------------------------------------------------------------
// Procedure: setThrustReflect()

bool USM_Model::setThrustReflect(string value)
{
  if(tolower(value) == "true")
    m_thrust_map.setReflect(true);
  else if(tolower(value) == "false")
    m_thrust_map.setReflect(false);
  else
    return(false);

  return(true);
}

//--------------------------------------------------------------------- 
// Procedure: handleFullThrustMapping()

bool USM_Model::handleFullThrustMapping(string mapping)
{
  m_thrust_map = stringToThrustMap(mapping);
  return(m_thrust_map.isConfigured());
}

//--------------------------------------------------------------------- 
// Procedure: handleFullThrustMapFan()

bool USM_Model::handleFullThrustMapFan(string mapping)
{
  m_thrust_map_fan = stringToThrustMap(mapping);
  return(m_thrust_map_fan.isConfigured());
}

//--------------------------------------------------------------------- 
// Procedure: addThrustMapping()

bool USM_Model::addThrustMapping(double thrust, double speed)
{
  bool result = m_thrust_map.addPair(thrust, speed);
  return(result);
}

//------------------------------------------------------------------------
// Procedure: getDriftSources()

string USM_Model::getDriftSources() const
{
  return(setToString(m_drift_sources));
}

//---------------------------------------------------------------------
// Procedure: getDriftSummary()

string USM_Model::getDriftSummary()
{
  // Revert to c^2 = a^2 + b^2 
  double c_squared = (m_drift_x * m_drift_x) + (m_drift_y * m_drift_y);
  double magnitude = sqrt(c_squared);
  double angle = relAng(0, 0, m_drift_x, m_drift_y);

  string val = "ang=";
  val += doubleToStringX(angle,2);
  val += ", mag=";
  val += doubleToStringX(magnitude,2);
  val += ", xmag=";
  val += doubleToStringX(m_drift_x,3);
  val += ", ymag=";
  val += doubleToStringX(m_drift_y,3);
  return(val);
}


//------------------------------------------------------------------------
// Procedure: usingThrustFactor()

bool USM_Model::usingThrustFactor() const
{
  return(m_thrust_map.usingThrustFactor());
}

//------------------------------------------------------------------------
// Procedure: getThrustFactor()

double USM_Model::getThrustFactor() const
{
  return(m_thrust_map.getThrustFactor());
}


//------------------------------------------------------------------------
// Procedure: initPosition()
//
//  "x=20, y=-35, speed=2.2, heading=180, depth=20"


bool USM_Model::initPosition(string str)
{
  m_reset_count++;
  vector<string> svector = parseString(str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    svector[i] = tolower(stripBlankEnds(svector[i]));
    string param = biteStringX(svector[i], '=');
    string value = svector[i];

    // Support older style spec "5,10,180,2.0,0" - x,y,hdg,spd,dep
    if(value == "") {
      value = param;
      if(i==0)      param = "x";
      else if(i==1) param = "y";
      else if(i==2) param = "heading";
      else if(i==3) param = "speed";
      else if(i==4) param = "depth";
    }

    double dval  = atof(value.c_str());
    if(param == "x") {
      m_record.setX(dval);
      m_record_gt.setX(dval);
    }
    else if(param == "y") {
      m_record.setY(dval);
      m_record_gt.setY(dval);
    }
    else if((param == "heading") || (param=="deg") || (param=="hdg")) {
      m_record.setHeading(dval);
      m_record_gt.setHeading(dval);
    }
    else if((param == "speed") || (param == "spd")) {
      m_record.setSpeed(dval);
      m_record_gt.setSpeed(dval);
    }
    else if((param == "depth") || (param == "dep")) {
      m_record.setDepth(dval);
      m_record_gt.setDepth(dval);
    }
    else
      return(false);
  }
  return(true);
}

//------------------------------------------------------------------------
// Procedure: propagateNodeRecord()

void USM_Model::propagateNodeRecord(NodeRecord& record, 
				    double delta_time, 
				    bool apply_external_forces)
{
  double prior_spd = record.getSpeed();
  double prior_hdg = record.getHeading();

  m_sim_engine.setThrustModeReverse(m_thrust_mode_reverse);
  
  // Switch depending on thrust mode
  if(m_thrust_mode == "sailing") {
    double max_sail_spd = m_wind_model.getMaxSpeed(m_polar_plot, prior_hdg);

    cout << "USM_Model:: mode=sailing, rudder=" << m_rudder << endl;
    
    m_sim_engine.propagateSpeedSailing(record, m_thrust_map,
				       delta_time, m_thrust, m_rudder,
				       m_max_acceleration, m_max_deceleration,
				       m_thrust_map_fan, m_thrust_fan,
				       max_sail_spd);

    m_sim_engine.propagateHeading(record, delta_time, m_rudder, 
				  m_thrust, m_turn_rate, 
				  m_rotate_speed);
  }
    
  else if(m_thrust_mode == "differential") {
    m_sim_engine.propagateSpeedDiffMode(record, m_thrust_map, delta_time,
					m_thrust_lft, m_thrust_rgt,
					m_max_acceleration, m_max_deceleration);

    m_sim_engine.propagateHeadingDiffMode(record, delta_time, m_thrust_lft, 
					  m_thrust_rgt, m_turn_rate, 
					  m_rotate_speed);
  }
  else {
    m_sim_engine.propagateSpeed(record, m_thrust_map, delta_time,
				m_thrust, m_rudder, m_max_acceleration,
				m_max_deceleration);
    m_sim_engine.propagateHeading(record, delta_time, m_rudder, 
				  m_thrust, m_turn_rate, 
				  m_rotate_speed);
  }


  m_sim_engine.propagateDepth(record, delta_time, 
			      m_elevator, m_buoyancy_rate, 
			      m_max_depth_rate,
			      m_max_depth_rate_speed);

  // Begin - Handle thrust_mode reverse
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
  // End - Handle thrust_mode reverse
  
  // Calculate the total external forces on the vehicle first.
  double total_drift_x = 0;
  double total_drift_y = 0;
  
  if(apply_external_forces) {
    total_drift_x = m_drift_x;
    total_drift_y = m_drift_y;
  }

  m_sim_engine.propagate(record, delta_time, prior_hdg, prior_spd,
			 total_drift_x, total_drift_y);


  // If Geodesy is properly configured, update Lat/Lon based on x/y
  if(m_geo_ok) {
    double lat, lon;
#ifdef USE_UTM
    m_geodesy.UTM2LatLong(record.getX(), record.getY(), lat, lon);
#else
    m_geodesy.LocalGrid2LatLong(record.getX(), record.getY(), lat, lon);
#endif
    record.setLat(lat);
    record.setLon(lon);
  }
  
  // If m_water_depth > 0 then something is known about the present
  // water depth and thus we update the vehicle altitude.
  if(m_water_depth > 0) {
    double depth = record.getDepth();
    double altitude = m_water_depth - depth;
    if(altitude < 0) 
      altitude = 0;
    record.setAltitude(altitude);
  }
}

//------------------------------------------------------------------------
// Procedure: getDriftMag()

double USM_Model::getDriftMag() const
{
  return(hypot(m_drift_x, m_drift_y));
}

//------------------------------------------------------------------------
// Procedure: getDriftAng()

double USM_Model::getDriftAng() const
{
  return(relAng(0, 0, m_drift_x, m_drift_y));
}

//------------------------------------------------------------------------
// Procedure: cacheStartingInfo()

void USM_Model::cacheStartingInfo()
{
  NodeRecord record = m_record;
  double nav_x = m_record.getX();
  double nav_y = m_record.getY();
  double depth = m_water_depth;
  double nav_depth = m_record.getDepth();
  double nav_alt = depth - nav_depth;

  double nav_lat = 0;
  double nav_lon = 0;

#if 0
  if(m_geo_ok) {
#ifdef USE_UTM
    m_geodesy.UTM2LatLong(nav_x, nav_y, nav_lat, nav_lon);
#else
    m_geodesy.LocalGrid2LatLong(nav_x, nav_y, nav_lat, nav_lon);
#endif
  }
#endif
  
  m_start_nav_x     = doubleToStringX(nav_x,2);
  m_start_nav_y     = doubleToStringX(nav_y,2);
  m_start_nav_lat   = doubleToStringX(nav_lat,8);
  m_start_nav_lon   = doubleToStringX(nav_lon,8);
  m_start_nav_spd   = doubleToStringX(record.getSpeed(),2);
  m_start_nav_hdg   = doubleToStringX(record.getHeading(),1);
  m_start_nav_dep   = doubleToStringX(record.getDepth(),2);
  m_start_nav_alt   = doubleToStringX(nav_alt,2);
  m_start_buoyrate  = doubleToStringX(getBuoyancyRate(),8);
  m_start_drift_x   = doubleToStringX(getDriftX(),4);
  m_start_drift_y   = doubleToStringX(getDriftY(),4);
  m_start_drift_mag = doubleToStringX(getDriftMag(),4);
  m_start_drift_ang = doubleToStringX(getDriftAng(),4);
  m_start_rotate_spd = doubleToStringX(getRotateSpd(),4);

  m_start_datum_lat = "?"; 
  m_start_datum_lon = "?"; 
  if(m_geo_ok) {
    m_start_datum_lat = doubleToStringX(m_geodesy.GetOriginLatitude(),9);
    m_start_datum_lon = doubleToStringX(m_geodesy.GetOriginLongitude(),9);
  }

  // Added Feb 4th 2022 mikerb
  // Pass the user-configured TSM to the SimEngine
  m_sim_engine.setTurnSpeedMap(m_turn_speed_map);
}


