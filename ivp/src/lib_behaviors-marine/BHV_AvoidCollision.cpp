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
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "avoid_collision");
  this->setParam("build_info", "uniform_piece = discrete @ course:3,speed:3");
  this->setParam("build_info", "uniform_grid  = discrete @ course:9,speed:6");
  
  m_domain = subDomain(m_domain, "course,speed");

  m_active_outer_dist = 200;
  m_active_inner_dist = 50;
  m_collision_dist    = 10; 
  m_all_clear_dist    = 75; 
  m_active_grade      = "quasi";
  m_roc_max_dampen    = -2.0; 
  m_roc_max_heighten  = 2.0; 

  // Behavior will only post a warning - not error - if no contact info
  m_on_no_contact_ok  = true;

  m_extrapolate  = true;
  m_decay_start = 60;
  m_decay_end   = 120;
  m_extrapolator.setDecay(m_decay_start, m_decay_end);
  

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_AvoidCollision::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParam(g_param, g_val))
    return(true);

  if((g_param == "them") || (g_param == "contact")) {
    m_contact = toupper(g_val);
    addInfoVars(m_contact+"_NAV_UTC");
    addInfoVars(m_contact+"_NAV_X");
    addInfoVars(m_contact+"_NAV_Y");
    addInfoVars(m_contact+"_NAV_SPEED");
    addInfoVars(m_contact+"_NAV_HEADING");
    return(true);
  }  
  else if((g_param == "active_distance") ||
	  (g_param == "active_outer_distance")) {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_active_outer_dist = dval;
    if(m_active_inner_dist > m_active_outer_dist)
      m_active_inner_dist = m_active_outer_dist;
    return(true);
  }  
  else if(g_param == "active_inner_distance") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_active_inner_dist = dval;
    if(m_active_outer_dist < m_active_inner_dist)
      m_active_outer_dist = m_active_inner_dist;
    return(true);
  }  
  else if(g_param == "collision_distance") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_collision_dist = dval;
    return(true);
  }  
  else if(g_param == "all_clear_distance") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_all_clear_dist = dval;
    return(true);
  }  
  else if(g_param == "active_grade") {
    g_val = tolower(g_val);
    if((g_val!="linear") && (g_val!="quadratic") && 
       (g_val!="quasi"))
      return(false);
    m_active_grade = g_val;
    return(true);
  }  
  else if(g_param == "roc_max_heighten") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_roc_max_heighten = dval;
    if(m_roc_max_dampen > m_roc_max_heighten)
      m_roc_max_dampen = m_roc_max_heighten;
    return(true);
  }  
  else if(g_param == "roc_max_dampen") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_roc_max_dampen = dval;
    if(m_roc_max_heighten < m_roc_max_heighten)
      m_roc_max_heighten = m_roc_max_dampen;
    return(true);
  }  
  else if(g_param == "on_no_contact_ok") {
    g_val = tolower(g_val);
    if((g_val != "true") && (g_val != "false") && 
       (g_val != "yes")  && (g_val != "no"))
      return(false);
    m_on_no_contact_ok = ((g_val == "true") || (g_val == "yes"));
    return(true);
  }  
  else if (g_param == "extrapolate") 
    {
      string extrap = toupper(g_val);
      m_extrapolate = (extrap == "TRUE");
      return(true);
    }
  else if(g_param == "decay") {
    vector<string> svector = parseString(g_val, ',');
    if(svector.size() == 2) {
      svector[0] = stripBlankEnds(svector[0]);
      svector[1] = stripBlankEnds(svector[1]);
      if(isNumber(svector[0]) && isNumber(svector[1])) {
	double start = atof(svector[0].c_str());
	double end   = atof(svector[1].c_str());
	if((start >= 0) && (start <= end)) {
	  m_decay_start = start;
	  m_decay_end   = end;
	  m_extrapolator.setDecay(start,end);
	  return(true);
	}
      }
    }
  }  
  else if(g_param == "decay_end") {
    if(isNumber(g_val)) {
      m_decay_end = atof(g_val.c_str());
      
      return(true);
    }
  }  
    return(false);
}


//-----------------------------------------------------------
// Procedure: onIdleState()

void BHV_AvoidCollision::onIdleState() 
{
  bool ok = getBufferInfo();
  if(!ok)
    postRange(false);
  else {
    m_curr_distance = hypot((m_osx - m_cnx),(m_osy - m_cny));
    postRange(true);
  }
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_AvoidCollision::onRunState() 
{
  if(m_contact == "") {
    postRange(false);
    postEMessage("contact ID not set.");
    return(0);
  }

  bool ok = getBufferInfo();
  if(!ok) {
    postRange(false);
    return(0);
  }

  double relevance = getRelevance();
  postRange(true);

  if(relevance <= 0)
    return(0);


  AOF_AvoidCollision aof(m_domain);

  ok = true;
  ok = ok && aof.setParam("osy", m_osy);
  ok = ok && aof.setParam("osx", m_osx);
  ok = ok && aof.setParam("cny", m_cny);
  ok = ok && aof.setParam("cnx", m_cnx);
  ok = ok && aof.setParam("cnh", m_cnh);
  ok = ok && aof.setParam("cnv", m_cnv);
  ok = ok && aof.setParam("tol", 120);
  ok = ok && aof.setParam("collision_distance", m_collision_dist);
  ok = ok && aof.setParam("all_clear_distance", m_all_clear_dist);
  ok = ok && aof.initialize();

  if(!ok) {
    postEMessage("Unable to init AOF_AvoidCollision.");
    return(0);
  }

  OF_Reflector reflector(&aof, 1);
  

  reflector.create(m_build_info);
  IvPFunction *ipf = reflector.extractOF();

  ipf->getPDMap()->normalize(0.0, 100.0);

  ipf->setPWT(relevance * m_priority_wt);

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: getBufferInfo

bool BHV_AvoidCollision::getBufferInfo()
{
  bool ok = true;

  if(ok) m_cnh = getBufferDoubleVal(m_contact+"_NAV_HEADING", ok);
  if(ok) m_cnv = getBufferDoubleVal(m_contact+"_NAV_SPEED",   ok);
  if(ok) m_cnutc = getBufferDoubleVal(m_contact+"_NAV_UTC", ok);
  if(!ok) {    
    if(!m_on_no_contact_ok) {
      string msg = m_contact + " heading/speed info not found";
      postEMessage(msg);
    }
    return(false);
  }

  if(ok) m_osh = getBufferDoubleVal("NAV_HEADING", ok);
  if(ok) m_osv = getBufferDoubleVal("NAV_SPEED", ok);
  if(!ok) {
    postEMessage("ownship course/speed info not found.");
    return(false);
  }

  m_osh = angle360(m_osh);
  m_cnh = angle360(m_cnh);

  if(ok) m_cnx = getBufferDoubleVal(m_contact+"_NAV_X", ok);
  if(ok) m_cny = getBufferDoubleVal(m_contact+"_NAV_Y", ok);
  if(!ok) {
    if(!m_on_no_contact_ok) {
      string msg = m_contact + " NAV_X/Y info not found";
      postEMessage(msg);
    }
    return(false);
  }
  
  if(ok) m_osx = getBufferDoubleVal("NAV_X", ok);
  if(ok) m_osy = getBufferDoubleVal("NAV_Y", ok);
  if(!ok) {
    postEMessage("ownship x/y info not found.");
    return(false);
  }

  // extrapolation added by H. Schmidt in GLINT08 for use underwater with spotty comms.

  double curr_time = getBufferCurrTime();
  double delta_time = curr_time - m_cnutc;
  
  postIntMessage(m_contact+"_CONTACT_TIME", m_cnutc);
  postIntMessage(m_contact+"_DELTA_TIME", delta_time);
  
  if(!m_extrapolate)
    return(true);
  
  m_extrapolator.setPosition(m_cnx, m_cny, m_cnv, m_cnh, m_cnutc);
  
  // Even if mark_time is zero and thus "fresh", still derive the 
  // the contact position from the extrapolator since the UTC time
  // of the position in this report may still be substantially 
  // behind the current own-platform UTC time.
  
  double new_cnx, new_cny;
  ok = m_extrapolator.getPosition(new_cnx, new_cny, curr_time);
  
  if(ok) {
    m_cnx = new_cnx;
    m_cny = new_cny;
    return(true);
  }
  else {
    postWMessage("Incomplete Linear Extrapolation");
    return(false);
  }

  return(true);
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
  
  m_curr_distance = hypot((m_osx - m_cnx),(m_osy - m_cny));
  m_curr_closing_spd = closingSpeed(m_osx, m_osy, m_osv, m_osh,
				    m_cnx, m_cny, m_cnv, m_cnh);
  
  if(m_curr_distance >= m_active_outer_dist) {
    //postInfo(0,0);
    return(0);
  }

  double dpct, drange;
  if(m_curr_distance <= m_active_inner_dist)
    dpct = max_dist_relevance;
  
  // Note: drange should never be zero since either of the above
  // conditionals would be true and the function would have returned.
  drange = (m_active_outer_dist - m_active_inner_dist);
  dpct = (m_active_outer_dist - m_curr_distance) / drange;
  
  // Apply the grade scale to the raw distance
  double mod_dpct = dpct; // linear case
  if(m_active_grade == "quadratic")
    mod_dpct = dpct * dpct;
  else if(m_active_grade == "quasi")
    mod_dpct = pow(dpct, 1.5);

  double d_relevance = (mod_dpct * rng_dist_relevance) + min_dist_relevance;


  return(d_relevance);  // *********** DISABLED BELOW ******

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
    postMessage("CLSG_SPD_AVD_"+ m_contact, 0);
  }
  else {
    // round the speed a bit first so to reduce the number of 
    // posts to the db which are based on change detection.
    double cls_speed = snapToStep(m_curr_closing_spd, 0.1);
    postMessage(("CLSG_SPD_AVD_"+m_contact), cls_speed);
    //postMessage(("CLSG_SPD_AVD_"+m_contact), m_curr_closing_spd);
    
    // Post to integer precision unless very close to contact
    if(m_curr_distance <= 10)
      postMessage(("RANGE_AVD_"+m_contact), m_curr_distance);
    else
      postIntMessage(("RANGE_AVD_"+m_contact), m_curr_distance);
  }
}



