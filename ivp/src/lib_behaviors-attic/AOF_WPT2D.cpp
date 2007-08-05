/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_WPT2D.cpp                                        */
/*    DATE: Febuary 9th, 2001                                    */
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
#include "AOF_WPT2D.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_WPT2D::AOF_WPT2D(IvPDomain g_domain) 
  : AOF(g_domain)
{
  crs_ix = g_domain.getIndex("course");
  spd_ix = g_domain.getIndex("speed");

  os_lat_set = false;
  os_lon_set = false;
  pt_lat_set = false;
  pt_lon_set = false;
  desired_spd_set = false;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_WPT2D::setParam(const string& param, double param_val)
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
  else if(param == "ptlat") {
    pt_lat = param_val;
    pt_lat_set = true;
    return(true);
  }
  else if(param == "ptlon") {
    pt_lon = param_val;
    pt_lon_set = true;
    return(true);
  }
  else if(param == "desired_speed") {
    desired_spd = param_val;
    desired_spd_set = true;
    return(true);
  }
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_WPT2D::initialize()
{
  if((crs_ix==-1)||(spd_ix==-1))
    return(false);

  if(!os_lat_set || !os_lon_set || !pt_lat_set || !pt_lon_set)
    return(false);

  if(!desired_spd_set)
    return(false);
    
  relative_angle = relAng(os_lon, os_lat, pt_lon, pt_lat);
  return(true);
}

//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given <Course, Speed> tuple 
//               given by a 2D ptBox (b).

double AOF_WPT2D::evalBox(const IvPBox *b) const
{
  double eval_crs = 0;
  double eval_spd = 0;
  
  domain.getVal(crs_ix, b->pt(crs_ix,0), eval_crs);
  domain.getVal(spd_ix, b->pt(spd_ix,0), eval_spd);

  double crs_diff = fabs(eval_crs - relative_angle);
  if(crs_diff > 180)
    crs_diff = 360.0 - crs_diff;

  double spd_diff = fabs(desired_spd - eval_spd);

  // Apply a metrric to the crs_diff and spd_diff values
  double val = ((180.0 - crs_diff) + (10*(5.0 - spd_diff)));

  return(val);
}








