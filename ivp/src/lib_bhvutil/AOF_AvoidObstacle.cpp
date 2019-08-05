/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_AvoidObstacle.cpp                                */
/*    DATE: Aug 2nd, 2006                                        */
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

#include <string>
#include "AOF_AvoidObstacle.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_AvoidObstacle::AOF_AvoidObstacle(IvPDomain gdomain) : AOF(gdomain)
{
  m_crs_ix = gdomain.getIndex("course");
  m_spd_ix = gdomain.getIndex("speed");

  m_osx             = 0;
  m_osy             = 0;
  m_osh             = 0;
  m_allowable_ttc   = 20;  // time-to-collision in seconds

  m_osx_set             = false;
  m_osy_set             = false;
  m_osh_set             = false;
  m_allowable_ttc_set   = false;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_AvoidObstacle::setParam(const string& param, double param_val)
{
  if(param == "os_y") {
    m_osy = param_val;
    m_osy_set = true;
  }
  else if(param == "os_x") {
    m_osx = param_val;
    m_osx_set = true;
  }
  else if(param == "allowable_ttc") {
    if(param_val <= 0)
      return(false);
    m_allowable_ttc = param_val;
    m_allowable_ttc_set = true;
  }
  else if(param == "os_h") {
    m_osh = param_val;
    m_osh_set = true;
  }
  else
    return(false);

  return(true);  
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_AvoidObstacle::setParam(const string& param, const string& value)
{
  return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_AvoidObstacle::initialize()
{
  // Part 1: Sanity Checks
  if(m_crs_ix == -1) {
    postMsgAOF("crs_ix is not set");
    return(false);
  }
  if(m_spd_ix == -1) {
    postMsgAOF("spd_ix is not set");
    return(false);
  }
  if((!m_osx_set) || (!m_osy_set)) {
    postMsgAOF("osx or osy is not set");
    return(false);
  }
  if(!m_osh_set) {
    postMsgAOF("osh is not set");
    return(false);
  }
  if(!m_allowable_ttc_set) {
    postMsgAOF("m_allowable_ttc is not set");
    return(false);
  }
  if(!m_obstacle_orig.is_convex()) {
    postMsgAOF("m_obstacle_orig is not convex");
    return(false);
  }
  if(!m_obstacle_buff.is_convex()) {
    postMsgAOF("m_obstacle_orig is not convex");
    return(false);
  }
  if(m_obstacle_orig.contains(m_osx, m_osy)) {
    postMsgAOF("m_obstacle_orig contains osx,osy");
    return(false);
  }

  // Part 2: Cache the distances mapping a particular heading 
  // to the minimum/closest distance to any of the obstacle polygons.
  // A distance of -1 indicates infinite distance.
  m_cache_distance.clear();
  unsigned int hsize = m_domain.getVarPoints(m_crs_ix);
  vector<double> virgin_cache(hsize, -1);
  m_cache_distance = virgin_cache;

  double heading=0; 
  for(unsigned int i=0; i<hsize; i++) {
    bool ok = m_domain.getVal(m_crs_ix, i, heading);
    if(!ok)
      return(false);
    double min_dist = -1; 
    double dist = m_obstacle_buff.dist_to_poly(m_osx, m_osy, heading);
    if(dist != -1) {
      min_dist = dist;
    }
    m_cache_distance[i] = min_dist;
  }

  // Part 3: Determine if the obstacle is currently on owship port side
  double pcx = m_obstacle_orig.get_center_x();
  double pcy = m_obstacle_orig.get_center_y();
  
  double curr_rel_bng_to_poly = relBearing(m_osx, m_osy, m_osh, pcx, pcy);

  m_obstacle_on_port_curr = true;
  if(curr_rel_bng_to_poly <= 180)
    m_obstacle_on_port_curr = false;

  return(true);
}

//----------------------------------------------------------------
// Procedure: evalBox

double AOF_AvoidObstacle::evalBox(const IvPBox *b) const
{
  // Part 1: Declare the utility range and get the eval crs and speed
  double max_utility = 100;
  double min_utility = 0;

  double eval_crs = 0;
  double eval_spd = 0;
  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix,0), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix,0), eval_spd);

  //===================================================================
  // Part 2: Determine if the eval crs/spd is on a collision trajectory
  // to the obstacle. If on a collision course with unacceptable ttc,
  // then return min_utility now with no further consideration.
  //===================================================================

  int   heading_index = b->pt(m_crs_ix, 0);
  double dist_to_poly = m_cache_distance[heading_index];

  // If on a collision trajectory at non-zero speed, look further at ttc
  if((dist_to_poly != -1) && (eval_spd != 0)) {
  
    // determine time to collision w/ poly (in seconds)
    double time_to_collision = dist_to_poly / eval_spd;
    if(time_to_collision <= m_allowable_ttc)
      return(min_utility);
  }

  //===================================================================
  // Part 3: If hdg/spd ok, then check that the given hdg/spd does NOT
  // result in a change to the side of the polygon of the current traj.
  //===================================================================

  double pcx = m_obstacle_orig.get_center_x();
  double pcy = m_obstacle_orig.get_center_y();
  
  // Determine if the polygon *would be* on ownhip's port or 
  // starboard if the crs being evaluated would be true instead.
  double eval_crs_bng_to_poly = relBearing(m_osx, m_osy, eval_crs, pcx, pcy);
  
  bool obstacle_on_port_eval = true;
  if(eval_crs_bng_to_poly <= 180)
    obstacle_on_port_eval = false;
  
  // If they do not match, return min_utility
  if(m_obstacle_on_port_curr != obstacle_on_port_eval)
    return(min_utility);

  return(max_utility);
}
