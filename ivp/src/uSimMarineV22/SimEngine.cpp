/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: SimEngine.cpp                                        */
/*    DATE: Mar 8th, 2005 just another day at CSAIL              */
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
#include "AngleUtils.h"
#include "SimEngine.h"

using namespace std;

//-----------------------------------------------------------------
// Constructor()

SimEngine::SimEngine()
{
  m_verbose = false;
  m_turn_spd_loss = 0.85;  // [0,1] 0:no_loss, 1:halt at full rudder

  m_thrust_mode_reverse = false;
}

//-----------------------------------------------------------------
// Procedure: setTurnSpdLoss()

void SimEngine::setTurnSpdLoss(double dval)
{
  if((dval < 0) || (dval > 1))
    return;
  m_turn_spd_loss = dval;
}

//-----------------------------------------------------------------
// Procedure: propagate()

void SimEngine::propagate(NodeRecord &record, double delta_time,
			  double prior_heading, double prior_speed,
			  double drift_x, double drift_y)
{
  double speed   = (record.getSpeed() + prior_speed) / 2;

  double s = sin(degToRadians(prior_heading)) +
    sin(degToRadians(record.getHeading()));

  double c = cos(degToRadians(prior_heading)) +
    cos(degToRadians(record.getHeading()));

  double hdg_rad = atan2(s, c);

  double prev_x = record.getX();
  double prev_y = record.getY();

  double cos_ang = cos(hdg_rad);
  double sin_ang = sin(hdg_rad);

  double xdot  = (sin_ang * speed);
  double ydot  = (cos_ang * speed);

  double new_speed = hypot(xdot, ydot);
  if(speed < 0)               // Added mikerb sep1514 bugfix
    new_speed = -new_speed;

  double new_x = prev_x + (xdot * delta_time) + (drift_x * delta_time);
  double new_y = prev_y + (ydot * delta_time) + (drift_y * delta_time);
  double new_time = record.getTimeStamp() + delta_time;
  double new_sog = hypot((xdot + drift_x), (ydot + drift_y));
  double new_hog = relAng(prev_x, prev_y,  new_x, new_y);
  
  record.setSpeed(new_speed);
  record.setX(new_x);
  record.setY(new_y);
  record.setTimeStamp(new_time);
  record.setSpeedOG(new_sog);
  record.setHeadingOG(new_hog);
}

//--------------------------------------------------------------------
// Procedure: propagateDepth()

void SimEngine::propagateDepth(NodeRecord& record,
			       double delta_time,
			       double elevator_angle, 
			       double buoyancy_rate,
			       double max_depth_rate, 
			       double max_depth_rate_speed)
{
  double speed = record.getSpeed();
  double prev_depth = record.getDepth();
  elevator_angle = vclip(elevator_angle, -100, 100);
  if(speed <= 0) {
    double new_depth = prev_depth + (-1 * buoyancy_rate * delta_time);
    record.setDepth(new_depth);
    record.setPitch(0.0);
  }
  else {
    double pct = 1.0;
    if(max_depth_rate_speed > 0) {
      pct = (speed / max_depth_rate_speed);
      if(pct > 1.0)
	pct = 1.0;
    }
    if(pct < 0)
      pct = -1 * sqrt(-1 * pct);
    else
      pct = sqrt(pct);
    double depth_rate = pct * max_depth_rate;
    double speed = record.getSpeed();
    double pitch_depth_rate = - sin(record.getPitch())*speed;
    double actuator_depth_rate = (elevator_angle/100) * depth_rate;
    double total_depth_rate = (-buoyancy_rate) +  pitch_depth_rate + actuator_depth_rate;

    double new_depth = prev_depth + (1 * total_depth_rate * delta_time);
    record.setDepth(new_depth);

    // Pitch added by HS 120124

    double pitch =0 ;
    if (speed > 0 && (fabs(pitch_depth_rate+actuator_depth_rate) <= speed))
      pitch = - asin((pitch_depth_rate+actuator_depth_rate)/speed);
    record.setPitch(pitch);
  }
    
  if(record.getDepth() < 0)
    record.setDepth(0);
}


//--------------------------------------------------------------------
// Procedure: propagateSpeed()

void SimEngine::propagateSpeed(NodeRecord& record, const ThrustMap& tmap,
			       double delta_time, double thrust,
			       double rudder, double max_accel, 
			       double max_decel, double max_sail_spd)
{
  if(delta_time <= 0)
    return;

  if(m_thrust_mode_reverse) {
    thrust = -thrust;
    rudder = -rudder;
  }

  double next_speed  = tmap.getSpeedValue(thrust);
  double prev_speed  = record.getSpeed();

  if((max_sail_spd > 0) && (next_speed > max_sail_spd))
    next_speed = max_sail_spd;

  if(m_verbose) {
    cout << "prev_speed:" << prev_speed << endl;
    cout << "next_speedA:" << next_speed << endl;
  }
  
  // Apply a slowing penalty proportional to the rudder/turn
  rudder = vclip(rudder, -100, 100);
  double rudder_magnitude = fabs(rudder);
  double vpct = (rudder_magnitude / 100) * m_turn_spd_loss;
  next_speed *= (1.0 - vpct);
  if(m_verbose) {
    cout << "rudder:" << doubleToStringX(rudder,4) << endl;
    cout << "fabs rudder:" << doubleToStringX(rudder_magnitude,4) << endl;
    cout << "vpct:" << doubleToStringX(vpct,4) << endl;
    cout << "next_speedB:" << next_speed << endl;
  }
  
  if(next_speed > prev_speed) {
    double acceleration = (next_speed - prev_speed) / delta_time;
    if(m_verbose) {
      cout << "delta_time:" << doubleToStringX(delta_time, 5) << endl;
      cout << "acceleration:" << doubleToStringX(acceleration,5) << endl;
      cout << "max_accel:" << max_accel << endl;
    }
    if((max_accel > 0) && (acceleration > max_accel))
      next_speed = (max_accel * delta_time) + prev_speed;
  }

  if(next_speed < prev_speed) {
    double deceleration = (prev_speed - next_speed) / delta_time;
    if((max_decel > 0) && (deceleration > max_decel))
      next_speed = (max_decel * delta_time * -1) + prev_speed;
  }

  if(m_verbose) 
    cout << "next_speedC:" << next_speed << endl;

  record.setSpeed(next_speed);
}


//--------------------------------------------------------------------
// Procedure: propagateHeading()

void SimEngine::propagateHeading(NodeRecord& record,
				 double delta_time, 
				 double rudder,
				 double thrust,
				 double turn_rate,
				 double rotate_speed)
{
  // Assumption is that the thruster and rudder are on the same
  // actuator, e.g., like the kayaks, or typical UUVs. A rotated
  // thruster contributes nothing to a turn unless the prop is 
  // moving.
  double speed = record.getSpeed();
  if(speed == 0) 
    rudder = 0;

  if(m_thrust_mode_reverse) {
    thrust = -thrust;
    rudder = -rudder;
  }

  // Added Feb 4th, 2022 mikerb
  m_turn_speed_map.setFullRate(turn_rate);
  m_turn_speed_map.setFullSpeed(5);
  turn_rate = m_turn_speed_map.getTurnRate(speed);
  
  // Even if speed is zero, need to continue on in case the 
  // torque is non-zero.
  rudder    = vclip(rudder, -100, 100);
  turn_rate = vclip(turn_rate, 0, 100);
  
  // Step 1: Calculate raw delta change in heading
  double delta_deg = rudder * (turn_rate/100) * delta_time;

  // Step 2: Calculate change in heading factoring thrust
  delta_deg = (1 + ((thrust-50)/50)) * delta_deg;

  // Step 3: Calculate change in heading factoring external drift
  delta_deg += (delta_time * rotate_speed);

  // Step 4: Calculate final new heading in the range [0,359]
  double prev_heading = record.getHeading();
  double new_heading  = angle360(delta_deg + prev_heading);
  record.setHeading(new_heading);
  record.setYaw(-degToRadians(angle180(new_heading)));
}

//--------------------------------------------------------------------
// Procedure: propagateSpeedDiffMode()

void SimEngine::propagateSpeedDiffMode(NodeRecord& record, const ThrustMap& tmap,
				       double delta_time, 
				       double thrust_lft,
				       double thrust_rgt, 
				       double max_accel, 
				       double max_decel)
{
  // Sanity check
  if(delta_time <= 0)
    return;

  double thrust = (thrust_lft + thrust_rgt) / 2.0;

  // Calculate a pseudo rudder position in the range of [-100, 100]
  // Since the rudder_diff is in the range [-200, 200], just divide by 2.
  double rudder = (thrust_lft - thrust_rgt) / 2;

  propagateSpeed(record, tmap, delta_time, thrust, rudder, max_accel, max_decel);  
}


//--------------------------------------------------------------------
// Procedure: propagateHeadingDiffMode()

void SimEngine::propagateHeadingDiffMode(NodeRecord& record,
					 double delta_time, 
					 double thrust_lft,
					 double thrust_rgt,
					 double turn_rate,
					 double rotate_speed)
{
  // Sanity check
  if(delta_time <= 0)
    return;

  if(m_thrust_mode_reverse) {
    double tmp = thrust_lft;
    thrust_lft = -thrust_rgt;
    thrust_rgt = tmp;
  }

  // Calculate the raw magnitude of the turn component. Since both thrusts
  // range in [-100, 100], the maximum difference is 200. The turn_mag 
  // should range between [-1, 1].
  double turn_mag = (thrust_lft - thrust_rgt) / 200;

  // double rate_mag = (turn_rate / 100);

  // Initially we ballpark that a full-thrust forward left and full-thrust
  // backward left should turn the vehicle in 10 seconds, or 36 degrees/sec
  
  double degs_per_second = 36 * turn_mag;    // [-36, 36]

  // Step 1: Calculate raw delta change in heading
  double delta_deg = degs_per_second * delta_time;

  // Step 2: Calculate change in heading factoring external drift
  delta_deg += (delta_time * rotate_speed);

  // Step 3: Calculate final new heading in the range [0,359]
  double prev_heading = record.getHeading();
  double new_heading  = angle360(delta_deg + prev_heading);
  record.setHeading(new_heading);
  record.setYaw(-degToRadians(angle180(new_heading)));
}










