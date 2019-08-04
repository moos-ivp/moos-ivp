/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_AvdColregsV17.cpp                                */
/*    DATE: May 16th, 2013 (combined from individual colrg bvs)  */
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


#include <iostream>
#include <cmath>
#include <cstdlib>
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BHV_AvdColregsV17.h"
#include "AOF_R17.h"
#include "AOF_R16.h"
#include "AOF_R14.h"
#include "AOF_R13.h"
#include "AOF_CPA.h"
#include "OF_Reflector.h"
#include "OF_Coupler.h"
#include "ZAIC_SPD.h"
#include "ZAIC_HDG.h"
#include "BuildUtils.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_AvdColregsV17::BHV_AvdColregsV17(IvPDomain gdomain) : 
  IvPContactBehavior(gdomain)
{
  this->setParam("descriptor", "colregs");
  //this->setParam("build_info", "uniform_piece=discrete@course:12,speed:6");
  //this->setParam("build_info", "uniform_grid=course:12,speed:6");
  this->setParam("build_info", "uniform_piece=discrete@course:3,speed:3");
  this->setParam("build_info", "uniform_grid=course:9,speed:6");

  m_domain = subDomain(m_domain, "course,speed");

  m_pwt_outer_dist    = 100;
  m_pwt_inner_dist    = 90;
  m_min_util_cpa_dist = 10; 
  m_max_util_cpa_dist = 75; 

  m_actual_pwt = 0;

  // Rule 13 Overtaking Threshold
  m_overtaking_bng_range = 67.5; // Was 45 changed sep2713

  // Rule 14 Head-on threshold
  //  m_headon_bng_range     = 25;
  m_headon_abs_relbng_thresh = 12;//25;

  m_completed_dist    = 100;
  m_initial_speed     = 0;

  m_avoid_mode    = "none";
  m_avoid_submode = "none";
  m_pwt_grade     = "linear";

  m_memory_time = 5;
  m_turn_radius = 0;
  
  m_no_alert_request = false;

  m_iterations = 0;
  m_cn_crossed_os_port_star = false;

  m_time_on_leg = 120;
  
  m_debug1     = "n/a";
  m_debug2     = "n/a";
  m_debug3     = "n/a";
  m_debug4     = "n/a";

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING");
  
  m_standon_set = false;
}

//-----------------------------------------------------------
// Procedure: setParam

// m_all_clear_dist    = m_max_util_cpa_dist
// m_collision_dist    = m_min_util_cpa_dist
// m_active_inner_dist = m_pwt_inner_dist
// m_active_outer_dist = m_pwt_outer_dist

bool BHV_AvdColregsV17::setParam(string param, string value) 
{
  if(IvPContactBehavior::setParam(param, value))
    return(true);

  double dval = atof(value.c_str());
  bool   non_neg_number = (isNumber(value) && (dval >= 0));

  
  if((param == "pwt_outer_dist") && non_neg_number) {
    m_pwt_outer_dist = dval;
    // enforce: inner_dist <= outer_dist <= completed_dist
    if(m_pwt_outer_dist < m_pwt_inner_dist)
      m_pwt_inner_dist = m_pwt_outer_dist;
    if(m_pwt_outer_dist > m_completed_dist)
      m_completed_dist = m_pwt_outer_dist;
  }  
  else if((param == "pwt_inner_dist") && non_neg_number) {
    m_pwt_inner_dist = dval;
    // enforce: inner_dist <= outer_dist <= completed_dist
    if(m_pwt_inner_dist > m_pwt_outer_dist)
      m_pwt_outer_dist = m_pwt_inner_dist;
    if(m_pwt_inner_dist > m_completed_dist)
      m_completed_dist = m_pwt_inner_dist;
  }  
  else if((param == "completed_dist") && non_neg_number) {
    m_completed_dist = dval;
    // enforce: inner_dist <= outer_dist <= completed_dist
    if(m_completed_dist < m_pwt_outer_dist)
      m_pwt_outer_dist = m_completed_dist;
    if(m_completed_dist < m_pwt_inner_dist)
      m_pwt_inner_dist = m_completed_dist;
  }
  else if((param == "min_util_cpa_dist") && non_neg_number) {
    m_min_util_cpa_dist = dval;
  }
  else if((param == "max_util_cpa_dist") && non_neg_number) {
    m_max_util_cpa_dist = dval;
    if(m_max_util_cpa_dist < m_min_util_cpa_dist)
      m_min_util_cpa_dist = m_max_util_cpa_dist;
  }
  else if((param == "turn_radius") && non_neg_number) 
    m_turn_radius = dval;
  else if(param == "avoid_mode")
    return(resetAvoidModes(value));
  else if(param == "avoid_submode")
    return(resetAvoidModes(m_avoid_mode, value));
  else if(param == "no_alert_request")
    return(setBooleanOnString(m_no_alert_request, value));
  else if((param == "contact_type_required") && (value != "")) {
    m_contact_type_required = tolower(value);
    return(true);
  }  
  else if(param == "pwt_grade") {
    value = tolower(value);
    if((value!="linear") && (value!="quadratic") && (value!="quasi"))
      return(false);
    m_pwt_grade = value;
    return(true);
  }  
  else
    return(false);

  return(true);
}

//-----------------------------------------------------------
// Procedure: onHelmStart()

void BHV_AvdColregsV17::onHelmStart()
{
  if(m_no_alert_request || (m_update_var == ""))
    return;

  string s_alert_range = doubleToStringX(m_pwt_outer_dist,1);
  string s_cpa_range   = doubleToStringX(m_completed_dist,1);
  string s_alert_templ = "name=$[VNAME] # contact=$[VNAME]";

  string alert_request = "id=avdcol_" + getDescriptor();
  alert_request += ", var=" + m_update_var;
  alert_request += ", val=" + s_alert_templ;
  alert_request += ", alert_range=" + s_alert_range;
  alert_request += ", cpa_range=" + s_cpa_range;

  if(m_contact_type_required != "")
    alert_request += ", contact_type=" + m_contact_type_required;
    
  postMessage("BCM_ALERT_REQUEST", alert_request);
}


//-----------------------------------------------------------
// Procedure: onIdleState()

void BHV_AvdColregsV17::onIdleState() 
{
  if(updatePlatformInfo())
    postStatusInfo();
}

//-----------------------------------------------------------
// Procedure: onCompleteState()

void BHV_AvdColregsV17::onCompleteState() 
{
  m_avoid_mode = "complete";
  postStatusInfo();
}

//-----------------------------------------------------------
// Procedure: onRunStatePrior()

bool BHV_AvdColregsV17::onRunStatePrior() 
{
  return(true);
  if(m_last_runcheck_post) {
    m_last_runcheck_post = false;
    return(false);
  }

  m_last_runcheck_post = true;
  return(true);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_AvdColregsV17::onRunState() 
{
  m_iterations++;

  bool prev_cn_port_of_os = m_cn_port_of_os;

  if(!updatePlatformInfo())
    return(0);
  if(!checkContactGroupRestrictions())
    return(0);

  m_cn_crossed_os_port_star = false;
  if((m_iterations > 1) && (m_cn_port_of_os != prev_cn_port_of_os))
    m_cn_crossed_os_port_star = true;

  postMessage("CN_FORE_OF_OS_"+m_contact, m_cn_fore_of_os);
  postMessage("CN_PORT_OF_OS_"+m_contact, m_cn_port_of_os);

  postMessage("CN_CROSSED_OS_PORT_STAR_"+m_contact, m_cn_crossed_os_port_star);
  
  if(m_contact_range >= m_completed_dist) {
    setComplete();
    return(0);
  }

  if(m_cn_retired) {
    setComplete();
    return(0);
  }
  
  double curr_time = getBufferCurrTime();
  addHeading(m_cnh, curr_time);
  double os_cn_rel_bng = relBearing(m_osx, m_osy, m_osh, m_cnx, m_cny);
  addRelBng(os_cn_rel_bng, curr_time);


  updateAvoidMode();
  double relevance = getRelevance();

  postMessage("COL17_RELEVANCE", relevance);
  
  IvPFunction *ipf = 0;
  if(relevance > 0) {
    if(m_avoid_mode == "headon") 
      ipf = buildHeadOnIPF();
    else if(m_avoid_mode == "overtaking") 
      ipf = buildOvertakingIPF();
    else if(m_avoid_mode == "giveway") 
      ipf = buildGiveWayIPF();
    else if((m_avoid_mode == "standon") || (m_avoid_mode == "standon_ot")) 
      ipf = buildStandOnIPF();
    else if(m_avoid_mode == "cpa")
      ipf = buildCPA_IPF();
  }

  if(ipf) {
    m_actual_pwt = relevance * m_priority_wt;
    ipf->getPDMap()->normalize(0.0, 100.0);
    ipf->setPWT(m_actual_pwt);

    m_debug2 = doubleToString(m_priority_wt,2) + ":" +
      doubleToString(m_actual_pwt,2);
    m_debug4 = "pieces: " + uintToString(ipf->size());
  }
  else {
    m_debug2 = "n/a";
    m_debug4 = "n/a";
  }
    
  postStatusInfo();
  return(ipf);
}

//-----------------------------------------------------------
// Procedure: getInfo

string BHV_AvdColregsV17::getInfo(string param)
{
  string result;
  
  if(param == "full_avoid_mode")
    result = m_avoid_mode + "#[" + m_avoid_submode + "]";
  else if(param == "avoid_mode")
    result = m_avoid_mode;
  else if(param == "avoid_submode")
    result = m_avoid_submode;
  else if(param == "debug1")
    result = m_debug1;
  else if(param == "debug2")
    result = m_debug2;
  else if(param == "debug3")
    result = m_debug3;
  else if(param == "debug4")
    result = m_debug4;

  return(result);
}

//-----------------------------------------------------------
// Procedure: getDoubleInfo

double BHV_AvdColregsV17::getDoubleInfo(string param)
{
  double result = 0;

  if(param == "pwt_inner_dist")
    result = m_pwt_inner_dist;
  else if(param == "pwt_outer_dist")
    result = m_pwt_outer_dist;

  return(result);
}

//--------------------------------------------------------------
// Procedure: updateAvoidMode

void BHV_AvdColregsV17::updateAvoidMode()
{
  // Part 0: Note the current mode and submode so we can check for a change
  string prev_avoid_mode    = m_avoid_mode;
  string prev_avoid_submode = m_avoid_submode;

  // Part 1: Do the actual mode and submode determinations
  if((m_avoid_mode=="none")||(m_avoid_mode=="overtaking")||(m_avoid_mode=="cpa"))
    checkModeOvertaking();
 
  if((m_avoid_mode=="none")||(m_avoid_mode=="headon")||(m_avoid_mode=="cpa"))
    checkModeHeadOn();
  
  if((m_avoid_mode=="none")||(m_avoid_mode=="standon_ot")||(m_avoid_mode=="cpa"))
    checkModeStandOnOT();

  if((m_avoid_mode=="none")||(m_avoid_mode=="giveway")||(m_avoid_mode=="cpa"))
    checkModeGiveWay();

  if((m_avoid_mode=="none")||(m_avoid_mode=="standon")||(m_avoid_mode=="cpa"))
    checkModeStandOn();

  if((m_avoid_mode == "none") || (m_avoid_mode == "cpa"))
    checkModeCPA();

  // Part 2: Compare the newly determined mode and submode and note if they have
  // changed. Then update the amount of elapsed time in this mode
  double curr_time = getBufferCurrTime();
  if(prev_avoid_mode != m_avoid_mode) 
    m_avoid_mode_start_time = curr_time;
  if((prev_avoid_mode != m_avoid_mode) || (prev_avoid_submode != m_avoid_submode))
    m_avoid_submode_start_time = curr_time;

  m_avoid_mode_elaps_time    = (curr_time - m_avoid_mode_start_time);
  m_avoid_submode_elaps_time = (curr_time - m_avoid_submode_start_time);
  
  // Part 3: If transitioned into standon or giveway, mark certain values
  if(((prev_avoid_mode != "standon") && (prev_avoid_mode != "standon_ot")) &&
     ((m_avoid_mode=="standon") || (m_avoid_mode=="standon_ot"))) {
    if(!m_standon_set){
      m_osh_orig_standon = m_osh;
      m_spd_orig_standon = m_osv;
      m_standon_set = true;
    }  
  }
  else
    m_standon_set = false;
  
}

//=====================================================================
// Protected Functions
//=====================================================================

//-----------------------------------------------------------
// Procedure: checkModeOvertaking                   (Rule 13)
//      Note: TARB [150,180]
//      Note: OCRB [-45,135]

void BHV_AvdColregsV17::checkModeOvertaking()
{
  //=====================================================================  
  // Part 1: Sanity checks, and absolute release check based on range
  //=====================================================================  
  // Check if existing mode is either "cpa", "none", or "overtaking"
  if((m_avoid_mode!="overtaking") && (m_avoid_mode!="none") && (m_avoid_mode!="cpa"))
    return;
  // Check if contact beyond outer priority weight distance
  if(m_contact_range >= m_pwt_outer_dist) {
    resetAvoidModes();
    return;
  }

  // Part 1B: check rate of closure. Regardless of whether in this mode 
  // or not, if not actually overtaking, then not in overtaking mode.
  if(m_rate_of_closure < 0) {
    resetAvoidModes();
    return;
  }

  //=====================================================================  
  // Part 2: RELEASE conditions: if already in giveway mode, check mode-exit 
  // criteria. Release/entry conditions are not same, to prevent mode thrashing.
  //=====================================================================  
  if(m_avoid_mode == "overtaking") {
    if((m_cn_os_rel_bng > 337.5) || (m_cn_os_rel_bng < 22.5)){
      // Rule 13(d) Any subsequent alteration of the bearing between
      // the two vessels shall not make the overtaking vessel a
      // crossing vessel within the meaning of these Rules or relieve
      // her of the duty of keeping clear of the overtaken vessel until
      // she is finally past and clear.
      resetAvoidModes("cpa","cpa");
    }

    // Added jun0316 mikerb
    // OK, we're still in the overtaking mode, but we may want to
    // re-think which side we are trying to pass the contact.
    double thrash_buffer = 5; // degrees
    if(m_avoid_submode == "port") {  // possibly exit the port submode
      if((m_os_cn_rel_bng > 180) && (m_os_cn_rel_bng < (360-thrash_buffer)))
	m_avoid_submode = "starboard";
    }
    else { // possibly exit the starboard submode
      if(m_os_cn_rel_bng < thrash_buffer)
	m_avoid_submode = "port";
    }
    return;
  }

  //=====================================================================  
  // Part 3: ENTRY conditions checked it not already in Overtaking mode
  //=====================================================================  
  // Part 3: Examine our entry criteria. Now that we've established
  // that we were not in this mode on the previous iteration, see if
  // this mode is now right.
  
  // Part 3A: Check if contact angle (relbng from cn to os) criteria is met
  double corb_min = 180 - m_overtaking_bng_range;
  double corb_max = 180 + m_overtaking_bng_range;
  if((m_cn_os_rel_bng < corb_min) || (m_cn_os_rel_bng > corb_max)) {
    resetAvoidModes();
    return;
  }
  // Part 3B: Check if ownship is on a trajectory to pass the contact
  // Not the same as checking for rate-of-closer/range-rate. It's possible
  // to be closing range but not on a trajectory to pass/overtake.
  if(!m_os_passes_cn) {
    resetAvoidModes();
    return;
  }
  m_avoid_mode = "overtaking";

  //=====================================================================  
  // Part 4: SUBMODE determination is made
  //=====================================================================  

  // Determine which side we should aspire to pass on based on present
  // trajectory.
  if(m_cn_port_of_os) 
    m_avoid_submode = "starboard";
  else
    m_avoid_submode = "port";

}

//-----------------------------------------------------------
// Procedure: buildOvertakingIPF                    (Rule 13)

IvPFunction* BHV_AvdColregsV17::buildOvertakingIPF()
{
  // If ownship is already within the min_util_cpa_dist then we shrink
  // this distance, othewise the resulting ivp function will be universally
  // negative, resulting a function with no preference in any direction.
  double min_util_cpa_dist = m_min_util_cpa_dist;
  if(m_contact_range <= m_min_util_cpa_dist)
    min_util_cpa_dist = (m_contact_range / 2);

  //AOF_AvoidCollision aof(m_domain);
  AOF_R13 aof(m_domain);
  aof.setOwnshipParams(m_osx, m_osy);
  aof.setContactParams(m_cnx, m_cny, m_cnh, m_cnv);
  aof.setParam("tol", m_time_on_leg);
  aof.setParam("passing_side", m_avoid_submode);
  aof.setParam("collision_distance", min_util_cpa_dist);
  aof.setParam("all_clear_distance", m_max_util_cpa_dist);
  bool init_ok = aof.initialize();
  
  if(!init_ok) {
    postEMessage("Unable to init AOF_R13.");
    return(0);
  }
  
  OF_Reflector reflector(&aof, 1);
  reflector.create(m_build_info);
  //reflector.create(400000);
  IvPFunction *ipf = reflector.extractOF();

  return(ipf);
}

//--------------------------------------------------------------
// Procedure: checkModeHeadOn                          (Rule 14)

void BHV_AvdColregsV17::checkModeHeadOn()
{
  //=====================================================================  
  // Part 1: Sanity checks, and absolute release check based on range
  //=====================================================================  
  // Check if existing mode is either "cpa", "none", or "headon"
  if((m_avoid_mode!="headon") && (m_avoid_mode!="none") && (m_avoid_mode!="cpa"))
    return;

  string debug_var = "HEADON_DEBUG_" + toupper(m_us_name);
  string debug_msg = "contact=" + m_contact + ",";
  
  //=====================================================================
  // Part 2: RELEASE conditions: if already in headon mode, check
  // mode-exit criteria. Release/entry conditions are not same, to
  // prevent mode thrashing.
  // =====================================================================
  if(m_avoid_mode == "headon") {
    if((m_contact_range > m_min_util_cpa_dist) && (m_rate_of_closure < 0)) {
      postMessage(debug_var, debug_msg + "release due to opening range");
      resetAvoidModes();
    }
    if(m_os_aft_of_cn) {
      postMessage(debug_var, debug_msg + "release due to os aft of cn");
      resetAvoidModes();
    }

    // Check if vehicles have progressed (due to other influences) passing
    // to the left, to point where passing to right is no longer wanted.
    if(m_os_starboard_of_cn && m_cn_starboard_of_os) {
      double os_bng_extra = m_os_cn_rel_bng - m_headon_abs_relbng_thresh;
      double cn_bng_extra = m_cn_os_rel_bng - m_headon_abs_relbng_thresh;
      double tot_bng_extra = os_bng_extra + cn_bng_extra;
      if((os_bng_extra > 5) || (cn_bng_extra > 5) || ((tot_bng_extra > 7.5))) {
	resetAvoidModes();
	postMessage(debug_var, debug_msg + "release due to passing to port");
      }
    }
    return;
  }

  //=====================================================================  
  // Part 3: ENTRY conditions checked it not already in Overtaking mode
  //=====================================================================  
  
  // Part 3a: First criteria applied is overall range to contact
  // If mode entry criteria not met, reset mode/submode to none, return now.
  if(m_contact_range >= m_pwt_outer_dist*2) {
    resetAvoidModes();
    postMessage(debug_var, debug_msg + "not entered due to range");
    return;
  }

  // Part 3b: Second criteria based on total absolute relative bearing
  // If mode entry criteria not met, reset mode/submode to none, and return now.

  double os_cn_abs_rel_bearing = absRelBearing(m_osx,m_osy,m_osh,m_cnx,m_cny);
  double cn_os_abs_rel_bearing = absRelBearing(m_cnx,m_cny,m_cnh,m_osx,m_osy);

  if((os_cn_abs_rel_bearing > m_headon_abs_relbng_thresh) ||
     (cn_os_abs_rel_bearing > m_headon_abs_relbng_thresh)) {
    postMessage(debug_var, debug_msg + "not entered due to rel bearing");
    resetAvoidModes();
    return;
  }

  postMessage(debug_var, debug_msg + "entered head-on mode");
  m_avoid_mode    = "headon";
  m_avoid_submode = "none";
}

//--------------------------------------------------------------
// Procedure: buildHeadOnIPF                           (Rule 14)

IvPFunction* BHV_AvdColregsV17::buildHeadOnIPF()
{
  // If ownship is already within the min_util_cpa_dist then we shrink
  // this distance, othewise the resulting ivp function will be universally
  // negative, resulting a function with no preference in any direction.
  double min_util_cpa_dist = m_min_util_cpa_dist;
  if(m_contact_range <= m_min_util_cpa_dist)
    min_util_cpa_dist = (m_contact_range / 2);

  AOF_R14 aof(m_domain);
  aof.setOwnshipParams(m_osx, m_osy);
  aof.setContactParams(m_cnx, m_cny, m_cnh, m_cnv);
  aof.setParam("tol", 120);
  aof.setParam("collision_distance", min_util_cpa_dist);
  aof.setParam("all_clear_distance", m_max_util_cpa_dist);
  bool init_ok = aof.initialize();
  
  if(!init_ok) {
    postEMessage("Unable to init AOF_R14.");
    return(0);
  }
  
  OF_Reflector reflector(&aof, 1);
  reflector.create(m_build_info);
  IvPFunction *ipf = reflector.extractOF();
  return(ipf);
}



//--------------------------------------------------------------
// Procedure: checkModeGiveWay                         (Rule 16)

void BHV_AvdColregsV17::checkModeGiveWay()
{
  //=====================================================================  
  // Part 1: Sanity checks, and absolute release check based on range
  //=====================================================================  
  // Check if existing mode is either "cpa", "none", or "standon"
  if((m_avoid_mode!="giveway") && (m_avoid_mode!="none") && (m_avoid_mode!="cpa"))
    return;
  // Check if contact beyond outer priority weight distance
  if(m_contact_range >= m_pwt_outer_dist) {
    resetAvoidModes();
    return;
  }

  // NOTE: we dont really let the vehicle switch from bow to stern
  // mode. Kind of stuck in the original mode. Add some extra logic to
  // allow for switch espcially if ownship is aft of the contact.

  //=====================================================================
  // Part 2: RELEASE conditions: if already in giveway mode, check
  // mode-exit criteria. Release/entry conditions are not same, to
  // prevent mode thrashing.
  // =====================================================================
  if(m_avoid_mode == "giveway") {
    
    // transition to cpa mode if past cpa and opening
    if(m_contact_range > m_min_util_cpa_dist) {
      if(m_rate_of_closure < 0) 
        resetAvoidModes("cpa");
    }
    // transition to cpa mode if ownship now on contact's starboard side
    if(m_os_starboard_of_cn)
      resetAvoidModes("cpa");
    return;
  }

  //=====================================================================  
  // Part 3: ENTRY conditions checked it not already in GiveWay mode
  //=====================================================================  

  // Check contact angle, (alpha<247.5). Check rel bearing, (beta > 112.5)
  if((m_cn_os_rel_bng < 247.5) || (m_os_cn_rel_bng > 112.5)) {
    resetAvoidModes();
    return;
  }
  // Entry criteria passed - set the mode to "giveway"
  m_avoid_mode = "giveway";
  
  //=====================================================================  
  // Part 4: SUBMODE determination is made
  //=====================================================================  
  // By default we will try to cross to the stern
  m_avoid_submode = "stern";

  // But if our present course has us crossing the bow, and crossing by
  // a healthy distance, then the giveway behavior will try to achieve
  // its goal by crossing the bow, setting the avoid_submode to "bow".
  string mvar1 = "XCN_BOW_DIST_" + toupper(m_contact);
  postMessage(mvar1, m_os_crosses_cn_bow_dist);

  if(m_os_crosses_cn_bow) {
    // Criteria A: os crossing cn by by a healthy amount
    double acceptable_dist  = (2*m_max_util_cpa_dist + m_min_util_cpa_dist) / 3; 
    if(m_os_crosses_cn_bow_dist > acceptable_dist)
      m_avoid_submode = "bow";
    // Criteria B: os cannot cut behind cn even if it wanted to.
    double turn_gap = 0;

    double cnx2, cny2;
    projectPoint(m_cnh, 100, m_cnx, m_cny, cnx2, cny2);
    turn_gap = turnGap(m_osx, m_osy, m_osh, m_turn_radius,
		       m_cnx, m_cny, cnx2, cny2, true);
    if(turn_gap < acceptable_dist)
      m_avoid_submode = "bow";

    string mvar2 = "XCN_BOW_TURN_GAP_" + toupper(m_contact);
    postMessage(mvar2, turn_gap);
  }

  
  
  // We will also set the mode to cross by bow if ownship is presently 
  // not on a trajectory to cross the bow or the stern. If so, set to
  // cross by bow if the speed in contact heading is greater than zero.
  // That is, it if is travelling alongside the contact but opening range
  // and travelling the same general direction.
  double os_spd_in_cn_hdg = speedInHeading(m_osh, m_osv, m_cnh);
  
  if(!m_os_crosses_cn_bow_or_stern && (os_spd_in_cn_hdg > 0))
    m_avoid_submode = "bow";

}

//--------------------------------------------------------------
// Procedure: buildGiveWayIPF                          (Rule 16)

IvPFunction* BHV_AvdColregsV17::buildGiveWayIPF()
{
  // If ownship is already within the min_util_cpa_dist then we shrink
  // this distance, othewise the resulting ivp function will be universally
  // negative, resulting a function with no preference in any direction.
  double min_util_cpa_dist = m_min_util_cpa_dist;
  if(m_contact_range <= m_min_util_cpa_dist)
    min_util_cpa_dist = (m_contact_range / 2);

  m_debug2 = "Building Giveway IPF";

  AOF_R16  aof(m_domain);

  aof.setOwnshipParams(m_osx, m_osy);
  aof.setContactParams(m_cnx, m_cny, m_cnh, m_cnv);
  bool ok = true;
  ok = ok && aof.setParam("tol", 120);
  ok = ok && aof.setParam("osh", m_osh);
  ok = ok && aof.setParam("passing_side", m_avoid_submode);  
  ok = ok && aof.setParam("collision_distance", min_util_cpa_dist);
  ok = ok && aof.setParam("all_clear_distance", m_max_util_cpa_dist);
  bool init_ok = ok && aof.initialize();

  m_debug3 = boolToString(init_ok);
  
  if(!init_ok) {
    m_debug1 = "PROBLEM Init AOF_R16!!!!";
    string aof_msg = aof.getCatMsgsAOF();
    postEMessage("Unable to init AOF_R16:"+aof_msg);
    return(0);
  }
  m_debug4 = "GiveWay AOF initialized OK";

  OF_Reflector reflector(&aof, 1);
  reflector.create(m_build_info);
  IvPFunction *ipf = reflector.extractOF();

  m_debug3 = "";
  unsigned int msize = reflector.getMessageCnt();
  for(unsigned int i=0; i<msize; i++) {
    string msg = reflector.getMessage(i);
    postMessage("AOF_R16", msg);
    m_debug3 += " # " + msg;
  }

  return(ipf);
}


//-----------------------------------------------------------
// Procedure: checkModeStandOn                      (Rule 17)

void BHV_AvdColregsV17::checkModeStandOn()
{
  //=====================================================================  
  // Part 1: Sanity checks, and absolute release check based on range
  //=====================================================================  
  // Check if existing mode is either "cpa", "none", or "standon"
  if((m_avoid_mode!="standon") && (m_avoid_mode!="none") && (m_avoid_mode != "cpa"))
    return;
  // Check if contact beyond outer priority weight distance
  if(m_contact_range >= m_pwt_outer_dist) {
    resetAvoidModes();
    return;
  }

  //=====================================================================  
  // Part 2: RELEASE conditions: if already in standon mode, check mode-exit 
  // criteria. Release/entry conditions are not same, to prevent mode thrashing.
  //=====================================================================  
  if(m_avoid_mode == "standon") {
    if(m_cn_crossed_os_port_star) {
    //if(!m_cn_port_of_os) {
      resetAvoidModes();
      return;
    }
    if((m_contact_range > m_min_util_cpa_dist) && (m_rate_of_closure < 0)) {
      resetAvoidModes();
      return;
    }
  }
    
  //=====================================================================  
  // Part 3: ENTRY conditions checked it not already in StandOn mode
  //=====================================================================  

  // Part 3A: First criteria is based on relative bearing and contact angle
  if((m_os_cn_rel_bng < 247.5) || (m_cn_os_rel_bng > 112.5))
    return;
  
  // Part 2B: If meet relative bearing criteria but not crossing, reject.
  if(!m_cn_crosses_os_bow_or_stern)
    return;
  
  // Part 2C: If rate of closure is less than zero, ie "opening", return;
  if(m_rate_of_closure < 0)
    return;
  
  // Passed the four criteria, set the mode and submode
  m_avoid_mode = "standon";
  m_avoid_submode = "unsure";

  
  //=====================================================================  
  // Part 4: SUBMODE determination is made
  //=====================================================================  
#if 1  
  // kw added for rule 17.a.ii to allow in-extremis actions
  // once in extremis, retain authority to maneuver as necessary
  if(m_avoid_submode == "inextremis") 
    return;

  if((m_avoid_mode == "standon") && (m_contact_range <= m_max_util_cpa_dist) &&
     (m_os_curr_cpa_dist <= m_min_util_cpa_dist)){
    // assert 17.a.ii actions if:
    //    1) a stand on vessel
    //    2) range closer than pwt_inner (already taking max action)
    //    3) expected cpa distance less than minimum acceptable cpa dist
    m_avoid_submode = "inextremis";
    return;
  }
#endif

  // Part 3A: The "neither" submode is terminal. Can never go to any
  // other submode, and any other submode can transition to "neither".
  // It indicates ownship has crossed to the port side of contact.
  if((m_avoid_mode == "standon") && m_os_port_of_cn) 
    m_avoid_submode = "neither";

  if((m_avoid_mode == "standon") && (m_avoid_submode == "neither"))
    return;

  // Part 3B: The bow and stern modes cannot transition to anything else
  // but "neither" and we already checked for this.
  if((m_avoid_mode == "standon") && (m_avoid_submode == "bow"))
    return;
  if((m_avoid_mode == "standon") && (m_avoid_submode == "stern"))
    return;

  // Part 3C: If in any of the "unsure" submodes, we try to transition to
  // either the bow or stern modes, or a less unsure mode if we're still in 
  // the unsure mode.

  // First an easy one: if contact has crossed ownships bow, submode is bow.
  // Disabled by mikerb May 20, 2016
  //if(!m_cn_port_of_os) {
  //  m_avoid_submode = "bow";
  //  return;
  //}

  // Here's a summary of the evidence we use:
  //   1. Where CN crosses OS bow-stern line and the crossing distance
  //   2. os relative bearing - closer to 360, more likely crossing bow
  //   3. bearing rate - increasing means more likely crossing bow
  //   4. contact turning rate - positive means turning toward ownship

  // Get turn rate
  double cn_turn_rate = 0;
  bool   cn_turn_rate_ok = getHeadingRate(cn_turn_rate);

  // declare a useful range value
  double half_util_cpa_dist = (m_max_util_cpa_dist + m_min_util_cpa_dist) / 2;

  // Bearing Range #1 --------------------------------
  if(m_os_cn_rel_bng > 350) {
    if(m_cn_crosses_os_bow_dist >= m_min_util_cpa_dist) {
      if(cn_turn_rate_ok && (cn_turn_rate < 0))
	m_avoid_submode = "bow";
      else
	m_avoid_submode = "unsure_bow";
    }
    else if(m_cn_crosses_os_bow_dist > 0) {
      if(cn_turn_rate_ok && (cn_turn_rate < 0))
	m_avoid_submode = "unsure_bow";
      else
	m_avoid_submode = "unsure";
    }
    else { // cn_crosses_os_stern_dist > 0
      m_avoid_submode = "unsure_stern";
    }
  }
  // Bearing Range #2 --------------------------------
  else if(m_os_cn_rel_bng > 315) {
    if(m_cn_crosses_os_bow_dist >= half_util_cpa_dist) {
      if(cn_turn_rate_ok && (cn_turn_rate < 3))
	m_avoid_submode = "bow";
      else
	m_avoid_submode = "unsure_bow";
    }
    else if(m_cn_crosses_os_bow_dist >= m_min_util_cpa_dist) {
      if(cn_turn_rate_ok && (cn_turn_rate < 0.1))
	m_avoid_submode = "bow";
      else
	m_avoid_submode = "unsure_bow";
    }
    else if(m_cn_crosses_os_bow_dist > 0) {
      if(cn_turn_rate_ok && (cn_turn_rate < 0))
	m_avoid_submode = "unsure_bow";
      else
	m_avoid_submode = "unsure";
    }
    else { // cn_crosses_os_stern_dist > 0
      m_avoid_submode = "stern";
    }
  }
  // Bearing Range #3 --------------------------------
  else if(m_os_cn_rel_bng >= 270) {
    if(m_cn_crosses_os_bow_dist >= m_max_util_cpa_dist) 
      m_avoid_submode = "bow";
    else if(m_cn_crosses_os_bow_dist >= half_util_cpa_dist) {
      if(cn_turn_rate_ok && (cn_turn_rate < 0.1))
	m_avoid_submode = "bow";
      else
	m_avoid_submode = "unsure_bow";
    }
    else if(m_cn_crosses_os_bow_dist > 0) {
      if(cn_turn_rate_ok && (cn_turn_rate < 0))
	m_avoid_submode = "unsure_bow";
      else
	m_avoid_submode = "unsure";
    }
    else { // cn_crosses_os_stern_dist > 0
      m_avoid_submode = "stern";
    }
  }
  // Bearing Range #4 --------------------------------
  else { //   180 < os_cn_rel_bng < 270
    if(m_cn_crosses_os_bow_dist >= half_util_cpa_dist) {
      if(cn_turn_rate_ok && (cn_turn_rate < 0.1))
	m_avoid_submode = "unsure_bow";
      else
	m_avoid_submode = "unsure";
    }
    else if(m_cn_crosses_os_bow_dist > 0) 
      m_avoid_submode = "unsure";
    else 
      m_avoid_submode = "stern";
  }
}
      

//---------------------------------------------------------------------
// Procedure: checkModeStandOnOT            (Rule 17, Overtaken Vessel)

void BHV_AvdColregsV17::checkModeStandOnOT()
{
  //=====================================================================  
  // Part 1: Sanity checks, and absolute release check based on range
  //=====================================================================  
  // Check if existing mode is either "cpa", "none", or "standon"
  if((m_avoid_mode!="standon_ot") && (m_avoid_mode!="none") && (m_avoid_mode != "cpa"))
    return;
  // Check if contact beyond outer priority weight distance
  if(m_contact_range >= m_pwt_outer_dist) {
    resetAvoidModes();
    return;
  }

  //=====================================================================  
  // Part 2: RELEASE conditions: if already in standon_ot mode, check mode-exit 
  // criteria. Release/entry conditions are not same, to prevent mode thrashing.
  //=====================================================================  
  if(m_avoid_mode == "standon_ot") {
    // If cn crosses os bow-stern line *in either direction*, then release.
    if(m_cn_crossed_os_port_star) {  
      resetAvoidModes();
      return;
    }
    if(m_rate_of_closure < 0) {
      resetAvoidModes();
      return;
    }
  }
    
  //=====================================================================  
  // Part 3: ENTRY conditions checked it not already in StandOnOT mode
  //=====================================================================  

  // kw added: we must check for rule 17 applicability to overtaken
  // vessel (Rule 13). Logic modified by mikerb to fix unwanted mode
  // entries when contact was not actually on a trajectory to overtake.
  
  // Rule 13: (a) Notwithstanding anything contained in Rules 4-18, any vessel
  // overtaking any other shall keep out of way of vessel being overtaken.    
  // Rule 17: (a) (i) Where one of two vessels is to keep out of the
  // way, the other shall keep her course and speed.
  
  if(m_avoid_mode != "standon_ot") {
    // Check if contact is within bearings for standon mode (due to overtaking)
    if((m_os_cn_rel_bng < 112.5) || (m_os_cn_rel_bng > 247.5)) {
      resetAvoidModes();
      return;
    }
    // Check if contact is on trajectory to pass ownship
    if(!m_cn_passes_os) {
      resetAvoidModes();
      return;
    }
    // Check if cpa is of concern
    if(m_os_curr_cpa_dist > m_max_util_cpa_dist) {
      resetAvoidModes();
      return;
    }
  }

  // Entry criteria passed - set the mode to "standon_ot"
  m_avoid_mode = "standon_ot";
  m_avoid_submode = "unsure";
  
  //=====================================================================  
  // Part 3: Examine our change submode criteria. 
  //=====================================================================  
  
  // kw added for rule 17.a.ii to allow in-extremis actions
  // once in extremis, retain authority to maneuver as necessary
  if(m_avoid_submode == "inextremis") 
    return;

  if((m_avoid_mode == "standon_ot") && (m_contact_range <= m_max_util_cpa_dist) &&
     (m_os_curr_cpa_dist <= m_min_util_cpa_dist)){
    // assert 17.a.ii actions if:
    //    1) a stand on vessel
    //    2) range closer than pwt_inner (already taking max action)
    //    3) expected cpa distance less than minimum acceptable cpa dist
    m_avoid_submode = "inextremis";
    return;
  }

  if(m_cn_port_of_os)
    m_avoid_submode = "port";
  else
    m_avoid_submode = "starboard";
}  

//--------------------------------------------------------------
// Procedure: checkCPA                          (Catch-all)

void BHV_AvdColregsV17::checkModeCPA()
{
  // Part 0: Sanity check. Existing mode must be either "none", or "cpa"
  if((m_avoid_mode != "cpa") && (m_avoid_mode != "none"))
    return;

  if(m_contact_range > m_pwt_outer_dist) {
    resetAvoidModes();
    return;
  }

  m_avoid_mode    = "cpa";
  m_avoid_submode = "none";
}

//-----------------------------------------------------------
// Procedure: buildStandOnIPF                    (Rule 17)

IvPFunction* BHV_AvdColregsV17::buildStandOnIPF()
{
  // If ownship is already within the min_util_cpa_dist then we shrink
  // this distance, otherwise the resulting ivp function will be
  // universally negative, resulting a function with no preference in
  // any direction.

  m_debug3 = "Bldg StandOn V17 IPF";

  if(m_avoid_submode == "inextremis")
    return(buildCPA_IPF());

  ZAIC_SPD spd_zaic(m_domain, "speed");
  spd_zaic.setMedSpeed(m_spd_orig_standon);
  spd_zaic.setLowSpeed(0.1);
  spd_zaic.setHghSpeed(m_spd_orig_standon + 0.4);
  spd_zaic.setLowSpeedUtil(50);
  spd_zaic.setHghSpeedUtil(50);

  IvPFunction *spd_ipf = spd_zaic.extractIvPFunction();
  if(!spd_ipf)
    postWMessage("Failure on the SPD ZAIC");
  
  ZAIC_HDG crs_zaic(m_domain, "course");
  crs_zaic.setSummit(m_osh_orig_standon);
  crs_zaic.setLowDelta(20);
  crs_zaic.setHighDelta(20);
  crs_zaic.setLowDeltaUtil(25);
  crs_zaic.setHighDeltaUtil(25);
  IvPFunction *crs_ipf = crs_zaic.extractIvPFunction();
  if(!crs_ipf) 
    postWMessage("Failure on the CRS ZAIC");
  
  OF_Coupler coupler;
  IvPFunction *ipf = coupler.couple(crs_ipf, spd_ipf, 0.5, 0.5);
  if(!ipf)
    postWMessage("Failure on the CRS_SPD COUPLER");
  
  return(ipf);
}


//-----------------------------------------------------------
// Procedure: buildCPA_IPF                    (Catch-all)

IvPFunction* BHV_AvdColregsV17::buildCPA_IPF()
{
  // If ownship is already within the min_util_cpa_dist then we shrink
  // this distance, othewise the resulting ivp function will be universally
  // negative, resulting a function with no preference in any direction.
  double min_util_cpa_dist = m_min_util_cpa_dist;
  if(m_contact_range <= m_min_util_cpa_dist)
    min_util_cpa_dist = (m_contact_range / 2);

  m_debug2 = "Building CPA IPF";

  AOF_CPA aof(m_domain);
  aof.setOwnshipParams(m_osx, m_osy);
  aof.setContactParams(m_cnx, m_cny, m_cnh, m_cnv);
  aof.setParam("tol", 120);
  aof.setParam("osh", m_osh);
  aof.setParam("osv", m_osv);
  aof.setParam("collision_distance", min_util_cpa_dist);
  aof.setParam("all_clear_distance", m_max_util_cpa_dist);
  bool init_ok = aof.initialize();

  m_debug3 = boolToString(init_ok);
  
  if(!init_ok) {
    m_debug1 = "PROBLEM Init AOF_AvoidCollision!!!!";
    postEMessage("Unable to init AOF_AvoidCollision.");
    return(0);
  }
  m_debug4 = "Basic CPA AvoidCollision AOF initialized OK";

  OF_Reflector reflector(&aof, 1);
  reflector.create(m_build_info);
  IvPFunction *ipf = reflector.extractOF();

  return(ipf);
}


//-----------------------------------------------------------
// Procedure: getRelevance
//            Calculate the relevance first. If zero-relevance, 
//            we won't bother to create the objective function.

double BHV_AvdColregsV17::getRelevance() const
{
  // First declare the ange of relevance values to be calc'ed
  double min_dist_relevance = 0.0;
  double max_dist_relevance = 1.0;
  double rng_dist_relevance = max_dist_relevance - min_dist_relevance;
  
  double contact_range = hypot((m_osx - m_cnx),(m_osy - m_cny));

  double pwt_outer_dist = m_pwt_outer_dist;
  double pwt_inner_dist = m_pwt_inner_dist;
  if(m_avoid_mode == "headon") {
    pwt_outer_dist = m_pwt_outer_dist;
    pwt_inner_dist = m_pwt_inner_dist;
    //pwt_outer_dist = 2* m_pwt_outer_dist;
    //pwt_inner_dist = 2* m_pwt_inner_dist;
  }
  else if(m_avoid_mode == "giveway") {  
    pwt_outer_dist = m_pwt_outer_dist;
    pwt_inner_dist = m_pwt_inner_dist;
    //pwt_outer_dist = 1.5 * m_pwt_outer_dist;
    //pwt_inner_dist = 1.5 * m_pwt_inner_dist;
  }

  if(contact_range >= pwt_outer_dist)
    return(min_dist_relevance);

  if(contact_range <= pwt_inner_dist)
    return(max_dist_relevance);
  
  double dpct, drange;
  // Note: drange should never be zero since either of the above
  // conditionals would be true and the function would have returned.
  drange = (pwt_outer_dist - pwt_inner_dist);
  dpct   = (pwt_outer_dist - m_contact_range) / drange;
  dpct   = vclip(dpct, 0, 1);

  // Apply the grade scale to the raw distance
  double mod_dpct = dpct; // linear case
  if(m_pwt_grade == "quadratic")
    mod_dpct = dpct * dpct;
  else if(m_pwt_grade == "quasi")
    mod_dpct = pow(dpct, 1.5);

  mod_dpct = vclip(mod_dpct, 0, 1);
  
  double d_relevance = (mod_dpct * rng_dist_relevance) + min_dist_relevance;
  
  return(d_relevance);  
}
 


//-----------------------------------------------------------
// Procedure: resetAvoidModes

bool BHV_AvdColregsV17::resetAvoidModes(string mode, string submode)
{
  bool valid = true;
  if((mode != "headon") && (mode != "giveway") && (mode != "none") &&
     (mode != "overtaking") && (mode != "standon") && (mode != "cpa"))
    valid = false;

  if((mode == "giveway")  && (submode != "stern") && (submode != "bow") && 
     (submode != "neither") && (submode != "none"))
    valid = false;
  
  if((mode == "overtaking") && (submode != "port") && 
     (submode != "starboard") && (submode != "none"))
    valid = false;
  
  if((mode == "standon") && (submode != "bow") && (submode != "stern") && 
     (submode != "unsure") && (submode != "neither") && (submode != "none")
     && (submode != "inextremis"))
    valid = false;
  
  if(!valid)
    return(false);

  m_avoid_mode    = mode;
  m_avoid_submode = submode;

  return(true);
}


//-----------------------------------------------------------
// Procedure: findSlowdownSpeedRange
//   Purpose: Find two numbers, [VL, VH]. 
//            All calculations are based on a present contact position,
//            heading and speed, and present ownship position, heading
//            and speed.
//            Both VL and VH must be <= present ownship speed.
//            The first number, VL, is the speed, above which, the CPA
//            is within the max_util_cpa_dist. 
//            The second number is the speed, above which, the CPA is 
//            within the min_util_cpa_dist (~ virtual collision).
//      Note: The context is an ownship with a contact on a trajectory to
//            cross its bow. Slowing down should only help widen the CPA.


bool BHV_AvdColregsV17::findDecelerateSpeedRange(double& minv, double& maxv)
{
  // Part 0: initialize the results
  minv = 0;
  maxv = 0;

  // Part 1: Domain Sanity Check 
  if(!m_domain.hasDomain("speed"))
    return(false);

  // Part 2: utility santity range check
  if(m_min_util_cpa_dist > m_max_util_cpa_dist)
    return(false);

  // Part 3: Find the range
  double spd_domain_delta = m_domain.getVarDelta("speed");
  for(double spd=0; spd <= m_osv; spd+=spd_domain_delta) {
    if(m_os_curr_cpa_dist >= m_max_util_cpa_dist)
      minv = m_os_curr_cpa_dist;
    if(m_os_curr_cpa_dist >= m_min_util_cpa_dist)
      maxv = m_os_curr_cpa_dist;
  }

  if(minv > m_osv)
    minv = m_osv;
  if(maxv > m_osv)
    maxv = m_osv;

  return(true);
}

//-----------------------------------------------------------
// Procedure: findAccelerateSpeedRange
//   Purpose: Find two numbers, [VL, VH]. 
//            All calculations are based on a present contact position,
//            heading and speed, and present ownship position, heading
//            and speed.
//            Both VL and VH must be >= present ownship speed.
//            The second number, VH, is the speed, below which, the CPA 
//            is within the max_util_cpa_dist.
//            The first number, VL, is the speed, below which, the CPA
//            is within the min_util_cpa_dist (~ virtual collision).
//      Note: The context is an ownship with a contact on a trajectory to
//            cross ownship's stern. Slowing down should only help widen the CPA.

bool BHV_AvdColregsV17::findAccelerateSpeedRange(double& minv, double& maxv)
{
  // Part 0: initialize the results
  minv = 0;
  maxv = 0;

  // Part 1: Domain Sanity Check 
  if(!m_domain.hasDomain("speed"))
    return(false);

  // Part 2: utility santity range check
  if(m_min_util_cpa_dist > m_max_util_cpa_dist)
    return(false);

  // Part 3: Find the range
  double spd_delta = m_domain.getVarDelta("speed");
  double max_spd   = m_domain.getVarHigh("speed");
  for(double spd=max_spd; spd >= m_osv; spd-=spd_delta) {
    if(m_os_curr_cpa_dist <= m_max_util_cpa_dist)
      maxv = m_os_curr_cpa_dist;
    if(m_os_curr_cpa_dist <= m_min_util_cpa_dist)
      minv = m_os_curr_cpa_dist;
  }
  
  if(minv > m_osv)
    minv = m_osv;
  if(maxv > m_osv)
    maxv = m_osv;

  return(true);
}


//-----------------------------------------------------------
// Procedure: addHeading()
//   Purpose: Keep track of the contact heading history. Used in the 
//            standon mode where we try to make an intelligent guess 
//            as to whether the contact will giveway ahead or behind
//            the standon vehicle.

void BHV_AvdColregsV17::addHeading(double heading, double currtime)
{
  m_cn_heading_val.push_back(heading);
  m_cn_heading_time.push_back(currtime);
  
  bool done = false;
  while(!done) {
    if(m_cn_heading_time.size() == 0)
      done = true;
    else {
      double front_time = m_cn_heading_time.front();
      if((currtime-front_time) < m_memory_time)
	done = true;
      else {
	m_cn_heading_val.pop_front();
	m_cn_heading_time.pop_front();
      }
    }
  }
}

//-----------------------------------------------------------
// Procedure: addRelBng()
//   Purpose: Keep track of the ownship-contact relative bearing history.
//            Used in the standon mode where we try to make an intelligent 
//            guess as to whether the contact will giveway ahead or behind
//            the standon vehicle.

void BHV_AvdColregsV17::addRelBng(double rel_bng, double currtime)
{
  m_cn_rel_bng_val.push_back(rel_bng);
  m_cn_rel_bng_time.push_back(currtime);
  
  bool done = false;
  while(!done) {
    if(m_cn_rel_bng_time.size() == 0)
      done = true;
    else {
      double front_time = m_cn_rel_bng_time.front();
      if((currtime-front_time) < m_memory_time)
	done = true;
      else {
	m_cn_rel_bng_val.pop_front();
	m_cn_rel_bng_time.pop_front();
      }
    }
  }
}

//-----------------------------------------------------------
// Procedure: clearHeadings()
//   Purpose: Clear the contact heading history.

void BHV_AvdColregsV17::clearHeadings()
{
  m_cn_heading_val.clear();
  m_cn_heading_time.clear();
}

//-----------------------------------------------------------
// Procedure: clearRelBngs
//   Purpose: Clear the contact relative bearing history.

void BHV_AvdColregsV17::clearRelBngs()
{
  m_cn_rel_bng_val.clear();
  m_cn_rel_bng_time.clear();
}

//-----------------------------------------------------------
// Procedure: getHeadingRate()
//   Purpose: Determine which way the contact is turning, if it is.

bool BHV_AvdColregsV17::getHeadingRate(double& result, double min_secs)
{
  result = 0;
  if(m_cn_heading_val.size() <= 1)
    return(false);

  double total_time = m_cn_heading_time.back() - m_cn_heading_time.front();
  double total_turn = 0;

  if(total_time < min_secs)
    return(false);
  
  double prev_heading = 0;
  list<double>::iterator p;
  for(p=m_cn_heading_val.begin(); p!=m_cn_heading_val.end(); p++) {
    double this_heading = *p;
    if(p != m_cn_heading_val.begin()) {
      double delta = this_heading - prev_heading;
      if(delta < -180)
	delta += 360;
      else if(delta > 180)
	delta -= 360;
      total_turn += delta;
    }
    prev_heading = this_heading;
  }
  
  result = total_turn / total_time;

  return(true);
}

//-----------------------------------------------------------
// Procedure: getRelBngRate()
//   Purpose: Determine the relative bearing rate

bool BHV_AvdColregsV17::getRelBngRate(double& result)
{
  result = 0;
  if(m_cn_rel_bng_val.size() <= 1)
    return(false);

  double total_time = m_cn_rel_bng_time.back() - m_cn_rel_bng_time.front();
  double total_bng_delta = 0;
  
  double prev_rel_bng = 0;
  list<double>::iterator p;
  for(p=m_cn_rel_bng_val.begin(); p!=m_cn_rel_bng_val.end(); p++) {
    double this_rel_bng = *p;
    if(p != m_cn_rel_bng_val.begin()) {
      double delta = this_rel_bng - prev_rel_bng;
      if(delta < -180)
	delta += 360;
      else if(delta > 180)
	delta -= 360;
      total_bng_delta += delta;
    }
    prev_rel_bng = this_rel_bng;
  }
  
  result = total_bng_delta / total_time;

  return(true);
}

//-----------------------------------------------------------
// Procedure: postStatusInfo

void BHV_AvdColregsV17::postStatusInfo()
{
  string suffix = "_" + toupper(m_contact);
  postMessage("AVDCOL_RANGE" + suffix, m_contact_range);

  string full_mode = m_avoid_mode;
  if(m_avoid_submode != "none")
    full_mode += ":" + m_avoid_submode;
  
  postMessage("COLREGS_AVOID_MODE" + suffix, full_mode);

  if((m_avoid_mode != "none") && (m_avoid_mode != "complete")) {
    m_bearing_segl.clear();
    m_bearing_segl.set_label(m_us_name + m_descriptor);
    m_bearing_segl.add_vertex(m_osx, m_osy);
    m_bearing_segl.add_vertex(m_cnx, m_cny);
    m_bearing_segl.set_active(true);
    m_bearing_segl.set_msg(full_mode);
  }
  else
    m_bearing_segl.set_active(false);

  postMessage("VIEW_SEGLIST", m_bearing_segl.get_spec());


  unsigned int avoid_mode_ix = 0;
  if(m_avoid_mode == "complete")
    avoid_mode_ix = 3;
  if(m_avoid_mode == "headon")
    avoid_mode_ix = 10;
  else if(m_avoid_mode == "giveway") {
    if(m_avoid_submode == "stern")
      avoid_mode_ix = 20;
    else if(m_avoid_submode == "bow")
      avoid_mode_ix = 22;
    else if(m_avoid_submode == "bow_must")
      avoid_mode_ix = 25;
  }
  else if(m_avoid_mode == "standon") {
    if(m_avoid_submode == "stern")
      avoid_mode_ix = 30;
    if(m_avoid_submode == "inextremis")
      avoid_mode_ix = 31;
    else if(m_avoid_submode == "bow")
      avoid_mode_ix = 32;
    else if(m_avoid_submode == "unsure_stern")
      avoid_mode_ix = 34;
    else if(m_avoid_submode == "unsure_bow")
      avoid_mode_ix = 36;
    else if(m_avoid_submode == "unsure")
      avoid_mode_ix = 38;
    else if(m_avoid_submode == "neither")
      avoid_mode_ix = 39;
  }
  else if(m_avoid_mode == "overtaking") {
    if(m_avoid_submode == "port")
      avoid_mode_ix = 43;
    else
      avoid_mode_ix = 47;
  }
  else if(m_avoid_mode == "cpa") {
    avoid_mode_ix = 50;
  }

  string summary = "mode=" + m_avoid_mode + ":" + m_avoid_submode;

  postMessage("COLREGS_SUMMARY" + suffix, summary);

  postRepeatableMessage("COLREGS_AVOID_IX"  + suffix, avoid_mode_ix);
  postRepeatableMessage("COLREGS_AVOID_PWT" + suffix, m_actual_pwt);

  postMessage("COLREGS_CONTACT_HDG" + suffix, m_cnh);

  return; // shut off the rest fornow
#if 0
  double heading_rate  = 0;
  bool   heading_history_valid = getHeadingRate(heading_rate);
  if(heading_history_valid)
    postRepeatableMessage("COLREGS_TURN_RATE" + suffix, heading_rate);

  double bearing_rate  = 0;
  bool   bearing_history_valid = getRelBngRate(bearing_rate);
  if(bearing_history_valid)
    postRepeatableMessage("COLREGS_BNG_RATE" + suffix, bearing_rate);

  if(m_cn_rel_bng_val.size() > 0) 
    postRepeatableMessage("COLREGS_BNG" + suffix, m_cn_rel_bng_val.back());
#endif
}




