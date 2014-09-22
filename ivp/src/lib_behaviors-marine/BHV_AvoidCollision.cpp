/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_AvoidCollision.cpp                               */
/*    DATE: Nov 18th 2006                                        */
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

#include <iostream>
#include <cmath>
#include <cstdlib>
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "AOF_AvoidCollision.h"
#include "AOF_AvoidCollisionDepth.h"
#include "BHV_AvoidCollision.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "MBUtils.h"
#include "CPA_Utils.h"
#include "XYSegList.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_AvoidCollision::BHV_AvoidCollision(IvPDomain gdomain) : 
  IvPContactBehavior(gdomain)
{
  this->setParam("descriptor", "avoid_collision");
  this->setParam("build_info", "uniform_piece = discrete @ course:3,speed:3");
  this->setParam("build_info", "uniform_grid  = discrete @ course:9,speed:6");
  
  if(m_domain.hasDomain("depth"))
    m_domain = subDomain(m_domain, "course,speed,depth");
  else
    m_domain = subDomain(m_domain, "course,speed");
  
  m_collision_depth   = 0;

  m_completed_dist    = 500;
  m_pwt_outer_dist    = 200;
  m_pwt_inner_dist    = 50;
  m_min_util_cpa_dist = 10; 
  m_max_util_cpa_dist = 75; 
  m_pwt_grade         = "quasi";
  m_roc_max_dampen    = -2.0; 
  m_roc_max_heighten  = 2.0; 
  m_bearing_line_show = false;
  m_time_on_leg       = 120;  // Overriding the superclass default=60

  m_no_alert_request  = false;

  // Initialize state variables
  m_curr_closing_spd = 0;
  m_avoiding = false;
  
  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING, AVOIDING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_AvoidCollision::setParam(string param, string param_val) 
{
  if(IvPContactBehavior::setParam(param, param_val))
    return(true);

  double dval = atof(param_val.c_str());
  bool non_neg_number = (isNumber(param_val) && (dval >= 0));

  if((param == "pwt_outer_dist") ||          // preferred
     (param == "active_distance") ||         // deprecated 4/10
     (param == "active_outer_distance")) {   // deprecated 4/10
    if(!non_neg_number)
      return(false);
    m_pwt_outer_dist = dval;
    if(m_pwt_inner_dist > m_pwt_outer_dist)
      m_pwt_inner_dist = m_pwt_outer_dist;
    return(true);
  }  
  else if((param == "pwt_inner_dist") ||          // preferred
	  (param == "active_inner_distance")) {   // deprecated 4/10
    if(!non_neg_number)
      return(false);
    m_pwt_inner_dist = dval;
    if(m_pwt_outer_dist < m_pwt_inner_dist)
      m_pwt_outer_dist = m_pwt_inner_dist;
    return(true);
  }  
  else if((param == "completed_dist") ||         // preferred
	  (param == "completed_distance")) {     // deprecated 4/10
    if(dval <= 0)
      return(false);
    m_completed_dist = dval;
    return(true);
  }  
  else if(param == "collision_depth") {
    if(dval <= 0)
      return(false);
    if(!m_domain.hasDomain("depth"))
      return(false);
    if(dval >= m_domain.getVarHigh("depth"))
      return(true);  // Should not be considered a config error
    m_collision_depth = dval;
    return(true);
  }  
  else if((param == "max_util_cpa_dist") ||      // preferred
	  (param == "all_clear_distance")) {     // deprecated 4/10
    if(!non_neg_number)
      return(false);
    m_max_util_cpa_dist = dval;
    return(true);
  }  
  else if((param == "min_util_cpa_dist")  ||     // preferred
	  (param == "collision_distance")) {     // deprecated 4/10
    if(!non_neg_number)
      return(false);
    m_min_util_cpa_dist = dval;
    return(true);
  }  

  else if((param == "pwt_grade") ||              // preferred
	  (param == "active_grade")) {           // deprecated 4/10
    param_val = tolower(param_val);
    if((param_val!="linear") && (param_val!="quadratic") && 
       (param_val!="quasi"))
      return(false);
    m_pwt_grade = param_val;
    return(true);
  }  
  else if(param == "roc_max_heighten") {
    if(!non_neg_number)
      return(false);
    m_roc_max_heighten = dval;
    if(m_roc_max_dampen > m_roc_max_heighten)
      m_roc_max_dampen = m_roc_max_heighten;
    return(true);
  }  
  else if(param == "roc_max_dampen") {
    if(!non_neg_number)
      return(false);
    m_roc_max_dampen = dval;
    if(m_roc_max_heighten < m_roc_max_heighten)
      m_roc_max_heighten = m_roc_max_dampen;
    return(true);
  }  
  else if(param == "no_alert_request") {
    return(setBooleanOnString(m_no_alert_request, param_val));
  }  
  return(false);
}


//-----------------------------------------------------------
// Procedure: onHelmStart()

void BHV_AvoidCollision::onHelmStart()
{
  if(m_no_alert_request || (m_update_var == ""))
    return;

  string s_alert_range = doubleToStringX(m_pwt_outer_dist,1);
  string s_cpa_range   = doubleToStringX(m_completed_dist,1);
  string s_alert_templ = "name=avd_$[VNAME] # contact=$[VNAME]";

  string alert_request = "id=avd, var=" + m_update_var;
  alert_request += ", val=" + s_alert_templ;
  alert_request += ", alert_range=" + s_alert_range;
  alert_request += ", cpa_range=" + s_cpa_range;

  postMessage("BCM_ALERT_REQUEST", alert_request);
}

//-----------------------------------------------------------
// Procedure: onRunToIdleState()

void BHV_AvoidCollision::onRunToIdleState() 
{
  postErasableBearingLine();
}

//-----------------------------------------------------------
// Procedure: onIdleState()

void BHV_AvoidCollision::onIdleState() 
{
  bool ok = updatePlatformInfo();
  if(!ok)
    postRange(false);
  else
    postRange(true);
}

//-----------------------------------------------------------
// Procedure: onCompleteState()

void BHV_AvoidCollision::onCompleteState() 
{
  postErasableBearingLine();
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_AvoidCollision::onRunState() 
{
  if(!updatePlatformInfo()) {
    postRange(false);
    return(0);
  }

  m_relevance = getRelevance();
  postRange(true);

  if(m_contact_range >= m_completed_dist) {
    setComplete();
    return(0);
  }
  
  if(m_relevance <= 0) {
    postViewableBearingLine();
    return(0);
  }

  double min_util_cpa_dist = m_min_util_cpa_dist;
  if(m_contact_range <= m_min_util_cpa_dist)
    min_util_cpa_dist = (m_contact_range / 2);
    
  bool ok = true;
  IvPFunction *ipf = 0;
  if(m_collision_depth == 0) {
    AOF_AvoidCollision aof(m_domain);
    aof.setOwnshipParams(m_osx, m_osy);
    aof.setContactParams(m_cnx, m_cny, m_cnh, m_cnv);
    aof.setParam("tol", m_time_on_leg);
    aof.setParam("collision_distance", min_util_cpa_dist);
    aof.setParam("all_clear_distance", m_max_util_cpa_dist);
    bool ok = aof.initialize();
    
    if(!ok) {
      postEMessage("Unable to init AOF_AvoidCollision.");
      postErasableBearingLine();
      return(0);
    }    
    OF_Reflector reflector(&aof, 1);
    m_domain = subDomain(m_domain, "course,speed");
    reflector.create(m_build_info);
    ipf = reflector.extractIvPFunction();
    string warnings = reflector.getWarnings();
    postMessage("AVD_STATUS", warnings);
  }    

  else if(m_domain.hasDomain("depth")) {
    AOF_AvoidCollisionDepth aof(m_domain);
    ok = ok && aof.setParam("osy", m_osy);
    ok = ok && aof.setParam("osx", m_osx);
    ok = ok && aof.setParam("osh", m_osh);
    ok = ok && aof.setParam("osv", m_osv);
    ok = ok && aof.setParam("cny", m_cny);
    ok = ok && aof.setParam("cnx", m_cnx);
    ok = ok && aof.setParam("cnh", m_cnh);
    ok = ok && aof.setParam("cnv", m_cnv);
    ok = ok && aof.setParam("tol", m_time_on_leg);
    ok = ok && aof.setParam("collision_distance", m_min_util_cpa_dist);
    ok = ok && aof.setParam("all_clear_distance", m_max_util_cpa_dist);
    ok = ok && aof.setParam("collision_depth", m_collision_depth);
    ok = ok && aof.initialize();
 
    double roc = aof.getROC();
    postMessage("ROC", roc);

    if(!ok) {
      postEMessage("Unable to init AOF_AvoidCollision.");
      postErasableBearingLine();
      return(0);
    }    
    OF_Reflector reflector(&aof, 1);
    
    unsigned int index = (unsigned int)(m_domain.getIndex("depth"));
    unsigned int disc_val = m_domain.getDiscreteVal(index, m_collision_depth, 1);

    IvPBox region_bot = domainToBox(m_domain);
    region_bot.pt(index,0) = disc_val;

    IvPBox region_top = domainToBox(m_domain);
    region_top.pt(index,1) = disc_val-1;

    reflector.setParam("uniform_amount", "1");
    reflector.setParam("refine_region", region_top);
    reflector.setParam("refine_piece", "discrete @ course:3,speed:3,depth:100");

    reflector.setParam("refine_region", region_bot);
    reflector.setParam("refine_piece", region_bot);
    reflector.create();
    ipf = reflector.extractIvPFunction();
    string warnings = reflector.getWarnings();
    postMessage("AVD_STATUS", warnings);
  }

  if(ipf) {
    ipf->getPDMap()->normalize(0.0, 100.0);
    ipf->setPWT(m_relevance * m_priority_wt);
  }


  postViewableBearingLine();

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: getRelevance
//            Calculate the relevance first. If zero-relevance, 
//            we won't bother to create the objective function.

double BHV_AvoidCollision::getRelevance()
{
  // First declare the range of relevance values to be calc'ed
  double min_dist_relevance = 0.0;
  double max_dist_relevance = 1.0;
  double rng_dist_relevance = max_dist_relevance - min_dist_relevance;
  
  m_contact_range = hypot((m_osx - m_cnx),(m_osy - m_cny));
  m_curr_closing_spd = closingSpeed(m_osx, m_osy, m_osv, m_osh,
				    m_cnx, m_cny, m_cnv, m_cnh);

  if(m_contact_range >= m_pwt_outer_dist) {
    //postInfo(0,0);
    return(0);
  }

  double dpct, drange;
  if(m_contact_range <= m_pwt_inner_dist)
    dpct = max_dist_relevance;
  
  // Note: drange should never be zero since either of the above
  // conditionals would be true and the function would have returned.
  drange = (m_pwt_outer_dist - m_pwt_inner_dist);
  dpct = (m_pwt_outer_dist - m_contact_range) / drange;
  
  // Apply the grade scale to the raw distance
  double mod_dpct = dpct; // linear case
  if(m_pwt_grade == "quadratic")
    mod_dpct = dpct * dpct;
  else if(m_pwt_grade == "quasi")
    mod_dpct = pow(dpct, 1.5);

  double d_relevance = (mod_dpct * rng_dist_relevance) + min_dist_relevance;


  return(d_relevance);  

#if 0
  // *********** DISABLED BELOW ******
  //  default:            0.0                         1.0            
  //                       o---------------------------o
  //
  //          o---------------------------o
  //        -0.5                         0.5
  //

  double min_roc_relevance = min_dist_relevance - (0.75*rng_dist_relevance);
  double max_roc_relevance = max_dist_relevance - (0.75*rng_dist_relevance);
  double rng_roc_relevance = max_roc_relevance - min_roc_relevance;

  double srange = m_roc_max_heighten - m_roc_max_dampen;
  double spct = 0.0;
  double eval_closing_spd = m_curr_closing_spd;
  if(srange > 0) {
    if(m_curr_closing_spd < m_roc_max_dampen)
      eval_closing_spd = m_roc_max_dampen;
    if(m_curr_closing_spd > m_roc_max_heighten)
      eval_closing_spd = m_roc_max_heighten;
    spct = (eval_closing_spd - m_roc_max_dampen) / srange;
  }

  double s_relevance = (spct * rng_roc_relevance) + min_roc_relevance;
  
  double combined_relevance = d_relevance + s_relevance;
  
  if(combined_relevance < min_dist_relevance)
    combined_relevance = min_dist_relevance;
  if(combined_relevance > max_dist_relevance)
    combined_relevance = max_dist_relevance;
  
  postInfo(dpct, spct);

  return(combined_relevance);
#endif
}

//-----------------------------------------------------------
// Procedure: postInfo

void BHV_AvoidCollision::postInfo(double dpct, double spct)
{
  string bhv_tag = toupper(getDescriptor());
  bhv_tag = findReplace(bhv_tag, "BHV_", "");
  bhv_tag = findReplace(bhv_tag, "(d)", "");

  postMessage("DPCT_BHV_"+bhv_tag, dpct);
  postMessage("SPCT_BHV_"+bhv_tag, spct);
}


//-----------------------------------------------------------
// Procedure: postRange

void BHV_AvoidCollision::postRange(bool ok)
{
  string bhv_tag = toupper(getDescriptor());
  bhv_tag = findReplace(bhv_tag, "BHV_", "");
  bhv_tag = findReplace(bhv_tag, "(d)", "");
  if(!ok) {
    postMessage("RANGE_AVD_" + m_contact, -1);
    postMessage("CLOSING_SPD_AVD_"+ m_contact, 0);
  }
  else {
    // round the speed a bit first so to reduce the number of 
    // posts to the db which are based on change detection.
    double cls_speed = snapToStep(m_curr_closing_spd, 0.1);
    postMessage(("CLSG_SPD_AVD_"+m_contact), cls_speed);
    
    // Post to integer precision unless very close to contact
    if(m_contact_range <= 10)
      postMessage(("RANGE_AVD_"+m_contact), m_contact_range);
    else
      postIntMessage(("RANGE_AVD_"+m_contact), m_contact_range);
  }
}


//-----------------------------------------------------------
// Procedure: updatePlatformInfo

bool BHV_AvoidCollision::updatePlatformInfo()
{
#if 0
  bool ok = true;
  string avoiding = getBufferStringVal("AVOIDING", ok);
  if(ok && (tolower(avoiding) == "true"))
    m_avoiding = true;
  else
    m_avoiding = false;
#endif
  return(IvPContactBehavior::updatePlatformInfo());
}




