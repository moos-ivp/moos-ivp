/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VehicleSet.cpp                                       */
/*    DATE: Aug 12th 2008                                        */
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
#include <iostream>
#include "VPlug_VehiSettings.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

//-------------------------------------------------------------
// Constructor
// Note: The setParam routines are used rather than initializing 
//       the variables directly so the automatic recording in the 
//       m_report_map is performed for reporting on the state of 
//       variables in later queries.

VPlug_VehiSettings::VPlug_VehiSettings()
{
  setParam("trails_point_size", 1);
  setParam("trails_length", 100);
  setParam("trails_color", "white");
  setParam("trails_viewable", "true");
  setParam("trails_connect_viewable", "false");
  setParam("trails_future_viewable", "false");

  setParam("vehicles_active_color", "red");
  setParam("vehicles_inactive_color", "yellow");
  setParam("vehicles_name_color", "white");
  setParam("vehicles_name_viewable", "names+shortmode");
  setParam("vehicles_shape_scale", 1.0);
  setParam("vehicles_viewable", "true");
  setParam("bearing_lines_viewable",  "true");
  setParam("stale_report_thresh", 60);
  setParam("stale_remove_thresh", 180);
}


//-------------------------------------------------------------
// Procedure: setParam
//     Ntoes: The "handled" variable is set to true if a known
//            and acceptable value are passed. This boolean is 
//            returned and may be vital to the caller to either
//            pass a warning or error to the user, or perhaps
//            allow the parameter and value to be processed in 
//            some other way.

bool VPlug_VehiSettings::setParam(string param, string value)
{
  bool handled  = false;
  param = tolower(param);
  value = stripBlankEnds(value);

  if(((param == "active_vehicle_color") ||
      (param == "vehicles_active_color")) && isColor(value)) {
    m_vehicles_active_color      = colorParse(value);
    m_vehicles_active_color_orig = value;
    handled = true;
  }
  else if(((param == "inactive_vehicle_color") ||
	   (param == "vehicles_inactive_color")) && isColor(value)) {
    m_vehicles_inactive_color      = colorParse(value);
    m_vehicles_inactive_color_orig = value;
    handled = true;
  }
  else if(((param == "vehicle_names_color") ||
	   (param == "vehicles_name_color")) && isColor(value)) {
    m_vehicles_name_color      = colorParse(value);
    m_vehicles_name_color_orig = value;
    handled = true;
  }
  else if((param=="trails_color") && isColor(value)) {
    m_trails_color      = colorParse(value);
    m_trails_color_orig = value;
    handled = true;
  }
  else if((param=="stale_report_thresh") && isNumber(value)) 
    handled = setParam(param, atof(value.c_str()));
  else if((param=="stale_remove_thresh") && isNumber(value))
    handled = setParam(param, atof(value.c_str()));

  else if(param == "vehicles_viewable")
    handled = setBooleanOnString(m_vehicles_viewable, value);
  else if(param == "bearing_lines_viewable")
    handled = setBooleanOnString(m_bearing_lines_viewable, value);
  
  // "names", "names+mode", "names+shortmode", "names+depth"
  else if((param == "vehicle_name_viewable")  ||  // deprecated
	  (param == "vehicles_name_viewable") ||  // deprecated
	  (param == "vehicles_name_mode")) {
    handled = true;
    value = tolower(stripBlankEnds(value));
    if((value == "true") || (value == "names"))
      m_vehicles_name_mode = "names";
    else if((value == "false") || (value == "off"))
      m_vehicles_name_mode = "off";
    else if(value == "names+mode")
      m_vehicles_name_mode = value;
    else if(value == "names+shortmode")
      m_vehicles_name_mode = value;
    else if(value == "names+depth")
      m_vehicles_name_mode = value;
    else if(value == "toggle") {
      if(m_vehicles_name_mode == "off")
	m_vehicles_name_mode = "names";
      else if(m_vehicles_name_mode == "names")
	m_vehicles_name_mode = "names+mode";
      else if(m_vehicles_name_mode == "names+mode")
	m_vehicles_name_mode = "names+shortmode";
      else if(m_vehicles_name_mode == "names+shortmode")
	m_vehicles_name_mode = "names+depth";
      else if(m_vehicles_name_mode == "names+depth")
	m_vehicles_name_mode = "names+auxmode";
      else if(m_vehicles_name_mode == "names+auxmode")
	m_vehicles_name_mode = "off";
    }
    else
      handled = false;
  }
  else if(param == "trails_viewable")
    handled = setBooleanOnString(m_trails_viewable, value);
  else if(param == "trails_connect_viewable")
    handled = setBooleanOnString(m_trails_connect_viewable, value);
  else if(param == "trails_future_viewable")
    handled = setBooleanOnString(m_trails_future_viewable, value);
  else if(param == "trails_point_size") {
    if(value == "bigger")
      handled = setParam("trails_point_size", m_trails_point_size*1.25);
    else if(value == "smaller")
      handled = setParam("trails_point_size", m_trails_point_size*0.80);
  }

  else if(param == "trails_length") {
    if(value == "longer")
      handled = setParam("trails_length", m_trails_length*1.25);
    else if(value == "shorter")
      handled = setParam("trails_length", m_trails_length*0.80);
  }
  else if((param == "vehicle_shape_scale") ||
	  (param == "vehicles_shape_scale")) {
    if(value == "bigger")
      handled = setParam(param, m_vehicles_shape_scale*1.25);
    else if(value == "smaller")
      handled = setParam(param, m_vehicles_shape_scale*0.80);
    else if(value == "reset")
      handled = setParam(param, 1.0);
    else if(isNumber(value))
      handled = setParam(param, atof(value.c_str()));
  }

  return(handled);
}


//-------------------------------------------------------------
// Procedure: setParam
//     Ntoes: The "handled" variable is set to true if a known
//            and acceptable value are passed. This boolean is 
//            returned and may be vital to the caller to either
//            pass a warning or error to the user, or perhaps
//            allow the parameter and value to be processed in 
//            some other way.

bool VPlug_VehiSettings::setParam(string param, double value)
{
  // Handle deprecated parameters
  if(param == "vehicle_shape_scale")
    param = "vehicles_shape_scale";

  bool handled = true;
  if((param == "trails_point_size") && (value >= 0)) {
    m_trails_point_size = value;
    m_trails_point_size = vclip(m_trails_point_size, 0, 10);
  }
  else if((param == "trails_length") && (value >= 0)) {
    m_trails_length = value;
    m_trails_length = vclip(m_trails_length, 0, 65000);
  }
  else if((param == "stale_report_thresh") && (value >= 0)) 
    m_stale_report_thresh = vclip_min(value, 2);
  else if((param == "stale_remove_thresh") && (value >= 0)) 
    m_stale_remove_thresh = value;
  else if((param == "vehicles_shape_scale") && (value >= 0)) {
    m_vehicles_shape_scale = value;
    m_vehicles_shape_scale = vclip(m_vehicles_shape_scale, 0.1, 100);
  }
  else
    handled = false;

  return(handled);
}

//-------------------------------------------------------------
// Procedure: strvalue

string VPlug_VehiSettings::strvalue(const string& attribute)
{
  if(attribute == "vehicles_viewable")        
    return(boolToString(m_vehicles_viewable));
  if(attribute == "trails_viewable")        
    return(boolToString(m_trails_viewable));
  if(attribute == "trails_connect_viewable")
    return(boolToString(m_trails_connect_viewable));

  if(attribute == "trails_future_viewable")
    return(boolToString(m_trails_future_viewable));
  if(attribute == "bearing_lines_viewable")
    return(boolToString(m_bearing_lines_viewable));

  if(attribute == "vehicles_name_mode")
    return(m_vehicles_name_mode);
  
  if(attribute == "trails_color") 
    return(m_trails_color_orig);
  if(attribute == "vehicles_active_color") 
    return(m_vehicles_active_color_orig);
  if(attribute == "vehicles_inactive_color") 
    return(m_vehicles_inactive_color_orig);
  if(attribute == "vehicles_name_color") 
    return(m_vehicles_name_color_orig);
  if(attribute == "vehicles_name_mode") 
    return(m_vehicles_name_mode);

  if(attribute == "trails_length")
    return(uintToString(m_trails_length));
  if(attribute == "vehicles_shape_scale")
    return(doubleToString(m_vehicles_shape_scale));
  if(attribute == "trails_point_size")
    return(doubleToString(m_trails_point_size));
  if(attribute == "stale_report_thresh")
    return(doubleToString(m_stale_report_thresh));
  if(attribute == "stale_remove_thresh")
    return(doubleToString(m_stale_remove_thresh));

  return("");
}








