/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AOF_R16.cpp                                          */
/*    DATE: Feb 9th, 2001 (CPA logic written)                    */
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

#include <cmath> 
#include <cstdlib> 
#include "AOF_R16.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_R16::AOF_R16(IvPDomain gdomain) : AOF_Contact(gdomain)
{
  m_crs_ix = gdomain.getIndex("course");
  m_spd_ix = gdomain.getIndex("speed");

  m_pass_to_stern    = false;
  m_pass_to_bow      = false;
  m_passing_side_set = false;

  m_osh_orig = 90;
  m_osh_orig_set = false;

  // If ok_cn_bow_cross_dist is zero, this means
  m_ok_cn_bow_cross_dist = 0;
  
  m_osh = 0;
  m_osh_set = false;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_R16::setParam(const string& param, double param_val)
{
  if(param == "osh") {
    m_osh = angle360(param_val);
    m_osh_set = true;
    return(true);
  }
  
  if(param == "osh_orig") {
    // kw added to prevent turns to port qtr prevents turns to port
    // quarter in giveway (long turns to right, or worse, turns to
    // left to reach the port quarter hot spot on IPF
    m_osh_orig = angle360(param_val);
    m_osh_orig_set = true;
    return(true);
  }

  return(AOF_Contact::setParam(param, param_val));
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_R16::setParam(const string& param, const string& param_val)
{
  if(AOF_Contact::setParam(param, param_val))
    return(true);
  
  if(param == "passing_side") {
    m_passing_side_set = true;
    if(param_val == "stern") {
      m_pass_to_stern    = true;
      m_pass_to_bow      = false;
    }
    else if(param_val == "bow") {
      m_pass_to_stern    = false;
      m_pass_to_bow      = true;
    }
    else {
      m_passing_side_set = false;      
      string msg = "bad param[" + param + "], value[" + param_val + "]";
      postMsgAOF(msg);    
      return(false);
    }
  }
  else if(param == "ok_cn_bow_cross_dist") {
    bool ok = setNonNegDoubleOnString(m_ok_cn_bow_cross_dist, param_val);
    if(!ok) {
      string msg = "bad param[" + param + "], value[" + param_val + "]";
      postMsgAOF(msg);
    }
    return(ok);
  }
  else {
    string msg = "bad param[" + param + "], value[" + param_val + "]";
    postMsgAOF(msg);    
    return(false);
  }

  return(true);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_R16::initialize()
{
  if(AOF_Contact::initialize() == false) {
    postMsgAOF("crs_ix or spd_ix not set");
    return(false);
  }
  
  if((m_crs_ix==-1) || (m_spd_ix==-1)) {
    postMsgAOF("crs_ix or spd_ix not set");
    return(false);
  }

  if(!m_passing_side_set) {
    postMsgAOF("passing side is not set");
    return(false);
  }
    
  if(!m_osh_set) {
    postMsgAOF("osh is not set");
    return(false);
  }
  
  if(m_pass_to_stern && m_pass_to_bow) {
    postMsgAOF("pass_to_stern or pass_to_bow is not set");
    return(false);
  }

  return(true);
}

//----------------------------------------------------------------
// Procedure: evalbox
//   Purpose: Evaluates a given <Course, Speed> tuple 
//               given by a 3D ptBox (b).
//            Determines naut mile Closest-Point-of-Approach (CPA)
//               and returns a value after passing it thru the 
//               metric() function.

double AOF_R16::evalBox(const IvPBox *b) const 
{
  // Part 1: Determine the course and speed being evaluated.
  double eval_crs, eval_spd;
  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix), eval_spd);

  // Part 2: Disallow certain maneuvers if crossing cn stern
  // If we're passing to stern, penalize for crossing the bow
  if(m_pass_to_stern) {

    // Rule out turns that cross the bow, plain and simple
    if(m_cpa_engine.crossesBow(eval_crs, eval_spd))
      return(0);

    // If a left-hand turn, only allow turns that still cross cn stern
    bool os_turns_rgt = m_cpa_engine.turnsRight(m_osh, eval_crs);
    if(!os_turns_rgt) {
      if(!m_cpa_engine.crossesStern(eval_crs, eval_spd))
	return(0);
    }
  }
    
    // Part 3: Disallow certain maneuvers if crossing cn bow
  // If we're passing to bow, penalize for crossing the stern
  if(m_pass_to_bow) {
    if(m_cpa_engine.turnsRight(m_osh, eval_crs)) {
      if(!m_cpa_engine.crossesBow(eval_crs, eval_spd))
	return(0);
      
      if(m_ok_cn_bow_cross_dist > 0) {
	double cn_bow_cross_dist = m_cpa_engine.crossesBowDist(eval_crs,eval_spd);
	if(cn_bow_cross_dist <= m_ok_cn_bow_cross_dist)
	  return(0);
      }
    }
  }

  // Part 4: Determine the raw CPA distance that would result from the 
  // given course and speed and configured time-on-leg.
  double cpa_dist = m_cpa_engine.evalCPA(eval_crs, eval_spd, m_tol);
 
  // Part 5: Determine the utility of the calculated CPA distance based
  // on the tolerance ranges given by m_collision_dist and m_all_clear_dist
  double utility = metricCPA(cpa_dist);

  return(utility);
}


//----------------------------------------------------------------
// Procedure: metricCPA

double AOF_R16::metricCPA(double eval_dist) const
{
  double min_dist = m_collision_distance;
  double max_dist = m_all_clear_distance;

  if(eval_dist < min_dist) 
    return(0);
  if(eval_dist > max_dist) 
    return(100);
  
  double range = max_dist - min_dist;
  if(range == 0)
    return(0);

  double tween = 25.0 + 75.0 * (eval_dist - min_dist) / range;
  return(tween);
}



//----------------------------------------------------------------
// Procedure: metricCRX
//      Note: We convert a CPA distance to a utility value. The 
//            range is between 0 and 100, but it is not important
//            since the IvP problems solver normalizes the range
//            output from each behavior before applying priority
//            weights.
//
//             ^                               
//             |                             --------------------- 1
//             |                           /                      
//             |                        /    |                    
//             |                     /                            
//             |                  /          |                    
//             |               /                                  
//  drop_val   |            /                |                    
//          ---|         /                                        
//             |       |                     |                   
//             |       |                                          
//             |       |                     |                  
//             |_______|__________________________________________ 0
//                     |                     |                             
//                    min_dist              max_dist                       
//                                                                
//
//   Returns: A utlity value in the range [0, 1]


double AOF_R16::metricCRX(double dist, double min_dist, double max_dist) const
{
  
  double min_util = 0.0;
  double max_util = 1.0;

  if(dist < min_dist) 
    return(min_util);
  if(dist > max_dist) 
    return(max_util);

  double drop_val   = 0.2;
  double return_val = 0;

  double range = max_dist - min_dist;
  if(range <= 0)
    return(0);

  return_val = drop_val;
  return_val += (max_util - drop_val) * (dist - min_dist) / range;

  return(return_val);
}






