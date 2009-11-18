/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: CPAEngine.cpp                                        */
/*    DATE: May 12th 2005                                        */
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
#include "CPAEngine.h"
#include "GeomUtils.h"
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

CPAEngine::CPAEngine(double gcnlat, double gcnlon, double gcncrs,
		     double gcnspd, double goslat, double goslon)
{
  cnLAT   = gcnlat; 
  cnLON   = gcnlon;
  cnSPD   = gcnspd;
  cnCRS   = angle360(gcncrs);
  osLAT   = goslat;
  osLON   = goslon;
  this->setStatic();
}

//----------------------------------------------------------------
// Procedure: evalCPA
//   Purpose: Evaluates the given <Course, Speed, Time-on-leg> tuple 
//            Determines Closest-Point-of-Approach (CPA)

double CPAEngine::evalCPA(double osCRS, double osSPD, 
			  double osTOL, double *calcROC)
{
  osCRS = angle360(osCRS);

  double gamOS  = degToRadians(osCRS);    // Angle in radians.
  double cgamOS = cos(gamOS);             // Cosine of Angle (osCRS).
  double sgamOS = sin(gamOS);             // Sine   of Angle (osCRS).

  double k2 = statK2;
  double k1 = statK1;
  double k0 = statK0;
  
  if((cnCRS==osCRS) && (cnSPD==osSPD)) {
    if(calcROC)
      *calcROC = 0;
    return(sqrt(k0));                    // be 0, resuling in NaN.
  }
  
  k2 +=          cgamOS * cgamOS * osSPD * osSPD;   // (1,1)(a)
  k2 +=          sgamOS * sgamOS * osSPD * osSPD;   // (1,1)(b)
  k2 += (-2.0) * cgamOS * osSPD * cgamCN * cnSPD;   // (1,3)(3,1)(a)
  k2 += (-2.0) * sgamOS * osSPD * sgamCN * cnSPD;   // (1,3)(3,1)(b)

  k1 += ( 2.0) * cgamOS * osSPD * osLAT;  // (1,2)(2,1)(a)
  k1 += ( 2.0) * sgamOS * osSPD * osLON;  // (1,2)(2,1)(b)
  k1 += (-2.0) * cgamOS * osSPD * cnLAT;  // (1,4)(4,1)(a)
  k1 += (-2.0) * sgamOS * osSPD * cnLON;  // (1,4)(4,1)(b)

  double cpaDist;
  double minT = 0;
  if(k2 != 0)
    minT = ((-1.0) * k1) / (2.0 * k2);

  if(minT <= 0) 
    cpaDist = sqrt(k0); 
  else { 
    if(minT >= osTOL)
      minT = osTOL;
    double dist_squared = (k2*minT*minT) + (k1*minT) + k0;
    if(dist_squared < 0)
      cpaDist = 0;
    else
      cpaDist = sqrt(dist_squared);
  }

  if(calcROC)
    *calcROC = -k1;

  return(cpaDist);
}

//----------------------------------------------------------------
// Procedure: evalROC
//   Purpose: Determine rate-of-closure for a given heading,speed

double CPAEngine::evalROC(double osCRS, double osSPD)
{
  osCRS = angle360(osCRS);
  
  double gamOS  = degToRadians(osCRS);    // Angle in radians.
  double cgamOS = cos(gamOS);             // Cosine of Angle (osCRS).
  double sgamOS = sin(gamOS);             // Sine   of Angle (osCRS).

  double k1 = statK1;
  
  if((cnCRS==osCRS) && (cnSPD==osSPD))
    return(0);
  
  k1 += ( 2.0) * cgamOS * osSPD * osLAT;  // (1,2)(2,1)(a)
  k1 += ( 2.0) * sgamOS * osSPD * osLON;  // (1,2)(2,1)(b)
  k1 += (-2.0) * cgamOS * osSPD * cnLAT;  // (1,4)(4,1)(a)
  k1 += (-2.0) * sgamOS * osSPD * cnLON;  // (1,4)(4,1)(b)

  return(-k1);
}

//----------------------------------------------------------------
// Procedure: minMaxROC
//   Purpose: Determine max Rate-Of-Closure for a given number of  
//            headings and max_speed

double CPAEngine::minMaxROC(double speed, double heading_clicks, 
			    double &min_roc, double& max_roc)
{
  double heading_delta = 360.0 / heading_clicks;
  
  double max_heading;
  
  double try_roc;
  double try_heading = 0;
  while(try_heading < 360) {
    evalCPA(try_heading, speed, 60, &try_roc);
    if((try_heading == 0) || (try_roc > max_roc)) {
      max_heading = try_heading;
      max_roc = try_roc;
    }
    if((try_heading == 0) || (try_roc < min_roc))
      min_roc = try_roc;

    try_heading += heading_delta;
  }

  return(max_heading);
}


//----------------------------------------------------------------
// Procedure: crossesBow
//   Purpose: Determine if for the given ownship course and speed
//            whether it is on a path to cross the path of the 
//            contact on its bow.
//   Returns: true if it will cross its bow.
//            false otherwise.
//
//                B        a          C                          
//                 z-----------------y                           
//                /                 /                            
//               /               /                               
//              /             /                                  
//          c  /           /        sinA    sinB    sinC          
//            /         /           ---- == ---- == ----          
//           /       /    b          a       b       c            
//          /     /                                              
//         /   /         x: ownship's position t0                
//        / /            y: contact's position t0                
//       x               z: where the two paths cross            
//      A                                                        
//                                                               

bool CPAEngine::crossesBow(double osCRS, double osSPD)
{
  // First handle special case where osSPD is zero
  if(osSPD == 0)
    return(false);

  // Then handle special case where ownship, currently IS on the
  // "bowline", or path, of the contact. If so, only answer true
  // if the ownship course heads directly at the contact.

  osCRS = angle360(osCRS);
  double angle_cn_to_os = relAng(cnLON, cnLAT, osLON, osLAT);
  double angle_os_to_cn = relAng(osLON, osLAT, cnLON, cnLAT);
  if(angle_cn_to_os == cnCRS) {
    if(osCRS == angle_os_to_cn)
      return(true);
    else
      return(false);
  }

  // First determine, based solely on ownship course whether it
  // could cross the contact bow given an infinite ownship speed.

  bool contained = containsAngle(statCLOW, statCHGH, osCRS);
  //cout << "statCLOW:" << statCLOW << "  statCHGH:" << statCHGH;
  //cout << "  osCRS:" << osCRS << "  result:" << contained << endl;
  if(!contained)
    return(false);

  double dist_b  = statCNDIS;
  double angle_a = smallAngle(osCRS, statCNANG);

  //double feasible_angle_range = angle360(statCHGH - statCLOW);
  //double angle_b = feasible_angle_range - angle_a;
  assert(statCRNG >= 0);
  //cout << "statCRNG: " << statCRNG << endl;
  double angle_b = statCRNG - angle_a;
  assert(angle_b >= 0);
  double angle_c = (180.0 - (angle_a + angle_b));
  assert(angle_c >= 0);

  double sin_angle_a = sin(degToRadians(angle_a));
  double sin_angle_b = sin(degToRadians(angle_b));
  double sin_angle_c = sin(degToRadians(angle_c));

  double dist_a = sin_angle_a * (dist_b / sin_angle_b);
  double dist_c = sin_angle_c * (dist_b / sin_angle_b);

  double secs_c = dist_c / osSPD;
  double secs_a = dist_a / cnSPD;

  bool crosses_bow = (secs_c <= secs_a);

  return(crosses_bow);
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

void CPAEngine::setStatic()
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

  // Set static variables for quickening the "crossingType" queries
  statCNDIS = distPointToPoint(osLON, osLAT, cnLON, cnLAT);
  statCNANG = relAng(osLON, osLAT, cnLON, cnLAT);
  if((cnCRS - statCNANG) <= 180) {
    statCLOW = statCNANG;
    statCHGH = cnCRS;
  }
  else {
    statCLOW = cnCRS;
    statCHGH = statCNANG;
  }

  double ang1 = angle360(statCHGH - statCLOW);
  double ang2 = angle360(statCLOW - statCHGH);

  if(ang1 < ang2)
    statCRNG = ang1;
  else
    statCRNG = ang2;

#if 0
  double cn_angle_to_ownship = relAng(cnLON, cnLAT, osLON, osLAT);
  double opposite_cnCRS = angle360(cnCRS + 180);

  if(containsAngle(cnCRS, opposite_cnCRS)) {
    statCLOW = opposite_cnCRS;
    statCHGH = cnCRS;
  }
  else {
    statCLOW = cnCRS;
    statCHGH = opposite_cnCRS;
  }
#endif
}



//----------------------------------------------------------------
// Procedure: smallAngle
//   Purpose: 

double CPAEngine::smallAngle(double ang_a, double ang_b)
{
  double ang1 = angle360(ang_a - ang_b);
  double ang2 = angle360(ang_b - ang_a);
  if(ang1 < ang2)
    return(ang1);
  else
    return(ang2);
}









