/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_GiveWay16.cpp                                    */
/*    DATE:                                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <iostream>
#include <math.h>
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BHV_GiveWay16.h"
#include "AOF_R16.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "MBUtils.h"
#include "CPA_Utils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_GiveWay16::BHV_GiveWay16(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)avoid_collision");
  this->setParam("unifbox", "course=3, speed=3");
  this->setParam("gridbox", "course=9, speed=6");
  
  domain = subDomain(domain, "course,speed");

  m_active_outer_dist = 200;
  m_active_inner_dist = 50;
  m_collision_dist    = 10; 
  m_all_clear_dist    = 75; 
  m_active_grade      = "quasi";
  m_roc_max_dampen    = -2.0; 
  m_roc_max_heighten  = 2.0; 

  m_on_no_contact_ok  = true;

  info_vars.push_back("NAV_X");
  info_vars.push_back("NAV_Y");
  info_vars.push_back("NAV_SPEED");
  info_vars.push_back("NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_GiveWay16::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParamCommon(g_param, g_val))
    return(true);

  if((g_param == "them") || (g_param == "contact")) {
    if(!param_lock) {
      m_contact = toupper(g_val);
      info_vars.push_back(m_contact+"_NAV_X");
      info_vars.push_back(m_contact+"_NAV_Y");
      info_vars.push_back(m_contact+"_NAV_SPEED");
      info_vars.push_back(m_contact+"_NAV_HEADING");
    }
    return(true);
  }  
  else if((g_param == "active_distance") ||
	  (g_param == "active_outer_distance")) {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock) {
      m_active_outer_dist = dval;
      if(m_active_inner_dist > m_active_outer_dist)
	m_active_inner_dist = m_active_outer_dist;
    }
    return(true);
  }  
  else if(g_param == "active_inner_distance") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock) {
      m_active_inner_dist = dval;
      if(m_active_outer_dist < m_active_inner_dist)
	m_active_outer_dist = m_active_inner_dist;
    }
    return(true);
  }  
  else if(g_param == "collision_distance") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock)
      m_collision_dist = dval;
    return(true);
  }  
  else if(g_param == "all_clear_distance") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock)
      m_all_clear_dist = dval;
    return(true);
  }  
  else if(g_param == "active_grade") {
    g_val = tolower(g_val);
    if((g_val!="linear") && (g_val!="quadratic") && 
       (g_val!="quasi"))
      return(false);
    if(!param_lock)
      m_active_grade = g_val;
    return(true);
    return(true);
  }  
  else if(g_param == "roc_max_heighten") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock) {
      m_roc_max_heighten = dval;
      if(m_roc_max_dampen > m_roc_max_heighten)
	m_roc_max_dampen = m_roc_max_heighten;
    }
    return(true);
  }  
  else if(g_param == "roc_max_dampen") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock) {
      m_roc_max_dampen = dval;
      if(m_roc_max_heighten < m_roc_max_heighten)
	m_roc_max_heighten = m_roc_max_dampen;
    }
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
  return(false);
}


//-----------------------------------------------------------
// Procedure: onIdleState()

void BHV_GiveWay16::onIdleState() 
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
// Procedure: produceOF

IvPFunction *BHV_GiveWay16::produceOF() 
{
  if(!unif_box || !grid_box) {
    postRange(false);
    postEMessage("Null UnifBox or GridBox.");
    return(0);
  }
  
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


  AOF_R16 aof(domain);

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
  reflector.createUniform(unif_box, grid_box);
  IvPFunction *ipf = reflector.extractOF();

  ipf->getPDMap()->normalize(0.0, 100.0);

  ipf->setPWT(relevance * priority_wt);

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: getBufferInfo

bool BHV_GiveWay16::getBufferInfo()
{
  bool ok1, ok2;

  m_cnh = info_buffer->dQuery(m_contact+"_NAV_HEADING", ok1);
  m_cnv = info_buffer->dQuery(m_contact+"_NAV_SPEED",   ok2);
  if(!ok1 || !ok2) {    
    string msg = m_contact + " heading/speed info not found";
    if(m_on_no_contact_ok)
      postWMessage(msg);
    else
      postEMessage(msg);
    return(false);
  }

  m_osh = info_buffer->dQuery("NAV_HEADING", ok1);
  m_osv = info_buffer->dQuery("NAV_SPEED", ok2);
  if(!ok1 || !ok2) {
    postEMessage("ownship course/speed info not found.");
    return(false);
  }

  m_osh = angle360(m_osh);
  m_cnh = angle360(m_cnh);

  m_cnx = info_buffer->dQuery(m_contact+"_NAV_X", ok2);
  m_cny = info_buffer->dQuery(m_contact+"_NAV_Y", ok1);
  if(!ok1 || !ok2) {
    string msg = m_contact + " NAV_X/Y info not found";
    if(m_on_no_contact_ok)
      postWMessage(msg);
    else
      postEMessage(msg);
    return(false);
  }

  m_osx = info_buffer->dQuery("NAV_X", ok2);
  m_osy = info_buffer->dQuery("NAV_Y", ok1);
  if(!ok1 || !ok2) {
    postEMessage("ownship x/y info not found.");
    return(false);
  }

  return(true);
}


//-----------------------------------------------------------
// Procedure: getRelevance

double BHV_GiveWay16::getRelevance()
{
  
  // If for some reason the two vehicles are in the same x,y 
  // position - return zero relevance.
  if((m_osx == m_cnx) && (m_osy == m_cny))
    return(0);

  // If range was specified, and we're out of range, return
  // zero relevance. Range of -1 means range wasn't specified.
  double dist = hypot((m_osx - m_cnx), (m_osy - m_cny));
  if(dist > m_active_outer_dist)
    return(0);
  
  double ang_to_os = relAng(m_cnx, m_cny, m_osx, m_osy);

  //cout << "ang_to_os:" << ang_to_os << endl;

  // Determine the "bearing delta" - the difference between the 
  // course of a vehicle and the bearing of that vehicle to the 
  // other vehicle.
  //
  //                 os     Bearing to ownship is 35 degrees
  //                /
  //               /
  //              /        "Bearing delta" is 55 degrees
  //             /
  //            /
  //          cn------->   contact course is 90 degrees
  //

  double cn_bearing_delta = angle360(m_cnh - ang_to_os);

  //cout << "cn_bearing_delta:" << cn_bearing_delta << endl;

  if((cn_bearing_delta <= 15.0) || (cn_bearing_delta > 90.0))
    return(0.0);
  
  if(dist < m_active_outer_dist)
    dist = m_active_outer_dist;
  
  double range_range = (m_active_outer_dist - m_active_inner_dist);
  double pct = 1.0 -  ((dist - m_active_inner_dist) / range_range);
  return(pct);
}


//-----------------------------------------------------------
// Procedure: postRange

void BHV_GiveWay16::postRange(bool ok)
{
  string bhv_tag = toupper(getDescriptor());
  bhv_tag = findReplace(bhv_tag, "BHV_", "");
  bhv_tag = findReplace(bhv_tag, "(d)", "");
  if(!ok) {
    postMessage("RANGE_AVD_" + m_contact, -1);
    postMessage("CLSG_SPD_AVD_"+ m_contact, 0);
  }
  else {
    postMessage("RANGE_AVD_" + m_contact, m_curr_distance);
    postMessage("CLSG_SPD_AVD_" + m_contact, m_curr_closing_spd);
  }
}



