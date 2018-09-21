/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_AvoidCollisionDepth.cpp                          */
/*    DATE: August 6th, 2008                                     */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <cmath> 
#include "AOF_AvoidCollisionDepth.h"
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

AOF_AvoidCollisionDepth::AOF_AvoidCollisionDepth(IvPDomain gdomain) 
  : AOF(gdomain)
{
  m_crs_ix = gdomain.getIndex("course");
  m_spd_ix = gdomain.getIndex("speed");
  m_dep_ix = gdomain.getIndex("depth");

  m_tol_set = false;
  m_osx_set = false;
  m_osy_set = false;
  m_osh_set = false;
  m_osv_set = false;
  m_cnx_set = false;
  m_cny_set = false;
  m_cnh_set = false;
  m_cnv_set = false;

  m_collision_distance_set = false;
  m_all_clear_distance_set = false;
  m_collision_depth = 0;
  cpa_engine = 0;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_AvoidCollisionDepth::setParam(const string& param, double param_val)
{
  if(param == "osx") {
    m_osx = param_val;
    m_osx_set = true;
    return(true);
  }
  else if(param == "osy") {
    m_osy = param_val;
    m_osy_set = true;
    return(true);
  }
  else if(param == "osh") {
    m_osh = param_val;
    m_osh_set = true;
    return(true);
  }
  else if(param == "osv") {
    m_osv = param_val;
    m_osv_set = true;
    return(true);
  }
  else if(param == "cnx") {
    m_cnx = param_val;
    m_cnx_set = true;
    return(true);
  }
  else if(param == "cny") {
    m_cny = param_val;
    m_cny_set = true;
    return(true);
  }
  else if(param == "cnh") {
    m_cnh = param_val;
    m_cnh_set = true;
    return(true);
  }
  else if(param == "cnv") {
    m_cnv = param_val;
    m_cnv_set = true;
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
  else if(param == "collision_depth") {
    if(param_val >= 0) {
      m_collision_depth = param_val;
      return(true);
    }
    return(false);
  }
  else if(param == "tol") {
    m_tol = param_val;
    m_tol_set = true;
    return(true);
  }
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_AvoidCollisionDepth::initialize()
{
  if((m_crs_ix==-1) || (m_spd_ix==-1) || (m_dep_ix==-1))
    return(false);

  if(!m_osx_set || !m_osy_set || !m_cnx_set) 
    return(false);

  if(!m_cny_set || !m_cnh_set || !m_cnv_set) 
    return(false);

  if(!m_collision_distance_set || 
     !m_all_clear_distance_set || !m_tol_set) 
    return(false);

  if(cpa_engine)
    delete(cpa_engine);

  cpa_engine = new CPAEngine(m_cny, m_cnx, m_cnh, 
			     m_cnv, m_osy, m_osx);

  m_max_decision_depth = m_domain.getVarHigh(m_dep_ix);
  m_rate_of_closure = cpa_engine->evalROC(m_osh, m_osv);
  
  return(true);
}


//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given <Course, Speed, Time-on-leg> tuple 
//               given by a 3D ptBox (b).
//            Determines naut mile Closest-Point-of-Approach (CPA)
//               and returns a value after passing it thru the 
//               metric() function.

double AOF_AvoidCollisionDepth::evalBox(const IvPBox *b) const
{
  if(!cpa_engine)
    return(0);

  double eval_crs = 0;
  double eval_spd = 0;
  double eval_dep = 0;;
  m_domain.getVal(m_dep_ix, b->pt(m_dep_ix), eval_dep);
  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix), eval_spd);

  double return_value = 0;
  if(eval_dep >= m_collision_depth)
    return_value = 90 + evalDiveDepth(eval_dep, 10);
  else{
    double cpa_dist  = cpa_engine->evalCPA(eval_crs, eval_spd, m_tol);
    double eval_dist = metric(cpa_dist);
    return_value = eval_dist + evalDiveDepth(eval_dep, 10);
  }
  return(return_value);
}

//----------------------------------------------------------------
// Procedure: metric

double AOF_AvoidCollisionDepth::metric(double eval_dist) const
{
  double min = m_collision_distance;
  double max = m_all_clear_distance;

  // Rate of closer is negative, the two vehicles opening range, 
  // the skew the metric a bit by effectively reducing the all_clear
  // distance to be closer to the collision_distance. 
  if(m_rate_of_closure < 0)
    max += m_rate_of_closure;

  // For the purposes of the metric, to avoid a zero denominator, 
  // enforce that the range is at a minimum of 10 meters.
  if((max-min) < 10)
    max = min+10;

  if(eval_dist < min) 
    return(0);
  if(eval_dist > max) 
    return(100);

  double pct = (eval_dist - min) / (max-min);

  return(pct * 100);
}


//----------------------------------------------------------------
// Procedure: evalDiveDepth
//   Purpose: Add a bit more utility for being at depth closer to 
//            the surface. Just a linear function.

double AOF_AvoidCollisionDepth::evalDiveDepth(double eval_depth,
					      double max_util) const
{
  // If max_decision_depth is not set - something's wrong
  if(m_max_decision_depth <= 0)
    return(0);

  // If eval_depth is not in the below range, something's wrong but
  // we move on by just clipping the eval_depth value.
  if(eval_depth < 0)
    eval_depth = 0;
  if(eval_depth > m_max_decision_depth)
    eval_depth = m_max_decision_depth;

  double pct = (m_max_decision_depth - eval_depth) / m_max_decision_depth;
  
  double util = pct * max_util;

  return(util);
}




















