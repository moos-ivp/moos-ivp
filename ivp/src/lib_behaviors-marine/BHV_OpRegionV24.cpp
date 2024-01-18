/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_OpRegionV24.cpp                                  */
/*    DATE: Nov 24th, 2023                                       */
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
#include "BHV_OpRegionV24.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "VarDataPairUtils.h"
#include "XYFormatUtilsPoly.h"
#include "MacroUtils.h"
#include "XYPolyExpander.h"
#include "ZAIC_Vector.h"
#include "ZAIC_SPD.h"
#include "ZAIC_PEAK.h"
#include "BuildUtils.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Constructor()

BHV_OpRegionV24::BHV_OpRegionV24(IvPDomain gdomain) :
  IvPBehavior(gdomain)
{
  m_domain = subDomain(m_domain, "course,speed");  
  m_descriptor = "opregx";

  // ==================================================
  // PART 1: Init config vars
  // ==================================================
  m_max_depth = 0;
  m_min_alt   = 0;
  m_max_time  = 0;
  m_save_dist = -1; // -1 means not used
  m_halt_dist = -1; // -1 means not used

  // Visual Hint Defaults for the leg/turn paths
  m_hints.setMeasure("vertex_size", 0);
  m_hints.setMeasure("edge_size", 1);
  m_hints.setColor("vertex_color", "gray50");
  m_hints.setColor("edge_color", "gray50");
  m_hints.setColor("label_color", "off");
  m_hints.setColor("core_edge_color", "gray30");
  m_hints.setColor("save_edge_color", "gray50");
  m_hints.setColor("halt_edge_color", "pink");

  m_hints.setColor("save_status_edge_color", "yellow");
  m_hints.setColor("halt_status_edge_color", "red");
  
  // Declare if halt_poly is enabled immediately (default) or
  // triggered only when the vehicle first enters halt_poly.
  // Useful if vehicle/mission launched from outside halt_poly.
  m_trigger_on_poly_entry = true;

  // Time required for the vehicle to be within the halt_poly 
  // before halt_poly condition is enforced. Only relevant 
  // if the trigger_on_poly_entry flag is true.
  m_trigger_entry_time = 1.0;

  // Time required for vehicle to be out of halt_poly before poly
  // containment condition triggers a declaration of emergency.
  m_trigger_exit_time = 0.5;

  m_draw_save_status = true;
  m_draw_halt_status = true;
  
  // ==================================================
  // PART 2: Init state vars
  // ==================================================
  m_start_time   = 0; // For enforcing max_time
  
  // Time stamps for calculating how long the vehicle has been
  // inside or out of the polygon.
  m_time_entered_halt_poly = 0;
  m_time_exited_halt_poly  = 0;
  m_secs_in_halt_poly      = 0;
  m_secs_out_halt_poly     = 0;
  m_halt_poly_entry_made   = false;

  m_time_exited_save_poly  = 0;
  m_secs_out_save_poly     = 0;
  m_save_poly_entry_made   = false;
  m_save_flags_posted      = false;
  
  // Maintain bool, true if vehicle has entered halt_poly. 
  // Only relevant if trigger_on_poly_entry is set to true.

  m_committed_turn = "none";  // Always none, left, or right

  // Declare the variables we will need from the info_buffer
  addInfoVars("NAV_X, NAV_Y, NAV_HEADING, NAV_SPEED");
}

//-----------------------------------------------------------
// Procedure: setParam()

bool BHV_OpRegionV24::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  if(param == "core_poly")
    return(setPolyOnString(m_core_poly, val, "core" + m_us_name));
  else if(param == "save_poly")
    return(setPolyOnString(m_save_poly, val, "save" + m_us_name));
  else if(param == "halt_poly")
    return(setPolyOnString(m_halt_poly, val, "halt" + m_us_name));
  else if(param == "save_dist")
    return(setNonNegDoubleOnString(m_save_dist, val));
  else if(param == "halt_dist")
    return(setNonNegDoubleOnString(m_halt_dist, val));
  else if(param == "max_depth") 
    return(setNonNegDoubleOnString(m_max_depth, val));
  else if(param == "min_altitude") 
    return(setNonNegDoubleOnString(m_min_alt, val));
  else if(param == "max_time")
    return(setNonNegDoubleOnString(m_max_time, val));
  else if(param == "reset")
    return(handleConfigReset(val));
  else if(param == "trigger_on_poly_entry") 
    return(setBooleanOnString(m_trigger_on_poly_entry, val));
  else if(param == "trigger_entry_time") 
    return(setNonNegDoubleOnString(m_trigger_entry_time, val));
  else if(param == "trigger_exit_time") 
    return(setNonNegDoubleOnString(m_trigger_exit_time, val));
  else if(param == "breached_poly_flag") 
    return(addVarDataPairOnString(m_breached_poly_flags, val));
  else if(param == "breached_time_flag") 
    return(addVarDataPairOnString(m_breached_time_flags, val));
  else if(param == "breached_altitude_flag") 
    return(addVarDataPairOnString(m_breached_altitude_flags, val));
  else if(param == "breached_depth_flag") 
    return(addVarDataPairOnString(m_breached_depth_flags, val));
  else if(param == "save_flag") 
    return(addVarDataPairOnString(m_save_flags, val));
  else if(param == "savex_flag") 
    return(addVarDataPairOnString(m_savex_flags, val));
  else if(param == "draw_save_status") 
    return(setBooleanOnString(m_draw_save_status, val));
  else if(param == "draw_halt_status") 
    return(setBooleanOnString(m_draw_halt_status, val));
  else if(param == "visual_hints") 
    return(m_hints.setHints(val));
  return(false);
}

//-----------------------------------------------------------
// Procedure: onSetParamComplete()

void BHV_OpRegionV24::onSetParamComplete()
{
  updateRegionPolys();
    
  if(m_start_time == 0)
    m_start_time = getBufferCurrTime();

  postViewablePolys();
  postConfigStatus();
}

//-----------------------------------------------------------
// Procedure: onRunState()
//     Notes: Always returns NULL, never returns an IvPFunction*
//     Notes: Sets state_ok = false and posts an error message if
//            any of the OpRegionV24 conditions are not met.

IvPFunction *BHV_OpRegionV24::onRunState() 
{
  // Sanity check
  if(!updateInfoIn())
      return(0);

  // Part 1: Check for halt violations. If any violation, posted
  // err msg will result in an all_stop, so just return null her
  bool ok_dep = depthVerify();
  bool ok_alt = altitudeVerify();
  bool ok_time = timeoutVerify();
  bool ok_poly = haltPolyVerify();

  if(!ok_dep || !ok_alt || !ok_time || !ok_poly)
    return(0);

  // Part 2A: If not breaching save_poly, then just simply done.
  if(m_save_poly.contains(m_osx, m_osy)) {
    m_committed_turn = "none";
    m_heading_to_poly = false;
    m_save_flags_posted = false;
    m_secs_out_save_poly = 0;
    if(m_save_poly.contains(m_osx, m_osy))
      m_save_poly_entry_made = true;
    XYPolygon poly(m_osx, m_osy, 4, 12, m_us_name + "_stat");
    postMessage("VIEW_POLYGON", poly.get_spec_inactive());
    return(0);
  }

  // Part 2B: If outside the save poly but we have never been
  // within, just simply done.
  if(!m_save_poly_entry_made)
    return(0);
  
  // Part 3: No halt violations, but we are outside the save_poly,
  // so we generate an IPF to influence the vehicle back in.

  // Check and handle if first iteration outside of the save poly
  if(!m_save_flags_posted) {
    m_save_flags_posted = true;
    m_time_exited_save_poly = getBufferCurrTime();
    if(m_save_poly_entry_made)
      postFlags(m_save_flags);
  }
  // Update 
  m_secs_out_save_poly = getBufferCurrTime() - m_time_exited_save_poly;
  if(m_save_poly_entry_made)
    postFlags(m_savex_flags);


  if(m_draw_save_status && m_save_poly_entry_made) {
    XYPolygon poly(m_osx, m_osy, 4, 12, m_us_name + "_stat");
    poly.set_duration(2);
    applyHints(poly, m_hints, "save_status");
    postMessage("VIEW_POLYGON", poly.get_spec());
  }

  IvPFunction *ipf = buildOF();

  postViewablePolys();

  return(ipf);
}
  
//-----------------------------------------------------------
// Procedure: buildOF()

IvPFunction *BHV_OpRegionV24::buildOF()
{
  //=========================================================
  // Part 1: Preliminaries
  //=========================================================
  updateRangeCache();

  // Are we heading toward the poly?
  // Note we don't set to false if currently not heading to poly. Once
  // set to true it stays true until we get back inside the poly. 

  double curr_hdg_dist_to_poly = m_save_poly.dist_to_poly(m_osx, m_osy, m_osh);
  if(curr_hdg_dist_to_poly != -1) {
    m_heading_to_poly = true;
    m_committed_turn = "none";
  }
  // If NOT heading to save_poly, stay committed to a turn. If not now
  // committed to a turn, figure out which turn to commit to.
  else {
    if(m_committed_turn == "none")
      m_committed_turn = determineInitialTurn();
  }

  postRepeatableMessage("COMMITTED", m_committed_turn);
  
  //=========================================================
  // Part 2: Create the HEADING ZAIC
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
  // Part 3: Create the SPEED ZAIC
  //=========================================================
#if 1
  ZAIC_PEAK zaic_spd(m_domain, "speed");
  double cruise_speed = 1.0;
  double peak_width = cruise_speed / 2;
  //  zaic_spd.setParams(cruise_speed, peak_width, 1.6, 20, 0, 100);
  zaic_spd.setParams(cruise_speed, peak_width, 0.2, 20, 0, 100);
#endif

#if 0
  ZAIC_SPD zaic_spd(m_domain, "speed");

  zaic_spd.setMedSpeed(1.4);
  zaic_spd.setLowSpeed(1.2);
  zaic_spd.setHghSpeed(1.6);  
  
  zaic_spd.setMinMaxUtil(80, 30, 100);
#endif


  IvPFunction *ipf_spd = zaic_spd.extractIvPFunction();
  if(!ipf_spd) {
    postRepeatableMessage("OPR_ZAIC_SPD_ERR", zaic_spd.getWarnings());
    return(0);
  }

  OF_Coupler coupler;

  IvPFunction *ipf = 0;

  if((m_helm_iter % 5) == 0)
    ipf = coupler.couple(ipf_hdg, ipf_spd, 95, 5);
  else if((m_helm_iter % 5) == 1)
    ipf = coupler.couple(ipf_hdg, ipf_spd, 80, 20);
  else if((m_helm_iter % 5) == 2)
    ipf = coupler.couple(ipf_hdg, ipf_spd, 50, 50);
  else if((m_helm_iter % 5) == 3)
    ipf = coupler.couple(ipf_hdg, ipf_spd, 20, 80);
  else if((m_helm_iter % 5) == 4)
    ipf = coupler.couple(ipf_hdg, ipf_spd, 5, 95);
  

#if 0
  if(m_heading_to_poly)
    ipf = coupler.couple(ipf_hdg, ipf_spd, 95, 5);
  else
    ipf = coupler.couple(ipf_hdg, ipf_spd, 50, 50);
#endif
  
  if(!ipf)
    postWMessage("Failure on the CRS_SPD COUPLER");
  else
    ipf->setPWT(m_priority_wt);
  
  return(ipf);
}

//-----------------------------------------------------------
// Procedure: haltVerify()
//      Note: Verify halt_poly hasn't been violated.
//    Return: true if the vehicle is considered to be "in" the polygon.
//            Note it may be consider "in" when in fact it is outside, 
//            depending on the trigger_entry_time and trigger_exit_time
//            parameter settings.

bool BHV_OpRegionV24::haltPolyVerify()
{
  bool contained = m_halt_poly.contains(m_osx, m_osy);
  
  if(contained) {
    // Determine accumulated time within the polygon
    if(m_time_entered_halt_poly == 0)
      m_time_entered_halt_poly = getBufferCurrTime();
    
    m_secs_in_halt_poly = getBufferCurrTime() - m_time_entered_halt_poly;

    // Check if in poly long enough to be official entry
    if(m_secs_in_halt_poly >= m_trigger_entry_time)
      m_halt_poly_entry_made = true;

    m_time_exited_halt_poly = 0;
    m_secs_out_halt_poly = 0;
    return(true);
  }
  else {
    // Determine accumulated time outside the polygon
    if(m_time_exited_halt_poly == 0)
      m_time_exited_halt_poly = getBufferCurrTime();
    
    m_secs_out_halt_poly = getBufferCurrTime() - m_time_exited_halt_poly;
    m_time_entered_halt_poly = 0;
    m_secs_in_halt_poly = 0;
  }    

  // Case 1: Vehicle not in polygon and no prior polygon
  //         entry is required for triggering emergency flag.
  //         Return based on accumulated time outside of poly.
  if(!contained && !m_trigger_on_poly_entry) {
    if(m_secs_out_halt_poly < m_trigger_exit_time) {
      return(true);
    }
  }

  // Case 2: Vehicle not in polygon, poly entry is needed to
  //         trigger emergency, but no entry has been made yet.
  if(!contained && m_trigger_on_poly_entry && !m_halt_poly_entry_made)
    return(true);

  // Case 3: Vehicle not in polygon, poly entry is needed to
  //         trigger emergency, and previous entry detected.
  //         Return based on accumulated time outside of poly.
  if(!contained && m_trigger_on_poly_entry && m_halt_poly_entry_made) {
    if(m_secs_out_halt_poly < m_trigger_exit_time) 
      return(true);
  }

  if(m_draw_halt_status) {
    XYPolygon poly(m_osx, m_osy, 4, 8, m_us_name + "_stat");
    applyHints(poly, m_hints, "halt_status");
    postMessage("VIEW_POLYGON", poly.get_spec());
  }
  
  postFlags(m_breached_poly_flags);
  postEMessage("OpRegion HaltPoly Failure");
  return(false);
}

//-----------------------------------------------------------
// Procedure: updateInfoIn()

bool BHV_OpRegionV24::updateInfoIn()
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

  return(true);
}


//-----------------------------------------------------------
// Procedure: updateRegionPolys()

bool BHV_OpRegionV24::updateRegionPolys()
{
  // Sanity check: If save_dist or halt_dist is being used
  // then core_poly must have been provided and be convex.
  if((m_save_dist >= 0) || (m_halt_dist >= 0)) {
    if(!m_core_poly.is_convex())
      return(false);
  }
  
  if((m_save_dist >= 0)) {
    if(m_save_dist == 0)
      m_save_poly = m_core_poly;
    else {
      XYPolyExpander expander;
      expander.setPoly(m_core_poly);
      expander.setDegreeDelta(10);
      m_save_poly = expander.getBufferPoly(m_save_dist);
      m_save_poly.set_label("opreg_save_" + m_us_name);
    }
  }
  
  if(m_halt_dist >= 0) {
    if(m_halt_dist == 0)
      m_halt_poly = m_core_poly;
    else {
      XYPolyExpander expander;
      expander.setPoly(m_core_poly);
      expander.setDegreeDelta(10);
      m_halt_poly = expander.getBufferPoly(m_halt_dist);
      m_halt_poly.set_label("opreg_halt_" + m_us_name);
    }
  }
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: postViewablePolys()
//      Note: Even if the polygon is posted on each iteration, the
//            helm will filter out unnecessary duplicate posts.

void BHV_OpRegionV24::postViewablePolys()
{
  applyHints(m_core_poly, m_hints, "core");
  postMessage("VIEW_POLYGON", m_core_poly.get_spec());
  
  if(m_save_dist > 0) {
    applyHints(m_save_poly, m_hints, "save");
    postMessage("VIEW_POLYGON", m_save_poly.get_spec());
  }

  if(m_halt_dist > 0) {
    applyHints(m_halt_poly, m_hints, "halt");
    postMessage("VIEW_POLYGON", m_halt_poly.get_spec());
  }
}

//-----------------------------------------------------------
// Procedure: postErasablePolys()
//      Note: Even if the polygon is posted on each iteration, the
//            helm will filter out unnecessary duplicate posts.

void BHV_OpRegionV24::postErasablePolys()
{
  postMessage("VIEW_POLYGON", m_core_poly.get_spec_inactive());
  postMessage("VIEW_POLYGON", m_save_poly.get_spec_inactive());
  postMessage("VIEW_POLYGON", m_halt_poly.get_spec_inactive());
}

//-----------------------------------------------------------
// Procedure: handleConfigReset()
//   Purpose: The behavior may be reset to a state similar to
//            initial conditions, where being outside halt_poly
//            is ok until it first enters the halt_poly().
//            Reset message comes via a config "updates" msg.

bool BHV_OpRegionV24::handleConfigReset(string val)
{
  if(tolower(val) != "true")
    return(false);

  m_start_time = 0;
  
  m_secs_in_halt_poly  = 0;
  m_secs_out_halt_poly = 0;
  m_time_entered_halt_poly = 0;
  m_time_exited_halt_poly = 0;

  m_secs_out_save_poly = 0;
  m_time_exited_save_poly = 0;
  
  m_halt_poly_entry_made = false;
  m_save_poly_entry_made = false;
  return(true);
}

//-----------------------------------------------------------
// Procedure: updateRangeCache()

void BHV_OpRegionV24::updateRangeCache()
{
  // Step 1: Clear the current caches and sanity check
  m_heading_cache.clear();
  m_range_cache.clear();
  m_range_cache_norm.clear();
  m_range_cache_norm_no_lft.clear();
  m_range_cache_norm_no_rgt.clear();
  if(m_save_poly.size() == 0)
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
    double dist_to_region = m_save_poly.dist_to_poly(m_osx, m_osy, heading);
    m_heading_cache[i] = heading;
    m_range_cache[i]   = dist_to_region;
  }

  // Step 5. Normalize the values for all headings that reach the poly.
  // Longest distance will be zero, closest distance will be 100.
  // Step 5A: First find the min/max values
  double closest_dist = -1;
  double farthest_dist = -1;
  for(unsigned int i=0; i<hdg_pts; i++) {
    double range = m_range_cache[i];
    if(range != -1) {
      if((closest_dist == -1) || (range < closest_dist))
	closest_dist = range;
      if((farthest_dist == -1) || (range > farthest_dist))
	farthest_dist = range;
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
    double irange = m_range_cache[i];
    // For headings that do not intersect pol, set norm vals to zero
    if(irange == -1)
      m_range_cache_norm[i] = 0;
    // Otherwise normalize
    else
      m_range_cache_norm[i] = 100 * (farthest_dist - irange) / range_of_vals;
  }
  
  // Step 6: Build a cache where all left turns are masked out
  for(unsigned int i=0; i<hdg_pts; i++) {
    double hdg = m_heading_cache[i];    
    if(portTurn(m_osh, hdg))
      m_range_cache_norm_no_lft[i] = 0;
    else
      m_range_cache_norm_no_lft[i] = m_range_cache_norm[i];
  }

  // Step 7: Build a cache where all right turns are masked out
  for(unsigned int i=0; i<hdg_pts; i++) {
    double hdg = m_heading_cache[i];
    if(!portTurn(m_osh, hdg))
      m_range_cache_norm_no_rgt[i] = 0;
    else
      m_range_cache_norm_no_rgt[i] = m_range_cache_norm[i];
  }
}

//-----------------------------------------------------------
// Procedure: determineInitialTurn()

string BHV_OpRegionV24::determineInitialTurn()
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
// Procedure: depthVerify()
//      Note: Verify that the depth limit hasn't been violated.
//   Returns: true if no violation, false otherwise. 
//      Note: If a violoation, an error message is communicated 
//            by a call to postEMessage().

bool BHV_OpRegionV24::depthVerify()
{
  if(m_max_depth <= 0) // Just return if no max_depth enabled
    return(true);

  bool ok;
  double depth = getBufferDoubleVal("NAV_DEPTH", ok);

  // Must get ownship depth from info_buffer
  if(!ok) { 
    postEMessage("No ownship depth in info_buffer.");
    return(false);
  }

  if(depth > m_max_depth) {
    postFlags(m_breached_depth_flags);
    postEMessage("OpRegion Depth failure");
    return(false);
  }
  return(true);
}

//-----------------------------------------------------------
// Procedure: altitudeVerify()
//      Note: Verify that the altitude limit hasn't been violated.
//   Returns: true if no violation, false otherwise. 
//      Note: If a violoation, an error message is communicated 
//            by a call to postEMessage().

bool BHV_OpRegionV24::altitudeVerify()
{
  if(m_min_alt <= 0)  // Just return if no min_altitude enabled
    return(true);

  bool ok;
  double curr_altitude = getBufferDoubleVal("NAV_ALTITUDE", ok);

  if(!ok) { 
    postEMessage("No ownship altitude in info_buffer.");
    return(false);
  }

  if(curr_altitude < m_min_alt) {
    postFlags(m_breached_altitude_flags);
    postEMessage("OpRegion Altitude failure");
    return(false);
  }
  return(true);
}

//-----------------------------------------------------------
// Procedure: timeoutVerify()
//      Note: Verify that the timeout limit hasn't been violated.
//   Returns: true if no violation, false otherwise. 
//      Note: If a violoation, an error message is communicated 
//            by a call to postEMessage().

bool BHV_OpRegionV24::timeoutVerify()
{
  if(m_max_time <= 0)  // Just return if no max_time enabled
    return(true);
  
  double curr_time = getBufferCurrTime();
  double elapsed = curr_time - m_start_time;
  
  if(elapsed > m_max_time) {
    postFlags(m_breached_time_flags);
    postEMessage("OpRegion timeout failure");
    return(false);
  }
  return(true);
}


//-----------------------------------------------------------
// Procedure: expandMacros()

string BHV_OpRegionV24::expandMacros(string sdata)
{
  // =======================================================
  // First expand the macros defined at the superclass level
  // =======================================================
  sdata = IvPBehavior::expandMacros(sdata);

  // =======================================================
  // Expand configuration parameters
  // =======================================================
  sdata = macroExpand(sdata, "CORE_POLY", m_core_poly.get_spec());
  sdata = macroExpand(sdata, "SAVE_DIST", m_save_dist);
  sdata = macroExpand(sdata, "HALT_DIST", m_halt_dist);

  sdata = macroExpand(sdata, "SAVE_POLY", m_save_poly.get_spec());
  sdata = macroExpand(sdata, "HALT_POLY", m_halt_poly.get_spec());

  // =======================================================
  // Expand Behavior State
  // =======================================================

  sdata = macroExpand(sdata, "SECS_IN_HALT_POLY",  m_secs_in_halt_poly);
  sdata = macroExpand(sdata, "SECS_OUT_HALT_POLY", m_secs_out_halt_poly);
  sdata = macroExpand(sdata, "SECS_OUT_SAVE_POLY", m_secs_out_save_poly);

  if(m_max_time > 0) {
    double elapsed = getBufferCurrTime() - m_start_time;
    double time_remaining = m_max_time - elapsed;
    if(time_remaining < 0)
      time_remaining = 0;
    sdata = macroExpand(sdata, "TIME_LEFT", time_remaining);
  }
  
  if(m_max_depth > 0) {
    double depth = getBufferDoubleVal("NAV_DEPTH");
    double gap = m_max_depth - depth;
    if(gap < 0)
      gap = 0;
    if(gap >= 0)
      gap = (int)(gap);
    sdata = macroExpand(sdata, "DEPTH_LEFT", gap);
  }
  
  if(m_min_alt > 0) {
    double alt = getBufferDoubleVal("NAV_ALTITUDE");
    double gap = alt - m_min_alt;
    if(gap < 0)
      gap = 0;
    if(gap >= 0)
      gap = (int)(gap);
    sdata = macroExpand(sdata, "ALT_LEFT", gap);
  }
  
  // =======================================================
  // Expand Behavior State (DIST and ETA to Polys)
  // =======================================================
  // NOTE: For distance and ETA macros, we take steps to avoid
  // calculation of values by first checking if macros present.
  
  if(strContains(sdata, "DIST") || strContains(sdata, "ETA")) {

    double max_osv = getMaxOSV();

    if(strContains(sdata, "CORE")) {
      double dist_to_core = m_core_poly.dist_to_poly(m_osx, m_osy);
      double trdist_to_core = m_core_poly.dist_to_poly(m_osx, m_osy, m_osh);
      sdata = macroExpand(sdata, "DIST_TO_CORE", dist_to_core);
      sdata = macroExpand(sdata, "TRAJ_DIST_TO_CORE", dist_to_core);
      if(max_osv > 0) {
	double core_eta   = dist_to_core / max_osv;
	double trcore_eta = trdist_to_core / max_osv;
	sdata = macroExpand(sdata, "ETA_TO_CORE", core_eta);
	sdata = macroExpand(sdata, "TRAJ_ETA_TO_CORE", trcore_eta);
      }
    }
    
    if(strContains(sdata, "SAVE")) {
      double dist_to_save = m_save_poly.dist_to_poly(m_osx, m_osy);
      double trdist_to_save = m_save_poly.dist_to_poly(m_osx, m_osy, m_osh);
      sdata = macroExpand(sdata, "DIST_TO_SAVE", dist_to_save);
      sdata = macroExpand(sdata, "TRAJ_DIST_TO_SAVE", trdist_to_save);
      if(max_osv > 0) {
	double save_eta = dist_to_save / max_osv;
	double trsave_eta = trdist_to_save / max_osv;
	sdata = macroExpand(sdata, "ETA_TO_SAVE", save_eta);
	sdata = macroExpand(sdata, "TRAJ_ETA_TO_SAVE", trsave_eta);
      }
    }      

    if(strContains(sdata, "HALT")) {
      double dist_to_halt = m_halt_poly.dist_to_poly(m_osx, m_osy);
      double trdist_to_halt = m_halt_poly.dist_to_poly(m_osx, m_osy, m_osh);
      sdata = macroExpand(sdata, "DIST_TO_HALT", dist_to_halt);
      sdata = macroExpand(sdata, "TRAJ_DIST_TO_HALT", trdist_to_halt);
      if(max_osv > 0) {
	double halt_eta = dist_to_halt / max_osv;
	double trhalt_eta = trdist_to_halt / max_osv;
	sdata = macroExpand(sdata, "ETA_TO_HALT", halt_eta);
	sdata = macroExpand(sdata, "TRAJ_ETA_TO_HALT", trhalt_eta);
      }
    }      
  }

  return(sdata);
}
