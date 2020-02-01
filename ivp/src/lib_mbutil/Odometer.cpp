/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Odometer.cpp                                         */
/*    DATE: Jan 31st, 2020                                       */
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

#include <cmath>
#include "Odometer.h"

using namespace std;

//-----------------------------------------------------------
// Constructor:

Odometer::Odometer()
{
  // Init state variables
  m_curr_x = 0;
  m_curr_y = 0;
  m_prev_x = 0;
  m_prev_y = 0;

  m_nav_x_received = false;
  m_nav_y_received = false;  
  m_total_distance = 0;

  m_paused = false;
}

//-----------------------------------------------------------
// Procedure: setX()

void Odometer::setX(double dval)
{
  m_curr_x = dval;

  // For the first reading, we also set the prev_x to this val
  if(!m_nav_x_received)
    m_prev_x = m_curr_x;

  m_nav_x_received = true;
}

//-----------------------------------------------------------
// Procedure: setY()

void Odometer::setY(double dval)
{
  m_curr_y = dval;

  // For the first reading, we also set the prev_y to this val
  if(!m_nav_y_received)
    m_prev_y = m_curr_y;

  m_nav_y_received = true;
}

//-----------------------------------------------------------
// Procedure: updateDistance()

void Odometer::updateDistance()
{
  if(!m_paused) {
    double xdelta = m_curr_x - m_prev_x;
    double ydelta = m_curr_y - m_prev_y;
    m_total_distance += hypot(xdelta, ydelta);
  }
  
  m_prev_x = m_curr_x;
  m_prev_y = m_curr_y;
}
