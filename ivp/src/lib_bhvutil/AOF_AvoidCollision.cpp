/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_AvoidCollision.cpp                               */
/*    DATE: Nov 18th, 2006                                       */
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

  m_tol_set = false;
  m_osx_set = false;
  m_osy_set = false;
  m_cnx_set = false;
  m_cny_set = false;
  m_cnh_set = false;
  m_cnv_set = false;

  m_collision_distance_set = false;
  m_all_clear_distance_set = false;
  cpa_engine = 0;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_AvoidCollision::setParam(const string& param, double param_val)
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

bool AOF_AvoidCollision::initialize()
{
  if((m_crs_ix==-1) || (m_spd_ix==-1))
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

  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix), eval_spd);

  cpa_dist  = cpa_engine->evalCPA(eval_crs, eval_spd, m_tol);

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










