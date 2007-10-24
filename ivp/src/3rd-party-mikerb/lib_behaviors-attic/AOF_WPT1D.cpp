/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_WPT1D.cpp                                        */
/*    DATE:                                                      */
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

#include <iostream>
#include <math.h> 
#include <assert.h>
#include "AOF_WPT1D.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_WPT1D::AOF_WPT1D(IvPDomain g_domain, 
		     double g_osLAT, double g_osLON, 
		     double g_ptLAT, double g_ptLON)
{
  int crs_ix = g_domain.getIndex("course");

  assert(crs_ix != -1);

  crsDelta = g_domain.get_ddelta(crs_ix);
  crsBase  = g_domain.get_dlow(crs_ix);

  universe = Box(1); 
  universe.setPTS(0, 0, g_domain.get_dpoints(crs_ix)-1);

  osLAT = g_osLAT;
  osLON = g_osLON;
  ptLAT = g_ptLAT;
  ptLON = g_ptLON;
}

//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given <Course, Speed, Time-on-leg> tuple 
//               given by a 3D ptBox (b).
//            Determines naut mile Closest-Point-of-Approach (CPA)
//               and returns a value after passing it thru the 
//               metric() function.

double AOF_WPT1D::evalBox(Box *b) const
{
  double rel_angle = relAng(osLON, osLAT, ptLON, ptLAT);

  //cout << "------------------------------------------------------" << endl;
  //cout << "osLON:" << osLON << "osLAT:" << osLAT << endl;
  //cout << "ptLON:" << ptLON << "ptLAT:" << ptLAT << endl;

  double evalCRS  = crsBase + ((double)(b->pt(0,0)) * crsDelta);

  //cout << "evalCRS: " << evalCRS << " relANG: " << rel_angle << endl;

  double crs_diff = fabs(evalCRS - rel_angle);
  if(crs_diff > 180)
    crs_diff = 360.0 - crs_diff;

  double val = metric(crs_diff);
  //cout << "crsDIFF: " << crs_diff << "retVal: " << val << endl;
  return(val);
}

//----------------------------------------------------------------
// Procedure: metric

double AOF_WPT1D::metric(double val) const
{
  return(180.0 - val);
}






