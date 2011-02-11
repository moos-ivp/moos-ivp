/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Loiter.cpp                                       */
/*    DATE: July 26th 2005 In Elba w/ M.Grund, P.Newman          */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the termxs of the GNU General Public License   */
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

#include <math.h>
#include <stdlib.h>
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
  m_iterations      = 0;

  // Initialize State Variables with initial vals of zero which
  // have no real meaning, but better than nondeterministic.
  m_osx             = 0;
  m_osy             = 0;
  m_osh             = 0;
  m_ptx             = 0;
  m_pty             = 0;
  m_dist_to_poly    = 0;

  // Initialize postMessage state variables.
  m_var_report    = "LOITER_REPORT";
  m_var_index     = "LOITER_INDEX";
  m_var_acquire   = "LOITER_ACQUIRE";
  m_var_dist2poly = "LOITER_DIST_TO_POLY";
  m_var_eta2poly  = "LOITER_ETA_TO_POLY";
  m_var_suffix    = "";

  // Initialize Configuration Parameters
  m_desired_speed   = 0;      // m/s
  m_clockwise       = true;
  m_acquire_dist    = 10;
  m_center_pending  = false;
  m_center_activate = false;

  m_waypoint_engine.setPerpetual(true);
  m_waypoint_engine.setRepeatsEndless(true);

  addInfoVars("NAV_X, NAV_Y, NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_Loiter::setParam(string param, string value) 
{
  if(param == "polygon") {
    XYPolygon new_poly = string2Poly(value);
    if(!new_poly.is_convex())  // Should be convex - false otherwise
      return(false);
    if(new_poly.is_clockwise() != m_clockwise)
      new_poly.reverse();
    new_poly.apply_snap(0.1); // snap to tenth of meter
    m_loiter_engine.setPoly(new_poly);
    m_waypoint_engine.setSegList(new_poly);
    m_acquire_mode  = true;
    return(true);
  }  
  else if(param == "center_assign") {
    m_center_assign  = value;
    m_center_pending = true;
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
    bool ok = setBooleanOnString(m_clockwise, value);
    if(!ok)
      return(false);
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
  else if(param == "var_report")  {
    if(strContainsWhite(value) || (value == ""))
      return(false);
    m_var_report = value;
    return(true);
  }
  else if(param == "var_acquire")  {
    if(strContainsWhite(value) || (value == ""))
      return(false);
    m_var_acquire = value;
    return(true);
  }
  else if(param == "var_dist2poly")  {
    if(strContainsWhite(value) || (value == ""))
      return(false);
    m_var_dist2poly = value;
    return(true);
  }
  else if(param == "var_index")  {
    if(strContainsWhite(value) || (value == ""))
      return(false);
    m_var_index = value;
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
  if(!m_center_activate)
    return;
  m_center_pending = true;
  m_center_assign  = "present_position";
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_Loiter::onRunState() 
{
  m_iterations++;

  // Set m_osx, m_osy, m_osh
  if(!updateInfoIn()) {
    postErasablePoint();
    return(0);
  }

  updateCenter();

  if(m_dist_to_poly > m_acquire_dist)
    m_acquire_mode = true;
  else
    m_acquire_mode = false;

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
  m_dist_to_poly = poly.dist_to_poly(m_osx, m_osy);

  bool ok1, ok2, ok3;
  // ownship position in meters from some 0,0 reference point.
  m_osx = getBufferDoubleVal("NAV_X", ok1);
  m_osy = getBufferDoubleVal("NAV_Y", ok2);
  m_osh = getBufferDoubleVal("NAV_HEADING", ok3);

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2) {
    postEMessage("No ownship X/Y info in info_buffer.");
    return(false);
  }
  
  if(!ok3) {
    postEMessage("No ownship HEADING info in info_buffer.");
    return(false);
  }
  
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

  // Default for m_var_dist2poly = LOITER_REPORT
  if((m_var_report != "SILENT") && (m_var_report != "OFF"))
    postMessage((m_var_report + m_var_suffix), loiter_report);

  // Default for m_var_dist2poly = LOITER_INDEX
  if((m_var_index != "SILENT") && (m_var_index != "OFF"))
    postMessage((m_var_index + m_var_suffix), curr_index);

  // Default for m_var_dist2poly = LOITER_ACQUIRE
  if((m_var_acquire != "SILENT") && (m_var_acquire != "OFF")) {
    if(m_acquire_mode)
      postMessage((m_var_acquire + m_var_suffix), 1);
    else
      postMessage((m_var_acquire + m_var_suffix), 0);
  }
  
  // Default for m_var_dist2poly = LOITER_DIST_TO_POLY
  if((m_var_dist2poly != "SILENT") && (m_var_dist2poly != "OFF")) 
    postIntMessage((m_var_dist2poly + m_var_suffix), m_dist_to_poly);
}

//-----------------------------------------------------------
// Procedure: postViewablePolygon()
//      Note: Even if the polygon is posted on each iteration, the
//            helm will filter out unnecessary duplicate posts.

void BHV_Loiter::postViewablePolygon()
{
  XYSegList seglist = m_waypoint_engine.getSegList();
  seglist.set_vertex_color(m_hint_vertex_color);
  seglist.set_edge_color(m_hint_edge_color);
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
  view_point.set_type("waypoint");
  view_point.set_source(m_us_name + "_" + bhv_tag);
  view_point.set_label_color(m_hint_nextpt_lcolor);
  view_point.set_vertex_color(m_hint_nextpt_color);
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
  view_point.set_type("waypoint");
  view_point.set_source(m_us_name + "_" + bhv_tag);
  view_point.set_active(false);
  postMessage("VIEW_POINT", view_point.get_spec());
}


//-----------------------------------------------------------
// Procedure: handleVisualHint()

void BHV_Loiter::handleVisualHint(string hint)
{
  string param = tolower(stripBlankEnds(biteString(hint, '=')));
  string value = stripBlankEnds(hint);
  
  if((param == "nextpt_color") && isColor(value))
    m_hint_nextpt_color = value;
  else if((param == "nextpt_lcolor") && isColor(value))
    m_hint_nextpt_lcolor = value;
  else if((param == "vertex_color") && isColor(value))
    m_hint_vertex_color = value;
  else if((param == "edge_color") && isColor(value))
    m_hint_edge_color = value;
  else if((param == "edge_size") && isNumber(value))
    m_hint_edge_size = atof(value.c_str());
  else if((param == "vertex_size") && isNumber(value))
    m_hint_vertex_size = atof(value.c_str());
  else if(param == "label")
    m_hint_poly_label = value;
}
