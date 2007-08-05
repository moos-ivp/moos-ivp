/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_AvoidCollision.cpp                               */
/*    DATE: Nov 18th 2006                                        */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <math.h>
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

  info_vars.push_back("NAV_X");
  info_vars.push_back("NAV_Y");
  info_vars.push_back("NAV_SPEED");
  info_vars.push_back("NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_AvoidCollision::setParam(string g_param, string g_val) 
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
  return(false);
}


//-----------------------------------------------------------
// Procedure: produceOF

IvPFunction *BHV_AvoidCollision::produceOF() 
{
  if(!unif_box || !grid_box) {
    postEMessage("Null UnifBox or GridBox.");
    return(0);
  }
  
  if(m_contact == "") {
    postEMessage("contact ID not set.");
    return(0);
  }

  bool ok = getBufferInfo();
  if(!ok) 
    return(0);

  double relevance = getRelevance();
  postMessage("AVD_REL", relevance);
  if(relevance <= 0)
    return(0);


  AOF_AvoidCollision aof(domain);
  ok = true;
  ok = ok && aof.setParam("os_lat", m_osy);
  ok = ok && aof.setParam("os_lon", m_osx);
  ok = ok && aof.setParam("cn_lat", m_cny);
  ok = ok && aof.setParam("cn_lon", m_cnx);
  ok = ok && aof.setParam("cn_crs", m_cnh);
  ok = ok && aof.setParam("cn_spd", m_cnv);
  ok = ok && aof.setParam("os_tol", 120);
  ok = ok && aof.setParam("collision_distance", m_collision_dist);
  ok = ok && aof.setParam("all_clear_distance", m_all_clear_dist);
  ok = ok && aof.initialize();

  if(!ok) {
    postEMessage("Unable to init AFO_AvoidCollision.");
    return(0);
  }

  OF_Reflector reflector(&aof, 1);
  reflector.createUniform(unif_box, grid_box);
  IvPFunction *ipf = reflector.extractOF();

  ipf->getPDMap()->normalize(0.0, 100.0);

  ipf->setPWT(relevance * priority_wt);

  postMessage("AVD_ON", 1);
  return(ipf);
}

//-----------------------------------------------------------
// Procedure: getBufferInfo

bool BHV_AvoidCollision::getBufferInfo()
{
  bool ok1, ok2;

  m_cnh = info_buffer->dQuery(m_contact+"_NAV_HEADING", ok1);
  m_cnv = info_buffer->dQuery(m_contact+"_NAV_SPEED",   ok2);
  if(!ok1 || !ok2) {    
    string msg = m_contact + " heading/speed info not found";
    info_buffer->print();
    postWMessage(msg);
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
    postWMessage("contact x/y info not found.");
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
//            Calculate the relevance first. If zero-relevance, 
//            we won't bother to create the objective function.

double BHV_AvoidCollision::getRelevance()
{
  // First declare the ange of relevance values to be calc'ed
  double min_dist_relevance = 0.0;
  double max_dist_relevance = 1.0;
  double rng_dist_relevance = max_dist_relevance - min_dist_relevance;
  
  double dist = hypot((m_osx - m_cnx),(m_osy - m_cny));
  if(dist >= m_active_outer_dist)
    return(min_dist_relevance);
  if(dist <= m_active_inner_dist)
    return(max_dist_relevance);

  // Note: drange should never be zero since either of the above
  // conditionals would be true and the function would have returned.
  double drange = (m_active_outer_dist - m_active_inner_dist);
  double dpct = (m_active_outer_dist - dist) / drange;
  
  // Apply the grade scale to the raw distance
  double mod_dpct = dpct; // linear case
  if(m_active_grade == "quadratic")
    mod_dpct = dpct * dpct;
  else if(m_active_grade == "quasi")
    mod_dpct = pow(dpct, 1.5);

  double d_relevance = (mod_dpct * rng_dist_relevance) + min_dist_relevance;

  //return(d_relevance);

  double closing_spd = closingSpeed(m_osx, m_osy, m_osv, m_osh,
				    m_cnx, m_cny, m_osv, m_osh);

  //  default:            0.0                         1.0            
  //                       o---------------------------o
  //
  //          o---------------------------o
  //        -0.5                         0.5
  //

  double min_roc_relevance = min_dist_relevance - (rng_dist_relevance/2);
  double max_roc_relevance = min_roc_relevance + (rng_dist_relevance/2);
  double rng_roc_relevance = max_roc_relevance - min_roc_relevance;

  double srange = m_roc_max_heighten - m_roc_max_dampen;
  double spct = 0.0;
  if(srange > 0)
    spct = (m_roc_max_heighten - closing_spd) / srange;
  
  double s_relevance = (spct * rng_roc_relevance) + min_roc_relevance;
  
  double combined_relevance = d_relevance + s_relevance;
  
  if(combined_relevance < min_dist_relevance)
    combined_relevance = min_dist_relevance;
  if(combined_relevance > max_dist_relevance)
    combined_relevance = max_dist_relevance;
  
  return(combined_relevance);
    
}








