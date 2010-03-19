/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSAE Newport RI and MIT Cambridge MA               */
/*    FILE: VehicleSet.cpp                                       */
/*    DATE: Aug 12th 2008                                        */
/*          (Broken out from the Viewer class(es)                */
/*****************************************************************/

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
  setParam("bearing_lines_viewable", "true");
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
    m_vehicles_active_color = colorParse(value);
    handled = true;
  }
  else if(((param == "inactive_vehicle_color") ||
	   (param == "vehicles_inactive_color")) && isColor(value)) {
    m_vehicles_inactive_color = colorParse(value);
    handled = true;
  }
  else if(((param == "vehicle_names_color") ||
	   (param == "vehicles_name_color")) && isColor(value)) {
    m_vehicles_name_color = colorParse(value);
    handled = true;
  }
  else if((param=="trails_color") && isColor(value)) {
    m_trails_color = colorParse(value);
    handled = true;
  }
  else if(param == "vehicles_viewable")
    handled = setBooleanOnString(m_vehicles_viewable, value);
  else if(param == "bearing_lines_viewable")
    handled = setBooleanOnString(m_bearing_lines_viewable, value);
  else if((param == "vehicle_name_viewable") ||
	  (param == "vehicles_name_viewable")) {
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
  bool handled  = false;
  if((param == "history_size") || (param == "trails_history_size")) {
    if(value > 0) {
      m_history_size = (int)(value);
      handled = true;
    }
    m_history_size = vclip(m_history_size, 0, 10000);
  }
  else if(param == "trails_point_size") {
    if(value >= 0) {
      m_trails_point_size = value;
      handled = true;
    }
    m_trails_point_size = vclip(m_trails_point_size, 0, 100);
  }
  else if(param == "trails_length") {
    if(value >= 0) {
      m_trails_length = value;
      handled = true;
    }
    m_trails_length = vclip(m_trails_length, 0, 10000);
  }
  else if((param == "vehicle_shape_scale") ||
	  (param == "vehicles_shape_scale")) {
    if(value >= 0) {
      m_vehicles_shape_scale = value;
      handled = true;
    }
    m_vehicles_shape_scale = vclip(m_vehicles_shape_scale, 0.1, 100);
  }

  return(handled);
}
