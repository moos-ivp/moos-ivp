/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_OpArea.cpp                                       */
/*    DATE: July 6th, 2005                                       */
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
#include "AOF_OpArea.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
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

AOF_OpArea::AOF_OpArea(IvPDomain gdomain, const XYPolygon& g_poly)
  : AOF(gdomain)
{
  poly = g_poly;

  crs_ix = gdomain.getIndex("course");
  spd_ix = gdomain.getIndex("speed");
  tol_ix = gdomain.getIndex("tol");

  os_lat_set = false;
  os_lon_set = false;
  buffer_set = false;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_OpArea::setParam(const string& param, double param_val)
{
  if(param == "os_lat") {
    os_lat = param_val;
    os_lat_set = true;
    return(true);
  }
  if(param == "os_lon") {
    os_lon = param_val;
    os_lon_set = true;
    return(true);
  }
  if(param == "buffer") {
    buffer = param_val;
    buffer_set = true;
    return(true);
  }
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_OpArea::initialize()
{
  if((crs_ix==-1)||(spd_ix==-1)||(tol_ix==-1))
    return(false);

  if(!os_lat_set || !os_lon_set || !buffer_set)
    return(false);

  return(true);
}

//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given <Course, Speed, Time-on-leg> tuple 
//            given by a 3D ptBox (b).

double AOF_OpArea::evalBox(const IvPBox *b) const
{
  double eval_crs, eval_spd, eval_tol;

  m_domain.getVal(crs_ix, b->pt(crs_ix,0), eval_crs);
  m_domain.getVal(spd_ix, b->pt(spd_ix,0), eval_spd);
  m_domain.getVal(tol_ix, b->pt(tol_ix,0), eval_tol);

  double dist = eval_spd * eval_tol;  // meters/second * seconds
  double new_x, new_y;

  projectPoint(eval_crs, dist, os_lon, os_lat, new_x, new_y);
  
  if(!poly.contains(new_x, new_y))
    return(0);

  double dist_to_poly = poly.dist_to_poly(new_x, new_y);
  if(dist_to_poly > buffer)
    return(100.0);

  double base  = 25.0;
  double range = 75.0;

  double util = base + range * (dist_to_poly / buffer);
  return(util);
}








