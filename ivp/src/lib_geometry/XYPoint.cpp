/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYPoint.cpp                                          */
/*    DATE: July 17th, 2008                                      */
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
#include <stdlib.h>
#include <string.h>
#include "XYPoint.h"
#include "MBUtils.h"
#include "GeomUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: clear

void XYPoint::clear()
{
  XYObject::clear();

  m_x     = 0; 
  m_y     = 0; 
  m_z     = 0; 
  m_size  = 1;
  m_valid = false;
}

//---------------------------------------------------------------
// Procedure: apply_snap

void XYPoint::apply_snap(double snapval)
{
  m_x = snapToStep(m_x, snapval);
  m_y = snapToStep(m_y, snapval);
  m_z = snapToStep(m_z, snapval);
}

//---------------------------------------------------------------
// Procedure: print

void XYPoint::print() const
{
  cout << "label:" << m_label;
  cout << " type: " << m_type;
  cout << " size: " << m_size;
  if(m_vertex_color.set())
    cout << " vertex_color: " << m_vertex_color.str() << endl;
  cout << "  x=" << m_x << "  y=" << m_y << "  z=" << m_z << endl;
}

//---------------------------------------------------------------
// Procedure: projectPt
//   Purpose: 

void XYPoint::projectPt(const XYPoint& pt, double ang, double dist)
{
  projectPoint(ang, dist, pt.x(), pt.y(), m_x, m_y);
}

//---------------------------------------------------------------
// Procedure: get_spec
//   Purpose: 

string XYPoint::get_spec(string param) const
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
    spec += dstringCompact(doubleToString(m_edge_size,3)) + ":"; 
  }
  
  spec += dstringCompact(doubleToString(m_x, m_sdigits)) + ",";
  spec += dstringCompact(doubleToString(m_y, m_sdigits)) + ",";
  spec += dstringCompact(doubleToString(m_z, m_sdigits));

  return(spec);
}
