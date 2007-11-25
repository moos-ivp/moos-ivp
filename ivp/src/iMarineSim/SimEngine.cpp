/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
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
#include <math.h>
#include "AngleUtils.h"
#include "SimEngine.h"

using namespace std;

//--------------------------------------------------------------------
// Constructor

SimEngine::SimEngine(double g_push_x, double g_push_y, 
			  double g_float_rate)
{
  m_push_x  = g_push_x;
  m_push_y  = g_push_y;

  m_push_theta     = 0;
  m_float_rate     = 0;
  m_top_turn_speed = 3.0; // meters per second
}


//--------------------------------------------------------------------
// Procedure: setParam

bool SimEngine::setParam(const string& g_param, double g_value)
{
  if(g_param == "top_turn_speed") {
    if(m_top_turn_speed > 0) {
      m_top_turn_speed = g_value;
      return(true);
    }
  }
  if(g_param == "float_rate") {
    m_float_rate = g_value;
    return(true);
  }
  if(g_param == "push_x") {
    m_push_x = g_value;
    return(true);
  }
  if(g_param == "push_y") {
    m_push_y = g_value;
    return(true);
  }
  if(g_param == "push_theta") {
    m_push_theta = g_value;
    return(true);
  }

  return(false);
}


//--------------------------------------------------------------------
// Procedure: propagate
//      Note: rudder_angle is in RADIANS
//            delta_time is in SECONDS
//            velocity is in METERS / SECOND

void SimEngine::propagate(VState &vstate, double velocity,
			  double rudder_angle, double delta_time,
			  double elevator_angle)
{
  if(rudder_angle > 100)
    rudder_angle = 100;
  if(rudder_angle < -100)
    rudder_angle = -100;

  if(velocity == 0) {
    rudder_angle   = 0;
    elevator_angle = 0;
  }

  double delta_theta_deg = rudder_angle * 40 * delta_time;

  delta_theta_deg += (delta_time * m_push_theta);

  double pct = velocity / m_top_turn_speed;
  if(pct > 1.0)
    pct = 1.0;
  if(pct < -1.0)
    pct = -1.0;

  double rudder_magnitude = fabs(rudder_angle);
  
  double vpct = (rudder_magnitude / 100) * 0.85;
  velocity = velocity * (1.0 - vpct);
  

  double delta_theta_rad = degToRadians(delta_theta_deg);

  double vy_body =  velocity * cos(delta_theta_rad);
  double vx_body = -velocity * sin(delta_theta_rad);
  
  double cos_ang = cos(-vstate.m_dfHeading);
  double sin_ang = sin(-vstate.m_dfHeading);

  double xdot  = (cos_ang * vx_body) - (sin_ang * vy_body);
  double ydot  = (sin_ang * vx_body) + (cos_ang * vy_body);

  vstate.m_dfSpeed    = hypot(xdot, ydot);
  vstate.m_dfX       += (xdot * delta_time) + (m_push_x * delta_time);
  vstate.m_dfY       += (ydot * delta_time) + (m_push_y * delta_time);
  vstate.m_dfHeading += delta_theta_rad;
  vstate.m_dfHeading  = radAngleWrap(vstate.m_dfHeading);
  vstate.m_dfTime    += delta_time;

  if(velocity > 0) 
    vstate.m_dfDepth   += (elevator_angle/10.0) * delta_time;
  else
    vstate.m_dfDepth += (-1 * m_float_rate * delta_time);

  if(vstate.m_dfDepth < 0)
    vstate.m_dfDepth = 0;
}








