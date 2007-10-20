/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_SearchGridTOL.cpp                                */
/*    DATE: Sept 3rd, 2005                                       */
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
#include "AOF_SearchGridTOL.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_SearchGridTOL::AOF_SearchGridTOL(IvPDomain g_domain, 
			       const XYGrid *g_grid)
  : AOF(g_domain)
{
  crs_ix = g_domain.getIndex("course");
  spd_ix = g_domain.getIndex("speed");
  tol_ix = g_domain.getIndex("tol");

  search_grid = g_grid;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_SearchGridTOL::setParam(const string& param, double param_val)
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
  else
    return(false);
}


//----------------------------------------------------------------
// Procedure: initialize

bool AOF_SearchGridTOL::initialize()
{
  if((crs_ix==-1)||(spd_ix==-1)||(tol_ix==-1))
    return(false);

  if(!os_lat_set || !os_lon_set)
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

double AOF_SearchGridTOL::evalBox(const IvPBox *b) const
{
  double eval_crs, eval_spd, eval_tol;

  domain.getVal(crs_ix, b->pt(crs_ix,0), eval_crs);
  domain.getVal(spd_ix, b->pt(spd_ix,0), eval_spd);
  domain.getVal(tol_ix, b->pt(tol_ix,0), eval_tol);

  double gamOS  = degToRadians(eval_crs); // Angle in radians.
  double cgamOS = cos(gamOS);             // Cosine of Angle (osCRS).
  double sgamOS = sin(gamOS);             // Sine   of Angle (osCRS).

  double eval_lat = (cgamOS * eval_spd * eval_tol) + os_lat;
  double eval_lon = (sgamOS * eval_spd * eval_tol) + os_lon;
  
  double x1 = os_lon;
  double y1 = os_lat;
  double x2 = eval_lon;
  double y2 = eval_lat;

  double ret_val = 0;

  double gmin_val = search_grid->getMinVal();
  double gmax_val = search_grid->getMaxVal();

  //cout << "gmin_val:" << gmin_val;
  //cout << "gmax_val:" << gmax_val << endl;

  int gsize = search_grid->size();
  for(int i=0; i<gsize; i++) {
    XYSquare square = search_grid->getElement(i);
    double length = square.segIntersectLength(x1,y1,x2,y2);
    if((eval_spd > 0) && (length > 0)) {
#if 1
      //double time_in_square = length / evalSPD;
      double time_in_square = length;
      double cur_val = search_grid->getVal(i);
      if(cur_val > 10)
	cur_val = 10.0;
      double interest = 10.0 - cur_val;
      double utility = time_in_square * interest;
      ret_val += utility;
#endif
#if 0
      ret_val += length;
#endif
    }
  }
  
  return(ret_val/5.0);
}

//----------------------------------------------------------------
// Procedure: metric
//                                                   
// 100 |\                                                 
//     |  \          K = (3 * direct_distance)                                      
//     |    \                                              
//     |      \                                            
//     |        \                                          
//     |          \
//   0 |____________\___________________                                                 
//     0            K
//          detour_distance


double AOF_SearchGridTOL::metric() const
{
  return(0);
}







