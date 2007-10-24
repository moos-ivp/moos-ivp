/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_GENERIC.cpp                                      */
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
#include <assert.h>
#include "AOF_GENERIC.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_GENERIC::AOF_GENERIC(IvPDomain g_domain, double g_speed,
		     double g_osLAT, double g_osLON, 
		     double g_ptLAT, double g_ptLON)
  : AOF(g_domain)
{
  int crs_ix = g_domain.getIndex("course");
  int spd_ix = g_domain.getIndex("speed");
  int tol_ix = g_domain.getIndex("tol");
  
  assert(crs_ix != -1);
  assert(spd_ix != -1);
  assert(tol_ix != -1);
  
  crsDelta = g_domain.getVarDelta(crs_ix);
  crsBase  = g_domain.getVarLow(crs_ix);

  spdDelta = g_domain.getVarDelta(spd_ix);
  spdBase  = g_domain.getVarLow(spd_ix);
  spdHigh  = g_domain.getVarHigh(spd_ix);

  tolDelta = g_domain.getVarDelta(tol_ix);
  tolBase  = g_domain.getVarLow(tol_ix);
  
  osLAT = g_osLAT;
  osLON = g_osLON;
  ptLAT = g_ptLAT;
  ptLON = g_ptLON;

  desired_spd = g_speed;
  rel_angle   = relAng(osLON, osLAT, ptLON, ptLAT);
  direct_dist = hypot(osLON-ptLON, osLAT-ptLAT);
}

//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given <Course, Speed, Time-on-leg> tuple 
//               given by a 3D ptBox (b).
//            Determines naut mile Closest-Point-of-Approach (CPA)
//               and returns a value after passing it thru the 
//               metric() function.


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


double AOF_GENERIC::evalBox(const IvPBox *b) const
{
  //cout << "------------------------------------------------------" << endl;
  //cout << "osLON:" << osLON << "osLAT:" << osLAT << endl;
  //cout << "ptLON:" << ptLON << "ptLAT:" << ptLAT << endl;

  double evalCRS  = crsBase + ((double)(b->pt(0,0)) * crsDelta);
  double evalSPD  = spdBase + ((double)(b->pt(1,0)) * spdDelta);
  double evalTOL  = tolBase + ((double)(b->pt(2,0)) * tolDelta);
  
  if(evalTOL <= 0)
    return(0);

  if(evalCRS < 0) evalCRS += 360.0;
  double radCRS = degToRadians(evalCRS);
  double newLAT = (cos(radCRS) * (evalSPD * evalTOL)) + osLAT;
  double newLON = (sin(radCRS) * (evalSPD * evalTOL)) + osLON;

  double dist1  = hypot(osLON-newLON, osLAT-newLAT); 
  double dist2  = hypot(newLON-ptLON, newLAT-ptLAT);

  double indirect_dist = dist1 + dist2;
  double progress_dist = direct_dist - dist2;

  double detour_dist = indirect_dist - direct_dist;
  double progress_rate = progress_dist / evalTOL;

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

double AOF_GENERIC::metric(double detour_dist, 
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

  double speed_pct = (progress_rate / spdHigh);
#if 0
  if(speed_pct > 1.0) {
    cout << "speed_pct: " << speed_pct << endl;
    cout << "progress_rate: " << progress_rate << endl;
    cout << "spdHigh: " << spdHigh << endl;
    exit(0);
  }
#endif

  double combined_pct = (0.5 * dist_pct) + (0.5 * speed_pct);

  return(combined_pct * 100.0);
}








