/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Loiter.cpp                                       */
/*    DATE: July 26th 2005 In Elba w/ M.Grund, P.Newman          */
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

#include <math.h>
#include <stdlib.h>
#include "AngleUtils.h"
#include "BHV_Loiter.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "XYBuildUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Loiter::BHV_Loiter(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_loiter");
  m_domain = subDomain(m_domain, "course,speed");

  // Initialize State Variable with meaningful initial vals
  m_acquire_mode    = true;
  m_iterations      = 0;

  // Initialize State Variable with initial vals of zero which
  // have no real meaning, but better than nondeterministic.
  m_osx             = 0;
  m_osy             = 0;
  m_osh             = 0;
  m_ptx             = 0;
  m_pty             = 0;
  m_dist_to_poly    = 0;

  // Initialize Configuration Parameters
  m_desired_speed   = 0;      // m/s
  m_clockwise       = true;
  m_acquire_dist    = 10;
  m_center_pending  = false;
  m_center_activate = false;

  m_waypoint_engine.setPerpetual(true);

  addInfoVars("NAV_X, NAV_Y, NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_Loiter::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParam(g_param, g_val))
    return(true);

  if(g_param == "polygon") {
    XYPolygon new_poly = stringToPoly(g_val);
    if(!new_poly.is_convex())  // Should be convex - false otherwise
      return(false);
    if(new_poly.is_clockwise() != m_clockwise)
      new_poly.reverse();
    new_poly.apply_snap(0.1); // snap to tenth of meter
    m_loiter_engine.setPoly(new_poly);
    m_waypoint_engine.setSegList(new_poly);
    m_poly_changed  = true;
    m_acquire_mode  = true;
    return(true);
  }  
  else if(g_param == "center_assign") {
    m_center_assign  = g_val;
    m_center_pending = true;
    return(true);
  }  
  else if(g_param == "center_activate") {
    g_val = tolower(g_val);
    if((g_val!="true")&&(g_val!="false"))
      return(false);
    m_center_activate = (g_val == "true");
    return(true);
  }  
  else if(g_param == "clockwise") {
    g_val = tolower(g_val);
    if((g_val!="true")&&(g_val!="false"))
      return(false);
    m_clockwise = (g_val == "true");
    m_loiter_engine.setClockwise(m_clockwise);
    m_waypoint_engine.setSegList(m_loiter_engine.getPolygon());
    return(true);
  }  
  else if(g_param == "speed") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_desired_speed = dval;
    return(true);
  }
  else if(g_param == "radius") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_waypoint_engine.setCaptureRadius(dval);
    return(true);
  }
  else if(g_param == "acquire_dist") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_acquire_dist = dval;
    return(true);
  }
  else if(g_param == "nm_radius")  {
    double dval = atof(g_val.c_str());
    // val=0 is ok, interpreted as inactive
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_waypoint_engine.setNonmonotonicRadius(dval);
    return(true);
  }
  return(false);
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
    updateInfoOutNull();
    return(0);
  }

  updateCenter();

  if(m_dist_to_poly > m_acquire_dist)
    m_acquire_mode = true;
  
  if(m_acquire_mode) {
    int curr_waypt = m_loiter_engine.acquireVertex(m_osh, m_osx, m_osy); 
    m_waypoint_engine.setCurrIndex(curr_waypt);
  }
  
  string feedback_msg = m_waypoint_engine.setNextWaypoint(m_osx, m_osy);
  if(feedback_msg == "advanced")
    m_acquire_mode = false;
  
  m_ptx = m_waypoint_engine.getPointX();
  m_pty = m_waypoint_engine.getPointY();

  IvPFunction *ipf = buildIPF("zaic");

  if(ipf) {
    ipf->getPDMap()->normalize(0,100);
    ipf->setPWT(m_priority_wt);
  }

  updateInfoOut();
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

void BHV_Loiter::updateCenter()
{
  if(!m_center_pending)
    return;
  
  m_center_assign = tolower(m_center_assign);

  if(m_center_assign == "present_position") {
    m_loiter_engine.setCenter(m_osx, m_osy);
    m_waypoint_engine.setCenter(m_osx, m_osy);
    m_poly_changed = true;
  }
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
	m_poly_changed = true;
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
    IvPFunction *spd_of = spd_zaic.extractOF();
    double rel_ang_to_wpt = relAng(m_osx, m_osy, m_ptx, m_pty);
    ZAIC_PEAK crs_zaic(m_domain, "course");
    crs_zaic.setSummit(rel_ang_to_wpt);
    crs_zaic.setBaseWidth(180.0);
    crs_zaic.setValueWrap(true);
    IvPFunction *crs_of = crs_zaic.extractOF();
    OF_Coupler coupler;
    ipf = coupler.couple(crs_of, spd_of);
  }

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: updateInfoOut()

#if 0
void BHV_Loiter::updateInfoOut()
{
  int nonmono_hits = m_waypoint_engine.getNonmonoHits();
  int capture_hits = m_waypoint_engine.getCaptureHits();
  int curr_index   = m_waypoint_engine.getCurrIndex();

  string loiter_report = "Pt:" + intToString(curr_index);
  loiter_report += " Dist:"    + doubleToString(m_dist_to_poly,0);
  loiter_report += " CP Hits:" + intToString(capture_hits);
  loiter_report += " NM_Hits:" + intToString(nonmono_hits);
  loiter_report += " AQ_MODE:" + boolToString(m_acquire_mode);
  postMessage("LOITER_REPORT", loiter_report);

  postMessage("LOITER_INDEX", curr_index);
  if(m_acquire_mode)
    postMessage("LOITER_ACQUIRE", 1);
  else
    postMessage("LOITER_ACQUIRE", 0);

  XYSegList seglist = m_waypoint_engine.getSegList();
  string bhv_tag = toupper(getDescriptor());
  bhv_tag = findReplace(bhv_tag, "BHV_", "");
  bhv_tag = findReplace(bhv_tag, "(d)", "");
  bhv_tag = m_us_name + "-" + bhv_tag;
  string spec = "label," + bhv_tag + ":" + seglist.get_spec();
  postMessage("VIEW_POLYGON", spec);
  
  string ptmsg;
  ptmsg =  "x=" + dstringCompact(doubleToString(m_ptx,2));
  ptmsg += ",y=" + dstringCompact(doubleToString(m_pty,2));
  ptmsg += ",label=loiter_" + m_us_name;
  ptmsg += ",type=waypoint";
  postMessage("VIEW_POINT", ptmsg);
  
  postIntMessage("DIST_TO_REGION", m_dist_to_poly);
}
#endif

#if 1
//-----------------------------------------------------------
// Procedure: updateInfoOut()

void BHV_Loiter::updateInfoOut()
{
  int nonmono_hits = m_waypoint_engine.getNonmonoHits();
  int capture_hits = m_waypoint_engine.getCaptureHits();
  int curr_index   = m_waypoint_engine.getCurrIndex();

  string loiter_report = "Pt:" + intToString(curr_index);
  loiter_report += " Dist:"    + doubleToString(m_dist_to_poly,0);
  loiter_report += " CP Hits:" + intToString(capture_hits);
  loiter_report += " NM_Hits:" + intToString(nonmono_hits);
  loiter_report += " AQ_MODE:" + boolToString(m_acquire_mode);
  postMessage("LOITER_REPORT", loiter_report);

  postMessage("LOITER_INDEX", curr_index);
  if(m_acquire_mode)
    postMessage("LOITER_ACQUIRE", 1);
  else
    postMessage("LOITER_ACQUIRE", 0);

  XYSegList seglist = m_waypoint_engine.getSegList();
  if(m_poly_changed) {
    string bhv_tag = toupper(getDescriptor());
    bhv_tag = findReplace(bhv_tag, "BHV_", "");
    bhv_tag = findReplace(bhv_tag, "(d)", "");
    bhv_tag = m_us_name + "-" + bhv_tag;
    string spec = "label," + bhv_tag + ":" + seglist.get_spec();
    postMessage("VIEW_POLYGON", spec);
  }
  m_poly_changed = false;
  
  if(m_waypoint_engine.currPtChanged()) {
    string ptmsg;
    ptmsg =  "x=" + dstringCompact(doubleToString(m_ptx,2));
    ptmsg += ",y=" + dstringCompact(doubleToString(m_pty,2));
    ptmsg += ",label=loiter_" + m_us_name;
    ptmsg += ",type=waypoint";
    postMessage("VIEW_POINT", ptmsg);
  }
  
  postIntMessage("DIST_TO_REGION", m_dist_to_poly);
}
#endif

//-----------------------------------------------------------
// Procedure: updateInfoOutNull()

void BHV_Loiter::updateInfoOutNull()
{
  string bhv_tag = toupper(getDescriptor());

  string null_poly_spec = "label," + bhv_tag + ":0,0:0,0:0,0";
  postMessage("VIEW_POLYGON", null_poly_spec);
  
  string null_point_spec;
  null_point_spec =  "x=0,y=0,active=false,type=waypoint,";
  null_point_spec += "label=loiter_" + m_us_name;
  postMessage("VIEW_POINT", null_point_spec);
}


