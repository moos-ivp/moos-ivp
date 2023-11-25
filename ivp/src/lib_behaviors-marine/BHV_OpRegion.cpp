/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_OpRegion.cpp                                     */
/*    DATE: May 1st, 2005 Sunday at Joe's in Maine               */
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
#include "BHV_OpRegion.h"
#include "MBUtils.h"
#include "VarDataPairUtils.h"
#include "XYFormatUtilsPoly.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_OpRegion::BHV_OpRegion(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  m_descriptor   = "bhv_opregion";
  m_max_depth    = 0;
  m_min_altitude = 0;
  m_max_time     = 0;
  m_hint_vertex_size  = 3;
  m_hint_vertex_color = "brown"; 
  m_hint_edge_size    = 1;
  m_hint_edge_color   = "aqua"; 
  m_hint_label_color  = "aqua"; 

  m_breached_poly_flags_posted  = false;
  m_breached_time_flags_posted  = false;
  m_breached_altitude_flags_posted = false;
  m_breached_depth_flags_posted = false;

  m_soft_poly_breach = false;
  m_soft_poly_breach_posted = false;
  // Be explicit about this variable is empty string by default
  m_time_remaining_var = "";

  // Be explicit about this variable is empty string by default
  m_opregion_poly_var = "";

  // Keep track of whether the vehicle was in the polygon on the
  // previous invocation of the behavior. Initially assume false.
  m_previously_in_poly = false;

  // Keep a flag indicating whether this is the first time the
  // behavior is invoked.
  m_first_time   = true;

  // Time stamps for calculating how long the vehicle has been
  // inside or out of the polygon.
  m_previous_time = 0;
  m_current_time  = 0;
  m_elapsed_time  = 0;
  m_start_time    = 0;
  m_delta_time    = 0;
  m_secs_in_poly  = 0;
  m_secs_out_poly = 0;
  
  // Declare whether the polygon containment condition is effective
  // immediately (default) or triggered only when the vehicle has
  // first entered the polygon region. This is useful if the vehicle
  // is being launched from a dock or area which is outside the 
  // safe zone
  m_trigger_on_poly_entry = false;

  // Maintain a flag indicating whether the vehicle has entered
  // the polygon region. This value is only relevant if the 
  // trigger_on_poly_entry flag is set to be true.
  m_poly_entry_made = false;

  // Declare the amount of time required for the vehicle to be 
  // within the polygon region before the polygon containment 
  // condition is enforced. This value is only relevant if the 
  // trigger_on_poly_entry flag is set to be true.
  m_trigger_entry_time = 1.0;

  // Declare the amount of time required for the vehicle to be 
  // outside the polygon region before the polygon containment 
  // condition triggers a declaration of emergency. Setting this
  // to be non-zero may be useful if the position sensor (GPS) 
  // occasionally has a whacky single position reading outside
  // the polygon region.
  m_trigger_exit_time = 0.5;

  // Declare the variables we will need from the info_buffer
  addInfoVars("NAV_X, NAV_Y, NAV_HEADING");
  addInfoVars("NAV_SPEED, NAV_DEPTH, NAV_ALTITUDE");
  addInfoVars("MOOS_MANUAL_OVERRIDE");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_OpRegion::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  // Typical line: polygon  = 0,0:0,100:100,0:100,100
  if(param == "polygon") {
    XYPolygon new_poly = string2Poly(val);
    if(!new_poly.is_convex())  // Should be convex - false otherwise
      return(false);
    m_polygon = new_poly;
    m_polygons.clear();
    m_polygons.push_back(new_poly);
    return(true);
  }
  else if(param == "max_depth") 
    return(setNonNegDoubleOnString(m_max_depth, val));
  else if(param == "reset_var") {
    bool ok = setNonWhiteVarOnString(m_reset_var, val);
    if(ok)
      addInfoVars(m_reset_var);
    return(ok);
  }
  else if(param == "time_remaining_var")
    return(setNonWhiteVarOnString(m_time_remaining_var, val));
  else if(param == "breached_poly_flag") 
    return(addVarDataPairOnString(m_breached_poly_flags, val));
  else if(param == "breached_time_flag") 
    return(addVarDataPairOnString(m_breached_time_flags, val));
  else if(param == "breached_altitude_flag") 
    return(addVarDataPairOnString(m_breached_altitude_flags, val));
  else if(param == "breached_depth_flag") 
    return(addVarDataPairOnString(m_breached_depth_flags, val));
  else if(param == "soft_poly_breach") 
    return(setBooleanOnString(m_soft_poly_breach, val));
  else if(param == "min_altitude") 
    return(setNonNegDoubleOnString(m_min_altitude, val));
  else if(param == "max_time")
    return(setNonNegDoubleOnString(m_max_time, val));
  else if(param == "trigger_exit_time") 
    return(setNonNegDoubleOnString(m_trigger_exit_time, val));
  else if(param == "opregion_poly_var") 
    return(setNonWhiteVarOnString(m_opregion_poly_var, val));
  else if(param == "trigger_entry_time") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    m_trigger_entry_time = dval;
    if(m_trigger_entry_time > 0)
      m_trigger_on_poly_entry = true;
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
// Procedure: onSetParamComplete()

void BHV_OpRegion::onSetParamComplete()
{
  if(m_opregion_poly_var != "") {
    if(m_polygon.size() > 0) {
      string spec = m_polygon.get_spec_pts(2);
      postMessage(m_opregion_poly_var, spec);
    }
  }

  postConfigStatus();
}

//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_OpRegion::onIdleState() 
{
  checkForReset();
  postTimeRemaining();
  postErasablePolygon();
}

//-----------------------------------------------------------
// Procedure: onRunState
//     Notes: Always returns NULL, never returns an IvPFunction*
//     Notes: Sets state_ok = false and posts an error message if
//            any of the OpRegion conditions are not met.

IvPFunction *BHV_OpRegion::onRunState() 
{
  checkForReset();
  setTimeStamps();
  checkForSoftPolyReset();
  
  // Each of the below calls will check their critical conditions
  // and post an error message if a violation is detected. The call
  // to postEMessage() also sets state_ok = false;
  polygonVerify();
  postPolyStatus();
  depthVerify();
  altitudeVerify();
  timeoutVerify();
  postTimeRemaining();

  postMessage("SECS_IN_POLY", m_secs_in_poly);

  
  postViewablePolygon();
  return(0);
}

//-----------------------------------------------------------
// Procedure: polygonVerify()
//      Note: Verify that polygon boundary hasn't been violated.
//    Return: void - An error message is communicated by a call to 
//            postEMessage().

void BHV_OpRegion::polygonVerify()
{
  if(m_polygons.size() == 0)
    return;

  bool ok1, ok2;
  double osX = getBufferDoubleVal("NAV_X", ok1);
  double osY = getBufferDoubleVal("NAV_Y", ok2);

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2) {
    string msg = "No ownship info in info_buffer";
    postEMessage(msg);
    return;
  }

  bool contained = true;
  unsigned int i, vsize = m_polygons.size();
  for(i=0; i<vsize; i++) {
    if(!m_polygons[i].contains(osX, osY))
      contained = false;
  }

  // Determine the accumulated time within the polygon
  if(contained) {
    m_secs_out_poly = 0;
    if(m_previously_in_poly)
      m_secs_in_poly += m_delta_time;
    else
      m_secs_in_poly = 0;
    m_previously_in_poly = true;
  }
  // Determine the accumulated time outside the polygon
  if(!contained) {
    m_secs_in_poly = 0;
    if(!m_previously_in_poly)
      m_secs_out_poly += m_delta_time;
    else
      m_secs_out_poly = 0;
    m_previously_in_poly = false;
  }

  //cout << "---------------------------" << endl;
  //cout << "Previously In - " << previously_in_poly << endl;
  //cout << "Contained -     " << contained << endl;
  //cout << "Secs In  Poly - " << secs_in_poly << endl;
  //cout << "Secs Out Poly - " << secs_out_poly << endl;
  //cout << "Entry_Made:     " << poly_entry_made << endl;


  // Case 1: Vehicle in polygon. Check to see if its been
  //         in the poly long enough to be considered an 
  //         official entry into the polygon.
  if(contained) {
    if(m_secs_in_poly >= m_trigger_entry_time)
      m_poly_entry_made = true;
    return;
  }

  // Case 2: Vehicle not in polygon and no prior polygon
  //         entry is required for triggering emergency flag.
  //         Return based on accumulated time outside of poly.
  if(!contained && !m_trigger_on_poly_entry)
    if(m_secs_out_poly < m_trigger_exit_time)
      return;

  // Case 3: Vehicle not in polygon, poly entry is needed to
  //         trigger emergency, but no entry has been made yet.
  if(!contained && m_trigger_on_poly_entry && !m_poly_entry_made)
    return;

  // Case 4: Vehicle not in polygon, poly entry is needed to
  //         trigger emergency, and previous entry detected.
  //         Return based on accumulated time outside of poly.
  if(!contained && m_trigger_on_poly_entry && m_poly_entry_made)
    if(m_secs_out_poly < m_trigger_exit_time)
      return;


  // All verification cases failed. Handle the polygon breach.
  // Regardless of soft breach mode or not, post the breach flags
  if(!m_breached_poly_flags_posted)
    postBreachFlags("poly");

  if(m_soft_poly_breach) {
    postRepeatableMessage("MOOS_MANUAL_OVERRIDE", "true");
    postWMessage("Soft polygon containment breach");
    m_soft_poly_breach_posted = true;
    m_breached_poly_flags_posted = false;
    m_secs_in_poly = 0;
    m_previously_in_poly = false;
    m_poly_entry_made = false;
  }
  else {
    string emsg = "BHV_OpRegion Polygon containment failure: ";
    emsg += " x=" + doubleToString(osX,1);
    emsg += " y=" + doubleToString(osY,1);
    postEMessage(emsg);
  }
}

//-----------------------------------------------------------
// Procedure: postPolyStatus
//   Purpose: To calculate information about impending OpRegion
//            violations. It calculates and posts the following
//            variables:
//        (1) OPREG_TRAJECTORY_PERIM_ETA: (double) 
//            The time in seconds until the vehicle exits the 
//            polygon containment region if it stays on the 
//            current trajectory.
//        (2) OPREG_TRAJECTORY_PERIM_DIST: (double)
//            Distance in meters until the vehicle exits the 
//            polygon containment region if it stays on the 
//            current trajectory.
//        (3) OPREG_ABSOLUTE_PERIM_ETA: (double) 
//            Time in seconds until the vehicle would exit the
//            polygon if it were to take the shortest path at
//            top vehicle speed.
//        (4) OPREG_ABSOLUTE_PERIM_DIST: (double) 
//            Distance in meters between the vehicle and the 
//            polygon perimeter regardless of current heading
//        (5) OPREG_TIME_REMAINING: (double)
//            Time in seconds before a OpRegion timeout would
//            occur. If max_time=0, then no such message posted.
//   
//    Return: void

void BHV_OpRegion::postPolyStatus()
{
  bool ok1, ok2, ok3, ok4;
  double osX   = getBufferDoubleVal("NAV_X", ok1);
  double osY   = getBufferDoubleVal("NAV_Y", ok2);
  double osSPD = getBufferDoubleVal("NAV_SPEED", ok3);
  double osHDG = getBufferDoubleVal("NAV_HEADING", ok4);

  string msg;
  if(!ok1) 
    msg = "No ownship NAV_X (" + m_us_name + ") in info_buffer";
  if(!ok2) 
    msg = "No ownship NAV_Y (" + m_us_name + ") in info_buffer";
  if(!ok3) 
    msg = "No ownship NAV_SPEED (" + m_us_name + ") in info_buffer";
  if(!ok4) 
    msg = "No ownship NAV_HEADING (" + m_us_name + ") in info_buffer";

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2 || !ok3 || !ok4) {
    postEMessage(msg);
    return;
  }

  // Must find the top vehicle speed in the behavior ivp domain
  int index = m_domain.getIndex("speed");
  if(index == -1) {
    string msg = "No Top-Speed info found in the decision domain";
    postEMessage(msg);
    return;
  }
  double osTopSpeed = m_domain.getVarHigh(index);
  
  // Calculate the time and the distance to the perimeter along the
  // current heading (CH).
  double trajectory_perim_dist = m_polygon.dist_to_poly(osX, osY, osHDG);
  double trajectory_perim_eta = 0;
  if(osSPD > 0)
    trajectory_perim_eta = trajectory_perim_dist / osSPD;
  
  // post the distance at integer precision unless close to zero
  if(trajectory_perim_dist <= 1)
    postMessage("OPREG_TRAJECTORY_PERIM_DIST", trajectory_perim_dist);
  else  
    postIntMessage("OPREG_TRAJECTORY_PERIM_DIST", trajectory_perim_dist);
  postIntMessage("OPREG_TRAJECTORY_PERIM_ETA",  trajectory_perim_eta);
  
  // Calculate the absolute (ABS) distance and ETA to the perimeter.
  double absolute_perim_dist = m_polygon.dist_to_poly(osX, osY);
  double absolute_perim_eta  = 0;
  if(osTopSpeed > 0)
    absolute_perim_eta  = absolute_perim_dist / osTopSpeed;
  
  // post the distance at integer precision unless close to zero
  if(absolute_perim_dist <= 1)
    postMessage("OPREG_ABSOLUTE_PERIM_DIST", absolute_perim_dist);
  else
    postIntMessage("OPREG_ABSOLUTE_PERIM_DIST", absolute_perim_dist);
  postIntMessage("OPREG_ABSOLUTE_PERIM_ETA",  absolute_perim_eta);
  
  if(m_max_time > 0) {
    double remaining_time = m_max_time - m_elapsed_time;
    if(remaining_time < 0)
      remaining_time = 0;
    postMessage("OPREG_TIME_REMAINING", remaining_time);
  }
}

//-----------------------------------------------------------
// Procedure: depthVerify()
//      Note: Verify that the depth limit hasn't been violated.
//    Return: void - An error message is communicated by a call to 
//            postEMessage().

void BHV_OpRegion::depthVerify()
{
  // If no max_depth specified, return with no error message posted.
  if(m_max_depth <= 0)
    return;

  bool ok;
  double depth = getBufferDoubleVal("NAV_DEPTH", ok);

  // Must get ownship depth from info_buffer
  if(!ok) { 
    //cout << "No NAV_DEPTH in info_buffer for vehicle: " << m_us_name << endl;
    postEMessage("No ownship depth in info_buffer.");
    return;
  }

  if(depth > m_max_depth) {
    string emsg = "OpRegion Depth failure: max:";
    emsg += doubleToString(m_max_depth);
    emsg += " detected:" + doubleToString(depth);
    postBreachFlags("depth");
    postEMessage(emsg);
  }
}

//-----------------------------------------------------------
// Procedure: altitudeVerify()
//      Note: Verify that the altitude limit hasn't been violated.
//    Return: void - An error message is communicated by a call to 
//            postEMessage().

void BHV_OpRegion::altitudeVerify()
{
  // If no min_altitude specified, return with no error message posted.
  if(m_min_altitude <= 0)
    return;

  bool ok;
  double curr_altitude = getBufferDoubleVal("NAV_ALTITUDE", ok);

  // Must get ownship altitude from info_buffer
  if(!ok) { 
    postEMessage("No ownship altitude in info_buffer.");
    return;
  }

  if(curr_altitude < m_min_altitude) {
    string emsg = "OpRegion Altitude failure: Min-Altitude:";
    emsg += doubleToString(m_min_altitude);
    emsg += "  Detected Altitude: ";
    emsg += doubleToString(curr_altitude);
    postBreachFlags("altitude");
    postEMessage(emsg);
  }
}

//-----------------------------------------------------------
// Procedure: timeoutVerify()
//      Note: Verify that the timeout limit hasn't been violated.
//    Return: void - An error message is communicated by a call to 
//            postEMessage().

void BHV_OpRegion::timeoutVerify()
{
  // If no max_time specified, return with no error message posted.
  if(m_max_time <= 0)
    return;
   
  if(m_elapsed_time > m_max_time) {
    string emsg = "OpRegion timeout failure: MaxTime:";
    emsg += doubleToString(m_max_time);
    emsg += "  Elapsed Time: ";
    emsg += doubleToString(m_elapsed_time);
    postBreachFlags("time");
    postEMessage(emsg);
  }
}


//-----------------------------------------------------------
// Procedure: postTimeRemaining()

void BHV_OpRegion::postTimeRemaining()
{
  if(m_time_remaining_var == "")
    return;

  if(m_max_time <= 0) 
    postMessage(m_time_remaining_var, -1);
   
  double time_remaining = m_max_time - m_elapsed_time;
  if(time_remaining < 0)
    time_remaining = 0;
  if(time_remaining > 10)
    postIntMessage(m_time_remaining_var, time_remaining);
  else
    postMessage(m_time_remaining_var, time_remaining);
}


//-----------------------------------------------------------
// Procedure: setTimeStamps()

void BHV_OpRegion::setTimeStamps()
{
  // Grab current time from Info Buffer
  m_current_time = getBufferCurrTime();
  
  //cout << "Current Time -    " << delta_time << endl;
  //cout << "Previous Time -    " << delta_time << endl;

  // Calculate the Delta time since this behavior was invoked.
  // The delta time is 0 on first invocation.
  if(m_first_time) {
    m_start_time = m_current_time;
    m_delta_time = 0;
    m_first_time = false;
  }
  else
    m_delta_time = m_current_time - m_previous_time;

  // No longer need to access previous time. Set it now for
  // access on the next invocation of this behavior.
  m_previous_time = m_current_time;

  m_elapsed_time = m_current_time - m_start_time;
}


//-----------------------------------------------------------
// Procedure: postViewablePolygon()
//      Note: Even if the polygon is posted on each iteration, the
//            helm will filter out unnecessary duplicate posts.

void BHV_OpRegion::postViewablePolygon()
{
  if(m_polygon.size() == 0)
    return;

  if(m_us_name == "")
    return;

  if(m_polygon.get_label() == "") {
    string label = m_us_name + ":" + m_descriptor + ":opreg";
    m_polygon.set_label(label);
  }

  XYPolygon poly_duplicate = m_polygon;
  if(m_hint_vertex_color != "")
    poly_duplicate.set_color("vertex", m_hint_vertex_color);
  if(m_hint_edge_color != "")
    poly_duplicate.set_color("edge", m_hint_edge_color);
  if(m_hint_label_color != "")
    poly_duplicate.set_color("label", m_hint_label_color);
  if(m_hint_edge_size >= 0)
    poly_duplicate.set_edge_size(m_hint_edge_size);
  if(m_hint_vertex_size >= 0)
    poly_duplicate.set_vertex_size(m_hint_vertex_size);

  string poly_spec = poly_duplicate.get_spec();
  postMessage("VIEW_POLYGON", poly_spec);
}

//-----------------------------------------------------------
// Procedure: postErasablePolygon()
//      Note: Even if the polygon is posted on each iteration, the
//            helm will filter out unnecessary duplicate posts.

void BHV_OpRegion::postErasablePolygon()
{
  if(m_polygon.size() == 0)
    return;
  XYPolygon poly_duplicate = m_polygon;
  poly_duplicate.set_active(false);
  string poly_spec = poly_duplicate.get_spec();
  postMessage("VIEW_POLYGON", poly_spec);
}

//-----------------------------------------------------------
// Procedure: checkForSoftPolyReset()

void BHV_OpRegion::checkForSoftPolyReset()
{
  // If soft_poly_breach is not even being used, just return
  if(!m_soft_poly_breach || !m_soft_poly_breach_posted)
    return;

  // If a soft_poly_breach is not currently in play
  if(!m_soft_poly_breach || !m_soft_poly_breach_posted)
    return;

  // Check value of MOOS_MANUAL_OVERRIDE. If it has been set to
  // false, then retract the posted Run Warning
  bool result;
  string sval = getBufferStringVal("MOOS_MANUAL_OVERRIDE", result);
  if(!result)
    return;

  if(tolower(sval) == "false") {
    postRetractWMessage("Soft polygon containment breach");
    m_soft_poly_breach_posted = false;
  }
}


//-----------------------------------------------------------
// Procedure: checkForReset()

void BHV_OpRegion::checkForReset()
{
  double time_since_reset = getBufferTimeVal(m_reset_var);
  if(time_since_reset != 0)
    return;
  
  bool result;
  string reset_str = getBufferStringVal(m_reset_var, result);
  if(!result)
    return;

  // reset_str if:
  // "true", then reset all components
  // "time", then reset the time component only
  // "poly", then reset the poly component only
  // "depth", then reset the depth component only
  // "altitude", then reset the altitude component only

  reset_str = tolower(reset_str);
  if((reset_str == "poly") || (reset_str == "true")) {
    m_breached_poly_flags_posted = false;
    m_secs_in_poly = 0;
    m_previously_in_poly = false;
    m_poly_entry_made = false;
  }
  if((reset_str == "time") || (reset_str == "true")) {
    m_first_time = true;  
    m_breached_time_flags_posted = false;
  }
  if((reset_str == "altitude") || (reset_str == "true"))
    m_breached_altitude_flags_posted = false;
  if((reset_str == "altitude") || (reset_str == "true"))
    m_breached_depth_flags_posted = false;
}

//-----------------------------------------------------------
// Procedure: postBreachFlags()

void BHV_OpRegion::postBreachFlags(string str)
{
  if(str == "poly") {
    if(!m_breached_poly_flags_posted) {
      m_breached_poly_flags_posted = true;
      postFlags(m_breached_poly_flags);
    }
  }
  else if(str == "time") {
    if(!m_breached_time_flags_posted) {
      m_breached_time_flags_posted = true;
      postFlags(m_breached_time_flags);
    }
  }
  else if(str == "altitude") {
    if(!m_breached_altitude_flags_posted) {
      m_breached_altitude_flags_posted = true;
      postFlags(m_breached_altitude_flags);
    }
  }
  else if(str == "depth") {
    if(!m_breached_depth_flags_posted) {
      m_breached_depth_flags_posted = true;
      postFlags(m_breached_depth_flags);
    }
  }

}

//-----------------------------------------------------------
// Procedure: handleVisualHint()

void BHV_OpRegion::handleVisualHint(string hint)
{
  string param = tolower(biteStringX(hint, '='));
  string value = hint;
  double dval  = atof(value.c_str());

  if((param == "vertex_color") && isColor(value))
    m_hint_vertex_color = value;
  else if((param == "edge_color") && isColor(value))
    m_hint_edge_color = value;
  else if((param == "label_color") && isColor(value))
    m_hint_label_color = value;
  else if((param == "edge_size") && isNumber(value) && (dval >= 0))
    m_hint_edge_size = dval;
  else if((param == "vertex_size") && isNumber(value) && (dval >= 0))
    m_hint_vertex_size = dval;
}





