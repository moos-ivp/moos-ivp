/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYVector.cpp                                         */
/*    DATE: October 17th, 2010                                   */
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

#include <cstdlib>
#include <cstring>
#include <cmath>
#include "XYVector.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

XYVector::XYVector()
{
  XYObject::clear();
  clear();
}

//---------------------------------------------------------------
// Constructor

XYVector::XYVector(double x, double y, double mag, double ang)
{
  clear();

  m_x    = x;
  m_y    = y;
  m_mag  = mag;
  m_ang  = ang;

  double rads = headingToRadians(ang);
  m_xdot = cos(rads) * mag;
  m_ydot = sin(rads) * mag;

  m_valid = true;
}

//---------------------------------------------------------------
// Procedure: clear

void XYVector::clear()
{
  m_x     = 0;
  m_y     = 0;
  m_mag   = 0;
  m_ang   = 0;
  m_xdot  = 0;
  m_ydot  = 0;
  m_valid = false;

  // Drawing hint specific to vectors. A -1 value indicates that
  //   the size of the head should defer to the settings of whatever
  //   application is rendering it. A non-negative value usually is
  //   interpreted as a request to override the app's default value.
  // The size is typically interpreted as meters from the point of 
  //   the vector tip back to the side points rendering the arrow.
  m_head_size = -1;
}

//---------------------------------------------------------------
// Procedure: applySnap

void XYVector::applySnap(double snapval)
{
  m_x = snapToStep(m_x, snapval);
  m_y = snapToStep(m_y, snapval);
}

//---------------------------------------------------------------
// Procedure: augMagnitude

void XYVector::augMagnitude(double amt)
{
  m_mag += amt;

  // Re-sync the other representation scheme.
  double rads = headingToRadians(m_ang);
  m_xdot = cos(rads) * m_mag;
  m_ydot = sin(rads) * m_mag;
}

//---------------------------------------------------------------
// Procedure: augAngle

void XYVector::augAngle(double amt)
{
  m_ang = angle360(m_ang + amt);

  // Re-sync the other representation scheme.
  double rads = headingToRadians(m_ang);
  m_xdot = cos(rads) * m_mag;
  m_ydot = sin(rads) * m_mag;
}

//---------------------------------------------------------------
// Procedure: setPosition
//      Note: The vector is considered valid once the x,y position
//            is set.

void XYVector::setPosition(double x, double y)
{
  m_x = x;
  m_y = y;

  m_valid = true;
}

//---------------------------------------------------------------
// Procedure: setVectorXY

void XYVector::setVectorXY(double xdot, double ydot)
{
  m_xdot = xdot;
  m_ydot = ydot;
  
  // Sync the other representation scheme.
  m_mag = hypot(xdot, ydot);
  m_ang = relAng(0, 0, xdot, ydot);

  m_valid = true;
}


//---------------------------------------------------------------
// Procedure: setVectorMA

void XYVector::setVectorMA(double mag, double ang)
{
  m_mag  = mag;
  m_ang  = ang;

  // Sync the other representation scheme.
  double rads = headingToRadians(ang);
  m_xdot = cos(rads) * mag;
  m_ydot = sin(rads) * mag;
}

//---------------------------------------------------------------
// Procedure: mergeVectorXY

void XYVector::mergeVectorXY(double xdot, double ydot)
{
  m_xdot += xdot;
  m_ydot += ydot;
  
  // Sync the other representation scheme.
  m_mag = hypot(m_xdot, m_ydot);
  m_ang = relAng(0, 0, m_xdot, m_ydot);
}

//---------------------------------------------------------------
// Procedure: mergeVectorMA

void XYVector::mergeVectorMA(double mag, double ang)
{
  // Convert new components to the other representation scheme.
  double rads = headingToRadians(ang);
  double new_xdot = cos(rads) * mag;
  double new_ydot = sin(rads) * mag;

  // Augment the vector
  m_xdot += new_xdot;
  m_ydot += new_ydot;
  
  // Sync with the other representation scheme.
  m_mag = hypot(m_xdot, m_ydot);
  m_ang = relAng(0, 0, m_xdot, m_ydot);
}


//---------------------------------------------------------------
// Procedure: get_spec
//   Purpose: 

string XYVector::get_spec(string param) const
{
  string spec;
  
  spec += "x="   + doubleToStringX(m_x)   + ",";
  spec += "y="   + doubleToStringX(m_y)   + ",";
  spec += "ang=" + doubleToStringX(m_ang) + ",";
  spec += "mag=" + doubleToStringX(m_mag);

  if(m_head_size >= 0) {
    spec += ",head_size=";
    spec += doubleToStringX(m_head_size);
  }

  string obj_spec = XYObject::get_spec(param);
  if(obj_spec != "")
    spec += ("," + obj_spec);
  
  return(spec);
}










