/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Waypoint.cpp                                     */
/*    DATE: Nov 2004                                             */
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
#include "BHV_Waypoint.h"
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
#include "XYFormatUtilsPoly.h"

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
  m_cruise_speed    = 0;  // meters/second
  m_lead_distance   = -1; // meters - default of -1 means unused
  m_lead_damper     = -1; // meters - default of -1 means unused
  m_ipf_type        = "zaic";

  m_var_report      = "WPT_STAT";
  m_var_index       = "WPT_INDEX";
  m_var_cyindex     = "CYCLE_INDEX";
  m_var_suffix      = "";

  // The completed and perpetual vars are initialized in superclass
  // but we initialize here just to be safe and clear.
  m_completed     = false; 
  m_perpetual     = false;

  m_osx   = -1;
  m_osy   = -1;
  m_osv   = -1;
  m_ptx   = -1;
  m_pty   = -1;

  m_trackpt_x = -1;
  m_trackpt_y = -1;

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED");
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition".
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_Waypoint::setParam(string param, string val) 
{
  if((param == "polygon") || (param == "points")) {
    XYSegList new_seglist = stringToSegList(val);
    if(new_seglist.size() == 0) {
      XYPolygon new_poly = stringToPoly(val);
      //XYPolygon new_poly = stringToPoly(val);
      new_seglist = new_poly.exportSegList(0,0);
    }
    if(new_seglist.size() == 0)
      return(false);
    m_waypoint_engine.setSegList(new_seglist);
    return(true);
  }
  else if(param == "speed") {
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    m_cruise_speed = dval;
    return(true);
  }
  else if((param == "wpt_status") || (param == "wpt_status_var")) {
    if(strContainsWhite(val) || (val == ""))
      return(false);
    m_var_report = val;
    if(tolower(m_var_report)=="silent")
      m_var_report = "silent";
    return(true);
  }
  else if((param == "wpt_index") || (param == "wpt_index_var")) {
    if(strContainsWhite(val) || (val == ""))
      return(false);
    m_var_index = val;
    if(tolower(m_var_index)=="silent")
      m_var_index = "silent";
    return(true);
  }
  else if(param == "cycle_index_var") {
    if(strContainsWhite(val) || (val == ""))
      return(false);
    m_var_cyindex = val;
    if(tolower(m_var_cyindex)=="silent")
      m_var_index = "silent";
    return(true);
  }
  else if(param == "post_suffix") {
    if(strContainsWhite(val))
      return(false);
    if((val.length() > 0) && (val.at(0) != '_'))
      val = '_' + val;
    m_var_suffix = val;
    return(true);
  }
  else if(param == "cycleflag") {
    string variable = stripBlankEnds(biteString(val, '='));
    string value    = stripBlankEnds(val);
    if((variable=="") || (value==""))
      return(false);
    VarDataPair pair(variable, value, "auto");
    m_cycle_flags.push_back(pair);
    return(true);
  }
  else if(param == "ipf-type") {
    val = tolower(val);
    if((val=="zaic") || (val=="roc") || (val=="rate_of_closure"))
      m_ipf_type = val;
    return(true);
  }
  else if(param == "lead") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    m_lead_distance = dval;
    return(true);
  }
  else if(param == "lead_damper") {
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    m_lead_damper = dval;
    return(true);
  }
  else if(param == "order") {
    if((val!="reverse") && (val!="reversed") && (val!="normal"))
      return(false);
    bool reverse = ((val == "reverse") || (val == "reversed"));
    m_waypoint_engine.setReverse(reverse);
    return(true);
  }
  else if(param == "repeat") {
    IvPBehavior::setParam("perpetual", "true");
    int ival = atoi(val.c_str());
    if((ival < 0) || (!isNumber(val)))
      return(false);
    m_waypoint_engine.setRepeat(ival);
    return(true);
  }
  else if((param == "radius") || (param == "capture_radius")) {
    double dval = atof(val.c_str());
    if(dval <= 0)
      return(false);
    m_waypoint_engine.setCaptureRadius(dval);
    return(true);
  }
  else if(param == "nm_radius")  {
    double dval = atof(val.c_str());
    if(dval <= 0) 
      return(false);
    m_waypoint_engine.setNonmonotonicRadius(dval);
    return(true);
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_Waypoint::onIdleState() 
{
  postErasablePoint();
  postErasableSegList();
  m_waypoint_engine.resetCPA();
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_Waypoint::onRunState() 
{
  m_waypoint_engine.setPerpetual(m_perpetual);

  // Set m_osx, m_osy, m_osv
  if(!updateInfoIn()) {
    postErasablePoint();
    return(0);
  }

  // Set m_ptx, m_pty, m_trackpt_x, m_trackpt_y;
  
  bool next_point = setNextWaypoint();

  // We want to report the updated cycle info regardless of the 
  // above result. Even if the next_point is false and there are
  // no more points, this means the cyindex is probably incremented.
  if(m_var_cyindex != "silent") {
    int waypt_cycles  = m_waypoint_engine.getCycleCount();
    postMessage((m_var_cyindex + m_var_suffix), waypt_cycles);
  }

  // Only publish these reports if we have another point to go.
  if(next_point) {
    postStatusReport();
    postViewablePoint();
    postViewableSegList();
  }
  // Otherwise "erase" the next waypoint marker
  else {
    postErasablePoint();
    return(0);
  }
  
  IvPFunction *ipf = buildOF(m_ipf_type);
  if(ipf)
    ipf->setPWT(m_priority_wt);


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

bool BHV_Waypoint::updateInfoIn()
{
  bool ok1, ok2, ok3;
  m_osx = getBufferDoubleVal("NAV_X",     ok1);
  m_osy = getBufferDoubleVal("NAV_Y",     ok2);
  m_osv = getBufferDoubleVal("NAV_SPEED", ok3);

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2) {
    postEMessage("No ownship X/Y info in info_buffer.");
    return(false);
  }

  // If NAV_SPEED info is not found in the info_buffer, its
  // not a show-stopper. A warning will be posted.

  return(true);
}


//-----------------------------------------------------------
// Procedure: setNextWaypoint

bool BHV_Waypoint::setNextWaypoint()
{
  string feedback_msg = m_waypoint_engine.setNextWaypoint(m_osx, m_osy);
  
  if((feedback_msg=="completed") || (feedback_msg=="cycled")) {
    if(tolower(m_var_report) != "silent") {
      string feedback_msg_aug = "behavior-name=" + m_descriptor + ",";
      feedback_msg_aug += feedback_msg;
      postMessage((m_var_report + m_var_suffix), feedback_msg_aug);
    }
    
    postCycleFlags();

    if(feedback_msg == "completed") {
      setComplete();
      if(m_perpetual)
	m_waypoint_engine.resetForNewTraversal();
      return(false);
    }
  }
  
  m_ptx = m_waypoint_engine.getPointX();
  m_pty = m_waypoint_engine.getPointY();
  
  // By default, the steering point is the next waypoint.
  m_trackpt_x = m_ptx;
  m_trackpt_y = m_pty;

  // If m_lead_distance is non-neg, and we've already hit the
  // first waypoint, then steer to an intermediate point that
  // is m_lead_distance away from the perpendicular intersection
  // point between the current position and the trackline.
  
  int current_waypt = m_waypoint_engine.getCurrIndex();
  if(m_lead_distance >= 0) {
    if(current_waypt > 0) {
      double x1 = m_waypoint_engine.getPointX(current_waypt-1);
      double y1 = m_waypoint_engine.getPointY(current_waypt-1);

      double nx, ny;
      perpSegIntPt(x1,y1,m_ptx,m_pty,m_osx,m_osy,nx,ny);

      double damper_factor = 1.0;
      if(m_lead_damper > 0) {
	double dist_to_trackline = hypot((nx-m_osx),(ny-m_osy));
	if(dist_to_trackline < m_lead_damper) {
	  double augment = 1 - (dist_to_trackline / m_lead_damper);
	  damper_factor += 2*augment;
	}
      }
	  
      double angle = relAng(x1, y1, m_ptx, m_pty);
      double dist  = distPointToPoint(nx, ny, m_ptx, m_pty);
      if(dist > (m_lead_distance * damper_factor)) 
	dist = m_lead_distance * damper_factor;  
      projectPoint(angle, dist, nx, ny, m_trackpt_x, m_trackpt_y);
    }
  }
  return(true);
}


//-----------------------------------------------------------
// Procedure: buildOF

IvPFunction *BHV_Waypoint::buildOF(string method) 
{
  IvPFunction *ipf = 0;

  if((method == "roc") || (method == "rate_of_closure")) {
    bool ok = true;
    AOF_Waypoint aof_wpt(m_domain);
    ok = ok && aof_wpt.setParam("desired_speed", m_cruise_speed);
    ok = ok && aof_wpt.setParam("osx", m_osx);
    ok = ok && aof_wpt.setParam("osy", m_osy);
    ok = ok && aof_wpt.setParam("ptx", m_trackpt_x);
    ok = ok && aof_wpt.setParam("pty", m_trackpt_y);
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
    
    double rel_ang_to_wpt = relAng(m_osx, m_osy, m_trackpt_x, m_trackpt_y);
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
// Procedure: postStatusReport()

void BHV_Waypoint::postStatusReport()
{
  int    current_waypt = m_waypoint_engine.getCurrIndex();
  int    waypt_cycles  = m_waypoint_engine.getCycleCount();
  double dist_meters   = hypot((m_osx - m_ptx), (m_osy - m_pty));
  double eta_seconds   = dist_meters / m_osv;
  
  string stat = "vname=" + m_us_name + ",";
  stat += "behavior-name=" + m_descriptor + ",";
  stat += "index=" + intToString(current_waypt)   + ",";
  stat += "cycles=" + intToString(waypt_cycles)   + ",";
  stat += "dist="  + doubleToString(dist_meters, 0)  + ",";
  stat += "eta="   + doubleToString(eta_seconds, 0);

  if(m_var_report != "silent")
    postMessage((m_var_report + m_var_suffix), stat);
  if(m_var_index != "silent")
    postMessage((m_var_index + m_var_suffix), current_waypt);
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
  string segmsg = seglist.get_spec();
  postMessage("VIEW_SEGLIST", segmsg);
}


//-----------------------------------------------------------
// Procedure: postErasableSegList()
//      Note: Recall that for a seglist to be drawn and erased, 
//            it must match in the label. For a seglist to be 
//            "ignored" it must set active=false.

void BHV_Waypoint::postErasableSegList()
{
  XYSegList seglist = m_waypoint_engine.getSegList();
  seglist.set_label(m_us_name + "_" + m_descriptor);
  seglist.set_active(false);
  string segmsg = seglist.get_spec();
  postMessage("VIEW_SEGLIST", segmsg);
}


//-----------------------------------------------------------
// Procedure: postViewablePoint()

void BHV_Waypoint::postViewablePoint()
{
  // Recall that for a point to be drawn and erased, it must match
  // in the (1) label (2) type (3) and source.

  string source_tag = (m_us_name + tolower(getDescriptor()));
  string ptmsg;
  ptmsg = "label=" + m_us_name + "'s next waypoint";
  ptmsg += ",type=waypoint, source=" + source_tag;
  ptmsg += ",x=" + dstringCompact(doubleToString(m_ptx,1));
  ptmsg += ",y=" + dstringCompact(doubleToString(m_pty,1));
  ptmsg += ",size=1";
  postMessage("VIEW_POINT", ptmsg);
}

//-----------------------------------------------------------
// Procedure: postErasablePoint()

void BHV_Waypoint::postErasablePoint()
{
  // Recall that for a point to be drawn and erased, it must match
  // in the (1) label (2) type (3) and source.
  // For a point to be "ignored" it must set active=false.

  string source_tag = (m_us_name + tolower(getDescriptor()));
  string ptmsg;
  ptmsg = "label=" + m_us_name + "'s next waypoint";
  ptmsg += ",type=waypoint, source=" + source_tag;
  ptmsg += ",x=0, y=0, z=0, active=false, size=0";
  postMessage("VIEW_POINT", ptmsg);
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
