/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_Waypoint.cpp                                     */
/*    DATE: Nov 2004                                             */
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
#include "BHV_Waypoint.h"
#include "OF_Reflector.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "AOF_Waypoint.h"
#include "GeomUtils.h"
#include "BuildUtils.h"
#include "PathUtils.h"
#include "FunctionEncoder.h"
#include "ZAIC_PEAK.h"
#include "ZAIC_SPD.h"
#include "OF_Coupler.h"
#include "XYFormatUtilsPoly.h"
#include "XYFormatUtilsPoint.h"
#include "XYFormatUtilsSegl.h"
#include "ColorParse.h"
//---------------------------------
#include "IO_Utilities.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Waypoint::BHV_Waypoint(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  // First set variables at the superclass level
  m_descriptor = "bhv_waypoint";  
  m_domain     = subDomain(m_domain, "course,speed");

  // Set behavior configuration variables
  m_cruise_speed     = 0;  // meters/second
  m_cruise_speed_alt = -1; // meters/second - default of -1 means unused
  m_lead_distance    = -1; // meters - default of -1 means unused
  m_lead_damper      = -1; // meters - default of -1 means unused
  m_lead_allowed     = true;
  m_efficiency_measure = "off"; // or "off" or "all"
  m_ipf_type        = "zaic";

  m_var_report      = "WPT_STAT";
  m_var_index       = "WPT_INDEX";
  m_var_cyindex     = "CYCLE_INDEX";
  m_var_suffix      = "";
  m_use_alt_speed   = false;

  // Visual Hint Defaults
  m_hint_vertex_size   = 3;
  m_hint_edge_size     = 1;
  m_hint_vertex_color  = "dodger_blue";
  m_hint_edge_color    = "white";
  m_hint_label_color   = "white";
  m_hint_nextpt_color  = "yellow";
  m_hint_nextpt_lcolor = "aqua";
  m_hint_nextpt_vertex_size = 5;

  m_lead_to_start     = false;

  // The completed and perpetual vars are initialized in superclass
  // but we initialize here just to be safe and clear.
  m_completed  = false; 
  m_perpetual  = false;

  m_course_pct = 50;
  m_speed_pct  = 50;

  m_osx   = 0;
  m_osy   = 0;
  m_osv   = 0;
  m_osh   = 0;

  m_odo_set_flag = false;
  m_odo_setx = 0;
  m_odo_sety = 0;
  m_odo_virgin = true;

  m_dist_leg_odo = 0;
  m_dist_total_odo    = 0;
  m_dist_total_linear = 0;
  m_time_total_odo    = 0;
  m_time_total_linear = 0;

  m_osx_prev = 0;
  m_osy_prev = 0;

  m_greedy_tour_pending = false;

  m_prev_cycle_index = 0;
  m_prev_waypt_index = -1;
  
  addInfoVars("NAV_X, NAV_Y, NAV_SPEED");
  m_markpt.set_active(false);
  m_markpt.set_vertex_size(4);

  m_prevpt.set_label("prev_pt");
}

//-----------------------------------------------------------
// Procedure: onSetParamComplete()

void BHV_Waypoint::onSetParamComplete()
{
  m_trackpt.set_label(m_us_name + "'s track-point");
  m_trackpt.set_vertex_size(4);
  m_trackpt.set_color("label", m_hint_nextpt_color);
  m_trackpt.set_color("vertex", m_hint_nextpt_color);
  
  m_nextpt.set_label(m_us_name + "'s next waypoint");
  m_nextpt.set_vertex_size(m_hint_nextpt_vertex_size);
  m_nextpt.set_color("vertex", m_hint_nextpt_color);
  m_nextpt.set_color("label", m_hint_nextpt_lcolor);

  // Get the lead_condition variables and register for them
  vector<string> svector;
  for(unsigned i=0; i<m_lead_conditions.size(); i++)
    svector = mergeVectors(svector, m_lead_conditions[i].getVarNames());

  svector = removeDuplicates(svector);
  for(unsigned int i=0; i<svector.size(); i++)
    addInfoVars(svector[i], "nowarning");  
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition".
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_Waypoint::setParam(string param, string param_val) 
{
  double dval = atof(param_val.c_str());
  string param_val_lower = tolower(param_val);

  if((param == "polygon") || (param == "points")) {
    XYSegList new_seglist = string2SegList(param_val);
    if(new_seglist.size() == 0) {
      XYPolygon new_poly = string2Poly(param_val);
      new_seglist = new_poly.exportSegList(m_osx, m_osy);
    }
    if(new_seglist.size() == 0)
      return(false);
    m_waypoint_engine.setSegList(new_seglist);
    m_markpt.set_active(false);
    return(true);
  }
  else if(param == "point") {
    XYPoint point = string2Point(param_val);
    XYSegList new_seglist;
    new_seglist.add_vertex(point);
    if(new_seglist.size() == 0)
      return(false);
    m_waypoint_engine.setSegList(new_seglist);
    m_markpt.set_active(false);
    return(true);
  }
  else if((param == "speed") && isNumber(param_val) && (dval >= 0)) {
    if(dval > m_domain.getVarHigh("speed"))
      dval = m_domain.getVarHigh("speed");

    if(dval != m_cruise_speed)
      m_odo_leg_disq = true;
    m_cruise_speed = dval;
    return(true);
  }
  else if((param == "speed_alt") && isNumber(param_val) && (dval >= 0)) {
    if(dval != m_cruise_speed_alt)
      m_odo_leg_disq = true;
    m_cruise_speed_alt = dval;
    return(true);
  }
  else if((param == "currix") && (dval >= 0)) {
    m_waypoint_engine.setCurrIndex((unsigned int)(dval));
    return(true);
  }
  else if((param == "greedy_tour") || (param == "shortest_tour")) {
    setBooleanOnString(m_greedy_tour_pending, param_val);
    return(true);
  }
  else if(param == "use_alt_speed") {
    bool prev_use_alt_speed = m_use_alt_speed;
    setBooleanOnString(m_use_alt_speed, param_val);
    if(prev_use_alt_speed != m_use_alt_speed)
      m_odo_leg_disq = true;
    return(true);
  }
  else if((param == "wpt_status") || (param == "wpt_status_var")) {
    if(strContainsWhite(param_val) || (param_val == ""))
      return(false);
    m_var_report = param_val;
    if(tolower(m_var_report)=="silent")
      m_var_report = "silent";
    return(true);
  }
  else if(param == "wpt_dist_to_prev") {
    if(strContainsWhite(param_val))
      return(false);
    if((param_val == "") || (tolower(param_val) == "silent"))
      m_var_dist_to_prev = "";
    else
      m_var_dist_to_prev = param_val;
    return(true);
  }
  else if(param == "wpt_dist_to_next") {
    if(strContainsWhite(param_val))
      return(false);
    if((param_val == "") || (tolower(param_val) == "silent"))
      m_var_dist_to_next = "";
    else
      m_var_dist_to_next = param_val;
    return(true);
  }
  else if((param == "wpt_index") || (param == "wpt_index_var")) {
    if(strContainsWhite(param_val) || (param_val == ""))
      return(false);
    m_var_index = param_val;
    if(tolower(m_var_index)=="silent")
      m_var_index = "silent";
    return(true);
  }
  else if(param == "cycle_index_var") {   // Depricated
    if(strContainsWhite(param_val) || (param_val == ""))
      return(false);
    m_var_cyindex = param_val;
    if(tolower(m_var_cyindex)=="silent")
      m_var_cyindex = "silent";
    return(true);
  }
  else if(param == "post_suffix") {
    if(strContainsWhite(param_val))
      return(false);
    if((param_val.length() > 0) && (param_val.at(0) != '_'))
      param_val = '_' + param_val;
    m_var_suffix = param_val;
    return(true);
  }
  else if(param == "cycleflag") {
    string variable = biteStringX(param_val, '=');
    string value    = param_val;
    if((variable=="") || (value==""))
      return(false);
    VarDataPair pair(variable, value, "auto");
    m_cycle_flags.push_back(pair);
    return(true);
  }
  else if(param == "wptflag") {
    string variable = biteStringX(param_val, '=');
    string value    = param_val;
    if((variable=="") || (value=="")) 
      return(false);
     VarDataPair pair(variable, value, "auto");
    m_wpt_flags.push_back(pair);
    return(true);
  }
  else if((param == "ipf-type") || (param == "ipf_type")) {
    param_val = tolower(param_val);
    if((param_val=="zaic") || (param_val=="zaic_spd") || 
       (param_val=="roc")  || (param_val=="rate_of_closure"))
      m_ipf_type = param_val;
    return(true);
  }
  else if(param == "lead"){
    if(dval <= 0) // indicating it is off
      m_lead_distance = -1;
    else
      m_lead_distance = dval;
    return(true);
  }
  else if(param == "lead_to_start")
    return(setBooleanOnString(m_lead_to_start, param_val));
  else if((param == "lead_damper") && (dval > 0)) {
    m_lead_damper = dval;
    return(true);
  }
  else if(param == "lead_condition") {
    param_val = findReplace(param_val, ',', '=');
    bool ok = true;
    LogicCondition new_condition;
    ok = new_condition.setCondition(param_val);
    if(ok)
      m_lead_conditions.push_back(new_condition);
    return(ok);
  }
  else if(param == "order") {
    if((param_val!="reverse") && (param_val!="reversed") && 
       (param_val!="normal") && (param_val!="toggle"))
      return(false);
    if(param_val=="toggle")
      m_waypoint_engine.setReverseToggle();
    else {
      bool reverse = ((param_val == "reverse") || (param_val == "reversed"));
      m_waypoint_engine.setReverse(reverse);
    }
    return(true);
  }
  else if((param == "repeat") && (tolower(param_val) == "forever")) {
    IvPBehavior::setParam("perpetual", "true");
    m_waypoint_engine.setRepeatsEndless(true);
    return(true);
  }
  else if(param == "efficiency_measure") {
    param_val = tolower(param_val);
    if((param_val == "all") || (param_val == "off")) {
      m_efficiency_measure = param_val;
      return(true);
    }
    return(false);
  }
  else if(param == "repeat") {
    int ival = atoi(param_val.c_str());
    if((ival < 0) || (!isNumber(param_val)))
      return(false);
    if(ival > 0)
      IvPBehavior::setParam("perpetual", "true");
    //    else
    //      IvPBehavior::setParam("perpetual", "false");
    m_waypoint_engine.setRepeat(ival);
    m_waypoint_engine.setRepeatsEndless(false);
    return(true);
  }
  else if((param == "radius") || (param == "capture_radius")) {
    if(dval <= 0)
      return(false);
    m_waypoint_engine.setCaptureRadius(dval);
    return(true);
  }
  else if((param=="nm_radius") || (param=="slip_radius")) {
    if(dval < 0)
      return(false);
    m_waypoint_engine.setNonmonotonicRadius(dval);
    return(true);
  }
  else if(param == "capture_line") {
    param_val = tolower(param_val);
    if(!isBoolean(param_val) && (param_val != "absolute"))
      return(false);
    if(param_val == "absolute") {
      param_val = "true";
      m_waypoint_engine.setSlipRadius(0);
      m_waypoint_engine.setCaptureRadius(0);
    }
    if(param_val == "true") 
      m_waypoint_engine.setCaptureLine(true);
    else
      m_waypoint_engine.setCaptureLine(false);
    return(true);
  }
  else if(param == "crs_spd_zaic_ratio") {
    // require dval such that course and speed pcts are each in [1,99]
    // and sum to 100.
    if((dval < 1) || (dval > 99))
      return(false);
    m_course_pct = dval;
    m_speed_pct = 100-dval;
    return(true);
  }

  else if(param == "visual_hints")  {
    vector<string> svector = parseStringQ(param_val, ',');
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++) 
      handleVisualHint(svector[i]);
    return(true);
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: onIdleToRunState

void BHV_Waypoint::onIdleToRunState() 
{
  if(!updateInfoIn()) 
    return;
  m_odo_leg_disq = true;
  
  // Set to true so odometry picks up from where it is now, not from
  // where it was when the behavior became idle
  m_odo_virgin = true;
}

//-----------------------------------------------------------
// Procedure: onRunToIdleState
//      Note: Invoked automatically by the helm when the behavior
//            first transitions from the Running to Idle state.

void BHV_Waypoint::onRunToIdleState() 
{
  postErasables();
  m_waypoint_engine.resetCPA();
  m_odo_set_flag = false;
  m_odo_leg_disq = true;

  // If we were publishing distances to prev or next waypoint, publish
  // one final time, values that indicate that we're not progressing on pts
  if(m_var_dist_to_prev != "")
    postMessage(m_var_dist_to_prev, -1);
  if(m_var_dist_to_next != "")
    postMessage(m_var_dist_to_next, -1);

}

//-----------------------------------------------------------
// Procedure: onRunState

BehaviorReport BHV_Waypoint::onRunState(string input) 
{
  IvPFunction *ipf = onRunState();
  BehaviorReport bhv_report(m_descriptor);
  
  bhv_report.addIPF(ipf);
  bhv_report.setPriority(m_priority_wt);
  return(bhv_report);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_Waypoint::onRunState() 
{
  m_waypoint_engine.setPerpetual(m_perpetual);

  // Set m_osx, m_osy, m_osv, osh
  if(!updateInfoIn()) {
    postMessage("VIEW_POINT", m_nextpt.get_spec("active=false"), "wpt");
    return(0);
  }
  updateOdoDistance();
  checkLeadConditions();

  // Note the waypoint prior to possibly incrementing the waypoint
  double this_x = m_waypoint_engine.getPointX();
  double this_y = m_waypoint_engine.getPointY();
  if(!m_prevpt.valid())
    m_prevpt.set_vertex(m_osx, m_osy);

  // Possibly increment the waypoint
  // Set m_nextpt, m_trackpt
  bool next_point = setNextWaypoint();
    
  // Update things if the waypoint was indeed incremented
  double next_x = m_waypoint_engine.getPointX();
  double next_y = m_waypoint_engine.getPointY();
  if((next_x != this_x) || (next_y != this_y)) {
    m_prevpt.set_vertex(this_x, this_y);
    postWptFlags(this_x, this_y);
  }


  // We want to report the updated cycle info regardless of the 
  // above result. Even if the next_point is false and there are
  // no more points, this means the cyindex is probably incremented.
  if(m_var_cyindex != "silent") {
    int waypt_cycles  = m_waypoint_engine.getCycleCount();
    if(waypt_cycles != m_prev_cycle_index) {
      postMessage((m_var_cyindex + m_var_suffix), waypt_cycles);
      m_prev_cycle_index = waypt_cycles;
    }
  }

  // Only publish these reports if we have another point to go.
  if(next_point) {
    postStatusReport();
    postViewableSegList();
    //postMessage("VIEW_POINT", m_prevpt.get_spec("active=true"), "prevpt");
    postMessage("VIEW_POINT", m_nextpt.get_spec("active=true"), "wpt");
    double dist = hypot((m_nextpt.x() - m_trackpt.x()), 
			(m_nextpt.y() - m_trackpt.y()));
    // If the trackpoint and next waypoint differ by more than five
    // meters then post a visual cue for the track point.
    if(dist > 5)
      postMessage("VIEW_POINT", m_trackpt.get_spec("active=true"), "trk");
    else
      postMessage("VIEW_POINT", m_trackpt.get_spec("active=false"), "trk");
    
  }
  // Otherwise "erase" the next waypoint marker
  else {
    postMessage("VIEW_POINT", m_nextpt.get_spec("active=false"), "wpt");
    return(0);
  }
  
  if(m_var_dist_to_prev != "")
    postMessage(m_var_dist_to_prev, m_waypoint_engine.distToPrevWpt(m_osx, m_osy));
  if(m_var_dist_to_next != "")
    postMessage(m_var_dist_to_next, m_waypoint_engine.distToNextWpt(m_osx, m_osy));

  IvPFunction *ipf = buildOF(m_ipf_type);
  if(ipf)
    ipf->setPWT(m_priority_wt);


  return(ipf);
}

//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_Waypoint::onIdleState() 
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

bool BHV_Waypoint::updateInfoIn()
{
  bool ok1, ok2, ok3, ok4;
  m_osx = getBufferDoubleVal("NAV_X",       ok1);
  m_osy = getBufferDoubleVal("NAV_Y",       ok2);
  m_osh = getBufferDoubleVal("NAV_HEADING", ok3);
  m_osv = getBufferDoubleVal("NAV_SPEED",   ok4);

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

  if(m_greedy_tour_pending) {
    m_greedy_tour_pending = false;
    XYSegList original_segl = m_waypoint_engine.getSegList();
    XYSegList shtour_segl = greedyPath(original_segl, m_osx, m_osy);
    m_waypoint_engine.setSegList(shtour_segl);
  }

  // If NAV_SPEED info is not found in the info_buffer, its
  // not a show-stopper. A warning will be posted.
  if(!ok4)
    postWMessage("No ownship speed info in info_buffer");

  return(true);
}


//-----------------------------------------------------------
// Procedure: setNextWaypoint

bool BHV_Waypoint::setNextWaypoint()
{
  if(m_waypoint_engine.size() == 0)
    return(false);

  m_waypoint_engine.setPrevPoint(m_prevpt);

  // Returns either: empty_seglist, completed, cycled, advanced, or in-transit
  string feedback_msg = m_waypoint_engine.setNextWaypoint(m_osx, m_osy);
  
  if(feedback_msg == "empty_seglist")
    return(false);
  if(feedback_msg != "in-transit")
    markOdoLeg();

  postMessage("WPT_ODO" + m_var_suffix, m_dist_leg_odo);

  if((feedback_msg=="completed") || (feedback_msg=="cycled")) {
    if(tolower(m_var_report) != "silent") {
      string feedback_msg_aug = "behavior-name=" + m_descriptor + ",";
      feedback_msg_aug += feedback_msg;
      postMessage((m_var_report + m_var_suffix), feedback_msg_aug);
    }
    
    postCycleFlags();
  }
   
  if(feedback_msg == "completed") {
    double this_x = m_waypoint_engine.getPointX();
    double this_y = m_waypoint_engine.getPointY();
    postWptFlags(this_x, this_y);
    
    setComplete();
    m_markpt.set_active(false);
    if(m_perpetual)
      m_waypoint_engine.resetForNewTraversal();
    return(false);
  }
  
  double next_ptx = m_waypoint_engine.getPointX();
  double next_pty = m_waypoint_engine.getPointY();
  m_nextpt.set_vertex(next_ptx, next_pty);

  // By default, the steering point is the next waypoint.
  m_trackpt.set_vertex(next_ptx, next_pty);

  // If m_lead_distance is non-neg, and we've already hit the
  // first waypoint, then steer to an intermediate point that
  // is m_lead_distance away from the perpendicular intersection
  // point between the current position and the trackline.
  
  if(m_lead_allowed && (m_lead_distance >= 0)) {
    int current_waypt = m_waypoint_engine.getCurrIndex();
    bool track_anchor = false;
    double tx, ty;
    if(current_waypt > 0) {
      tx = m_waypoint_engine.getPointX(current_waypt-1);
      ty = m_waypoint_engine.getPointY(current_waypt-1);
      track_anchor = true;
    }
    else if(m_waypoint_engine.getCycleCount() > 0) {
      unsigned int pt_count = m_waypoint_engine.size();
      tx = m_waypoint_engine.getPointX(pt_count-1);
      ty = m_waypoint_engine.getPointY(pt_count-1);
      track_anchor = true;
    }
    else if(m_lead_to_start) {
      if(!m_markpt.active()) {
	m_markpt.set_vertex(m_osx, m_osy);
	m_markpt.set_active(true);
      }	
      tx = m_markpt.x();
      ty = m_markpt.y();
      track_anchor = true;
    }

    if(track_anchor) {
      double nx, ny;
      perpSegIntPt(tx, ty, m_nextpt.x(), m_nextpt.y(), 
		   m_osx, m_osy, nx, ny);
      XYPoint perp_pt(nx, ny);
      
      double damper_factor = 1.0;
      if(m_lead_damper > 0) {
	double dist_to_trackline = hypot((nx-m_osx),(ny-m_osy));
	if(dist_to_trackline < m_lead_damper) {
	  double augment = 1 - (dist_to_trackline / m_lead_damper);
	  damper_factor += 2*augment;
	}
      }
	  
      double angle = relAng(tx, ty, m_nextpt.x(), m_nextpt.y());
      double dist  = distPointToPoint(nx, ny, m_nextpt.x(), m_nextpt.y());
      if(dist > (m_lead_distance * damper_factor)) 
	dist = m_lead_distance * damper_factor;  

      m_trackpt.projectPt(perp_pt, angle, dist);
    }
  }
  return(true);
}


//-----------------------------------------------------------
// Procedure: buildOF

IvPFunction *BHV_Waypoint::buildOF(string method) 
{
  IvPFunction *ipf = 0;
 
  double cruise_speed = m_cruise_speed;
  if((m_use_alt_speed) && (m_cruise_speed_alt >= 0))
    cruise_speed = m_cruise_speed_alt;
  
  if((method == "roc") || (method == "rate_of_closure")) {
    bool ok = true;
    AOF_Waypoint aof_wpt(m_domain);
    ok = ok && aof_wpt.setParam("desired_speed", cruise_speed);
    ok = ok && aof_wpt.setParam("osx", m_osx);
    ok = ok && aof_wpt.setParam("osy", m_osy);
    ok = ok && aof_wpt.setParam("ptx", m_trackpt.x());
    ok = ok && aof_wpt.setParam("pty", m_trackpt.y());
    ok = ok && aof_wpt.initialize();
    
    if(ok) {
      OF_Reflector reflector(&aof_wpt);
      reflector.create(600, 500);
      //reflector.setParam("uniform_piece", "discrete @ course:3,speed:3");
      //reflector.create();
      //string info = reflector.getUniformPieceStr();
      //postMessage("WAYPT_UPIECE", info);
      ipf = reflector.extractIvPFunction();
    }
  }    
  else { // if (method == "zaic","zaic_spd")

    IvPFunction *spd_ipf = 0;

    if(method == "zaic_spd") {
      ZAIC_SPD spd_zaic(m_domain, "speed");
      spd_zaic.setParams(m_cruise_speed, 0.1, m_cruise_speed+0.4, 70, 20);
      spd_ipf = spd_zaic.extractIvPFunction();
      //spd_ipf->getPDMap()->print();
      if(!spd_ipf)
	postWMessage("Failure on the SPD ZAIC via ZAIC_SPD utility");
    }
    else {
      ZAIC_PEAK spd_zaic(m_domain, "speed");
      double peak_width = m_cruise_speed / 2;
      spd_zaic.setParams(m_cruise_speed, peak_width, 1.6, 20, 0, 100);
      //    spd_zaic.setParams(m_cruise_speed, 1, 1.6, 20, 0, 100);               
      spd_ipf = spd_zaic.extractIvPFunction();
      if(!spd_ipf)
	postWMessage("Failure on the SPD ZAIC via ZAIC_PEAK utility");
    }    
    
    double rel_ang_to_trk_pt = relAng(m_osx, m_osy, m_trackpt.x(), m_trackpt.y());

    ZAIC_PEAK crs_zaic(m_domain, "course");
    crs_zaic.setValueWrap(true);
    crs_zaic.setParams(rel_ang_to_trk_pt, 0, 180, 50, 0, 100);
    
    int ix = crs_zaic.addComponent();
    crs_zaic.setParams(m_osh, 30, 180, 5, 0, 20, ix);
    
    IvPFunction *crs_ipf = crs_zaic.extractIvPFunction(false);

    //cout << "crs pieces: " << crs_ipf->getPDMap()->size() << endl;
    //crs_ipf->getPDMap()->print();

    
    if(!crs_ipf) 
      postWMessage("Failure on the CRS ZAIC");
    
    OF_Coupler coupler;
    ipf = coupler.couple(crs_ipf, spd_ipf, m_course_pct, m_speed_pct);
    if(!ipf)
      postWMessage("Failure on the CRS_SPD COUPLER");
  }    
  return(ipf);
}

//-----------------------------------------------------------
// Procedure: postStatusReport()

void BHV_Waypoint::postStatusReport()
{
  int    current_waypt = m_waypoint_engine.getCurrIndex();
  unsigned int waypt_cycles = m_waypoint_engine.getCycleCount();
  unsigned int total_hits   = m_waypoint_engine.getTotalHits();
  unsigned int capture_hits = m_waypoint_engine.getCaptureHits();

  double dist_meters   = hypot((m_osx - m_nextpt.x()), 
			       (m_osy - m_nextpt.y()));
  double eta_seconds   = dist_meters / m_osv;
  
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
    postMessage((m_var_report + m_var_suffix), stat);
  if(m_var_index != "silent") {
    if(current_waypt != m_prev_waypt_index) {
      postMessage((m_var_index + m_var_suffix), current_waypt);
      m_prev_waypt_index = current_waypt;
    }
  }
}


//-----------------------------------------------------------
// Procedure: postViewableSegList()
//      Note: Recall that for a seglist to be drawn and erased, 
//            it must match in the label. For a seglist to be 
//            "ignored" it must set active=false.

void BHV_Waypoint::postViewableSegList()
{
  XYSegList seglist = m_waypoint_engine.getSegList();
  seglist.set_label(m_us_name + "_" + m_descriptor);
  if(m_hint_vertex_color != "")
    seglist.set_color("vertex", m_hint_vertex_color);
  if(m_hint_edge_color != "")
    seglist.set_color("edge", m_hint_edge_color);
  if(m_hint_label_color != "")
    seglist.set_color("label", m_hint_label_color);
  if(m_hint_edge_size >= 0)
    seglist.set_edge_size(m_hint_edge_size);
  if(m_hint_vertex_size >= 0)
    seglist.set_vertex_size(m_hint_vertex_size);
  string segmsg = seglist.get_spec();
  postMessage("VIEW_SEGLIST", segmsg);
}

//-----------------------------------------------------------
// Procedure: postErasables
//      Note: Recall that for a seglist to be drawn and erased, 
//            it must match in the label. For a seglist to be 
//            "ignored" it must set active=false.

void BHV_Waypoint::postErasables()
{
  postMessage("VIEW_POINT", m_trackpt.get_spec("active=false"), "trk");
  postMessage("VIEW_POINT", m_nextpt.get_spec("active=false"), "wpt");

  XYSegList seglist = m_waypoint_engine.getSegList();
  seglist.set_label(m_us_name + "_" + m_descriptor);
  seglist.set_active(false);
  string segmsg = seglist.get_spec();
  postMessage("VIEW_SEGLIST", segmsg);
}


//-----------------------------------------------------------
// Procedure: postCycleFlags()

void BHV_Waypoint::postCycleFlags()
{
  int vsize = m_cycle_flags.size();
  for(int i=0; i<vsize; i++) {
    string var   = m_cycle_flags[i].get_var();
    string sdata = m_cycle_flags[i].get_sdata();
    double ddata = m_cycle_flags[i].get_ddata();
    if(m_cycle_flags[i].is_string())
      postRepeatableMessage(var, sdata);
    else
      postRepeatableMessage(var, ddata);
  }
}

//-----------------------------------------------------------
// Procedure: postWptFlags()

void BHV_Waypoint::postWptFlags(double x, double y)
{
  string xpos = doubleToStringX(x,2);
  string ypos = doubleToStringX(y,2);

  string nextx = doubleToStringX(m_nextpt.x(),2);
  string nexty = doubleToStringX(m_nextpt.y(),2);

  int vsize = m_wpt_flags.size();
  for(int i=0; i<vsize; i++) {
    string var   = m_wpt_flags[i].get_var();
    if(m_wpt_flags[i].is_string()) {
      string sdata = m_wpt_flags[i].get_sdata();

      sdata = findReplace(sdata, "$(X)", xpos);
      sdata = findReplace(sdata, "$(Y)", ypos);
      sdata = findReplace(sdata, "$[X]", xpos);
      sdata = findReplace(sdata, "$[Y]", ypos);

      sdata = findReplace(sdata, "$(NX)", nextx);
      sdata = findReplace(sdata, "$(NY)", nexty);
      sdata = findReplace(sdata, "$[NX]", nextx);
      sdata = findReplace(sdata, "$[NY]", nexty);
      postRepeatableMessage(var, sdata);
    }
    else {
      double ddata = m_wpt_flags[i].get_ddata();
      postRepeatableMessage(var, ddata);
    }
  }
}

//-----------------------------------------------------------
// Procedure: handleVisualHint()

void BHV_Waypoint::handleVisualHint(string hint)
{
  string param = tolower(stripBlankEnds(biteString(hint, '=')));
  string value = stripBlankEnds(hint);
  double dval  = atof(value.c_str());

  if((param == "vertex_size") && isNumber(value) && (dval >= 0))
    m_hint_vertex_size = dval;
  else if((param == "edge_size") && isNumber(value) && (dval >= 0))
    m_hint_edge_size = dval;
  else if((param == "vertex_color") && isColor(value))
    m_hint_vertex_color = value;
  else if((param == "edge_color") && isColor(value))
    m_hint_edge_color = value;
  else if((param == "label_color") && isColor(value))
    m_hint_label_color = value;
  else if((param == "nextpt_color") && isColor(value)) 
    m_hint_nextpt_color = value;
  else if((param == "nextpt_lcolor") && isColor(value)) 
    m_hint_nextpt_lcolor = value;
  else if((param == "nextpt_vertex_size") && isNumber(value) && (dval >=0))
    m_hint_nextpt_vertex_size = dval;
}

//-----------------------------------------------------------
// Procedure: checkLeadConditions()

void BHV_Waypoint::checkLeadConditions()
{
  if(!m_info_buffer) 
    return;
  
  // Phase 1: get all the variable names from all present conditions.
  vector<string> all_vars;
  unsigned int csize = m_lead_conditions.size();
  for(unsigned int i=0; i<csize; i++) {
    vector<string> svector = m_lead_conditions[i].getVarNames();
    all_vars = mergeVectors(all_vars, svector);
  }
  all_vars = removeDuplicates(all_vars);

  // Phase 2: get values of all variables from the info_buffer and 
  // propogate these values down to all the lead logic conditions.
  unsigned int vsize = all_vars.size();
  for(unsigned int i=0; i<vsize; i++) {
    string varname = all_vars[i];
    bool   ok_s, ok_d;
    string s_result = m_info_buffer->sQuery(varname, ok_s);
    double d_result = m_info_buffer->dQuery(varname, ok_d);

    for(unsigned int j=0; (j<csize)&&(ok_s); j++)
      m_lead_conditions[j].setVarVal(varname, s_result);
    for(unsigned int j=0; (j<csize)&&(ok_d); j++)
      m_lead_conditions[j].setVarVal(varname, d_result);
  }

  // Phase 3: evaluate all lead conditions. Return true only if all
  // conditions evaluate to be true.
  m_lead_allowed = true;
  for(unsigned int i=0; i<csize; i++) {
    bool satisfied = m_lead_conditions[i].eval();
    if(!satisfied) {
      m_lead_allowed = false;
    }
  }
}


//-----------------------------------------------------------
// Procedure: updateOdoDistance()

void BHV_Waypoint::updateOdoDistance()
{
  if(!m_odo_virgin) {
    double delta = distPointToPoint(m_osx, m_osy, m_osx_prev, m_osy_prev);
    m_dist_leg_odo += delta;
  }
  else
    m_odo_virgin = false;

  m_osx_prev = m_osx;
  m_osy_prev = m_osy;
}


//-----------------------------------------------------------
// Procedure: markOdoLeg()
//  Examples: 
//     WPT_EFFICIENCY_LEG = linear_dist=123.4, odo_dist=143.2, efficiency=86.2, 
//     WPT_EFFICIENCY_SUM = linear_dist=1883.1, odo_dist=2310.1, efficience=81.5
//     WPT_EFFICIENCY_VAL = 81.5

void BHV_Waypoint::markOdoLeg()
{
  if(m_efficiency_measure == "off")
    return;
  // First time through just mark the set point and return
  if(!m_odo_set_flag || m_odo_leg_disq) {
    m_odo_setx = m_osx;
    m_odo_sety = m_osy;
    m_odo_settime = getBufferCurrTime();
    m_odo_set_flag = true;
    m_dist_leg_odo = 0;
    m_odo_leg_disq = false;
    return;
  }

  // Part 1A: Note the straight line linear distance between the previously
  //          noted set point, and our present position
  double dist_leg_linear = distPointToPoint(m_osx, m_osy, m_odo_setx, m_odo_sety);
  m_dist_total_linear += dist_leg_linear;

  // Part 1B: Note the time since the previous noted set point
  double time_leg_linear = 0;
  if(m_cruise_speed > 0) {
    time_leg_linear = dist_leg_linear / m_cruise_speed;
    m_time_total_linear += time_leg_linear;
  }

  // Part 2A: Note the actual odometry distance travelled
  double dist_leg_odo = m_dist_leg_odo;
  m_dist_total_odo += dist_leg_odo;

  // Part 2B: Note the actual time travelled
  double time_leg_odo = getBufferCurrTime() - m_odo_settime;
  m_time_total_odo += time_leg_odo;


  // Part 3A: Calculate the Leg Odo Efficiency
  double leg_odo_efficiency = 0;
  if(dist_leg_odo > 0)
    leg_odo_efficiency = dist_leg_linear / dist_leg_odo;

  // Part 3B: Calculate the Leg Time Efficiency
  double leg_time_efficiency = 0;
  if(time_leg_odo > 0)
    leg_time_efficiency = time_leg_linear / time_leg_odo;

  // Part 4A: Calculate the  Total Odo Efficiency
  double total_dist_efficiency = 0;
  if(m_dist_total_odo > 0)
    total_dist_efficiency = m_dist_total_linear / m_dist_total_odo;

  // Part 4B: Calculate the Total Time Efficiency
  double total_time_efficiency = 0;
  if(m_time_total_odo > 0)
    total_time_efficiency = m_time_total_linear / m_time_total_odo;

  // Part 6: Build the leg output message:  WPT_EFFICIENCY_LEG
  string str = "vname=" + m_us_name;
  str += ", bhv_name=" + m_descriptor;
  str += ", linear_dist=" + doubleToStringX(dist_leg_linear, 1);
  str += ", odo_dist=" +  doubleToStringX(dist_leg_odo,1);
  str += ", efficiency_dist=" +  doubleToStringX(leg_odo_efficiency, 3);
  str += ", linear_time=" + doubleToStringX(time_leg_linear, 1);
  str += ", odo_time=" +  doubleToStringX(time_leg_odo,1);
  str += ", efficiency_time=" +  doubleToStringX(leg_time_efficiency, 3);
  postMessage("WPT_EFF_SUMM_LEG"+m_var_suffix, str);

  // Part 7: Build the total efficiency message: WPT_EFFICIENCY_SUM
  str = "vname=" + m_us_name;
  str += ", bhv_name=" + m_descriptor;
  str += ", linear_dist=" + doubleToStringX(m_dist_total_linear,1);
  str += ", odo_dist=" +  doubleToStringX(m_dist_total_odo,1);
  str += ", efficiency_dist=" +  doubleToStringX(total_dist_efficiency,3);
  str += ", linear_time=" + doubleToStringX(m_time_total_linear,1);
  str += ", odo_time=" +  doubleToStringX(m_time_total_odo,1);
  str += ", efficiency_time=" +  doubleToStringX(total_time_efficiency,3);
  postMessage("WPT_EFF_SUMM_ALL"+m_var_suffix, str);

  // Part 8: Post the simple total efficiency message: WPT_EFFICIENCY_DIST_VAL
  postMessage("WPT_EFF_DIST_ALL"+m_var_suffix, total_dist_efficiency);
  postMessage("WPT_EFF_TIME_ALL"+m_var_suffix, total_time_efficiency);
  postMessage("WPT_EFF_DIST_LEG"+m_var_suffix, leg_odo_efficiency);
  postMessage("WPT_EFF_TIME_LEG"+m_var_suffix, leg_time_efficiency);

  // Part 9: Reset the set point the present position
  m_odo_setx = m_osx;
  m_odo_sety = m_osy;
  m_odo_settime = getBufferCurrTime();

  m_dist_leg_odo = 0;
}








