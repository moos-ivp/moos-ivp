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

  // Cruise speed of -1 means use os speed when activated
  m_cruise_speed       = 1; // meters/sec
  
  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING");
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
}

//-----------------------------------------------------------
// Procedure: setParam()

bool BHV_FixTurn::setParam(string param, string value) 
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
    m_cruise_speed = -1;
    handled = true;
  }
  else if(param == "speed")
    handled = setNonNegDoubleOnString(m_cruise_speed, value);
  else if(param == "turn_spec")
    handled = handleConfigTurnSpec(value);
  else if(param == "stale_nav_thresh")
    handled = setPosDoubleOnString(m_stale_nav_thresh, value);
  else if(param == "visual_hints")  
    return(m_hints.setHints(value));
  else
    handled = false;

  return(handled);
}

//-----------------------------------------------------------
// Procedure: handleNewHdg()

bool BHV_FixTurn::handleNewHdg()
{
  // Part 1: If in stem state, pick a side and set a heading 
  if(m_state == "stem") {
    if(m_port_turn)
      setState("port");
    else
      setState("star");

    m_osh_prev = m_osh;
    m_stem_hdg = m_osh;
    m_stem_spd = m_osv;
    m_stem_utc = getBufferCurrTime();
    m_hdg_delta_sofar = 0;
    m_odometer.reset(m_stem_utc);      
  }

  double hdg_delta = angleDiff(m_osh_prev, m_osh);
  if(hdg_delta > 0) {
    bool delta_port = true;
    if(angle360(m_osh_prev - m_osh) > angle360(m_osh - m_osh_prev))
      delta_port = false;

    if(m_port_turn != delta_port)
      hdg_delta = -hdg_delta;

    m_hdg_delta_sofar += hdg_delta;
  }
  m_osh_prev = m_osh;
  
  // ========================================================
  // Part 2: Check if turn is still in progress
  // ========================================================
  if(m_hdg_delta_sofar < m_fix_turn)
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
  // Part 1A: Determine the Set Speed
  //===================================================
  // By default just use the configured cruise speed
  double spd = m_cruise_speed;
  // If cruise_speed set to auto or -1, use stem speed
  if(m_cruise_speed < 0)
    spd = m_stem_spd;
  // If there is explicit spd schedule, this spd prevails
  if(m_curr_tix < m_turn_spds.size()) {
    double scheduled_turn_spd = m_turn_spds[m_curr_tix];
    if(scheduled_turn_spd > 0)
      spd = scheduled_turn_spd;
  }

  //===================================================
  // Part 1: Build the Speed ZAIC
  //===================================================
  ZAIC_SPD spd_zaic(m_domain, "speed");
  spd_zaic.setParams(spd, 0.1, spd+0.4, 85, 20);  

  IvPFunction *spd_ipf = spd_zaic.extractIvPFunction();
  if(!spd_ipf)
    postWMessage("Failure on the SPD ZAIC component");


 
  //===================================================
  // Part 2A: Determine the Delta (Mod) Heading
  //===================================================
  // By default just use the configured cruise speed
  double mod_hdg = m_mod_hdg;
  // If there is explicit mhdg schedule, this mhdg prevails
  if(m_curr_tix < m_turn_hdgs.size()) {
    double scheduled_mod_hdg = m_turn_hdgs[m_curr_tix];
    if(scheduled_mod_hdg > 0)
      mod_hdg = scheduled_mod_hdg;
  }
  
  //===================================================
  // Part 2B: Determine the Turn direction
  //===================================================
  // By default just use the configured turn direction
  bool port_turn = m_port_turn;
  // If there is explicit turn_dir schedule, use this
  if(m_curr_tix < m_turn_dirs.size()) {
    string turn_dir = m_turn_dirs[m_curr_tix];
    if(turn_dir == "port")
      port_turn = true;
    else if(turn_dir == "star")
      port_turn = false;
    else if(turn_dir == "auto")
      port_turn = m_port_turn;
  }
  
 //===================================================
  // Part 2: Build the Heading ZAIC
  //===================================================
  double hdg = m_osh;
  if(m_port_turn)
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

  // Part 2: Update the Odometer
  m_odometer.updateDistance(m_osx, m_osy);
  m_odometer.updateTime(getBufferCurrTime());

  // Part 3: Process new heading info and check for done
  bool done = handleNewHdg();
  if(done) {
    setComplete();
    m_curr_tix++;
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
  if((str != "stem") && (str != "port") && (str != "star"))
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

  double new_spd = -1;
  double new_mhdg = -1;
  string new_tdir = "auto";

  vector<string> svector = parseString(spec, ',');  
  
  bool terse_style = true;
  if(strContains(spec, "="))
    terse_style = false;

  // Handle Terse Style of Configuration
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
    
  // Handle Verbose Style of Configuration
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

  m_turn_hdgs.push_back(new_mhdg);
  m_turn_spds.push_back(new_spd);
  m_turn_dirs.push_back(new_tdir);

  return(true);
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

