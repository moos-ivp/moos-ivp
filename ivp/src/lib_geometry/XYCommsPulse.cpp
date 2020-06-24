/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYCommsPulse.cpp                                     */
/*    DATE: Dec 5th 2011                                         */
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
#include "XYCommsPulse.h"
#include "GeomUtils.h"
#include "ColorPack.h"
#include "AngleUtils.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

XYCommsPulse::XYCommsPulse()
{
  initialize();
}

XYCommsPulse::XYCommsPulse(double sx, double sy, double tx, double ty)
{
  initialize();
  m_sx = sx;
  m_sy = sy;
  m_tx = tx;
  m_ty = ty;

  m_sx_set = true;
  m_sy_set = true;
  m_tx_set = true;
  m_ty_set = true;
}

//-------------------------------------------------------------
// Procedure: initialize

void XYCommsPulse::initialize()
{
  // Superclass member variables
  set_edge_size(0);
  set_color("fill", "green");
  
  // Local member variables
  m_sx    = 0;
  m_sy    = 0;
  m_tx    = 0;
  m_ty    = 0;
  m_beam_width = 10;

  m_sx_set = false;
  m_sy_set = false;
  m_tx_set = false;
  m_ty_set = false;

  m_duration = 3;
  m_fill = 0.35;
}

//-------------------------------------------------------------
// Procedure: get_triangle()

vector<double> XYCommsPulse::get_triangle(double timestamp) const
{
  vector<double> vpts;
  if(!m_sx_set || !m_sy_set || !m_tx_set || !m_ty_set)
    return(vpts);

  double elapsed_time = timestamp - m_time;
  if((elapsed_time < 0) || (elapsed_time > m_duration))
    return(vpts);

  // First point is the source
  vpts.push_back(m_sx);
  vpts.push_back(m_sy);

  // Calculate the 2nd and 3rd points
  double angle = relAng(m_sx, m_sy, m_tx, m_ty);
  double ax, ay, angle_a = angle360(angle + 90);
  projectPoint(angle_a, (m_beam_width/2), m_tx, m_ty, ax, ay);
  vpts.push_back(ax);
  vpts.push_back(ay);

  double bx, by, angle_b = angle360(angle + 270);
  projectPoint(angle_b, (m_beam_width/2), m_tx, m_ty, bx, by);
  vpts.push_back(bx);
  vpts.push_back(by);

  return(vpts);
}

//-------------------------------------------------------------
// Procedure: set_beam_width()

void XYCommsPulse::set_beam_width(double val)
{
  m_beam_width = val;
  if(m_beam_width < 0)
    m_beam_width = 0;
}

//-------------------------------------------------------------
// Procedure: set_fill()

void XYCommsPulse::set_fill(double val)
{
  if(val < 0)
    val = 0;
  if(val > 1)
    val = 1;

  m_fill = val;
}

//-------------------------------------------------------------
// Procedure: set_pulse_type()

void XYCommsPulse::set_pulse_type(string str)
{
  m_pulse_type = str;
}

//-------------------------------------------------------------
// Procedure: get_fill()

double XYCommsPulse::get_fill(double timestamp) const
{
  double elapsed_time = timestamp - m_time;

  if(elapsed_time <= 0)
    return(m_fill);
  if(elapsed_time >= m_duration)
    return(0);

  double pct = 1.0;
  if(m_duration > 0) 
    pct = (1-(elapsed_time / m_duration));
  double rval = pct  * m_fill;
  return(rval);
}


//-------------------------------------------------------------
// Procedure: valid()

bool XYCommsPulse::valid() const
{
  return(m_sx_set && m_sy_set && m_tx_set && m_ty_set);
}

//-------------------------------------------------------------
// Procedure: get_spec()

string XYCommsPulse::get_spec(string param) const
{
  string spec = "sx=";
  spec += doubleToStringX(m_sx); 
  spec += ",sy=";
  spec += doubleToStringX(m_sy);
  spec += ",tx=";
  spec += doubleToStringX(m_tx);
  spec += ",ty=";
  spec += doubleToStringX(m_ty);
  spec += ",beam_width=";
  spec += doubleToStringX(m_beam_width); 
  spec += ",fill=";
  spec += doubleToStringX(m_fill); 

  if(m_pulse_type != "")
    spec += ",ptype=" + m_pulse_type;
  
  string obj_spec = XYObject::get_spec(param);
  if(obj_spec != "")
    spec += ("," + obj_spec);
  
  return(spec);
}










