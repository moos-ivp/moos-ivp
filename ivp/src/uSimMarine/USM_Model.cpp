/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: USM_Model.cpp                                        */
/*    DATE: Nov 19th 2006 (as separate class under MVC paradigm  */
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

  m_thrust_map.setThrustFactor(20);

  // Initalize the state variables
  m_rudder       = 0;
  m_thrust       = 0;
  m_elevator     = 0;
  m_force_x      = 0;
  m_force_y      = 0; 
  m_torque_theta = 0;
  m_force_fresh  = true;
  m_water_depth  = 0;    // zero means nothing known, no altitude reported

}

//------------------------------------------------------------------------
// Procedure: resetTime()

void USM_Model::resetTime(double g_curr_time)
{
  m_record.setTimeStamp(g_curr_time);
  m_record_gt.setTimeStamp(g_curr_time);
}

//------------------------------------------------------------------------
// Procedure: setParam

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
  }
  else if(param == "buoyancy_rate")
    m_buoyancy_rate = value;
  else if(param == "turn_rate")
    m_turn_rate = vclip(value, 0, 100);
  else if(param == "force_x")
    m_force_x = value;
  else if(param == "force_y")
    m_force_y = value;
  else if(param == "torque_theta")
    m_torque_theta = value;
  else if(param == "max_acceleration") {
    m_max_acceleration = value;
    if(m_max_acceleration < 0)
      m_max_acceleration = 0;
  }
  else if(param == "max_deceleration") {
    m_max_deceleration = value;
    if(m_max_deceleration < 0)
      m_max_deceleration = 0;
  }
  else if(param == "max_depth_rate")
    m_max_depth_rate = value;
  else if(param == "max_depth_rate_speed")
    m_max_depth_rate_speed = value;
  else if(param == "water_depth") {
    if(value >= 0)
      m_water_depth = value;
  }
  else
    return(false);
  return(true);
}



//------------------------------------------------------------------------
// Procedure: propagate
//      Note: 

bool USM_Model::propagate(double g_curr_time)
{
  if(m_paused) {
    cout << "Simulator PAUSED..................." << endl;
    return(true);
  }
    
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
// Procedure: setForceVector(string, bool)

void USM_Model::setForceVector(string str, bool add_new_force)
{
  string left  = stripBlankEnds(biteString(str, ','));
  string right = stripBlankEnds(str);

  if(!isNumber(left) || !isNumber(right))
    return;
  
  double ang  = angle360(atof(left.c_str()));
  double mag  = atof(right.c_str());
  double rads = headingToRadians(ang);

  double xmps = cos(rads) * mag;
  double ymps = sin(rads) * mag;

  if(add_new_force) {
    m_force_x += xmps;
    m_force_y += ymps;
  }
  else {
    m_force_x = xmps;
    m_force_y = ymps;
  }

  m_force_fresh = true;
}

//--------------------------------------------------------------------
// Procedure: magForceVector(double)
//   Purpose: Grow the existing force vector by the specified percent.
//            Negative values allowed, but each will flip the direction
//            of the vector.

void USM_Model::magForceVector(double pct)
{
  double ang = relAng(0, 0, m_force_x, m_force_y);
  double mag = hypot(m_force_x, m_force_y);

  double new_mag = mag * pct;
  double rads = headingToRadians(ang);

  double xmps = cos(rads) * new_mag;
  double ymps = sin(rads) * new_mag;

  m_force_x = xmps;
  m_force_y = ymps;

  m_force_fresh = true;
}

//------------------------------------------------------------------------
// Procedure: setPaused
//      Note: 

void USM_Model::setPaused(bool g_paused)
{
  if(m_paused == g_paused)
    return;
  else
    m_paused = g_paused;
 
  if(m_paused)
    m_pause_timer.start();
  else
    m_pause_timer.stop();  
}

//------------------------------------------------------------------------
// Procedure: setThrustFactor
//      Note: 

void USM_Model::setThrustFactor(double value)
{
  m_thrust_map.setThrustFactor(value);
}

//------------------------------------------------------------------------
// Procedure: setThrustReflect

void USM_Model::setThrustReflect(bool value)
{
  m_thrust_map.setReflect(value);
}

//--------------------------------------------------------------------- 
// Procedure: addThrustMapping

bool USM_Model::addThrustMapping(double thrust, double speed)
{
  bool result = m_thrust_map.addPair(thrust, speed);
  return(result);
}

//---------------------------------------------------------------------
// Procedure: getForceSummary()

string USM_Model::getForceSummary()
{
  // Revert to c^2 = a^2 + b^2 
  double c_squared = (m_force_x * m_force_x) + (m_force_y * m_force_y);
  double magnitude = sqrt(c_squared);
  double angle = relAng(0, 0, m_force_x, m_force_y);

  string val = "ang=";
  val += doubleToStringX(angle,2);
  val += ", mag=";
  val += doubleToStringX(magnitude,2);
  val += ", xmag=";
  val += doubleToStringX(m_force_x,3);
  val += ", ymag=";
  val += doubleToStringX(m_force_y,3);
  return(val);
}


//------------------------------------------------------------------------
// Procedure: initPosition
//
//  "x=20, y=-35, speed=2.2, heading=180, depth=20"


bool USM_Model::initPosition(const string& str)
{
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
// Procedure: propagateNodeRecord

void USM_Model::propagateNodeRecord(NodeRecord& record, 
				    double delta_time, 
				    bool apply_external_forces)
{
  double prior_spd = record.getSpeed();
  double prior_hdg = record.getHeading();

  m_sim_engine.propagateSpeed(record, m_thrust_map, delta_time,
			      m_thrust, m_rudder, m_max_acceleration,
			      m_max_deceleration);

  m_sim_engine.propagateHeading(record, delta_time, m_rudder, 
				m_thrust, m_turn_rate, 
				m_torque_theta);

  m_sim_engine.propagateDepth(record, delta_time, 
			      m_elevator, m_buoyancy_rate, 
			      m_max_depth_rate,
			      m_max_depth_rate_speed);

  // Calculate the total external forces on the vehicle first.
  double total_force_x = 0;
  double total_force_y = 0;
  
  if(apply_external_forces) {
    total_force_x = m_force_x;
    total_force_y = m_force_y;
  }

  m_sim_engine.propagate(record, delta_time, prior_hdg, prior_spd,
			 total_force_x, total_force_y);

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

