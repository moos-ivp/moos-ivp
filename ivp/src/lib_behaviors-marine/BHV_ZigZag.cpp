/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_ZigZag.cpp                                       */
/*    DATE: Sep 16th 2022                                        */
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
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "VarDataPairUtils.h"
#include "BHV_ZigZag.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "ZAIC_SPD.h"
#include "MacroUtils.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Constructor()

BHV_ZigZag::BHV_ZigZag(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  this->setParam("descriptor", "zig");
  
  m_domain = subDomain(m_domain, "course,speed");
  
  //====================================================
  // Init State Vars
  //====================================================
  m_state   = "stem";
  m_req_hdg = 0;
  m_set_hdg = 0;
  m_zig_cnt = 0;

  m_stem_x1 = 0;
  m_stem_y1 = 0;
  m_stem_x2 = 0;
  m_stem_y2 = 0;
  m_stem_dist = 0;
  m_stem_odo  = 0;

  m_zig_spd_start = 0;
  m_zig_spd_min   = -1;
  m_zig_spd_delta = 0;
  
  //====================================================
  // Init Config Vars
  //====================================================
  m_hdg_thresh = 0;
  m_zig_angle  = 0;
  m_zig_angle_fierce = -1;

  m_stem_hdg = 0;
  m_speed = 0;
  m_speed_orig = 0;
  m_speed_on_active = false;

  m_stale_nav_thresh = 5;

  m_stem_on_active = false;
  m_fierce_zigging = false;

  m_draw_set_hdg = true;
  m_draw_req_hdg = true;
  
  m_max_zig_legs  = 2;
  m_max_stem_odo  = -1;

  m_zig_first = "port";

  m_hint_set_hdg_color = "white";
  m_hint_req_hdg_color = "yellow";
  
  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam()

bool BHV_ZigZag::setParam(string param, string value) 
{
  if(IvPBehavior::setParam(param, value))
    return(true);
  
  bool handled = true;
  if(param == "speed")
    handled = setNonNegDoubleOnString(m_speed, value);
  else if(param == "mod_speed")
    handled = handleConfigModSpeed(value);    
  else if((param == "spd_on_active") || (param == "speed_on_active"))
    handled = setBooleanOnString(m_speed_on_active, value);
  else if(param == "stem_hdg") {
    handled = setDoubleOnString(m_stem_hdg, value);
    if(handled)
      m_stem_hdg = angle360(m_stem_hdg);
  }  
  else if(param == "stem_on_active")
    handled = setBooleanOnString(m_stem_on_active, value);
  else if(param == "hdg_thresh")
    handled = setNonNegDoubleOnString(m_hdg_thresh, value);
  else if(param == "fierce_zigging")
    handled = setBooleanOnString(m_fierce_zigging, value);
  else if(param == "zig_angle")
    handled = handleConfigZigAngle(value);
  else if(param == "zig_first")
    handled = handleConfigZigFirst(value);
  else if(param == "max_zig_legs")
    handled = setUIntOnString(m_max_zig_legs, value);
  else if(param == "max_stem_odo")
    handled = setPosDoubleOnString(m_max_stem_odo, value);
  else if(param == "max_zig_zags")
    handled = handleConfigZigZags(value);
  else if(param == "delta_heading" || (param == "zig_angle_fierce"))
    handled = handleConfigZigAngleFierce(value);

  else if(param == "zigflag")
    handled = addVarDataPairOnString(m_zig_flags, value);
  else if(param == "zagflag")
    handled = addVarDataPairOnString(m_zag_flags, value);
  else if(param == "portflag")
    handled = addVarDataPairOnString(m_port_flags, value);
  else if(param == "starflag")
    handled = addVarDataPairOnString(m_star_flags, value);
  else if(param == "portflagx")
    handled = addVarDataPairOnString(m_portx_flags, value);
  else if(param == "starflagx")
    handled = addVarDataPairOnString(m_starx_flags, value);

  else if(param == "visual_hints")  {
    vector<string> svector = parseStringQ(value, ',');
    for(unsigned int i=0; i<svector.size(); i++) 
      handled = handled && handleConfigVisualHint(svector[i]);
  }
  else
    handled = false;

  return(handled);
}

//-----------------------------------------------------------
// Procedure: handleConfigZigZags()

bool BHV_ZigZag::handleConfigZigZags(string str)
{
  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());
  if(dval < 1)
    return(false);

  unsigned int max_zig_zags = (unsigned int)(dval);

  m_max_zig_legs = 2 * max_zig_zags;

  return(true);
}

//-----------------------------------------------------------
// Procedure: handleConfigZigFirst()
//      Note: Possible setting: port or star
//      Note: Posssble given values:
//            port, star, left, right, starboar

bool BHV_ZigZag::handleConfigZigFirst(string str)
{
  str = tolower(str);

  if(str == "starboard")
    str = "star";
  else if(str == "right")
    str = "star";
  else if(str == "left")
    str = "port";

  if((str == "port") || (str == "star"))
    m_zig_first = str;
  else 
    return(false);
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: handleConfigZigAngle()

bool BHV_ZigZag::handleConfigZigAngle(string str)
{
  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());
  if((dval < 1) || (dval > 75))
    return(false);
  
  m_zig_angle = dval;
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: handleConfigZigAngleFierce()

bool BHV_ZigZag::handleConfigZigAngleFierce(string str)
{
  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());
  if((dval < 1) || (dval > 75))
    return(false);
  
  m_zig_angle_fierce = dval;
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: handleConfigSpeed()

bool BHV_ZigZag::handleConfigSpeed(string str)
{
  if(!isNumber(str))
    return(false);

  double spd_max = m_domain.getVarHigh("speed");
  double spd_req = atof(str.c_str());
  if(spd_req < 0)
    return(false);
  
  m_speed = vclip(spd_req, 0, spd_max);
  m_speed_orig = m_speed;
  return(true);
}

//-----------------------------------------------------------
// Procedure: handleConfigModSpeed()

bool BHV_ZigZag::handleConfigModSpeed(string str)
{
  if(tolower(str) == "reset") {
    m_speed = m_speed_orig;
    return(true);
  }
    
  if(!isNumber(str))
    return(false);

  double spd_max = m_domain.getVarHigh("speed");
  double spd_mod = atof(str.c_str());
  double new_spd = m_speed + spd_mod;

  m_speed = vclip(new_spd, 0, spd_max);
  return(true);
}

//-----------------------------------------------------------
// Procedure: updateSetHdg()

void BHV_ZigZag::updateSetHdg()
{
  // ========================================================
  // Part 1: If in stem state, pick a side and set a heading 
  // ========================================================
  if(m_state == "stem") {
    if(m_zig_first == "port") {
      setState("port");
      m_set_hdg = angle360(m_stem_hdg - m_zig_angle);
    }
    else {
      setState("star");
      m_set_hdg = angle360(m_stem_hdg + m_zig_angle);
    }
    return;
  }

  // ========================================================
  // Part 2: Check if turn is still in progress
  // ========================================================
  double diff = angleDiff(m_osh, m_set_hdg);
  bool turn_complete = false;
  if(m_state == "port") {
    if(diff < m_hdg_thresh)
      turn_complete = true;
    else if(!containsAngle(m_osh, m_osh-170, m_set_hdg))
      turn_complete = true;
  }
  else {
    if(diff < m_hdg_thresh)
      turn_complete = true;
    else if(!containsAngle(m_osh, m_osh+170, m_set_hdg))
      turn_complete = true;
  }

  if((m_zig_spd_min < 0) || (m_osv < m_zig_spd_min))
    m_zig_spd_min = m_osv;
  m_zig_spd_delta = m_zig_spd_start - m_zig_spd_min;
  
  if(!turn_complete)
    return;
  
  // ========================================================
  // Part 3: Achieved set_hdg so change state
  // ========================================================
  m_zig_cnt++;
  m_zig_cnt_ever++;

  postFlags(m_zig_flags);
  if((m_zig_cnt % 2) == 0)
    postFlags(m_zag_flags);

  // Switch port/star modes and post appropriate flags
  if(m_state == "port") {
    postFlags(m_portx_flags); // finishing port leg
    setState("star");
    m_set_hdg = angle360(m_stem_hdg + m_zig_angle);
  }
  else if(m_state == "star") {
    postFlags(m_starx_flags);  // finishing star let
    setState("port");
    m_set_hdg = angle360(m_stem_hdg - m_zig_angle);
  }

  // Reset the odometry only after all/any flags posted
  m_zig_odo.reset();
  if((m_zig_cnt % 2) == 0) 
    m_zag_odo.reset();

  m_zig_spd_start = m_osh;
  m_zig_spd_min = -1;
  m_zig_spd_delta = 0;
}

//-----------------------------------------------------------
// Procedure: updateReqHdg()

void BHV_ZigZag::updateReqHdg()
{
  if(!m_fierce_zigging)
    m_req_hdg = m_set_hdg;

  else {
    double delta_hdg = m_zig_angle;
    if(m_zig_angle_fierce > 0)
      delta_hdg = m_zig_angle_fierce;
    
    if(m_state == "port") 
      m_req_hdg = m_osh - delta_hdg;
    else if(m_state == "star") 
      m_req_hdg = m_osh + delta_hdg;
    else if(m_state == "stem") 
      m_req_hdg = m_stem_hdg;
  }

  m_req_hdg = angle360(m_req_hdg);
}

//-----------------------------------------------------------
// Procedure: buildOF()

IvPFunction *BHV_ZigZag::buildOF() 
{
  IvPFunction *ipf = 0;  
  // Sanity Check
  if(m_state == "off")
    return(0);
  
  //===================================================
  // Part 1: Build the Speed ZAIC
  //===================================================
  IvPFunction *spd_ipf = 0;  

  ZAIC_SPD spd_zaic(m_domain, "speed");
  spd_zaic.setParams(m_speed, 0.1, m_speed+0.4, 85, 20);  

  spd_ipf = spd_zaic.extractIvPFunction();
  if(!spd_ipf)
    postWMessage("Failure on the SPD ZAIC component");

  //===================================================
  // Part 2: Build the Heading ZAIC
  //===================================================
  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setSummit(m_req_hdg);
  crs_zaic.setBaseWidth(180);
  crs_zaic.setValueWrap(true);
  
  IvPFunction *crs_ipf = crs_zaic.extractIvPFunction(false);  
  if(!crs_ipf) 
    postWMessage("Failure on the CRS ZAIC");

  OF_Coupler coupler;
  ipf = coupler.couple(crs_ipf, spd_ipf, 50, 50);
  if(!ipf)
    postWMessage("Failure on the CRS_SPD COUPLER");

  ipf->getPDMap()->normalize(0,100);
  ipf->setPWT(m_priority_wt);

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: onRunState()

IvPFunction *BHV_ZigZag::onRunState() 
{
  if(!updateOSPos() || !updateOSHdg() || !updateOSSpd())
    return(0);

  updateOdometry();
  updateSetHdg();
  updateReqHdg();

  postSetHdgLine();
  postReqHdgLine();

  bool stem_odo_reached = false;
  if((m_max_stem_odo > 0) && (m_stem_odo >= m_max_stem_odo))
    stem_odo_reached = true;
  
  if(legsComplete() || stem_odo_reached) {
    setComplete();
    //    if(m_perpetual) {
    //  m_zig_cnt = 0;
    //  m_state = "stem";
    // }
    return(0);
  }
  
  // Part 2: Generate the IvP function 
  IvPFunction *ipf = buildOF();

  // Part 3: Apply the relevance and priority weight
  if(ipf) 
    ipf->setPWT(m_priority_wt);
  else 
    postMessage("ZIG_DEBUG", "Unable to build IvP Function");    

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: onRunToIdleState()

void BHV_ZigZag::onRunToIdleState()
{
  eraseSetHdgLine();
  eraseReqHdgLine();

  resetOdometry();
  m_zig_cnt = 0;
  setState("stem");
}

//-----------------------------------------------------------
// Procedure: onIdleToRunState()

void BHV_ZigZag::onIdleToRunState()
{
  if(m_stem_on_active && updateOSHdg())
    m_stem_hdg = m_osh;

  if(m_speed_on_active && updateOSSpd()) {
    m_speed = m_osv;
    m_speed_orig = m_osv;
  }
  updateOSPos();
  
  // Reset the stuff that gets reset upon entering the run state.
  m_zig_cnt = 0;
  resetOdometry();

  // Calculate the stem line so we can later report the ownship
  // distance to the stem line.
  m_stem_x1 = m_osx;
  m_stem_y1 = m_osy;
  projectPoint(m_stem_hdg, 5000, m_osx, m_osy, m_stem_x2, m_stem_y2);

  setState("stem");
}

//-----------------------------------------------------------
// Procedure: updateOSPos()
//   Returns: true if NAV_X/Y info is found and not stale
//            false otherwise

bool BHV_ZigZag::updateOSPos(string fail_action) 
{
  bool   ok_osx = true;
  bool   ok_osy = true;
  double new_osx = getBufferDoubleVal("NAV_X", ok_osx);
  double new_osy = getBufferDoubleVal("NAV_Y", ok_osy);
  double age_osx = getBufferTimeVal("NAV_X");
  double age_osy = getBufferTimeVal("NAV_Y");

  bool all_ok = true;
  if(!ok_osy || !ok_osy)
    all_ok = false;
  if((age_osx > m_stale_nav_thresh) ||
     (age_osy > m_stale_nav_thresh))
    all_ok = false;

  if(!all_ok) {
    if(fail_action == "err")
      postEMessage("ownship NAV_X/Y not found or stale.");
    else if(fail_action == "warn")
      postWMessage("ownship NAV_X/Y not found or stale.");
    return(false);
  }

  m_osx = new_osx;
  m_osy = new_osy;
  return(true);
}

//-----------------------------------------------------------
// Procedure: updateOSHdg()
//   Returns: true if NAV_HEADING info is found and not stale
//            false otherwise

bool BHV_ZigZag::updateOSHdg(string fail_action) 
{
  bool   ok_osh = true;
  double new_osh = getBufferDoubleVal("NAV_HEADING", ok_osh);
  double age_osh = getBufferTimeVal("NAV_HEADING");

  if(!ok_osh || (age_osh > m_stale_nav_thresh)) {
    if(fail_action == "err")
      postEMessage("ownship NAV_HEADING not found or stale.");
    else if(fail_action == "warn")
      postWMessage("ownship NAV_HEADING not found or stale.");
    return(false);
  }

  m_osh = new_osh;
  return(true);
}

//-----------------------------------------------------------
// Procedure: updateOSSpd()
//   Returns: true: if NAV_SPEED info is found and not stale
//            false: otherwise

bool BHV_ZigZag::updateOSSpd(string fail_action) 
{
  bool   ok_osv  = true;
  double new_osv = getBufferDoubleVal("NAV_SPEED", ok_osv);
  double age_osv = getBufferTimeVal("NAV_SPEED");
  
  if(!ok_osv || (age_osv > m_stale_nav_thresh)) {
    if(fail_action == "err")
      postEMessage("ownship NAV_SPEED not found or stale.");
    else if(fail_action == "warn")
      postWMessage("ownship NAV_SPEED not found or stale.");
    return(false);
  }

  m_osv = new_osv;
  return(true);
}

//-----------------------------------------------------------
// Procedure: updateOdometry()

void BHV_ZigZag::updateOdometry()
{
  m_odometer.setXY(m_osx, m_osy);
  m_zig_odo.setXY(m_osx, m_osy);
  m_zag_odo.setXY(m_osx, m_osy);

  m_odometer.updateDistance();
  m_zig_odo.updateDistance();
  m_zag_odo.updateDistance();

  // Sanity check stem line has been set
  if((m_stem_x1==0) && (m_stem_y1==0) && (m_stem_x2==0) && (m_stem_y2==0))
    return;
  
  m_stem_dist = distPointToLine(m_osx, m_osy, m_stem_x1, m_stem_y1,
				m_stem_x2, m_stem_y2);


  double ix, iy;
  perpSegIntPt(m_stem_x1, m_stem_y1, m_stem_x2, m_stem_y2,
	       m_osx, m_osy, ix, iy);

  m_stem_odo = hypot(m_stem_x1-ix, m_stem_y1-iy);
}

//-----------------------------------------------------------
// Procedure: resetOdometry()

void BHV_ZigZag::resetOdometry()
{
  m_odometer.reset();
  m_zig_odo.reset();
  m_zag_odo.reset();

  m_stem_dist = 0;
  m_stem_odo = 0;
}

//-----------------------------------------------------------
// Procedure: setState()

void BHV_ZigZag::setState(std::string new_state)
{
  new_state = tolower(new_state);
  if((new_state != "stem") && (new_state != "port") && (new_state != "star"))
    return;
  if(m_state == new_state)
    return;

  // post flags for new leg
  if((new_state == "star") && !legsComplete())
    postFlags(m_star_flags);
  if((new_state == "port") && !legsComplete())
    postFlags(m_port_flags);
  
  m_state = new_state;

}

//------------------------------------------------------------
// Procedure: postSetHdgLine()
//   Purpose: Draw the heading of the desired zig leg

void BHV_ZigZag::postSetHdgLine()
{
  if(!m_draw_set_hdg)
    return;

  double x2,y2;
  projectPoint(m_set_hdg, 20, m_osx, m_osy, x2, y2);
  
  XYSegList segl;
  segl.add_vertex(m_osx, m_osy);
  segl.add_vertex(x2, y2);

  segl.set_label("zig_set_" + m_us_name);
  segl.set_label_color("off");
  segl.set_edge_color(m_hint_set_hdg_color);
  segl.set_vertex_color(m_hint_set_hdg_color);
  segl.set_edge_size(1);
  segl.set_vertex_size(2);
  segl.set_duration(5);
  string spec = segl.get_spec();
  postMessage("VIEW_SEGLIST", spec);
}

//------------------------------------------------------------
// Procedure: postReqHdgLine()
//   Purpose: Draw the heading of the desired zig leg

void BHV_ZigZag::postReqHdgLine()
{
  if(!m_draw_req_hdg)
    return;

  double x2,y2;
  projectPoint(m_req_hdg, 20, m_osx, m_osy, x2, y2);
  
  XYSegList segl;
  segl.add_vertex(m_osx, m_osy);
  segl.add_vertex(x2, y2);

  segl.set_label("zig_req_" + m_us_name);
  segl.set_label_color("off");
  segl.set_edge_color(m_hint_req_hdg_color);
  segl.set_vertex_color(m_hint_req_hdg_color);
  segl.set_edge_size(1);
  segl.set_vertex_size(2);
  segl.set_duration(5);
  string spec = segl.get_spec();
  postMessage("VIEW_SEGLIST", spec);
}

//------------------------------------------------------------
// Procedure: eraseSetHdgLine()
//   Purpose: Erase the heading of the desired zig leg

void BHV_ZigZag::eraseSetHdgLine()
{
  XYSegList segl;
  segl.set_label("zig_set_" + m_us_name);
  postMessage("VIEW_SEGLIST", segl.get_spec_inactive());
}

//------------------------------------------------------------
// Procedure: eraseReqHdgLine()
//   Purpose: Erase the heading of the desired zig leg

void BHV_ZigZag::eraseReqHdgLine()
{
  XYSegList segl;
  segl.set_label("zig_req_" + m_us_name);
  postMessage("VIEW_SEGLIST", segl.get_spec_inactive());
}

//-----------------------------------------------------------
// Procedure: handleConfigVisualHint()

bool BHV_ZigZag::handleConfigVisualHint(string hint)
{
  string param = tolower(biteStringX(hint, '='));
  string value = hint;

  bool handled = false;
  if(param == "draw_set_hdg")
    handled = setBooleanOnString(m_draw_set_hdg, value);
  else if(param == "draw_req_hdg")
    handled = setBooleanOnString(m_draw_req_hdg, value);
  else if(param == "set_hdg_color")
    handled = setColorOnString(m_hint_set_hdg_color, value);
  else if((param == "req_hdg_color") && isColor(value))
    handled = setColorOnString(m_hint_req_hdg_color, value);

  return(handled);  
}


//-----------------------------------------------------------
// Procedure: expandMacros() 

string BHV_ZigZag::expandMacros(string sdata)
{
  sdata = IvPBehavior::expandMacros(sdata);

  sdata = macroExpand(sdata, "ODO", m_odometer.getTotalDist());
  sdata = macroExpand(sdata, "ZIG_ODO", m_zig_odo.getTotalDist());
  sdata = macroExpand(sdata, "ZAG_ODO", m_zag_odo.getTotalDist());

  sdata = macroExpand(sdata, "STEM_DIST", doubleToStringX(m_stem_dist,2));
  sdata = macroExpand(sdata, "STEM_ODO", doubleToStringX(m_stem_odo,2));

  sdata = macroExpand(sdata, "ZIGS", m_zig_cnt);
  sdata = macroExpand(sdata, "ZAGS", (int)(m_zig_cnt/2));
  sdata = macroExpand(sdata, "ZIGS_EVER", m_zig_cnt_ever);
  sdata = macroExpand(sdata, "ZAGS_EVER", (int)(m_zig_cnt_ever/2));
  
  sdata = macroExpand(sdata, "STEM_HDG", m_stem_hdg);
  sdata = macroExpand(sdata, "STEM_SPD", m_speed_orig);
  sdata = macroExpand(sdata, "ZIG_START_SPD", m_zig_spd_start);
  sdata = macroExpand(sdata, "ZIG_MIN_SPD", m_zig_spd_min);
  sdata = macroExpand(sdata, "ZIG_SPD_DELTA", m_zig_spd_delta);

  unsigned int zigs_togo = 0;
  unsigned int zags_togo = 0;
  if(m_max_zig_legs > m_zig_cnt) {
    zigs_togo = m_max_zig_legs - m_zig_cnt;
    zags_togo = (int)(zigs_togo+1 / 2);
  }

  sdata = macroExpand(sdata, "ZIGS_TOGO", zigs_togo);
  sdata = macroExpand(sdata, "ZAGS_TOGO", zags_togo);

  string port = boolToString(m_state == "port");
  string star = boolToString(m_state == "star");
  sdata = macroExpand(sdata, "ZIG_PORT", port);
  sdata = macroExpand(sdata, "ZIG_STAR", star);
  sdata = macroExpand(sdata, "ZIG_SIDE", m_state);

  return(sdata);
}

