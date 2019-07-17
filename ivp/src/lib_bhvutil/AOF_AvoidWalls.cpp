/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_AvoidWalls.cpp                               */
/*    DATE: Oct 29th, 2018                                       */
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

#include <iostream>
#include <cmath> 
#include "AOF_AvoidWalls.h"
#include "AngleUtils.h"
#include "IvPDomain.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_AvoidWalls::AOF_AvoidWalls(IvPDomain gdomain) : AOF(gdomain)
{
  m_crs_ix = gdomain.getIndex("course");
  m_spd_ix = gdomain.getIndex("speed");

  m_tol = 60;
  m_osx = 0;
  m_osy = 0;
  m_osh = 0;

  m_turn_radius = 0;
  
  m_nogo_ttcpa = 10;
  m_safe_ttcpa = 30;
  
  m_collision_distance = 0;
  m_all_clear_distance = 0;

  m_tol_set = false;
  m_osx_set = false;
  m_osy_set = false;
  m_osh_set = false;

  m_turn_radius_set = false;

  m_nogo_ttcpa_set = false;
  m_safe_ttcpa_set = false;

  m_collision_distance_set = false;
  m_all_clear_distance_set = false;

  m_wall_engine_initialized = false;

  m_helm_iter = 0;

  m_ttc_base = 10;   // secs
  m_ttc_rate = 0.1;  // m/sec
  
  m_max_util = 100;
  fptr = 0;
}

//----------------------------------------------------------
// Procedure: Destructor

AOF_AvoidWalls::~AOF_AvoidWalls()
{
  if(fptr)
    fclose(fptr);
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_AvoidWalls::setParam(const string& param, double param_val)
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
  else if((param == "helm_iteration") && (param_val > 0)) {
    m_helm_iter = (unsigned int)(param_val);
    return(true);
  }
  
  else if((param == "turn_radius") && (param_val > 0)) {
    m_turn_radius = param_val;
    m_turn_radius_set = true;
    return(true);
  }
  else if((param == "nogo_ttcpa") && (param_val > 0)) {
    m_nogo_ttcpa = param_val;
    if(m_safe_ttcpa < m_nogo_ttcpa)
      m_safe_ttcpa = m_nogo_ttcpa;
    m_nogo_ttcpa_set = true;
    return(true);
  }
  else if((param == "safe_ttcpa") && (param_val > 0)) {
    m_safe_ttcpa = param_val;
    if(m_nogo_ttcpa > m_safe_ttcpa)
      m_nogo_ttcpa = m_safe_ttcpa;
    m_safe_ttcpa_set = true;
    return(true);
  }
  else if((param == "ttc_base") && (param_val > 0)) {
    m_ttc_base = param_val;
    m_ttc_base_set = true;
    return(true);
  }
  else if((param == "ttc_rate") && (param_val > 0)) {
    m_ttc_rate = param_val;
    m_ttc_rate_set = true;
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
  
  string error_msg = "Unhandled config param: [" + param + "]";
  postMsgAOF(error_msg);
  return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_AvoidWalls::initialize()
{  
  // Check that the domain has the two vars course and speed.
  if(!m_domain.hasDomain("course") || !m_domain.hasDomain("speed")) {
    postMsgAOF("IvP domain does not have course and speed");
    return(false);
  }

  if(!m_osx_set || !m_osy_set || !m_osh_set) {
    postMsgAOF("osx, osy or osh not set");
    return(false);
  }
  
  if(!m_turn_radius_set) {
    postMsgAOF("turn_radius not set");
    return(false);
  }
  
  if(!m_nogo_ttcpa_set || !m_safe_ttcpa_set) {
    postMsgAOF("nogo_ttcpa or safe_ttcpa not set");
    return(false);
  }
  
  if(m_nogo_ttcpa >= m_safe_ttcpa) {
    postMsgAOF("nogo_ttcpa must be less than safe_ttcpa");
    return(false);
  }
  
  if(!m_tol_set) {
    postMsgAOF("time on leg not set");
    return(false);
  }

  if(!m_collision_distance_set || !m_all_clear_distance_set) {
    postMsgAOF("collision_dist or all_clear_dist not set");
    return(false);
  }

  if(m_collision_distance > m_all_clear_distance) {
    postMsgAOF("collision_dist <= all_clear_dist");
    return(false);
  }

  // Initialization will be avoided if the user has already initialized
  // or if the WallEngine was set already via the setWallEngine() function.
  // For this reason, users should make sure that setWallEngine() is called
  // first, before this function, to make sure that the WallEngine is not
  // initialized twice. Not incorrect if so, but this is inefficient since
  // WallEngine initialization involves the building of caches.

  bool ok = m_wall_engine.setEngine(m_osx, m_osy, m_osh,
				    m_turn_radius, m_walls);

  double min_dist_to_wall = m_wall_engine.getMinRangeToWall();
  if(min_dist_to_wall < m_collision_distance)
    m_collision_distance = min_dist_to_wall;

  
  return(ok);  
  if((m_vname != "") && (m_helm_iter > 0)) {
    string filename = m_vname + "_aof_walls_" + uintToString(m_helm_iter);
    fptr = fopen(filename.c_str(), "w");
  }
}


//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given <Course, Speed, Time-on-leg> tuple 
//               given by a 3D ptBox (b).
//            Determines naut mile Closest-Point-of-Approach (CPA)
//               and returns a value after passing it thru the 
//               metric() function.

double AOF_AvoidWalls::evalBox(const IvPBox *b) const
{
  double eval_crs = 0;
  double eval_spd = 0; 

  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix), eval_spd);

  double xcpa  = m_wall_engine.getXCPA(eval_crs, eval_spd,
				       m_ttc_base, m_ttc_rate);
  if(xcpa < 0)
    return(100);
  
  double eval = metric(xcpa);

#if 0
  if(fptr) {
    string msg = "x=" + doubleToStringX(m_osx);
    msg += ", y=" + doubleToStringX(m_osy);
    msg += ", h=" + doubleToStringX(eval_crs);
    msg += ", v=" + doubleToStringX(eval_spd);
    msg += " --> " + doubleToStringX(xcpa);
    msg += " --> " + doubleToStringX(eval);
    fprintf(fptr, "%s\n", msg.c_str());
  }
#endif
  
  return(eval);
}

//----------------------------------------------------------------
// Procedure: setWalls

bool AOF_AvoidWalls::setWalls(vector<XYSegList> walls) 
{
  if(walls.size() == 0)
    return(false);
  m_walls = walls;
  return(true);
}

//----------------------------------------------------------------
// Procedure: metric

double AOF_AvoidWalls::metric(double xcpa) const
{
  double min_cpa = m_collision_distance;
  double max_cpa = m_all_clear_distance;

  // Sanity check: Confirm the range that we'll be using as a
  // denominator and make sure the ranges are positive numbers.
  double cpa_range   = (max_cpa - min_cpa);
  if(cpa_range <= 0)
    return(0);


  // Part 1: Apply the cpa distance metric
  double eval_cpa_pct = 0;
  if(xcpa <= min_cpa)
    eval_cpa_pct = 0;
  else if(xcpa >= max_cpa)
    eval_cpa_pct = 1;
  else
    eval_cpa_pct = (xcpa - min_cpa) / cpa_range;

  double retval = eval_cpa_pct * 100;
  return(retval);
}

