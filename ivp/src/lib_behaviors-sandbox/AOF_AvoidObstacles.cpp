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
  buffer_dist     = 0;
  buffer_applied  = false;
  activation_dist = -1;
  allowable_ttc   = 20;  // time-to-collision in seconds

  os_x_set        = false;
  os_y_set        = false;
  activation_dist_set = false;
  allowable_ttc_set = false;
  
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
    buffer_dist = param_val;
    buffer_applied = false;
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
  if(param == "polygon") {
    XYPolygon new_polygon = stringToPoly(param_val);
    if(!new_polygon.is_convex())
      return(false);
    obstacles.push_back(new_polygon);
    obstacles_buff.push_back(new_polygon);
    return(true);
  }
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: applyBuffer

void AOF_AvoidObstacles::applyBuffer()
{
  obstacles_buff.clear();

  int vsize = obstacles.size();
  for(int i=0; i<vsize; i++) {
    XYPolygon new_poly = obstacles[i];
    new_poly.grow_by_amt(buffer_dist);
    obstacles_buff.push_back(new_poly);
  }
  buffer_applied = true;
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
  
  if(obstacles.size() == 0)
    return(false);

  if(!buffer_applied)
    applyBuffer();

  // Fill in a cache of distances mapping a particular heading to
  // the minimum/closest distance to any of the obstacle polygons.
  // A distance of -1 indicates infinite distance.

  int  i, j;
  int  osize = obstacles_buff.size();
  int  hsize = m_domain.getVarPoints(crs_ix);
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
      double position_dist_to_poly = obstacles_buff[j].dist_to_poly(os_x, os_y);
      if(position_dist_to_poly < activation_dist) {
	dist_to_poly = obstacles_buff[j].dist_to_poly(os_x, os_y, heading);
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

int AOF_AvoidObstacles::obstaclesInRange()
{
  int  vsize = obstacles_buff.size();
  int  count = 0;

  for(int i=0; i<vsize; i++) {
    double position_dist_to_poly = obstacles_buff[i].dist_to_poly(os_x, os_y);
    cout << "pdist: " << position_dist_to_poly << "  adist: " << activation_dist << endl;
    if(position_dist_to_poly < activation_dist)
      count++;
  }
  cout << "vsize: " << vsize << "  count: " << count << endl << endl;
  return(count);
}

//----------------------------------------------------------------
// Procedure: evalBox

double AOF_AvoidObstacles::evalBox(const IvPBox *b) const
{
  double max_utility = 100;
  double min_utility = 0;

  int osize = obstacles_buff.size();
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








