/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: DubinsTurng.cpp                                      */
/*    DATE: Nov 11th 2018                                        */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include <cmath> 
#include "DubinsTurn.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

#define MPI 3.14159265359

//----------------------------------------------------------
// Procedure: Constructor

DubinsTurn::DubinsTurn(double osx, double osy, double osh, double rad) 
{
  m_osx = osx;
  m_osy = osy;
  m_osh = osh;
  m_rad = rad;

  m_new_hdg   = 0;

  m_ax        = 0;
  m_ay        = 0;
  m_langle    = 0;
  m_rangle    = 0;

  m_rx = 0;
  m_ry = 0;

  m_arc_length = 0;
  m_turn_type = 0;    // 0 no turn, -1 port turn, +1 starboard
}

//----------------------------------------------------------------
// Procedure: setTurn

void DubinsTurn::setTurn(double new_desired_hdg)
{
  double delta_hdg = angleDiff(m_osh, new_desired_hdg);

  double beam_star = angle360(m_osh + 90);
  double beam_port = angle360(m_osh - 90);

  // If no turn, treat technically as a port turn with an arc
  // of length zero.
  if(delta_hdg == 0) {
    m_langle = beam_star;
    m_rangle = beam_star;
    projectPoint(beam_port, m_rad, m_osx, m_osy, m_ax, m_ay);
    m_rx = m_osx; 
    m_ry = m_osy;
    return;
  }
  
  if((new_desired_hdg < 0) || (new_desired_hdg >= 360))
    new_desired_hdg = angle360(new_desired_hdg);
  m_new_hdg = new_desired_hdg;

  // Part 1: Determine if this is a port or starboard turn
  bool starboard_turn = true;
  if(angleDiff(m_osh-90, new_desired_hdg) <
     angleDiff(m_osh+90, new_desired_hdg))
    starboard_turn = false;

  if(starboard_turn)
    m_turn_type = 1;
  else
    m_turn_type = -1;
  
  // Part 2: Set the arc center and angle back to ownship. Note
  // these will be same for all starboard turns and same for all
  // port turns. 
  
  if(starboard_turn) { 
    projectPoint(beam_star, m_rad, m_osx, m_osy, m_ax, m_ay);
    // calculate angle from arc center back to ownship position
    m_langle = beam_port;
  }  
  else { // Turn to Port
    projectPoint(beam_port, m_rad, m_osx, m_osy, m_ax, m_ay);
    // calculate angle from arc center back to ownship position
    m_rangle = beam_star;    
  }

  // Part 3: Set the remainder of arc values and also ownship's
  // position at the end of the arc, at the beginning of the ray

  // angle from arc center to ownship position at end of turn
  if(starboard_turn) { 
    m_rangle = angle360(beam_port + delta_hdg);
    projectPoint(m_rangle, m_rad, m_ax, m_ay, m_rx, m_ry);
  }
  else {
    m_langle = angle360(beam_star - delta_hdg);
    projectPoint(m_langle, m_rad, m_ax, m_ay, m_rx, m_ry);
  }

  double circumference = MPI * m_rad * 2;
  double angle_diff = m_rangle - m_langle;
  if(m_rangle < m_langle) 
    angle_diff = (360 - m_langle) + m_rangle;
  double pct_circle = (angle_diff) / 360;

  m_arc_length = pct_circle * circumference;
}


