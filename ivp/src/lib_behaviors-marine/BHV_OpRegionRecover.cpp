/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_OpRegionRecover.cpp                              */
/*    DATE: Aug 10th, 2016                                       */
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
#include <cmath> 
#include <cstdlib>
#include "BHV_OpRegionRecover.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "XYFormatUtilsPoly.h"
#include "XYPolyExpander.h"
#include "ZAIC_Vector.h"
#include "ZAIC_SPD.h"
#include "BuildUtils.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_OpRegionRecover::BHV_OpRegionRecover(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  m_domain = subDomain(m_domain, "course,speed");

  m_descriptor        = "op_recover";
  m_hint_vertex_size  = 1;
  m_hint_vertex_color = "brown"; 
  m_hint_edge_size    = 1;
  m_hint_edge_color   = "aqua"; 

  m_buffer_dist = 0;
  
  // Be explicit about this variable is empty string by default
  m_opregion_poly_var = "";

  // Keep track of whether the vehicle was in the polygon on the
  // previous invocation of the behavior. Initially assume false.
  m_previously_in_poly = false;

  // Time stamps for calculating how long the vehicle has been
  // inside or out of the polygon.
  m_time_entered_poly = 0;
  m_time_exited_poly  = 0;
  m_secs_in_poly      = 0;
  m_secs_out_poly     = 0;
  
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

  m_committed_turn = "none";  // Always none, left, or right
  
  // Declare the variables we will need from the info_buffer
  addInfoVars("NAV_X, NAV_Y, NAV_HEADING, NAV_SPEED");
}

//-----------------------------------------------------------
// Procedure: setParam()

bool BHV_OpRegionRecover::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  // Typical line: polygon  = 0,0:0,100:100,0:100,100
  if(param == "polygon") {
    XYPolygon new_poly = string2Poly(val);
    if(!new_poly.is_convex())  // Should be convex - false otherwise
      return(false);
    m_polygon_core = new_poly;
    return(true);
  }
  else if(param == "reset_var") {
    if(strContainsWhite(val))
      return(false);
    m_reset_var = val;
    addInfoVars(m_reset_var);
    return(true);
  }
  else if(param == "dynamic_region_var") {
    if(strContainsWhite(val))
      return(false);
    m_dynamic_region_var = val;
    addInfoVars(m_dynamic_region_var);
    return(true);
  }
  else if(param == "buffer_dist") {
    bool handled = setNonNegDoubleOnString(m_buffer_dist, val);
    return(handled);
  }
  else if(param == "trigger_entry_time") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    m_trigger_entry_time = dval;
    if(m_trigger_entry_time > 0)
      m_trigger_on_poly_entry = true;
    return(true);
  }
  else if(param == "trigger_exit_time") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    m_trigger_exit_time = dval;
    return(true);
  }
  else if(param == "opregion_poly_var") 
    return(setNonWhiteVarOnString(m_opregion_poly_var, val));
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

void BHV_OpRegionRecover::onSetParamComplete()
{
  m_polygon_buff = m_polygon_core;
  if(m_polygon_core.is_convex() && (m_buffer_dist > 0)) {
    XYPolyExpander expander;
    expander.setPoly(m_polygon_core);
    expander.setDegreeDelta(15);
    m_polygon_buff = expander.getBufferPoly(m_buffer_dist);
  }
  
  if(m_opregion_poly_var != "") {
    if(m_polygon_buff.size() > 0) {
      string spec = m_polygon_buff.get_spec_pts(2);
      postMessage(m_opregion_poly_var, spec);
    }
  }

  postConfigStatus();
}

//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_OpRegionRecover::onIdleState() 
{
  checkForReset();
  postErasablePolygon();
}

//-----------------------------------------------------------
// Procedure: onRunState
//     Notes: Always returns NULL, never returns an IvPFunction*
//     Notes: Sets state_ok = false and posts an error message if
//            any of the OpRegionRecover conditions are not met.

IvPFunction *BHV_OpRegionRecover::onRunState() 
{
  bool ok = updateInfoIn();
  if(!ok) {
    postMessage("OPR_DEBUG", "bad info");
    return(0);
  }

  // Do nothing unless our working poly (polygon_buff) is convex
  if(!m_polygon_buff.is_convex())
    return(0);

  checkForReset();

  bool in_poly = polygonVerify();
  if(in_poly) {
    m_committed_turn = "none";
    m_heading_to_poly = false;
    postPolyStatus();
    postViewablePolygon();
    postRepeatableMessage("OPR_DEBUG", "in poly no problemo");
    return(0);
  }

  updateRangeCache();

  postRepeatableMessage("OPR_DEBUG", "not in poly");
  // Step 1: are we heading toward the poly?
  // Note we don't set to false if currently not heading to poly. Once
  // set to true it stays true until we get back inside the poly. 
  double curr_hdg_dist_to_poly = m_polygon_buff.dist_to_poly(m_osx, m_osy, m_osh);
  postRepeatableMessage("OPR_HDG_DIST_TO_POLY", curr_hdg_dist_to_poly);
  if(curr_hdg_dist_to_poly != -1) {
    m_heading_to_poly = true;
    m_committed_turn = "none";
  }
  // If NOT heading to the poly, stay committed to a turn. If not presently
  // committed to a turn, figure out which turn to commit to.
  else {
    if(m_committed_turn == "none")
      m_committed_turn = determineInitialTurn();
  }
  postRepeatableMessage("OPR_TURN", m_committed_turn);
  
  //=========================================================
  // Step 2A: Create the HEADING ZAIC
  //=========================================================
  ZAIC_Vector zaic_hdg(m_domain, "course");
  zaic_hdg.setDomainVals(m_heading_cache);
  zaic_hdg.setTolerance(0.5);
  if(m_committed_turn == "left")
    zaic_hdg.setRangeVals(m_range_cache_norm_no_rgt);
  else if(m_committed_turn == "right")
    zaic_hdg.setRangeVals(m_range_cache_norm_no_lft);
  else
    zaic_hdg.setRangeVals(m_range_cache_norm);

  IvPFunction *ipf_hdg = zaic_hdg.extractIvPFunction();
  if(!ipf_hdg) {
    postRepeatableMessage("OPR_ZAIC_HDG_ERR", zaic_hdg.getErrors());
    return(0);
  }
    
  //=========================================================
  // Step 2B: Create the SPEED ZAIC
  //=========================================================
  ZAIC_SPD zaic_spd(m_domain, "speed");
  zaic_spd.setMedSpeed(0.3);
  zaic_spd.setLowSpeed(0.1);
  zaic_spd.setHghSpeed(0.5);
  zaic_spd.setMinMaxUtil(80, 30, 100);

  IvPFunction *ipf_spd = zaic_spd.extractIvPFunction();
  if(!ipf_spd) {
    postRepeatableMessage("OPR_ZAIC_SPD_ERR", zaic_spd.getWarnings());
    return(0);
  }

  OF_Coupler coupler;
  IvPFunction *ipf = coupler.couple(ipf_hdg, ipf_spd, 50, 50);
  if(!ipf)
    postWMessage("Failure on the CRS_SPD COUPLER");
  else
    ipf->setPWT(m_priority_wt);
  
  postPolyStatus();
  postViewablePolygon();
  return(ipf);
}

//-----------------------------------------------------------
// Procedure: polygonVerify()
//      Note: Verify that polygon boundary hasn't been violated.
//    Return: true if the vehicle is considered to be "in" the polygon.
//            Note it may be consider "in" when in fact it is outside, 
//            depending on the trigger_entry_time and trigger_exit_time
//            parameter settings.

bool BHV_OpRegionRecover::polygonVerify()
{
  if(m_polygon_buff.size() == 0)
    return(false);

  bool contained = m_polygon_buff.contains(m_osx, m_osy);

  // Determine the accumulated time within the polygon
  if(contained) {
    if(!m_previously_in_poly) {
      m_time_entered_poly = getBufferCurrTime();
      m_time_exited_poly = 0;
      m_secs_out_poly = 0;
      m_secs_in_poly  = 0;
    }      
    else 
      m_secs_in_poly = getBufferCurrTime() - m_time_entered_poly;
    m_previously_in_poly = true;
  }
  // Determine the accumulated time outside the polygon
  if(!contained) {
    if(m_previously_in_poly) {
      m_time_exited_poly  = getBufferCurrTime();
      m_time_entered_poly = 0;
      m_secs_out_poly = 0;
      m_secs_in_poly  = 0;
    }
    else
      m_secs_out_poly = getBufferCurrTime() - m_time_exited_poly;
    m_previously_in_poly = false;
  }

  // Case 1: Vehicle in polygon. Check to see if its been
  //         in the poly long enough to be considered an 
  //         official entry into the polygon.

  postRepeatableMessage("OPR_SECS_IN_POLY", m_secs_in_poly);
  if(contained) {
    if(m_secs_in_poly >= m_trigger_entry_time)
      m_poly_entry_made = true;
    return(true);
  }

  // Case 2: Vehicle not in polygon and no prior polygon
  //         entry is required for triggering emergency flag.
  //         Return based on accumulated time outside of poly.
  if(!contained && !m_trigger_on_poly_entry)
    if(m_secs_out_poly < m_trigger_exit_time) {
      return(true);
    }

  // Case 3: Vehicle not in polygon, poly entry is needed to
  //         trigger emergency, but no entry has been made yet.
  if(!contained && m_trigger_on_poly_entry && !m_poly_entry_made) {
    return(true);
  }

  // Case 4: Vehicle not in polygon, poly entry is needed to
  //         trigger emergency, and previous entry detected.
  //         Return based on accumulated time outside of poly.
  if(!contained && m_trigger_on_poly_entry && m_poly_entry_made) {
    if(m_secs_out_poly < m_trigger_exit_time) {
      return(true);
    }
  }

  return(false);
}

//-----------------------------------------------------------
// Procedure: updateInfoIn()

bool BHV_OpRegionRecover::updateInfoIn()
{
  bool ok1, ok2, ok3, ok4;
  double osx = getBufferDoubleVal("NAV_X", ok1);
  double osy = getBufferDoubleVal("NAV_Y", ok2);
  double osv = getBufferDoubleVal("NAV_SPEED", ok3);
  double osh = getBufferDoubleVal("NAV_HEADING", ok4);

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
    return(false);
  }

  m_osx = osx;
  m_osy = osy;
  m_osv = osv;
  m_osh = osh;

  if(m_dynamic_region_var != "") {
    if(getBufferVarUpdated(m_dynamic_region_var)) {
      bool ok;
      string new_poly_core = getBufferStringVal(m_dynamic_region_var, ok);
      XYPolygon new_poly = string2Poly(new_poly_core);
      if(new_poly.is_convex()) {
	m_polygon_core = new_poly;
	m_polygon_buff = m_polygon_core;
	if(m_buffer_dist > 0) {
	  XYPolyExpander expander;
	  expander.setPoly(m_polygon_core);
	  expander.setDegreeDelta(15);
	  m_polygon_buff = expander.getBufferPoly(m_buffer_dist);
	}
      }
    }
  }

  return(true);
}


//-----------------------------------------------------------
// Procedure: postPolyStatus()
//   Purpose: To calculate information about impending 
//            OpRegionRecover violations. It calculates and 
//            posts the following variables:
//        (1) OPR_TRAJECTORY_PERIM_ETA: (double) 
//            The time in seconds until the vehicle exits the 
//            polygon containment region if it stays on the 
//            current trajectory.
//        (2) OPR_TRAJECTORY_PERIM_DIST: (double)
//            Distance in meters until the vehicle exits the 
//            polygon containment region if it stays on the 
//            current trajectory.
//        (3) OPR_ABSOLUTE_PERIM_ETA: (double) 
//            Time in seconds until the vehicle would exit the
//            polygon if it were to take the shortest path at
//            top vehicle speed.
//        (4) OPR_ABSOLUTE_PERIM_DIST: (double) 
//            Distance in meters between the vehicle and the 
//            polygon perimeter regardless of current heading
//        (5) OPR_TIME_REMAINING: (double)
//            Time in seconds before a OpRegionRecover timeout would
//            occur. If max_time=0, then no such message posted.

void BHV_OpRegionRecover::postPolyStatus()
{
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
  double trajectory_perim_dist = m_polygon_buff.dist_to_poly(m_osx, m_osy, m_osh);
  double trajectory_perim_eta = 0;
  if(m_osv > 0)
    trajectory_perim_eta = trajectory_perim_dist / m_osv;
  
  // post the distance at integer precision unless close to zero
  if(trajectory_perim_dist <= 1)
    postMessage("OPR_TRAJECTORY_PERIM_DIST", trajectory_perim_dist);
  else  
    postIntMessage("OPR_TRAJECTORY_PERIM_DIST", trajectory_perim_dist);
  postIntMessage("OPR_TRAJECTORY_PERIM_ETA",  trajectory_perim_eta);
  
  // Calculate the absolute (ABS) distance and ETA to the perimeter.
  double absolute_perim_dist = m_polygon_buff.dist_to_poly(m_osx, m_osy);
  double absolute_perim_eta  = 0;
  if(osTopSpeed > 0)
    absolute_perim_eta  = absolute_perim_dist / osTopSpeed;
  
  // post the distance at integer precision unless close to zero
  if(absolute_perim_dist <= 1)
    postMessage("OPR_ABSOLUTE_PERIM_DIST", absolute_perim_dist);
  else
    postIntMessage("OPR_ABSOLUTE_PERIM_DIST", absolute_perim_dist);
  postIntMessage("OPR_ABSOLUTE_PERIM_ETA",  absolute_perim_eta);
  
}

//-----------------------------------------------------------
// Procedure: postViewablePolygon()
//      Note: Even if the polygon is posted on each iteration, the
//            helm will filter out unnecessary duplicate posts.

void BHV_OpRegionRecover::postViewablePolygon()
{
  if(m_polygon_buff.size() == 0)
    return;

  if(m_us_name == "")
    return;

  if(m_polygon_buff.get_label() == "") {
    string label = m_us_name + ":" + m_descriptor + ":opreg";
    m_polygon_buff.set_label(label);
  }

  XYPolygon poly_duplicate = m_polygon_buff;
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

void BHV_OpRegionRecover::postErasablePolygon()
{
  if(m_polygon_buff.size() == 0)
    return;
  XYPolygon poly_duplicate = m_polygon_buff;
  poly_duplicate.set_active(false);
  string poly_spec = poly_duplicate.get_spec();
  postMessage("VIEW_POLYGON", poly_spec);
}

//-----------------------------------------------------------
// Procedure: checkForReset()

void BHV_OpRegionRecover::checkForReset()
{
  double time_since_reset = getBufferTimeVal(m_reset_var);
  if(time_since_reset != 0)
    return;
  
  bool result;
  string reset_str = getBufferStringVal(m_reset_var, result);
  if(!result)
    return;
  
  if(tolower(reset_str == "true")) {
    m_secs_in_poly = 0;
    m_previously_in_poly = false;
    m_poly_entry_made = false;
  }
}

//-----------------------------------------------------------
// Procedure: updateRangeCache()

void BHV_OpRegionRecover::updateRangeCache()
{
  // Step 1: Clear the current caches and sanity check
  m_heading_cache.clear();
  m_range_cache.clear();
  m_range_cache_norm.clear();
  m_range_cache_norm_no_lft.clear();
  m_range_cache_norm_no_rgt.clear();
  if(m_polygon_buff.size() == 0)
    return;
  
  // Step 2: Get the IvP Domain index for later use. Most likely the 
  // "heading" domain will be called "course", but we handle the 
  // case where it it actually "heading".
  int hdg_ix = m_domain.getIndex("course");
  if(hdg_ix == -1)
    hdg_ix = m_domain.getIndex("heading");
  if(hdg_ix == -1)
    return;
  
  // Step 3: Get the number of heading domain values, and initialize
  // all the caches.
  unsigned int hdg_pts = m_domain.getVarPoints(hdg_ix);
  vector<double> new_heading_cache(hdg_pts, 0);
  vector<double> new_range_cache(hdg_pts, -1);
  vector<double> new_range_cache_norm(hdg_pts, 0);
  vector<double> new_range_cache_norm_no_lft(hdg_pts, 0);
  vector<double> new_range_cache_norm_no_rgt(hdg_pts, 0);

  m_heading_cache    = new_heading_cache;
  m_range_cache      = new_range_cache;
  m_range_cache_norm = new_range_cache_norm;
  m_range_cache_norm_no_lft = new_range_cache_norm_no_lft;
  m_range_cache_norm_no_rgt = new_range_cache_norm_no_rgt;

  // Step 4. Fill in the heading and range caches by calculating the 
  // distance to the containent polygon for each possible heading.
  for(unsigned int i=0; i<hdg_pts; i++) {
    double heading = m_domain.getVal(hdg_ix, i);
    double dist_to_region = m_polygon_buff.dist_to_poly(m_osx, m_osy, heading);
    m_heading_cache[i] = heading;
    m_range_cache[i]   = dist_to_region;
  }

  // Step 5. Normalize the values for all headings that reach the poly.
  // Longest distance will be zero, closest distance will be 100.
  // Step 5A: First find the min/max values
  double closest_dist = -1;
  double farthest_dist = -1;
  for(unsigned int i=0; i<hdg_pts; i++) {
    if(m_range_cache[i] != -1) {
      if((closest_dist == -1) || (m_range_cache[i] < closest_dist))
	closest_dist = m_range_cache[i];
      if((farthest_dist == -1) || (m_range_cache[i] > farthest_dist))
	farthest_dist = m_range_cache[i];
    }
  }
  // Step 5B: Sanity checks
  if((closest_dist == -1) || (farthest_dist == -1))
    return;
  if(closest_dist >= farthest_dist)
    return;
  double range_of_vals = farthest_dist - closest_dist;

  // Step 5D: Calculate the normalized values to the range [0, 100]
  for(unsigned int i=0; i<hdg_pts; i++) {
    // For headings that dont intersect the polygon, set norm values to zero
    if(m_range_cache[i] == -1)
      m_range_cache_norm[i] = 0;
    // Otherwise normalize
    else
      m_range_cache_norm[i] = 100 * (farthest_dist - m_range_cache[i]) / range_of_vals;
  }

  // Step 6: Build a cache where all left turns are masked out
  for(unsigned int i=0; i<hdg_pts; i++) {
    bool left_turn = false;
    double hdg = m_heading_cache[i];
    if((m_osh > hdg) && ((m_osh-hdg) < 180))
      left_turn = true;
    if((hdg > m_osh) && (m_osh < 180) && (hdg > (360 - (180-m_osh))))
      left_turn = true;
    if(left_turn)
      m_range_cache_norm_no_lft[i] = 0;
    else
      m_range_cache_norm_no_lft[i] = m_range_cache_norm[i];
  }

  // Step 7: Build a cache where all right turns are masked out
  for(unsigned int i=0; i<hdg_pts; i++) {
    bool right_turn = false;
    double hdg = m_heading_cache[i];
    if((m_osh < hdg) && ((hdg-m_osh) < 180))
      right_turn = true;
    if((hdg < m_osh) && (m_osh > 180) && (hdg < (m_osh-180)))
      right_turn = true;
    if(right_turn)
      m_range_cache_norm_no_rgt[i] = 0;
    else
      m_range_cache_norm_no_rgt[i] = m_range_cache_norm[i];
  }
}

//-----------------------------------------------------------
// Procedure: determineInitialTurn()

string BHV_OpRegionRecover::determineInitialTurn()
{
  // First determine which of all headings that would reach the
  // polygon, which heading is closest to ownship's current hdg.
  double lowest_delta = -1;
  double lowest_delta_hdg = 0;
  for(unsigned int i=0; i<m_heading_cache.size(); i++) {
    if(m_range_cache[i] != -1) {
      double hdg_i = m_heading_cache[i];
      double delta = angleDiff(m_osh, hdg_i);
      if((lowest_delta == -1) || (delta < lowest_delta)) {
	lowest_delta = delta;
	lowest_delta_hdg = hdg_i;
      }
    }
  }

  string result;
  double hdg = lowest_delta_hdg;

  // Determine if the lowest_delta_hdg is a right or left turn
  if(m_osh < hdg) {
    if((hdg - m_osh) < 180) 
      result = "right";
    else
      result = "left";
  }
  else {
    if((m_osh - hdg) < 180)
      result = "left";
    else
      result = "right";
  }

  return(result);
}

//-----------------------------------------------------------
// Procedure: handleVisualHint()

void BHV_OpRegionRecover::handleVisualHint(string hint)
{
  string param = tolower(stripBlankEnds(biteString(hint, '=')));
  string value = stripBlankEnds(hint);
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

