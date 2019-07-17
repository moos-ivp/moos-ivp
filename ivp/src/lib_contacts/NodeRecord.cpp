/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: NodeRecord.cpp                                       */
/*    DATE: Feb 27th 2010                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include "NodeRecord.h"
#include "MBUtils.h"
#include "AngleUtils.h"

using namespace std;

#ifndef M_PI
#define M_PI 3.1415926
#endif

//---------------------------------------------------------
// Constructor

NodeRecord::NodeRecord(string vname, string vtype)
{
  m_x          = 0;
  m_y          = 0;
  m_lat        = 0;
  m_lon        = 0;
  m_speed      = 0;
  m_speed_og   = 0;
  m_heading    = 0;
  m_heading_og = 0;
  m_depth      = 0;
  m_altitude   = 0;
  m_timestamp  = 0;
  m_length     = 0;
  m_name       = vname;
  m_type       = vtype;
 
  m_index      = 0;

  m_x_set   = false;
  m_y_set   = false;
  m_lat_set = false;
  m_lon_set = false;
  m_speed_set      = false;
  m_speed_og_set   = false;
  m_heading_set    = false;
  m_heading_og_set = false;
  m_depth_set      = false;
  m_altitude_set   = false;
  m_length_set     = false;
  m_timestamp_set  = false;
  m_trajectory_set = false;

  m_thrust_mode_reverse = false;
}

//---------------------------------------------------------------
// Procedure: getStringValue

string NodeRecord::getStringValue(string key) const
{
  key = tolower(key);
  if(key == "x")
    return(doubleToStringX(m_x, 2));
  else if(key == "y")
    return(doubleToStringX(m_y, 2));
  else if(key == "lat")
    return(doubleToStringX(m_lat, 8));
  else if(key == "lon")
    return(doubleToStringX(m_lon, 8));
  else if((key == "speed") || (key == "spd"))
    return(doubleToStringX(m_speed, 2));
  else if((key == "speed_og") || (key == "spd_og"))
    return(doubleToStringX(m_speed_og, 2));
  else if((key == "heading") || (key == "hdg"))
    return(doubleToStringX(m_heading, 2));
  else if((key == "heading_og") || (key == "hdg_og"))
    return(doubleToStringX(m_heading_og, 2));
  else if(key == "yaw")
    return(doubleToStringX(m_yaw, 4));
  else if((key == "depth") || (key == "dep"))
    return(doubleToStringX(m_depth, 2));
  else if((key == "altitude") || (key == "alt"))
    return(doubleToStringX(m_altitude, 2));
  else if((key == "length") || (key == "len"))
    return(doubleToStringX(m_length, 2));
  else if((key == "timestamp") || (key == "time") || (key == "utime"))
    return(doubleToStringX(m_timestamp, 2));
  else if((key == "trajectory") || (key == "traj"))
    return(m_trajectory);
  else if(hasProperty(key))
    return(getProperty(key));
  else
    return("");  
}


//------------------------------------------------------------
// Procedure: getElapsedTime()

double NodeRecord::getElapsedTime(double curr_time) const
{
  if(!m_timestamp_set)
    return(-1);
  return(curr_time - m_timestamp);
}


//------------------------------------------------------------
// Procedure: setProperty

void NodeRecord::setProperty(string key, string value)
{
  m_properties[key] = value;
}

//------------------------------------------------------------
// Procedure: getProperty

string NodeRecord::getProperty(string key) const
{
  map<string, string>::const_iterator p = m_properties.find(key);
  if(p != m_properties.end())
    return(p->second);
  
  return("");
}

//------------------------------------------------------------
// Procedure: hasProperty

bool NodeRecord::hasProperty(string key) const
{
  if(m_properties.count(key) == 1)
    return(true);
  return (false);
}


//------------------------------------------------------------
// Procedure: getSpec()

string NodeRecord::getSpec(bool terse) const
{
  string str = "NAME=" + m_name;
  if(m_x_set)
    str += ",X="    + doubleToStringX(m_x,2);
  if(m_y_set)
    str += ",Y="    + doubleToStringX(m_y,2);
  if(m_speed_set)
    str += ",SPD="  + doubleToStringX(m_speed,2);
  if(m_heading_set)
    str += ",HDG="  + doubleToStringX(m_heading,2);
  if(m_depth_set && !terse)
    str += ",DEP="  + doubleToStringX(m_depth,2);

  if(m_lat_set && !terse)
    str += ",LAT=" + doubleToStringX(m_lat, 8);
  if(m_lon_set && !terse)
    str += ",LON=" + doubleToStringX(m_lon, 8);
  if(m_type != "")
    str += ",TYPE=" + m_type;
  if(m_color != "")
    str += ",COLOR=" + m_color;
  if(m_group != "")
    str += ",GROUP=" + m_group;
  if(m_mode != "")
    str += ",MODE=" + m_mode;
  if(m_mode_aux != "")
    str += ",MODE_AUX=" + m_mode_aux;
  if(m_allstop != "")
    str += ",ALLSTOP=" + m_allstop;
  if(m_load_warning != "")
    str += ",LOAD_WARNING=" + m_load_warning;

  if(m_altitude_set)
    str += ",ALTITUDE=" + doubleToStringX(m_altitude,2);

  if(m_speed_og_set)
    str += ",SPD_OG=" + doubleToStringX(m_speed_og,2);

  if(m_heading_og_set)
    str += ",HDG_OG="  + doubleToStringX(m_heading_og,2);

  if(m_index != 0)
    str += ",INDEX="  + intToString(m_index);
  
  if(m_thrust_mode_reverse)
    str += ",THRUST_MODE_REVERSE=true";

  if(m_yaw_set && !terse) 
    str += ",YAW=" + doubleToStringX(headingToRadians(m_heading),7);
    //str += ",YAW="  + doubleToStringX(m_heading,2);
  if(m_timestamp_set)
    str += ",TIME=" + doubleToStringX(m_timestamp,2);
  if(m_length_set)
    str += ",LENGTH=" + doubleToStringX(m_length,2);

  if(m_trajectory_set)
    str += ",TRAJECTORY={" + m_trajectory + "}";

  map<string, string>::const_iterator p;
  for(p=m_properties.begin(); p!=m_properties.end(); p++) {
    str += "," + p->first;
    str += "=" + p->second;
  }
  
  return(str);
}

//---------------------------------------------------------------
// Procedure: getName

string NodeRecord::getName(string default_name) const
{
  if(m_name == "")
    return(default_name);
  return(m_name);
}

//---------------------------------------------------------------
// Procedure: getGroup

string NodeRecord::getGroup(string default_group) const
{
  if(m_group == "")
    return(default_group);
  return(m_group);
}

//---------------------------------------------------------------
// Procedure: getType

string NodeRecord::getType(string default_type) const
{
  if(m_type == "")
    return(default_type);
  return(m_type);
}

//---------------------------------------------------------------
// Procedure: getColor

string NodeRecord::getColor(string default_color) const
{
  if(m_color == "")
    return(default_color);
  return(m_color);
}

//---------------------------------------------------------------
// Procedure: getMode

string NodeRecord::getMode(string default_mode) const
{
  if(m_mode == "")
    return(default_mode);
  return(m_mode);
}

//---------------------------------------------------------------
// Procedure: getModeAux

string NodeRecord::getModeAux(string default_mode_aux) const
{
  if(m_mode_aux == "")
    return(default_mode_aux);
  return(m_mode_aux);
}

//---------------------------------------------------------------
// Procedure: getAllStop

string NodeRecord::getAllStop(string default_allstop) const
{
  if(m_allstop == "")
    return(default_allstop);
  return(m_allstop);
}

//---------------------------------------------------------------
// Procedure: getLoadWarning

string NodeRecord::getLoadWarning(string default_warning) const
{
  if(m_load_warning == "")
    return(default_warning);
  return(m_load_warning);
}

//---------------------------------------------------------------
// Procedure: valid
//      Note: Determines if all the required fields have been set

bool NodeRecord::valid() const
{
  string why;
  return(valid("name,x,y,speed,heading", why));
}

//---------------------------------------------------------------
// Procedure: valid
//      Note: Determines if all the required fields have been set

bool NodeRecord::valid(string check) const
{
  string why;
  return(valid(check, why));
}

//---------------------------------------------------------------
// Procedure: valid
//      Note: Determines if all the required fields have been set

bool NodeRecord::valid(string check, string& why) const
{
  string missing;

  vector<string> svector = parseStringZ(check, ',', "{");
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string field = tolower(stripBlankEnds(svector[i]));
    if((field == "name") && (m_name == "")) 
      missing += "name,";
    if((field == "type") && (m_type == "")) 
      missing += "type,";
    if((field == "mode") && (m_mode == "")) 
      missing += "mode,";
    if((field == "mode_aux") && (m_mode_aux == "")) 
      missing += "mode_aux,";
    if((field == "allstop") && (m_allstop == "")) 
      missing += "allstop,";
    if((field == "load_warning") && (m_load_warning == "")) 
      missing += "load_warning,";
    if((field == "x") && !m_x_set) 
      missing += "x,";
    if((field == "y") && !m_y_set) 
      missing += "y,";
    if((field == "speed") && !m_speed_set) 
      missing += "speed,";
    if((field == "heading") && !m_heading_set) 
      missing += "heading,";
    if((field == "depth") && !m_depth_set) 
      missing += "depth,";
    if((field == "time") && !m_timestamp_set) 
      missing += "time,";
    if((field == "length") && !m_length_set) 
      missing += "length,";
    if((field == "yaw") && !m_yaw_set) 
      missing += "yaw,";
    if((field == "lat") && !m_lat_set) 
      missing += "lat,";
    if((field == "lon") && !m_lon_set) 
      missing += "lon,";
    if((field == "trajectory") && !m_trajectory_set)
      missing += "trajectory,";
  }

  if(missing != "") {
    unsigned int slen = missing.length();
    why = "missing:" + missing.substr(0, slen-1);
    return(false);
  }

  return(true);
}


