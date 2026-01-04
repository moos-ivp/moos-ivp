/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYVessel.cpp                                         */
/*    DATE: Nov 7th 2025                                         */
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
#include <cstdlib>
#include "XYVessel.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------
// Constructor()

XYVessel::XYVessel(double x, double y, double hdg, double len)
{
  m_x    = x;
  m_y    = y;
  m_len  = len;
  m_hdg  = hdg;
  m_type = "ship";
}

//-------------------------------------------------------------
// Procedure: setXY()

void XYVessel::setXY(double x, double y)
{
  m_x  = x;
  m_y  = y;
}

//-------------------------------------------------------------
// Procedure: setLen()

void XYVessel::setLen(double len)
{
  if(len <= 0)
    return;

  m_len = len;
}

//-------------------------------------------------------------
// Procedure: setHdg()

void XYVessel::setHdg(double hdg)
{
  m_hdg = hdg;
}


//-------------------------------------------------------------
// Procedure: get_spec()

string XYVessel::get_spec(string param) const
{
  string spec;
  
  spec += "x=";
  spec += doubleToStringX(m_x,2) + ","; 
  spec += "y=";
  spec += doubleToStringX(m_y,2) + ","; 
  spec += "hdg=";
  spec += doubleToStringX(m_hdg,2) + ","; 
  spec += "len=";
  spec += doubleToStringX(m_len,2) + ","; 
  spec += "type=";
  spec += m_type + ","; 

  string obj_spec = XYObject::get_spec(param);
  if(obj_spec != "")
    spec += ("," + obj_spec);
  
  return(spec);
}

//-------------------------------------------------------------
// Procedure: stringToVessel()

XYVessel stringToVessel(string str)
{
  XYVessel vessel;

  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    double dval = atof(value.c_str());
    
    if(param == "x")
      vessel.setX(dval);
    else if(param == "y")
      vessel.setY(dval);
    else if((param == "hdg") && isNumber(value))
      vessel.setHdg(dval);
    else if((param == "len") && isNumber(value))
      vessel.setLen(dval);
    else if((param == "type") && isKnownVehicleType(value))
      vessel.set_type(value);
    else if((param == "color") && isColor(value))
      vessel.set_color("fill", value);
    else
      vessel.set_param(param, value);
  }

  return(vessel);
}

