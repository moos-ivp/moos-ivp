/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_Shadow.cpp                                       */
/*    DATE: May 10th, 2005                                       */
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
#include "AOF_Shadow.h"
#include "AngleUtils.h"
#include "IvPDomain.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor
//      args: gcnlat  Given Contact Latitude Position
//      args: gcnlon  Given Contact Longitude Position
//      args: gcncrs  Given Contact Course
//      args: gcnspd  Given Contact Speed
//      args: goslat  Given Ownship Latitude Position
//      args: goslon  Given Ownship Latitude Position

AOF_Shadow::AOF_Shadow(IvPDomain gdomain)
  : AOF(gdomain)
{
  crs_ix = gdomain.getIndex("course");
  spd_ix = gdomain.getIndex("speed");

  cn_spd_set = false;
  cn_crs_set = false;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_Shadow::setParam(const string& param, double param_val)
{
  if(param == "cn_crs") {
    cn_crs = param_val;
    cn_crs_set = true;
    return(true);
  }
  else if(param == "cn_spd") {
    cn_spd = param_val;
    cn_spd_set = true;
    return(true);
  }
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_Shadow::initialize()
{
  if((crs_ix==-1)||(spd_ix==-1))
    return(false);
  if(!cn_crs_set || !cn_spd_set)
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

double AOF_Shadow::evalBox(const IvPBox *b) const
{
  double eval_crs = 0;
  double eval_spd = 0;

  m_domain.getVal(crs_ix, b->pt(crs_ix,0), eval_crs);
  m_domain.getVal(spd_ix, b->pt(spd_ix,0), eval_spd);

  double course_diff = cn_crs - eval_crs;
  if(course_diff > 180)
    course_diff -= 360;
  if(course_diff < -180)
    course_diff += 360;
  if(course_diff < 0)
    course_diff *= -1;

  double speed_diff = cn_spd - eval_spd;
  if(speed_diff < 0)
    speed_diff *= -1;

  double utility = metric(course_diff, speed_diff);
  return(utility);
}

//----------------------------------------------------------------
// Procedure: metric

double AOF_Shadow::metric(double cval, double sval) const
{
  double cpct = cval / 180.0;
  cpct = cpct * cpct;

  double spct = sval / 12;  // 20 is arbitrary, can we do better?
  spct = spct * spct;

  double cwt = 0.5;  // Should range between 0.0 and 1.0
  double total_pct = (cwt * cpct) + ((1-cwt) * spct);

  double range = 100.0;
  return((1.0 - total_pct) * range);
}









