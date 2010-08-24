/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_CutRangeFCPA.cpp                                 */
/*    DATE: Nov 4th, 2006                                        */
/*****************************************************************/
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <math.h> 
#include "AOF_CutRangeCPA.h"
#include "AngleUtils.h"
#include "CPA_Utils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor
//      args: gcnlat  Given Contact Latitude Position
//      args: gcnlon  Given Contact Longitude Position
//      args: gcncrs  Given Contact Course
//      args: gcnspd  Given Contact Speed
//      args: goslat  Given Ownship Latitude Position
//      args: goslon  Given Ownship Latitude Position

AOF_CutRangeCPA::AOF_CutRangeCPA(IvPDomain gdomain)
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
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_CutRangeCPA::setParam(const string& param, double param_val)
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
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_CutRangeCPA::initialize()
{
  if((crs_ix==-1)||(spd_ix==-1))
    return(false);

  if(!os_lat_set || !os_lon_set || !cn_lat_set || !tol_set)
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

double AOF_CutRangeCPA::evalBox(const IvPBox *b) const
{
  double eval_crs = 0;
  double eval_spd = 0;
  double eval_tol = tol;

  m_domain.getVal(crs_ix, b->pt(crs_ix,0), eval_crs);
  m_domain.getVal(spd_ix, b->pt(spd_ix,0), eval_spd);

  double eval_dist = evalCPA(cn_lon, cn_lat, cn_spd, cn_crs,
			     os_lon, os_lat, eval_spd, eval_crs, 
			     eval_tol);

  //double eval_dist = cpa_engine->evalCPA(eval_crs, eval_spd, eval_tol);

  return(100 - metric(eval_dist));

  return(metric(eval_dist));
}

//----------------------------------------------------------------
// Procedure: metric

double AOF_CutRangeCPA::metric(double gval) const
{
  double min_val = 10;
  double max_val = 125;

  if(gval < min_val)
    return(100);
  if(gval > max_val)
    return(0);
  
  else {
    double pct = (gval-min_val) / (max_val - min_val);
    return(100 - (pct * 100));
  }
  
}







