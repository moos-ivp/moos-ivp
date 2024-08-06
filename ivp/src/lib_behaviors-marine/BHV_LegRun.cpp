/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng MIT                           */
/*    FILE: BHV_LegRun.cpp                                       */
/*    DATE: May 30th, 2023                                       */
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
#include "BHV_LegRun.h"
#include "MBUtils.h"
#include "MacroUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "ZAIC_SPD.h"
#include "OF_Coupler.h"
#include "VarDataPairUtils.h"
#include "ColorParse.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor()

BHV_LegRun::BHV_LegRun(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  // =================================================
  // Superclass vars
  m_descriptor = "legrun";  
  m_domain     = subDomain(m_domain, "course,speed");
  m_max_spd    = m_domain.getVarHigh("speed");

  // =================================================
  // Config vars
  m_cruise_spd  = 0;  // meters/second
  m_patience    = 50;
  
  m_mid_leg_pct   = 25;
  m_mid_turn_pct  = 25;
  m_init_leg_mode = "fixed";
  
  // Visual Hint Defaults for the leg/turn paths
  m_hints.setMeasure("vertex_size", 3);
  m_hints.setMeasure("edge_size", 1);
  m_hints.setColor("vertex_color", "dodger_blue");
  m_hints.setColor("edge_color", "gray60");
  m_hints.setColor("label_color", "off");

  // Visual Hint Defaults for the next/track points
  m_hints.setMeasure("nextpt_vertex_size", 3);
  m_hints.setColor("nextpt_vertex_color", "yellow");
  m_hints.setColor("nextpt_label_color", "aqua");

  // Visual Hint Defaults for leg end points
  m_hints.setMeasure("legpt_vertex_size", 7);
  m_hints.setColor("legpt_vertex_color", "orange");
  m_hints.setColor("legpt_label_color", "off");

  // Visual Hint Defaults for the turn previews
  m_hints.setMeasure("turn_vertex_size", 2);
  m_hints.setMeasure("turn_edge_size", 1);
  m_hints.setColor("turn_vertex_color", "gray70");
  m_hints.setColor("turn_edge_color", "gray30");
  m_hints.setColor("turn_label_color", "off");

  m_leg_spds_repeat = false;
  m_leg_spds_onturn = false;

  // =================================================
  // State Vars
  m_mid_leg_event_yet  = false;
  m_mid_turn_event_yet = false;
  m_preview_pending = true;
  
  m_leg_count = 0;
  m_leg_count1 = 0;
  m_leg_count2 = 0;
  
  m_leg_spds_ix = 0;
  m_leg_spds_curr = -1; // Neg means use cruise spd
  
  m_mode = "init";

  // The completed and perpetual vars are initialized in superclass
  // but we initialize here just to be safe and clear.
  m_completed = false; 
  m_perpetual = true;

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED");
}

//-----------------------------------------------------------
// Procedure: setParam()

bool BHV_LegRun::setParam(string param, string value) 
{
  if(param == "capture_line")
    return(setEngineParam(param, value));
  else if(param == "init_leg_mode")
    return(setInitLegMode(value));
  else if(param == "lead") 
    return(setEngineParam(param, value));
  else if(param == "lead_damper")
    return(setEngineParam(param, value));
  else if(param == "leg_spds")
    return(handleConfigLegSpeed(value));
  else if(param == "leg_spds_onturn")
    return(setBooleanOnString(m_leg_spds_onturn, value));
  else if(param == "leg_spds_repeat")
    return(setBooleanOnString(m_leg_spds_repeat, value));

  else if((param == "leg_endflag") || (param == "leg_end_flag")) 
    return(addFlagOnString(m_leg_endflags, value));
  else if((param == "leg1_endflag") || (param == "leg1_end_flag")) 
    return(addFlagOnString(m_leg1_endflags, value));
  else if((param == "leg2_endflag") || (param == "leg2_end_flag")) 
    return(addFlagOnString(m_leg2_endflags, value));

  else if((param == "midlegflag") || (param == "mid_leg_flag"))
    return(addFlagOnString(m_mid_leg_flags, value));
  else if((param == "midturnflag") || (param == "mid_turn_flag"))
    return(addFlagOnString(m_mid_turn_flags, value));
  else if(param == "mid_leg_pct") 
    return(setDoubleStrictRngOnString(m_mid_leg_pct, value, 0, 100));
  else if(param == "mid_turn_pct") 
    return(setDoubleStrictRngOnString(m_mid_turn_pct, value, 0, 100));
  else if(param == "patience")
    return(setDoubleClipRngOnString(m_patience, value, 1, 99));
  else if((param == "radius") || (param == "capture_radius"))
    return(setEngineParam(param, value));
  else if(param == "repeat")
    return(m_wpteng_legs.setParam(param, value));
  else if(param == "slip_radius")
    return(setEngineParam(param, value));
  else if(param == "speed")
    return(setDoubleRngOnString(m_cruise_spd, value, 0, m_max_spd));

  else if((param == "turn_endflag") || (param == "turn_end_flag")) 
    return(addFlagOnString(m_turn_endflags, value));
  else if((param == "turn1_endflag") || (param == "turn1_end_flag")) 
    return(addFlagOnString(m_turn1_endflags, value));
  else if((param == "turn2_endflag") || (param == "turn2_end_flag")) 
    return(addFlagOnString(m_turn2_endflags, value));

  else if(param == "visual_hints") 
    return(m_hints.setHints(value));
	   
  // All other params are handled in the LegRun level
  m_preview_pending = true;
  return(m_legrun.setParam(param, value));
}

//-----------------------------------------------------------
// Procedure: setEngineParam()
	   
bool BHV_LegRun::setEngineParam(string param, string value)
      {
  bool ok1 = m_wpteng_turn.setParam(param, value);
  bool ok2 = m_wpteng_legs.setParam(param, value);
  return(ok1 && ok2);
}

//-----------------------------------------------------------
// Procedure: setInitLegMode()
	   
bool BHV_LegRun::setInitLegMode(string str)
{
  if((str == "fixed") || (str == "close_turn") ||
     (str == "far_turn") || (str == "resume")) {
    m_init_leg_mode = str;
    return(true);
  }
  else
    return(false);
}

//-----------------------------------------------------------
// Procedure: onSetParamComplete()

void BHV_LegRun::onSetParamComplete()
{
  if(m_wpteng_legs.getMaxRepeats() > 0)
    IvPBehavior::setParam("perpetual", "true");

  if(!m_legrun.valid())
    postWMessage("Invalid or unset legrun");
  else
    postRetractWMessage("Invalid or unset legrun");

  // Handle change in core leg run. 
  if(m_legrun.legModified()) {
    XYSegList segl;
    segl.add_vertex(m_legrun.getPoint1());
    segl.add_vertex(m_legrun.getPoint2());
    m_wpteng_legs.setSegList(segl, true);

    if((m_mode == "turn1") || (m_mode == "turn2")) {
      postTurnSegList(false);
      postSteerPoints(false);
    }
    postAllSegList();
    postLegRing();
    setMode("init");
    m_legrun.setLegModified(false);
  }

  postConfigStatus();
}

//-----------------------------------------------------------
// Procedure: onRunToIdleState()
//      Note: Invoked automatically by the helm when the behavior
//            first transitions from the Running to Idle state.

void BHV_LegRun::onRunToIdleState() 
{
  eraseAllViewables();
  m_wpteng_legs.resetCPA();
  m_wpteng_turn.resetCPA();
}

//-----------------------------------------------------------
// Procedure: onIdleToRunState()
//      Note: Invoked automatically by the helm when the behavior
//            first transitions from the Idle to Running state.

void BHV_LegRun::onIdleToRunState() 
{
  m_preview_pending = true;

  if(m_mode == "turn1")
    m_mode = "leg2";
  else if(m_mode == "turn2")
    m_mode = "leg1";

#if 0
  if((m_mode == "turn1") || (m_mode == "turn2")) {
    m_mode = "init";
    initLegMode();
  }
#endif
}

//-----------------------------------------------------------
// Procedure: onRunState()

IvPFunction *BHV_LegRun::onRunState() 
{
  // Set m_osx, m_osy, m_osv, osh
  if(!updateInfoIn()) {
    eraseAllViewables();
    return(0);
  }

  if(!m_legrun.valid())
    return(0);
  
  if(m_mode == "init")
    initLegMode();

  if(m_preview_pending)
    postTurnPreview();
  
  // Pass 1: Generate a track point. A return of false means leg/turn
  // was emptry (err) or completed. The latter means a mode switch is
  // coming.
  bool ok = false;
  if((m_mode == "leg1") || (m_mode == "leg2"))
    ok = onRunStateLegMode();
  else if((m_mode == "turn1") || (m_mode == "turn2"))
    ok = onRunStateTurnMode();

  handleModeSwitch();

  // Pass 2: If pass 1 was a mode switch, try 2nd pass.
  if(!ok) {
    if((m_mode == "leg1") || (m_mode == "leg2"))
      ok = onRunStateLegMode();
    else if((m_mode == "turn1") || (m_mode == "turn2"))
      ok = onRunStateTurnMode();
  }

  if(!ok)
    return(0);

  IvPFunction *ipf = buildOF();
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: onRunStateTurnMode()

bool BHV_LegRun::onRunStateTurnMode() 
{
  m_wpteng_turn.setNextWaypoint(m_osx, m_osy);    

  m_trackpt = m_wpteng_turn.getTrackPoint();
  m_nextpt  = m_wpteng_turn.getNextPoint();

  if(m_wpteng_turn.hasCompleted()) {
    postFlags(m_turn_endflags, true);
    if(m_mode == "turn1")
      postFlags(m_turn1_endflags, true);
    else if(m_mode == "turn2")
      postFlags(m_turn2_endflags, true);

    advanceModePending();
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

  // Check for midflag conditions and maybe post
  double pct_turn = 100 * m_wpteng_turn.pctToEnd(m_osx, m_osy);
  if((pct_turn > m_mid_turn_pct) && !m_mid_turn_event_yet) {
    postFlags(m_mid_turn_flags, true);
    m_mid_turn_event_yet = true;
  }
  
  return(true); 
}

//-----------------------------------------------------------
// Procedure: onRunStateLegMode()

bool BHV_LegRun::onRunStateLegMode() 
{
  // Sanity check
  if(m_wpteng_legs.size() == 0)
    return(false);

  // Part 1: Apply ownship position to the engine
  m_wpteng_legs.setNextWaypoint(m_osx, m_osy);

  // Part 2: Handle advanced, cycled and completed
  if(m_wpteng_legs.hasAdvanced()) {
    postFlags(m_leg_endflags, true);
    if(m_mode == "leg1")
      postFlags(m_leg1_endflags, true);
    else if(m_mode == "leg2")
      postFlags(m_leg2_endflags, true);
    advanceModePending();
    m_odometer.pause();
  }

  if(m_wpteng_legs.hasCompleted()) {
    postFlags(m_end_flags);
    if(m_perpetual)
      m_wpteng_legs.resetForNewTraversal();
    postLegSegList(false);
    postSteerPoints(false);
    return(false);
  }

  // Part 3: Not completed so update core vars and visuals
  m_trackpt = m_wpteng_legs.getTrackPoint();
  m_nextpt  = m_wpteng_legs.getNextPoint();
  
  postLegSegList(true);
  postSteerPoints(true);
  postLegPoints(true);

  // Part 4: Check for midflag conditions and maybe post
  if(m_leg_count > 0) {
    double pct_leg = 100 * m_wpteng_legs.pctToNextWpt(m_osx, m_osy);
    if((pct_leg > m_mid_leg_pct) && !m_mid_leg_event_yet) {
      postFlags(m_mid_leg_flags, true);
      m_mid_leg_event_yet = true;
    }
  }
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: setMode()

void BHV_LegRun::setMode(string mode)
{
  if((mode != "leg1") && (mode != "turn1") &&
     (mode != "leg2") && (mode != "turn2") &&
     (mode != "init"))
    return;
  
  m_mode = mode;
  postMessage("LR_MODE", m_mode);

  double utc = getBufferCurrTime();
  string report = "vname=" + m_us_name + ",mode=" + m_mode;
  report += ",utc=" + doubleToStringX(utc);
  
  postMessage("LR_MODE_REPORT", report);
}

//-----------------------------------------------------------
// Procedure: initLegMode()

void BHV_LegRun::initLegMode()
{
  // Sanity check
  if(m_mode != "init")
    return;
  
  if(m_init_leg_mode == "fixed") {
    setMode("leg1");
    m_wpteng_legs.setCurrIndex(0);
    return;
  }
  
  XYPoint p1 = m_legrun.getPoint1();
  XYPoint p2 = m_legrun.getPoint2();
  double  d1 = hypot(m_osx - p1.x(), m_osy - p1.y());
  double  d2 = hypot(m_osx - p2.x(), m_osy - p2.y());

  // set the index of the leg waypoint based on
  // user-set parameter
  if (m_init_leg_mode == "far_turn") {
    if(d1 > d2) { 
      setMode("leg1");
      m_wpteng_legs.setCurrIndex(0);
    }
    else {
      setMode("leg2");
      m_wpteng_legs.setCurrIndex(1);
    }
    
  } else if (m_init_leg_mode == "close_turn") {
    if(d1 < d2) { 
      setMode("leg1");
      m_wpteng_legs.setCurrIndex(0);
    }
    else {
      setMode("leg2");
      m_wpteng_legs.setCurrIndex(1);
    }
  } else {
    postEMessage("Bad init leg direction, check init_leg_mode param.");
  }
  
}

//-----------------------------------------------------------
// Procedure: handleModeSwitch()

void BHV_LegRun::handleModeSwitch()
{
  // Sanity checks
  if((m_mode_pending == "") || (m_mode_pending == m_mode))
    return;

  setMode(m_mode_pending);

  m_mode_pending  = "";
  m_mid_leg_event_yet  = false;
  m_mid_turn_event_yet = false;

  // Entering turn1 implies a leg1 vertex arrival/counter++
  if(m_mode == "turn1") {
    m_leg_count++;
    m_leg_count1++;
    initTurnPoints();  
  }
  else if(m_mode == "turn2") {
    m_leg_count++;
    m_leg_count2++;
    initTurnPoints();  
  }

  updateLegSpd();
}

//-----------------------------------------------------------
// Procedure: updateLegSpd()
//      Legs: o Speed at by default is m_cruise_spd
//            o If no speed schedule is specified, the var
//              m_leg_spds_curr is -1, indicating the the
//              cruise speed will be used.
//            o If speed schedule specified, we pick the
//              proper speed here from the schedule.
//            o If schedule does not repeat and it has been
//              exhausted, m_leg_spds_curr reverts to -1
//     Turns: o If m_leg_spds_onturn is false (default) the
//              speed during a turn is the m_cruise_spd
//            o If m_leg_spds_onturn is true, then borrow
//              the most recent leg speed.

void BHV_LegRun::updateLegSpd()
{
  // Part 1: If we just entered a leg mode, perhaps set the
  // m_leg_spds_curr from a schedule, if the schedule exists
  // and has not yet been exhausted
  if((m_mode == "leg1") || (m_mode == "leg2")) {

    // -1 value says leg spd schedule not in use
    m_leg_spds_curr = -1;

    if(m_leg_spds.size() > 0) {
      if(m_leg_spds_ix >= (int)(m_leg_spds.size())) {
	if(m_leg_spds_repeat) {
	  m_leg_spds_ix = 0;
	  m_leg_spds_curr = m_leg_spds[0];
	}
	else
	  m_leg_spds.clear(); // schedule exhausted
      }
      else {
	m_leg_spds_curr = m_leg_spds[m_leg_spds_ix];
	m_leg_spds_ix++;
      }
    }
  }
    
  // Part 2: If we just entered one of the turn modes, keep the
  // m_leg_spds_curr set to whatever was prevailing during the
  // previous leg, unless m_leg_spds_onturn is fasle.
  if((m_mode == "turn1") || (m_mode == "turn2")) {
    if(!m_leg_spds_onturn)
      m_leg_spds_curr = -1;
  }
}

//-----------------------------------------------------------
// Procedure: onIdleState()

void BHV_LegRun::onIdleState() 
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

bool BHV_LegRun::updateInfoIn()
{
  // ==========================================================
  // Part 1: Update Ownship position and speed from the buffer
  // ==========================================================
  bool ok1, ok2, ok3, ok4;
  m_osx = getBufferDoubleVal("NAV_X", ok1);
  m_osy = getBufferDoubleVal("NAV_Y", ok2);
  m_osv = getBufferDoubleVal("NAV_SPEED", ok3);
  m_osh = getBufferDoubleVal("NAV_HEADING", ok4);

  m_odometer.updateDistance(m_osx, m_osy);
  m_wrap_detector.updatePosition(m_osx, m_osy);
  
  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2) {
    postEMessage("No ownship X/Y info in info_buffer.");
    return(false);
  } 

  // If NAV_SPEED info is not found in the info_buffer, its
  // not a show-stopper. A warning will be posted.
  string warning_msg = "No ownship speed info in info_buffer";
  if(!ok3)
    postWMessage(warning_msg);
  else 
    postRetractWMessage(warning_msg);
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: buildOF()

IvPFunction *BHV_LegRun::buildOF() 
{
  IvPFunction *spd_ipf = 0;

  XYPoint trackpt = m_wpteng_legs.getTrackPoint();
  if(m_mode == "turn")
    trackpt = m_wpteng_turn.getTrackPoint();
  
  // ========================================
  // Part 1: Build the Speed ZAIC
  // ========================================

  double now_speed = m_cruise_spd;
  if(m_leg_spds_curr > 0)
    now_speed = m_leg_spds_curr;

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
  crs_zaic.setSummit(rel_ang_to_trk_pt);
  crs_zaic.setBaseWidth(180);
  crs_zaic.setValueWrap(true);
  
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
// Procedure: initTurnPoints()

bool BHV_LegRun::initTurnPoints()
{
  if((m_mode != "turn1") && (m_mode != "turn2"))
    return(false);
  
  XYSegList turn_segl;
  if(m_mode == "turn1") 
    turn_segl = m_legrun.initTurnPoints1();
  else 
    turn_segl = m_legrun.initTurnPoints2();
  
  m_wpteng_turn.resetState();
  m_wpteng_turn.setSegList(turn_segl);

  postTurnSegList();

  return(true);
}

//-----------------------------------------------------------
// Procedure: postAllSegList()

void BHV_LegRun::postAllSegList(bool active)
{
  return;
  XYSegList turn_segl1 = m_legrun.initTurnPoints1();
  XYSegList turn_segl2 = m_legrun.initTurnPoints2();
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
// Procedure: postLegSegList()

void BHV_LegRun::postLegSegList(bool active)
{
  return;
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
// Procedure: postLegRing()

void BHV_LegRun::postLegRing(bool active)
{
  return;
  XYPolygon ringpoly;

  XYPoint cpt = m_legrun.getCenterPt();
  double cx = cpt.x();
  double cy = cpt.y();
  double diam = m_legrun.getLegLen();
  cx = (int)cx;
  cy = (int)cy;
  
  ringpoly.setRadial(cx,cy, diam/2, 36, 0.01);
  ringpoly.set_label("center_joust");
  
  if(!active)
    postMessage("VIEW_POLYGON", ringpoly.get_spec_inactive());
  else {
    applyHints(ringpoly, m_hints, "ring");
    postMessage("VIEW_POLYGON", ringpoly.get_spec());
  }  
}

//-----------------------------------------------------------
// Procedure: postTurnSegList()

void BHV_LegRun::postTurnSegList(bool active)
{
  XYSegList segl = m_wpteng_turn.getSegList();
  segl.set_label(m_us_name + "_turn");

  if(!active) {
    postMessage("VIEW_SEGLIST", segl.get_spec_inactive());
    return;
  }

#if 0
  XYPoint fpt = segl.get_first_point();
  XYPoint lpt = segl.get_last_point();

  fpt.set_vertex_size(38);
  fpt.set_vertex_color("dodger_blue");
  fpt.set_label_color("off");
  fpt.set_label(m_us_name + "_tfpt");
  fpt.set_duration(600);
  string fpt_spec = fpt.get_spec();
  postMessage("VIEW_POINT", fpt_spec);

  lpt.set_vertex_size(28);
  lpt.set_vertex_color("white");
  lpt.set_label_color("off");
  lpt.set_label(m_us_name + "_tlpt");
  lpt.set_duration(600);
  string lpt_spec = lpt.get_spec();
  postMessage("VIEW_POINT", lpt_spec);
#endif
  
  applyHints(segl, m_hints, "turn");
  postMessage("VIEW_SEGLIST", segl.get_spec());
}

//-----------------------------------------------------------
// Procedure: postTurnPreview()

void BHV_LegRun::postTurnPreview(bool active)
{
  string label1 = m_us_name + "_pview1";
  string label2 = m_us_name + "_pview2";
  string label3 = m_us_name + "_pview3";
  
  XYSegList segl1(label1);
  XYSegList segl2(label2);
  XYSegList segl3(label3);

  if(!active) {
    postMessage("VIEW_SEGLIST", segl1.get_spec_inactive());
    postMessage("VIEW_SEGLIST", segl2.get_spec_inactive());
    postMessage("VIEW_SEGLIST", segl3.get_spec_inactive());
    return;
  }
  
  segl1 = m_legrun.initTurnPoints1();
  segl2 = m_legrun.initTurnPoints2();
  segl3 = XYSegList(m_legrun.getPoint1(), m_legrun.getPoint2());
  segl1.set_label(label1);
  segl2.set_label(label2);
  segl3.set_label(label3);
  
  if(segl1.size() != 0) {
    applyHints(segl1, m_hints, "turn");
    postMessage("VIEW_SEGLIST", segl1.get_spec());
  }
  
  if(segl2.size() != 0) {
    applyHints(segl2, m_hints, "turn");
    postMessage("VIEW_SEGLIST", segl2.get_spec());
  }

  if(segl3.size() != 0) {
    applyHints(segl3, m_hints, "legpt");
    postMessage("VIEW_SEGLIST", segl3.get_spec());
  }

  bool turn_pt_change = false;
  if(segl1.size() > 1) {
    double dist = segl1.length() * 0.42;
    XYPoint pt1 = segl1.get_dist_point(dist);
    if(distPointToPoint(pt1, m_turn_pt1) > 1) {
      m_turn_pt1 = pt1;
      turn_pt_change = true;
    }
  }
  if(segl2.size() > 1) {
    double dist = segl1.length() * 0.42;
    XYPoint pt2 = segl2.get_dist_point(dist);
    if(distPointToPoint(pt2, m_turn_pt2) > 1) {
      m_turn_pt2 = pt2;
      turn_pt_change = true;
    }
  }
  if(turn_pt_change)
    postTurnPoints();
  
  m_preview_pending = false;
}

//-----------------------------------------------------------
// Procedure: postSteerPoints()

void BHV_LegRun::postSteerPoints(bool active)
{
  m_trackpt.set_label(m_us_name + "'s track-point");
  m_nextpt.set_label(m_us_name + "'s next waypoint");
  applyHints(m_nextpt, m_hints, "nextpt");
  applyHints(m_trackpt, m_hints, "nextpt");
  string np_spec = m_nextpt.get_spec_inactive();
  string tp_spec = m_trackpt.get_spec_inactive();
  
  if(active) {
    np_spec = m_nextpt.get_spec();
    if(distPointToPoint(m_nextpt, m_trackpt) > 5)
      tp_spec = m_trackpt.get_spec();
  }
  
  postMessage("VIEW_POINT", tp_spec, "trk");
  postMessage("VIEW_POINT", np_spec, "wpt");
  
}

//-----------------------------------------------------------
// Procedure: postLegPoints()

void BHV_LegRun::postLegPoints(bool active)
{
  XYPoint p1 = m_legrun.getPoint1();
  XYPoint p2 = m_legrun.getPoint2();
  p1.set_label(m_us_name + "_p1");
  p2.set_label(m_us_name + "_p2");

  string spec1 = p1.get_spec_inactive();
  string spec2 = p2.get_spec_inactive();
  
  if(active) {
    applyHints(p1, m_hints, "legpt");
    applyHints(p2, m_hints, "legpt");
    // Draw P1 a bit bigger to distinguish the two.
    int vsize = p1.get_vertex_size();
    p1.set_vertex_size(vsize * 1.5);
    spec1 = p1.get_spec();
    spec2 = p2.get_spec();
  }

  postMessage("VIEW_POINT", p1.get_spec(), "p1");
  postMessage("VIEW_POINT", p2.get_spec(), "p2");
}

//-----------------------------------------------------------
// Procedure: eraseAllViewables()

void BHV_LegRun::eraseAllViewables()
{
  postLegSegList(false);
  postTurnSegList(false);
  postSteerPoints(false); 
  postLegPoints(false); 
  postTurnPreview(false); 
  postTurnPoints(false); 
}


//-----------------------------------------------------------
// Procedure: handleConfigLegSpeed()

bool BHV_LegRun::handleConfigLegSpeed(string str)
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
// Procedure: advanceModePending()

void BHV_LegRun::advanceModePending()
{
  if(m_mode == "leg1")
    m_mode_pending = "turn1";
  else if(m_mode == "turn1")
    m_mode_pending = "leg2";
  else if(m_mode == "leg2")
    m_mode_pending = "turn2";
  else if(m_mode == "turn2")
    m_mode_pending = "leg1";
  else if(m_mode == "leg1a")
    m_mode_pending = "leg2";
  else if(m_mode == "leg2a")
    m_mode_pending = "leg1";
}

//-----------------------------------------------------------
// Procedure: postConfigStatus()

void BHV_LegRun::postConfigStatus()
{
  string str = "type=BHV_LegRun,name=" + m_descriptor;

  str += ",speed=" + doubleToStringX(m_cruise_spd,1);

  str += ",currix=" + intToString(m_wpteng_legs.getCurrIndex());

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
// Procedure: expandMacros()

string BHV_LegRun::expandMacros(string sdata)
{
  // =======================================================
  // First expand the macros defined at the superclass level
  // =======================================================
  sdata = IvPBehavior::expandMacros(sdata);

  // =======================================================
  // Expand configuration parameters
  // =======================================================
  sdata = macroExpand(sdata, "SPEED", m_cruise_spd);
  
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

//-----------------------------------------------------------
// Procedure: postTurnPoints()

void BHV_LegRun::postTurnPoints(bool active)
{
  string tp1_spec = m_turn_pt1.get_spec_inactive();
  string tp2_spec = m_turn_pt2.get_spec_inactive();
  if(active) {
    m_turn_pt1.set_vertex_size(8);
    m_turn_pt1.set_vertex_color("limegreen");
    m_turn_pt1.set_label_color("off");
    m_turn_pt1.set_label(m_us_name + "_fpt1");
    m_turn_pt1.set_duration(600);
    
    m_turn_pt2.set_vertex_size(8);
    m_turn_pt2.set_vertex_color("limegreen");
    m_turn_pt2.set_label_color("off");
    m_turn_pt2.set_label(m_us_name + "_fpt2");
    m_turn_pt2.set_duration(600);
    
    tp1_spec = m_turn_pt1.get_spec();
    tp2_spec = m_turn_pt2.get_spec();
  }
  
  postMessage("VIEW_POINT", tp1_spec);
  postMessage("VIEW_POINT", tp2_spec);
}
