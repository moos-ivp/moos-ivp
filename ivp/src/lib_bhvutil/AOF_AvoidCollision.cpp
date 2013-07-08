/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_AvoidCollision.cpp                               */
/*    DATE: Nov 18th, 2006                                       */
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
#include <cmath> 
#include "AOF_AvoidCollision.h"
#include "AngleUtils.h"
#include "IvPDomain.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor
AOF_AvoidCollision::AOF_AvoidCollision(IvPDomain gdomain) 
  : AOF_Contact(gdomain)
{
  m_crs_ix = gdomain.getIndex("course");
  m_spd_ix = gdomain.getIndex("speed");
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_AvoidCollision::setParam(const string& param, double param_val)
{
  if(AOF_Contact::setParam(param, param_val))
    return(true);
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_AvoidCollision::initialize()
{
  if(AOF_Contact::initialize() == false)
    return(false);

  if((m_crs_ix==-1) || (m_spd_ix==-1))
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

double AOF_AvoidCollision::evalBox(const IvPBox *b) const
{
  double eval_crs, eval_spd, cpa_dist, eval_dist;

  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix), eval_spd);

  cpa_dist  = m_cpa_engine.evalCPA(eval_crs, eval_spd, m_tol);

  eval_dist = metric(cpa_dist);
  return(eval_dist);
}

//----------------------------------------------------------------
// Procedure: metric

double AOF_AvoidCollision::metric(double eval_dist) const
{
  double min = m_collision_distance;
  double max = m_all_clear_distance;

  if(eval_dist < min) return(0);
  if(eval_dist > max) return(100);

  //double tween = 100.0 * (gval-min) / (max-min);
  double tween = 25.0 + 75.0 * (eval_dist - min) / (max-min);
  return(tween);
}












