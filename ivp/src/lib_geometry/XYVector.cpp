/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYVector.cpp                                         */
/*    DATE: October 17th, 2010                                   */
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
  clear();
}

//---------------------------------------------------------------
// Constructor

XYVector::XYVector(double x, double y, double mag, double ang)
{
  m_x    = x;
  m_y    = y;
  m_mag  = mag;
  m_ang  = ang;

  double rads = headingToRadians(ang);
  m_xdot = cos(rads) * mag;
  m_ydot = sin(rads) * mag;
}

//---------------------------------------------------------------
// Procedure: clear

void XYVector::clear()
{
  m_x    = 0;
  m_y    = 0;
  m_mag  = 0;
  m_ang  = 0;
  m_xdot = 0;
  m_ydot = 0;
}


//---------------------------------------------------------------
// Procedure: setVectorXY

void XYVector::setVectorXY(double xdot, double ydot)
{
  m_xdot = xdot;
  m_ydot = ydot;
  
  m_mag = hypot(xdot, ydot);
  m_ang = relAng(0, 0, xdot, ydot);
}


//---------------------------------------------------------------
// Procedure: setVectorMA

void XYVector::setVectorMA(double mag, double ang)
{
  m_mag  = mag;
  m_ang  = ang;

  double rads = headingToRadians(ang);
  m_xdot = cos(rads) * mag;
  m_ydot = sin(rads) * mag;
}


//---------------------------------------------------------------
// Procedure: print

void XYVector::print() const
{
  cout << "label:" << m_label;
  cout << " type: " << m_type;
  cout << "  x=" << m_x << ", y=" << m_y;
  cout << ", mag=" << m_mag << ", ang=" << m_ang << endl;
}


//---------------------------------------------------------------
// Procedure: get_spec
//   Purpose: 

string XYVector::get_spec(string param) const
{
  string spec;
  
  if(param == "") {
    if(m_active == false)
      spec += "active,false:";
  }
  else if(param == "active=true") 
    spec += "active,true:";
  else if(param == "active=false") 
    spec += "active,false:";
    

  if(m_label != "")
    spec += "label," + m_label + ":"; 
  if(m_label_color.set())
    spec += "label_color," + m_label_color.str() + ":"; 
  if(m_type != "")
    spec += "type," + m_type + ":"; 
  if(m_time_set)
    spec += "time," + doubleToString(m_time,2) + ":"; 
  if(m_source != "")
    spec += "source," + m_source + ":"; 
  if(m_vertex_color.set())
    spec += "vertex_color," + m_vertex_color.str() + ":"; 
  if(vertex_size_set()) {
    spec += "vertex_size,";
    spec += dstringCompact(doubleToString(m_vertex_size,3)) + ":"; 
  }
  if(edge_color_set())
    spec += "edge_color," + m_edge_color.str() + ":"; 
  if(edge_size_set()) {
    spec += "edge_size,";
    spec += dstringCompact(doubleToString(m_edge_size)) + ":"; 
  }
  
  spec += "xpos," + dstringCompact(doubleToString(m_x))   + ":";
  spec += "ypos," + dstringCompact(doubleToString(m_y))   + ":";
  spec += "ang,"  + dstringCompact(doubleToString(m_ang)) + ":";
  spec += "mag,"  + dstringCompact(doubleToString(m_mag));

  return(spec);
}
