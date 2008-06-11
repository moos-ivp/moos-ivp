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

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Waypoint::BHV_Waypoint(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_waypoint");
  m_domain = subDomain(m_domain, "course,speed");

  m_cruise_speed    = 0;  // Meters/second
  m_lead_distance   = -1;
  m_ipf_type        = "zaic";

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
  if(IvPBehavior::setParam(param, val))
    return(true);

  if((param == "polygon") || (param == "points")) {
    XYSegList new_seglist = stringToSegList(val);
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
  else if(param == "order") {
    if((val!="reverse") && (val!="reversed") && (val!="normal"))
      return(false);
    bool reverse = ((val == "reverse") || (val == "reversed"));
    m_waypoint_engine.setReverse(reverse);
    return(true);
  }
  else if(param == "repeat") {
    int ival = atoi(val.c_str());
    if((ival < 0) || (!isNumber(val)))
      return(false);
    m_waypoint_engine.setRepeat(ival);
    return(true);
  }
  else if(param == "radius") {
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
  updateInfoOut(false);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_Waypoint::onRunState() 
{
  //waypoint_engine.setPerpetual(perpetual);

  // Set m_osx, m_osy, m_osv
  if(!updateInfoIn()) {
    updateInfoOut(false);
    return(0);
  }
  // Set m_ptx, m_pty, m_trackpt_x, m_trackpt_y;
  if(!setNextWaypoint()) {
    updateInfoOut(false);
    return(0);
  }
  
  IvPFunction *ipf = buildOF(m_ipf_type);
  if(ipf)
    ipf->setPWT(m_priority_wt);

  updateInfoOut(true);

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

  if(m_waypoint_engine.isComplete()) {
    postMessage("WPT_STAT", "complete");
    setComplete();
    if(m_perpetual)
      m_waypoint_engine.reset();
    return(false);
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

      double angle = relAng(x1, y1, m_ptx, m_pty);
      double dist  = distPointToPoint(nx, ny, m_ptx, m_pty);
      if(dist > m_lead_distance) 
	dist = m_lead_distance; 
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
      ipf = reflector.extractOF();
    }
  }    
  else { // if (method == "zaic")
    ZAIC_PEAK spd_zaic(m_domain, "speed");
    spd_zaic.setParams(m_cruise_speed, 0, 2.6, 0, 0, 100);
    IvPFunction *spd_of = spd_zaic.extractOF();
    
    double rel_ang_to_wpt = relAng(m_osx, m_osy, m_trackpt_x, m_trackpt_y);
    ZAIC_PEAK crs_zaic(m_domain, "course");
    crs_zaic.setValueWrap(true);
    crs_zaic.setParams(rel_ang_to_wpt, 0, 180, 50, 0, 100);
    IvPFunction *crs_of = crs_zaic.extractOF();

    OF_Coupler coupler;
    ipf = coupler.couple(crs_of, spd_of);
  }    
  return(ipf);
}

//-----------------------------------------------------------
// Procedure: updateInfoOut()

void BHV_Waypoint::updateInfoOut(bool post)
{
  if(m_osv > 0) {
    int    current_waypt = m_waypoint_engine.getCurrIndex();
    double dist_meters   = hypot((m_osx - m_ptx), (m_osy - m_pty));
    double eta_seconds   = dist_meters / m_osv;

    string stat = "vname=" + m_us_name + ",";
    stat += "index=" + intToString(current_waypt)   + ",";
    stat += "dist="  + doubleToString(dist_meters)  + ",";
    stat += "eta="   + doubleToString(eta_seconds);
    
    postMessage("WPT_STAT_LOCAL", stat);
    postMessage("WPT_INDEX", current_waypt);
  }

  XYSegList seglist = m_waypoint_engine.getSegList();
  seglist.set_label(m_us_name + "_" + m_descriptor);
  string segmsg = seglist.get_spec();
  postMessage("VIEW_SEGLIST", segmsg);

  if(post) {
    if(m_waypoint_engine.currPtChanged()) {
      string ptmsg;
      ptmsg += doubleToString(m_ptx,2) + ",";
      ptmsg += doubleToString(m_pty,2) + ",5," + m_us_name + "_wpt";
      postMessage("VIEW_POINT", ptmsg);
    }
  }
  else {
    string ptmsg = "0,0,0," + m_descriptor;
    postMessage("VIEW_POINT", ptmsg);
  }

}


