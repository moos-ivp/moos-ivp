/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_StationKeep.cpp                                  */
/*    DATE: Aug 25 2006                                          */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <math.h> 
#include <stdlib.h>
#include "BHV_StationKeep.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_StationKeep::BHV_StationKeep(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  // First set variables at the superclass level
  m_descriptor = "station_keep";  
  m_domain     = subDomain(m_domain, "course,speed");

  // Default values for Configuration Parameters
  m_station_x    = 0;
  m_station_y    = 0;
  m_outer_radius = 15;
  m_inner_radius = 4;
  m_outer_speed  = 1.2;
  m_extra_speed  = 2.5;
  m_center_activate = false;

  // Configuration parameters for UUV station-keeping
  m_station_depth_radius   = -1;   // -1 indicates not enabled
  m_station_depth          = 5.0;  // meters
  m_station_depth_state    = "disabled";

  // Default values for State  Variables
  m_center_pending  = false;
  m_center_assign   = "";
  m_station_set     = false;

  // Declare information needed by this behavior
  addInfoVars("NAV_X, NAV_Y");
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition".
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_StationKeep::setParam(string param, string val) 
{
  param = tolower(param);
  val   = stripBlankEnds(val);

  if(param == "station_pt") {
    m_center_assign  = val;
    m_center_pending = true;
    return(updateCenter());
    return(true);
  }

  else if(param == "station_depth_radius") {
    if((tolower(val) == "off") || (val == "-1")) {
      m_station_depth_state = "disabled";
      m_station_depth_radius = -1;
      return(true);
    }
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    m_station_depth_radius = dval;
    if(m_station_depth_state == "disabled")
      m_station_depth_state = "inactive";
    return(true);
  }

  else if(param == "station_depth") {
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    m_station_depth = dval;
    return(true);
  }

  else if(param == "center_activate") {
    val = tolower(val);
    if((val!="true")&&(val!="false"))
      return(false);
    m_center_activate = (val == "true");
    return(true);
  }  

  else if(param == "outer_radius") {
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    m_outer_radius = dval;
    return(true);
  }

  else if(param == "inner_radius") {
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    m_inner_radius = dval;
    return(true);
  }

  else if(param == "outer_speed") {
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    m_outer_speed = dval;
    if(m_extra_speed < m_outer_speed)
      m_extra_speed = m_outer_speed;
    return(true);
  }

  else if((param == "extra_speed") || (param == "transit_speed")) {
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    m_extra_speed = dval;
    return(true);
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: onIdleState
//      Note: If m_center_pending is true, each time the behavior
//            goes inactive (and thus this function is called), 
//            a pending new center is declared, and set to the 
//            special value of present_position, which will be 
//            determined when the activation occurs.

void BHV_StationKeep::onIdleState()
{
  postStationMessage(false);
  if(!m_center_activate)
    return;
  m_center_pending = true;
  m_center_assign  = "present_position";
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_StationKeep::onRunState() 
{
  // Set m_osx, m_osy
  if(!updateInfoIn())
    return(0);

  updateCenter();

  if(!m_station_set) {
    postWMessage("STATION_POINT_NOT_SET");
    postStationMessage(false);
    return(0);
  }

  // Calculate and post the distance to station only after updateInfoIn()
  // and updateCenter() are called and m_station_set is determined set.
  m_dist_to_station  = hypot((m_osx-m_station_x), (m_osy-m_station_y));
  postMessage("DIST_TO_STATION", m_dist_to_station);

  // If station-keeping at depth is enabled, determine current state.
  updateStationDepthState();

  postMessage("STATION_DEPTH_STATE", toupper(m_station_depth_state));

  postStationMessage(true);

  // If the station_depth_state is inactive it means that station-keeping
  // at depth is enabled, but currently not warranting action.
  if(m_station_depth_state == "inactive")
    return(0);

  // Action may also not be warranted due to being close enough to the 
  // station-keep point.
  if(m_dist_to_station <= m_inner_radius)
    return(0);
  
  double angle_to_station = relAng(m_osx, m_osy, 
				   m_station_x, m_station_y);

  double desired_speed = 0;
  if((m_dist_to_station > m_inner_radius) && (m_dist_to_station < m_outer_radius)) {
    double range  = m_outer_radius - m_inner_radius;
    double pct    = (m_dist_to_station - m_inner_radius) / range;
    desired_speed = pct * m_outer_speed;
  }

  if(m_dist_to_station >= m_outer_radius)
    desired_speed = m_extra_speed;

  ZAIC_PEAK spd_zaic(m_domain, "speed");
  spd_zaic.setSummit(desired_speed);
  spd_zaic.setBaseWidth(0.4);
  spd_zaic.setPeakWidth(0.0);
  spd_zaic.setSummitDelta(0.0);
  spd_zaic.setMinMaxUtil(0, 25);
  IvPFunction *spd_ipf = spd_zaic.extractIvPFunction();
  
  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setSummit(angle_to_station);
  crs_zaic.setBaseWidth(180.0);
  crs_zaic.setValueWrap(true);
  IvPFunction *crs_ipf = crs_zaic.extractIvPFunction();
  
  OF_Coupler coupler;
  IvPFunction *ipf = coupler.couple(crs_ipf, spd_ipf);

  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}


//-----------------------------------------------------------
// Procedure: updateInfoIn()
//   Purpose: Update info need by the behavior from the info_buffer.
//            Error or warning messages can be posted.
//   Returns: true if no vital info is missing from the info_buffer.
//            false otherwise.
//      Note: By posting an EMessage, this sets the state_ok member
//            variable to false which will communicate the gravity
//            of the situation to the helm.

bool BHV_StationKeep::updateInfoIn()
{
  bool ok1, ok2;
  // ownship position in meters from some 0,0 reference point.
  m_osx = getBufferDoubleVal("NAV_X", ok1);
  m_osy = getBufferDoubleVal("NAV_Y", ok2);

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2) {
    postEMessage("No ownship X/Y info in info_buffer.");
    return(false);
  }
  
  return(true);
}



//-----------------------------------------------------------
// Procedure: updateCenter()

bool BHV_StationKeep::updateCenter()
{
  if(!m_center_pending)
    return(true);
  
  bool ok_update = false;

  m_center_assign = tolower(m_center_assign);

  if(m_center_assign == "present_position") {
    m_station_x   = m_osx;
    m_station_y   = m_osy;
    m_station_set = true;
    ok_update = true;
  }
  else {
    vector<string> svector = parseString(m_center_assign, ',');
    if(svector.size() == 2) {
      svector[0] = stripBlankEnds(svector[0]);
      svector[1] = stripBlankEnds(svector[1]);
      if(isNumber(svector[0]) && isNumber(svector[1])) {
	double xval = atof(svector[0].c_str());
	double yval = atof(svector[1].c_str());
	m_station_x   = xval;
	m_station_y   = yval;
	m_station_set = true;
	ok_update     = true;
      }
    }
  }
  
  m_center_assign = "";
  m_center_pending = false;
  return(ok_update);
}


//-----------------------------------------------------------
// Procedure: postStationMessage()

void BHV_StationKeep::postStationMessage(bool post)
{
  string str_x = doubleToString(m_station_x,1);
  string str_y = doubleToString(m_station_y,1);

  string poly_str = "radial:: x=" + str_x;

  string label  = m_us_name + ":station-keep";
  string source = m_us_name + ":" + m_descriptor;

  poly_str += ",y=" + str_y;
  poly_str += ",radius=" + doubleToString(m_outer_radius,1);
  poly_str += ",pts=12, label=" + label;
  poly_str += ",source=" + source;

  if(post==false)
    poly_str += ",active=false";

  postMessage("VIEW_POLYGON", poly_str);
}


//-----------------------------------------------------------
// Procedure: updateStationDepthState

void BHV_StationKeep::updateStationDepthState()
{
  if(m_station_depth_state == "disabled")
    return;
  
  if(m_station_depth_state == "inactive") {
    if(m_dist_to_station > m_station_depth_radius)
      m_station_depth_state = "active";
    return;
  }

  if(m_station_depth_state == "active")
    if(m_dist_to_station <= m_inner_radius)
      m_station_depth_state = "inactive";
  
}


