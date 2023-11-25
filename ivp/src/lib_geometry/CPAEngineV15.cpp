/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CPAEngineV15.cpp                                     */
/*    DATE: May 12th 2005                                        */
/*    NOTE: This version of the CPAEngine (V15) is not used in   */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include <cmath> 
#include <cassert>
#include "CPAEngineV15.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

CPAEngineV15::CPAEngineV15()
{
  cnLAT = 0;
  cnLON = 0;
  cnSPD = 0;
  cnCRS = 0;
  osLAT = 0;
  osLON = 0;
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

CPAEngineV15::CPAEngineV15(double gcnlat, double gcnlon, double gcncrs,
			 double gcnspd, double goslat, double goslon)
{
  cnLAT   = gcnlat; 
  cnLON   = gcnlon;
  cnSPD   = gcnspd;
  cnCRS   = angle360(gcncrs);
  osLAT   = goslat;
  osLON   = goslon;
  if(cnSPD < 0)
    cnSPD = 0;
  this->setStatic();
  initTrigCache();
}

//----------------------------------------------------------------
// Procedure: setContactCacheTimeDelta()

void CPAEngineV15::setContactCacheTimeDelta(double val)
{
  if(val > 0.1)
    m_cn_cache_tdelta = val;
}


//----------------------------------------------------------------
// Procedure: setContactCache()

void CPAEngineV15::setContactCache(double secs)
{
  m_cn_cache_x.clear();
  m_cn_cache_y.clear();

  if((secs < 0) || (m_cn_cache_tdelta < 0))
    return;

  unsigned int clicks = (unsigned int)(secs / m_cn_cache_tdelta);
  double dist = cnSPD * m_cn_cache_tdelta;

  double prev_x = cnLON;
  double prev_y = cnLAT;
  for(unsigned int i=0; i<clicks; i++) {
    double new_x, new_y;
    projectPoint(cnCRS, dist, prev_x, prev_y, new_x, new_y);
    m_cn_cache_x.push_back(new_x);
    m_cn_cache_y.push_back(new_x);
    prev_x = new_x;
    prev_y = new_y;
  }
}


//----------------------------------------------------------------
// Procedure: evalCPA
//   Purpose: Evaluates the given <Course, Speed, Time-on-leg> tuple 
//            Determines Closest-Point-of-Approach (CPA)

double CPAEngineV15::evalCPA(double osCRS, double osSPD, 
			  double osTOL, double *calcROC) const
{
  osCRS = angle360(osCRS);

  double k2 = statK2;
  double k1 = statK1;
  double k0 = statK0;
  
#if 1
  double gamOS  = degToRadians(osCRS);    // Angle in radians.
  double cgamOS = cos(gamOS);             // Cosine of Angle (osCRS).
  double sgamOS = sin(gamOS);             // Sine   of Angle (osCRS).
#endif

#if 0
  double cgamOS = m_cos_cache[(unsigned int)(osCRS)];            
  double sgamOS = m_sin_cache[(unsigned int)(osCRS)];
#endif

  if((cnCRS==osCRS) && (cnSPD==osSPD)) {
    if(calcROC)
      *calcROC = 0;
    return(sqrt(k0));                    // be 0, resuling in NaN.
  }
  
  k1 += ( 2.0) * cgamOS * osSPD * osLAT;  // (1,2)(2,1)(a)
  k1 += ( 2.0) * sgamOS * osSPD * osLON;  // (1,2)(2,1)(b)
  k1 += (-2.0) * cgamOS * osSPD * cnLAT;  // (1,4)(4,1)(a)
  k1 += (-2.0) * sgamOS * osSPD * cnLON;  // (1,4)(4,1)(b)

#if 0
  if(k1 > 0) // opening
    return(sqrt(k0));
#endif

  k2 +=          cgamOS * cgamOS * osSPD * osSPD;   // (1,1)(a)
  k2 +=          sgamOS * sgamOS * osSPD * osSPD;   // (1,1)(b)
  k2 += (-2.0) * cgamOS * osSPD * cgamCN * cnSPD;   // (1,3)(3,1)(a)
  k2 += (-2.0) * sgamOS * osSPD * sgamCN * cnSPD;   // (1,3)(3,1)(b)

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

double CPAEngineV15::evalROC(double osCRS, double osSPD) const
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

bool CPAEngineV15::crossesLines(double osCRS) const
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

double CPAEngineV15::minMaxROC(double speed, double heading_clicks, 
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
// Procedure: bearingRate

double CPAEngineV15::bearingRateOSCN(double osh, double osv, double time)
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

double CPAEngineV15::bearingRateCNOS(double osh, double osv, double time)
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

void CPAEngineV15::setStatic()
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

double CPAEngineV15::smallAngle(double ang_a, double ang_b) const
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

bool CPAEngineV15::crossesBow(double osCRS, double osSPD) const
{
  double dist;
  return(crossesBowDist(osCRS, osSPD, dist));
}

//----------------------------------------------------------------
// Procedure: crossesBowDist  (Convenience function)

double CPAEngineV15::crossesBowDist(double osCRS, double osSPD) const
{
  double dist;
  crossesBowDist(osCRS, osSPD, dist);
  return(dist);
}

//----------------------------------------------------------------
// Procedure: crossesBowDist

bool CPAEngineV15::crossesBowDist(double osCRS, double osSPD, double& xdist) const
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

bool CPAEngineV15::crossesStern(double osCRS, double osSPD) const
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

double CPAEngineV15::crossesSternDist(double osCRS, double osSPD) const
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

bool CPAEngineV15::crossesSternDist(double osCRS, double osSPD, double& xdist) const
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

bool CPAEngineV15::crossesBowOrStern(double osCRS, double osSPD) const
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

bool CPAEngineV15::turnsRight(double present_heading, double heading) const
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

bool CPAEngineV15::turnsLeft(double present_heading, double heading) const
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

bool CPAEngineV15::passesContact(double osCRS, double osSPD) const
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

bool CPAEngineV15::passesContactPort(double osCRS, double osSPD) const
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

bool CPAEngineV15::passesContactStarboard(double osCRS, double osSPD) const
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

bool CPAEngineV15::foreOfContact() const
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

bool CPAEngineV15::aftOfContact() const
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

bool CPAEngineV15::portOfContact() const
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

bool CPAEngineV15::starboardOfContact() const
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

void CPAEngineV15::initTrigCache()
{
  vector<double> new_cos_cache(360,0);
  vector<double> new_sin_cache(360,0);

  m_cos_cache = new_cos_cache;
  m_sin_cache = new_sin_cache;

  for(unsigned int i=0; i<360; i++) {
    double rad = degToRadians(i);
    m_cos_cache[i] = cos((double)(rad));
    m_sin_cache[i] = sin((double)(rad));
  }
}

