/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: VehicleSet.cpp                                       */
/*    DATE: Aug 12th 2008                                        */
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

#include <iostream>
#include <cstdlib>
#include <cmath>
#include "VehicleSet.h"
#include "MBUtils.h"
#include "ColorParse.h"
#include "NodeRecord.h"
#include "NodeRecordUtils.h"
#include "LinearExtrapolator.h"

using namespace std;

//-------------------------------------------------------------
// Constructor()

VehicleSet::VehicleSet()
{
  m_curr_time    = 0;
  m_history_size = 1000;

  m_xmin = 0;
  m_xmax = 0;
  m_ymin = 0;
  m_ymax = 0;
}

//-------------------------------------------------------------
// Procedure: setParam()
//     Ntoes: The "handled" variable is set to true if a known and 
//            acceptable value are passed. This Boolean is returned 
//            and may be vital to the caller to either pass a warning
//            or error to the user, or perhaps allow the parameter
//            and value to be processed in some other way.

bool VehicleSet::setParam(string param, string value)
{
  bool handled  = false;
  param = tolower(param);
  value = stripBlankEnds(value);

  if(param == "bearing_line")
    handled = updateVehicleBearingLine(value);
  else if((param == "active_vehicle_name") || 
	  (param == "vehicles_active_name")) {
    if(m_rec_map.count(value)) {
      handled = true;
      m_vehicles_active_name = value;
    }
  }
  else if((param == "center_vehicle_name") || 
	  (param == "vehicles_center_name")) {
    m_vehicles_center_name = value;
  }
  else if(param == "cycle_active") {
    handled = true;
    map<string, NodeRecord>::iterator p;
    for(p=m_rec_map.begin(); p!=m_rec_map.end(); p++) {
      string vname = p->first;
      if(vname == m_vehicles_active_name) {
	p++;
	if(p == m_rec_map.end())
	  p = m_rec_map.begin();
	m_vehicles_active_name = p->first;
      }
    }
  }

  return(handled);
}


//-------------------------------------------------------------
// Procedure: setParam()
//     Ntoes: The "handled" variable is set to true if a known
//            and acceptable value are passed. This boolean is 
//            returned and may be vital to the caller to either
//            pass a warning or error to the user, or perhaps
//            allow the parameter and value to be processed in 
//            some other way.

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

//-------------------------------------------------------------
// Procedure: clear()

void VehicleSet::clear(const string& vname)
{
  if((vname == "all") || (vname == "")) {
    m_rec_map.clear();
    m_hist_map.clear();
    m_bearing_map.clear();
  }
  else {
    m_rec_map.erase(vname);
    m_hist_map.erase(vname);
    m_bearing_map.erase(vname);
  }
  m_xmin = 0;
  m_xmax = 0;
  m_ymin = 0;
  m_ymax = 0;
}

// ----------------------------------------------------------
// Procedure: getNodeRecord()

NodeRecord VehicleSet::getNodeRecord(const string& vname) const
{
  map<string, NodeRecord>::const_iterator p;
  p = m_rec_map.find(vname);
  if(p != m_rec_map.end())
    return(p->second);
  
  return(NodeRecord());
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

  NodeRecord record;
  map<string, NodeRecord>::const_iterator p = m_rec_map.find(vname);
  if(p == m_rec_map.end())
    p = m_rec_map.find(tolower(vname));
  if(p == m_rec_map.end())
    p = m_rec_map.find(toupper(vname));

  if(p != m_rec_map.end())
    record = p->second;
  else
    return(false);

  // If x/y position is being queried, extrapolate the position
  if((info_type == "xpos") || (info_type == "meters_x") ||
     (info_type == "ypos") || (info_type == "meters_y")) {
    LinearExtrapolator extrapolator;
    extrapolator.setPosition(record.getX(), record.getY(),
			     record.getSpeed(), record.getHeading(),
			     record.getTimeStamp());

    double new_x, new_y;
    bool ok = extrapolator.getPosition(new_x, new_y, m_curr_time);

    if(ok && ((info_type == "xpos") || (info_type == "meters_x"))) {
      result = new_x;
      return(true);
    }
    if(ok && ((info_type == "ypos") || (info_type == "meters_y"))) {
      result = new_y;
      return(true);
    }
  }
    
  double node_local_time = 0;
  map<string,double>::const_iterator q = m_map_node_local_time.find(vname);
  if(q != m_map_node_local_time.end())
    node_local_time = q->second;

  if(info_type == "age_ais")
    result = m_curr_time - node_local_time;
  else if(info_type == "vlength")
    result = record.getLength();
  else if((info_type == "heading") || (info_type == "course"))
    result = record.getHeading();
  else if((info_type == "xpos") || (info_type == "meters_x"))
    result = record.getX();
  else if((info_type == "ypos") || (info_type == "meters_y"))
    result = record.getY();
  else if(info_type == "speed")
    result = record.getSpeed();
  else if(info_type == "lat") 
    result = record.getLat();
  else if(info_type == "lon")
    result = record.getLon();
  else if(info_type == "depth")
    result = record.getDepth();
  else if(info_type == "curr_time")
    result = m_curr_time;
  else
    return(false);

  return(true);
}

// ----------------------------------------------------------
// Procedure: getStringInfo()
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

  NodeRecord record;
  map<string, NodeRecord>::const_iterator p = m_rec_map.find(vname);
  if(p != m_rec_map.end())
    record = p->second;
  else
    return(false);

  if(info_type == "active_vehicle_name")
    result = m_vehicles_active_name; 
  else if((info_type == "body") || (info_type == "type"))
    result = record.getType("unknown-type");
  else if(info_type == "helm_mode") 
    result = record.getMode("unknown-mode");
  else if(info_type == "helm_mode_aux") 
    result = record.getModeAux();
  else if(info_type == "helm_allstop_mode")
    result = record.getAllStop("unknown-amode");
  else  
    return(false);

  return(true);
}

// ----------------------------------------------------------
// Procedure: getStringInfo()
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
// Procedure: getDoubleInfo()
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
// Procedure: getStringInfo()
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

bool VehicleSet::hasVehiName(const string& vname) const
{  
  return(m_rec_map.count(vname));
}

//-------------------------------------------------------------
// Procedure: getVehiNames()

vector<string> VehicleSet::getVehiNames() const
{
  vector<string> rvect;

  map<string, NodeRecord>::const_iterator p;
  for(p=m_rec_map.begin(); p!=m_rec_map.end(); p++)
    rvect.push_back(p->first);

  return(rvect);
}

//-------------------------------------------------------------
// Procedure: print()

void VehicleSet::print() const
{
  cout << "Vehicle Set Summary: =====================" << endl;
  cout << "Total entries: " << m_rec_map.size() << endl;
  
  unsigned int index = 0;

  map<string, NodeRecord>::const_iterator p;
  for(p=m_rec_map.begin(); p!=m_rec_map.end(); p++) {
    cout << "[" << index << "]" << p->first << endl;
    cout << "Spec:" << p->second.getSpec() << endl;
  }
}

//-------------------------------------------------------------
// Procedure: getBearingLine

BearingLine VehicleSet::getBearingLine(const string& given_vname) const
{
  string vname = given_vname;
  if(vname == "active")
    vname = m_vehicles_active_name;

  BearingLine bearing_line;
  BearingLine invalid_line;
  map<string, BearingLine>::const_iterator p;
  p = m_bearing_map.find(vname);
  if(p != m_bearing_map.end())
    bearing_line = p->second;

  if(!bearing_line.isValid())
    return(invalid_line);

  double time_limit = bearing_line.getTimeLimit();
  if(time_limit == -1)
    return(bearing_line);

  double time_stamp = bearing_line.getTimeStamp();
  if((m_curr_time - time_stamp) > time_limit)
    return(invalid_line);
  else
    return(bearing_line);
}

//-------------------------------------------------------------
// Procedure: getClosestVehicle

string VehicleSet::getClosestVehicle(double mx, double my) const
{
  if(m_rec_map.size() == 0)
    return("");

  double closest_range = -1;
  string closest_vname;
  
  map<string, NodeRecord>::const_iterator p;
  for(p=m_rec_map.begin(); p!=m_rec_map.end(); p++) {
    double vx = p->second.getX();
    double vy = p->second.getY();
    double range = hypot(vx-mx, vy-my);
    if((closest_range == -1) || (range < closest_range)) {
      closest_range = range;
      closest_vname = p->second.getName();
    }      
  }

  return(closest_vname);
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

  int msize = m_rec_map.size();
  if(msize == 0) {
    x =0; y=0;
    return(false);
  }
  
  map<string, NodeRecord>::const_iterator p;
  for(p=m_rec_map.begin(); p!=m_rec_map.end(); p++) {
    total_x += p->second.getX();
    total_y += p->second.getY();
  }

  x = total_x / (double)(msize);
  y = total_y / (double)(msize);

  return(true);
}


//-------------------------------------------------------------
// Procedure: handleNodeReport

bool VehicleSet::handleNodeReport(string node_report_str, string& whynot)
{
  return(handleNodeReport(m_curr_time, node_report_str, whynot));
}


//-------------------------------------------------------------
// Procedure: handleNodeReport

bool VehicleSet::handleNodeReport(double local_time, 
				  string node_report_str, 
				  string& whynot)
{
  NodeRecord new_record = string2NodeRecord(node_report_str);

  if(!new_record.valid("x,y") && !new_record.valid("lat,lon")) {
    whynot = "must have either x,y or lat,lon";
    return(false);
  }
  if(!new_record.valid("name,type,speed,heading,time", whynot))
    return(false);
  
  // Do some "Type-Fixing". Known types: kayak, auv, glider, ship
  string vtype = tolower(new_record.getType());
  if(vtype == "uuv")
    vtype = "auv";
  else if((vtype == "slocum") || (vtype == "seaglider"))
    vtype = "glider";
  else if(!isKnownVehicleType(vtype))
    vtype = "ship";
  
  if(((vtype == "auv") || (vtype == "glider")) && !new_record.valid("depth")) {
    whynot = "underwater vehicle type with no depth reported";
    return(false);
  }

  // If there is no active vehicle declared - make the active vehicle
  // the first one that the VehicleSet knows about.
  string vname = new_record.getName();
  if(m_vehicles_active_name == "")
    m_vehicles_active_name = vname;
  
  double vlen = new_record.getLength();
  if(!new_record.valid("length") || (vlen == 0)) {
    if((vtype=="auv") || (vtype=="kayak") || (vtype=="heron"))
      vlen = 3.0; // meters
    else if(vtype=="glider")
      vlen = 2.0; // meters
    else if(vtype=="ship")
      vlen = 10; // meters
    else if(vtype=="longship")
      vlen = 10; // meters
  }
  
  new_record.setType(vtype);
  new_record.setLength(vlen);
  m_rec_map[vname] = new_record;

  // If local time not provided, reluctantly use the timestamp from the 
  // node report itself.
  if(local_time == 0)
    local_time = new_record.getTimeStamp();

  m_map_node_local_time[vname] = local_time;

  // Handle updating the NodeRecord with the new information
  double pos_x = new_record.getX();
  double pos_y = new_record.getY();

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

  // Update the maximum boundaries
  if(pos_x < m_xmin)
    m_xmin = pos_x;
  if(pos_x > m_xmax)
    m_xmax = pos_x;
  if(pos_y < m_ymin)
    m_ymin = pos_y;
  if(pos_y > m_ymax)
    m_ymax = pos_y;

  return(true);
}

//-------------------------------------------------------------
// Procedure: updateVehicleBearingLine
//
//     BEARING_LINE = "vname=alpha, bearing=124, vector_length=40, 
//         vector_color=red, vector_time=nolimit" 


bool VehicleSet::updateVehicleBearingLine(const string& str) 
{
  BearingLine bearing_line;
  bearing_line.setTimeStamp(m_curr_time);

  string vname;

  vector<string> svector = parseString(str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string left  = tolower(biteStringX(svector[i], '='));
    string right = svector[i];
    if(left == "vname")
      vname = right;
    else if((left == "bearing") && isNumber(right))
      bearing_line.setBearing(atof(right.c_str()));
    else if((left == "range") && isNumber(right))
      bearing_line.setRange(atof(right.c_str()));
    else if((left == "vector_width") && isNumber(right))
      bearing_line.setVectorWidth(atof(right.c_str()));
    else if((left == "time_stamp") && isNumber(right))
      bearing_line.setTimeStamp(atof(right.c_str()));
    else if((left == "time_limit") && isNumber(right))
      bearing_line.setTimeLimit(atof(right.c_str()));
    else if((left == "vector_color") && isColor(right))
      bearing_line.setVectorColor(right);
    else if(left == "label")
      bearing_line.setLabel(right);
    else if(left == "bearing_absolute") {
      if(tolower(right) == "true")
	bearing_line.setBearingAbsolute(true);
      else if(tolower(right) == "false")
	bearing_line.setBearingAbsolute(false);
    }
  }

  if(vname == "")
    return(false);

  if(!bearing_line.isValid())
    return(false);
    
  m_bearing_map[vname] = bearing_line; 
  return(true);
}











