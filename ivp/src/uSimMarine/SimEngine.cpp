/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SimEngine.cpp                                        */
/*    DATE: Mar 8th, 2005 just another day at CSAIL              */
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
#include "AngleUtils.h"
#include "SimEngine.h"

using namespace std;

//--------------------------------------------------------------------
// Procedure: propagate

void SimEngine::propagate(NodeRecord &record, double delta_time,
			  double prior_heading, double prior_speed,
			  double force_x, double force_y)
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
  double new_x = prev_x + (xdot * delta_time) + (force_x * delta_time);
  double new_y = prev_y + (ydot * delta_time) + (force_y * delta_time);
  double new_time = record.getTimeStamp() + delta_time;
  double new_sog = hypot((xdot + force_x), (ydot + force_y));
  double new_hog = relAng(prev_x, prev_y,  new_x, new_y);
  
  record.setSpeed(new_speed);
  record.setX(new_x);
  record.setY(new_y);
  record.setTimeStamp(new_time);
  record.setSpeedOG(new_sog);
  record.setHeadingOG(new_hog);
}

//--------------------------------------------------------------------
// Procedure: propagateDepth

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
    double actuator_depth_rate = (elevator_angle/100) * depth_rate;
    double total_depth_rate = (-buoyancy_rate) +  actuator_depth_rate;

    double new_depth = prev_depth + (1 * total_depth_rate * delta_time);
    record.setDepth(new_depth);
  }
    
  if(record.getDepth() < 0)
    record.setDepth(0);
}


//--------------------------------------------------------------------
// Procedure: propagateSpeed

void SimEngine::propagateSpeed(NodeRecord& record, const ThrustMap& tmap,
			       double delta_time, double thrust,
			       double rudder, double max_accel, 
			       double max_decel)
{
  if(delta_time <= 0)
    return;

  double next_speed  = tmap.getSpeedValue(thrust);
  double prev_speed  = record.getSpeed();
  
  // Apply a slowing penalty proportional to the rudder/turn
  rudder = vclip(rudder, -100, 100);
  double rudder_magnitude = fabs(rudder);
  double vpct = (rudder_magnitude / 100) * 0.85;
  next_speed *= (1.0 - vpct);

  if(next_speed > prev_speed) {
    double acceleration = (next_speed - prev_speed) / delta_time;
    if((max_accel > 0) && (acceleration > max_accel))
      next_speed = (max_accel * delta_time) + prev_speed;
  }

  if(next_speed < prev_speed) {
    double deceleration = (prev_speed - next_speed) / delta_time;
    if((max_decel > 0) && (deceleration > max_decel))
      next_speed = (max_decel * delta_time * -1) + prev_speed;
  }
  record.setSpeed(next_speed);
}


//--------------------------------------------------------------------
// Procedure: propagateHeading

void SimEngine::propagateHeading(NodeRecord& record,
				 double delta_time, 
				 double rudder,
				 double thrust,
				 double turn_rate,
				 double torque_theta)
{
  // Assumption is that the thruster and rudder are on the same
  // actuator, e.g., like the kayaks, or typical UUVs. A rotated
  // thruster contributes nothing to a turn unless the prop is 
  // moving.
  double speed = record.getSpeed();
  if(speed == 0) 
    rudder = 0;

  // Even if speed is zero, need to continue on in case the 
  // torque is non-zero.
  rudder    = vclip(rudder, -100, 100);
  turn_rate = vclip(turn_rate, 0, 100);
  
  // Step 1: Calculate raw delta change in heading
  double delta_deg = rudder * (turn_rate/100) * delta_time;

  // Step 2: Calculate change in heading factoring thrust
  delta_deg = (1 + ((thrust-50)/50)) * delta_deg;

  // Step 3: Calculate change in heading factoring external force
  delta_deg += (delta_time * torque_theta);

  // Step 4: Calculate final new heading in the range [0,359]
  double prev_heading = record.getHeading();
  double new_heading  = angle360(delta_deg + prev_heading);
  record.setHeading(new_heading);
}

