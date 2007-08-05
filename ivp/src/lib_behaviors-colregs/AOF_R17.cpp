/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_R17.cpp                                          */
/*    DATE: Febuary 9th, 2001 (CPA logic written)                */
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
#include <math.h> 
#include "AOF_R17.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor
//      args: gcnlat  Given Contact Latitude Position
//      args: gcnlon  Given Contact Longitude Position
//      args: gcncrs  Given Contact Course
//      args: gcnspd  Given Contact Speed
//      args: goslat  Given Ownship Latitude Position
//      args: goslon  Given Ownship Latitude Position

AOF_R17::AOF_R17(IvPDomain gdomain) : AOF(gdomain)
{
  crs_ix = gdomain.getIndex("course");
  spd_ix = gdomain.getIndex("speed");
  tol_ix = gdomain.getIndex("tol");

  os_lat_set = false;
  os_lon_set = false;
  cn_lat_set = false;
  cn_lon_set = false;
  cn_crs_set = false;
  cn_spd_set = false;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_R17::setParam(const string& param, double param_val)
{
  if(param == "os_lat") {
    os_lat = param_val;
    os_lat_set = true;
    return(true);
  }
  else if(param == "os_lon") {
    os_lon = param_val;
    os_lon_set = true;
    return(true);
  }
  else if(param == "cn_lat") {
    cn_lat = param_val;
    cn_lat_set = true;
    return(true);
  }
  else if(param == "cn_lon") {
    cn_lon = param_val;
    cn_lon_set = true;
    return(true);
  }
  else if(param == "cn_crs") {
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

bool AOF_R17::initialize()
{
  if((crs_ix==-1)|| (spd_ix==-1)|| (tol_ix==-1))
    return(false);

  if(!os_lat_set || !os_lon_set || !cn_lat_set)
    return(false);

  if(!cn_lon_set || !cn_crs_set || !cn_spd_set)
    return(false);

  cpa_engine = new CPAEngine(cn_lat, cn_lon, cn_crs, 
			     cn_spd, os_lat, os_lon);

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
  double eval_crs, eval_spd, eval_tol, cpa_dist, eval_dist;

  m_domain.getVal(crs_ix, b->pt(crs_ix,0), eval_crs);
  m_domain.getVal(spd_ix, b->pt(spd_ix,0), eval_spd);
  m_domain.getVal(tol_ix, b->pt(tol_ix,0), eval_tol);

  cpa_dist  = cpa_engine->evalCPA(eval_crs, eval_spd, eval_tol);

  eval_dist = metric(cpa_dist);
  eval_dist = metric2(eval_dist, eval_crs);
  return(eval_dist);
}

//----------------------------------------------------------------
// Procedure: metric

double AOF_R17::metric(double gval) const
{
  double min = 8.0;
  double max = 100.0;

  if(gval < min) return(0);
  if(gval > max) return(100);

  //double tween = 100.0 * (gval-min) / (max-min);
  double tween = 25.0 + 75.0 * (gval-min) / (max-min);
  return(tween);

#if 0
  double pct = gval / (sqrt(cpa_engine->getK0()));
  pct = pct * pct;
  pct = 1.0 - pct;

  double return_val = -100.0 * pct;
  return(return_val);
#endif
}


//----------------------------------------------------------------
// Procedure: metric2

double AOF_R17::metric2(double g_val, double g_crs) const
{
  if((os_lon == cn_lon) && (os_lat == cn_lat))
    return(0);

  double bearing = relAng(os_lon, os_lat, cn_lon, cn_lat);
  double relative_bearing = (g_crs - bearing);

  if(relative_bearing < 0)
    relative_bearing += 360.0;

  double rad_relbearing = degToRadians(relative_bearing);

  double extra = sin(rad_relbearing) * 10.0;

  return(g_val + (4.0*extra));
}








