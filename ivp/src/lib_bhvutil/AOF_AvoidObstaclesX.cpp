/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_AvoidObstaclesX.cpp                               */
/*    DATE: Aug 2nd, 2006                                        */
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
#include "AOF_AvoidObstaclesX.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_AvoidObstaclesX::AOF_AvoidObstaclesX(IvPDomain gdomain) : AOF(gdomain)
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

bool AOF_AvoidObstaclesX::setParam(const string& param, double param_val)
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
  else if((param == "os_h") ||
	  (param == "present_heading")) {  // deprecated
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

bool AOF_AvoidObstaclesX::setParam(const string& param, 
				  const string& param_val)
{
  return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_AvoidObstaclesX::initialize()
{
  // Part 1: Sanity Checks
  unsigned int object_cnt = m_obstacles_orig.size();
  if(object_cnt == 0)
    return(false);
  if(m_obstacles_buff.size() != object_cnt)
    return(false);
  if(m_obstacles_pert.size() != object_cnt)
    return(false);
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

bool AOF_AvoidObstaclesX::postInitialize()
{
  // Part 1: Sanity Checks
  unsigned int object_cnt = m_obstacles_orig.size();
  if(object_cnt == 0)
    return(false);

  // Part 2: Figure out and cache which obstacles are pertinent
  for(unsigned int i=0; i<object_cnt; i++) {
    m_obstacles_pert[i] = true;
    if(polyAft(m_osx, m_osy, m_osh, m_obstacles_buff[i]))
      m_obstacles_pert[i] = false;
    if(m_obstacles_buff[i].dist_to_poly(m_osx, m_osy) > m_activation_dist)
      m_obstacles_pert[i] = false;
  }
  
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
    for(unsigned int j=0; j<object_cnt; j++) {
      if(m_obstacles_pert[j]) {
	double dist = m_obstacles_buff[j].dist_to_poly(m_osx, m_osy, heading);
	if(dist != -1) {
	  if((min_dist==-1) || (dist < min_dist))
	    min_dist = dist;
	}
      }
    }
    m_cache_distance[i] = min_dist;
  }
  
  return(true);
}

//----------------------------------------------------------------
// Procedure: obstaclesInRange
//      Note: Return the number of obstacles within a given range

unsigned int AOF_AvoidObstaclesX::obstaclesInRange()
{
  unsigned int  vsize = m_obstacles_buff.size();
  unsigned int  count = 0;

  for(unsigned int i=0; i<vsize; i++) {
    double position_dist_to_poly = m_obstacles_buff[i].dist_to_poly(m_osx, m_osy);
    if(position_dist_to_poly < m_activation_dist)
      count++;
  }
  return(count);
}

//----------------------------------------------------------------
// Procedure: addObstacle

void AOF_AvoidObstaclesX::addObstacle(const XYPolygon& new_poly)
{
  XYPolygon new_buff_poly = new_poly;
  //new_buff_poly.grow_by_amt(m_buffer_dist);

  string new_poly_label = new_poly.get_label();
  
  string new_buff_poly_label = new_poly_label + "_buff";
  new_buff_poly.set_label(new_buff_poly_label);
  
  m_obstacles_orig.push_back(new_poly);
  m_obstacles_buff.push_back(new_buff_poly);
  m_obstacles_pert.push_back(true);
}

//----------------------------------------------------------------
// Procedure: ownshipInObstacle

bool AOF_AvoidObstaclesX::ownshipInObstacle(bool use_buffered)
{
  unsigned int vsize = m_obstacles_orig.size();
  for(unsigned int i=0; i<vsize; i++) {
    if(ownshipInObstacle(i, use_buffered)) {
      m_debug_msg = "osx=" + doubleToString(m_osx);
      m_debug_msg += ",osy=" + doubleToString(m_osy);
      m_debug_msg += ",olabel=" + m_obstacles_buff[i].get_label();
      m_debug_msg += ",vsize=" + uintToString(m_obstacles_buff[i].size());
      m_debug_msg += ",spec=" + m_obstacles_buff[i].get_spec();
      return(true);
    }
  }
  return(false);
}

//----------------------------------------------------------------
// Procedure: ownshipInObstacle

bool AOF_AvoidObstaclesX::ownshipInObstacle(unsigned int ix, 
					   bool use_buffered)
{
  if(!use_buffered) {
    if(m_obstacles_orig[ix].contains(m_osx, m_osy))
      return(true);
  }
  else {
    if(m_obstacles_buff[ix].contains(m_osx, m_osy))
      return(true);
  }
  return(false);
}

//----------------------------------------------------------------
// Procedure: pertObstacleCount
//      Note: 

unsigned int AOF_AvoidObstaclesX::pertObstacleCount()
{
  unsigned count = 0;
  unsigned int i, vsize = m_obstacles_pert.size();
  for(i=0; i<vsize; i++)
    if(m_obstacles_pert[i])
      count++;
  return(count);
}


//----------------------------------------------------------------
// Procedure: applyBuffer

void AOF_AvoidObstaclesX::applyBuffer()
{
  unsigned int vsize = m_obstacles_orig.size();
  for(unsigned int i=0; i<vsize; i++) {
    m_obstacles_buff[i] = m_obstacles_orig[i];
    if(m_buffer_dist > 0)
      m_obstacles_buff[i].grow_by_amt(m_buffer_dist);
    m_obstacles_buff[i].set_label(m_obstacles_orig[i].get_label()+"_buff");
  }    
}

//----------------------------------------------------------------
// Procedure: isObstaclePert

bool AOF_AvoidObstaclesX::isObstaclePert(unsigned int ix)
{
  if(ix >= m_obstacles_pert.size()) 
    return(false);
  return(m_obstacles_pert[ix]);
}

//----------------------------------------------------------------
// Procedure: distToObstaclesBuff

double AOF_AvoidObstaclesX::distToObstaclesBuff()
{
  double min_dist = 0;
  for(unsigned int i=0; i<m_obstacles_buff.size(); i++) {
    double dist = m_obstacles_buff[i].dist_to_poly(m_osx, m_osy);
    if((i==0) || (dist < min_dist))
      min_dist = dist;
  }

  return(min_dist);
}

//----------------------------------------------------------------
// Procedure: distToObstaclesOrig

double AOF_AvoidObstaclesX::distToObstaclesOrig()
{
  double min_dist = 0;
  for(unsigned int i=0; i<m_obstacles_orig.size(); i++) {
    double dist = m_obstacles_orig[i].dist_to_poly(m_osx, m_osy);
    if((i==0) || (dist < min_dist))
      min_dist = dist;
  }
  
  return(min_dist);
}

//----------------------------------------------------------------
// Procedure: bearingMinMaxToBufferPoly()

bool AOF_AvoidObstaclesX::bearingMinMaxToBufferPoly(double& bmin, double& bmax)
{
  if(m_obstacles_buff.size() != 1)
    return(false);

  return(bearingMinMaxToPoly(m_osx, m_osy, m_obstacles_buff[0], bmin, bmax));    
}

//----------------------------------------------------------------
// Procedure: getObstacleOrig

XYPolygon AOF_AvoidObstaclesX::getObstacleOrig(unsigned int ix)
{
  if(ix >= m_obstacles_orig.size()) {
    XYPolygon null_poly;
    return(null_poly);
  }
  return(m_obstacles_orig[ix]);
}

//----------------------------------------------------------------
// Procedure: getObstacleBuff

XYPolygon AOF_AvoidObstaclesX::getObstacleBuff(unsigned int ix)
{
  if(ix >= m_obstacles_buff.size()) {
    XYPolygon null_poly;
    return(null_poly);
  }
  return(m_obstacles_buff[ix]);
}

//----------------------------------------------------------------
// Procedure: bufferBackOff
//   Purpose: For each of the buffered obstacles, if the ownship 
//            position is within the obstacle boundary, it will 
//            shrink until ownship is no longer inside.

void AOF_AvoidObstaclesX::bufferBackOff(double osx, double osy)
{
  if(m_buffer_dist <= 0)
    return;

  unsigned int i, j, vsize = m_obstacles_orig.size();
  for(i=0; i<vsize; i++) {
    XYPolygon orig_plus_buffer = m_obstacles_orig[i];
    orig_plus_buffer.grow_by_amt(m_buffer_dist);
    
    string buff_label = m_obstacles_orig[i].get_label() + "_buff";
    bool   os_in_orig = m_obstacles_orig[i].contains(osx, osy);
    bool   os_in_buff = orig_plus_buffer.contains(osx, osy);

    if(os_in_buff && !os_in_orig) {
      bool os_in_newb = false;
      for(j=1; ((j<=100) && (!os_in_newb)); j++) {
	XYPolygon new_poly = m_obstacles_orig[i];
	double grow_amt = ((double)(j) / 100.0) * m_buffer_dist;
	new_poly.grow_by_amt(grow_amt);
	os_in_newb = new_poly.contains(osx, osy);
	if(!os_in_newb) {
	  m_obstacles_buff[i] = new_poly;
	  m_obstacles_buff[i].set_label(buff_label);
	}
      }
    }

    if(!os_in_buff) {
      orig_plus_buffer.set_label(buff_label);
      m_obstacles_buff[i] = orig_plus_buffer;
    }

    if(os_in_orig) {
      m_obstacles_buff[i] = m_obstacles_orig[i];
      m_obstacles_buff[i].set_label(buff_label);
    }

    // In the exceptionally rare possibility that the polygon was shrunk
    // down to as far as possible and it *still* contains the ownship
    // position, we concede and drop this obstacle from the list by 
    // clearing the buffered obstacle - detectable by size() == 0
    if(os_in_orig) 
      m_obstacles_pert[i] = false;
  }
}

//----------------------------------------------------------------
// Procedure: evalBox

double AOF_AvoidObstaclesX::evalBox(const IvPBox *b) const
{
  double avd_obstacles_utility  = evalAuxObstacles(b);
  double center_points_utility  = evalAuxCtrPoints(b);

  double utility = ((2*avd_obstacles_utility) + 
		    (2*center_points_utility)) / 4.0;

  return(utility);
}

//----------------------------------------------------------------
// Procedure: evalAuxObstacles

double AOF_AvoidObstaclesX::evalAuxObstacles(const IvPBox* b) const
{
  double max_utility = 100;
  double min_utility = 0;

  unsigned i, osize = m_obstacles_buff.size();
  if(osize == 0)
    return(max_utility);
  
  double eval_crs = 0;
  double eval_spd = 0;
  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix,0), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix,0), eval_spd);

  double lowest_utility = 0;
  for(i=0; i<osize; i++) {
    double i_utility = 0;

    int   heading_index = b->pt(m_crs_ix, 0);
    double dist_to_poly = m_cache_distance[heading_index];

    if(dist_to_poly == -1) 
      i_utility = max_utility;
    else if(eval_spd == 0)
     i_utility = max_utility;
    else {
      // determine time to collision w/ poly (in seconds)
      double time_to_collision = dist_to_poly / eval_spd;
      if(time_to_collision > m_allowable_ttc)
	i_utility = max_utility;
      else
	i_utility = min_utility;
    }
    if((i==0) || (i_utility < lowest_utility))
      lowest_utility = i_utility;
  }
      
  double utility = lowest_utility;

  return(utility);
}

//----------------------------------------------------------------
// Procedure: evalAuxCtrPoints

double AOF_AvoidObstaclesX::evalAuxCtrPoints(const IvPBox* b) const
{
  double max_utility = 100;
  double min_utility = 0;

  unsigned i, osize = m_obstacles_buff.size();
  if(osize == 0)
    return(max_utility);
  
  double eval_crs = 0;
  double eval_spd = 0;
  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix,0), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix,0), eval_spd);

  double lowest_utility = max_utility;
  for(i=0; i<osize; i++) {
    if(m_obstacles_pert[i]) {
      double i_utility = 0;

      double poly_center_x = m_obstacles_orig[i].get_center_x();
      double poly_center_y = m_obstacles_orig[i].get_center_y();
      
      // Determine if the polygon is *presently* on ownhip's port or 
      // starboard side.
      double curr_crs_bng_to_poly = relBearing(m_osx, m_osy, m_osh,
					       poly_center_x, poly_center_y);
      bool poly_on_port_curr = true;
      if(curr_crs_bng_to_poly <= 180)
	poly_on_port_curr = false;
      
      // Determine if the polygon *would be* on ownhip's port or 
      // starboard if the crs being evaluated would be true instead.
      double eval_crs_bng_to_poly = relBearing(m_osx, m_osy, eval_crs,
					       poly_center_x, poly_center_y);
      bool poly_on_port_eval = true;
      if(eval_crs_bng_to_poly <= 180)
	poly_on_port_eval = false;
      
      // If they do not match, set i_utility to min_utility
      if(poly_on_port_curr != poly_on_port_eval)
	i_utility = min_utility;
      else
	i_utility = max_utility;
      
      if((i==0) || (i_utility < lowest_utility))
	lowest_utility = i_utility;
    }
  }
      
  double utility = lowest_utility;

  return(utility);
}


//----------------------------------------------------------------
// Procedure: polyIsSmall

bool AOF_AvoidObstaclesX::polyIsSmall(const XYPolygon& poly, 
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
