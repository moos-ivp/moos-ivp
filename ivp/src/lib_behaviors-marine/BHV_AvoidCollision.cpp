/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_AvoidCollision.cpp                               */
/*    DATE: Nov 18th 2006                                        */
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
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "AOF_AvoidCollision.h"
#include "BHV_AvoidCollision.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "MBUtils.h"
#include "CPA_Utils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_AvoidCollision::BHV_AvoidCollision(IvPDomain gdomain) : 
  IvPContactBehavior(gdomain)
{
  this->setParam("descriptor", "avoid_collision");
  this->setParam("build_info", "uniform_piece = discrete @ course:3,speed:3");
  this->setParam("build_info", "uniform_grid  = discrete @ course:9,speed:6");
  
  m_domain = subDomain(m_domain, "course,speed");

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
  
  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_AvoidCollision::setParam(string param, string param_val) 
{
  if(IvPBehavior::setParam(param, param_val))
    return(true);
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
  // bearing_lines = white:0, green:0.65, yellow:0.8, red:1.0
  else if(param == "bearing_line_config") {
    m_bearing_line_show = false;
    vector<string> svector = parseString(param_val, ',');
    unsigned int i, vsize = svector.size();
    bool valid_components = true;
    for(i=0; i<vsize; i++) {
      string left  = tolower(stripBlankEnds(biteString(svector[i],':')));
      string right = stripBlankEnds(svector[i]);
      if(isColor(left) && isNumber(right) && valid_components) {
	m_bearing_line_colors.push_back(left);
	double dval = vclip(atof(right.c_str()), 0, 1);
	m_bearing_line_thresh.push_back(dval);
	m_bearing_line_show = true;
      }
      else
	valid_components = false;
    }
    return(true);
  }  
  return(false);
}


//-----------------------------------------------------------
// Procedure: onRunToIdleState()

void BHV_AvoidCollision::onRunToIdleState() 
{
  postErasableSegList();
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
  postErasableSegList();
  postRepeatableMessage("CONTACT_RESOLVED", m_contact);
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_AvoidCollision::onRunState() 
{
  if(!updatePlatformInfo()) {
    postRange(false);
    return(0);
  }

  double relevance = getRelevance();
  postRange(true);

  if(m_contact_range >= m_completed_dist) {
    setComplete();
    return(0);
  }
  
  if(relevance <= 0) {
    postViewableSegList();
    return(0);
  }

  AOF_AvoidCollision aof(m_domain);
  bool ok = true;
  ok = ok && aof.setParam("osy", m_osy);
  ok = ok && aof.setParam("osx", m_osx);
  ok = ok && aof.setParam("cny", m_cny);
  ok = ok && aof.setParam("cnx", m_cnx);
  ok = ok && aof.setParam("cnh", m_cnh);
  ok = ok && aof.setParam("cnv", m_cnv);
  ok = ok && aof.setParam("tol", m_time_on_leg);
  ok = ok && aof.setParam("collision_distance", m_min_util_cpa_dist);
  ok = ok && aof.setParam("all_clear_distance", m_max_util_cpa_dist);
  ok = ok && aof.initialize();

  if(!ok) {
    postEMessage("Unable to init AOF_AvoidCollision.");
    postErasableSegList();
    return(0);
  }

  OF_Reflector reflector(&aof, 1);
  
  reflector.create(m_build_info);
  IvPFunction *ipf = reflector.extractIvPFunction();

  ipf->getPDMap()->normalize(0.0, 100.0);
  ipf->setPWT(relevance * m_priority_wt);

  postViewableSegList(relevance);

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: getRelevance
//            Calculate the relevance first. If zero-relevance, 
//            we won't bother to create the objective function.

double BHV_AvoidCollision::getRelevance()
{
  // First declare the ange of relevance values to be calc'ed
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
// Procedure: postViewableSegList

void BHV_AvoidCollision::postViewableSegList(double pct)
{
  if(!m_bearing_line_show)
    return;

  string color = "";
  unsigned int i, vsize = m_bearing_line_colors.size();
  for(i=0; (i<vsize)&&(color==""); i++) {
    if(pct <= m_bearing_line_thresh[i])
      color = m_bearing_line_colors[i];
  }

  m_seglist.clear(); 
  m_seglist.set_active(true);
  m_seglist.add_vertex(m_osx, m_osy);
  m_seglist.add_vertex(m_cnx, m_cny);
  m_seglist.set_label(m_us_name + "_" + m_descriptor);
  m_seglist.set_edge_color(color);

  postMessage("VIEW_SEGLIST", m_seglist.get_spec());
}


//-----------------------------------------------------------
// Procedure: postErasableSegList

void BHV_AvoidCollision::postErasableSegList()
{
  m_seglist.set_active(false);
  postMessage("VIEW_SEGLIST", m_seglist.get_spec());
}
