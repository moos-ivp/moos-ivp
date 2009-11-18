/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_CutRangeCPA.cpp                                  */
/*    DATE: Nov 4th, 2006                                        */
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
#include "AOF_AttractorCPA.h"
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

AOF_AttractorCPA::AOF_AttractorCPA(IvPDomain gdomain)
  : AOF(gdomain)
{
  crs_ix = gdomain.getIndex("course");
  spd_ix = gdomain.getIndex("speed");

  os_lat_set = false;
  os_lon_set = false;
  cn_lat_set = false;
  cn_lon_set = false;
  cn_crs_set = false;
  cn_spd_set = false;
  tol_set    = false;

  min_util_cpa_dist_set = false;
  max_util_cpa_dist_set = false;

  cpa_engine = 0;
  patience   = 0;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_AttractorCPA::setParam(const string& param, double param_val)
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
  else if(param == "cnlat") {
    cn_lat = param_val;
    cn_lat_set = true;
    return(true);
  }
  else if(param == "cnlon") {
    cn_lon = param_val;
    cn_lon_set = true;
    return(true);
  }
  else if(param == "cncrs") {
    cn_crs = param_val;
    cn_crs_set = true;
    return(true);
  }
  else if(param == "cnspd") {
    cn_spd = param_val;
    cn_spd_set = true;
    return(true);
  }
  else if(param == "tol") {
    tol = param_val;
    tol_set = true;
    return(true);
  }
  else if(param == "patience") {
    if((param_val < 0) || (param_val > 100))
      return(false);
    patience = param_val;
    return(true);
  }
  else if(param == "min_util_cpa_dist") {
    min_util_cpa_dist = param_val;
    min_util_cpa_dist_set = true;
    return(true);
  }
  else if(param == "max_util_cpa_dist") {
    max_util_cpa_dist = param_val;
    max_util_cpa_dist_set = true;
    return(true);
  }
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_AttractorCPA::initialize()
{
  if((crs_ix==-1)||(spd_ix==-1))
    return(false);

  if(!os_lat_set || !os_lon_set || !cn_lat_set || !tol_set)
    return(false);

  if(!cn_lon_set || !cn_crs_set || !cn_spd_set)
    return(false);

  if(!min_util_cpa_dist_set || !max_util_cpa_dist_set)
    return(false);
  
  if(min_util_cpa_dist <= max_util_cpa_dist)
    return(false);

  if(tol < 1)
    return(false);

  cpa_engine = new CPAEngine(cn_lat, cn_lon, cn_crs, cn_spd,
			     os_lat, os_lon);

  max_heading = cpa_engine->minMaxROC(5, 360, min_roc, max_roc);
  
  range_roc = max_roc - min_roc;

  return(true);
}

//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Eval given <Course, Speed> tuple given by a 2D ptBox (b).
//            Determines naut mile Closest-Point-of-Approach (CPA)
//               and returns a value after passing it thru the 
//               metric() function.

double AOF_AttractorCPA::evalBox(const IvPBox *b) const
{
  double eval_crs = 0;
  double eval_spd = 0;

  m_domain.getVal(crs_ix, b->pt(crs_ix,0), eval_crs);
  m_domain.getVal(spd_ix, b->pt(spd_ix,0), eval_spd);

  double roc;

  double eval_dist = cpa_engine->evalCPA(eval_crs, eval_spd, tol, &roc);

  double nroc = 0;

  if(range_roc > 0)
    nroc = ((roc - min_roc) / (range_roc)) * 100.0;

  double metric_eval = metric(eval_dist);
  
  double pct = patience / 100.0;

  double compromise = ((1.0-pct) * nroc) + (pct * metric_eval);

  return(compromise);
}

//----------------------------------------------------------------
// Procedure: metric

double AOF_AttractorCPA::metric(double gval) const
{
  double min_val = max_util_cpa_dist;
  double max_val = min_util_cpa_dist;

  if(min_val >= max_val)
    return(0);

  if(gval < min_val)
    return(100);
  if(gval > max_val)
    return(0);
  
  
  else {
    double pct = (gval-min_val) / (max_val - min_val);
    return(100 - (pct * 100));
  }
  
}









