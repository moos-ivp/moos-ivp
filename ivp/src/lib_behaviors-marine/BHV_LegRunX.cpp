/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_LegRunX.cpp                                      */
/*    DATE: Oct 2022                                             */
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

#include <cmath> 
#include <cstdlib>
#include <iostream>
#include "BHV_LegRunX.h"
#include "MBUtils.h"
#include "MacroUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "ZAIC_SPD.h"
#include "OF_Coupler.h"
#include "VarDataPairUtils.h"
#include "TurnGenWilliamson.h"
#include "ColorParse.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_LegRunX::BHV_LegRunX(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  // =================================================
  // Superclass vars
  m_descriptor = "bhv_legrun";  
  m_domain     = subDomain(m_domain, "course,speed");

  // =================================================
  // Config vars
  m_cruise_speed  = 0;  // meters/second
  m_patience = 50;
  
  m_min_leg_length = 50;
  m_min_turn_rad   = 5;
  m_warn_overshoot = false;
  m_turn_pt_gap    = 25;  // dist between turn points

  m_turn1_dir = "star";
  m_turn2_dir = "star";
  m_turn1_rad = 15;
  m_turn2_rad = 15;
  m_turn1_bias = 0;
  m_turn2_bias = 0;
  m_turn1_ext = 0;
  m_turn2_ext = 0;

  m_mid_pct = 25;
  
  m_var_report  = "WPT_STAT";

  // Visual Hint Defaults for the leg/turn paths
  m_hints.setMeasure("vertex_size", 3);
  m_hints.setMeasure("edge_size", 1);
  m_hints.setColor("vertex_color", "dodger_blue");
  m_hints.setColor("edge_color", "white");
  m_hints.setColor("label_color", "off");

  // Visual Hint Defaults for the next/track points
  m_hints.setMeasure("nextpt_vertex_size", 5);
  m_hints.setColor("nextpt_vertex_color", "yellow");
  m_hints.setColor("nextpt_label_color", "aqua");

  // Visual Hint Defaults for leg end points
  m_hints.setMeasure("legpt_vertex_size", 12);
  m_hints.setColor("legpt_vertex_color", "lime_green");
  m_hints.setColor("legpt_label_color", "off");

  // Visual Hint Defaults for the turn previews
  m_hints.setMeasure("turn_vertex_size", 1);
  m_hints.setMeasure("turn_edge_size", 1);
  m_hints.setColor("turn_vertex_color", "gray70");
  m_hints.setColor("turn_edge_color", "gray30");
  m_hints.setColor("turn_label_color", "off");

  m_leg_spds_repeat = false;
  m_leg_spds_onturn = false;
  
  // =================================================
  // State Vars
  m_mid_event_yet   = false;
  m_new_leg_pending = true;
  m_preview_pending = true;
  m_turn_interrupt_pending = false;
  
  m_leg_count1 = 0;
  m_leg_count2 = 0;
  
  m_leglen_req = -1;
  m_leglen_mod_req = 0;

  m_legang_req = -1;
  m_legang_mod_req = 0;

  m_leg_spds_ix = -1;
  m_leg_spds_curr = -1; // Neg means use cruise spd
  
  m_mode = "leg1";
  // The completed and perpetual vars are initialized in superclass
  // but we initialize here just to be safe and clear.
  m_completed = false; 
  m_perpetual = false;

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED");
  addInfoVars("LR_TURN_DIST");

  // turn_spd is used if multi-vehicle coordinatin is used
  m_turn_coord_spd = -1;
}

//-----------------------------------------------------------
// Procedure: setParam()

bool BHV_LegRunX::setParam(string param, string value) 
{
  if(param == "vx1") {
    m_new_leg_pending = true;
    return(setPointOnString(m_vx1, value));
  }
  else if(param == "vx2") {
    m_new_leg_pending = true;
    return(setPointOnString(m_vx2, value));
  }
  else if((param == "shift_point") || (param == "shift_pt")) {
    m_new_leg_pending = true;
    return(setPointOnString(m_shift_pt, value));
  }
  else if(param == "speed") {
    double maxv = m_domain.getVarHigh("speed");
    return(setDoubleRngOnString(m_cruise_speed, value, 0, maxv));
  }
  else if(param == "leg_spds")
    return(handleConfigLegSpeed(value));
  else if(param == "leg_spds_repeat")
    return(setBooleanOnString(m_leg_spds_repeat, value));
  else if(param == "leg_spds_onturn")
    return(setBooleanOnString(m_leg_spds_onturn, value));
  else if(param == "wpt_status_var")
    return(setStatusVarOnString(m_var_report, value));
  else if(param == "cycleflag") 
    return(addVarDataPairOnString(m_cycle_flags, value));
  else if(param == "wptflag") 
    return(addVarDataPairOnString(m_wpt_flags, value));
  else if(param == "legflag") 
    return(addVarDataPairOnString(m_leg_flags, value));
  else if(param == "midflag") 
    return(addVarDataPairOnString(m_mid_flags, value));
  else if(param == "warn_overshoot")
    return(setBooleanOnString(m_warn_overshoot, value));
  
  else if(param == "lead") {
    m_wpteng_turn.setParam(param, value);
    return(m_wpteng_legs.setParam(param, value));
  }
  else if(param == "lead_damper") {
    m_wpteng_turn.setParam(param, value);
    return(m_wpteng_legs.setParam(param, value));
  }
  else if(param == "capture_line") {
    m_wpteng_turn.setParam(param, value);
    return(m_wpteng_legs.setParam(param, value));
  }
  else if(param == "repeat")
    return(m_wpteng_legs.setParam(param, value));
  else if((param == "radius") || (param == "capture_radius")) {
    m_wpteng_turn.setParam(param, value);
    return(m_wpteng_legs.setParam(param, value));
  }
  else if(param == "slip_radius") {
    m_wpteng_turn.setParam(param, value);
    return(m_wpteng_legs.setParam(param, value));
  }
  else if(param == "mid_pct") 
    return(setDoubleStrictRngOnString(m_mid_pct, value, 0, 100));
  
  else if(param == "leg_length")
    return(setPosDoubleOnString(m_leglen_req, value));
  else if(param == "leg_length_mod")
    return(setDoubleOnString(m_leglen_mod_req, value));
  else if((param == "leg_length_min") && isNumber(value)) {
    m_min_leg_length = atof(value.c_str());
    if(m_min_leg_length < 5)
      m_min_leg_length = 5;
    return(true);
  }
  else if(param == "leg_angle")
    return(setDoubleOnString(m_legang_req, value));
  else if(param == "leg_angle_mod")
    return(setDoubleOnString(m_legang_mod_req, value));
  else if(param == "turn_pt_gap")
    return(setNonNegDoubleOnString(m_turn_pt_gap, value));
  
  else if(param == "patience")
    return(setDoubleClipRngOnString(m_patience, value, 1, 99));
  else if(strBegins(param, "turn"))
    return(handleConfigTurnParam(param, value));
  else if(param == "coord")
    return(handleConfigCoord(value));
  else if(param == "visual_hints")  
    return(m_hints.setHints(value));

  return(false);
}

//-----------------------------------------------------------
// Procedure: onSetParamComplete()

void BHV_LegRunX::onSetParamComplete()
{
  if(m_wpteng_legs.getMaxRepeats() > 0)
    IvPBehavior::setParam("perpetual", "true");

  if(!m_vx1.valid() || !m_vx2.valid())
    postWMessage("Invalid or unset leg point(s)");

  // Handle leg length request. Can only be handled after vertices
  // have been set. Typically mid-mission.
  if(m_leglen_req > 0) {
    handleLegLenRequest(m_leglen_req);
    m_leglen_req = -1;
  }    

  // Handle leg ANGLE request. Can only be handled after vertices
  // have been set. Typically mid-mission.
  if(m_legang_req >= 0) {
    handleLegAngRequest(m_legang_req);
    m_legang_req = -1;
  }    

  // Handle leg LENGTH request. Can only be handled after vertices
  // have been set. Typically mid-mission.
  if(m_leglen_mod_req != 0) {
    double curr_len = distPointToPoint(m_vx1, m_vx2);
    double new_len = curr_len + m_leglen_mod_req;
    if(new_len < m_min_leg_length)
      new_len = m_min_leg_length;
    if(curr_len != new_len)
      handleLegLenRequest(new_len);
    m_leglen_mod_req = 0;
  }    

  // Handle leg ANGLE request. Can only be handled after vertices
  // have been set. Typically mid-mission.
  if(m_legang_mod_req != 0) {
    double curr_ang = relAng(m_vx1, m_vx2);
    double new_ang = angle360(curr_ang + m_legang_mod_req);
    if(curr_ang != new_ang)
      handleLegAngRequest(new_ang);
    m_legang_mod_req = 0;
  }    

  if(m_shift_pt.valid())
    handleShiftPoint();
  
  // Handle change in core leg run. 
  if(m_new_leg_pending) {
    double dist = distPointToPoint(m_vx1, m_vx2);
    if(dist < m_min_leg_length)
      postWMessage("LegRun Leg too short: " + doubleToStringX(dist,2));
    else {
      XYSegList segl;
      segl.add_vertex(m_vx1);
      segl.add_vertex(m_vx2);
      m_wpteng_legs.setSegList(segl, true);
    }
    m_new_leg_pending = false;
    m_vx1.set_label("leg_vx1");
    m_vx2.set_label("leg_vx2");

    postAllSegList();
    if(m_mode == "turn")
      m_turn_interrupt_pending = true;
  }
  
  postConfigStatus();
}

//-----------------------------------------------------------
// Procedure: onRunToIdleState()
//      Note: Invoked automatically by the helm when the behavior
//            first transitions from the Running to Idle state.

void BHV_LegRunX::onRunToIdleState() 
{
  eraseAllViewables();

  m_wpteng_legs.resetCPA();
  m_wpteng_turn.resetCPA();
}

//-----------------------------------------------------------
// Procedure: onIdleToRunState()
//      Note: Invoked automatically by the helm when the behavior
//            first transitions from the Idle to Running state.

void BHV_LegRunX::onIdleToRunState() 
{
  m_preview_pending = true;
}

//-----------------------------------------------------------
// Procedure: onRunState()

IvPFunction *BHV_LegRunX::onRunState() 
{
  // Set m_osx, m_osy, m_osv, osh
  if(!updateInfoIn()) {
    eraseAllViewables();
    return(0);
  }

  if(m_preview_pending)
    postTurnPreview();
  
  // Pass 1: Generate a track point.
  bool ok = false;
  if((m_mode == "leg1") || (m_mode == "leg2"))
    ok = onRunStateLegMode();
  else if(m_mode == "turn")
    ok = onRunStateTurnMode();

  handleModeSwitch();

  // Pass 2: If pass 1 was a mode switch, try 2nd pass.
  if(!ok) {
    if((m_mode == "leg1") || (m_mode == "leg2"))
      ok = onRunStateLegMode();
    else if(m_mode == "turn")
      ok = onRunStateTurnMode();
  }

  postLegSpdsReport();
  postMessage("LEGRUN_MODE", m_mode);
  if(!ok)
    return(0);

  if(m_coord != "") {
    postTurnDist();
    setTurnCoordSpd();
  }
  
  IvPFunction *ipf = buildOF();
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: onRunStateTurnMode()

bool BHV_LegRunX::onRunStateTurnMode() 
{
  m_wpteng_turn.setNextWaypoint(m_osx, m_osy);    

  m_trackpt = m_wpteng_turn.getTrackPoint();
  m_nextpt  = m_wpteng_turn.getNextPoint();

  // If there was a new update to the basic leg, we'll interrupt this
  // and essentially transition to the right leg point.
  if(m_turn_interrupt_pending) {
    m_mode_pending = "leg";
    m_turn_interrupt_pending = false;
    postTurnSegList(false);
    postSteerPoints(false);
    return(false);
  }
  
  if(m_wpteng_turn.hasCompleted()) {
    m_mode_pending = "leg";
    postTurnSegList(false);
    postSteerPoints(false);
    m_wrap_detector.reset();
    m_odometer.reset();
    m_odometer.unpause();
    return(false);
  }

  if(m_wpteng_turn.hasAdvanced())
    postTurnSegList(true);

  postSteerPoints(true);
  postLegPoints(true);
    
  return(true); 
}

//-----------------------------------------------------------
// Procedure: onRunStateLegMode()

bool BHV_LegRunX::onRunStateLegMode() 
{
  // Sanity check
  if(m_wpteng_legs.size() == 0)
    return(false);

  // Part 1: Apply ownship position to the engine
  m_wpteng_legs.setNextWaypoint(m_osx, m_osy);

  // Part 2: Handle advanced, cycled and completed
  if(m_wpteng_legs.hasAdvanced()) {
    m_mode_pending = "turn";
    postFlags(m_wpt_flags);
    m_odometer.pause();
  }

  if(m_wpteng_legs.hasCycled())
    postFlags(m_cycle_flags);
    
  if(m_wpteng_legs.hasCompleted()) {
    setComplete();
    if(m_perpetual)
      m_wpteng_legs.resetForNewTraversal();
    postLegSegList(false);
    postSteerPoints(false);
    return(false);
  }

  // Advancement may mean mode switch so don't pub legflags
  if(!m_wpteng_legs.hasAdvanced())
    postFlags(m_leg_flags);
  
  // Part 3: Not completed so update core vars and visuals
  m_trackpt = m_wpteng_legs.getTrackPoint();
  m_nextpt  = m_wpteng_legs.getNextPoint();
  
  postStatusReport();
  postLegSegList(true);
  postSteerPoints(true);
  postLegPoints(true);

  // Part 4: Check for midflag conditions and maybe post
  if(m_leg_count1 > 0) {
    double pct_leg = 100 * m_wpteng_legs.pctToNextWpt(m_osx, m_osy);
    if((pct_leg > m_mid_pct) && !m_mid_event_yet) {
      postFlags(m_mid_flags, true);
      m_mid_event_yet = true;
    }
  }
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: handleModeSwitch()

void BHV_LegRunX::handleModeSwitch()
{
  // Sanity checks
  if((m_mode_pending == "") || (m_mode_pending == m_mode))
    return;

  // Transistion from leg to turn
  if(m_mode_pending == "turn") {
    if(m_mode == "leg1")
      m_leg_count1++;
    else if(m_mode == "leg2")
      m_leg_count2++;
    initTurnPoints();  
    m_mid_event_yet = false;

    // Revert to cruise_speed if not using leg spds on turning
    if(!m_leg_spds_onturn)
      m_leg_spds_curr = -1;
  }

  // Transistion from turn to leg1
  else if(m_mode_pending == "leg") {
    if(m_leg_count1 == m_leg_count2)
      m_mode = "leg1";
    else
      m_mode = "leg2";

    m_leg_spds_curr = -1;
    if(m_leg_spds.size() > 0) {
      m_leg_spds_ix++;

      if(m_leg_spds_ix >= (int)(m_leg_spds.size())) {
	if(m_leg_spds_repeat)
	  m_leg_spds_ix = 0;
	else
	  m_leg_spds_ix = -1;
      }

      if(m_leg_spds_ix >= 0)
	m_leg_spds_curr = m_leg_spds[m_leg_spds_ix];
    }
  }

  m_mode_pending = "";
}

//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_LegRunX::onIdleState() 
{
  if(!updateInfoIn()) 
    return;
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

bool BHV_LegRunX::updateInfoIn()
{
  bool ok1, ok2, ok3, ok4;
  m_osx = getBufferDoubleVal("NAV_X",       ok1);
  m_osy = getBufferDoubleVal("NAV_Y",       ok2);
  m_osh = getBufferDoubleVal("NAV_HEADING", ok3);
  m_osv = getBufferDoubleVal("NAV_SPEED",   ok4);

  m_odometer.updateDistance(m_osx, m_osy);
  m_wrap_detector.updatePosition(m_osx, m_osy);
  
  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2) {
    postEMessage("No ownship X/Y info in info_buffer.");
    return(false);
  }

  // Must get ownship heading from InfoBuffer
  if(!ok3) {
    postEMessage("No ownship heading info in info_buffer.");
    return(false);
  }

  // If NAV_SPEED info is not found in the info_buffer, its
  // not a show-stopper. A warning will be posted.
  if(!ok4)
    postWMessage("No ownship speed info in info_buffer");

  return(true);
}

//-----------------------------------------------------------
// Procedure: buildOF()

IvPFunction *BHV_LegRunX::buildOF() 
{
  IvPFunction *spd_ipf = 0;

  XYPoint trackpt = m_wpteng_legs.getTrackPoint();
  if(m_mode == "turn")
    trackpt = m_wpteng_turn.getTrackPoint();
  
  // ========================================
  // Part 1: Build the Speed ZAIC
  // ========================================

  double now_speed = m_cruise_speed;
  if(m_leg_spds_curr > 0)
    now_speed = m_leg_spds_curr;
  if(m_turn_coord_spd > 0)
    now_speed = m_turn_coord_spd;

  postMessage("LR_TCOORD_SPD", m_turn_coord_spd);
  postMessage("LR_NOW_SPD", now_speed);

  ZAIC_SPD spd_zaic(m_domain, "speed");
  spd_zaic.setParams(now_speed, 0.1, now_speed+0.4, 70, 20);
  spd_ipf = spd_zaic.extractIvPFunction();
  if(!spd_ipf)
    postWMessage("Failure on the SPD ZAIC via ZAIC_SPD utility");
  
  // ========================================
  // Part 2: Build the Course ZAIC
  // ========================================
  double tpx = m_trackpt.x();
  double tpy = m_trackpt.y();
  double rel_ang_to_trk_pt = relAng(m_osx, m_osy, tpx, tpy);
  
  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setValueWrap(true);
  crs_zaic.setParams(rel_ang_to_trk_pt, 0, 180, 50, 0, 100);
  int ix = crs_zaic.addComponent();
  crs_zaic.setParams(m_osh, 30, 180, 5, 0, 20, ix);
  
  IvPFunction *crs_ipf = crs_zaic.extractIvPFunction(false);
  if(!crs_ipf) 
    postWMessage("Failure on the CRS ZAIC");
  
  // ========================================
  // Part 3: Couple the Two Functions
  // ========================================
  OF_Coupler coupler;
  double crs_pct = m_patience;
  double spd_pct = 100-m_patience;
  
  IvPFunction *ipf = coupler.couple(crs_ipf, spd_ipf, crs_pct, spd_pct);
  if(!ipf)
    postWMessage("Failure on the CRS_SPD COUPLER");

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: postTurnDist()
//      Note: During a leg, the distance is just the distance to
//            the next leg.
//            During a turn, it is the distance remaining on the
//            current turn plus theh length of the full leg.
//      Note: In one direction the reported distance is positive
//            and the other it is negative.

//      Note: The sign difference is to convey to the speed
//            balancing heuristic that two vehicles are on the
//            "same" leg/turn. If the sign is opposite then that
//            vehicle would be disregarded from balancing
//            considerations.

void BHV_LegRunX::postTurnDist()
{
  double dist = 0;
  if(m_mode == "leg1")
    dist = hypot(m_osx - m_vx1.x(), m_osy - m_vx1.y());

  else if(m_mode == "leg2")
    dist = -hypot(m_osx - m_vx2.x(), m_osy - m_vx2.y());

  else {
    dist = m_wpteng_turn.distToEnd(m_osx, m_osy);
    dist += distPointToPoint(m_vx1, m_vx2);

    if(m_leg_count1 != m_leg_count2)
      dist = -dist;
  }

  m_turn_dist = dist;
  
  string report = "vname=" + m_us_name;
  report += ",tdist=" + doubleToStringX(dist,2);
  
  postOffboardMessage(m_coord, "LR_TURN_DIST", report);  
}

//-----------------------------------------------------------
// Procedure: setTurnCoordSpd()

void BHV_LegRunX::setTurnCoordSpd()
{
  // By default coord turn spd is disabled
  m_turn_coord_spd = -1;

  // coordination disabled or if we are not currently in the
  // turning mode, just return with turn_spd remaining at -1
  if((m_coord == "") || (m_mode != "turn"))
    return;

  double os_turn_dist = m_turn_dist;
  if(os_turn_dist < 0)
    os_turn_dist = -os_turn_dist;
  
  // ====================================================
  // Part 1: Get turn distances from all other vehicles
  // ====================================================
  double curr_time = getBufferCurrTime();
  bool   ok = true;
  vector<string> svector = getBufferStringVector("LR_TURN_DIST", ok);
  for(unsigned int i=0; i<svector.size(); i++) {
    string report = svector[i];
    string vname = tokStringParse(report, "vname");
    string sdist = tokStringParse(report, "tdist");
    double dist = atof(sdist.c_str());
    m_map_coord_dist[vname] = dist;
    m_map_coord_tstamp[vname] = curr_time;
  }

  // ====================================================
  // Part 2: Remove any stale and sign-mismatch entries
  // ====================================================
  vector<string> stale_vnames;
  map<string, double>::iterator p;
  for(p=m_map_coord_tstamp.begin(); p!=m_map_coord_tstamp.end(); p++) {
    string vname = p->first;
    double tstamp = p->second;
    double elapsed = curr_time - tstamp;
    if(elapsed > 5)
      stale_vnames.push_back(vname);
    if(m_turn_dist * m_map_coord_dist[vname] <= 0)
      stale_vnames.push_back(vname);
  }
  for(unsigned int i=0; i<stale_vnames.size(); i++) {
    string vname = stale_vnames[i];
    m_map_coord_dist.erase(vname);
    m_map_coord_tstamp.erase(vname);
  }
  
  // ====================================================
  // Part 3: Get avg of all other vehicles' turn dists.
  // ====================================================
  int    count = 0;
  double total = 0;
  for(p=m_map_coord_dist.begin(); p!=m_map_coord_dist.end(); p++) {
    double dist = p->second;
    if(dist < 0)
      dist = -dist;

    

    count++;
    total += dist;
  }

  // ====================================================
  // Part 4: Set min/max dist of os and relevant others
  // ====================================================
  double max_dist = os_turn_dist;
  double min_dist = os_turn_dist;
  for(p=m_map_coord_dist.begin(); p!=m_map_coord_dist.end(); p++) {
    double dist = p->second;
    if(dist < 0)
      dist = -dist;
    if((max_dist < 0) || (dist > max_dist))
      max_dist = dist;
    if((min_dist < 0) || (dist < min_dist))
      min_dist = dist;
  }

  // If there are no peer to coordinate with, just return with
  // turn_spd remaining -1
  if((count == 0) || (total <= 0))
    return;

  double avg_dist = total / (double)(count);
  
  double ratio = os_turn_dist / avg_dist;

  m_turn_coord_spd = ratio * ratio * m_cruise_speed;

  postMessage("LR_MIN_DIST", min_dist);
  postMessage("LR_MAX_DIST", max_dist);  
}

//-----------------------------------------------------------
// Procedure: postStatusReport()

void BHV_LegRunX::postStatusReport()
{
  int    current_waypt = m_wpteng_legs.getCurrIndex();
  unsigned int waypt_cycles = m_wpteng_legs.getCycleCount();
  unsigned int total_hits   = m_wpteng_legs.getTotalHits();
  unsigned int capture_hits = m_wpteng_legs.getCaptureHits();

  double dist_meters = m_wpteng_legs.distToNextWpt(m_osx, m_osy);
  double eta_seconds = dist_meters / m_osv;
  
  string hits_str = uintToString(capture_hits);
  hits_str += "/" + uintToString(total_hits);

  string stat = "vname=" + m_us_name + ",";
  stat += "behavior-name=" + m_descriptor + ",";
  stat += "index="  + intToString(current_waypt)   + ",";
  stat += "hits="   + hits_str + ",";
  stat += "cycles=" + uintToString(waypt_cycles)   + ",";
  stat += "dist="   + doubleToString(dist_meters, 0)  + ",";
  stat += "eta="    + doubleToString(eta_seconds, 0);

  if(m_var_report != "silent")
    postMessage((m_var_report), stat);
}

//-----------------------------------------------------------
// Procedure: initTurnPoints()

bool BHV_LegRunX::initTurnPoints()
{
  // Sanity checks
  XYSegList segl = m_wpteng_legs.getSegList();
  if(segl.size() != 2)
    return(false);

  XYSegList turn_segl;
  if(m_mode == "leg1")
    turn_segl = initTurnPoints1();
  else if(m_mode == "leg2")
    turn_segl = initTurnPoints2();
  else
    return(false);
  
  m_wpteng_turn.resetState();
  m_wpteng_turn.setSegList(turn_segl);

  postTurnSegList();
  
  m_mode = "turn";
  return(true);
}

//-----------------------------------------------------------
// Procedure: initTurnPoints1()

XYSegList BHV_LegRunX::initTurnPoints1(bool preview)
{
  XYSegList turn_segl;

  // Sanity checks
  if(!preview && (m_mode != "leg1"))
    return(turn_segl);
  XYSegList segl = m_wpteng_legs.getSegList();
  if(segl.size() != 2)
    return(turn_segl);
  
  // Part 1: Determine the target position and heading

  double px0 = segl.get_vx(0); 
  double py0 = segl.get_vy(0);
  double px1 = segl.get_vx(1); 
  double py1 = segl.get_vy(1);
  double th = relAng(px0, py0, px1, py1);

  if(m_turn1_ext > 0)
    projectPoint(th-180, m_turn1_ext, px0,py0, px0,py0);
  
  // If we have over-shot the end point, then use os position as
  // start/end pts of the turn instead
  double lane_dist = 0.1;
  if(!preview) {
    double angle = angleFromThreePoints(px0,py0, px1,py1, m_osx, m_osy);
    if(angle > 90) {
      lane_dist = hypot(m_osx-px0, m_osy-py0);
      px0 = m_osx;
      py0 = m_osy;
      if(m_warn_overshoot)
	postWMessage("Overshoot of core leg");
    }
  }
  
  TurnGenWilliamson turngen;
  turngen.setStartPos(px0, py0, th+180);
  turngen.setEndPos(segl.get_vx(0), segl.get_vy(0));
  turngen.setEndHeading(th);
  turngen.setTurnRadius(m_turn1_rad);
  turngen.setPointGap(m_turn_pt_gap);
  turngen.setLaneGap(lane_dist);
  turngen.setBiasPct(m_turn1_bias);
  turngen.setPortTurn(m_turn1_dir == "port");
  
  turngen.generate();
  vector<XYPoint> pts = turngen.getPts();
  for(unsigned int i=0; i<pts.size(); i++)
    turn_segl.add_vertex(pts[i]);

  return(turn_segl);
}

//-----------------------------------------------------------
// Procedure: initTurnPoints2()

XYSegList BHV_LegRunX::initTurnPoints2(bool preview)
{
  XYSegList turn_segl;

  // Sanity checks
  if(!preview && (m_mode != "leg2"))
    return(turn_segl);
  XYSegList segl = m_wpteng_legs.getSegList();
  if(segl.size() != 2)
    return(turn_segl);
  
  // Part 1: Determine the target position and heading
  double px0 = segl.get_vx(0); 
  double py0 = segl.get_vy(0);
  double px1 = segl.get_vx(1); 
  double py1 = segl.get_vy(1);
  double th = relAng(px1, py1, px0, py0);

  if(m_turn2_ext > 0)
    projectPoint(th-180, m_turn2_ext, px1,py1, px1,py1);
  

  
  // If we have over-shot the end point, then use os position as
  // start/end pts of the turn instead
  double lane_dist = 0.1;
  if(!preview) {
    double angle = angleFromThreePoints(px1,py1, px0,py0, m_osx, m_osy);
    if(angle > 90) {
      lane_dist = hypot(m_osx-px1, m_osy-py1);
      px1 = m_osx;
      py1 = m_osy;
      if(m_warn_overshoot)
	postWMessage("Overshoot of core leg");
    }
  }
  
  TurnGenWilliamson turngen;
  turngen.setStartPos(px1, py1, th+180);
  turngen.setEndPos(segl.get_vx(1), segl.get_vy(1));
  turngen.setEndHeading(th);
  turngen.setTurnRadius(m_turn2_rad);
  turngen.setPointGap(m_turn_pt_gap);
  turngen.setLaneGap(lane_dist);
  turngen.setBiasPct(m_turn2_bias);
  turngen.setPortTurn(m_turn2_dir == "port");
  
  turngen.generate();
  vector<XYPoint> pts = turngen.getPts();
  for(unsigned int i=0; i<pts.size(); i++)
    turn_segl.add_vertex(pts[i]);

  return(turn_segl);
}

//-----------------------------------------------------------
// Procedure: postLegSegList()

void BHV_LegRunX::postLegSegList(bool active)
{
  XYSegList seglist = m_wpteng_legs.getSegList();
  seglist.set_label(m_us_name + "_" + m_descriptor);

  if(!active)
    postMessage("VIEW_SEGLIST", seglist.get_spec_inactive());
  else {
    applyHints(seglist, m_hints);
    postMessage("VIEW_SEGLIST", seglist.get_spec());
  }  
}

//-----------------------------------------------------------
// Procedure: postTurnSegList()

void BHV_LegRunX::postTurnSegList(bool active)
{
  XYSegList seglist = m_wpteng_turn.getSegList();
  seglist.set_label(m_us_name + "_turn");

  if(!active)
    postMessage("VIEW_SEGLIST", seglist.get_spec_inactive());
  else {
    applyHints(seglist, m_hints);
    postMessage("VIEW_SEGLIST", seglist.get_spec());
  }
}

//-----------------------------------------------------------
// Procedure: postAllSegList()

void BHV_LegRunX::postAllSegList(bool active)
{
  XYSegList turn_segl1 = initTurnPoints1(true);
  XYSegList turn_segl2 = initTurnPoints2(true);
  turn_segl2.set_label("full_path");

  if(!active) {
    postMessage("VIEW_SEGLIST", turn_segl2.get_spec_inactive());
    return;
  }
  
  for(unsigned int i=0; i<turn_segl1.size(); i++)
    turn_segl2.add_vertex(turn_segl1.get_point(i));

  if(turn_segl2.size() == 0)
    return;
  
  string spec = turn_segl2.get_spec();
  postMessage("VIEW_SEGLIST", spec);  
}

//-----------------------------------------------------------
// Procedure: postTurnPreview()

void BHV_LegRunX::postTurnPreview(bool active)
{
  XYSegList segl1;
  XYSegList segl2;
  XYSegList segl3;

  if(!active) {
    segl1.set_label(m_us_name + "_preview1");
    segl2.set_label(m_us_name + "_preview2");  
    segl3.set_label(m_us_name + "_preview3");  
    postMessage("VIEW_SEGLIST", segl1.get_spec_inactive());
    postMessage("VIEW_SEGLIST", segl2.get_spec_inactive());
    postMessage("VIEW_SEGLIST", segl3.get_spec_inactive());
    return;
  }
  
  segl1 = initTurnPoints1(true);
  segl2 = initTurnPoints2(true);
  segl3 = XYSegList(m_vx1, m_vx2);
  
  if(segl1.size() != 0) {
    segl1.set_label(m_us_name + "_preview1");
    applyHints(segl1, m_hints, "turn");
    postMessage("VIEW_SEGLIST", segl1.get_spec());
  }
  
  if(segl2.size() != 0) {
    segl2.set_label(m_us_name + "_preview2");  
    applyHints(segl2, m_hints, "turn");
    postMessage("VIEW_SEGLIST", segl2.get_spec());
  }

  if(segl3.size() != 0) {
    segl3.set_label(m_us_name + "_preview3");  
    applyHints(segl3, m_hints, "turn");
    postMessage("VIEW_SEGLIST", segl3.get_spec());
  }
  m_preview_pending = false;
}

//-----------------------------------------------------------
// Procedure: postSteerPoints()

void BHV_LegRunX::postSteerPoints(bool active)
{
  m_trackpt.set_label(m_us_name + "'s track-point");
  m_nextpt.set_label(m_us_name + "'s next waypoint");

  applyHints(m_nextpt, m_hints, "nextpt");
  applyHints(m_trackpt, m_hints, "nextpt");

  if(!active) {
    postMessage("VIEW_POINT", m_trackpt.get_spec_inactive(), "trk");
    postMessage("VIEW_POINT", m_nextpt.get_spec_inactive(), "wpt");
    return;
  }

  XYPoint nextpt  = m_wpteng_legs.getNextPoint();
  XYPoint trackpt = m_wpteng_legs.getTrackPoint();

  applyHints(nextpt, m_hints, "nextpt");
  applyHints(trackpt, m_hints, "nextpt");

  trackpt.set_label(m_us_name + "'s track-point");
  nextpt.set_label(m_us_name + "'s next waypoint");

  postMessage("VIEW_POINT", m_nextpt.get_spec(), "wpt");
  
  // If trackpt,nextpt 5m apart or more, draw trackpt
  double dist = distPointToPoint(nextpt, trackpt);
  //if((m_mode != "turn") && (dist > 5))
  if(dist > 5)
    postMessage("VIEW_POINT", m_trackpt.get_spec(), "trk");
  else
    postMessage("VIEW_POINT", m_trackpt.get_spec_inactive(), "trk");
}

//-----------------------------------------------------------
// Procedure: postLegPoints()

void BHV_LegRunX::postLegPoints(bool active)
{
  if(!active) {
    postMessage("VIEW_POINT", m_vx1.get_spec_inactive(), "vx1");
    postMessage("VIEW_POINT", m_vx2.get_spec_inactive(), "vx2");
    return;
  }

  applyHints(m_vx1, m_hints, "legpt");
  applyHints(m_vx2, m_hints, "legpt");

  postMessage("VIEW_POINT", m_vx1.get_spec(), "vx1");
  postMessage("VIEW_POINT", m_vx2.get_spec(), "vx2");
}

//-----------------------------------------------------------
// Procedure: eraseAllViewables()

void BHV_LegRunX::eraseAllViewables()
{
  postLegSegList(false);
  postTurnSegList(false);
  postSteerPoints(false); 
  postLegPoints(false); 
  postTurnPreview(false); 
}


//-----------------------------------------------------------
// Procedure: handleConfigLegSpeed()

bool BHV_LegRunX::handleConfigLegSpeed(string str)
{
  str = tolower(removeWhite(str));
  
  // Part 1: Handle clearing the leg_spds vector
  if(str == "clear") {
    m_leg_spds.clear();
    m_leg_spds_ix = 0;
    return(true);
  }
  
  // Part 2: Handle resetting the speeds vector
  if(str == "reset") {
    m_leg_spds_ix = 0;
    return(true);
  }
  
  // Part 3: Handle replacing the speeds vector
  if(strBegins(str, "replace,")) {
    m_leg_spds.clear();
    m_leg_spds_ix = 0;
    biteString(str, ',');
  }
  
  double max_spd = m_domain.getVarHigh("speed");

  vector<double> new_leg_spds;
  
  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {

    if(strContains(svector[i], ":")) {
      string amt_str = biteString(svector[i], ':');
      string spd_str = svector[i];
      if(!isNumber(amt_str) || !isNumber(spd_str))
	return(false);
      int ival = atoi(amt_str.c_str());
      if(ival <= 0)
	return(false);
      double spd = atof(spd_str.c_str());
      if((spd < 0) || (spd > max_spd))
	return(false);
      for(int j=0; j<ival; j++)
	new_leg_spds.push_back(spd);
    }
    else {
      string spd_str = svector[i];
      if(!isNumber(spd_str))
	return(false);
      double spd = atof(spd_str.c_str());
      if((spd < 0) || (spd > max_spd))
	return(false);
      new_leg_spds.push_back(spd);
    }	
  }

  for(unsigned int i=0; i<new_leg_spds.size(); i++) {
    m_leg_spds.push_back(new_leg_spds[i]);
  }
  return(true);
}

//-----------------------------------------------------------
// Procedure: handleConfigTurnParam()

bool BHV_LegRunX::handleConfigTurnParam(string param, string val)
{
  double dval = atof(val.c_str());

  m_preview_pending = true;
  
  // Part 1: Setting the Turn Direction
  if(param == "turn1_dir")
    return(setPortStarOnString(m_turn1_dir, val));
  else if(param == "turn2_dir")
    return(setPortStarOnString(m_turn2_dir, val));
  else if(param == "turn_dir") {
    bool ok1 = setPortStarOnString(m_turn1_dir, val);
    bool ok2 = setPortStarOnString(m_turn2_dir, val);
    return(ok1 && ok2);
  }

  // Part 2: Setting the Turn Radius
  else if(param == "turn1_rad")
    return(setPosDoubleOnString(m_turn1_rad, val));
  else if(param == "turn2_rad")
    return(setPosDoubleOnString(m_turn2_rad, val));
  else if(param == "turn_rad") {
    if(dval > 0) {
      m_turn1_rad = dval;
      m_turn2_rad = dval;
      return(true);
    }
    return(false);
  }

  // Part 3: MODIFYING the Turn Radius
  else if((param == "turn_rad_min") && isNumber(val)) {
    m_min_turn_rad = dval;
    if(m_min_turn_rad < 2)
      m_min_turn_rad = 2;
    return(true);
  }      
  else if((param == "turn1_rad_mod") && isNumber(val)) {
    m_turn1_rad += dval;
    if(m_turn1_rad < 0)
      m_turn1_rad = 0;
    return(true);
  }      
  else if((param == "turn2_rad_mod") && isNumber(val)) {
    m_turn2_rad += dval;
    if(m_turn2_rad < 0)
      m_turn2_rad = 0;
    return(true);
  }      
  else if((param == "turn_rad_mod") && isNumber(val)) {
    m_turn1_rad += dval;
    m_turn2_rad += dval;
    if(m_turn1_rad < 0)
      m_turn1_rad = 0;
    if(m_turn2_rad < 0)
      m_turn2_rad = 0;
    return(true);
  }      
    
  // Part 4: Setting the Turn Bias
  else if(param == "turn1_bias")
    return(setDoubleStrictRngOnString(m_turn1_bias, val, -100, 100));
  else if(param == "turn2_bias")
    return(setDoubleStrictRngOnString(m_turn2_bias, val, -100, 100));
  else if(param == "turn1_ext")
    return(setNonNegDoubleOnString(m_turn1_ext, val));
  else if(param == "turn2_ext")
    return(setNonNegDoubleOnString(m_turn2_ext, val));
  else if((param == "turn_bias") && isNumber(val)) {
    if((dval >= -100) && (dval <= 100)) {
      m_turn1_bias = dval;
      m_turn2_bias = dval;
      return(true);
    }
  }

  // Part 5: Modifying the Turn Bias
  else if((param == "turn1_bias_mod") && isNumber(val)) {
    m_turn1_bias += dval;
    clipTurnBiases();
    return(true);
  } 
  else if((param == "turn2_bias_mod") && isNumber(val)) {
    m_turn2_bias += dval;
    clipTurnBiases();
    return(true);
  } 
  else if((param == "turn_bias_mod") && isNumber(val)) {
    m_turn1_bias += dval;
    m_turn2_bias += dval;
    clipTurnBiases();
    return(true);
  } 

  return(false);
}

//-----------------------------------------------------------
// Procedure: handleConfigCoord()
//     Notes: By default m_coord="" which indicates this behavior
//            will not try to coordinate the timing with other vehicles.
//            If m_coord is set to a non-empty string, this means
//            coordination is enabled. The value of this string
//            determines to which other vehicles coordination info
//            should be share. If set to "true" or "all", info will
//            be set to all other vehicles in the field. If set to
//            "group=blue" or "blue", it will be shared with other
//            vehicles in the "blue" group. 

bool BHV_LegRunX::handleConfigCoord(string val)
{
  // Since val could be a group name, which could have essentially an
  // unlimited set of possible names, the only thing we check for is that
  // there is no white space. 
  
  if(strContainsWhite(val))
    return(false);

  // The setting "all" and "true" are equivalent, and means coordination
  // info will be share with all other vehicles regardless of group name.
  
  val = tolower(val);
  if(val == "true")
    m_coord = "all";
  else
    m_coord = val;

  return(true);
}


//-----------------------------------------------------------
// Procedure: clipTurnBiases()

void BHV_LegRunX::clipTurnBiases()
{
  if(m_turn1_bias > 100)
    m_turn1_bias = 100;
  if(m_turn1_bias < -100)
    m_turn1_bias = -100;

  if(m_turn2_bias > 100)
    m_turn2_bias = 100;
  if(m_turn2_bias < -100)
    m_turn2_bias = -100;
}

//-----------------------------------------------------------
// Procedure: handleLegLenRequest()

void BHV_LegRunX::handleLegLenRequest(double len)
{
  // Handle leg length request. Can only handled after vertices
  // have been set. Typically mid-mission
  XYPoint rp1, rp2;
  bool ok = modSegLen(m_vx1, m_vx2, rp1, rp2, len);
  if(ok) {
    m_vx1 = rp1;
    m_vx2 = rp2;
    // new_leg_pending result in mod to wpt engine
    m_new_leg_pending = true;
    m_preview_pending = true;
  }
}

//-----------------------------------------------------------
// Procedure: handleLegAngRequest()
//   Purpose: Handle leg ANGLE request. Can only handled after
//            vertices have been set. Typically mid-mission.

void BHV_LegRunX::handleLegAngRequest(double angle)
{
  XYPoint rp1, rp2;
  bool ok = modSegAng(m_vx1, m_vx2, rp1, rp2, angle);
  if(ok) {
    m_vx1 = rp1;
    m_vx2 = rp2;
    // new_leg_pending result in mod to wpt engine
    m_new_leg_pending = true;
    m_preview_pending = true;
  }
}

//-----------------------------------------------------------
// Procedure: handleShiftPoint()
//   Purpose: Handle shift point if shift point is not
//            vertices have been set. Typically mid-mission.

void BHV_LegRunX::handleShiftPoint()
{
  if(!m_shift_pt.valid())
    return;
  
  XYPoint rp1, rp2;
  bool ok = modSegLoc(m_vx1, m_vx2, rp1, rp2, m_shift_pt);
  if(ok) {
    m_vx1 = rp1;
    m_vx2 = rp2;
    // new_leg_pending result in mod to wpt engine
    m_new_leg_pending = true;
    m_preview_pending = true;
  }
}

//-----------------------------------------------------------
// Procedure: postConfigStatus()

void BHV_LegRunX::postConfigStatus()
{
  string str = "type=BHV_LegRunX,name=" + m_descriptor;

  //str += ",points=" + m_wpteng_legs.getPointsStr();
  str += ",speed=" + doubleToStringX(m_cruise_speed,1);

  str += ",currix=" + intToString(m_wpteng_legs.getCurrIndex());
  str += ",wpt_status=" + m_var_report;

  //bool repeats_endless = m_wpteng_legs.getRepeatsEndless();
  //if(repeats_endless)
  //  str += ",repeats=forever";
  //else {
  //  unsigned int repeats = m_wpteng_legs.getRepeats();
  //  str += ",repeats=" + uintToString(repeats);
  // }

  double capture_radius = m_wpteng_legs.getCaptureRadius();
  double slip_radius = m_wpteng_legs.getSlipRadius();
  
  str += ",capture_radius=" + doubleToStringX(capture_radius, 1);
  str += ",slip_radius=" + doubleToStringX(slip_radius, 1);

  bool using_capture_line = m_wpteng_legs.usingCaptureLine();
  if(using_capture_line) {
    if((capture_radius == 0) && (slip_radius == 0))
      str += ",capture_line=absolute";
    else
      str += ",capture_line=" + boolToString(using_capture_line);
  }
  
  str += ",patience=" + doubleToStringX(m_patience, 2);

  postRepeatableMessage("BHV_SETTINGS", str);
}

//-----------------------------------------------------------
// Procedure: postLegSpdsReport()

void BHV_LegRunX::postLegSpdsReport()
{
  string str = "vname=" + m_us_name;
  if(m_leg_spds_curr >= 0)
    str += ",cruise=" + doubleToStringX(m_cruise_speed);
  else
    str += ",cruise=[" + doubleToStringX(m_cruise_speed) + "]";

  for(unsigned int i=0; i<m_leg_spds.size(); i++) {
    if(i==0)
      str += ",legs=";
    else
      str += ",";

    if((m_leg_spds_ix == (int)(i)) && (m_leg_spds_curr >= 0))
      str += "[" + doubleToStringX(m_leg_spds[i]) + "]";
    else
      str += "" + doubleToStringX(m_leg_spds[i]);
  }
  
  postMessage("LEG_SPDS_REP", str);
}

//-----------------------------------------------------------
// Procedure: expandMacros()

string BHV_LegRunX::expandMacros(string sdata)
{
  // =======================================================
  // First expand the macros defined at the superclass level
  // =======================================================
  sdata = IvPBehavior::expandMacros(sdata);

  // =======================================================
  // Expand configuration parameters
  // =======================================================
  sdata = macroExpand(sdata, "SPEED", m_cruise_speed);
  
  // =======================================================
  // Expand Behavior State
  // =======================================================
  sdata = macroExpand(sdata, "NI", m_wpteng_legs.getCurrIndex());
  sdata = macroExpand(sdata, "NX", m_wpteng_legs.getPointX());
  sdata = macroExpand(sdata," NY", m_wpteng_legs.getPointY());

  sdata = macroExpand(sdata, "CYCLES", m_wpteng_legs.getCycleCount());
  sdata = macroExpand(sdata, "CYCREM", m_wpteng_legs.repeatsRemaining());
  sdata = macroExpand(sdata, "WPTS_HIT", m_wpteng_legs.getTotalHits());
  sdata = macroExpand(sdata, "MODE", m_mode);

  double dist_to_next_wpt = m_wpteng_legs.distToNextWpt(m_osx, m_osy);
  sdata = macroExpand(sdata, "DIST_NP", dist_to_next_wpt);
  double dist_to_prev_wpt = m_wpteng_legs.distToPrevWpt(m_osx, m_osy);
  sdata = macroExpand(sdata, "DIST_PP", dist_to_prev_wpt);
  double pct_to_next_wpt = m_wpteng_legs.pctToNextWpt(m_osx, m_osy);
  sdata = macroExpand(sdata, "PCT_NP", pct_to_next_wpt);

  double leg_odo = m_odometer.getTotalDist();
  sdata = macroExpand(sdata, "LEG_ODO", leg_odo);
  unsigned int leg_id = m_leg_count1 + m_leg_count2;
  sdata = macroExpand(sdata, "LEG_ID", leg_id);

  unsigned int wraps = m_wrap_detector.getWraps();
  sdata = macroExpand(sdata, "WRAPS", wraps);
  
  return(sdata);
}

