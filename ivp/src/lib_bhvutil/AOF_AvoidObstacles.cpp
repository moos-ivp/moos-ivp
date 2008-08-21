/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_AvoidObstacles.cpp                               */
/*    DATE: Aug 2nd, 2006                                        */
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

#include <iostream> 
#include <math.h>
#include <string>
#include "AOF_AvoidObstacles.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "XYBuildUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_AvoidObstacles::AOF_AvoidObstacles(IvPDomain gdomain) : AOF(gdomain)
{
  crs_ix = gdomain.getIndex("course");
  spd_ix = gdomain.getIndex("speed");

  os_x            = 0;
  os_y            = 0;
  activation_dist = -1;
  allowable_ttc   = 20;  // time-to-collision in seconds

  os_x_set        = false;
  os_y_set        = false;
  activation_dist_set = false;
  allowable_ttc_set   = false;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_AvoidObstacles::setParam(const string& param, double param_val)
{
  if(param == "os_y") {
    os_y = param_val;
    os_y_set = true;
    return(true);
  }
  else if(param == "os_x") {
    os_x = param_val;
    os_x_set = true;
    return(true);
  }
  else if(param == "allowable_ttc") {
    if(param_val <= 0)
      return(false);
    allowable_ttc = param_val;
    allowable_ttc_set = true;
    return(true);
  }
  else if(param == "activation_dist") {
    if(param_val <= 0)
      return(false);
    activation_dist = param_val;
    activation_dist_set = true;
    return(true);
  }
  else if(param == "buffer_dist") {
    if(param_val < 0)
      return(false);
    if(m_buffer_dist != param_val) {
      m_buffer_dist = param_val;
      applyBuffer();
    }
    return(true);
  }
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_AvoidObstacles::setParam(const string& param, 
				  const string& param_val)
{
  return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_AvoidObstacles::initialize()
{
  if((crs_ix==-1) || (spd_ix==-1))
    return(false);
  if((!os_x_set) || (!os_y_set))
    return(false);
  if(!allowable_ttc_set)
    return(false);
  if(!activation_dist_set)
    return(false);
  
  if(m_obstacles_buff.size() == 0)
    return(false);

  bufferBackOff(os_x, os_y);

  // Fill in a cache of distances mapping a particular heading to
  // the minimum/closest distance to any of the obstacle polygons.
  // A distance of -1 indicates infinite distance.

  unsigned int  i, j;
  unsigned int  osize = m_obstacles_buff.size();
  unsigned int  hsize = m_domain.getVarPoints(crs_ix);
  for(i=0; i<hsize; i++)
    cache_distance.push_back(-1);

  double heading, dist_to_poly; 
  for(i=0; i<hsize; i++) {
    bool ok = m_domain.getVal(crs_ix, i, heading);
    if(!ok)
      return(false);
    double min_dist = -1; 
    bool   min_dist_set = false;
    for(j=0; j<osize; j++) {
      double position_dist_to_poly = m_obstacles_buff[j].dist_to_poly(os_x, os_y);
      if(position_dist_to_poly < activation_dist) {
	dist_to_poly = m_obstacles_buff[j].dist_to_poly(os_x, os_y, heading);
	if(dist_to_poly != -1) {
	  if(!min_dist_set || (dist_to_poly < min_dist))
	    min_dist = dist_to_poly;
	}
      }
    }
    cache_distance[i] = min_dist;
  }
  
  return(true);
}

//----------------------------------------------------------------
// Procedure: obstaclesInRange
//      Note: Return the number of obstacles within a given range

unsigned int AOF_AvoidObstacles::obstaclesInRange()
{
  unsigned int  vsize = m_obstacles_buff.size();
  unsigned int  count = 0;

  for(unsigned int i=0; i<vsize; i++) {
    double position_dist_to_poly = m_obstacles_buff[i].dist_to_poly(os_x, os_y);
    //cout << "pdist: " << position_dist_to_poly << "  adist: " << activation_dist << endl;
    if(position_dist_to_poly < activation_dist)
      count++;
  }
  //cout << "vsize: " << vsize << "  count: " << count << endl << endl;
  return(count);
}

//----------------------------------------------------------------
// Procedure: addObstacle

void AOF_AvoidObstacles::addObstacle(const XYPolygon& new_poly)
{
  XYPolygon new_buff_poly = new_poly;
  new_buff_poly.grow_by_amt(m_buffer_dist);

  string new_poly_label = new_poly.get_label();
  
  string new_buff_poly_label = new_poly_label + "_buff";
  new_buff_poly.set_label(new_buff_poly_label);
  
  m_obstacles_orig.push_back(new_poly);
  m_obstacles_buff.push_back(new_buff_poly);
}


//----------------------------------------------------------------
// Procedure: objectInObstacle

bool AOF_AvoidObstacles::objectInObstacle(double dx, double dy, 
					  bool use_buffered)
{
  int ix = objectInWhichObstacle(dx, dy, use_buffered);
  if(ix == -1)
    return(false);
  else
    return(true);
}

//----------------------------------------------------------------
// Procedure: objectInWhichObstacle
//      Note: Checks to see if the given point is within any of the
//            obstacles. If so, the index of the object is returned.
//            Otherwise -1 is returned. 

int AOF_AvoidObstacles::objectInWhichObstacle(double dx, double dy, 
					      bool use_buffered)
{
  if(!use_buffered) {
    int vsize = m_obstacles_orig.size();
    for(int i=0; i<vsize; i++) {
      if(m_obstacles_orig[i].contains(dx, dy))
	return(i);
    }
  }
  else {
    int vsize = m_obstacles_buff.size();
    for(int i=0; i<vsize; i++) {
      if(m_obstacles_buff[i].contains(dx, dy))
	return(i);
    }
  }

  return(-1);
}


//----------------------------------------------------------------
// Procedure: applyBuffer

void AOF_AvoidObstacles::applyBuffer()
{
  int vsize = m_obstacles_orig.size();
  for(int i=0; i<vsize; i++) {
    m_obstacles_buff[i] = m_obstacles_orig[i];
    m_obstacles_buff[i].grow_by_amt(m_buffer_dist);
  }    
}

//----------------------------------------------------------------
// Procedure: getObstacleSpec

string AOF_AvoidObstacles::getObstacleSpec(int ix, bool use_buffered)
{
  if((ix < 0) || (ix >= m_obstacles_buff.size()))
    return("");

  if(use_buffered)
    return(m_obstacles_buff[ix].get_spec(0));
  else
    return(m_obstacles_orig[ix].get_spec(0));
}

//----------------------------------------------------------------
// Procedure: bufferBackOff
//            For each of the buffered obstacles, if the ownship 
//            position is within the obstacle boundary, it will 
//            shrink until ownship is no longer inside.

void AOF_AvoidObstacles::bufferBackOff(double osx, double osy)
{
  int vsize = m_obstacles_buff.size();
  for(int i=0; i<vsize; i++) {
    bool contained = m_obstacles_buff[i].contains(osx, osy);
    
    double shrink_pct = 0.02;
    while((contained) && (shrink_pct < 100.0)) {
      XYPolygon new_poly = m_obstacles_buff[i];
      new_poly.grow_by_pct(-1*shrink_pct);
      contained = new_poly.contains(osx, osy);
      if(!contained) 
	m_obstacles_buff[i] = new_poly;
      else
	shrink_pct += 0.02;
    }

    // In the exceptionally rare possibility that the polygon was shrunk
    // down to as far as possible and it *still* contains the ownship
    // position, we concede and drop this obstacle from the list by 
    // clearing the buffered obstacle - detectable by size() == 0
    if(contained) 
      m_obstacles_buff[i].clear();
  }
}

//----------------------------------------------------------------
// Procedure: evalBox

double AOF_AvoidObstacles::evalBox(const IvPBox *b) const
{
  double max_utility = 100;
  double min_utility = 0;

  int osize = m_obstacles_buff.size();
  if(osize == 0)
    return(max_utility);
  
  double eval_crs = 0;
  double eval_spd = 0;
  
  m_domain.getVal(crs_ix, b->pt(crs_ix,0), eval_crs);
  m_domain.getVal(spd_ix, b->pt(spd_ix,0), eval_spd);

  double lowest_utility;
  for(int i=0; i<osize; i++) {
    double i_utility;

    int   heading_index = b->pt(crs_ix, 0);
    double dist_to_poly = cache_distance[heading_index];

    if(dist_to_poly == -1) 
      i_utility = max_utility;
    else {
      // determine time to collision w/ poly (in seconds)
      double time_to_collision = dist_to_poly / eval_spd;
      
      if(time_to_collision > allowable_ttc)
	i_utility = max_utility;
      else
	i_utility = min_utility;
    }
    if((i==0) || (i_utility < lowest_utility))
      lowest_utility = i_utility;
  }
      
  double utility = lowest_utility - (eval_spd * 0.00001);

  return(utility);
}








