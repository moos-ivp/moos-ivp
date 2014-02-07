/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_RIdentify.cpp                                    */
/*    DATE: April 2009                                           */
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
#include "BHV_RIdentify.h"
#include "OF_Reflector.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "AOF_Waypoint.h"
#include "GeomUtils.h"
#include "BuildUtils.h"
#include "FunctionEncoder.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"
#include "XYBuildUtils.h"
#include "IDUtils.h"
#include "PGEN_Star.h"
#include "PGEN_Pentagon.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_RIdentify::BHV_RIdentify(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  // First set variables at the superclass level
  m_descriptor = "bhv_waypoint";  
  m_domain     = subDomain(m_domain, "course,speed");

  // Set behavior configuration variables
  m_cruise_speed    = 0;  // Meters/second
  m_lead_distance   = -1; // meters - default of -1 means unused
  m_lead_damper     = -1; // meters - default of -1 means unused
  m_ipf_type        = "zaic";

  m_var_suffix      = "";
  m_var_idstream    = "";

  // The completed and perpetual vars are initialized in superclass
  // but we initialize here just to be safe and clear.
  m_completed     = false; 
  m_perpetual     = false;

  // Initialize state variables
  m_osx   = -1;
  m_osy   = -1;
  m_osv   = -1;
  m_osh   = -1;
  m_last_new_id = -1;

  m_last_busy_time = 0;

  // Once the behavior begins an RI pattern rotatable=false. Reset
  // to true when a new RI pattern is set 
  m_patt_rotatable = false; 
  m_patt_rotatable_ever = true;

  // The transiting state is similar to patt_rotatable except it is
  // not dependent on the patt_rotable_ever configuration.
  m_transiting = true;

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED");

  m_clustering      = false;
  m_greedy_traverse = true;

  m_hint_segl_edge_color = "white";

  m_pattern_generator = 0;
}

//-----------------------------------------------------------
// Procedure: Destructor

BHV_RIdentify::~BHV_RIdentify() 
{
  delete(m_pattern_generator);
}


//-----------------------------------------------------------
// Procedure: onSetParamComplete

void BHV_RIdentify::onSetParamComplete()
{
  m_trackpt.set_source(m_us_name + tolower(getDescriptor()));
  m_trackpt.set_label(m_us_name + "'s track-point");
  m_trackpt.set_type("track_point");
  m_trackpt.set_spec_digits(1);

  m_nextpt.set_source(m_us_name + tolower(getDescriptor()));
  m_nextpt.set_label(m_us_name + "'s next waypoint");
  m_nextpt.set_type("waypoint");
  m_nextpt.set_spec_digits(1);

  string result = setPatternParameters();
  
  return;
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition".
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_RIdentify::setParam(string param, string val) 
{
  double dval = atof(val.c_str());
  if(param == "id_points") {
    bool ok = handleNewIDPoints(val);
    return(ok);
  }
  else if(param == "id_stream") {
    val = stripBlankEnds(val);
    if(!strContainsWhite(val)) {
      m_var_idstream = val;
      addInfoVars(m_var_idstream);
      return(true);
    }
  }
  else if(param == "ripatt_rotatable")
    return(setBooleanOnString(m_patt_rotatable_ever, val));
  else if(param == "greedy_traverse")
    return(setBooleanOnString(m_greedy_traverse, val));
  else if(param == "clustering")
    return(setBooleanOnString(m_clustering, val));
  
  else if(param == "pattern_param") {
    string left  = stripBlankEnds(biteString(val, '='));
    string right = stripBlankEnds(val);
    if((left != "") && (right != "")) {
      m_pattern_params[left] = right;
      return(true);
    }
    return(false);
  }

  else if(param == "pattern_type") {
    val = stripBlankEnds(val);
    m_pattern_params["type"] = val;
    return(true);
  }

  else if(param == "speed") {
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    m_cruise_speed = dval;
    return(true);
  }
  else if(param == "post_suffix") {
    if(strContainsWhite(val))
      return(false);
    m_var_suffix = val;
    return(true);
  }
  else if(param == "ipf-type") {
    val = tolower(val);
    if((val=="zaic") || (val=="roc") || (val=="rate_of_closure"))
      m_ipf_type = val;
    return(true);
  }
  else if((param == "lead") && (dval > 0)) {
    m_lead_distance = dval;
    return(true);
  }
  else if(param == "lead_damper") {
    if((val == "off") || (val == "-1"))
      m_lead_damper = -1;
    else if(dval >= 0) 
      m_lead_damper = dval;
    else
      return(false);
    return(true);
  }
  else if(param == "repeat") {
    int ival = atoi(val.c_str());
    if((ival < 0) || (!isNumber(val)))
      return(false);
    m_waypoint_engine.setRepeat(ival);
    return(true);
  }
  else if((param == "radius") || (param == "capture_radius")) {
    if(dval <= 0)
      return(false);
    m_waypoint_engine.setCaptureRadius(dval);
    return(true);
  }
  else if((param == "nm_radius") && (dval > 0)) {
    m_waypoint_engine.setNonmonotonicRadius(dval);
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
// Procedure: onIdleState

void BHV_RIdentify::onIdleState() 
{
  updateInfoIn();
  postMessage("VIEW_POINT", m_nextpt.get_spec("active=false"));
  postMessage("VIEW_POINT", m_trackpt.get_spec("active=false"));
  postErasableSegList();
  postAwaitingPointsFlag();
  m_waypoint_engine.resetCPA();
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_RIdentify::onRunState() 
{
  if(m_pattern_generator == 0) {
    postWMessage("NULL Pattern Generator");
    return(0);
  }

  postAwaitingPointsFlag();
  m_waypoint_engine.setPerpetual(m_perpetual);

  bool active = true;
  
  // Set m_osx, m_osy, m_osv, m_osh
  if(!updateInfoIn())
    active = false;

  if(active) {
    if((m_waypoint_engine.size() == 0) || 
       m_waypoint_engine.isComplete()) {
      active = setNextRIPoint("completed_current");
    }
  }

  // Set m_nextpt, m_trackpt
  if(active && !setNextWaypoint())
    active = false;

  IvPFunction *ipf = 0;
  if(active) {
    ipf = buildOF(m_ipf_type);
    if(ipf)
      ipf->setPWT(m_priority_wt);
  }

  if(active) {
    postMessage("VIEW_POINT", m_nextpt.get_spec("active=true"), "nextpt");
    postMessage("VIEW_POINT", m_trackpt.get_spec("active=true"), "trackpt");
  }
  else {
    postMessage("VIEW_POINT", m_nextpt.get_spec("active=false"), "nextpt");
    postMessage("VIEW_POINT", m_trackpt.get_spec("active=false"), "trackpt");
  }

  postViewableSegList();
  postWaypointStatusReport();
  postIDStatusReport();

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

bool BHV_RIdentify::updateInfoIn()
{
  bool ok1, ok2, ok3, ok4;
  m_osx = getBufferDoubleVal("NAV_X",       ok1);
  m_osy = getBufferDoubleVal("NAV_Y",       ok2);
  m_osv = getBufferDoubleVal("NAV_SPEED",   ok3);
  m_osh = getBufferDoubleVal("NAV_HEADING", ok4);
  
  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2) {
    postEMessage("No ownship X/Y info in info_buffer.");
    return(false);
  }

  if(m_var_idstream != "") {
    bool ok;
    vector<string> id_streams = getBufferStringVector(m_var_idstream, ok);
    unsigned int i, vsize = id_streams.size();
    for(i=0; i<vsize; i++) 
      handleNewIDPoints(id_streams[i]);
  }


  // Would like to have the NAV_HEADING for perhaps optimizing the 
  // entry into the RI pattern, but its not a critical failure.
  if(!ok4) 
    postWMessage("No ownship NAV_HEADING info in info_buffer.");

  // If NAV_SPEED info is not found in the info_buffer, its
  // not a show-stopper. A warning will be posted.

  return(true);
}


//-----------------------------------------------------------
// Procedure: setNextWaypoint

bool BHV_RIdentify::setNextWaypoint()
{
  string feedback_msg = m_waypoint_engine.setNextWaypoint(m_osx, m_osy);

  if((feedback_msg=="completed") || (feedback_msg=="cycled")) {
    postMessage(("RI_WPT_STAT" + m_var_suffix), feedback_msg);

    // Note - this behavior never "completes" due to waypoints. At
    // best it is in a stand-by mode for more assignments. It may
    // however complete if the duration parameter is used, and the
    // perpetual parameter is false. As with any IvP behavior.

    return(false);
  }

  if(m_patt_rotatable)
    if(!m_pattern_generator->furtherModifiable(m_osx, m_osy))
      m_patt_rotatable = false;
  
  if(m_patt_rotatable)
    adjustRIPattern();
  else
    m_transiting = true;
  
  double ptx = m_waypoint_engine.getPointX();
  double pty = m_waypoint_engine.getPointY();
  ptx = snapToStep(ptx, 0.01);
  pty = snapToStep(pty, 0.01);
  m_nextpt.set_vertex(ptx, pty);

  // By default, the steering point is the next waypoint.
  m_trackpt.set_vertex(ptx, pty);

  // If m_lead_distance is non-neg, and we've already hit the
  // first waypoint, then steer to an intermediate point that
  // is m_lead_distance away from the perpendicular intersection
  // point between the current position and the trackline.
  
  if(m_lead_distance >= 0) {
    int current_waypt = m_waypoint_engine.getCurrIndex();
    if(current_waypt > 0) {
      double x1 = m_waypoint_engine.getPointX(current_waypt-1);
      double y1 = m_waypoint_engine.getPointY(current_waypt-1);

      double nx, ny;
      perpSegIntPt(x1,y1, ptx,pty, m_osx,m_osy, nx,ny);
      XYPoint perp_pt(nx, ny);

      double damper_factor = 1.0;
      if(m_lead_damper > 0) {
	double dist_to_trackline = hypot((nx-m_osx),(ny-m_osy));
	if(dist_to_trackline < m_lead_damper) {
	  double augment = 1 - (dist_to_trackline / m_lead_damper);
	  damper_factor += 2*augment;
	}
      }
      
      double angle = relAng(x1, y1, ptx, pty);
      double dist  = distPointToPoint(nx, ny, ptx, pty);
      if(dist > (m_lead_distance * damper_factor)) 
	dist = m_lead_distance * damper_factor;  
      m_trackpt.projectPt(perp_pt, angle, dist);
    }
  }
  return(true);
}


//-----------------------------------------------------------
// Procedure: buildOF

IvPFunction *BHV_RIdentify::buildOF(string method) 
{
  IvPFunction *ipf = 0;

  if((method == "roc") || (method == "rate_of_closure")) {
    bool ok = true;
    AOF_Waypoint aof_wpt(m_domain);
    ok = ok && aof_wpt.setParam("desired_speed", m_cruise_speed);
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
      string info = reflector.getUniformPieceStr();
      //postMessage("WAYPT_UPIECE", info);
      ipf = reflector.extractIvPFunction();
    }
  }    
  else { // if (method == "zaic")
    ZAIC_PEAK spd_zaic(m_domain, "speed");
    spd_zaic.setParams(m_cruise_speed, 0, 2.6, 0, 0, 100);
    IvPFunction *spd_of = spd_zaic.extractIvPFunction();
    if(!spd_of)
      postWMessage("Failure on the SPD ZAIC");
    
    double rel_ang_to_wpt = relAng(m_osx, m_osy, 
				   m_trackpt.x(), m_trackpt.y());
    ZAIC_PEAK crs_zaic(m_domain, "course");
    crs_zaic.setValueWrap(true);
    crs_zaic.setParams(rel_ang_to_wpt, 0, 180, 50, 0, 100);
    IvPFunction *crs_of = crs_zaic.extractIvPFunction();
    if(!crs_of)
      postWMessage("Failure on the CRS ZAIC");

    OF_Coupler coupler;
    ipf = coupler.couple(crs_of, spd_of);
    if(!ipf)
      postWMessage("Failure on the COUPLER");
  }    
  return(ipf);
}

//-----------------------------------------------------------
// Procedure: postWaypointStatusReport()

void BHV_RIdentify::postWaypointStatusReport()
{
  int    current_waypt = m_waypoint_engine.getCurrIndex();
  double dist_meters   = hypot((m_osx - m_nextpt.x()), 
			       (m_osy - m_nextpt.y()));
  double eta_seconds   = dist_meters / m_osv;
  
  string stat = "vname=" + m_us_name + ",";
  stat += "index=" + intToString(current_waypt)   + ",";
  stat += "dist="  + doubleToString(dist_meters, 0)  + ",";
  stat += "eta="   + doubleToString(eta_seconds, 0);
  
  postMessage(("RI_WPT_STAT" + m_var_suffix), stat);
  postMessage(("RI_WPT_INDEX" + m_var_suffix), current_waypt);
}

//-----------------------------------------------------------
// Procedure: postIDStatusReport()

void BHV_RIdentify::postIDStatusReport()
{
  string idstat = "done=";
  string active_stat = "";
  list<XYPoint>::iterator p;
  if(m_done_pts.size() == 0)
    idstat += "none";
  for(p=m_done_pts.begin(); p!=m_done_pts.end(); p++) {
    if(p!=m_done_pts.begin())
      idstat += ",";
    idstat += p->get_label();
  }
  
  idstat += " # active=";
  if(m_active_pts.size() == 0) {
    active_stat = "none";
    idstat += "none";
  }
  for(p=m_active_pts.begin(); p!=m_active_pts.end(); p++) {
    if(p!=m_active_pts.begin()) {
      idstat += ",";
      active_stat += ",";
    }
    idstat += p->get_label();
    active_stat += p->get_label();
  }

  idstat += " # pending=";
  if(m_todo_pts.size() == 0)
    idstat += "none";
  for(p=m_todo_pts.begin(); p!=m_todo_pts.end(); p++) {
    if(p!=m_todo_pts.begin())
      idstat += ",";
    idstat += p->get_label();
  }

  postMessage(("RI_ID_STAT" + m_var_suffix), idstat);
  postMessage(("RI_ID_INDEX" + m_var_suffix), active_stat);

  if(m_transiting)
    postMessage("RI_TRANSITING", "true");
  else
    postMessage("RI_TRANSITING", "false");
  
  postMessage("RI_PTS_COMPLETED", m_done_pts.size());
}


//-----------------------------------------------------------
// Procedure: postViewableSegList()
//      Note: Recall that for a seglist to be drawn and erased, 
//            it must match in the label. For a seglist to be 
//            "ignored" it must set active=false.

void BHV_RIdentify::postViewableSegList()
{
  XYSegList seglist = m_waypoint_engine.getSegList();
  seglist.set_label(m_us_name + "_" + m_descriptor);
  seglist.set_color("vertex", m_hint_segl_vertex_color);

  if(m_patt_rotatable)
    seglist.set_color("edge", m_hint_segl_edge_color);
  else
    seglist.set_color("edge", "tan");
    
  string segmsg = seglist.get_spec();
  postMessage("VIEW_SEGLIST", segmsg);
}


//-----------------------------------------------------------
// Procedure: postErasableSegList()
//      Note: Recall that for a seglist to be drawn and erased, 
//            it must match in the label. For a seglist to be 
//            "ignored" it must set active=false.

void BHV_RIdentify::postErasableSegList()
{
  XYSegList seglist = m_waypoint_engine.getSegList();
  seglist.set_label(m_us_name + "_" + m_descriptor);
  seglist.set_active(false);
  string segmsg = seglist.get_spec();
  postMessage("VIEW_SEGLIST", segmsg);
}

//-----------------------------------------------------------
// Procedure: postAwaitingPointsFlag

void BHV_RIdentify::postAwaitingPointsFlag()
{
  bool awaiting_points = false;
  if((m_todo_pts.size()==0) && (m_active_pts.size()==0))
    awaiting_points = true;
  
  if(awaiting_points)
    postMessage("RI_AWAITING_POINTS", "true");
  else
    postMessage("RI_AWAITING_POINTS", "false");

  double current_time = getBufferCurrTime();

  if(!awaiting_points || (m_last_busy_time==0))
    m_last_busy_time = current_time;

  double time_waiting = current_time - m_last_busy_time;
  
  postIntMessage("RI_AWAITING_POINTS_TIME", time_waiting);
}

//-----------------------------------------------------------
// Procedure: postViewableIDPoints()

void BHV_RIdentify::postViewableIDPoints(string ptype)
{
  // Recall that for a point to be drawn and erased, it must match
  // in the (1) label (2) type (3) and source.

  list<XYPoint>::iterator p;
  if((ptype=="all") || (ptype=="done")) {
    for(p=m_done_pts.begin(); p!=m_done_pts.end(); p++)
      postMessage("VIEW_POINT", p->get_spec());
  }
  if((ptype=="all") || (ptype=="active")) {
    for(p=m_active_pts.begin(); p!=m_active_pts.end(); p++)
      postMessage("VIEW_POINT", p->get_spec());
  }
  if((ptype=="all") || (ptype=="todo")) {
    for(p=m_todo_pts.begin(); p!=m_todo_pts.end(); p++)
      postMessage("VIEW_POINT", p->get_spec());
  }
}


//-----------------------------------------------------------
// Procedure: handleNewIDPoints
// Example::
//     "x=0,y=8,id-01, ... # x=2,y=4,id=09,spec"

bool BHV_RIdentify::handleNewIDPoints(string ptspecs)
{
  vector<string> svector = parseStringQ(ptspecs, '#');
  unsigned int i, vsize = svector.size();
  
  vector<XYPoint> new_points;

  for(i=0; i<vsize; i++) {
    svector[i] = stripBlankEnds(svector[i]);
    vector<string> jvector = parseStringQ(svector[i], ',');
    unsigned int j, jsize = jvector.size();

    double xpos=0;    bool xpos_set=false;
    double ypos=0;    bool ypos_set=false;
    string ptid=""; 
    string spec=""; 

    for(j=0; j<jsize; j++) {
      string left = tolower(stripBlankEnds(biteString(jvector[j], '=')));
      string right = stripBlankEnds(jvector[j]);
      if((left == "x") && (isNumber(right))) {
	xpos = atof(right.c_str());
	xpos_set = true;
      }
      if((left == "y") && (isNumber(right))) {
	ypos = atof(right.c_str());
	ypos_set = true;
      }
      if(left == "id")
	ptid = right;
      if(left == "spec")
	spec = right;
    }
    if(!xpos_set || !ypos_set)
      return(false);
    if(ptid == "") {
      m_last_new_id++;
      ptid = "AUTO-" + intToString(m_last_new_id);
    }

    XYPoint new_point(xpos, ypos);
    new_point.set_label(ptid);
    new_point.set_type(spec);
    new_point.set_spec_digits(1);
    new_point.set_color("vertex", m_hint_idpt_color);
    new_point.set_color("label", m_hint_idpt_lcolor);
    new_points.push_back(new_point);    
  }

  // Make a list of all the "new" points and remove any points
  // that have an ID that has already been received.
  list<XYPoint> new_todo_pts;
  
  vsize = new_points.size();
  for(i=0; i<vsize; i++) {
    string label_id = new_points[i].get_label();

    bool id_done = false;
    list<XYPoint>::iterator p;
    for(p=m_done_pts.begin(); p!=m_done_pts.end(); p++)
      if(p->get_label() == label_id)
	id_done = true;

    bool id_active = false;
    for(p=m_active_pts.begin(); p!=m_active_pts.end(); p++)
      if(p->get_label() == label_id)
	id_active = true;

    bool id_todo = false;
    for(p=m_todo_pts.begin(); p!=m_todo_pts.end(); p++)
      if(p->get_label() == label_id)
	id_todo = true;
    
    if(!id_done && !id_active && !id_todo)
      new_todo_pts.push_back(new_points[i]);
  }
  
  list<XYPoint>::iterator p;
  for(p=new_todo_pts.begin(); p!=new_todo_pts.end(); p++)
    m_todo_pts.push_back(*p);

  // If we're still transiting to an active RID point(s), then 
  // push the active point(s) back on the to-do list and recalc
  // the walk - only if greedy_traverse is true
  if(m_greedy_traverse && m_patt_rotatable)
    setNextRIPoint("reconsider_current");
    

  postViewableIDPoints();
  return(true);
}

//-----------------------------------------------------------
// Procedure: setNextRIPoint()
//     Notes: This procedure is called when an RID point has 
//            been completed and a new one needs to be set to
//            active.


bool BHV_RIdentify::setNextRIPoint(string situation)
{
  if(situation == "completed_current") {
    unsigned int i, vsize = m_active_pts.size();
    for(i=0; i<vsize; i++) {
      XYPoint pt = m_active_pts.front();
      pt.set_color("vertex", "green");
      m_done_pts.push_back(pt);
      m_active_pts.pop_front();
    }
    if(m_todo_pts.size() == 0) {
      postViewableIDPoints("done");
      return(false);
    }
  }
  else if(situation == "reconsider_current") {
    // Push all the active points back on the to-do list
    list<XYPoint>::iterator p;
    for(p=m_active_pts.begin(); p!=m_active_pts.end(); p++) {
      p->set_color("vertex", m_hint_idpt_color);
      m_todo_pts.push_back(*p);
    }
    m_active_pts.clear(); 
  }
  else 
    return(false);

  // Possibly order the to-do points based on a greedy walk thru
  // the points, always choosing the next closest point.
  if(m_greedy_traverse)
    m_todo_pts = simplePointWalk(m_todo_pts, m_osx, m_osy);

  // Here is where the pattern engine gets its marching orders
  if(!m_clustering) {
    m_pattern_generator->clear();
    m_curr_ript = m_todo_pts.front();
    m_todo_pts.pop_front();
    m_curr_ript.set_color("vertex", "Fuchsia");
    m_active_pts.push_back(m_curr_ript);
    m_pattern_generator->addIDPoint(m_curr_ript);
  }
  else {
    m_pattern_generator->clear();
    XYCircle bcirc = boundingCircle(m_todo_pts, 20, 5);
    bcirc.alterRad(0.1); // floating point precision buffer
    if(bcirc.getRad() > 0)
      postMessage("VIEW_CIRCLE", bcirc.get_spec());

    list<XYPoint>::iterator p;
    bool done = false;
    for(p=m_todo_pts.begin(); (!done && (p!=m_todo_pts.end())); ) {
      XYPoint iter_pt = *p;
      if(bcirc.containsPoint(iter_pt)) {
	p = m_todo_pts.erase(p);
	iter_pt.set_color("vertex", "Fuchsia");
	m_active_pts.push_back(iter_pt);
	bool pt_accepted = m_pattern_generator->addIDPoint(iter_pt);
	if(!pt_accepted)
	  done = true;
      }
      else {
	++p;
	done = true;
      }
    }
  }

  // Potentially reset m_patt_rotatable=true if it is allowed in 
  // the general case (m_patt_rotatable_ever==true).
  m_patt_rotatable = m_patt_rotatable_ever;
  m_transiting = true;

  adjustRIPattern();

  postViewableIDPoints("all");
  return(true);
}

//-----------------------------------------------------------
// Procedure: adjustRIPattern()

void BHV_RIdentify::adjustRIPattern()
{
  XYSegList new_segl;
  new_segl = m_pattern_generator->generatePattern(m_osx, m_osy, m_osh);
  m_waypoint_engine.setSegList(new_segl);
}

//-----------------------------------------------------------
// Procedure: handleVisualHint()

void BHV_RIdentify::handleVisualHint(string hint)
{
  string param = tolower(stripBlankEnds(biteString(hint, '=')));
  string value = stripBlankEnds(hint);

  if((param == "idpt_color") && isColor(value))
    m_hint_idpt_color = value;
  else if((param == "idpt_lcolor") && isColor(value))
    m_hint_idpt_lcolor = value;

  else if((param == "nextpt_color") && isColor(value))
    m_nextpt.set_color("vertex", value);
  else if((param == "nextpt_lcolor") && isColor(value))
    m_nextpt.set_color("label", value);

  else if((param == "trackpt_color") && isColor(value))
    m_trackpt.set_color("vertex", value);
  else if((param == "trackpt_lcolor") && isColor(value))
    m_trackpt.set_color("label", value);

  else if((param == "segl_vertex_color") && isColor(value))
    m_hint_segl_vertex_color = value;
  else if((param == "segl_edge_color") && isColor(value))
    m_hint_segl_edge_color = value;
}


//-----------------------------------------------------------
// Procedure: setPatternParameters()

string BHV_RIdentify::setPatternParameters()
{
  PatternGenerator *pgen = 0;

  string ptype = m_pattern_params["type"];
  if(ptype == "")
    return("Unspecified RI pattern type");
  
  if(ptype == "star")
    pgen = new PGEN_Star();
  else if(ptype == "pentagon")
    pgen = new PGEN_Pentagon();
  else
    return("Unknown pattern type: " + ptype);

  map<string, string>::iterator p;
  for(p=m_pattern_params.begin(); p!=m_pattern_params.end(); p++) {
    string left  = p->first;
    string right = p->second;

    // The "type" parameter is special - used only determining the 
    // class of the generator. Ignore here.
    if(left != "type") {
      bool ok = pgen->PatternGenerator::setParam(left, right);
      if(!ok)
	ok = pgen->setParam(left, right);
      if(!ok) 
	return("Problem w/ RI pattern param-value: " + left + ", " + right);
    }
  }
  // If the new PatternGenerator was created with no errors, go
  // ahead and make it the new generator for this behavior.
  delete(m_pattern_generator);
  m_pattern_generator = pgen;

  // Clear the list of parameters once a new PatternGenerator has
  // been created. So that further calls, which may have a different
  // type will not use the prior parameters.
  m_pattern_params.clear();
  return("");
}

