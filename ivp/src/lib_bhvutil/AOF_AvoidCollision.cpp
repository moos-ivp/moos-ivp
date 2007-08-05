/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_AvoidCollision.cpp                               */
/*    DATE: Nov 18th, 2006                                       */
/*****************************************************************/
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <math.h> 
#include "AOF_AvoidCollision.h"
#include "AngleUtils.h"
#include "IvPDomain.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor
//      args: gcnlat  Given Contact Latitude Position
//      args: gcnlon  Given Contact Longitude Position
//      args: gcncrs  Given Contact Course
//      args: gcnspd  Given Contact Speed
//      args: goslat  Given Ownship Latitude Position
//      args: goslon  Given Ownship Latitude Position

AOF_AvoidCollision::AOF_AvoidCollision(IvPDomain gdomain) 
  : AOF(gdomain)
{
  m_crs_ix = gdomain.getIndex("course");
  m_spd_ix = gdomain.getIndex("speed");

  m_os_tol_set = false;
  m_os_lat_set = false;
  m_os_lon_set = false;
  m_cn_lat_set = false;
  m_cn_lon_set = false;
  m_cn_crs_set = false;
  m_cn_spd_set = false;

  m_collision_distance_set = false;
  m_all_clear_distance_set = false;
  cpa_engine = 0;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_AvoidCollision::setParam(const string& param, double param_val)
{
  if(param == "os_lat") {
    m_os_lat = param_val;
    m_os_lat_set = true;
    return(true);
  }
  else if(param == "os_lon") {
    m_os_lon = param_val;
    m_os_lon_set = true;
    return(true);
  }
  else if(param == "cn_lat") {
    m_cn_lat = param_val;
    m_cn_lat_set = true;
    return(true);
  }
  else if(param == "cn_lon") {
    m_cn_lon = param_val;
    m_cn_lon_set = true;
    return(true);
  }
  else if(param == "cn_crs") {
    m_cn_crs = param_val;
    m_cn_crs_set = true;
    return(true);
  }
  else if(param == "cn_spd") {
    m_cn_spd = param_val;
    m_cn_spd_set = true;
    return(true);
  }
  else if(param == "collision_distance") {
    m_collision_distance = param_val;
    m_collision_distance_set = true;
    return(true);
  }
  else if(param == "all_clear_distance") {
    m_all_clear_distance = param_val;
    m_all_clear_distance_set = true;
    return(true);
  }
  else if(param == "os_tol") {
    m_os_tol = param_val;
    m_os_tol_set = true;
    return(true);
  }
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_AvoidCollision::initialize()
{
  if((m_crs_ix==-1) || (m_spd_ix==-1))
    return(false);

  if(!m_os_lat_set || !m_os_lon_set || !m_cn_lat_set) 
    return(false);

  if(!m_cn_lon_set || !m_cn_crs_set || !m_cn_spd_set) 
    return(false);

  if(!m_collision_distance_set || 
     !m_all_clear_distance_set || !m_os_tol_set) 
    return(false);

  if(cpa_engine)
    delete(cpa_engine);

  cpa_engine = new CPAEngine(m_cn_lat, m_cn_lon, m_cn_crs, 
			     m_cn_spd, m_os_lat, m_os_lon);

  return(true);
}


//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given <Course, Speed, Time-on-leg> tuple 
//               given by a 3D ptBox (b).
//            Determines naut mile Closest-Point-of-Approach (CPA)
//               and returns a value after passing it thru the 
//               metric() function.

double AOF_AvoidCollision::evalBox(const IvPBox *b) const
{
  double eval_crs, eval_spd, cpa_dist, eval_dist;

  if(!cpa_engine)
    return(0);

  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix,0), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix,0), eval_spd);

  cpa_dist  = cpa_engine->evalCPA(eval_crs, eval_spd, m_os_tol);

  eval_dist = metric(cpa_dist);
  return(eval_dist);
}

//----------------------------------------------------------------
// Procedure: metric

double AOF_AvoidCollision::metric(double eval_dist) const
{
  double min = m_collision_distance;
  double max = m_all_clear_distance;

  if(eval_dist < min) return(0);
  if(eval_dist > max) return(100);

  //double tween = 100.0 * (gval-min) / (max-min);
  double tween = 25.0 + 75.0 * (eval_dist - min) / (max-min);
  return(tween);
}









