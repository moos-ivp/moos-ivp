/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_R13.cpp                                          */
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

#include <iostream>
#include <cmath> 
#include "AOF_R13.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_R13::AOF_R13(IvPDomain gdomain) : AOF_Contact(gdomain)
{
  m_crs_ix = gdomain.getIndex("course");
  m_spd_ix = gdomain.getIndex("speed");

  m_pass_to_port     = true;
  m_passing_side_set = false;
  m_port_of_contact  = false;
}


//----------------------------------------------------------------
// Procedure: setParam

bool AOF_R13::setParam(const string& param, const string& param_val)
{
 if(AOF_Contact::setParam(param, param_val))
    return(true);

  if(param == "passing_side") {
    if(param_val == "port") {
      m_passing_side_set = true;
      m_pass_to_port     = true;
    }
    else if(param_val == "starboard") {
      m_passing_side_set = true;
      m_pass_to_port     = false;
    }
    else
      return(false);
  }
  else
    return(false);

  return(true);
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_R13::setParam(const string& param, double param_val)
{
  return(AOF_Contact::setParam(param, param_val));
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_R13::initialize()
{
  if(AOF_Contact::initialize() == false)
    return(false);

  if((m_crs_ix==-1) || (m_spd_ix==-1))
    return(false);

  if(!m_passing_side_set)
    return(false);
  
  // Calculate this here since it's the same for all evaluations
  m_port_of_contact  = m_cpa_engine.portOfContact();
  m_cn_to_os_rel_bng = m_cpa_engine.contactOwnshipRelBearing();
  
  return(true);
}
 
//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given <Course, Speed, Time-on-leg> tuple 
//               given by a 3D ptBox (b).
//            Determines naut mile Closest-Point-of-Approach (CPA)
//               and returns a value after passing it thru the 
//               metric() function.

double AOF_R13::evalBox(const IvPBox *b) const
{
  // Part 1: Determine the course and speed being evaluated.
  double eval_crs = 0;
  double eval_spd = 0;

  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix), eval_spd);

  // Part 2: Determine the raw CPA distance that would result from the 
  // given course and speed and configured time-on-leg.
  //double cpa_dist = 20;
  double cpa_dist = m_cpa_engine.evalCPA(eval_crs, eval_spd, m_tol);
  if(cpa_dist < m_collision_distance)
    return(0);
  
  // Part 3: Determine the utility of the calculated CPA distance based
  // on the tolerance ranges given by m_collision_dist and m_all_clear_dist
  //double utility  = 100;
  double utility  = metricCPA(cpa_dist);

  // Part 4: Frther refine the utility based on whether whether the 
  // bow-line or stern-lines should be crossed. If utility is already 
  // zero, don't bother with further refinement. Utilities can only 
  // go down in this step.

  if(utility > 0) 
    utility = metricPassesSide(utility, eval_crs, eval_spd);
 
  return(utility);
}
 

//----------------------------------------------------------------
// Procedure: metricCPA

double AOF_R13::metricCPA(double eval_dist) const
{
 double min_dist = m_collision_distance;
  double max_dist = m_all_clear_distance;

  if(eval_dist > max_dist) 
    return(100);

  double range = max_dist - min_dist;
  if(range == 0)
    return(0);

  double tween = 15.0 + (85.0 * (eval_dist - min_dist) / range);
  return(tween);
}


//----------------------------------------------------------------
// Procedure: metricPassesSide

double AOF_R13::metricPassesSide(double utility, double hdg, double spd) const
{
  // Handle special case. If ownship speed is zero we declare this has no loss
  // in utitlity.
  if(spd == 0)
    return(utility);

  //bool crosses_stern = true;
  //bool crosses_stern = m_cpa_engine.crossesStern(hdg, spd);

  if(m_pass_to_port) {
    if(m_cpa_engine.passesStar(hdg, spd)) {
      if(m_cpa_engine.passesPortDist(hdg, spd) < m_collision_distance)
	return(0);
      else
	return(utility);
    }
    else
      return(utility);
  }
  else {  // pass_to_port=false, passing to starboard
    if(m_cpa_engine.passesPort(hdg, spd)) {
      if(m_cpa_engine.passesStarDist(hdg, spd) < m_collision_distance)
	return(0);
      else
	return(utility);
    }
    else
      return(utility);
  }
}  

