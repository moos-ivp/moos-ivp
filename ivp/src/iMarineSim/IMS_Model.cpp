/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IMS_Model.cpp                                        */
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
#include <math.h> 
#include <stdlib.h>
#include "IMS_Model.h"
#include "SimEngine.h"
#include "MBUtils.h"
#include "AngleUtils.h"

#ifndef M_PI
  #define M_PI 3.141592653589793
#endif

using namespace std;

//------------------------------------------------------------------------
// Constructor

IMS_Model::IMS_Model() 
{
  m_rudder         = 0;
  m_thrust         = 0;
  m_elevator       = 0;
  m_force_x        = 0;
  m_force_y        = 0; 
  m_torque_theta   = 0;
  m_float_rate     = 0;
  m_deceleration   = 0.5;
  m_paused         = false;
  m_thrust_factor  = 20;
}

//------------------------------------------------------------------------
// Procedure: propagate
//      Note: 

bool IMS_Model::propagate(double g_curr_time)
{
  if(m_paused) {
    cout << "Simulator PAUSED..................." << endl;
    return(true);
  }
    
  // Calculate actual current time considering time spent paused.
  double a_curr_time = g_curr_time - m_pause_timer.get_wall_time();
  double delta_time  = a_curr_time - m_vstate.m_dfTime;
  double next_speed  = m_thrust / m_thrust_factor;

  //m_vstate.m_dfSpeed = next_speed; // mikerb

  // Convert rudder_angle to RADIANS
  double radian_rudder_angle = m_rudder * M_PI / 180.0;
  
  // Check saftey limits
#if 0
  if(m_rudder_angle < 0)
    m_rudder_angle = -(min(fabs(m_rudder), m_max_rotat_vel));
  else
    m_rudder_angle =  (min(fabs(m_rudder), m_max_rotat_vel));
#endif
  
  m_sim_engine.setParam("force_x", m_force_x);
  m_sim_engine.setParam("force_y", m_force_y);
  m_sim_engine.setParam("torque_theta", m_torque_theta);
  m_sim_engine.setParam("float_rate", m_float_rate);
  m_sim_engine.setParam("deceleration", m_deceleration);
  m_sim_engine.propagate(m_vstate, next_speed, radian_rudder_angle, 
			 delta_time, m_elevator);
   
  return(true);
}

//------------------------------------------------------------------------
// Procedure: getYaw()

double IMS_Model::getYaw()
{
  double heading = radToDegrees(radAngleWrap(m_vstate.m_dfHeading));
  double yaw     = (heading * 3.1415926) / -180.0;

  return(yaw);
}

//------------------------------------------------------------------------
// Procedure: getHeading()

double IMS_Model::getHeading()
{
  double heading = radToDegrees(radAngleWrap(m_vstate.m_dfHeading));

  heading = angle360(heading);  // added jun1410 mikerb

  return(heading);
}

//--------------------------------------------------------------------
// Procedure: getForceMagnitude

double IMS_Model::getForceMagnitude()
{
  // Revert to c^2 = a^2 + b^2 
  double c_squared = (m_force_x * m_force_x) + (m_force_y * m_force_y);
  double c = sqrt(c_squared);
  return(c);
}

//--------------------------------------------------------------------
// Procedure: getForceAngle

double IMS_Model::getForceAngle()
{
  if(m_force_x == 0) {
    if(m_force_y >= 0)
      return(0);
    else
      return(180);
  }

  double radians = atan(m_force_y / m_force_x);
  double degrees = radToHeading(radians);
    
  return(degrees);
}

//--------------------------------------------------------------------
// Procedure: setForceVector(string, bool)

void IMS_Model::setForceVector(string str, bool add_new_force)
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
}

//------------------------------------------------------------------------
// Procedure: reset()

void IMS_Model::reset(double g_curr_time)
{
  m_vstate = VState();
  
  m_vstate.m_dfTime    = g_curr_time;
  m_vstate.m_dfX       = 0.0;
  m_vstate.m_dfY       = 0.0;
  m_vstate.m_dfSpeed   = 0.0;
  m_vstate.m_dfHeading = 0.0;

  m_paused         = false;
  m_force_x         = 0;
  m_force_y         = 0;
  m_rudder         = 0;
  m_thrust         = 0;
  m_elevator       = 0;
}


//------------------------------------------------------------------------
// Procedure: setPaused
//      Note: 

void IMS_Model::setPaused(bool g_paused)
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
// Procedure: setPosition
//     Notes: Handle strings such as:  "23, 900, 180, 1.2, 10"
//            Assumes field (1) x-postion, (2) y-position, (3) heading
//            (4) speed, (5) depth 

void IMS_Model::setPosition(string str)
{
  if(strContains(str, "="))
    return(setPositionPairs(str));

  str = stripBlankEnds(str);
  vector<string> svector = parseString(str, ',');
  int vsize = svector.size();

  for(int i=0; i<vsize; i++)
    svector[i] = stripBlankEnds(svector[i]);

  if(vsize > 0)
    setPositionX(atof(svector[0].c_str()));
  if(vsize > 1)
    setPositionY(atof(svector[1].c_str()));
  if(vsize > 2)
    setHeadingDEG(atof(svector[2].c_str()));
  if(vsize > 3)
    setSpeed(atof(svector[3].c_str()));
  if(vsize > 4)
    setDepth(atof(svector[4].c_str()));
}


//------------------------------------------------------------------------
// Procedure: setPositionPairs
//      Note: Handle strings such as:
//            "x=23, y=-900, hdg=180, spd=1.2, dep=10"

void IMS_Model::setPositionPairs(string str)
{
  if(!strContains(str, "="))
    return(setPosition(str));

  str = stripBlankEnds(str);
  vector<string> svector = parseString(str, ',');
  int vsize = svector.size();

  for(int i=0; i<vsize; i++) {
    svector[i] = stripBlankEnds(svector[i]);
    string left  = tolower(stripBlankEnds(biteString(svector[i],'=')));
    string right = tolower(stripBlankEnds(svector[i]));
    if(left == "x")
      setPositionX(atof(right.c_str()));
    else if(left == "y")
      setPositionY(atof(right.c_str()));
    else if((left == "hdg") || (left == "heading") || (left == "deg"))
      setHeadingDEG(atof(right.c_str()));
    else if((left == "spd") || (left == "speed"))
      setSpeed(atof(right.c_str()));
    else if((left == "dep") || (left == "depth"))
      setDepth(atof(right.c_str()));
  }
}

//------------------------------------------------------------------------
// Procedure: printSummary()

void IMS_Model::printSummary()
{
  cout << "  F(X):" << m_force_x; 
  cout << ", F(Y):" << m_force_y; 
  cout << ", F(Ang):" << getForceAngle(); 
  cout << ", F(Mag):" << getForceMagnitude() << endl; 
}
