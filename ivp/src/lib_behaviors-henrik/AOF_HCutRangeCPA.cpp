/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_HCutRangeCPA.cpp                                  */
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
#include <math.h> 
#include "AOF_HCutRangeCPA.h"
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

AOF_HCutRangeCPA::AOF_HCutRangeCPA(IvPDomain gdomain)
  : AOF(gdomain)
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

bool AOF_HCutRangeCPA::setParam(const string& param, double param_val)
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
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_HCutRangeCPA::initialize()
{
  if((crs_ix==-1)||(spd_ix==-1)||(tol_ix==-1))
    return(false);

  if(!os_lat_set || !os_lon_set || !cn_lat_set)
    return(false);

  if(!cn_lon_set || !cn_crs_set || !cn_spd_set)
    return(false);

  cpa_engine = new CPAEngine(cn_lat, cn_lon, cn_crs, cn_spd,
			     os_lat, os_lon);
  return(true);
}

//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given <Course, Speed, Time-on-leg> tuple 
//               given by a 3D ptBox (b).
//            Determines naut mile Closest-Point-of-Approach (CPA)
//               and returns a value after passing it thru the 
//               metric() function.

double AOF_HCutRangeCPA::evalBox(const IvPBox *b) const
{
  double eval_crs = 0;
  double eval_spd = 0;
  double eval_tol = 0;

  m_domain.getVal(crs_ix, b->pt(crs_ix,0), eval_crs);
  m_domain.getVal(spd_ix, b->pt(spd_ix,0), eval_spd);
  m_domain.getVal(tol_ix, b->pt(tol_ix,0), eval_tol);

  double eval_dist = cpa_engine->evalCPA(eval_crs, eval_spd, eval_tol);

  return(metric(eval_dist));
}

//----------------------------------------------------------------
// Procedure: metric

double AOF_HCutRangeCPA::metric(double gval) const
{
  double pct = gval / (sqrt(cpa_engine->getK0()));
  pct = pct * pct;
  pct = 1.0 - pct;
  double retVal = 1000.0 * pct;
  return(retVal);
}







