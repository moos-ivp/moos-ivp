/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CPAEngineX.cpp                                       */
/*    DATE: May 12th 2005                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include <cmath> 
#include <cassert>
#include "CPAEngineX.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

#define MPI 3.14159265359

//----------------------------------------------------------
// Procedure: Constructor

CPAEngineX::CPAEngineX()
{
  cnLAT = 0;
  cnLON = 0;
  cnSPD = 0;
  cnCRS = 0;
  osLAT = 0;
  osLON = 0;

  m_counter  = 0;

  initTrigCache();
}

//----------------------------------------------------------
// Procedure: Constructor
//      args: gcnlat  Given Contact Latitude Position
//      args: gcnlon  Given Contact Longitude Position
//      args: gcncrs  Given Contact Course
//      args: gcnspd  Given Contact Speed
//      args: goslat  Given Ownship Latitude Position
//      args: goslon  Given Ownship Latitude Position

CPAEngineX::CPAEngineX(double gcnlat, double gcnlon, double gcncrs,
		       double gcnspd, double goslat, double goslon)
{
  cnLAT = gcnlat; 
  cnLON = gcnlon;
  cnSPD = gcnspd;
  cnCRS = angle360(gcncrs);
  osLAT = goslat;
  osLON = goslon;
  m_counter = 0;

  if(cnSPD < 0)
    cnSPD = 0;
  setStatic();
  initTrigCache();
  initRateCache();
}

//----------------------------------------------------------
// Procedure: reset

void CPAEngineX::reset(double gcnlat, double gcnlon, double gcncrs,
		       double gcnspd, double goslat, double goslon)
{
  cnLAT = gcnlat; 
  cnLON = gcnlon;
  cnSPD = gcnspd;
  cnCRS = angle360(gcncrs);
  osLAT = goslat;
  osLON = goslon;

  if(cnSPD < 0)
    cnSPD = 0;


  //cnLON = 100;
  //cnLAT = 100;
  //cnCRS = 225;
  //cnSPD = 2.0;

  //osLON = 50;
  //osLAT = 50;
  
  setStatic();
  if(m_cos_cache.size() == 0)
    initTrigCache();
  initRateCache();


  //exit(0);
}

//----------------------------------------------------------------
// Procedure: evalCPA
//   Purpose: Evaluates the given <Course, Speed, Time-on-leg> tuple 
//            Determines Closest-Point-of-Approach (CPA)

double CPAEngineX::evalCPA(double osCRS, double osSPD, 
			   double osTOL, double *calcROC) const
{
  if((osCRS >= 360) || (osCRS < 0))
    osCRS = angle360(osCRS);

  if(m_stat_cn_to_os_closing) {
    if(osSPD >= m_os_vthresh_cache_360[(unsigned int)(osCRS)])
      return(statRange);
  }
  else {
    if(osSPD <= m_os_vthresh_cache_360[(unsigned int)(osCRS)]) 
      return(statRange);
  }
  
  double k2 = statK2;
  double k1 = statK1;
  double k0 = statK0;
  
#if 0
  double gamOS  = degToRadians(osCRS);    // Angle in radians.
  double cgamOS = cos(gamOS);             // Cosine of Angle (osCRS).
  double sgamOS = sin(gamOS);             // Sine   of Angle (osCRS).
#endif

#if 1
  double cgamOS = m_cos_cache[(unsigned int)(osCRS)];            
  double sgamOS = m_sin_cache[(unsigned int)(osCRS)];
  double cgamOSQ = m_cos_sq_cache[(unsigned int)(osCRS)];            
  double sgamOSQ = m_sin_sq_cache[(unsigned int)(osCRS)];
#endif

  //double cgamOS_x_osSPD = cgamOS * osSPD;
  //double sgamOS_x_osSPD = sgamOS * osSPD;

  if((cnCRS==osCRS) && (cnSPD==osSPD)) {
    if(calcROC)
      *calcROC = 0;
    return(sqrt(k0));                     
  }
  
  k1 += ( 2.0) * cgamOS * osSPD * osLAT;  // (1,2)(2,1)(a)
  k1 += ( 2.0) * sgamOS * osSPD * osLON;  // (1,2)(2,1)(b)
  k1 += (-2.0) * cgamOS * osSPD * cnLAT;  // (1,4)(4,1)(a)
  k1 += (-2.0) * sgamOS * osSPD * cnLON;  // (1,4)(4,1)(b)

#if 1
  if(k1 > 0)  // vehicles are opening
    return(sqrt(k0));                    
#endif
  
  k2 += cgamOSQ * osSPD * osSPD;   // (1,1)(a)
  k2 += sgamOSQ * osSPD * osSPD;   // (1,1)(b)
  k2 += cgamOS  * osSPD * stat_cgamCNxcnSPD;   // (1,3)(3,1)(a)
  k2 += sgamOS  * osSPD * stat_sgamCNxcnSPD;   // (1,3)(3,1)(b)
  //k2 += (-2.0) * cgamOS * osSPD * stat_cgamCNxcnSPD;   // (1,3)(3,1)(a)
  //k2 += (-2.0) * sgamOS * osSPD * stat_sgamCNxcnSPD;   // (1,3)(3,1)(b)
  //  k2 += (-2.0) * cgamOS * osSPD * cgamCN * cnSPD;   // (1,3)(3,1)(a)
  //  k2 += (-2.0) * sgamOS * osSPD * sgamCN * cnSPD;   // (1,3)(3,1)(b)

  double cpaDist;
  double minT = 0;
  if(k2 != 0)
    minT = k1 / (-2.0 * k2);

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

double CPAEngineX::evalROC(double osCRS, double osSPD) const
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
// Procedure: crossesLines
//   Purpose: checks to see if the two lines determined by the 
//            present ownship and contact headings, cross. 

bool CPAEngineX::crossesLines(double osCRS) const
{
  // Step 1: check for parallel lines. If not parallel return true
  double delta_angle = angle360(osCRS - cnCRS);
  if((delta_angle != 0) && (delta_angle != 180))
    return(true);
    
  // Step 2: check if the parallel lines cross by checking if the 
  // contact is on the ownship line.
  
  // Step 2A: check ownship and contact on at the same present position
  if((osLAT == cnLAT) && (osLON == cnLON))
    return(true);

  // Step 2B: check the relative angle of the contact to ownship
  double ang_os_to_cn = relAng(osLON, osLAT, cnLON, cnLAT);

  if(ang_os_to_cn == osCRS)
    return(true);
  if(ang_os_to_cn == angle360(osCRS-180))
    return(true);

  return(false);
}

//----------------------------------------------------------------
// Procedure: minMaxROC
//   Purpose: Determine max Rate-Of-Closure for a given number of  
//            headings and max_speed

double CPAEngineX::minMaxROC(double speed, double heading_clicks, 
			    double &min_roc, double& max_roc) const
{
  double heading_delta = 360.0 / heading_clicks;
  
  double max_heading = 0;
  
  double try_roc = 0;
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
// Procedure: bearingRateOSCN() 

double CPAEngineX::bearingRateOSCN(double osCRS, double osSPD)
{
  // Part 1: Calculate thetaK and vK, the sum of the ownship and 
  //         contact velocity vectors.
  
  double xdot = 0;
  xdot += cos(degToRadians(osCRS)) * osSPD;
  xdot += cos(degToRadians(cnCRS)) * cnSPD;
  double ydot = 0;
  ydot += sin(degToRadians(osCRS)) * osSPD;
  ydot += sin(degToRadians(cnCRS)) * cnSPD;

  double kh = radToDegrees(atan(ydot/xdot));
  double kv = hypot(xdot, ydot);

  double relang_os_to_cn = relAng(osLON, osLAT, cnLON, cnLAT);
  
  double tangent_angle = angle360(relang_os_to_cn + 90 - kh);

  double spd_at_tangent_angle = cos(degToRadians(tangent_angle)) * kv;
  
  double bng_rate = spd_at_tangent_angle * (360 / (2*statRange*MPI));

  return(bng_rate);
}

//----------------------------------------------------------------
// Procedure: bearingRateOSCN() 

double CPAEngineX::bearingRateOSCN(double osh, double osv, double time)
{
  if(time <= 0)
    return(0);
  
  double osx1 = osLON;
  double osy1 = osLAT;
  double cnx1 = cnLON;
  double cny1 = cnLAT;

  double os_dist = (osv * time);
  double cn_dist = (cnSPD * time);

  // Create an ownship leg a bit forward and backward in time
  double osx0, osy0, osx2, osy2;
  projectPoint(osh-180, os_dist, osx1, osy1, osx0, osy0);
  projectPoint(osh,     os_dist, osx1, osy1, osx2, osy2);

  // Create a contact leg a bit forward and backward in time
  double cnx0, cny0, cnx2, cny2;
  projectPoint(cnCRS-180, cn_dist, cnx1, cny1, cnx0, cny0);
  projectPoint(cnCRS,     cn_dist, cnx1, cny1, cnx2, cny2);

  double relang0 = relAng(osx0, osy0, cnx0, cny0);
  double relang2 = relAng(osx2, osy2, cnx2, cny2);
  
  double diff = relang2 - relang0;
  if(diff < 180)
    diff += 360;
  if(diff > 180)
    diff -= 360;
  
  double rate = (diff / (time*2));
  return(rate);
}


//----------------------------------------------------------------
// Procedure: bearingRateCNOS

double CPAEngineX::bearingRateCNOS(double osh, double osv, double time)
{
  if(time <= 0)
    return(0);
  
  double osx1 = osLON;
  double osy1 = osLAT;
  double cnx1 = cnLON;
  double cny1 = cnLAT;

  double os_dist = (osv * time);
  double cn_dist = (cnSPD * time);

  // Create an ownship leg a bit forward and backward in time
  double osx0, osy0, osx2, osy2;
  projectPoint(osh-180, os_dist, osx1, osy1, osx0, osy0);
  projectPoint(osh,     os_dist, osx1, osy1, osx2, osy2);

  // Create a contact leg a bit forward and backward in time
  double cnx0, cny0, cnx2, cny2;
  projectPoint(cnCRS-180, cn_dist, cnx1, cny1, cnx0, cny0);
  projectPoint(cnCRS,     cn_dist, cnx1, cny1, cnx2, cny2);

  double relang0 = relAng(cnx0, cny0, osx0, osy0);
  double relang2 = relAng(cnx2, cny2, osx2, osy2);
  
  double diff = relang2 - relang0;
  if(diff < 180)
    diff += 360;
  if(diff > 180)
    diff -= 360;
  
  double rate = (diff / (time*2));
  return(rate);
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

void CPAEngineX::setStatic()
{
  // Part 1: Determine the speed of contact in the direction of ownship
  double relbng_cn_to_os = relBearing(cnLON, cnLAT, cnCRS, osLON, osLAT);
  m_stat_cn_to_os_spd = cnSPD * cos(degToRadians(relbng_cn_to_os));
  m_stat_cn_to_os_closing = (m_stat_cn_to_os_spd > 0);

#if 0
  cout << "==============================================================" << endl;
  cout << "CPAEngineX::setStatic()" << endl;
  cout << "cnLAT: " << cnLAT << endl;
  cout << "cnLON: " << cnLON << endl;
  cout << "cnCRS: " << cnCRS << endl;
  cout << "cnSPD: " << cnSPD << endl;
  cout << "relbng_cn_to_os: " << relbng_cn_to_os << endl;
  cout << "m_stat_cn_to_os_spd: " << m_stat_cn_to_os_spd << endl;
  cout << "m_stat_cn_to_os_spd_closing " << boolToString(m_stat_cn_to_os_closing) << endl;
  cout << "==============================================================" << endl;
#endif

  
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

  // Create the contact line segment representing its inf line
  stat_cnx1 = cnLON;
  stat_cny1 = cnLAT;
  projectPoint(cnCRS, 100, stat_cnx1, stat_cny1, stat_cnx2, stat_cny2);

  // check of os is on the present contact position
  stat_os_on_contact = false;
  if((osLON == cnLON) && (osLAT == cnLAT))
    stat_os_on_contact = true;


  // check if ownship currently IS on the sternline of the contact. 
  stat_os_on_bowline = false;
  stat_os_on_sternline = false;
  double angle_cn_to_os = relAng(cnLON, cnLAT, osLON, osLAT);
  if(angle_cn_to_os == angle360(cnCRS))
    stat_os_on_bowline = true;
  else if(angle_cn_to_os == angle360(cnCRS-180))
    stat_os_on_sternline = true;

  stat_cgamCNxcnSPD = -2 * cgamCN * cnSPD;
  stat_sgamCNxcnSPD = -2 * sgamCN * cnSPD;

  statRange = sqrt(statK0); 
  
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

double CPAEngineX::smallAngle(double ang_a, double ang_b) const
{
  double ang1 = angle360(ang_a - ang_b);
  double ang2 = angle360(ang_b - ang_a);
  if(ang1 < ang2)
    return(ang1);
  else
    return(ang2);
}


//----------------------------------------------------------------
// Procedure: crossesBow  (Convenience function)

bool CPAEngineX::crossesBow(double osCRS, double osSPD) const
{
  double dist;
  return(crossesBowDist(osCRS, osSPD, dist));
}

//----------------------------------------------------------------
// Procedure: crossesBowDist  (Convenience function)

double CPAEngineX::crossesBowDist(double osCRS, double osSPD) const
{
  double dist;
  crossesBowDist(osCRS, osSPD, dist);
  return(dist);
}

//----------------------------------------------------------------
// Procedure: crossesBowDist

bool CPAEngineX::crossesBowDist(double osCRS, double osSPD, double& xdist) const
{
  // Special case 1: ownship and contact position are the same
  if(stat_os_on_contact) {
    xdist = 0;
    return(true);
  }
  
  // Special case 2: ownship crossing the contact sternline now
  if(stat_os_on_bowline) {
    xdist = distPointToPoint(osLON, osLAT, cnLON, cnLAT);
    return(true);
  }

  // Special case 3: ownship has speed zero - cannot ever reach bow-stern line
  if(osSPD == 0) {
    xdist = -1;
    return(false);
  }

  // Create ownship line segment
  double x1 = osLON;
  double y1 = osLAT;
  double x2, y2;
  projectPoint(osCRS, 100, x1, y1, x2, y2);

  // Create contact line segment from cached values
  double x3 = stat_cnx1;
  double y3 = stat_cny1;
  double x4 = stat_cnx2;
  double y4 = stat_cny2;
  
  // Determine exactly where ownship crosses the contact bow-stern line
  double intx, inty;
  bool lines_cross = linesCross(x1,y1,x2,y2,x3,y3,x4,y4,intx,inty);

  if(!lines_cross) {
    xdist = -1;
    return(false);
  }

  // Check if the crossing point is in front of ownship. If it is not, 
  // then it doesn't cross the bow (or stern).
  double ang_os_to_cross_pt = relAng(osLON, osLAT, intx, inty);
  double os_delta_angle     = angle180(osCRS - ang_os_to_cross_pt);
  if(os_delta_angle < 0)
    os_delta_angle *= -1;
  if(os_delta_angle > 10) {
    xdist = -1;
    return(false);
  }
  double os_dist_to_cross = distPointToPoint(osLON, osLAT, intx, inty);
  double cn_dist_to_cross = distPointToPoint(cnLON, cnLAT, intx, inty);
  double os_time_to_cross    = os_dist_to_cross / osSPD;
  
  //----------------------------------------------------------------
  // Handle another edge case (If contact speed < 0 treat as zero)
  //----------------------------------------------------------------
  if(cnSPD <= 0) {
    xdist = cn_dist_to_cross;
    return(true);
  }

  // Now that we sure cnSPD is not zero calculate cn_time_to_cross
  double cn_time_to_cross  = cn_dist_to_cross / cnSPD;

  // Determine if inf line crossing point is fore or aft of contact's 
  // present position
  bool   xpoint_fore_of_contact_now = false;
  double ang_cn_to_cross_pt = relAng(cnLON, cnLAT, intx, inty);
  double cn_delta_angle = angle180(cnCRS - ang_cn_to_cross_pt);
  if(cn_delta_angle < 0)
    cn_delta_angle *= -1;
  if(cn_delta_angle < 10) 
    xpoint_fore_of_contact_now = true;

  // If the contact is already ahead of the cross point, or if the 
  // the contact gets to the crosspoint first, then ownwhip is on a
  // course to cross the contact's stern, not the bow.

  // If the xpoint is aft of the contact now, or  gets there first,
  // then ownship is crossing the contact's bow, not the stern.
  if(!xpoint_fore_of_contact_now || (cn_time_to_cross < os_time_to_cross)) {
    xdist = -1;
    return(false);
  }

  // ===================================================================
  // Now we know ownship crosses the contact's bowline, but how far in
  // front of the contact will it be when it does so?
  // ===================================================================

  double cn_dist_travelled_when_os_crosses_bowline = os_time_to_cross * cnSPD;
  
  xdist = cn_dist_to_cross - cn_dist_travelled_when_os_crosses_bowline;

  return(true);
}



//----------------------------------------------------------------
// Procedure: crossesStern()                  Convenience function
//
//   Purpose: Determine if for the given ownship course and speed
//            whether it is on a path to cross the path of the 
//            contact on its stern.

bool CPAEngineX::crossesStern(double osCRS, double osSPD) const
{
  double xdist;
  return(crossesSternDist(osCRS, osSPD, xdist));
}

//----------------------------------------------------------------
// Procedure: crossesStern()                  Convenience function
//
//   Purpose: Determine if for the given ownship course and speed
//            whether it is on a path to cross the path of the 
//            contact on its stern. And if so, at what distance when
//            it crosses?

double CPAEngineX::crossesSternDist(double osCRS, double osSPD) const
{
  double xdist;
  crossesSternDist(osCRS, osSPD, xdist);
  return(xdist);
}


//----------------------------------------------------------------
// Procedure: crossesSternDist
//   Purpose: Determine if for the given ownship course and speed
//            whether it is on a path to cross the path of the 
//            contact on its stern. And if so, at what distance when
//            it crosses?

bool CPAEngineX::crossesSternDist(double osCRS, double osSPD, double& xdist) const
{
  // Special case 1: ownship and contact position are the same
  if(stat_os_on_contact) {
    xdist = 0;
    return(true);
  }
  
  // Special case 2: ownship crossing the contact sternline now
  if(stat_os_on_sternline) {
    xdist = distPointToPoint(osLON, osLAT, cnLON, cnLAT);
    return(true);
  }

  // Special case 3: ownship has speed zero - cannot ever reach bow-stern line
  if(osSPD == 0) {
    xdist = -1;
    return(false);
  }

  // Create ownship line segment
  double x1 = osLON;
  double y1 = osLAT;
  double x2, y2;
  projectPoint(osCRS, 100, x1, y1, x2, y2);

  // Create contact line segment from cached values
  double x3 = stat_cnx1;
  double y3 = stat_cny1;
  double x4 = stat_cnx2;
  double y4 = stat_cny2;
  
  // Determine exactly where ownship crosses the contact bow-stern line
  double intx, inty;
  bool lines_cross = linesCross(x1,y1,x2,y2,x3,y3,x4,y4,intx,inty);

  if(!lines_cross) {
    xdist = -1;
    return(false);
  }

  // Check if the crossing point is in front of ownship. If it is not, 
  // then it doesn't cross the stern (or bow).
  double ang_os_to_cross_pt = relAng(osLON, osLAT, intx, inty);
  double os_delta_angle     = angle180(osCRS - ang_os_to_cross_pt);
  if(os_delta_angle < 0)
    os_delta_angle *= -1;
  if(os_delta_angle > 10) {
    xdist = -1;
    return(false);
  }
  double os_dist_to_cross = distPointToPoint(osLON, osLAT, intx, inty);
  double cn_dist_to_cross = distPointToPoint(cnLON, cnLAT, intx, inty);
  double os_time_to_cross = os_dist_to_cross / osSPD;
  
  //----------------------------------------------------------------
  // Handle another edge case (If contact speed < 0 treat as zero)
  //----------------------------------------------------------------
  if(cnSPD <= 0) {
    xdist = cn_dist_to_cross;
    return(true);
  }

  // Now that we sure cnSPD is not zero calculate cn_time_to_cross
  double cn_time_to_cross  = cn_dist_to_cross / cnSPD;

  // Determine if inf line crossing point is fore or aft of contact's 
  // present position
  bool   xpoint_fore_of_contact_now = false;
  double ang_cn_to_cross_pt = relAng(cnLON, cnLAT, intx, inty);
  double cn_delta_angle = angle180(cnCRS - ang_cn_to_cross_pt);
  if(cn_delta_angle < 0)
    cn_delta_angle *= -1;
  if(cn_delta_angle < 10) 
    xpoint_fore_of_contact_now = true;

  // If the xpoint is front of the contact now, and ownship gets there first,
  // then ownship is crossing the contact's bow, not the stern.
  if(xpoint_fore_of_contact_now && (os_time_to_cross < cn_time_to_cross)) {
    xdist = -1;
    return(false);
  }

  // ===================================================================
  // Now we know ownship crosses the contact's sternline, but how far in
  // back of the contact will it be when it does so?
  // ===================================================================

  double cn_dist_travelled_when_os_crosses_sternline = os_time_to_cross * cnSPD;
  
  if(xpoint_fore_of_contact_now)
    xdist = cn_dist_travelled_when_os_crosses_sternline - cn_dist_to_cross;
  else 
    xdist = cn_dist_to_cross + cn_dist_travelled_when_os_crosses_sternline;

  return(true);
}


//----------------------------------------------------------------
// Procedure: crossesBowOrStern
//   Purpose: Determine if for the given ownship course and speed
//            whether it is on a path to cross the path of the 
//            contact on its stern or bow. 

bool CPAEngineX::crossesBowOrStern(double osCRS, double osSPD) const
{
  // Special cases
  if(stat_os_on_contact || stat_os_on_sternline || stat_os_on_bowline)
    return(true);

  // Special case: ownship has speed zero - cannot ever reach bow-stern line
  if(osSPD == 0) 
    return(false);

  // Create ownship line segment
  double x1 = osLON;
  double y1 = osLAT;
  double x2, y2;
  projectPoint(osCRS, 100, x1, y1, x2, y2);

  // Create contact line segment from cached values
  double x3 = stat_cnx1;
  double y3 = stat_cny1;
  double x4 = stat_cnx2;
  double y4 = stat_cny2;
  
  // Determine exactly where ownship crosses the contact bow-stern line
  double intx, inty;
  bool lines_cross = linesCross(x1,y1,x2,y2,x3,y3,x4,y4,intx,inty);

  if(!lines_cross) 
    return(false);

  // Check if the crossing point is in front of ownship. If it is not, 
  // then it doesn't cross the stern (or bow).
  double ang_os_to_cross_pt = relAng(osLON, osLAT, intx, inty);
  double os_delta_angle     = angle180(osCRS - ang_os_to_cross_pt);
  if(os_delta_angle < 0)
    os_delta_angle *= -1;
  if(os_delta_angle > 10) 
    return(false);
  return(true);
}


//----------------------------------------------------------------
// Procedure: turnsRight
//   Purpose: Determine if for the present ownship heading, whether or not
//            the proposed heading represent a righthand turn (starboard)

bool CPAEngineX::turnsRight(double present_heading, double heading) const
{
  double delta = angle360(heading - present_heading);
  if((delta > 0) && (delta < 180))
    return(true);
  return(false);
}
  

//----------------------------------------------------------------
// Procedure: turnsLeft
//   Purpose: Determine if for the present ownship heading, whether or not
//            the proposed heading represent a lefthand turn (port)

bool CPAEngineX::turnsLeft(double present_heading, double heading) const
{
  double delta = angle360(heading - present_heading);
  if(delta > 180)
    return(true);
  return(false);
}
  

//----------------------------------------------------------------
// Procedure: passesContact()
//   Purpose: checks to see if ownship, on the given heading and speed,
//            will pass the contact. A "pass" means it will cross the 
//            line perpendicular to the bow-stern line. (crosses the beam)

bool CPAEngineX::passesContact(double osCRS, double osSPD) const
{
  bool os_fore_of_contact = foreOfContact();
  bool os_aft_of_contact  = aftOfContact();

  // Case 0: Ownship is ON the contact crossing line, return true
  if(os_fore_of_contact && os_aft_of_contact)
    return(true);

  double spd_in_cn_heading = speedInHeading(osCRS, osSPD, cnCRS);

  if(os_aft_of_contact) {
    if(spd_in_cn_heading > cnSPD)
      return(true);
    else
      return(false);
  }
  else {  // os_fore_of_contact
    if(spd_in_cn_heading >= cnSPD)
      return(false);
    else
      return(true);
  }
}

//----------------------------------------------------------------
// Procedure: passesContactPort()
//   Purpose: checks to see if ownship, on the given heading and speed,
//            will pass the contact on the contact's port side. 
//            A "pass" means it will cross the line perpendicular to the 
//            bow-stern line.

bool CPAEngineX::passesContactPort(double osCRS, double osSPD) const
{
  //============================================================
  // Handle Special Cases
  //============================================================
  // Special Case 1: ownship and contact are on the same point
  if((osLON == cnLON) && (osLAT == cnLAT))
    return(false);

  // Special Case 2: ownship does not pass the contact at all
  if(!passesContact(osCRS, osSPD))
    return(false);

  bool os_aft_of_contact  = aftOfContact();
  bool os_fore_of_contact = foreOfContact();
  bool os_port_of_contact = portOfContact();
  bool os_starboard_of_contact = starboardOfContact();

  // Special Case 3: ownship is ON the bow-stern line
  if(os_port_of_contact && os_starboard_of_contact) {
    double os_to_cn_rel_bng = relBearing(osLON, osLAT, osCRS, cnLON, cnLAT);
    if(os_fore_of_contact) {
      if(os_to_cn_rel_bng >= 180)
	return(true);
      else
	return(false);
    }
    else { // os is aft of contact
      if(os_to_cn_rel_bng >= 180)
	return(false);
      else
	return(true);
    }
  }

  // Special Case 4: ownship is on the perpendicular bow-stern line
  if(os_aft_of_contact && os_fore_of_contact)
    return(os_port_of_contact);

  //============================================================
  // Handle General Cases
  //============================================================
  // Case #1: ownship is aft and port of contact              //
  //                           |                              //       
  //                           |                              //       
  //                           |                              //       
  //                          / \                             //       
  //                          | |                             //
  //          ----------------|C|-----------------            //
  //          Case:           | |                             //
  //          Port and        ---                             //
  //          Aft of contact   |                              //
  //                           |                              //
  //                           |                              //
  //            (Ownship)      |                              //
  //
  if((os_aft_of_contact)  && (os_port_of_contact))
    return(!crossesStern(osCRS, osSPD));


  // Case #2: ownship is aft and starboard of contact         //
  //                           |                              //
  //                           |                              //
  //                           |                              //
  //                          / \                             //
  //                          | |                             //
  //          ----------------|C|-----------------            //
  //                          | |   Case:                     //
  //                          ---   Starboard and             //
  //                           |    Aft of contact            //
  //                           |                              //
  //                           |    (Ownship)                 //
  //                           |                              //
  //
  if((os_aft_of_contact)  && (os_starboard_of_contact))
    return(crossesStern(osCRS, osSPD));


  // Case #3: ownship is fore and port of contact             //
  //                           |                              //
  //          Case:            |                              //
  //          port and         |                              //
  //          Fore of contact  |                              //
  //            (Ownship)     / \                             //
  //                          | |                             //
  //          ----------------|C|-----------------            //
  //                          | |                             //
  //                          ---                             //
  //                           |                              //
  //                           |                              //
  //                           |                              //
  //                           |                              //
  //
  if((os_fore_of_contact)  && (os_port_of_contact))
    return(!crossesBow(osCRS, osSPD));

  // Case #4: ownship is fore and starboard of contact        //
  //                           |                              //
  //                           |     Case:                    //
  //                           |     Starboard and            //
  //                           |     Fore of contact          //
  //                          / \      (Ownship)              //
  //                          | |                             //
  //          ----------------|C|-----------------            //
  //                          | |                             //
  //                          ---                             //
  //                           |                              //
  //                           |                              //
  //                           |                              //
  //                           |                              //
  //
  if((os_fore_of_contact)  && (os_starboard_of_contact))
    return(crossesBow(osCRS, osSPD));

  return(false);
}

//----------------------------------------------------------------
// Procedure: passesContactStarboard()
//   Purpose: checks to see if ownship, on the given heading and speed,
//            will pass the contact on the contact's starboard side. 
//            A "pass" means it will cross the line perpendicular to the 
//            bow-stern line.

bool CPAEngineX::passesContactStarboard(double osCRS, double osSPD) const
{
  //============================================================
  // Handle Special Cases
  //============================================================
  // Special Case 1: ownship and contact are on the same point
  if((osLON == cnLON) && (osLAT == cnLAT))
    return(false);

  // Special Case 2: ownship does not pass the contact at all
  if(!passesContact(osCRS, osSPD))
    return(false);

  if(!passesContactPort(osCRS, osSPD))
    return(true);
  return(false);
}

//----------------------------------------------------------------
// Procedure: foreOfContact
//   Purpose: Checks to see if ownship is presently fore of the contact.

bool CPAEngineX::foreOfContact() const
{
  // First, edge case where ownship and contact are exact same position
  if((osLAT == cnLAT) && (osLON == cnLON))
    return(false);
  
  // returns value in the range [0,360)
  double rel_bng = relBearing(cnLON, cnLAT, cnCRS, osLON, osLAT);
  if((rel_bng >= 0) && (rel_bng <= 90))
    return(true);

  if((rel_bng >= 270) && (rel_bng <= 360))
    return(true);
  
  return(false);
}

//----------------------------------------------------------------
// Procedure: aftOfContact
//   Purpose: Checks to see if ownship is presently aft of the contact.

bool CPAEngineX::aftOfContact() const
{
  // First, edge case where ownship and contact are exact same position
  if((osLAT == cnLAT) && (osLON == cnLON))
    return(false);
  
  // returns value in the range [0,360)
  double rel_bng = relBearing(cnLON, cnLAT, cnCRS, osLON, osLAT);
  if((rel_bng >= 90) && (rel_bng <= 270))
    return(true);
  
  return(false);
}

//----------------------------------------------------------------
// Procedure: portOfContact
//   Purpose: True if ownship is presently on the port side of the contact.
//      Note: If ownship is ON the bow or stern line, it will return true.

bool CPAEngineX::portOfContact() const
{
  // First, edge case where ownship and contact are exact same position
  if((osLAT == cnLAT) && (osLON == cnLON))
    return(false);
  
  // returns value in the range [0,360)
  double rel_bng = relBearing(cnLON, cnLAT, cnCRS, osLON, osLAT);
  if((rel_bng >= 180) && (rel_bng < 360))
    return(true);
  
  // If contact ON the bow-stern line, return true
  if(rel_bng == 0)
    return(true);


  return(false);
}

//----------------------------------------------------------------
// Procedure: starboardOfContact
//   Purpose: True if ownship is presently on the starboard side of the contact.
//      Note: If ownship is ON the bow or stern line, it will return true.

bool CPAEngineX::starboardOfContact() const
{
  // First, edge case where ownship and contact are exact same position
  if((osLAT == cnLAT) && (osLON == cnLON))
    return(false);
  
  // returns value in the range [0,360)
  double rel_bng = relBearing(cnLON, cnLAT, cnCRS, osLON, osLAT);
  if((rel_bng >= 0) && (rel_bng <= 180))
    return(true);
  
  return(false);
}


//----------------------------------------------------------------
// Procedure: initTrigCache

void CPAEngineX::initTrigCache()
{
  // Part 1: Create the fine resolution cache for use with contact
  vector<double> virgin_cache_3600(3600,0);

  m_cos_cache_3600 = virgin_cache_3600;
  m_sin_cache_3600 = virgin_cache_3600;

  for(unsigned int i=0; i<3600; i++) {
    double rad = degToRadians(i/10);
    m_cos_cache_3600[i] = cos((double)(rad));
    m_sin_cache_3600[i] = sin((double)(rad));
  }
  
  // Part 2: Create the course resolution cache for use with ownship
  vector<double> virgin_cache(360,0);
  m_cos_cache = virgin_cache;
  m_sin_cache = virgin_cache;
  m_cos_sq_cache = virgin_cache;
  m_sin_sq_cache = virgin_cache;

  for(unsigned int i=0; i<360; i++) {
    double rad = degToRadians(i);
    m_cos_cache[i] = cos((double)(rad));
    m_sin_cache[i] = sin((double)(rad));
    m_cos_sq_cache[i] = m_cos_cache[i] * m_cos_cache[i];
    m_sin_sq_cache[i] = m_sin_cache[i] * m_sin_cache[i];
  }
}
//----------------------------------------------------------------
// Procedure: initRateCache

void CPAEngineX::initRateCache()
{
  vector<double> virgin_cache(360,0);
  m_os_vthresh_cache_360 = virgin_cache;
  
  // Case 1: contact is closing ownship position, i.e., contact speed
  //         vector is in the direction of ownship rather than away.
  //         The cache will be a cache of speeds ownship must be
  //         greather than or equal to, if the current point in time
  //         is to be considered the CPA.

  if(m_stat_cn_to_os_closing) {
    double relang_cn_to_os = relAng(cnLON, cnLAT, osLON, osLAT);
    for(unsigned int i=0; i<360; i++) {
      double delta = (double)(i) - relang_cn_to_os;
      if(delta > 180) 
	delta -= 360;
      else if(delta < -180) 
	delta += 360;
      if(delta < 0)
	delta = -delta;
	

      double cos_delta = -777;
      double thresh = 999;

      if(delta < 90) {
	//cos_delta = m_cos_cache[(unsigned int)(delta)];
	cos_delta = cos(degToRadians(delta));
	if(cos_delta > 0.0001)
	  thresh = (m_stat_cn_to_os_spd / cos_delta);
      }
      m_os_vthresh_cache_360[i] = thresh;

#if 0      
      if(verbose) cout << "[" << i << "]A  thresh: " << thresh << endl;
      if(verbose) cout << "   relang_cn_to_os: " << relang_cn_to_os << endl;
      if(verbose) cout << "   os_heading: " << i << endl;
      if(verbose) cout << "   os_delta: " << delta << endl;
      if(verbose) cout << "   cos_delta: " << cos_delta << endl;
#endif
    }
  }
    
  // Case 2: contact is opening ownship position, i.e., contact speed
  //         vector is in direction away from ownship rather than toward.
  //         The cache will be a cache of speeds ownship must be less
  //         than or equal to, if the current point in time is to be 
  //         considered the CPA.

  else {
    double relang_os_to_cn = relAng(osLON, osLAT, cnLON, cnLAT);
    for(unsigned int i=0; i<360; i++) {
      double delta = (double)(i) - relang_os_to_cn;
      if(delta > 180)
	delta -= 360;
      else if(delta < -180) 
	delta += 360;
      if(delta < 0)
	delta = -delta;
      
      double cos_delta = -777;
      double thresh = 999;

      if(delta < 90) {
	cos_delta = cos(degToRadians(delta));	
	//cos_delta = m_cos_cache[(unsigned int)(delta)];
	if(cos_delta > 0.0001)
	  thresh = (-m_stat_cn_to_os_spd / cos_delta);
      }
      m_os_vthresh_cache_360[i] = thresh;

#if 0      
      if(verbose) cout << "[" << i << "]B  thresh: " << thresh << endl;
      if(verbose) cout << "   relang_os_to_cn: " << relang_os_to_cn << endl;
      if(verbose) cout << "   os_heading: " << i << endl;
      if(verbose) cout << "   os_delta: " << delta << endl;
      if(verbose) cout << "   cos_delta: " << cos_delta << endl;
#endif
    }
  }
}
