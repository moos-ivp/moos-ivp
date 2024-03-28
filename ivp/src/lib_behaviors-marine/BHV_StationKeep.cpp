/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_StationKeep.cpp                                  */
/*    DATE: Aug 25 2006                                          */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <cmath> 
#include <cstdlib>
#include "BHV_StationKeep.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"
#include "XYFormatUtilsPoint.h"

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
  m_station_x        = 0;
  m_station_y        = 0;
  m_static_station_x = 0;
  m_static_station_y = 0;
  m_static_station_defined = false;
  m_outer_radius     = 15;
  m_inner_radius     = 4;
  m_outer_speed      = 1.2;
  m_extra_speed      = 2.5;
  m_center_activate  = false;
  m_pskeep_radius    = -1;   // -1 indicates not enabled
  m_pskeep_variable  = "PSKEEP_MODE";
  m_station_ctr_var  = "STAION_KEEP_CTR";
  m_swing_time       = 0;
  
  // All visual hints initially turned off
  m_hint_vertex_color = "red";
  m_hint_edge_color   = "light_blue";
  m_hint_label_color  = "white";
  m_hint_vertex_size  = 1;
  m_hint_edge_size    = 1;

  // Default values for State  Variables
  m_center_pending     = false;
  m_station_set        = false;
  m_pskeep_state       = "disabled";
  m_transit_state      = "disabled";
  m_dist_to_station    = -1;
  m_mark_time          = -1;

  // Declare information needed by this behavior
  addInfoVars("NAV_X, NAV_Y");
}

//-----------------------------------------------------------
// Procedure: setParam()
//     Notes: We expect the "station_pt" entry will be of the form
//            "xposition,yposition".
//            The "radius" parameters is given in meters.

bool BHV_StationKeep::setParam(string param, string val) 
{
  param = tolower(param);
  val   = stripBlankEnds(val);

#if 0
  if((param == "station_pt") || (param == "point")) {
    string left  = biteStringX(val, ',');
    string right = val;
    if(!isNumber(left) || !isNumber(right))
      return(false);
    m_static_station_x = atof(left.c_str());
    m_static_station_y = atof(right.c_str());
    m_static_station_defined = true;
    m_center_pending = true;
    return(true);
  }
#endif

  if((param == "station_pt") || (param == "point")) {
    XYPoint pt = string2Point(val);
    if(!pt.valid())
      return(false);
    m_static_station_x = pt.x();
    m_static_station_y = pt.y();
    m_static_station_defined = true;
    m_center_pending = true;
    return(true);
  }

  else if(param == "hibernation_radius") {
    if((tolower(val) == "off") || (val == "-1")) {
      m_pskeep_state = "disabled";
      m_pskeep_radius = -1;
      return(true);
    }
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    m_pskeep_radius = dval;
    if(m_pskeep_state == "disabled")
      m_pskeep_state = "hibernating";
    return(true);
  }

  else if(param == "center_activate") {
    val = tolower(val);
    if((val!="true")&&(val!="false"))
      return(false);
    m_center_activate = (val == "true");
    m_center_pending = true;
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

  else if(param == "swing_time") {
    double dval = atof(val.c_str());
    if(!isNumber(val))
      return(false);
    m_swing_time = vclip(dval, 0, 60);
    return(true);
  }

  else if((param == "extra_speed") || (param == "transit_speed")) {
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    m_extra_speed = dval;
    return(true);
  }
  else if(param == "visual_hints")  {
    vector<string> svector = parseStringQ(val, ',');
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++) 
      handleVisualHint(svector[i]);
    return(true);
  }
  return(false);
}

//-----------------------------------------------------------
// Procedure: onIdleToRunState()

void BHV_StationKeep::onIdleToRunState()
{
  postConfigStatus();

  // The below hack will ensure the vehicle actively seeks the station
  // center each time the vehicle enters the running state. Current 
  // thinking is that this is not necessary.
  // 
  // m_dist_to_station = m_pskeep_radius + 1;
  // updateHibernationState();
}

//-----------------------------------------------------------
// Procedure: onRunToIdleState()
//      Note: If m_center_pending is true, each time the behavior
//            goes inactive (and thus this function is called), 
//            a pending new center is declared, and set to the 
//            special value of present_position, which will be 
//            determined when the activation occurs.

void BHV_StationKeep::onRunToIdleState()
{
  historyClear();
  postStationMessage(false);

  // If conigured for center_activation, declare the need for it
  // here so when/if the behavior goes into the runstate, it will
  // know that it needs to update the station-keep position.
  if(m_center_activate)
    m_center_pending = true;

  // Time at which the behavior transitioned from idle to running.
  // So when idle, always reset to -1.
  m_mark_time = -1;
}

//-----------------------------------------------------------
// Procedure: onRunState()

IvPFunction *BHV_StationKeep::onRunState() 
{
  // Set m_osx, m_osy, m_currtime, m_dist_to_station
  if(!updateInfoIn())
    return(0);
  postMessage("DIST_TO_STATION", m_dist_to_station);

  if(!m_station_set) {
    postWMessage("STATION_POINT_NOT_SET");
    postStationMessage(false);
    return(0);
  }
  
  if(m_inner_radius > m_outer_radius)
    m_outer_radius = m_inner_radius;

  // If station-keeping at depth is enabled, determine current state.
  updateHibernationState();

  postMessage(m_pskeep_variable, toupper(m_pskeep_state));

  postStationMessage(true);

  double angle_to_station = relAng(m_osx, m_osy, 
				   m_station_x, m_station_y);

  if(m_center_pending)          // mikerb mod jan1423
    angle_to_station = m_osh;
  
  double desired_speed = 0;
  // If the pskeepp_state is hibernating it means that station-keeping
  // at depth is enabled, but currently not warranting action. Action
  // may also not be warranted due to being close enough to the
  // station-keep point.
  if((m_pskeep_state == "hibernating") || 
     (m_dist_to_station <= m_inner_radius)) {
    desired_speed = 0;
  }
  else if((m_dist_to_station > m_inner_radius) && 
	  (m_dist_to_station < m_outer_radius)) {
    // Note: range cannot be zero due to above if-condition
    double range  = m_outer_radius - m_inner_radius;
    double pct    = (m_dist_to_station - m_inner_radius) / range;
    desired_speed = pct * m_outer_speed;
  }
  else // m_dist_to_station >= m_outer_radius
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
  // PART 1: GET THE INFORMATION NEEDED FROM THE INFO_BUFFER
  bool ok1, ok2, ok3;
  // ownship position in meters from some 0,0 reference point.
  m_currtime = getBufferCurrTime();
  m_osx = getBufferDoubleVal("NAV_X", ok1);
  m_osy = getBufferDoubleVal("NAV_Y", ok2);
  m_osh = getBufferDoubleVal("NAV_HEADING", ok3);

  // If previous state was idle (mark_time=-1), note the time entered
  // into the running state.
  if(m_mark_time == -1)
    m_mark_time = m_currtime;

  // Must get ownship nav position from InfoBuffer
  if(!ok1 || !ok2 || !ok3) {
    postEMessage("No ownship X/Y or HDG info in info_buffer.");
    return(false);
  }

  // PART 2: UPDATE THE STATION-KEEP POINT IF NECESSARY
  bool ctr_changed = false;
  if(m_center_pending) {
    if(m_center_activate) {
      m_station_x   = m_osx;
      m_station_y   = m_osy;
      m_station_set = true;
      ctr_changed = true;
      if(m_currtime >= (m_mark_time + m_swing_time))
	m_center_pending = false;
    }
    else {
      if(m_static_station_defined) {
	m_station_x = m_static_station_x;
	m_station_y = m_static_station_y;
	m_station_set = true;
	m_center_pending = false;
	ctr_changed = true;
      }
    }
  }
  if(ctr_changed) {
    // Post the center to the MOOSDB
    m_pskeep_state = "hibernating";
    m_transit_state = "hibernating";
    return(true);
  }

  // PART 3: CALCULATE DISTANCE TO STATION-PT AND UPDATE HISTORIES
  // Calculate the distance to the station-point
  m_dist_to_station = hypot((m_osx-m_station_x), (m_osy-m_station_y));

  // Update the distance and time histories
  m_distance_history.push_front(m_dist_to_station);
  m_distance_thistory.push_front(m_currtime);

  // Remove all elements of the list(s) greater then 10 seconds old 
  unsigned int i, amt_to_remove = 0;
  list<double>::reverse_iterator p;
  for(p=m_distance_thistory.rbegin(); p!=m_distance_thistory.rend(); p++) {
    double ptime = *p;
    if((m_currtime - ptime) > 10)
      amt_to_remove++;
  }
  for(i=0; i<amt_to_remove; i++) {
    m_distance_history.pop_back();
    m_distance_thistory.pop_back();
  }
  return(true);
}

//-----------------------------------------------------------
// Procedure: postStationMessage()

void BHV_StationKeep::postStationMessage(bool post)
{
  string str_x = doubleToString(m_station_x,1);
  string str_y = doubleToString(m_station_y,1);

  string poly_str = "radial:: x=" + str_x;
  poly_str += ",y=" + str_y;
  poly_str += ",source=" + m_us_name+ ":" + m_descriptor;
  poly_str += ",pts=32";
  if(m_hint_edge_size >= 0)
    poly_str += ",edge_size=" + doubleToStringX(m_hint_edge_size);
  if(m_hint_vertex_size >= 0)
    poly_str += ",vertex_size=" + doubleToStringX(m_hint_vertex_size);
  if(m_hint_edge_color != "")
    poly_str += ",edge_color=" + m_hint_edge_color;
  if(m_hint_label_color != "")
    poly_str += ",label_color=" + m_hint_label_color;
  if(m_hint_vertex_color != "")
    poly_str += ",vertex_color=" + m_hint_vertex_color;

  string poly_str_outer = poly_str;
  string poly_str_inner = poly_str;
  string poly_str_hiber = poly_str;
  poly_str_outer += ",label="  + m_us_name + ":station-keep-out";
  poly_str_outer += ",radius=" + doubleToString(m_outer_radius,1);
  poly_str_inner += ",label="  + m_us_name + ":station-keep-in";
  poly_str_inner += ",radius=" + doubleToString(m_inner_radius,1);
  poly_str_hiber += ",label="  + m_us_name + ":station-keep-hiber";
  poly_str_hiber += ",radius=" + doubleToString(m_pskeep_radius,1);

  if(post==false) {
    poly_str_outer += ",active=false";
    poly_str_inner += ",active=false";
    poly_str_hiber += ",active=false";
  }

  postMessage("VIEW_POLYGON", poly_str_outer, "outer");

  // No need to post both circles if the radii are collapsed, but if
  // we're trying to erase a circle, post anyway just ensure no 
  // dangling artifacts from the radii being altered dynaically.
  if((m_inner_radius < m_outer_radius) || (post==false))
    postMessage("VIEW_POLYGON", poly_str_inner, "inner");

  // No need to post both circles if the radii are collapsed, but if
  // we're trying to erase a circle, post anyway just ensure no 
  // dangling artifacts from the radii being altered dynamically.
  if((m_pskeep_radius > m_outer_radius) || (post==false))
    postMessage("VIEW_POLYGON", poly_str_hiber, "hiber");
}


//-----------------------------------------------------------
// Procedure: updateHibernationState()

void BHV_StationKeep::updateHibernationState()
{
  if(m_pskeep_state == "disabled")
    return;  
  
  if(m_transit_state == "pending_progress_start") {
    if(m_dist_to_station <= m_pskeep_radius) {
      if(historyShowsProgressStart()) {
	historyClear();
	m_transit_state = "noted_progress_start";
      }
    }
  }
  else if(m_transit_state == "noted_progress_start") {
    if(historyShowsProgressEnd())
      m_transit_state = "noted_progress_end";
  }
  
  if(m_pskeep_state == "hibernating") {
    if(m_dist_to_station > m_pskeep_radius) {
      m_pskeep_state = "seeking_station";
      historyClear();
      m_transit_state = "pending_progress_start";
    }
    return;
  }

  if(m_pskeep_state == "seeking_station") {
    if((m_dist_to_station <= m_inner_radius) || 
       ((m_transit_state == "noted_progress_end") && 
	(m_dist_to_station <= m_pskeep_radius))) {

      m_pskeep_state = "hibernating";
      m_transit_state = "hibernating";
      historyClear();
    }
  }
}


//-----------------------------------------------------------
// Procedure: historyShowsProgressStart()
//
//     |                               
//     |                   o              
//     |                o     o           Progress declared   
// DIST| o     o  o  o           o        due to overall closer
//     |    o                       o     from beginning to end
//     |                                     
//     |                                          
//     |                               
//     ----------------------------------------------------->
//       0  1  2  3  4  5  6  7  8  9  
//

bool BHV_StationKeep::historyShowsProgressStart()
{
  double oldest_dist = m_distance_history.back();
  double newest_dist = m_distance_history.front();

  double oldest_time = m_distance_thistory.back();
  double newest_time = m_distance_thistory.front();

  double delta_time = newest_time - oldest_time;
  if(delta_time <= 5)
    return(false);

  double rate = (newest_dist - oldest_dist) / delta_time;
  if(rate <= 0)
    return(true);

  return(false);
}


//-----------------------------------------------------------
// Procedure: historyShowsProgressEnd()
//
//     |                               
//     |                   o              
//     |                o     o     o    Progress END declared due
// DIST| o     o  o  o           o       to overall opening dist
//     |    o                            from beginning to end
//     |                                     
//     |                                          
//     |                               
//     ----------------------------------------------------->
//       0  1  2  3  4  5  6  7  8  9  
//

bool BHV_StationKeep::historyShowsProgressEnd()
{
  double oldest_dist = m_distance_history.back();
  double newest_dist = m_distance_history.front();

  double oldest_time = m_distance_thistory.back();
  double newest_time = m_distance_thistory.front();

  double delta_time = newest_time - oldest_time;

  if(delta_time <= 5)
    return(false);

  double rate = (newest_dist - oldest_dist) / delta_time;
  if(rate >= 0)
    return(true);

  return(false);
}


//-----------------------------------------------------------
// Procedure: handleVisualHint()

void BHV_StationKeep::handleVisualHint(string hint)
{
  string param = tolower(stripBlankEnds(biteString(hint, '=')));
  string value = stripBlankEnds(hint);
  
  if((param == "vertex_color") && isColor(value))
    m_hint_vertex_color = value;
  else if((param == "edge_color") && isColor(value))
    m_hint_edge_color = value;
  else if((param == "edge_size") && isNumber(value))
    m_hint_edge_size = atof(value.c_str());
  else if((param == "vertex_size") && isNumber(value))
    m_hint_vertex_size = atof(value.c_str());
  else if((param == "label_color") && isColor(value))
    m_hint_label_color = value;
}

//-----------------------------------------------------------
// Procedure: historyClear()

void BHV_StationKeep::historyClear()
{
  m_distance_history.clear();
  m_distance_thistory.clear();
}


//-----------------------------------------------------------
// Procedure: postConfigStatus()

void BHV_StationKeep::postConfigStatus()
{
  string str = "type=BHV_StationKeep,name=" + m_descriptor;
  
  str += ",x=" + doubleToString(m_static_station_x,2);
  str += ",y=" + doubleToString(m_static_station_y,2);
  str += ",inner_radius=" + doubleToString(m_inner_radius,1);
  str += ",outer_radius=" + doubleToString(m_outer_radius,1);
  str += ",hibernation_radius=" + doubleToString(m_pskeep_radius,1);
  str += ",outer_speed=" + doubleToString(m_outer_speed,1);
  str += ",center_activate=" + boolToString(m_center_activate);

  postRepeatableMessage("BHV_SETTINGS", str);
}










