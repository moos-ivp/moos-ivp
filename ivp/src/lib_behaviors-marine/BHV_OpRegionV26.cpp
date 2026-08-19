/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_OpRegionV26.cpp                                  */
/*    DATE: Nov 24th, 2023                                       */
/*    DATE: Aug 4th, 2026 V26                                    */
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
#include "BHV_OpRegionV26.h"
#include "OF_Reflector.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "VarDataPairUtils.h"
#include "XYFormatUtilsPoly.h"
#include "XYFormatUtilsSegl.h"
#include "MacroUtils.h"
#include "XYPolyExpander.h"
#include "ZAIC_Vector.h"
#include "ZAIC_SPD.h"
#include "ZAIC_PEAK.h"
#include "BuildUtils.h"
#include "OF_Coupler.h"
#include "AOF_OpRegion.h"

using namespace std;

//-----------------------------------------------------------
// Constructor()

BHV_OpRegionV26::BHV_OpRegionV26(IvPDomain gdomain) :
  IvPBehavior(gdomain)
{
  m_domain = subDomain(m_domain, "course,speed");  
  this->setParam("m_descriptor", "opregz");
  this->setParam("build_info", "uniform_piece = discrete @ course:3,speed:3");
  this->setParam("build_info", "uniform_grid  = discrete @ course:9,speed:6");

  // ==================================================
  // PART 1: Init config vars
  // ==================================================
  m_lapse_dist  = 5;
  m_breach_dist = 15;

  m_min_util_eta = 10;
  m_max_util_eta = 30;

  m_get_back_enabled = true;
  m_stay_in_enabled = true;
  
  // Visual Hint Defaults for the leg/turn paths
  m_hints.setMeasure("vertex_size", 0);
  m_hints.setMeasure("edge_size", 1);
  m_hints.setColor("vertex_color", "gray50");
  m_hints.setColor("edge_color", "gray50");
  m_hints.setColor("label_color", "off");

  m_hints.setMeasure("cover_transparency", 0.05);
  m_hints.setMeasure("cover_vertex_size", 5);
  m_hints.setColor("cover_fill_color", "gray95");
  m_hints.setColor("cover_vertex_color", "white");
  m_hints.setColor("cover_edge_color", "gray20");

  // Declare if poly is enabled immediately (default) or
  // triggered only when the vehicle first enters poly.
  // Useful if vehicle/mission launched from outside poly.
  m_trigger_on_poly_entry = true;

  // Time required for the vehicle to be within the poly 
  // before containment is enforced. Only relevant 
  // if the trigger_on_poly_entry flag is true.
  m_trigger_entry_time = 1.0;

  // Time required for vehicle to be out of halt_poly before poly
  // containment condition triggers a declaration of emergency.
  m_trigger_breach_time = 0.5;

  // default speed when recovering when core poly breach.
  m_recover_spd = 0.8; // meters/sec
  
  // ==================================================
  // PART 2: Init state vars
  // ==================================================
  reInitStateVars();
  
  // ==================================================
  // PART 3: Declare Info Vars
  // ==================================================  
  addInfoVars("NAV_X, NAV_Y, NAV_HEADING, NAV_SPEED");
}

//-----------------------------------------------------------
// Procedure: reInitStateVars()

void BHV_OpRegionV26::reInitStateVars()
{
  // Time stamps for calculating how long the vehicle has been
  // in a particular state.
  m_time_entering_start = 0;
  m_time_entering_total = 0;
  m_time_contained_start = 0;
  m_time_contained_total = 0;
  m_time_lapsed_start = 0;
  m_time_lapsed_total = 0;
  m_time_breached_start = 0;
  m_time_breached_total = 0;

  m_contained_ever = false;
  m_state = "unset";
  m_prev_state = "unset";

  m_visuals_posted_utc = 0;
  
  m_committed_turn = "none";  // Always none, left, or right

  m_abs_rng_to_exit = -1;  // Rng to oparea border in any direc-tino
  m_bng_rng_to_exit = -1;  // Rng to oparea border in osh dir
  m_mix_rng_to_exit = -1;  // Weighted mix/avg between rng vals
}

//-----------------------------------------------------------
// Procedure: setParam()

bool BHV_OpRegionV26::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  if(param == "core_poly")
    return(setCorePolyBoundary(val));

  else if(param == "lapse_dist")
    return(setNonNegDoubleOnString(m_lapse_dist, val));
  else if(param == "breach_dist")
    return(setNonNegDoubleOnString(m_breach_dist, val));

  else if(param == "get_back_enabled") 
    return(setBooleanOnString(m_get_back_enabled, val));
  else if(param == "stay_in_enabled") 
    return(setBooleanOnString(m_stay_in_enabled, val));

  else if(param == "min_util_eta")
    return(setNonNegDoubleOnString(m_min_util_eta, val));
  else if(param == "max_util_eta")
    return(setNonNegDoubleOnString(m_max_util_eta, val));

  else if((param == "recover_spd") || (param == "recover_speed"))
    return(setPosDoubleOnString(m_recover_spd, val));
  else if(param == "reset")
    return(handleConfigReset(val));
  else if(param == "trigger_on_poly_entry") 
    return(setBooleanOnString(m_trigger_on_poly_entry, val));
  else if(param == "trigger_entry_time") 
    return(setNonNegDoubleOnString(m_trigger_entry_time, val));
  else if(param == "trigger_breach_time") 
    return(setNonNegDoubleOnString(m_trigger_breach_time, val));

  else if(param == "entering_flag") 
    return(addFlagOnString(m_entering_flags, val));
  else if(param == "enteringx_flag") 
    return(addFlagOnString(m_enteringx_flags, val));

  else if(param == "contained_flag") 
    return(addFlagOnString(m_contained_flags, val));
  else if(param == "containedx_flag") 
    return(addFlagOnString(m_containedx_flags, val));

  else if(param == "breached_flag") 
    return(addFlagOnString(m_breached_flags, val));

  else if(param == "lapsed_flag") 
    return(addFlagOnString(m_lapsed_flags, val));
  else if(param == "lapsedx_flag") 
    return(addFlagOnString(m_lapsedx_flags, val));

  else if(param == "visual_hints") 
    return(m_hints.setHints(val));
  else if(param == "dynamic_region_var") {
    bool ok = setNonWhiteVarOnString(m_dynamic_region_var, val);
    if(!ok)
      return(false);
    addInfoVars(m_dynamic_region_var);
    return(true);
  }
  return(false);
}

//-----------------------------------------------------------
// Procedure: onSetParamComplete()

void BHV_OpRegionV26::onSetParamComplete()
{
  postConfigStatus();
}

//-----------------------------------------------------------
// Procedure: onRunState()
//     Notes: Always returns NULL, never returns an IvPFunction*
//     Notes: Sets state_ok = false and posts an error message if
//            any of the OpRegionV26 conditions are not met.

IvPFunction *BHV_OpRegionV26::onRunState() 
{
  // Sanity check
  if(!updateInfoIn())
      return(0);

  postViewableRegion();

  // Update the state. Save the state prior to updating to be
  // considered the previous state. State transition affects
  // which flags are posted.
  m_prev_state = m_state;
  updateState();
  updateContainedRngs();
  
  if(m_state == "entering")
    handleStateEntering();
  else if(m_state == "contained")
    handleStateContained();
  else if(m_state == "lapsed")
    handleStateLapsed();
  else if(m_state == "breached")
    handleStateBreached();

  // Only in lapsed state will an IvPFunction be created
  if((m_state == "contained") && m_stay_in_enabled)
    return(buildOF_StayIn());

  else if((m_state == "lapsed") && m_get_back_enabled)
    return(buildOF_Recover());
    
  return(0);
}
  
//-----------------------------------------------------------
// Procedure: buildOF_Recover()

IvPFunction *BHV_OpRegionV26::buildOF_Recover()
{
  //=========================================================
  // Part 1: Preliminaries
  //=========================================================
  updateRangeCache();

  // Are we heading toward the poly?
  // Note we don't set to false if currently not heading to poly. Once
  // set to true it stays true until we get back inside the poly. 

  double curr_hdg_dist_to_poly = m_core_poly.distRayToEnterGP(m_osx, m_osy, m_osh);
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
  ZAIC_PEAK zaic_spd(m_domain, "speed");
  double peak_width = m_recover_spd / 2;
  zaic_spd.setParams(m_recover_spd, peak_width, 0.2, 20, 0, 100);

  IvPFunction *ipf_spd = zaic_spd.extractIvPFunction();
  if(!ipf_spd) {
    postRepeatableMessage("OPR_ZAIC_SPD_ERR", zaic_spd.getWarnings());
    return(0);
  }

  OF_Coupler coupler;
  IvPFunction *ipf = 0;

  ipf = coupler.couple(ipf_hdg, ipf_spd, 50, 50);

#if 0
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
#endif
  

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
// Procedure: buildOF_StayIn()

IvPFunction *BHV_OpRegionV26::buildOF_StayIn()
{
  AOF_OpRegion aof(m_domain);

  // osx, osy, osh, osv embedded in the plat model
  aof.setPlatModel(m_plat_model);
  aof.setGenPoly(m_core_poly);
  aof.setParam("min_util_eta", m_min_util_eta);
  aof.setParam("max_util_eta", m_max_util_eta);
  bool ok = aof.initialize();

  if(!ok) {
    postEMessage("Unable to init AOF_OpRegion.");
    cout << "Unable to init AOF_OpRegion." << endl;

    //list<string> msgs = aof.getMsgsAOF();
    //list<string>::iterator p;
    //for(p=msgs.begin(); p!=msgs.end(); p++)
    //  cout << "aof Msg: " << *p << endl;

    return(0);
  }    

  OF_Reflector reflector(&aof, 1);
  m_domain = subDomain(m_domain, "course,speed");

  reflector.create(m_build_info);
  IvPFunction *ipf = reflector.extractIvPFunction();
  ipf->setPWT(m_priority_wt);

  return(ipf);
}


//-----------------------------------------------------------
// Procedure: handleStateBreached()

void BHV_OpRegionV26::handleStateBreached()
{
  if(m_time_breached_total < m_trigger_breach_time)
    return;

  if(m_prev_state != "breached")
    postFlags(m_breached_flags);
  postEMessage("OpRegion HaltPoly Failure");
}

//-----------------------------------------------------------
// Procedure: handleStateLapsed()

void BHV_OpRegionV26::handleStateLapsed()
{
  if(m_prev_state == "contained")
    postFlags(m_lapsed_flags);
  postFlags(m_lapsedx_flags);
}

//-----------------------------------------------------------
// Procedure: handleStateContained()

void BHV_OpRegionV26::handleStateContained()
{
  if(m_prev_state != "contained")
    postFlags(m_contained_flags);
  postFlags(m_containedx_flags);
}

//-----------------------------------------------------------
// Procedure: handleStateEntering()

void BHV_OpRegionV26::handleStateEntering()
{
  if(m_prev_state != "entering")
    postFlags(m_entering_flags);
  postFlags(m_enteringx_flags);
}

//-----------------------------------------------------------
// Procedure: updateState()
//    States: entering, contained, lapsed, breached.

void BHV_OpRegionV26::updateState()
{
  bool contained = m_core_poly.contains(m_osx, m_osy);
  postRepeatableMessage("V26_IN", contained);

  if(contained)
    m_state = "contained";
  
  else if(m_trigger_on_poly_entry && !m_contained_ever)
    m_state = "entering";
  else {
    double dist_out_of_poly = m_core_poly.distPtToEnterGP(m_osx, m_osy);
    if(dist_out_of_poly > m_breach_dist)
      m_state = "breached";
    else
      m_state = "lapsed";
  }
  postRepeatableMessage("V26_STATE", m_state);

  // One of these will be set below to non-zero
  m_time_entering_total  = 0;
  m_time_contained_total = 0;
  m_time_lapsed_total    = 0;
  m_time_breached_total  = 0;

  
  // Update state variables related to contained state
  if(m_state == "entering") {
    if(m_time_entering_start == 0)
      m_time_entering_start = getBufferCurrTime();
    
    // Determine accumulated time in contained state
    m_time_entering_total = getBufferCurrTime() - m_time_entering_start;

    m_time_contained_start = 0;
    m_time_lapsed_start = 0;
    m_time_breached_start = 0;
  }

  // Update state variables related to contained state
  if(m_state == "contained") {
    // If entering the poly, note the time of entry
    if(m_time_contained_start == 0)
      m_time_contained_start = getBufferCurrTime();
    
    // Determine accumulated time in contained state
    m_time_contained_total = getBufferCurrTime() - m_time_contained_start;

    // Check if in poly long enough to be official entry
    if(m_time_contained_total >= m_trigger_entry_time)
      m_contained_ever = true;

    m_time_entering_start = 0;
    m_time_lapsed_start   = 0;
    m_time_breached_start = 0;
   }

  if(m_state == "lapsed") {
    // If entering the poly, note the time of entry
    if(m_time_lapsed_start == 0)
      m_time_lapsed_start = getBufferCurrTime();
    
    // Determine accumulated time in lapsed state
    m_time_lapsed_total    = getBufferCurrTime() - m_time_lapsed_start;

    m_time_entering_start  = 0;
    m_time_contained_start = 0;
    m_time_breached_start  = 0;
   }

  if(m_state == "breached") {
    if(m_time_breached_start == 0)
      m_time_breached_start = getBufferCurrTime();
    
    // Determine accumulated time in breached state
    m_time_breached_total = getBufferCurrTime() - m_time_breached_start;

    m_time_entering_start  = 0;
    m_time_contained_start = 0;
    m_time_lapsed_start    = 0;
   }

}

//-----------------------------------------------------------
// Procedure: updateContainedRngs()

void BHV_OpRegionV26::updateContainedRngs()
{
  m_abs_rng_to_exit = -1;
  m_bng_rng_to_exit = -1;
  m_mix_rng_to_exit = -1;

  if(m_state == "contained") {
    m_abs_rng_to_exit = m_core_poly.distPtToExitGP(m_osx, m_osy);
    m_bng_rng_to_exit = m_core_poly.distRayToExitGP(m_osx, m_osy, m_osh);
    m_mix_rng_to_exit = (m_abs_rng_to_exit + m_bng_rng_to_exit) / 2;
    if(m_mix_rng_to_exit > (m_abs_rng_to_exit * 2))
      m_mix_rng_to_exit = m_abs_rng_to_exit * 2;
  }
}


//-----------------------------------------------------------
// Procedure: updateInfoIn()

bool BHV_OpRegionV26::updateInfoIn()
{
  bool ok = IvPBehavior::updatePlatformInfo();
  if(!ok)
    return(false);
  
  if(getBufferVarUpdated(m_dynamic_region_var)) {
    bool ok;
    string new_poly_core = getBufferStringVal(m_dynamic_region_var, ok);
    ok = ok && setParam("core_poly", new_poly_core);
    onSetParamComplete();
    if(!ok)
      return(false);
  }
  return(true);
}


//-----------------------------------------------------------
// Procedure: postViewableRegion()

void BHV_OpRegionV26::postViewableRegion()
{
  double elapsed = getBufferCurrTime() - m_visuals_posted_utc;
  if(elapsed < 30)
    return;

  m_visuals_posted_utc = getBufferCurrTime();
  
  XYSegList segl_border = m_core_poly.getSegList();
  if(segl_border.size() > 1) {
    segl_border.add_vertex(segl_border.get_vx(0), segl_border.get_vy(0));
    segl_border.set_label("opborder");
    segl_border.set_duration(60);
    applyHints(segl_border, m_hints, "border");
    postRepeatableMessage("VIEW_SEGLIST", segl_border.get_spec(3));
  }
  
  vector<XYPolygon> polys = m_core_poly.getCoverPolys();
  for(unsigned int i=0; i<polys.size(); i++) {
    polys[i].set_label("cp" + uintToString(i));
    polys[i].set_duration(60);
    applyHints(polys[i], m_hints, "cover");
    postMessage("VIEW_POLYGON", polys[i].get_spec(3));
  }
}

//-----------------------------------------------------------
// Procedure: postErasableRegion()
//      Note: Even if the polygon is posted on each iteration, the
//            helm will filter out unnecessary duplicate posts.

void BHV_OpRegionV26::postErasableRegion()
{
  XYSegList segl_border = m_core_poly.getSegList();
  segl_border.set_label("opborder");
  postMessage("VIEW_SEGLIST", segl_border.get_spec_inactive());
  
  vector<XYPolygon> polys = m_core_poly.getCoverPolys();
  for(unsigned int i=0; i<polys.size(); i++) {
    XYPolygon poly = polys[i];
    poly.set_label("cp" + uintToString(i));
    postMessage("VIEW_POLYGON", polys[i].get_spec_inactive());
  }
}

//-----------------------------------------------------------
// Procedure: handleConfigReset()
//   Purpose: The behavior may be reset to a state similar to
//            initial conditions, where being outside halt_poly
//            is ok until it first enters the halt_poly().
//            Reset message comes via a config "updates" msg.

bool BHV_OpRegionV26::handleConfigReset(string val)
{
  if(tolower(val) != "true")
    return(false);

  reInitStateVars();
  return(true);
}

//-----------------------------------------------------------
// Procedure: updateRangeCache()

void BHV_OpRegionV26::updateRangeCache()
{
  // Step 1: Clear the current caches and sanity check
  m_heading_cache.clear();
  m_range_cache.clear();
  m_range_cache_norm.clear();
  m_range_cache_norm_no_lft.clear();
  m_range_cache_norm_no_rgt.clear();
  if(m_core_poly.valid() == 0)
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
    double dist_to_region = m_core_poly.distRayToEnterGP(m_osx, m_osy, heading);
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

string BHV_OpRegionV26::determineInitialTurn()
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
// Procedure: setCorePolyBoundary()

bool BHV_OpRegionV26::setCorePolyBoundary(string str)
{
  XYGenPolygon gpoly = stringToGenPoly(str);
  if(!gpoly.valid())
    return(false);
  
  m_core_poly = gpoly;
  return(true);
}


//-----------------------------------------------------------
// Procedure: expandMacros()

string BHV_OpRegionV26::expandMacros(string sdata)
{
  // =======================================================
  // First expand the macros defined at the superclass level
  // =======================================================
  sdata = IvPBehavior::expandMacros(sdata);

  double exit_dist  = m_core_poly.distRayToExitGP(m_osx, m_osy, m_osh);
  double entry_dist = m_core_poly.distPtToEnterGP(m_osx, m_osy);
  
  sdata = macroExpand(sdata, "EXIT_DIST", exit_dist);
  sdata = macroExpand(sdata, "ENTRY_DIST", entry_dist);
  
  // =======================================================
  // Expand Behavior State
  // =======================================================
  sdata = macroExpand(sdata, "SECS_CONTAINED",  m_time_contained_total);
  sdata = macroExpand(sdata, "SECS_LAPSED", m_time_lapsed_total);
  sdata = macroExpand(sdata, "SECS_ENTERING", m_time_entering_total);
  sdata = macroExpand(sdata, "SECS_BREACHED", m_time_breached_total);

  // =======================================================
  // Expand Behavior State (DIST and ETA to Polys)
  // =======================================================
  // NOTE: For distance and ETA macros, we take steps to avoid
  // calculation of values by first checking if macros present.
  if(strContains(sdata, "DIST")) {
    if(strContains(sdata, "EXIT")) {
      sdata = macroExpand(sdata, "ABS_DIST_TO_EXIT", m_abs_rng_to_exit);
      sdata = macroExpand(sdata, "BNG_DIST_TO_EXIT", m_bng_rng_to_exit);
      sdata = macroExpand(sdata, "MIX_DIST_TO_EXIT", m_mix_rng_to_exit);

      double max_osv = getMaxOSV();
      if(max_osv > 0) {
	double abs_exit_eta   = m_abs_rng_to_exit / max_osv;
	double bng_exit_eta   = m_bng_rng_to_exit / max_osv;
	double mix_exit_eta   = m_mix_rng_to_exit / max_osv;
	sdata = macroExpand(sdata, "ABS_ETA_TO_EXIT", abs_exit_eta);
	sdata = macroExpand(sdata, "BNG_ETA_TO_EXIT", bng_exit_eta);
	sdata = macroExpand(sdata, "MIX_ETA_TO_EXIT", mix_exit_eta);
      }
    }
  }
  return(sdata);
}

