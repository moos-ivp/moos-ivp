/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AOF_CPA.cpp                                          */
/*    DATE: Nov 18th, 2006                                       */
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
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <cmath> 
#include "AOF_CPA.h"
#include "AngleUtils.h"
#include "IvPDomain.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor
AOF_CPA::AOF_CPA(IvPDomain gdomain) 
  : AOF_Contact(gdomain)
{
  m_crs_ix = gdomain.getIndex("course");
  m_spd_ix = gdomain.getIndex("speed");

  m_inextremis = false;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_CPA::setParam(const string& param, double value)
{
  if(AOF_Contact::setParam(param, value))
    return(true);
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_CPA::setParam(const string& param, const string& value)
{
  if(AOF_Contact::setParam(param, value))
    return(true);
  else if(param == "inextremis")
    return(setBooleanOnString(m_inextremis, value));
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_CPA::initialize()
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

double AOF_CPA::evalBox(const IvPBox *b) const
{
  double eval_crs = 0;
  double eval_spd = 0; 

  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix), eval_spd);

  double cpa_dist  = m_cpa_engine.evalCPA(eval_crs, eval_spd, m_tol);
  double eval_dist = metric(cpa_dist);

  return(eval_dist);
}

//----------------------------------------------------------------
// Procedure: metric

double AOF_CPA::metric(double eval_dist) const
{
  double min = m_collision_distance;
  double max = m_all_clear_distance;

  if(eval_dist < min) return(0);
  if(eval_dist > max) return(100);

  //double tween = 100.0 * (gval-min) / (max-min);
  double tween = 25.0 + 75.0 * (eval_dist - min) / (max-min);
  return(tween);
}






