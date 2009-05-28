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
  m_active = true;
  m_time   = 0;
  
  vector<double> v(3,0);
  m_label_color_v = v;
  m_label_color_s = "";
}


//---------------------------------------------------------------
// Procedure: clear

void XYObject::clear()
{
  m_active = true; 
  m_time   = 0; 
  m_label  = ""; 
  m_type   = "";
  m_source = ""; 
  m_string = ""; 
  m_label_color_v = vector<double>(3,0);
  m_label_color_s = "";
 
}


//---------------------------------------------------------------
// Procedure: set_label_color()

void XYObject::set_label_color(const string& str)
{
  if(isColor(str))
    m_label_color_v = colorParse(str);
  
  m_label_color_s = colorVectorToString(m_label_color_v);
}
