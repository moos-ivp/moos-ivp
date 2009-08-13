/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSAE Newport RI and MIT Cambridge MA               */
/*    FILE: VehicleSet.cpp                                       */
/*    DATE: Aug 12th 2008                                        */
/*          (Broken out from the Viewer class(es)                */
/*****************************************************************/

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
  m_curr_time    = 0;
  m_history_size = 1000;

  m_node_report_vars.push_back("AIS_REPORT");
  m_node_report_vars.push_back("AIS_REPORT_LOCAL");
  m_node_report_vars.push_back("NODE_REPORT");
  m_node_report_vars.push_back("NODE_REPORT_LOCAL");
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
  param = tolower(param);
  value = stripBlankEnds(value);

  bool vehicle_report = false;
  unsigned int i, vsize = m_node_report_vars.size();
  for(i=0; i<vsize; i++) {
    if(param == tolower(m_node_report_vars[i]))
      vehicle_report = true;
  }

  if(vehicle_report)
    handled  = updateVehiclePosition(value);
  else if(param == "node_report_variable") {
    if(!strContainsWhite(value))
      m_node_report_vars.push_back(value);
  }
  else if(param == "clear_vehicle_trails") {
    m_hist_map.clear();
    handled  = true;
  }
  else if((param == "active_vehicle_name") || (param == "vehicles_active_name")) {
    handled = true;
    map<string,ObjectPose>::iterator p = m_pos_map.find(value);
    if(p == m_pos_map.end())
      handled = false;
    else
      m_vehicles_active_name = value;
  }
  else if((param == "center_vehicle_name") || (param == "vehicles_center_name")) {
    m_vehicles_center_name = value;
  }
  else if(param == "cycle_active") {
    handled = true;
    map<string, ObjectPose>::iterator p;
    for(p=m_pos_map.begin(); p!=m_pos_map.end(); p++) {
      string vname = p->first;
      if(vname == m_vehicles_active_name) {
	p++;
	if(p == m_pos_map.end())
	  p = m_pos_map.begin();
	m_vehicles_active_name = p->first;
      }
    }
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
//            The "makenote" parameter indicates that the param
//            value pair should be noted in the m_param_report
//            mapping for later reporting of all param-value 
//            pairs used for reconstructing the current state.

bool VehicleSet::setParam(string param, double value)
{
  bool handled = false;
  if(param == "curr_time") {
    if(value >= m_curr_time) {
      m_curr_time = value;
      handled = true;
    }
  }
  
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
    vname = m_vehicles_active_name;
  else if(vname == "center_vehicle")
    vname = m_vehicles_center_name;

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
  else if(info_type == "vlength") {
    map<string,double>::const_iterator p1;
    p1 = m_vlen_map.find(vname);
    if(p1 == m_vlen_map.end())
      return(false);
    result = p1->second;
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
    vname = m_vehicles_active_name;

  if(info_type == "active_vehicle_name")
    result = m_vehicles_active_name; 
  else if((info_type == "body") || (info_type == "type")) {
    map<string,string>::const_iterator p;
    p = m_vbody_map.find(vname);
    if(p != m_vbody_map.end()) 
      result = p->second;
    else
      result = "unknown-type";
  }
  else if(info_type == "helm_mode") {
    map<string,string>::const_iterator p;
    p = m_vmode_map.find(vname);
    if(p != m_vmode_map.end()) 
      result = p->second;
    else
      result = "unknown-mode";
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
// Procedure: getObjectPose

ObjectPose VehicleSet::getObjectPose(const string& given_vname) const
{
  string vname = given_vname;
  if(vname == "active")
    vname = m_vehicles_active_name;

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
    vname = m_vehicles_active_name;

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

bool VehicleSet::updateVehiclePosition(const string& node_report) 
{
  double pos_x = 0;
  double pos_y = 0; 
  double speed = 0;
  double hding = 0;
  double depth = 0;
  double utime = 0;
  double lat   = 0;
  double lon   = 0;
  double vlen  = 0;
  string vname = "";
  string vtype = "";
  string vmode = "";
  bool b_vname = tokParse(node_report, "NAME",  ',', '=', vname);
  bool b_vtype = tokParse(node_report, "TYPE",  ',', '=', vtype);
  bool b_pos_x = tokParse(node_report, "X",     ',', '=', pos_x);
  bool b_pos_y = tokParse(node_report, "Y",     ',', '=', pos_y);
  bool b_speed = tokParse(node_report, "SPD",   ',', '=', speed);
  bool b_hding = tokParse(node_report, "HDG",   ',', '=', hding);
  bool b_depth = tokParse(node_report, "DEPTH", ',', '=', depth);
  bool b_utime = tokParse(node_report, "UTC_TIME", ',', '=', utime);
  bool b_mtime = tokParse(node_report, "MOOS_TIME", ',', '=', utime);
  bool b_lat   = tokParse(node_report, "LAT", ',', '=', lat);
  bool b_lon   = tokParse(node_report, "LON", ',', '=', lon);
  bool b_vlen  = tokParse(node_report, "LENGTH", ',', '=', vlen);
  bool b_vmode = tokParse(node_report, "MODE", ',', '=', vmode);

  vtype = tolower(vtype);

  // Do some "Type-Fixing"
  if(vtype == "uuv")
    vtype = "auv";
  if((vtype == "slocum") || (vtype == "seaglider") || (vtype == "ant"))
    vtype = "glider";
  if((vtype!="auv")&&(vtype!="ship")&&(vtype!="glider")&&(vtype!="kayak"))
    vtype = "ship";

  if((!b_pos_x || !b_pos_y) && (!b_lat || !b_lon))
    return(false);

  if(!b_vname || !b_vtype || !b_speed ||!b_hding)
    return(false);

  if(!b_mtime && !b_utime)
    return(false);

  if(((vtype == "auv") || (vtype == "glider")) && !b_depth)
    return(false);
  
  if(!b_vlen || (vlen==0)) {
    if((vtype=="auv") || (vtype=="kayak"))
      vlen = 3.0; // meters
    if(vtype=="glider")
      vlen = 2.0; // meters
    if(vtype=="ship")
      vlen = 10; // meters
  }

  // If there is no active vehicle declared - make the active vehicle
  // the first one that the VehicleSet knows about.
  if(m_vehicles_active_name == "")
    m_vehicles_active_name = vname;
  
  // Handle updating the ObjectPose with the new information
  ObjectPose opose(pos_x, pos_y, hding, speed, depth);

  // If the lat/lon was included in the report, add to the object pose
  if(b_lat && b_lon)
    opose.setLatLon(lat, lon);

  m_vlen_map[vname]  = vlen; 
  m_pos_map[vname]   = opose;
  m_ais_map[vname]   = utime;
  if(b_vmode)
    m_vmode_map[vname] = vmode;

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

