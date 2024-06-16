/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_FixTurn.cpp                                      */
/*    DATE: Oct 16th 2022                                        */
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
#include "BHV_FixedTurn.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "ZAIC_SPD.h"
#include "XYTextBox.h"
#include "MacroUtils.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Constructor()

BHV_FixedTurn::BHV_FixedTurn(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  this->setParam("descriptor", "fixturn");
  
  m_domain = subDomain(m_domain, "course,speed");
  
  // =====================================================
  // Init State vars
  // =====================================================
  resetState();
  
  // =====================================================
  // Init Config vars
  // =====================================================

  // Init default params for a fixed turn if there is not
  // otherwise a schedule of fixed turns being used. Even
  // if a scheduled turn is being used, the scheduled turn
  // may defer to the default values here for one or more
  // components.
  m_fix_turn   = 0;      // degrees
  m_mod_hdg    = 20;     // degrees
  m_port_turn  = true;
  m_timeout    = -1;     // no timeout
  m_cruise_spd = 1;      // meters/sec

  m_turn_delay = -1;     // secs (neg num means disabled)
  m_turn_delay_utc = -1; // UTC time in secs since 1970
  
  m_stale_nav_thresh = 5;  // seconds
  
  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING");
  addInfoVars("DESIRED_RUDDER");
}

//-----------------------------------------------------------
// Procedure: resetState()

void BHV_FixedTurn::resetState()
{
  m_state = "stem";
  m_odometer.reset();
  m_osh_prev = 0;
  m_stem_hdg = 0;
  m_stem_spd = 0;
  m_stem_utc = 0;
  m_hdg_delta_sofar = 0;
  m_curr_rudder = 0;

  vector<XYPoint> new_mark_pts(360);
  m_mark_pts    = new_mark_pts;

  m_mark_rudder.clear();
}

//-----------------------------------------------------------
// Procedure: setParam()

bool BHV_FixedTurn::setParam(string param, string value) 
{
  if(IvPBehavior::setParam(param, value))
    return(true);
  
  bool handled = true;
  if(param == "fix_turn")
    handled = setNonNegDoubleOnString(m_fix_turn, value);
  else if(param == "mod_hdg")
    handled = setDoubleOnString(m_mod_hdg, value);  
  else if(param == "turn_dir")
    handled = setPortTurnOnString(m_port_turn, value);
  else if((param == "speed") && (value == "auto")) {
    m_cruise_spd = -1;
    handled = true;
  }
  else if(param == "speed")
    handled = setNonNegDoubleOnString(m_cruise_spd, value);
  else if(param == "timeout")  
    return(setDoubleOnString(m_timeout, value));
  else if(param == "turn_delay")  
    return(setDoubleOnString(m_turn_delay, value));

  else if(param == "turn_spec")
    handled = m_turn_set.setTurnParams(value);
  else if(param == "stale_nav_thresh")
    handled = setPosDoubleOnString(m_stale_nav_thresh, value);
  else if(param == "radius_rep_var")
    handled = setNonWhiteVarOnString(m_radius_rep_var, value);
  else if(param == "schedule_repeat")
    handled = m_turn_set.setRepeats(value);
  else if(param == "visual_hints")  
    return(m_hints.setHints(value));
  else
    handled = false;

  return(handled);
}

//-----------------------------------------------------------
// Procedure: handleNewHdg()
//   Returns: true if the turn has been accomplished.
//            false otherwise.

bool BHV_FixedTurn::handleNewHdg()
{
  // =====================================================
  // Part 0: If delay is being used, handle/check here.
  // =====================================================
  double curr_utc = getBufferCurrTime();
  if(m_state == "stem") {
    if(m_turn_delay > 0) {
      setState("delay");
      m_turn_delay_utc = curr_utc;
      return(false);
    }
  }
  
  if(m_state == "delay") {
    double elapsed = curr_utc - m_turn_delay_utc;
    if(elapsed < m_turn_delay)
      return(false);
  }
  
  // =====================================================
  // Part 1: If entering turning state, handle inits
  // =====================================================
  if((m_state == "stem") || (m_state == "delay")) {
    setState("turning");
    m_osh_prev = m_osh;
    m_stem_hdg = m_osh;
    m_stem_spd = m_osv;
    m_stem_utc = getBufferCurrTime();
    m_hdg_delta_sofar = 0;
    m_odometer.reset(m_stem_utc);
    clearTurnVisuals();
  }

  // =====================================================
  // Part 2: Det hdg direction apply to total hdg_delta
  // =====================================================
  double hdg_delta = angleDiff(m_osh_prev, m_osh);

  if(hdg_delta > 0) {
    bool delta_port = true;
    if((m_osh > m_osh_prev) || ((m_osh_prev - m_osh) > 180))
      delta_port = false;

    if((getCurrTurnDir() == "port") && !delta_port)
      hdg_delta = -hdg_delta;    
    if((getCurrTurnDir() == "star") && delta_port)
      hdg_delta = -hdg_delta;    

    m_hdg_delta_sofar += hdg_delta;

    int hdg_360 = (int)(angle360(m_hdg_delta_sofar));

    // ===================================================
    // Part 2B Process hdg into saved history
    // ===================================================
    // safety double check values and array dimension
    if((hdg_360 >= 0) && (hdg_360 < 360) && (m_mark_pts.size() == 360)) {
      XYPoint mark_pt(m_osx, m_osy);
      if(m_mark_pts[hdg_360].valid()) {
	XYPoint mid_pt = midPoint(m_mark_pts[hdg_360], mark_pt);
	m_mark_pts[hdg_360] = mid_pt;
      }
      else
	m_mark_pts[hdg_360] = mark_pt;
    }

    m_mark_rudder.push_front(m_curr_rudder);
  }

  m_osh_prev = m_osh;
  
  // ========================================================
  // Part 3: Check if turn is still in progress
  // ========================================================
  if(m_hdg_delta_sofar < getCurrFixTurn())
    return(false);

  setState("stem");
  return(true);
 
}

//-----------------------------------------------------------
// Procedure: buildOF()

IvPFunction *BHV_FixedTurn::buildOF() 
{
  IvPFunction *ipf = 0;  
  // Sanity Check
  if(m_state == "off")
    return(0);
  
  //===================================================
  // Part 1: Build the Speed ZAIC
  //===================================================
  double spd = getCurrTurnSpd();
  ZAIC_SPD spd_zaic(m_domain, "speed");
  spd_zaic.setParams(spd, 0.1, spd+0.4, 85, 20);  

  IvPFunction *spd_ipf = spd_zaic.extractIvPFunction();
  if(!spd_ipf)
    postWMessage("Failure on the SPD ZAIC component");

  //===================================================
  // Part 2B: Build the Heading ZAIC
  //===================================================
  double mod_hdg = getCurrModHdg();
  double hdg = m_osh;
  if(getCurrTurnDir() == "port")
    hdg = angle360(hdg - mod_hdg);
  else
    hdg = angle360(hdg + mod_hdg);

  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setSummit(hdg);
  crs_zaic.setBaseWidth(180);
  crs_zaic.setValueWrap(true);
  
  IvPFunction *crs_ipf = crs_zaic.extractIvPFunction(false);  
  if(!crs_ipf) 
    postWMessage("Failure on the CRS ZAIC");

  //===================================================
  // Part 3: Build the Coupled Heading/Speed IvP Function
  //===================================================
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

IvPFunction *BHV_FixedTurn::onRunState() 
{
  // Part 1: Update ownship position, heading and speed
  if(!updateOSPos() || !updateOSHdg() || !updateOSSpd())
    return(0);

  m_curr_rudder = getBufferDoubleVal("DESIRED_RUDDER");
  
  bool done_timeout = false;
  // Part 2: Update the Odometer and check for timeout
  m_odometer.updateDistance(m_osx, m_osy);
  m_odometer.updateTime(getBufferCurrTime());

  double elapsed = m_odometer.getTotalElapsed();
  double curr_timeout = getCurrTimeOut();
  if((curr_timeout > 0) && (elapsed > curr_timeout))
    done_timeout = true;

  // Part 3: Process new heading info and check for done
  bool done_hdg_change = handleNewHdg();
  
  if(done_timeout || done_hdg_change) {
    postTurnCompleteReport();
    setComplete();
    
    FixedTurn turn = m_turn_set.getFixedTurn();
    vector<VarDataPair> flags = turn.getFlags();
    postFlags(flags);
    m_turn_set.increment();
    
    if(m_perpetual)
      resetState();
    return(0);
  }
  
  // Part 5: Generate IvP function and apply priority
  IvPFunction *ipf = buildOF();
  if(ipf) 
    ipf->setPWT(m_priority_wt);
  else 
    postWMessage("Unable to build IvP Function");    

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: onRunToIdleState()

void BHV_FixedTurn::onRunToIdleState()
{
  setState("stem");
}

//-----------------------------------------------------------
// Procedure: onIdleToRunState()

void BHV_FixedTurn::onIdleToRunState()
{
  setState("stem");
}

//-----------------------------------------------------------
// Procedure: updateOSPos()
//   Returns: true  If NAV_X/Y info is found and not stale
//            false Otherwise

bool BHV_FixedTurn::updateOSPos(string fail_action) 
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
//   Returns: true  If NAV_HEADING info is found and not stale
//            false Otherwise

bool BHV_FixedTurn::updateOSHdg(string fail_action) 
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
//   Returns: true:  If NAV_SPEED info is found and not stale
//            false: Otherwise

bool BHV_FixedTurn::updateOSSpd(string fail_action) 
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
// Procedure: setState()

bool BHV_FixedTurn::setState(std::string str)
{
  str = tolower(str);
  if((str != "stem") && (str != "turning") && (str != "delay"))
    return(false);
  
  m_state = str;
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: getCurrTurnSpd()
//   Purpose: Determine speed to be used in the current turn.
//            If a schedule is given and not exhausted, it has
//            the last say.

double BHV_FixedTurn::getCurrTurnSpd() const
{
  // By default just use the configured cruise speed
  double turn_spd = m_cruise_spd;

  // If cruise_speed set to auto or -1, use stem speed
  if(m_cruise_spd < 0)
    turn_spd = m_stem_spd;

  FixedTurn turn = m_turn_set.getFixedTurn();
  double scheduled_turn_spd = turn.getTurnSpd();
  // If scheduled_turn_spd is negative, then it was
  // specified with "auto", meaning defers to stem or cruise
  if(scheduled_turn_spd > 0)
    turn_spd = scheduled_turn_spd;

  return(turn_spd);  
}


//-----------------------------------------------------------
// Procedure: getCurrModHdg()
//   Purpose: Determine mod_hdg to be used in current turn.
//            Used in calculating the desired heading.
//            desired_hdg is curr ownship hdg plus mod_hdg.

double BHV_FixedTurn::getCurrModHdg() const
{
  // If in the delay state, just continue current heading
  // until delay is finished, presumably to let the vehicle
  // adjust speed.
  if(m_state == "delay") 
    return(0);
      
  
  // By default just use the configured default mod_hdg
  double mod_hdg = m_mod_hdg;

  FixedTurn turn = m_turn_set.getFixedTurn();
  double scheduled_mod_hdg = turn.getTurnModHdg();
  // If scheduled_mod_hdg is negative, then it was
  // specified with "auto", meaning defers to m_mod_hdg
  if(scheduled_mod_hdg > 0)
    mod_hdg = scheduled_mod_hdg;
  
  return(mod_hdg);  
}


//-----------------------------------------------------------
// Procedure: getCurrFixTurn()
//   Purpose: Determine fix_turn to be used in current turn.
//            This is the total change in hdg required in 
//            order for the turn to be regarded complete

double BHV_FixedTurn::getCurrFixTurn() const
{
  // By default just use the configured default fix_turn
  double fix_turn = m_fix_turn;

  FixedTurn turn = m_turn_set.getFixedTurn();
  double scheduled_fix_turn = turn.getTurnFixHdg();
  // If scheduled_fix_hdg is negative, then it was
  // specified with "auto", meaning defers to m_fix_turn
  if(scheduled_fix_turn > 0)
    fix_turn = scheduled_fix_turn;
  
  return(fix_turn);  
}

//-----------------------------------------------------------
// Procedure: getCurrTurnDir()

string BHV_FixedTurn::getCurrTurnDir() const
{
  // Default turn, with no schedule, set from m_port_turn
  string turn_dir = "star";
  if(m_port_turn)
    turn_dir = "port";

  FixedTurn turn = m_turn_set.getFixedTurn();
  string scheduled_turn_dir = turn.getTurnDir();
  // If there is explicit turn_dir schedule, use this
  if(scheduled_turn_dir != "auto")
    turn_dir = scheduled_turn_dir;
  
  return(turn_dir);
}

//-----------------------------------------------------------
// Procedure: getCurrTimeOut()

double BHV_FixedTurn::getCurrTimeOut() const
{
  double timeout = m_timeout;

  FixedTurn turn = m_turn_set.getFixedTurn();
  double scheduled_timeout = turn.getTurnTimeOut();
  // If there is explicit turn_dir schedule, use this
  if(scheduled_timeout > 0)
    timeout = scheduled_timeout;

  return(timeout);
}

//-----------------------------------------------------------
// Procedure: postTurnCompleteReport()

void BHV_FixedTurn::postTurnCompleteReport()
{
  if(m_mark_pts.size() != 360)
    return;

  double min_rad = -1;
  double max_rad = -1;
  double total_rad = 0;
  int    rad_count = 0;

  vector<XYSegList> segls;
  
  for(unsigned int i=0; i<180; i++) {
    XYPoint pta = m_mark_pts[i]; 
    XYPoint ptb = m_mark_pts[i+180]; 
    if(pta.valid() && ptb.valid()) {
      double rad = distPointToPoint(pta, ptb);
      if((min_rad < 0) || (rad < min_rad))
	min_rad = rad;
      if((max_rad < 0) || (rad > max_rad))
	max_rad = rad;
      total_rad += rad;
      rad_count++;

      XYSegList segl(pta, ptb);
      segls.push_back(segl);
    }
  }

  for(unsigned int i=0; i<segls.size(); i++) {
    XYSegList segl = segls[i];
    string label = m_us_name + "ft" + intToString(i);
    m_set_radial_segls.insert(label);
    segl.set_label(label);
    segl.set_label_color("off");
    segl.set_edge_color("gray70");
    segl.set_vertex_color("light_green");
    segl.set_vertex_size(5);
    segl.set_duration(60);
    postMessage("VIEW_SEGLIST", segl.get_spec());
  }

  double rudder_avg = 0;
  if(m_mark_rudder.size() > 0) {
    double rudder_total = 0;
    list<double>::iterator p;
    for(p=m_mark_rudder.begin(); p!=m_mark_rudder.end(); p++)
      rudder_total += *p;
    rudder_avg = rudder_total / (double)(m_mark_rudder.size());
  }
  
  
  string str = "spd=" + doubleToStringX(getCurrTurnSpd());
  str += ",mod_hdg=" + doubleToStringX(getCurrModHdg());
  str += ",rudder_avg=" + doubleToStringX(rudder_avg,1);

  if(total_rad > 0) {
    double avg_rad = 0;
    if(rad_count > 0)
      avg_rad = (total_rad / (double)(rad_count));
    str += ",avg_rad=" + doubleToStringX(avg_rad,2);
    str += ",min_rad=" + doubleToStringX(min_rad,2);
    str += ",max_rad=" + doubleToStringX(max_rad,2);
  }

  postMessage("FT_REPORT", str);

#if 0
  XYTextBox tbox(-25, -85, "ft_report_"+m_us_name);
  tbox.setMsg(str);
  string spec = tbox.get_spec();  
  postMessage("VIEW_TEXTBOX", spec);
#endif
}


//-----------------------------------------------------------
// Procedure: clearTurnVisuals()
//      Note: The turn visuals have a duration, so they should
//            disappear on their own. Presently alogview does
//            not respect durations, so we clear them here just
//            to be cautious and ensure alogview is not littered.

void BHV_FixedTurn::clearTurnVisuals()
{
  set<string>::iterator p;
  for(p=m_set_radial_segls.begin(); p!=m_set_radial_segls.end(); p++) {
    string label = *p;
    string spec = getSeglSpecInactive(label);
    postMessage("VIEW_SEGLIST", spec);
  }      
  m_set_radial_segls.clear();
}

//-----------------------------------------------------------
// Procedure: expandMacros() 

string BHV_FixedTurn::expandMacros(string sdata)
{
  sdata = IvPBehavior::expandMacros(sdata);

  double turn_dist = m_odometer.getTotalDist();
  double turn_time = m_odometer.getTotalElapsed();
  
  sdata = macroExpand(sdata, "TURN_DIST", turn_dist, 1);
  sdata = macroExpand(sdata, "TURN_TIME", turn_time);
  
  return(sdata);
}


