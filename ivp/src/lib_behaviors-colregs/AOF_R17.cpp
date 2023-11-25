/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AOF_R17.cpp                                          */
/*    DATE: Febuary 9th, 2001 (CPA logic written)                */
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
#include "AOF_R17.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_R17::AOF_R17(IvPDomain gdomain) : AOF_Contact(gdomain)
{
  m_crs_ix = gdomain.getIndex("course");
  m_spd_ix = gdomain.getIndex("speed");

  m_pass_mode = "neither";

  m_osh = 0;
  m_osv = 0;
  m_osh_set = false;
  m_osv_set = false;

  m_original_course = 0;
  m_original_speed = 0;
  m_original_speed_set = false;
  m_original_course_set = false;
}



//----------------------------------------------------------------
// Procedure: setParam

bool AOF_R17::setParam(const string& param, double param_val)
{
  if(AOF_Contact::setParam(param, param_val))
    return(true);

  if(param == "osh") {         // The current ownship heading
    m_osh = param_val;
    m_osh_set = true;
  }
  else if(param == "osv") {    // The current ownship speed
    m_osv = param_val;
    m_osv_set = true;
  }
  else if(param == "original_course") {   // Ownship hdg when started
    m_original_course = param_val;
    m_original_course_set = true;
  }
  else if(param == "original_speed") {    // Ownship spd when started
    m_original_speed = param_val;
    m_original_speed_set = true;
  }
  else
    return(false);

  return(true);
}


//----------------------------------------------------------------
// Procedure: setParam

bool AOF_R17::setParam(const string& param, const string& param_val)
{
  if(AOF_Contact::setParam(param, param_val))
    return(true);
  
  if(param == "passing_side") {
    if((param_val == "stern")        ||
       (param_val == "bow")          ||
       (param_val == "unsure")       ||
       (param_val == "unsure_bow")   ||
       (param_val == "unsure_stern") ||
       (param_val == "neither")      ||
       (param_val == "inextremis"))
      m_pass_mode = param_val;
    else
      return(false);
  }
  else
    return(false);
  
  return(true);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_R17::initialize()
{
  if(AOF_Contact::initialize() == false)
    return(false);
  if((m_crs_ix==-1) || (m_spd_ix==-1))
    return(false);
  if(m_pass_mode == "")
    return(false);
  if(!m_osh_set)
    return(false);
  if(!m_original_course_set || !m_original_speed_set)
    return(false);
  
  return(true);
}

//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given <Course, Speed, Time-on-leg> tuple 
//               given by a 3D ptBox (b).
//            Determines naut mile Closest-Point-of-Approach (CPA)
//               and returns a value after passing it thru the 
//               metric() function.

double AOF_R17::evalBox(const IvPBox *b) const
{
  // Part 1: Determine the course and speed being evaluated.
  double eval_crs, eval_spd;
  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix,0), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix,0), eval_spd);

  // Rule 17- Action by Stand-on Vessel

  // (a) (i) Where one of two vessels is to keep out of the way, the
  // other shall keep her course and speed.
  // (ii) The latter vessel may, however, take action to avoid
  // collision by her maneuver alone, as soon as it becomes apparent
  // to her that the vessel required to keep out of the way is not
  // taking appropriate action in compliance with these Rules.

  // (b) When, from any cause, the vessel required to keep her course
  // and speed finds herself so close that collision cannot be avoided
  // by the action of the give-way vessel alone, she shall take such
  // action as will best aid to avoid collision.

  // (c) A power-driven vessel which takes action in a crossing
  // situation in accordance with Rule 17(a)(ii) to avoid collision
  // with another power-driven vessel shall, if the circumstances of
  // the case admit, not alter course to port for a vessel on her own
  // port side.

  // (d) This Rule does not relieve the give-way vessel of her
  // obligation to keep out of the way.

  // mapping of behavior parameters to AOF_17 variables:
  // m_all_clear_dist    = m_max_util_cpa_dist
  // m_collision_dist    = m_min_util_cpa_dist
  // m_active_inner_dist = m_pwt_inner_dist
  // m_active_outer_dist = m_pwt_outer_dist
    
  // if invoking 17.a.ii, then avoid using utilityAvoid
  if(m_pass_mode == "inextremis")
    return(utilityAvoid(eval_crs, eval_spd));

  // per 17.a.i, if not in extremis, then must maintain course and speed
  return(utilityHold(eval_crs, eval_spd));
}


//----------------------------------------------------------------
// Procedure: utilityHold
//   Purpose: Returns a value between [0, 100]

double AOF_R17::utilityHold(double eval_crs, double eval_spd) const
{
  // Part 1: Calculate the heading offset and heading offset factor
  // Part 1A: Determine the heading offset
  double delta_crs = angle180(eval_crs - m_original_course);
  if(delta_crs < 0)
    delta_crs *= -1;

  // Part 1B: Determine the heading offset factor from heading offset
  double delta_crs_factor = 0;   // [0,1]
  double delta_crs_max = 20;
  double delta_crs_rng = 20;
  if(delta_crs < delta_crs_max)
    delta_crs_factor = (delta_crs_max - delta_crs) / delta_crs_rng;


  // Part 2: Calculate the speed offset and speed offset factor
  // Part 2A: Determine the speed offset
  double delta_spd = eval_spd - m_original_speed;
  if(delta_spd < 0)
    delta_spd *= -1;

  // Part 2B: Determine the speed offset factor from speed offset
  double delta_spd_factor = 0;   // [0,1]
  double delta_spd_max = 1.5;
  double delta_spd_rng = 1.5;
  if(delta_spd < delta_spd_max)
    delta_spd_factor = (delta_spd_max - delta_spd) / delta_spd_rng;


  // Part 3: Combine the two factors, giving bit more weight to heading
  double total = ((2 * delta_crs_factor) + (delta_spd_factor)) / 3;

  // Convert from range of [0,1] to [0,100]
  return(total * 100);
}

//----------------------------------------------------------------
// Procedure: utilityAvoid
//   Purpose: Returns a value between [0, 100]

double AOF_R17::utilityAvoid(double eval_crs, double eval_spd) const
{
  //  17(c) A power-driven vessel which takes action in a crossing
  //  situation in accordance with Rule 17(a)(ii) to avoid collision
  //  with another power-driven vessel shall, if the circumstances of
  //  the case admit, not alter course to port for a vessel on her own
  //  port side.

  // Part 1: Determine the raw CPA distance that would result from the 
  // given course and speed and configured time-on-leg.
  double cpa_dist  = m_cpa_engine.evalCPA(eval_crs, eval_spd, m_tol);

  // Part 2: Determine a CPA discount based whether this hdg/spd crosses either
  // the bow or stern and whether it "should" cross the bow or stern. Recall a
  // discount or reduction in CPA ultimately means a reduced utility.
  double cpa_discount = 1.0;

  // kw addition
  double rb = relBearing(m_osx, m_osy, m_osh, m_cnx, m_cny);
  if(rb > 180) {
    if(m_cpa_engine.turnsLeft(m_osh,eval_crs)) 
      return(0);
  }


  // Apply the discount
  cpa_dist = cpa_dist * cpa_discount;

  double utility  = metricCPA(cpa_dist);

  return(utility);
}

//----------------------------------------------------------------
// Procedure: metricCPA

double AOF_R17::metricCPA(double eval_dist) const
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






