/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VehicleSet.cpp                                       */
/*    DATE: Aug 12th 2008                                        */
/*          (Broken out from the Viewer class(es)                */
/*****************************************************************/

#include <iostream>
#include "VehicleSet.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

//-------------------------------------------------------------
// Constructor
// Note: The setParam routines are used rather than initializing 
//       the variables directly so the automatic recording in the 
//       m_report_map is performed for reporting on the state of 
//       variables in later queries.

VehicleSet::VehicleSet()
{
  m_history_size = 1000;
  m_curr_time    = 0;

  setParam("vehicle_shape_scale", 0.1);
  setParam("trails_point_size", 1);
  setParam("trails_length", 100);
  setParam("trails_gap", 1);

  setParam("trails_viewable", "true");
  setParam("trails_connect_viewable", "false");
  setParam("vehicles_viewable", "true");
  setParam("vehicle_names_viewable", "false");

  setParam("trails_color", "white");
  setParam("active_vehicle_color", "red");
  setParam("inactive_vehicle_color", "yellow");
  setParam("vehicle_names_color", "white");
}


//-------------------------------------------------------------
// Procedure: setParam
//     Ntoes: The "handled" variable is set to true if a known
//            and acceptable value are passed. This boolean is 
//            returned and may be vital to the caller to either
//            pass a warning or error to the user, or perhaps
//            allow the parameter and value to be processed in 
//            some other way.
//            The "makenote" parameter indicates that the param
//            value pair should be noted in the m_param_report
//            mapping for later reporting of all param-value 
//            pairs used for reconstructing the current state.

bool VehicleSet::setParam(string param, string value)
{
  bool handled  = false;
  bool makenote = true;
  param = tolower(param);
  if((param == "ais_report") || (param == "ais_report_local")) {
    handled  = updateVehiclePosition(value);
    makenote = false;
  }
  else if((param == "active_vehicle_color") && isColor(value)) {
    m_active_vehicle_color = colorParse(value);
    handled = true;
  }
  else if((param == "inactive_vehicle_color") && isColor(value)) {
    m_inactive_vehicle_color = colorParse(value);
    handled = true;
  }
  else if((param == "vehicle_names_color") && isColor(value)) {
    m_vehicle_name_color = colorParse(value);
    handled = true;
  }
  else if((param=="trails_color") && isColor(value)) {
    m_trails_color = colorParse(value);
    handled = true;
  }
  else if(param == "clear_vehicle_trails") {
    m_hist_map.clear();
    handled  = true;
    makenote = false;
  }
  else if(param == "vehicles_viewable")
    handled = setBooleanOnString(m_vehicles_viewable, value);
  else if(param == "vehicle_names_viewable")
    handled = setBooleanOnString(m_vehicle_names_viewable, value);
  else if(param == "trails_viewable")
    handled = setBooleanOnString(m_trails_viewable, value);
  else if(param == "trails_connect_viewable")
    handled = setBooleanOnString(m_trails_connect_viewable, value);
  else if(param == "trails_point_size") {
    makenote = false;
    if(value == "bigger")
      handled = setParam("trails_point_size", m_trails_point_size*1.25);
    else if(value == "smaller")
      handled = setParam("trails_point_size", m_trails_point_size*0.80);
  }
  else if(param == "trails_length") {
    makenote = false;
    if(value == "longer")
      handled = setParam("trails_length", m_trails_length*1.25);
    else if(value == "shorter")
      handled = setParam("trails_length", m_trails_length*0.80);
  }
  else if(param == "vehicle_shape_scale") {
    makenote = false;
    if(value == "bigger")
      handled = setParam("vehicle_shape_scale", m_vehicle_shape_scale*1.25);
    else if(value == "smaller")
      handled = setParam("vehicle_shape_scale", m_vehicle_shape_scale*0.80);
  }
  else if(param == "active_vehicle_name") {
    handled = true;
    map<string,ObjectPose>::iterator p = m_pos_map.find(value);
    if(p == m_pos_map.end())
      handled = false;
    else
      m_active_vehicle_name = value;
  }
  else if(param == "cycle_active") {
    handled = true;
    makenote = false;
    map<string, ObjectPose>::iterator p;
    for(p=m_pos_map.begin(); p!=m_pos_map.end(); p++) {
      string vname = p->first;
      if(vname == m_active_vehicle_name) {
	p++;
	if(p == m_pos_map.end())
	  p = m_pos_map.begin();
	m_active_vehicle_name = p->first;
      }
    }
  }
  else if(param == "vehicolor") {
    string vname  = stripBlankEnds(biteString(value, ','));
    string vcolor = stripBlankEnds(value);
    if(isColor(vcolor)) {
      m_vehi_color[vname] = colorParse(vcolor);
      handled = true;
    }
  }

  if(handled && makenote)
    m_param_report[param] = value;

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
//            The "makenote" parameter indicates that the param
//            value pair should be noted in the m_param_report
//            mapping for later reporting of all param-value 
//            pairs used for reconstructing the current state.

bool VehicleSet::setParam(string param, double value)
{
  bool handled  = false;
  bool makenote = true;
  if(param == "history_size") {
    if(value > 0) {
      m_history_size = (int)(value);
      handled = true;
    }
  }
  else if(param == "curr_time") {
    makenote = false;
    if(value >= m_curr_time) {
      m_curr_time = value;
      handled = true;
    }
  }
  else if(param == "trails_point_size") {
    if(value >= 0) {
      m_trails_point_size = value;
      handled = true;
    }
  }
  else if(param == "trails_length") {
    if(value >= 0) {
      m_trails_length = value;
      handled = true;
    }
  }
  else if(param == "vehicle_shape_scale") {
    if(value >= 0) {
      m_vehicle_shape_scale = value;
      handled = true;
    }
  }

  if(handled && makenote)
    m_param_report[param] = dstringCompact(doubleToString(value));

  return(handled);
}


// ----------------------------------------------------------
// Procedure: getDoubleInfo
//   Purpose: Return the double info associated with the given
//            vehicle name and info_type. The result is placed
//            in the given double reference, and a boolean is 
//            returned indicating whether the vname and info_type
//            were matched.

bool VehicleSet::getDoubleInfo(const string& g_vname, 
			       const string& info_type, 
			       double& result) const
{
  string vname = g_vname;  
  if(vname == "active")
    vname = m_active_vehicle_name;

  map<string,ObjectPose>::const_iterator p;
  p = m_pos_map.find(vname);
  if(p == m_pos_map.end())
    return(false);
  ObjectPose opose = p->second;

  if(info_type == "age_ais") {
    map<string,double>::const_iterator p1;
    p1 = m_ais_map.find(vname);
    if(p1 == m_ais_map.end())
      return(false);
    result = m_curr_time - p1->second;
  }
  else if((info_type == "heading") || (info_type == "course"))
    result = opose.getTheta();
  else if((info_type == "xpos") || (info_type == "meters_x"))
    result = opose.getX();
  else if((info_type == "ypos") || (info_type == "meters_y"))
    result = opose.getY();
  else if(info_type == "speed")
    result = opose.getSpeed();
  else if(info_type == "lat")
    result = opose.getLat();
  else if(info_type == "lon")
    result = opose.getLon();
  else if(info_type == "depth")
    result = opose.getDepth();
  else if(info_type == "curr_time")
    result = m_curr_time;
  else if(info_type == "trails_point_size")
    result = m_trails_point_size;
  else if(info_type == "trails_length")
    result = m_trails_length;
  else if(info_type == "trails_gap")
    result = m_trails_gap;
  else if(info_type == "vehicle_shape_scale")
    result = m_vehicle_shape_scale;
  else
    return(false);
  return(true);
}

// ----------------------------------------------------------
// Procedure: getStringInfo
//   Purpose: Return the string info associated with the given
//            vehicle name and info_type. The result is placed
//            in the given string reference, and a boolean is 
//            returned indicating whether the vname and info_type
//            were matched.

bool VehicleSet::getStringInfo(const string& g_vname, 
			       const string& info_type, 
			       string& result) const
{
  string vname = g_vname;
  if(g_vname == "active")
    vname = m_active_vehicle_name;

  if(info_type == "active_vehicle_name")
    result = m_active_vehicle_name; 
  else if((info_type == "body") || (info_type == "type")) {
    map<string,string>::const_iterator p;
    p = m_vbody_map.find(vname);
    if(p != m_vbody_map.end()) 
      result = p->second;
    else
      result = "unknown-type";
  }
  else  
    return(false);

  return(true);
}

// ----------------------------------------------------------
// Procedure: getStringInfo
//   Purpose: Return the string info associated with the given
//            vehicle name and info_type. This is a convenience
//            function when the caller does not care to check
//            whether the vname and info_type were matched.

string VehicleSet::getStringInfo(const string& vname, 
				 const string& info_type) const
{
  string str;
  bool ok = getStringInfo(vname, info_type, str);
  if(ok)
    return(str);
  else
    return("");
}

// ----------------------------------------------------------
// Procedure: getDoubleInfo
//   Purpose: Return the numerical (double) info associated with
//            the given vehicle name and info_type. This is a 
//            convenience function when the caller does not care
//            to check whether the vname and info_type were matched.

double VehicleSet::getDoubleInfo(const string& vname, 
				 const string& info_type) const
{
  double val;
  bool ok = getDoubleInfo(vname, info_type, val);
  if(ok)
    return(val);
  else
    return(0);
}

// ----------------------------------------------------------
// Procedure: getStringInfo
//   Purpose: Return the string info associated with the "active"
//            vehicle and info_type. This is a convenience
//            function when the caller does not care to check
//            whether the vname and info_type were matched.

string VehicleSet::getStringInfo(const string& info_type) const
{
  string str;
  bool ok = getStringInfo("active", info_type, str);
  if(ok)
    return(str);
  else
    return("");
}

// ----------------------------------------------------------
// Procedure: getDoubleInfo
//   Purpose: Return the numerical (double) info associated with
//            the "active" vehicle and info_type. This is a 
//            convenience function when the caller does not care
//            to check whether the vname and info_type were matched.

double VehicleSet::getDoubleInfo(const string& info_type) const
{
  double val;
  bool ok = getDoubleInfo("active", info_type, val);
  if(ok)
    return(val);
  else
    return(0);
}

// ----------------------------------------------------------
// Procedure: getColor
//   Purpose: Return the color of certain attributes given by key.
//            If the key is not matched, it is assumed to be referring
//            to color of a particular vehicle, with the key being
//            the name of the vehicle. In this case the color of the
//            vehicle is returned (if it is in the m_vehi_color map)
//            or else the "inactive_vehicle_color" is returned.

vector<double> VehicleSet::getColor(const string& key) const
{
  vector<double> cvect = m_inactive_vehicle_color;
  if(key == "trails_color")
    return(m_trails_color);
  else if(key == "active_vehicle_color")
    return(m_active_vehicle_color);
  else if(key == "inactive_vehicle_color")
    return(m_inactive_vehicle_color);
  else if(key == "vehicle_name_color")
    return(m_vehicle_name_color);
  else {
    map<string, vector<double> >::const_iterator p;
    p = m_vehi_color.find(key);
    if(p == m_vehi_color.end())
      return(cvect);
    else
      return(p->second);
  }
}
    
// ----------------------------------------------------------
// Procedure: hasVehiName
//   Purpose: 

bool VehicleSet::hasVehiName(const string& vname) const
{  
  int vsize = m_pos_map.size();
  map<string, ObjectPose>::const_iterator p;
  for(p=m_pos_map.begin(); p != m_pos_map.end(); p++)
    if(p->first == vname)
      return(true);
  
  return(false);
}

// ----------------------------------------------------------
// Procedure: isViewable
//   Purpose: 

bool VehicleSet::isViewable(const string& feature) const
{  
  if(feature == "vehicles")
    return(m_vehicles_viewable);
  else if(feature == "vehicle_names")
    return(m_vehicle_names_viewable);
  else if(feature == "trails")
    return(m_trails_viewable);
  else if(feature == "trails_connect")
    return(m_trails_connect_viewable);
  else
    return(false);
}

//-------------------------------------------------------------
// Procedure: getVehiNames()

vector<string> VehicleSet::getVehiNames() const
{
  vector<string> rvect;

  map<string,ObjectPose>::const_iterator p;
  for(p=m_pos_map.begin(); p!=m_pos_map.end(); p++)
    rvect.push_back(p->first);

  return(rvect);
}

//-------------------------------------------------------------
// Procedure: getParamReport
//   Purpose: Return a set of strings where each is of the type:
//            "parameter = value".
//            The set returned should be enough to bring future
//            instantiations back to the same current state 
//            should the vector param-value pairs be re-applied 
//            using the setParam interface. This allows a user
//            to "save his/her preferences" in a file or some
//            other configuration block.

vector<string> VehicleSet::getParamReport() const
{
  vector<string> svect;

  svect.push_back("// Parameters for Vehicle Attributes");

  map<string, string>::const_iterator p;
  for(p=m_param_report.begin(); p!= m_param_report.end(); p++) {
    string param = p->first;
    string value = p->second;
    svect.push_back(param + " = " + value);
  }

  return(svect);
}

//-------------------------------------------------------------
// Procedure: getObjectPose

ObjectPose VehicleSet::getObjectPose(const string& given_vname) const
{
  string vname = given_vname;
  if(vname == "active")
    vname = m_active_vehicle_name;

  map<string, ObjectPose>::const_iterator p;
  p = m_pos_map.find(vname);
  if(p != m_pos_map.end())
    return(p->second);
  else {
    ObjectPose null_opose;
    return(null_opose);
  }
}

//-------------------------------------------------------------
// Procedure: getVehiHist

CPList VehicleSet::getVehiHist(const string& given_vname) const
{
  string vname = given_vname;
  if(vname == "active")
    vname = m_active_vehicle_name;

  map<string, CPList>::const_iterator p;
  p = m_hist_map.find(vname);
  if(p != m_hist_map.end())
    return(p->second);
  else {
    CPList null_cplist;
    return(null_cplist);
  }
}

//-------------------------------------------------------------
// Procedure: getWeightedCenter()

bool VehicleSet::getWeightedCenter(double& x, double& y) const
{
  double total_x = 0;
  double total_y = 0;

  int msize = m_pos_map.size();
  if(msize == 0) {
    x =0; y=0;
    return(false);
  }
  
  map<string,ObjectPose>::const_iterator p;
  for(p=m_pos_map.begin(); p!=m_pos_map.end(); p++) {
    ObjectPose opose = p->second;
    total_x += opose.getX();
    total_y += opose.getY();
  }


  x = total_x / (double)(msize);
  y = total_y / (double)(msize);

  return(true);
}

//-------------------------------------------------------------
// Procedure: updateVehiclePosition
//      Note: NAME, TYPE, MOOSDB_TIME, UTC_TIME, X, Y, 
//            LAT, LON, SPD, HDG, DEPTH

bool VehicleSet::updateVehiclePosition(const string& ais_report) 
{
  double pos_x = 0;
  double pos_y = 0; 
  double speed = 0;
  double hding = 0;
  double depth = 0;
  double utime = 0;
  double lat   = 0;
  double lon   = 0;
  string vname = "";
  string vtype = "";
  bool b_vname = tokParse(ais_report, "NAME",  ',', '=', vname);
  bool b_vtype = tokParse(ais_report, "TYPE",  ',', '=', vtype);
  bool b_pos_x = tokParse(ais_report, "X",     ',', '=', pos_x);
  bool b_pos_y = tokParse(ais_report, "Y",     ',', '=', pos_y);
  bool b_speed = tokParse(ais_report, "SPD",   ',', '=', speed);
  bool b_hding = tokParse(ais_report, "HDG",   ',', '=', hding);
  bool b_depth = tokParse(ais_report, "DEPTH", ',', '=', depth);
  bool b_utime = tokParse(ais_report, "UTC_TIME", ',', '=', utime);
  bool b_lat   = tokParse(ais_report, "LAT", ',', '=', lat);
  bool b_lon   = tokParse(ais_report, "LON", ',', '=', lon);

  if((!b_pos_x || !b_pos_y) && (!b_lat || !b_lon))
    return(false);

  if(!b_vname || !b_vtype || !b_speed ||!b_hding || !b_utime)
    return(false);

  if(((vtype == "auv") || (vtype == "glider")) && !b_depth)
    return(false);
  
  
  // If there is no active vehicle declared - make the active vehicle
  // the first one that the VehicleSet knows about.
  if(m_active_vehicle_name == "")
    m_active_vehicle_name = vname;
  
  // Handle updating the ObjectPose with the new information
  ObjectPose opose(pos_x, pos_y, hding, speed, depth);

  // If the lat/lon was included in the report, add to the object pose
  if(b_lat && b_lon)
    opose.setLatLon(lat, lon);
  
  m_pos_map[vname] = opose;
  m_ais_map[vname] = utime;
 
  ColoredPoint point(pos_x, pos_y);
  map<string,CPList>::iterator p2;
  p2 = m_hist_map.find(vname);
  if(p2 != m_hist_map.end()) {
    p2->second.push_back(point);
    if(p2->second.size() > m_history_size)
      p2->second.pop_front();
  }
  else {
    list<ColoredPoint> newlist;
    newlist.push_back(point);
    m_hist_map[vname] = newlist;
  }

  m_vbody_map[vname] = tolower(stripBlankEnds(vtype));
  return(true);
}

