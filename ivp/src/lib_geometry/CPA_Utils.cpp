/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: CPA_Utils.cpp                                        */
/*    DATE: Mar 11, 2007 Sat afternoon at Brugger's              */
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
 
#include <cmath>
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "CPA_Utils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: evalCPA
//   Purpose: 

double evalCPA(double cnLON, double cnLAT, double cnSPD, double cnCRS,
	       double osLON, double osLAT, double osSPD, double osCRS,
	       double osTOL)
{

  double gamCN   = degToRadians(cnCRS);  // Angle in radians.
  double cgamCN  = cos(gamCN);           // Cosine of Angle (cnCRS).
  double sgamCN  = sin(gamCN);           // Sine   of Angle (cnCRS).

  double statK2  = (+1.0) * cgamCN * cgamCN * cnSPD * cnSPD;    //(3,3)(a)
  statK2 += (+1.0) * sgamCN * sgamCN * cnSPD * cnSPD;    //(3,3)(b)

  double statK1  = (-2.0) * osLAT * cgamCN * cnSPD;    // (2,3)(3,2)(a)
  statK1 += (-2.0) * osLON * sgamCN * cnSPD;    // (2,3)(3,2)(b)
  statK1 += ( 2.0) * cnLAT * cgamCN * cnSPD;    // (3,4)(4,3)(a)
  statK1 += ( 2.0) * cnLON * sgamCN * cnSPD;    // (3,4)(4,3)(b)
  
  double statK0  =          osLAT * osLAT;  // (2,2)(a)
  statK0 +=          osLON * osLON;  // (2,2)(b)
  statK0 += (-2.0) * osLAT * cnLAT;  // (2,4)(4,2)(a)
  statK0 += (-2.0) * osLON * cnLON;  // (2,4)(4,2)(a)
  statK0 +=          cnLAT * cnLAT;  // (4,4)(a)
  statK0 +=          cnLON * cnLON;  // (4,4)(b)
  
  // Set static variables for quickening the "crossingType" queries
  double statCLOW, statCHGH, statCRNG;
  //double statCNDIS = distPointToPoint(osLON, osLAT, cnLON, cnLAT);
  double statCNANG = relAng(osLON, osLAT, cnLON, cnLAT);
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


  // Phase II ----------------------------------------------------
  
  osCRS = angle360(osCRS);

  double gamOS  = degToRadians(osCRS);    // Angle in radians.
  double cgamOS = cos(gamOS);             // Cosine of Angle (osCRS).
  double sgamOS = sin(gamOS);             // Sine   of Angle (osCRS).
  
  double k2 = statK2;
  double k1 = statK1;
  double k0 = statK0;
  
  if((cnCRS==osCRS) && (cnSPD==osSPD))  // Special case. K1,K2 would
    return(sqrt(k0));                   // be 0, resuling in NaN.
  
  k2 +=          cgamOS * cgamOS * osSPD * osSPD;   // (1,1)(a)
  k2 +=          sgamOS * sgamOS * osSPD * osSPD;   // (1,1)(b)
  k2 += (-2.0) * cgamOS * osSPD * cgamCN * cnSPD;   // (1,3)(3,1)(a)
  k2 += (-2.0) * sgamOS * osSPD * sgamCN * cnSPD;   // (1,3)(3,1)(b)
  
  k1 += ( 2.0) * cgamOS * osSPD * osLAT;  // (1,2)(2,1)(a)
  k1 += ( 2.0) * sgamOS * osSPD * osLON;  // (1,2)(2,1)(b)
  k1 += (-2.0) * cgamOS * osSPD * cnLAT;  // (1,4)(4,1)(a)
  k1 += (-2.0) * sgamOS * osSPD * cnLON;  // (1,4)(4,1)(b)
  
  double cpaDist;
  double minT = ((-1.0) * k1) / (2.0 * k2);

  if(minT <= 0) 
    cpaDist = sqrt(k0); 
  else { 
    if((minT >= osTOL) && (osTOL != -1)) {
      double dist_squared = (k2*osTOL*osTOL) + (k1*osTOL) + k0;
      if(dist_squared < 0)
	cpaDist = 0;
      else
	cpaDist = sqrt(dist_squared);
    }
    else {
      double dist_squared = (k2*minT*minT) + (k1*minT) + k0;
      if(dist_squared < 0)
	cpaDist = 0;
      else
	cpaDist = sqrt(dist_squared);
    }
  }
  return(cpaDist);




  return(0);
}

//---------------------------------------------------------------
// Procedure: crossesBow
//   Purpose: Determine if the ownship object with the given position
//            and trajectory will cross paths with the contact object
//            in front of the contact or not. 
//            If ostol is given (>0) then we apply the further 
//            condition that it cross in front within the time given
//            by ostol (ownship time on leg) in seconds.

bool crossesBow(double cnx, double cny, double cnspd, double cnhdg,
		double osx, double osy, double osspd, double oshdg,
		double ostol)
{
  // First find out where the tracks intersect (or if parallel)

  double cn_ptx;
  double cn_pty;

  projectPoint(cnhdg, 20, cnx, cny, cn_ptx, cn_pty);

  double os_ptx;
  double os_pty;

  projectPoint(oshdg, 20, osx, osy, os_ptx, os_pty);

  double int_ptx;
  double int_pty;

  bool res = linesCross(cnx, cny, cn_ptx, cn_pty, 
			osx, osy, os_ptx, os_pty, 
			int_ptx, int_pty);

  // If tracks are parallel return false
  if(res==false)
    return(false);


  // Second, determine if both objects are moving toward the 
  // intersection point. If not, then return false.

  bool   os_heading_toward_int = true;
  double os_angle_toward_int = relAng(osx, osy, int_ptx, int_pty);
  if(fabs(os_angle_toward_int - oshdg) > 90)
    os_heading_toward_int = false;

  if(!os_heading_toward_int)
    return(false);
  
  
  bool   cn_heading_toward_int = true;
  double cn_angle_toward_int = relAng(cnx, cny, int_ptx, int_pty);
  if(fabs(cn_angle_toward_int - cnhdg) > 90)
    cn_heading_toward_int = false;
  
  if(!cn_heading_toward_int)
    return(false);
  
  // Third, if both objects are indeed heading toward the 
  // intersection point, determine which one gets there first.

  double os_dist_to_int = hypot((int_pty - osy), (int_ptx - osx));
  double os_time_to_int = os_dist_to_int / osspd;

  // if ostol parameter given and os doesnt reach the intersection
  // within this time, then return false immediately. Dont bother
  // calculating the contact distance and time below.
  if((ostol > 0) && (ostol < os_time_to_int))
    return(false);

  double cn_dist_to_int = hypot((int_pty - cny), (int_ptx - cnx));
  double cn_time_to_int = cn_dist_to_int / cnspd;

  // if the os object gets there first, it crosses the bow of the
  // cn object.
  if(os_time_to_int < cn_time_to_int)    
    return(true);
  else
    return(false);
}

//---------------------------------------------------------------
// Procedure: closingSpeed
//   Purpose: Determine the rate at which an object at the given
//            position and trajectory is closing distance to an 
//            object at the given position.
//       osx: X position of the moving object (meters)
//       osy: Y position of the moving object (meters)
//     osspd: Speed of position of the moving object (m/sec)
//     oshdg: Heading of the moving object (m/sec)
//        px: X position of the stationary object (meters)
//        py: Y position of the stationary object (meters)

double closingSpeed(double osx,   double osy, double osspd, 
		    double oshdg, double ptx,  double pty)
{
  // Handle the special case where the two positions are identical
  if((osx==ptx)&&(osy==pty))
    return(0);

  double angle_to_wpt = relAng(osx, osy, ptx, pty);
  
  double angle_diff   = (oshdg - angle_to_wpt);

  // Put the angle in the 0-359.999 range
  while(angle_diff >= 360)
    angle_diff -= 360.0;
  while(angle_diff < 0)
    angle_diff += 360.0;

  // Convert Degrees to Radians
  double rad_diff        = (angle_diff / 180) * 3.1415926;
  double rate_of_closure = cos(rad_diff) * osspd;

  return(rate_of_closure);
}


//---------------------------------------------------------------
// Procedure: closingSpeed
//   Purpose: Determine the rate at which an object at the given
//            position and trajectory is closing distance to  
//            another moving object at the given position and 
//            trajectory.
//       osx: X position of the moving object (meters)
//       osy: Y position of the moving object (meters)
//     osspd: Speed of position of the moving object (m/sec)
//     oshdg: Heading of the moving object (m/sec)
//       cnx: X position of the stationary object (meters)
//       cny: Y position of the stationary object (meters)
//     cnspd: Speed of position of the other moving object (m/sec)
//     cnhdg: Heading of the moving other object (m/sec)

double closingSpeed(double osx,   double osy, double osspd, 
		    double oshdg, double cnx, double cny,
		    double cnspd, double cnhdg)
{
  double mod_cnhdg = angle360(cnhdg-180);

  double hdg_sum, spd_sum;

  velocityVectorSum(oshdg, osspd, mod_cnhdg, cnspd, hdg_sum, spd_sum);
  
  double rate_of_closure = closingSpeed(osx,osy,spd_sum,hdg_sum,cnx,cny);

  return(rate_of_closure);
}


//---------------------------------------------------------------
// Procedure: velocityVectorSum
//   Purpose: Determine the sum of two velocity vectors.

void velocityVectorSum(double hdg1, double spd1, double hdg2, 
		       double spd2, double& hdg, double& spd)
{
  double hdg1_rad = headingToRadians(hdg1);
  double hdg2_rad = headingToRadians(hdg2);
  
  double xspd1 = cos(hdg1_rad) * spd1;
  double yspd1 = sin(hdg1_rad) * spd1;
  double xspd2 = cos(hdg2_rad) * spd2;
  double yspd2 = sin(hdg2_rad) * spd2;

  double xspd = xspd1 + xspd2;
  double yspd = yspd1 + yspd2;

  spd = hypot(xspd, yspd);
  
  if(spd==0)
    hdg = 0;
  else
    hdg = relAng(0,0,xspd,yspd);
}

