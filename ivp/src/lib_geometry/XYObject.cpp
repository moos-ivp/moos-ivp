/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYObject.cpp                                         */
/*    DATE: July 18th, 2008                                      */
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
#include <string.h>
#include "XYObject.h" 

using namespace std;

//---------------------------------------------------------------
// Constructor

XYObject::XYObject()
{
  m_active      = true;
  m_time        = 0;
  m_time_set    = false;
  m_vertex_size = -1;
  m_edge_size   = -1;
}


//---------------------------------------------------------------
// Procedure: clear

void XYObject::clear()
{
  m_active      = true; 
  m_time        = 0; 
  m_time_set    = false; 
  m_label       = ""; 
  m_type        = "";
  m_source      = ""; 
  m_string      = ""; 
  m_vertex_size = -1;
  m_edge_size   = -1;
  m_label_color.clear();
  m_vertex_color.clear();
  m_edge_color.clear();
}


//---------------------------------------------------------------
// Procedure: set_label_color()
// Procedure: set_vertex_color()
// Procedure: set_edge_color()
// Procedure: set_vertex_size()
// Procedure: set_edge_size()

void XYObject::set_label_color(const string& str)
{
  m_label_color.setColor(str);
}
void XYObject::set_vertex_color(const string& str)
{
  m_vertex_color.setColor(str);
}
void XYObject::set_edge_color(const string& str)
{
  m_edge_color.setColor(str);
}
void XYObject::set_vertex_size(double val)
{
  if(val >= 0) m_vertex_size = val;
}
void XYObject::set_edge_size(double val)
{
  if(val >= 0) m_edge_size = val;
}
