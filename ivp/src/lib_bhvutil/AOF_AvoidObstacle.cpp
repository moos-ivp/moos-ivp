/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_AvoidObstacle.cpp                                */
/*    DATE: Aug 2nd, 2006                                        */
/*    DATE: Sep 22nd, 2014  Mods to single obstacle              */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
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
  m_activation_dist = -1;
  m_allowable_ttc   = 20;  // time-to-collision in seconds
  m_buffer_dist     = 0;

  m_osx_set             = false;
  m_osy_set             = false;
  m_osh_set             = false;
  m_activation_dist_set = false;
  m_allowable_ttc_set   = false;
  m_buffer_dist_set     = false;

  m_present_heading_influence = 1;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_AvoidObstacle::setParam(const string& param, double param_val)
{
  if(param == "os_y") {
    m_osy = param_val;
    m_osy_set = true;
    return(true);
  }
  else if(param == "os_x") {
    m_osx = param_val;
    m_osx_set = true;
    return(true);
  }
  else if(param == "allowable_ttc") {
    if(param_val <= 0)
      return(false);
    m_allowable_ttc = param_val;
    m_allowable_ttc_set = true;
    return(true);
  }
  else if(param == "activation_dist") {
    if(param_val <= 0)
      return(false);
    m_activation_dist = param_val;
    m_activation_dist_set = true;
    return(true);
  }
  else if(param == "os_h") {
    m_osh = param_val;
    m_osh_set = true;
    return(true);
  }
  else if(param == "present_heading_influence") {
    if(param_val < 0)
      param_val = 0;
    else if(param_val > 90)
      param_val = 90;
    m_present_heading_influence = param_val;
    return(true);
  }
  else if(param == "buffer_dist") {
    if(param_val < 0)
      return(false);
    m_buffer_dist = param_val;
    m_buffer_dist_set = true;
    return(true);
  }
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_AvoidObstacle::setParam(const string& param, 
				  const string& param_val)
{
  return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_AvoidObstacle::initialize()
{
  // Part 1: Sanity Checks
  if((m_crs_ix==-1) || (m_spd_ix==-1))
    return(false);
  if((!m_osx_set) || (!m_osy_set) || (!m_osh_set))
    return(false);
  if(!m_allowable_ttc_set)
    return(false);
  if(!m_activation_dist_set)
    return(false);
  if(!m_buffer_dist_set)
    return(false);

  // Part 2: Apply the buffer distance to each of the original obstacles
  applyBuffer();
  return(true);
}


//----------------------------------------------------------------
// Procedure: postInitialize

bool AOF_AvoidObstacle::postInitialize()
{
  // Part 1: Sanity Checks
  if(!m_obstacle_orig.is_convex())
    return(false);
  if(!m_obstacle_buff.is_convex())
    return(false);

  // Part 2: Figure out and cache which obstacles are pertinent
  m_obstacle_pert = true;

  if(polyAft(m_osx, m_osy, m_osh, m_obstacle_buff))
    m_obstacle_pert = false;
  if(m_obstacle_buff.dist_to_poly(m_osx, m_osy) > m_activation_dist)
    m_obstacle_pert = false;

  bufferBackOff(m_osx, m_osy);

  // Part 3: Cache the distances mapping a particular heading 
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
    if(m_obstacle_pert) {
      double dist = m_obstacle_buff.dist_to_poly(m_osx, m_osy, heading);
      if(dist != -1) {
	min_dist = dist;
      }
    }
    m_cache_distance[i] = min_dist;
  }
  
  return(true);
}

//----------------------------------------------------------------
// Procedure: setObstacle

void AOF_AvoidObstacle::setObstacle(const XYPolygon& orig_poly)
{
  // Part 1: get the raw label of the incoming obstacle
  string incoming_label = orig_poly.get_label();
  string new_label = incoming_label + "_avd";
  
  // Part 2: assign the new obstacle, with an augmented label
  XYPolygon new_poly = orig_poly;
  new_poly.set_label(new_label);
  m_obstacle_orig = new_poly;

  // Part 3: build a new buffered obstacle, with an augmented label
  XYPolygon new_buff_poly = orig_poly;
  new_buff_poly.set_label(new_label + "_buff");
  m_obstacle_buff = new_buff_poly;

  // Part 4: by default consider the new obstacle pertinent
  m_obstacle_pert = true;
}

//----------------------------------------------------------------
// Procedure: ownshipInObstacle

bool AOF_AvoidObstacle::ownshipInObstacle(bool use_buffered)
{
  XYPolygon poly = m_obstacle_orig;
  if(use_buffered)
    poly = m_obstacle_buff;
  
  bool contained = poly.contains(m_osx, m_osy);
  if(contained) {
    m_debug_msg = "osx=" + doubleToString(m_osx);
    m_debug_msg += ",osy=" + doubleToString(m_osy);
    m_debug_msg += ",olabel=" + poly.get_label();
    m_debug_msg += ",vsize=" + uintToString(poly.size());
    m_debug_msg += ",spec=" + poly.get_spec();
  }

  return(contained);
}

//----------------------------------------------------------------
// Procedure: applyBuffer

void AOF_AvoidObstacle::applyBuffer()
{
  m_obstacle_buff = m_obstacle_orig;
  if(m_buffer_dist > 0) {
    m_obstacle_buff.grow_by_amt(m_buffer_dist);
    m_obstacle_buff.set_label(m_obstacle_orig.get_label()+"_buff");
  }    
}

//----------------------------------------------------------------
// Procedure: isObstaclePert

bool AOF_AvoidObstacle::isObstaclePert()
{
  return(m_obstacle_pert);
}

//----------------------------------------------------------------
// Procedure: distToObstaclesBuff

double AOF_AvoidObstacle::distToObstacleBuff()
{
  double dist = m_obstacle_buff.dist_to_poly(m_osx, m_osy);
  return(dist);
}

//----------------------------------------------------------------
// Procedure: distToObstaclesOrig

double AOF_AvoidObstacle::distToObstacleOrig()
{
  double dist = m_obstacle_orig.dist_to_poly(m_osx, m_osy);
  return(dist);
}

//----------------------------------------------------------------
// Procedure: bearingMinMaxToBufferPoly()

bool AOF_AvoidObstacle::bearingMinMaxToBufferPoly(double& bmin, double& bmax)
{
  if(!m_obstacle_buff.is_convex())
    return(false);
  
  return(bearingMinMaxToPoly(m_osx, m_osy, m_obstacle_buff, bmin, bmax));    
}

//----------------------------------------------------------------
// Procedure: bufferBackOff
//   Purpose: For each of the buffered obstacles, if the ownship 
//            position is within the obstacle boundary, it will 
//            shrink until ownship is no longer inside.

void AOF_AvoidObstacle::bufferBackOff(double osx, double osy)
{
  if(m_buffer_dist <= 0)
    return;

  XYPolygon orig_plus_buffer = m_obstacle_orig;
  orig_plus_buffer.grow_by_amt(m_buffer_dist);
  
  string buff_label = m_obstacle_orig.get_label() + "_buff";
  bool   os_in_orig = m_obstacle_orig.contains(osx, osy);
  bool   os_in_buff = orig_plus_buffer.contains(osx, osy);

  if(os_in_buff && !os_in_orig) {
    bool os_in_newb = false;
    for(unsigned int j=1; ((j<=100) && (!os_in_newb)); j++) {
      XYPolygon new_poly = m_obstacle_orig;
      double grow_amt = ((double)(j) / 100.0) * m_buffer_dist;
      new_poly.grow_by_amt(grow_amt);
      os_in_newb = new_poly.contains(osx, osy);
      if(!os_in_newb) {
	m_obstacle_buff = new_poly;
	m_obstacle_buff.set_label(buff_label);
      }
    }
  }

  if(!os_in_buff) {
    orig_plus_buffer.set_label(buff_label);
    m_obstacle_buff = orig_plus_buffer;
  }
  
  if(os_in_orig) {
    m_obstacle_buff = m_obstacle_orig;
    m_obstacle_buff.set_label(buff_label);
  }
  
  // In the exceptionally rare possibility that the polygon was shrunk
  // down to as far as possible and it *still* contains the ownship
  // position, we concede and drop this obstacle from the list by 
  // declaring the obsticle to be not pertinent.
  if(os_in_orig) 
    m_obstacle_pert = false;
}

//----------------------------------------------------------------
// Procedure: evalBox

double AOF_AvoidObstacle::evalBox(const IvPBox *b) const
{
  double avd_obstacles_utility  = evalAuxObstacles(b);
  double center_points_utility  = evalAuxCtrPoints(b);

  double utility = ((2*avd_obstacles_utility) + 
		    (2*center_points_utility)) / 4.0;

  return(utility);
}

//----------------------------------------------------------------
// Procedure: evalAuxObstacles

double AOF_AvoidObstacle::evalAuxObstacles(const IvPBox* b) const
{
  double max_utility = 100;
  double min_utility = 0;

  double eval_crs = 0;
  double eval_spd = 0;
  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix,0), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix,0), eval_spd);

  double utility = 0;

  int   heading_index = b->pt(m_crs_ix, 0);
  double dist_to_poly = m_cache_distance[heading_index];
  
  if(dist_to_poly == -1) 
    utility = max_utility;
  else if(eval_spd == 0)
    utility = max_utility;
  else {
    // determine time to collision w/ poly (in seconds)
    double time_to_collision = dist_to_poly / eval_spd;
    if(time_to_collision > m_allowable_ttc)
      utility = max_utility;
    else
      utility = min_utility;
  }

  return(utility);
}

//----------------------------------------------------------------
// Procedure: evalAuxCtrPoints

double AOF_AvoidObstacle::evalAuxCtrPoints(const IvPBox* b) const
{
  double max_utility = 100;
  double min_utility = 0;

  if(!m_obstacle_pert)
    return(max_utility);

  double eval_crs = 0;
  double eval_spd = 0;
  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix,0), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix,0), eval_spd);

  double utility = 0;
    
  double poly_center_x = m_obstacle_orig.get_center_x();
  double poly_center_y = m_obstacle_orig.get_center_y();
  
  // Determine if the polygon is *presently* on ownhip's port or 
  // starboard side.
  double curr_crs_bng_to_poly = relBearing(m_osx, m_osy, m_osh,
					   poly_center_x, poly_center_y);
  bool poly_on_port_curr = true;
  if(curr_crs_bng_to_poly <= 180)
    poly_on_port_curr = false;
  
  // Determine if the polygon *would be* on ownhip's port or 
  // starboard if the crs being evaluated would be true instead.
  // Note: avd_add consider vehicle dynamics
  double eval_crs_bng_to_poly = relBearing(m_osx, m_osy, eval_crs,
					   poly_center_x, poly_center_y);
  bool poly_on_port_eval = true;
  if(eval_crs_bng_to_poly <= 180)
    poly_on_port_eval = false;
  
  // If they do not match, set i_utility to min_utility
  if(poly_on_port_curr != poly_on_port_eval)
    utility = min_utility;
  else
    utility = max_utility;
    
  return(utility);
}


//----------------------------------------------------------------
// Procedure: polyIsSmall

bool AOF_AvoidObstacle::polyIsSmall(const XYPolygon& poly, 
				    double psize) const
{
  unsigned int vertices = poly.size();
  if(vertices == 0)
    return(true);
  
  for(unsigned int i=0; i<vertices; i++) {
    double x = poly.get_vx(i);
    double y = poly.get_vy(i);
    if((x > psize) || (y > psize))
      return(false);
  }

  return(true);
}
