/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_WPT3D.cpp                                        */
/*    DATE: Nov 2004                                             */
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
#include "AOF_WPT3D.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_WPT3D::AOF_WPT3D(IvPDomain g_domain) : AOF(g_domain)
{
  crs_ix = g_domain.getIndex("course");
  spd_ix = g_domain.getIndex("speed");
  tol_ix = g_domain.getIndex("tol");
  
  os_lat_set      = false;
  os_lon_set      = false;
  pt_lat_set      = false;
  pt_lon_set      = false;
  desired_spd_set = false;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_WPT3D::setParam(const string& param, double param_val)
{
  if(param == "oslat") {
    os_lat = param_val;
    os_lat_set = true;
    return(true);
  }
  else if(param == "oslon") {
    os_lon = param_val;
    os_lon_set = true;
    return(true);
  }
  else if(param == "ptlat") {
    pt_lat = param_val;
    pt_lat_set = true;
    return(true);
  }
  else if(param == "ptlon") {
    pt_lon = param_val;
    pt_lon_set = true;
    return(true);
  }
  else if(param == "desired_speed") {
    desired_spd = param_val;
    desired_spd_set = true;
    return(true);
  }
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_WPT3D::initialize()
{
  if((crs_ix==-1)||(spd_ix==-1)||(tol_ix==-1))
    return(false);

  if(!os_lat_set || !os_lon_set || !pt_lat_set || !pt_lon_set)
    return(false);

  if(!desired_spd_set)
    return(false);

  direct_dist = hypot((os_lon - pt_lon), (os_lat - pt_lat));
  speed_high  = domain.getVarHigh(spd_ix);
  return(true);
}


//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given <Course, Speed, Time-on-leg> tuple 
//               given by a 3D ptBox (b).
//            Determines naut mile Closest-Point-of-Approach (CPA)
//               and returns a value after passing it thru the 
//               metric() function.
//
//                                                                
//                  (ptX,ptY)  o                          
//                            / \                        
//                           /   \  "dist2"                 
//                          /     \                            
//                         /       \                         
//                        /         \                 
//          "direct_dist"/           \                
//                      /             o (newX, newY) 
//                     /            /              
//                    /          /                       
//                   /        /                           
//                  /      /                          
//                 /    /    "dist1"                 
//                /  /                                
//    (osX,osY)  o                             
//                                                
//                                                                
//                                                                

double AOF_WPT3D::evalBox(const IvPBox *b) const
{
  double eval_crs = 0;
  double eval_spd = 0;
  double eval_tol = 0;

  domain.getVal(crs_ix, b->pt(crs_ix,0), eval_crs);
  domain.getVal(spd_ix, b->pt(spd_ix,0), eval_spd);
  domain.getVal(tol_ix, b->pt(tol_ix,0), eval_tol);

  if(eval_tol <= 0)
    return(0);

  if(eval_crs < 0) eval_crs += 360.0;
  double rad_crs = degToRadians(eval_crs);
  double new_lat = (cos(rad_crs) * (eval_spd * eval_tol)) + os_lat;
  double new_lon = (sin(rad_crs) * (eval_spd * eval_tol)) + os_lon;

  double dist1  = hypot((os_lon - new_lon), (os_lat - new_lat)); 
  double dist2  = hypot((new_lon - pt_lon), (new_lat - pt_lat));

  double indirect_dist = dist1 + dist2;
  double progress_dist = direct_dist - dist2;

  double detour_dist = indirect_dist - direct_dist;
  double progress_rate = progress_dist / eval_tol;

  //assert(progress_rate >= 0);

  double val = metric(detour_dist, progress_rate);
  return(val);
}

//----------------------------------------------------------------
// Procedure: metric
//                                                   
// 100 |\                                                 
//     |  \          K = (3 * direct_distance)                                      
//     |    \                                              
//     |      \                                            
//     |        \                                          
//     |          \
//   0 |____________\___________________                                                 
//     0            K
//          detour_distance

double AOF_WPT3D::metric(double detour_dist, 
			 double progress_rate) const
{
  // Handle oddball cases reasonably
  if(direct_dist == 0) 
    return(-detour_dist);

  double dist_pct = detour_dist / (3 * direct_dist);
  if(dist_pct > 1.0) 
    dist_pct = 1.0;
  dist_pct = (1.0 - dist_pct);
  //  dist_pct = dist_pct * dist_pct;

  double speed_pct = (progress_rate / speed_high);
#if 0
  if(speed_pct > 1.0) {
    cout << "speed_pct: " << speed_pct << endl;
    cout << "progress_rate: " << progress_rate << endl;
    cout << "speed high: " << speed_high << endl;
    exit(0);
  }
#endif

  double combined_pct = (0.5 * dist_pct) + (0.5 * speed_pct);

  return(combined_pct * 100.0);
}








