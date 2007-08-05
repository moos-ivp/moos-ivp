/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_OpenRangeCPA.cpp                                 */
/*    DATE: May 11th, 2005                                       */
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
#include "AOF_OpenRangeCPA.h"
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

AOF_OpenRangeCPA::AOF_OpenRangeCPA(IvPDomain gdomain, 
		     double gcnlat, double gcnlon, double gcncrs,
		     double gcnspd, double goslat, double goslon)
{
  int crs_ix = gdomain.getIndex("course");
  int spd_ix = gdomain.getIndex("speed");
  int tol_ix = gdomain.getIndex("tol");

  assert((crs_ix != -1) && (spd_ix != -1) && (tol_ix != -1));

  crsDelta = gdomain.get_ddelta(crs_ix);
  spdDelta = gdomain.get_ddelta(spd_ix);
  tolDelta = gdomain.get_ddelta(tol_ix);

  crsBase  = gdomain.get_dlow(crs_ix);
  spdBase  = gdomain.get_dlow(spd_ix);
  tolBase  = gdomain.get_dlow(tol_ix);

  universe = IvPBox(3); 
  universe.setPTS(0, 0, gdomain.get_dpoints(crs_ix)-1);
  universe.setPTS(1, 0, gdomain.get_dpoints(spd_ix)-1);
  universe.setPTS(2, 0, gdomain.get_dpoints(tol_ix)-1);

  cnLAT = gcnlat; 
  cnLON = gcnlon;
  cnSPD = gcnspd;
  cnCRS = gcncrs;
  osLAT = goslat;
  osLON = goslon;

  assert((cnSPD >=    0) && (cnSPD <= 400.0));
  assert((cnCRS >= -180) && (cnCRS <= 359.9));

  this->setStatic();

  minFlag = true;
}

//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given <Course, Speed, Time-on-leg> tuple 
//               given by a 3D ptBox (b).
//            Determines naut mile Closest-Point-of-Approach (CPA)
//               and returns a value after passing it thru the 
//               metric() function.

double AOF_OpenRangeCPA::evalBox(const IvPBox *b) const
{
  double osCRS = crsBase + ((double)(b->pt(0,0)) * crsDelta);
  double osSPD = spdBase + ((double)(b->pt(1,0)) * spdDelta);
  double osTOL = tolBase + ((double)(b->pt(2,0)) * tolDelta);
  //osTOL = osTOL / 60.0; // ?????????????????//

  if(osSPD == 0.0) return(metric(sqrt(statK0)));

  double gamOS  = degToRadians(osCRS);    // Angle in radians.
  double cgamOS = cos(gamOS);             // Cosine of Angle (osCRS).
  double sgamOS = sin(gamOS);             // Sine   of Angle (osCRS).

  double k2 = statK2;
  double k1 = statK1;
  double k0 = statK0;

  if((cnCRS==osCRS) && (cnSPD==osSPD))  // Special case. K1,K2 would
    return(metric(sqrt(k0)));           // be 0, resuling in NaN.

  k2 +=          cgamOS * cgamOS * osSPD * osSPD;   // (1,1)(a)
  k2 +=          sgamOS * sgamOS * osSPD * osSPD;   // (1,1)(b)
  k2 += (-2.0) * cgamOS * osSPD * cgamCN * cnSPD;   // (1,3)(3,1)(a)
  k2 += (-2.0) * sgamOS * osSPD * sgamCN * cnSPD;   // (1,3)(3,1)(b)

  k1 += ( 2.0) * cgamOS * osSPD * osLAT;  // (1,2)(2,1)(a)
  k1 += ( 2.0) * sgamOS * osSPD * osLON;  // (1,2)(2,1)(b)
  k1 += (-2.0) * cgamOS * osSPD * cnLAT;  // (1,4)(4,1)(a)
  k1 += (-2.0) * sgamOS * osSPD * cnLON;  // (1,4)(4,1)(b)

  double minT, minDist, endDist, evalDist; // Return Value;
  if(minFlag == true) {
    minT = ((-1.0) * k1) / (2.0 * k2);
    if(minT <= 0) 
      minDist = sqrt(k0); 
    else 
      if(minT >= osTOL)      
	minDist = sqrt((k2*osTOL*osTOL) + (k1*osTOL) + k0);
      else
	minDist = sqrt((k2*minT*minT) + (k1*minT) + k0);
    evalDist = minDist;
  }
  else {
    endDist = (k2*osTOL*osTOL) + (k1*osTOL) + k0;
    endDist = sqrt(endDist);
    evalDist = endDist;
  }

  evalDist = metric(evalDist);
  return(evalDist);
}

//----------------------------------------------------------------
// Procedure: print

void AOF_OpenRangeCPA::print() const
{
  cout << "cnLAT:" << cnLAT << "  cnLON: " << cnLON << endl;
  cout << "osLAT:" << osLAT << "  osLON: " << osLON << endl;
  cout << "cnCRS:" << cnCRS << "  cnSPD: " << cnSPD << endl;
}


//----------------------------------------------------------------
// Procedure: setStatic
//   Purpose: Determine all terms not dependent on osCRS, osSPD or
//            osTOL. These can be calculated once to save time.
//    Recall: The distance between OS and CN is calculated using
//            pythagorean theorem and Lat/Lon distances. C^2 is the
//            distance between ships and a^2 is LAT distance and 
//            b^2 is LON distance:
//                    a^2 = (new_osLAT - new_cnLAT)^2
//                    b^2 = (new_osLON - new_cnLON)^2
//
//            new_osLAT ... newcnLON indicates the new positions
//            after osTOL units of time, and the given starting
//            positions and trajectories:
//                    new_osLAT = cos(osCRS)*osSPD*osTOL + osLAT
//                    new_cnLAT = cos(cnCRS)*cnSPD*osTOL + cnLAT
//                    new_osLON = sin(osCRS)*osSPD*osTOL + osLON
//                    new_cnLON = sin(cnCRS)*cnSPD*osTOL + cnLON
//            Multiplying the terms out, we get some terms with
//            osTOL^2, osTOL, and constants. We lump the 
//            coeffecients as follows:
//                    K2: coefficients of osTOL^2.
//                    K1: coefficients of osTOL.
//                    K0: constants.
//            More than half of these are not dependent on osSPD
//            or osCRS, so we calculate them once, here.

void AOF_OpenRangeCPA::setStatic()
{
  //osLAT = osLAT*60.0;    osLON = osLON*60.0;
  //cnLAT = cnLAT*60.0;    cnLON = cnLON*60.0;

  gamCN   = degToRadians(cnCRS);  // Angle in radians.
  cgamCN  = cos(gamCN);           // Cosine of Angle (cnCRS).
  sgamCN  = sin(gamCN);           // Sine   of Angle (cnCRS).

  statK2  = (+1.0) * cgamCN * cgamCN * cnSPD * cnSPD;    //(3,3)(a)
  statK2 += (+1.0) * sgamCN * sgamCN * cnSPD * cnSPD;    //(3,3)(b)

  statK1  = (-2.0) * osLAT * cgamCN * cnSPD;    // (2,3)(3,2)(a)
  statK1 += (-2.0) * osLON * sgamCN * cnSPD;    // (2,3)(3,2)(b)
  statK1 += ( 2.0) * cnLAT * cgamCN * cnSPD;    // (3,4)(4,3)(a)
  statK1 += ( 2.0) * cnLON * sgamCN * cnSPD;    // (3,4)(4,3)(b)
  
  statK0  =          osLAT * osLAT;  // (2,2)(a)
  statK0 +=          osLON * osLON;  // (2,2)(b)
  statK0 += (-2.0) * osLAT * cnLAT;  // (2,4)(4,2)(a)
  statK0 += (-2.0) * osLON * cnLON;  // (2,4)(4,2)(a)
  statK0 +=          cnLAT * cnLAT;  // (4,4)(a)
  statK0 +=          cnLON * cnLON;  // (4,4)(b)
}

//----------------------------------------------------------------
// Procedure: metric

double AOF_OpenRangeCPA::metric(double gval) const
{
  double pct = gval / (sqrt(statK0));
  pct = pct * pct;
  pct = 1.0 - pct;
  double retVal = 1000.0 * pct;
  return(retVal);
}





