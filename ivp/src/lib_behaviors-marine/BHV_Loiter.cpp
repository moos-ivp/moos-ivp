/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_Loiter.cpp                                       */
/*    DATE: July 26th 2005 In Elba w/ M.Grund, P.Newman          */
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

#include <cmath>
#include <cstdlib>
#include "AngleUtils.h"
#include "BHV_Loiter.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "XYFormatUtilsPoly.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"
#include "XYPoint.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Loiter::BHV_Loiter(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  // First set variables at the superclass level
  m_descriptor = "bhv_loiter";  
  m_domain     = subDomain(m_domain, "course,speed");

  // Initialize State Variable with meaningful initial vals
  m_acquire_mode    = true;
  m_loiter_mode     = "idle";

  // Initialize State Variables with initial vals of zero which
  // have no real meaning, but better than nondeterministic.
  m_osx             = 0;
  m_osy             = 0;
  m_osh             = 0;
  m_ptx             = 0;
  m_pty             = 0;
  m_dist_to_poly    = 0;
  m_eta_to_poly     = 0;

  // Initialize Configuration Parameters
  m_desired_speed     = 0;      // meters per sec
  m_clockwise         = true;
  m_dynamic_clockwise = false;
  m_acquire_dist      = 10;
  m_center_pending    = false;
  m_center_activate   = false;


  // Visual Hint Defaults
  m_hint_vertex_size   = 1;
  m_hint_edge_size     = 1;
  m_hint_vertex_color  = "dodger_blue";
  m_hint_edge_color    = "white";
  m_hint_nextpt_color  = "yellow";
  m_hint_nextpt_lcolor = "aqua";
  m_hint_nextpt_vertex_size = 5;

  m_waypoint_engine.setPerpetual(true);
  m_waypoint_engine.setRepeatsEndless(true);

  addInfoVars("NAV_X, NAV_Y, NAV_HEADING, NAV_SPEED");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_Loiter::setParam(string param, string value) 
{
  if(param == "polygon") {
    XYPolygon new_poly = string2Poly(value);
    if(!new_poly.is_convex())  // Should be convex - false otherwise
      return(false);
    new_poly.apply_snap(0.1); // snap to tenth of meter
    m_loiter_engine.setPoly(new_poly);
    m_loiter_engine.setClockwise(m_clockwise);
    m_waypoint_engine.setSegList(m_loiter_engine.getPolygon());
    m_acquire_mode  = true;
    return(true);
  }  
  else if(param == "center_assign") {
    m_center_assign  = value;
    m_center_pending = true;
    updateCenter(); // Added by mikerb 08/29/12 to ensure updates are immediate
    return(true);
  }  
  else if(param == "xcenter_assign") {
    m_center_assign  += (",x=" + value);
    m_center_pending = true;
    return(true);
  }  
  else if(param == "ycenter_assign") {
    m_center_assign  += (",y=" + value);
    m_center_pending = true;
    return(true);
  }  
  else if(param == "center_activate") {
    value = tolower(value);
    if((value!="true")&&(value!="false"))
      return(false);
    m_center_activate = (value == "true");
    return(true);
  }  
  else if(param == "clockwise") {
    if(tolower(value) == "best") {
      m_dynamic_clockwise = true;
      return(true);
    }
    bool ok = setBooleanOnString(m_clockwise, value);
    if(!ok)
      return(false);
    m_dynamic_clockwise = false;
    m_loiter_engine.setClockwise(m_clockwise);
    m_waypoint_engine.setSegList(m_loiter_engine.getPolygon());
    return(true);
  }  
  else if(param == "speed") {
    double dval = atof(value.c_str());
    if((dval < 0) || (!isNumber(value)))
      return(false);
    m_desired_speed = dval;
    return(true);
  }
  else if((param == "radius") || (param == "capture_radius")) {
    double dval = atof(value.c_str());
    if((dval < 0) || (!isNumber(value)))
      return(false);
    m_waypoint_engine.setCaptureRadius(dval);
    return(true);
  }
  else if((param == "acquire_dist") || (param == "acquire_distance")) {
    double dval = atof(value.c_str());
    if((dval < 0) || (!isNumber(value)))
      return(false);
    m_acquire_dist = dval;
    return(true);
  }
  else if((param == "nm_radius") || (param == "slip_radius")) {
    double dval = atof(value.c_str());
    // val=0 is ok, interpreted as inactive
    if((dval < 0) || (!isNumber(value)))
      return(false);
    m_waypoint_engine.setNonmonotonicRadius(dval);
    return(true);
  }
  else if(param == "post_suffix")  {
    if(strContainsWhite(value) || (value == ""))
      return(false);
    m_var_suffix = "_" + toupper(value);
    return(true);
  }
  else if(param == "visual_hints")  {
    vector<string> svector = parseStringQ(value, ',');
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++) 
      handleVisualHint(svector[i]);
    return(true);
  }
  else if(param == "spiral_factor")  {
    m_loiter_engine.setSpiralFactor(atof(value.c_str()));
    return(true);
  }
  return(false);
}

//-----------------------------------------------------------
// Procedure: onCompleteState
//      Note: Invoked once by helm prior to behavior deletion.

void BHV_Loiter::onCompleteState()
{
  postErasablePoint();
  postErasablePolygon();
}


//-----------------------------------------------------------
// Procedure: onIdleState
//      Note: If m_center_pending is true, each time the behavior
//            goes inactive (and thus this function is called), 
//            a pending new center is declared, and set to the 
//            special value of present_position, which will be 
//            determined when the activation occurs.

void BHV_Loiter::onIdleState()
{
  postErasablePoint();
  postErasablePolygon();

  m_loiter_mode = "idle";
  postStatusReports();

  if(!m_center_activate)
    return;
  m_center_pending = true;
  m_center_assign  = "present_position";
}


//-----------------------------------------------------------
// Procedure: onIdleToRunState
//      Note: 

void BHV_Loiter::onIdleToRunState()
{
  postConfigStatus();

  // Fix made by HS, to avoid looping back to earlier approach vertex
  updateInfoIn();
  int curr_waypt = m_loiter_engine.acquireVertex(m_osh, m_osx, m_osy); 
  m_waypoint_engine.setCurrIndex(curr_waypt);

  if(m_dynamic_clockwise) {
    m_loiter_engine.resetClockwiseBest(m_osh, m_osx, m_osy);
    m_clockwise = m_loiter_engine.getClockwise();
    m_waypoint_engine.setSegList(m_loiter_engine.getPolygon());
  }
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_Loiter::onRunState() 
{
  // Set m_osx, m_osy, m_osh, m_osv
  if(!updateInfoIn()) {
    postErasablePoint();
    return(0);
  }

  updateLoiterMode();
  updateCenter();

  m_acquire_mode = false;
  if(m_dist_to_poly > m_acquire_dist)
    m_acquire_mode = true;

  if(m_acquire_mode) {
    int curr_waypt = m_loiter_engine.acquireVertex(m_osh, m_osx, m_osy); 
    m_waypoint_engine.setCurrIndex(curr_waypt);
  }
  
  string feedback_msg = m_waypoint_engine.setNextWaypoint(m_osx, m_osy);
  if((feedback_msg == "advanced") || (feedback_msg == "cycled"))
    m_acquire_mode = false;
  
  m_ptx = m_waypoint_engine.getPointX();
  m_pty = m_waypoint_engine.getPointY();

  IvPFunction *ipf = buildIPF("zaic");

  if(ipf) {
    ipf->getPDMap()->normalize(0,100);
    ipf->setPWT(m_priority_wt);
  }

  postViewablePolygon();
  postViewablePoint();
  postStatusReports();
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

bool BHV_Loiter::updateInfoIn()
{
  // Empty poly perhaps means standby for dynamic specification.
  XYPolygon poly = m_loiter_engine.getPolygon();
  if(poly.size() == 0) {
    postWMessage("Empty/NULL Loiter Specification.");
    return(false);
  }

#if 0  
  int rval = rand() % 1000;
  if((rval % 53) == 0)
    postWMessage("Test warning message: " + intToString(rval));
  if((rval % 307) == 0)
    postEMessage("Test Error message: " + intToString(rval));
#endif

  bool ok1, ok2, ok3, ok4;
  // ownship position in meters from some 0,0 reference point.
  m_osx = getBufferDoubleVal("NAV_X", ok1);
  m_osy = getBufferDoubleVal("NAV_Y", ok2);
  m_osh = getBufferDoubleVal("NAV_HEADING", ok3);
  m_osv = getBufferDoubleVal("NAV_SPEED", ok4);

  // Must get ownship information from the InfoBuffer
  if(!ok1 || !ok2)
    postEMessage("No ownship X/Y info in info_buffer.");  
  if(!ok3)
    postEMessage("No ownship HEADING info in info_buffer.");
  if(!ok4)
    postEMessage("No ownship SPEED info in info_buffer.");
  if(!ok1 || !ok2 || !ok3 || !ok4)
    return(false);
  
  // Calculate the distance to the polygon. A point internal to the
  // polygon is regarded as having a negative distance, the distance
  // back out to the closest polygon edge.
  m_dist_to_poly = poly.dist_to_poly(m_osx, m_osy);
  if(poly.contains(m_osx, m_osy))
    m_dist_to_poly *= -1.0;

  // Calculating ETA to poly - should we base it on progress history?
  // Or simply distance and current speed?

  m_eta_to_poly = 0;
  if((m_dist_to_poly > 0) && (m_osh > 0.001))
    m_eta_to_poly = (m_dist_to_poly / m_osv);

  return(true);
}

//-----------------------------------------------------------
// Procedure: updateCenter()
//      Note: Can handle strings of type:
//              "present_position"
//              "54.0,-120"
//              "x=54.0, y=-120"
//              "x=54.0"
//              "y=-120"

void BHV_Loiter::updateCenter()
{
  if(!m_center_pending)
    return;
  
  m_center_assign = tolower(m_center_assign);

  // Handle updates of the type: "present_position"
  if(m_center_assign == "present_position") {
    m_loiter_engine.setCenter(m_osx, m_osy);
    m_waypoint_engine.setCenter(m_osx, m_osy);
  }
  // Handle updates of the type: "x=45" and "x=54,y=-105"
  else if(strContains(m_center_assign, '=')) {
    double pending_center_x = m_loiter_engine.getCenterX();
    double pending_center_y = m_loiter_engine.getCenterY();
    bool   update_needed = false;

    vector<string> svector = parseString(m_center_assign, ',');
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++) {
      vector<string> ivector = parseString(svector[i], '=');
      unsigned int isize = ivector.size();
      if(isize == 2) {
	string left  = stripBlankEnds(ivector[0]);
	string right = stripBlankEnds(ivector[1]);
	if((left == "x") && isNumber(right)) {
	  pending_center_x = atof(right.c_str());
	  update_needed = true;
	}
	else if((left == "y") && isNumber(right)) {
	  pending_center_y = atof(right.c_str());
	  update_needed = true;
	}
      }
    }
    if(update_needed) {
      m_loiter_engine.setCenter(pending_center_x, pending_center_y);
      m_waypoint_engine.setCenter(pending_center_x, pending_center_y);
    }
  } 
  // Handle updates of the type: "54,-105"
  else {
    vector<string> svector = parseString(m_center_assign, ',');
    if(svector.size() == 2) {
      if(isNumber(svector[0]) && isNumber(svector[1]))
	svector[0] = stripBlankEnds(svector[0]);
	svector[1] = stripBlankEnds(svector[1]);
	double xval = atof(svector[0].c_str());
	double yval = atof(svector[1].c_str());
	m_loiter_engine.setCenter(xval, yval);
	m_waypoint_engine.setCenter(xval, yval);
    }
  }

  m_center_assign = "";
  m_center_pending = false;
}


//-----------------------------------------------------------
// Procedure: updateLoiterMode

void BHV_Loiter::updateLoiterMode()
{
  string position = "onpoly";
  if(m_dist_to_poly > m_acquire_dist)
    position = "outer";
  else if(m_dist_to_poly < (-1 * m_acquire_dist))
    position = "inner";
  
  if(position == "onpoly")
    m_loiter_mode = "stable";
  else if(position == "outer") {
    if(m_loiter_mode == "stable")
      m_loiter_mode = "recovering_external";
    else if(m_loiter_mode == "recovering_external")
      m_loiter_mode = "recovering_external";
    else
      m_loiter_mode = "acquiring_external";
  }
  else if(position == "inner") {
    if(m_loiter_mode == "stable")
      m_loiter_mode = "recovering_internal";
    else if(m_loiter_mode == "recovering_internal")
      m_loiter_mode = "recovering_internal";
    else 
      m_loiter_mode = "acquiring_internal";
  }
  else 
    postWMessage("Undefined LoiterMode");

}

//-----------------------------------------------------------
// Procedure: buildIPF

IvPFunction *BHV_Loiter::buildIPF(const string& method) 
{
  IvPFunction *ipf = 0;
  
  if(method == "zaic") {    
    ZAIC_PEAK spd_zaic(m_domain, "speed");
    spd_zaic.setSummit(m_desired_speed);
    spd_zaic.setBaseWidth(0.3);
    spd_zaic.setPeakWidth(0.0);
    spd_zaic.setSummitDelta(0.0);
    IvPFunction *spd_of = spd_zaic.extractIvPFunction();
    double rel_ang_to_wpt = relAng(m_osx, m_osy, m_ptx, m_pty);
    ZAIC_PEAK crs_zaic(m_domain, "course");
    crs_zaic.setSummit(rel_ang_to_wpt);
    crs_zaic.setBaseWidth(180.0);
    crs_zaic.setValueWrap(true);
    IvPFunction *crs_of = crs_zaic.extractIvPFunction();
    OF_Coupler coupler;
    ipf = coupler.couple(crs_of, spd_of);
  }

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: postStatusReports()

void BHV_Loiter::postStatusReports()
{
  unsigned int nonmono_hits = m_waypoint_engine.getNonmonoHits();
  unsigned int capture_hits = m_waypoint_engine.getCaptureHits();
  int curr_index   = m_waypoint_engine.getCurrIndex();

  string loiter_report = "index=" + intToString(curr_index);
  loiter_report += ",capture_hits=" + uintToString(capture_hits);
  loiter_report += ",nonmono_hits=" + uintToString(nonmono_hits);
  loiter_report += ",acquire_mode=" + boolToString(m_acquire_mode);

  postMessage(("LOITER_REPORT" + m_var_suffix), loiter_report);
  postMessage(("LOITER_INDEX" + m_var_suffix), curr_index);
  postMessage(("LOITER_MODE" + m_var_suffix), m_loiter_mode);

  if(m_acquire_mode)
    postMessage(("LOITER_ACQUIRE" + m_var_suffix), 1);
  else
    postMessage(("LOITER_ACQUIRE" + m_var_suffix), 0);
  
  string dist_var = ("LOITER_DIST_TO_POLY" + m_var_suffix);
  if(m_dist_to_poly > 10)
    postIntMessage(dist_var, m_dist_to_poly);
  else
    postMessage(dist_var, m_dist_to_poly);

  string eta_var = ("LOITER_ETA_TO_POLY" + m_var_suffix);
  if((m_eta_to_poly > 10) || (m_eta_to_poly == 0))
    postIntMessage(eta_var, m_eta_to_poly);
  else
    postMessage(eta_var, m_eta_to_poly);
}

//-----------------------------------------------------------
// Procedure: postViewablePolygon()
//      Note: Even if the polygon is posted on each iteration, the
//            helm will filter out unnecessary duplicate posts.

void BHV_Loiter::postViewablePolygon()
{
  XYSegList seglist = m_waypoint_engine.getSegList();
  seglist.set_color("vertex", m_hint_vertex_color);
  seglist.set_color("edge", m_hint_edge_color);
  seglist.set_edge_size(m_hint_edge_size);
  seglist.set_vertex_size(m_hint_vertex_size);
  // Handle the label setting
  string bhv_tag = tolower(getDescriptor());
  bhv_tag = m_us_name + "_" + bhv_tag;
  seglist.set_label(bhv_tag);
  if(m_hint_poly_label == "")
    seglist.set_label(bhv_tag);
  else
    seglist.set_label(m_hint_poly_label);

  string poly_spec = seglist.get_spec();
  postMessage("VIEW_POLYGON", poly_spec);
}

//-----------------------------------------------------------
// Procedure: postErasablePolygon()
//      Note: Even if the polygon is posted on each iteration, the
//            helm will filter out unnecessary duplicate posts.

void BHV_Loiter::postErasablePolygon()
{
  XYSegList seglist = m_waypoint_engine.getSegList();
  string bhv_tag = tolower(getDescriptor());
  bhv_tag = findReplace(bhv_tag, "(d)", "");
  bhv_tag = m_us_name + "_" + bhv_tag;
  seglist.set_label(bhv_tag);
  seglist.set_active(false);

  string null_poly_spec = seglist.get_spec();
  postMessage("VIEW_POLYGON", null_poly_spec);
}

//-----------------------------------------------------------
// Procedure: postViewablePoint()

void BHV_Loiter::postViewablePoint()
{
  string bhv_tag = tolower(getDescriptor());

  XYPoint view_point(m_ptx, m_pty);
  //view_point.set_label(m_us_name + "'s next_waypoint");
  view_point.set_label(m_us_name + "_waypoint");
  view_point.set_color("label", m_hint_nextpt_lcolor);
  view_point.set_color("vertex", m_hint_nextpt_color);
  view_point.set_vertex_size(m_hint_nextpt_vertex_size);
  postMessage("VIEW_POINT", view_point.get_spec());
}


//-----------------------------------------------------------
// Procedure: postErasablePoint()

void BHV_Loiter::postErasablePoint()
{
  string bhv_tag = tolower(getDescriptor());

  XYPoint view_point(m_ptx, m_pty);
  //view_point.set_label(m_us_name + "'s next waypoint");
  view_point.set_label(m_us_name + "_waypoint");
  view_point.set_active(false);
  postMessage("VIEW_POINT", view_point.get_spec());
}


//-----------------------------------------------------------
// Procedure: handleVisualHint()

void BHV_Loiter::handleVisualHint(string hint)
{
  string param = tolower(stripBlankEnds(biteString(hint, '=')));
  string value = stripBlankEnds(hint);
  
  if((param == "vertex_size") && isNumber(value))
    m_hint_vertex_size = atof(value.c_str());
  else if((param == "edge_size") && isNumber(value))
    m_hint_edge_size = atof(value.c_str());
  else if((param == "vertex_color") && isColor(value))
    m_hint_vertex_color = value;
  else if((param == "edge_color") && isColor(value))
    m_hint_edge_color = value;
  else if((param == "nextpt_color") && isColor(value))
    m_hint_nextpt_color = value;
  else if((param == "nextpt_lcolor") && isColor(value))
    m_hint_nextpt_lcolor = value;
  else if((param == "nextpt_vertex_size") && isNumber(value))
    m_hint_nextpt_vertex_size = atof(value.c_str());
  else if(param == "label")
    m_hint_poly_label = value;
}

//-----------------------------------------------------------
// Procedure: postConfigStatus

void BHV_Loiter::postConfigStatus()
{
  string str = "type=BHV_Loiter,name=" + m_descriptor;
  
  str += ",x=" + doubleToString(m_loiter_engine.getCenterX(),2);
  str += ",y=" + doubleToString(m_loiter_engine.getCenterY(),2);
  str += ",clockwise=" + boolToString(m_clockwise);
  str += ",dynamic_clockwise=" + boolToString(m_dynamic_clockwise);
  str += ",center_activate=" + boolToString(m_center_activate);
  str += ",desired_speed=" + doubleToString(m_desired_speed,1);

  string poly_str = m_loiter_engine.getPolygon().get_spec();
  str += ",polygon=" + poly_str;

  postRepeatableMessage("BHV_SETTINGS", str);
}



