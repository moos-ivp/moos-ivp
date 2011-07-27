/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_AvoidCollisionRC.cpp                             */
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
#include <cmath> 
#include "AOF_AvoidCollisionRC.h"
#include "AngleUtils.h"
#include "CPA_Utils.h"
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

AOF_AvoidCollisionRC::AOF_AvoidCollisionRC(IvPDomain gdomain) 
  : AOF(gdomain)
{
  crs_ix = gdomain.getIndex("course");
  spd_ix = gdomain.getIndex("speed");

  os_tol_set = false;
  os_lat_set = false;
  os_lon_set = false;
  cn_lat_set = false;
  cn_lon_set = false;
  cn_crs_set = false;
  cn_spd_set = false;

  collision_distance_set = false;
  all_clear_distance_set = false;
  cpa_engine = 0;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_AvoidCollisionRC::setParam(const string& param, double param_val)
{
  if(param == "os_lat") {
    os_lat = param_val;
    os_lat_set = true;
    return(true);
  }
  else if(param == "os_lon") {
    os_lon = param_val;
    os_lon_set = true;
    return(true);
  }
  else if(param == "cn_lat") {
    cn_lat = param_val;
    cn_lat_set = true;
    return(true);
  }
  else if(param == "cn_lon") {
    cn_lon = param_val;
    cn_lon_set = true;
    return(true);
  }
  else if(param == "cn_crs") {
    cn_crs = param_val;
    cn_crs_set = true;
    return(true);
  }
  else if(param == "cn_spd") {
    cn_spd = param_val;
    cn_spd_set = true;
    return(true);
  }
  else if(param == "collision_distance") {
    collision_distance = param_val;
    collision_distance_set = true;
    return(true);
  }
  else if(param == "all_clear_distance") {
    all_clear_distance = param_val;
    all_clear_distance_set = true;
    return(true);
  }
  else if(param == "os_tol") {
    os_tol = param_val;
    os_tol_set = true;
    return(true);
  }
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_AvoidCollisionRC::initialize()
{
  if((crs_ix==-1) || (spd_ix==-1))
    return(false);

  if(!os_lat_set || !os_lon_set || !cn_lat_set) 
    return(false);

  if(!cn_lon_set || !cn_crs_set || !cn_spd_set) 
    return(false);

  if(!collision_distance_set || 
     !all_clear_distance_set || !os_tol_set) 
    return(false);

  cpa_engine = new CPAEngine(cn_lat, cn_lon, cn_crs, 
			     cn_spd, os_lat, os_lon);

  return(true);
}


//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given <Course, Speed, Time-on-leg> tuple 
//               given by a 3D ptBox (b).
//            Determines naut mile Closest-Point-of-Approach (CPA)
//               and returns a value after passing it thru the 
//               metric() function.

double AOF_AvoidCollisionRC::evalBox(const IvPBox *b) const
{
  double eval_crs, eval_spd, cpa_dist, eval_dist;


  if(!cpa_engine)
    return(0);

  m_domain.getVal(crs_ix, b->pt(crs_ix,0), eval_crs);
  m_domain.getVal(spd_ix, b->pt(spd_ix,0), eval_spd);
  
#if 1
  bool cross = crossesBow(cn_lon, cn_lat, cn_spd, cn_crs, 
			  os_lon, os_lat, eval_spd, eval_crs, os_tol);

  if(cross) 
    return(0);
  return(100);
#endif

  cpa_dist  = cpa_engine->evalCPA(eval_crs, eval_spd, os_tol);

  eval_dist = metric(cpa_dist);
  return(eval_dist);
}

//----------------------------------------------------------------
// Procedure: metric

double AOF_AvoidCollisionRC::metric(double eval_dist) const
{
  double min = collision_distance;
  double max = all_clear_distance;

  if(eval_dist < min) return(0);
  if(eval_dist > max) return(100);

  //double tween = 100.0 * (gval-min) / (max-min);
  double tween = 25.0 + 75.0 * (eval_dist - min) / (max-min);
  return(tween);
}










