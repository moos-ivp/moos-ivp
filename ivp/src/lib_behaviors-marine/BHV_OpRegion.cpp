/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_OpRegion.cpp                                     */
/*    DATE: May 1st, 2005 Sunday at Joe's in Maine               */
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
#include "BHV_OpRegion.h"
#include "MBUtils.h"
#include "XYBuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_OpRegion::BHV_OpRegion(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_opregion");
  max_depth    = 0;
  min_altitude = 0;
  max_time     = 0;

  // Keep track of whether the vehicle was in the polygon on the
  // previous invocation of the behavior. Initially assume false.
  previously_in_poly = false;

  // Keep a flag indicating whether this is the first time the
  // behavior is invoked.
  first_time   = true;

  // Time stamps for calculating how long the vehicle has been
  // inside or out of the polygon.
  previous_time = 0;
  current_time  = 0;
  elapsed_time  = 0;
  start_time    = 0;
  delta_time    = 0;
  secs_in_poly  = 0;
  secs_out_poly = 0;
  
  // Declare whether the polygon containment condition is effective
  // immediately (default) or triggered only when the vehicle has
  // first entered the polygon region. This is useful if the vehicle
  // is being launched from a dock or area which is outside the 
  // safe zone
  trigger_on_poly_entry = false;

  // Maintain a flag indicating whether the vehicle has entered
  // the polygon region. This value is only relevant if the 
  // trigger_on_poly_entry flag is set to be true.
  poly_entry_made = false;

  // Declare the amount of time required for the vehicle to be 
  // within the polygon region before the polygon containment 
  // condition is enforced. This value is only relevant if the 
  // trigger_on_poly_entry flag is set to be true.
  trigger_entry_time = 1.0;

  // Declare the amount of time required for the vehicle to be 
  // outside the polygon region before the polygon containment 
  // condition triggers a declaration of emergency. Setting this
  // to be non-zero may be useful if the position sensor (GPS) 
  // occasionally has a whacky single position reading outside
  // the polygon region.
  trigger_exit_time = 0.5;

  // Declare the variables we will need from the info_buffer
  addInfoVars("NAV_X, NAV_Y, NAV_HEADING");
  addInfoVars("NAV_SPEED, NAV_DEPTH, NAV_ALTITUDE");
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition,tag" where the tag is an optional
//            descriptor of the waypoint.
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_OpRegion::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  // Typical line: polygon  = 0,0:0,100:100,0:100,100
  if(param == "polygon") {
    XYPolygon new_poly = stringToPoly(val);
    if(!new_poly.is_convex())  // Should be convex - false otherwise
      return(false);
    polygon = new_poly;
    return(true);
  }
  else if(param == "max_depth") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    max_depth = dval;
    return(true);
  }
  else if(param == "min_altitude") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    min_altitude = dval;
    return(true);
  }
  else if(param == "max_time") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    max_time = dval;
    return(true);
  }
  else if(param == "trigger_entry_time") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    trigger_entry_time = dval;
    if(trigger_entry_time > 0)
      trigger_on_poly_entry = true;
    return(true);
  }
  else if(param == "trigger_exit_time") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    trigger_exit_time = dval;
    return(true);
  }
  return(false);
}

//-----------------------------------------------------------
// Procedure: onRunState
//     Notes: Always returns NULL, never returns an IvPFunction*
//     Notes: Sets state_ok = false and posts an error message if
//            any of the OpRegion conditions are not met.

IvPFunction *BHV_OpRegion::onRunState() 
{
  // Each of the below calls will check their critical conditions
  // and post an error message if a violation is detected. The call
  // to postEMessage() also sets state_ok = false;
  setTimeStamps();
  polygonVerify();
  postPolyStatus();
  depthVerify();
  altitudeVerify();
  timeoutVerify();
  
  return(0);
}

//-----------------------------------------------------------
// Procedure: polygonVerify()
//      Note: Verify that polygon boundary hasn't been violated.
//    Return: void - An error message is communicated by a call to 
//            postEMessage().

void BHV_OpRegion::polygonVerify()
{
  bool ok1, ok2;
  double osX = getBufferDoubleVal("NAV_X", ok1);
  double osY = getBufferDoubleVal("NAV_Y", ok2);

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2) {
    string msg = "No ownship info in info_buffer";
    postEMessage(msg);
    return;
  }
  
  bool contained = polygon.contains(osX, osY);

  // Determine the accumulated time within the polygon
  if(contained) {
    secs_out_poly = 0;
    if(previously_in_poly)
      secs_in_poly += delta_time;
    else
      secs_in_poly = 0;
    previously_in_poly = true;
  }
  // Determine the accumulated time outside the polygon
  if(!contained) {
    secs_in_poly = 0;
    if(!previously_in_poly)
      secs_out_poly += delta_time;
    else
      secs_out_poly = 0;
    previously_in_poly = false;
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
    if(secs_in_poly >= trigger_entry_time)
      poly_entry_made = true;
    return;
  }

  // Case 2: Vehicle not in polygon and no prior polygon
  //         entry is required for triggering emergency flag.
  //         Return based on accumulated time outside of poly.
  if(!contained && !trigger_on_poly_entry)
    if(secs_out_poly < trigger_exit_time)
      return;

  // Case 3: Vehicle not in polygon, poly entry is needed to
  //         trigger emergency, but no entry has been made yet.
  if(!contained && trigger_on_poly_entry && !poly_entry_made)
    return;

  // Case 4: Vehicle not in polygon, poly entry is needed to
  //         trigger emergency, and previous entry detected.
  //         Return based on accumulated time outside of poly.
  if(!contained && trigger_on_poly_entry && poly_entry_made)
    if(secs_out_poly < trigger_exit_time)
      return;


  // All verification cases failed. Post an error message and
  // return verification = false;
  string emsg = "BHV_OpRegion Polygon containment failure: ";
  emsg += " x=" + doubleToString(osX);
  emsg += " y=" + doubleToString(osY);
  postEMessage(emsg);
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

#if 0
  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2 || !ok3 || !ok4) {
    string msg = "No ownship info (" + m_us_name + ") in info_buffer";
    postEMessage(msg);
    return;
  }
#endif  

#if 0
  // Must get ownship position from InfoBuffer
  if(!ok1) {
    string msg = "No ownship NAV_X (" + m_us_name + ") in info_buffer";
    postEMessage(msg);
    return;
  }
#endif
  // Must get ownship position from InfoBuffer
  if(!ok2) {
    string msg = "No ownship NAV_Y (" + m_us_name + ") in info_buffer";
    postEMessage(msg);
    return;
  }
  // Must get ownship position from InfoBuffer
  if(!ok3) {
    string msg = "No ownship NAV_SPEED (" + m_us_name + ") in info_buffer";
    postEMessage(msg);
    return;
  }
  // Must get ownship position from InfoBuffer
  if(!ok4) {
    string msg = "No ownship NAV_HEADING (" + m_us_name + ") in info_buffer";
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
  double trajectory_perim_dist = polygon.dist_to_poly(osX, osY, osHDG);
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
  double absolute_perim_dist = polygon.dist_to_poly(osX, osY);
  double absolute_perim_eta  = 0;
  if(osTopSpeed > 0)
    absolute_perim_eta  = absolute_perim_dist / osTopSpeed;
  
  // post the distance at integer precision unless close to zero
  if(absolute_perim_dist <= 1)
    postMessage("OPREG_ABSOLUTE_PERIM_DIST", absolute_perim_dist);
  else
    postIntMessage("OPREG_ABSOLUTE_PERIM_DIST", absolute_perim_dist);
  postIntMessage("OPREG_ABSOLUTE_PERIM_ETA",  absolute_perim_eta);
  
  if(max_time > 0) {
    double remaining_time = max_time - elapsed_time;
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
  if(max_depth <= 0)
    return;

  bool ok;
  double depth = getBufferDoubleVal("NAV_DEPTH", ok);

  // Must get ownship depth from info_buffer
  if(!ok) { 
    //cout << "No NAV_DEPTH in info_buffer for vehicle: " << m_us_name << endl;
    postEMessage("No ownship depth in info_buffer.");
    //m_info_buffer->print();
    return;
  }

  if(depth > max_depth) {
    string emsg = "OpRegion Depth failure: max:" + doubleToString(max_depth);
    emsg += " detected:" + doubleToString(depth);
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
  if(min_altitude <= 0)
    return;

  bool ok;
  double curr_altitude = getBufferDoubleVal("NAV_ALTITUDE", ok);

  // Must get ownship altitude from info_buffer
  if(!ok) { 
    postEMessage("No ownship altitude in info_buffer.");
    return;
  }

  if(curr_altitude < min_altitude) {
    string emsg = "OpRegion Altitude failure: Min-Altitude:";
    emsg += doubleToString(min_altitude);
    emsg += "  Detected Altitude: ";
    emsg += doubleToString(curr_altitude);
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
  if(max_time <= 0)
    return;

  if(elapsed_time > max_time) {
    string emsg = "OpRegion timeout failure: MaxTime:";
    emsg += doubleToString(max_time);
    emsg += "  Elapsed Time: ";
    emsg += doubleToString(elapsed_time);
    postEMessage(emsg);
  }
}


//-----------------------------------------------------------
// Procedure: setTimeStamps()

void BHV_OpRegion::setTimeStamps()
{
  // Grab current time from Info Buffer
  current_time = getBufferCurrTime();
  
  //cout << "Current Time -    " << delta_time << endl;
  //cout << "Previous Time -    " << delta_time << endl;

  // Calculate the Delta time since this behavior was invoked.
  // The delta time is 0 on first invocation.
  if(first_time) {
    start_time = current_time;
    delta_time = 0;
    first_time = false;
  }
  else
    delta_time = current_time - previous_time;

  // No longer need to access previous time. Set it now for
  // access on the next invocation of this behavior.
  previous_time = current_time;

  elapsed_time = current_time - start_time;
}


