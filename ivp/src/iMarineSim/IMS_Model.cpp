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
  m_rudder        = 0;
  m_thrust        = 0;
  m_elevator      = 0;
  m_push_x        = 0;
  m_push_y        = 0; 
  m_push_theta    = 0;
  m_float_rate    = 0;
  m_paused        = false;
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
  double next_speed  = m_thrust / 20.0;

  m_vstate.m_dfSpeed = next_speed;

  // Convert rudder_angle to RADIANS
  double radian_rudder_angle = m_rudder * M_PI / 180.0;
  
  // Check saftey limits
#if 0
  if(m_rudder_angle < 0)
    m_rudder_angle = -(min(fabs(m_rudder), m_max_rotat_vel));
  else
    m_rudder_angle =  (min(fabs(m_rudder), m_max_rotat_vel));
#endif
  
  m_sim_engine.setParam("push_x", m_push_x);
  m_sim_engine.setParam("push_y", m_push_y);
  m_sim_engine.setParam("push_theta", m_push_theta);
  m_sim_engine.setParam("float_rate", m_float_rate);
  m_sim_engine.propagate(m_vstate, m_vstate.m_dfSpeed, radian_rudder_angle, 
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

  return(heading);
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

  m_paused   = false;
  m_push_x   = 0;
  m_push_y   = 0;
  m_rudder   = 0;
  m_thrust   = 0;
  m_elevator = 0;
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
//      Note: 

void IMS_Model::setPosition(string str)
{
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
// Procedure: handleCommand
//      Note: 

void IMS_Model::handleCommand(char key)
{
  switch (key) {
  case 'a':
    m_push_y += 0.1;
    break;
  case 'z':
    m_push_y -= 0.1;
    break;
  case 'n':
    m_push_x -= 0.1;
    break;
  case 'm':
    m_push_x += 0.1;
    break;
  case 'p':
    setPaused(!m_paused);
    break;
  case ' ':
    m_push_x = m_push_x / 1.5;
    m_push_y = m_push_y / 1.5;

    if(fabs(m_push_x) < 0.05)
      m_push_x = 0;
    if(fabs(m_push_y) < 0.05)
      m_push_y = 0;
    break;
  default:
    break;
  }
  cout << "force_x: " << m_push_x << " force_y: " << m_push_y << endl;
}


