/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_FixTurn.cpp                                      */
/*    DATE: Oct 16th 2022                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#include <cmath>
#include <cstdlib>
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "VarDataPairUtils.h"
#include "BHV_FixTurn.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "ZAIC_SPD.h"
#include "MacroUtils.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Constructor()

BHV_FixTurn::BHV_FixTurn(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  this->setParam("descriptor", "zig");
  
  m_domain = subDomain(m_domain, "course,speed");
  
  // =====================================================
  // Init State vars
  resetState();
  m_curr_tix = 0; // This is NOT reset upon completion
  
  // =====================================================
  // Init Config vars
  m_fix_turn   = 0;    // degrees
  m_mod_hdg    = 20;   // degrees
  m_port_turn  = true;
  
  m_stale_nav_thresh = 5;  // seconds
  m_schedule_repeat  = false;
  
  m_hide_deprecation_warning = false;
  
  // Cruise speed of -1 means use os speed when activated
  m_cruise_speed = 1; // meters/sec
  
  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING");
  addInfoVars("DESIRED_RUDDER");
}

//-----------------------------------------------------------
// Procedure: resetState()

void BHV_FixTurn::resetState()
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

bool BHV_FixTurn::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);
  
  bool handled = true;
  if(param == "fix_turn")
    handled = setNonNegDoubleOnString(m_fix_turn, val);
  else if(param == "mod_hdg")
    handled = setDoubleOnString(m_mod_hdg, val);  
  else if(param == "turn_dir")
    handled = setPortTurnOnString(m_port_turn, val);
  else if((param == "speed") && (val == "auto")) {
    m_cruise_speed = -1;
    handled = true;
  }
  else if(param == "speed")
    handled = setNonNegDoubleOnString(m_cruise_speed, val);
  else if(param == "turn_spec")
    handled = handleConfigTurnSpec(val);
  else if(param == "stale_nav_thresh")
    handled = setPosDoubleOnString(m_stale_nav_thresh, val);
  else if(param == "radius_rep_var")
    handled = setNonWhiteVarOnString(m_radius_rep_var, val);
  else if(param == "i_understand_this_behavior_is_deprecated")
    return(setBooleanOnString(m_hide_deprecation_warning, val));
  else if(param == "schedule") {
    handled = true;
    if(tolower(val) == "repeat")
      m_schedule_repeat = true;
    else if(tolower(val) == "norepeat")
      m_schedule_repeat = false;
    else
      handled = false;
  }
  else if(param == "visual_hints")  
    return(m_hints.setHints(val));
  else
    handled = false;

  return(handled);
}

//-----------------------------------------------------------
// Procedure: isDeprecated()
//   Purpose: Users of this behavior will get a configuration
//            warning unless they use the "i_understand" param
//            essentially saying use at your own risk.

string BHV_FixTurn::isDeprecated()
{
  if(m_hide_deprecation_warning)
    return("");
  
  string msg;
  msg += "BHV_FixTurn not supported. Use BHV_FixedTurn instead.";
  msg += "# Set i_understand_this_behavior_is_deprecated=true  ";
  msg += "# to suppress this warning and use at your own risk. ";  
  return(msg);
}

//-----------------------------------------------------------
// Procedure: handleNewHdg()

bool BHV_FixTurn::handleNewHdg()
{
  // =====================================================
  // Part 1: If in stem state, pick a side and set stems
  // =====================================================
  if(m_state == "stem") {
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

IvPFunction *BHV_FixTurn::buildOF() 
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

IvPFunction *BHV_FixTurn::onRunState() 
{
  // Part 1: Update ownship position, heading and speed
  if(!updateOSPos() || !updateOSHdg() || !updateOSSpd())
    return(0);

  m_curr_rudder = getBufferDoubleVal("DESIRED_RUDDER");
  
  // Part 2: Update the Odometer
  m_odometer.updateDistance(m_osx, m_osy);
  m_odometer.updateTime(getBufferCurrTime());

  // Part 3: Process new heading info and check for done
  bool done = handleNewHdg();
  if(done) {
    postTurnCompleteReport();
    setComplete();
    m_curr_tix++;
    if(m_schedule_repeat && (m_curr_tix >= m_turn_hdgs.size()))
      m_curr_tix = 0;

    //postBeginPoint(false);
    if(m_perpetual)
      resetState();
    return(0);
  }
  
  // Part 4: Generate IvP function and apply priority
  IvPFunction *ipf = buildOF();
  if(ipf) 
    ipf->setPWT(m_priority_wt);
  else 
    postWMessage("Unable to build IvP Function");    

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: onRunToIdleState()

void BHV_FixTurn::onRunToIdleState()
{
  setState("stem");
}

//-----------------------------------------------------------
// Procedure: onIdleToRunState()

void BHV_FixTurn::onIdleToRunState()
{
  setState("stem");
}

//-----------------------------------------------------------
// Procedure: updateOSPos()
//   Returns: true  If NAV_X/Y info is found and not stale
//            false Otherwise

bool BHV_FixTurn::updateOSPos(string fail_action) 
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

bool BHV_FixTurn::updateOSHdg(string fail_action) 
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

bool BHV_FixTurn::updateOSSpd(string fail_action) 
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

bool BHV_FixTurn::setState(std::string str)
{
  str = tolower(str);
  if((str != "stem") && (str != "turning"))
    return(false);
  
  m_state = str;
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: handleConfigTurnSpec()
//  Examples: spd=3.5, mhdg=25, turn=port
//            clearall, spd=3.5, mhdg=25, turn=star
//            3.5, 25, port
//            3.5, 25
//            clearall, 3.5, 25
//            clearall, 3.5, 25, star

bool BHV_FixTurn::handleConfigTurnSpec(string spec)
{
  spec = tolower(stripBlankEnds(spec));

  // ===============================================
  // Part 1: Handle any clearing preamble
  // ===============================================
  if((spec == "clearall") || (spec == "clear")) {
    m_turn_spds.clear();
    m_turn_hdgs.clear();
    m_curr_tix = 0;
    return(true);
  }
		 
  if(strBegins(spec, "clearall,") ||
     strBegins(spec, "clear,")) {
    m_turn_spds.clear();
    m_turn_hdgs.clear();
    m_curr_tix = 0;
    biteString(spec, ',');
  }

  double new_spd  = -1;
  double new_mhdg = -1;
  double new_fixh = -1;
  string new_tdir = "auto";

  vector<string> svector = parseStringQ(spec, ',');  
  
  // ===============================================
  // Part 2: Determine if format is terse or verbose
  // ===============================================
  bool terse_style = true;
  if(strContains(spec, "="))
    terse_style = false;

  // ===============================================
  // Part 3: Handle Terse Style of Configuration
  // ===============================================
  if(terse_style) {
    for(unsigned int i=0; i<svector.size(); i++) {
      string str = svector[i];
      if(i==0) {
	if(str == "auto")
	  new_spd = -1;
	else if(!isNumber(str))
	  return(false);
	else
	  new_spd = atof(str.c_str());
      }
      else if(i==1) {
	if(str == "auto")
	  new_mhdg = -1;
	else if(!isNumber(str))
	  return(false);
	else
	  new_mhdg = atof(str.c_str());
      }
      else if(i==2) {
	if(str == "auto")
	  new_fixh = -1;
	else if(!isNumber(str))
	  return(false);
	else
	  new_fixh = atof(str.c_str());
      }
      else if(i==3) {
	if((str != "auto") && (str != "port") &&
	   (str != "star") && (str != "starboard"))
	  return(false);
	if(str == "starboard")
	  str = "star";
	new_tdir = str;
      }
      else
	return(false);
    }
  }
    
  // ===============================================
  // Part 4: Handle Non-Terse Style of Configuration
  // ===============================================
  else {
    for(unsigned int i=0; i<svector.size(); i++) {
      string param = biteStringX(svector[i], '=');
      string value = svector[i];
      if(param == "spd") {
	if(value == "auto")
	  new_spd = -1;
	else if(!isNumber(value))
	  return(false);
	else
	  new_spd = atof(value.c_str());
      }
      else if(param == "mhdg") {
	if(value == "auto")
	  new_mhdg = -1;
	else if(!isNumber(value))
	  return(false);
	else
	  new_mhdg = atof(value.c_str());
      }
      else if(param == "fix") {
	if(value == "auto")
	  new_fixh = -1;
	else if(!isNumber(value))
	  return(false);
	else
	  new_fixh = atof(value.c_str());
      }
      else if(param == "turn") {
	if((value != "auto") && (value != "port") &&
	   (value != "star") && (value != "starboard"))
	  return(false);
	if(value == "starboard")
	  value = "star";
	new_tdir = value;

      }
      else
	return(false);
    }
  }

  // ===============================================
  // Part 5: If syntax ok, add turn to the schedule
  // ===============================================
  m_turn_spds.push_back(new_spd);
  m_turn_hdgs.push_back(new_mhdg);
  m_turn_fixh.push_back(new_fixh);
  m_turn_dirs.push_back(new_tdir);

  return(true);
}

//-----------------------------------------------------------
// Procedure: getCurrTurnSpd()
//   Purpose: Determine speed to be used in the current turn.
//            If a schedule is given and not exhausted, it has
//            the last say.

double BHV_FixTurn::getCurrTurnSpd() const
{
  // By default just use the configured cruise speed
  double turn_spd = m_cruise_speed;

  // If cruise_speed set to auto or -1, use stem speed
  if(m_cruise_speed < 0)
    turn_spd = m_stem_spd;

  // If there is explicit spd schedule, this spd prevails
  if(m_curr_tix < m_turn_spds.size()) {
    double scheduled_turn_spd = m_turn_spds[m_curr_tix];
    // If scheduled_turn_spd is negative, then it was
    // specified with "auto", meaning defers to stem or cruise
    if(scheduled_turn_spd > 0)
      turn_spd = scheduled_turn_spd;
  }

  return(turn_spd);  
}


//-----------------------------------------------------------
// Procedure: getCurrModHdg()
//   Purpose: Determine mod_hdg to be used in current turn.
//            Used in calculating the desired heading.
//            desired_hdg is curr ownship hdg plus mod_hdg.

double BHV_FixTurn::getCurrModHdg() const
{
  // By default just use the configured default mod_hdg
  double mod_hdg = m_mod_hdg;

  // If there is explicit mhdg schedule, this mhdg prevails
  if(m_curr_tix < m_turn_hdgs.size()) {
    double scheduled_mod_hdg = m_turn_hdgs[m_curr_tix];
    // If scheduled_mod_hdg is negative, then it was
    // specified with "auto", meaning defers to m_mod_hdg
    if(scheduled_mod_hdg > 0)
      mod_hdg = scheduled_mod_hdg;
  }
  
  return(mod_hdg);  
}


//-----------------------------------------------------------
// Procedure: getCurrFixTurn()
//   Purpose: Determine fix_turn to be used in current turn.
//            This is the total change in hdg required in 
//            order for the turn to be regarded complete

double BHV_FixTurn::getCurrFixTurn() const
{
  // By default just use the configured default fix_turn
  double fix_turn = m_fix_turn;

  // If there is explicit fix_turn schedule, this prevails
  if(m_curr_tix < m_turn_fixh.size()) {
    double scheduled_fix_turn = m_turn_fixh[m_curr_tix];
    // If scheduled_fix_turn is negative, then it was
    // specified with "auto", meaning defers to m_fix_turn
    if(scheduled_fix_turn > 0)
      fix_turn = scheduled_fix_turn;
  }
  
  return(fix_turn);  
}


//-----------------------------------------------------------
// Procedure: getCurrTurnDir()
//   Purpose: Determine port or starboard in current turn.

string BHV_FixTurn::getCurrTurnDir() const
{
  // Default turn, with no schedule, set from m_port_turn
  string turn_dir = "star";
  if(m_port_turn)
    turn_dir = "port";

  // If there is explicit turn_dir schedule, use this
  if(m_curr_tix < m_turn_dirs.size()) {
    string scheduled_turn_dir = m_turn_dirs[m_curr_tix];
    if(scheduled_turn_dir != "auto")
      turn_dir = scheduled_turn_dir;
  }      
  
  return(turn_dir);
}

//-----------------------------------------------------------
// Procedure: postTurnCompleteReport()

void BHV_FixTurn::postTurnCompleteReport()
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
    str += ",avg_rad=" + doubleToStringX(avg_rad);
    str += ",min_rad=" + doubleToStringX(min_rad);
    str += ",max_rad=" + doubleToStringX(max_rad);
  }

  postMessage("FT_REPORT", str+"foo");

  postMessage("VIEW_TEXTBOX", "x=-25,y=-85,msg="+str);
}


//-----------------------------------------------------------
// Procedure: clearTurnVisuals()
//      Note: The turn visuals have a duration, so they should
//            disappear on their own. Presently alogview does
//            not respect durations, so we clear them here just
//            to be cautious and ensure alogvie is not littered.

void BHV_FixTurn::clearTurnVisuals()
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

string BHV_FixTurn::expandMacros(string sdata)
{
  sdata = IvPBehavior::expandMacros(sdata);

  double turn_dist = m_odometer.getTotalDist();
  double turn_time = m_odometer.getTotalElapsed();
  
  sdata = macroExpand(sdata, "TURN_DIST", turn_dist);
  sdata = macroExpand(sdata, "TURN_TIME", turn_time);
  
  return(sdata);
}

