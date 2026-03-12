/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CPXEngineThin.cpp                                    */
/*    DATE: Jan 4th 2026                                         */
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
#include "CPXEngineThin.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

#define MPI 3.14159265359

//----------------------------------------------------------
// Procedure: Constructor()

CPXEngineThin::CPXEngineThin(double cny, double cnx,
			     double cnh, double cnv,
			     double osy, double osx,
			     double osh, double osv)
  : CPXEngine(cny, cnx, cnh, cnv, osy, osx, osh, osv)
{
  CPXEngineThin::reset(cny, cnx, cnh, cnv, osy, osx, osh, osv);
}

//----------------------------------------------------------
// Procedure: reset()

void CPXEngineThin::reset(double cny, double cnx,
			  double cnh, double cnv,
			  double osy, double osx,
			  double osh, double osv)
{
  CPXEngine::reset(cny, cnx, cnh, cnv, osy, osx, osh, osv);

  // Init non-cache
  m_stat_k2 = 0;
  m_stat_k1 = 0;
  m_stat_k0 = 0;
  m_stat_range = 0;
  
  m_stat_cosCNH_x_cnSPD = 0;
  m_stat_sinCNH_x_cnSPD = 0;
  
  m_stat_theta_os_eps = 0;
  m_stat_theta_os_gam = 0;
  m_stat_theta_tn     = 0;
  m_stat_tn_constant  = 0;
  
  m_stat_spd_cn_at_tangent = 0;

  // Set static vars including at superclass level
  setStatic();

  // Initialized caches
  //initK1Cache();
  //initK2Cache();
  //initRateCache();

  //initOSCNRelBngCache();
  //initOSCNTangentCache();
  //initOSGammaCache();
  //initOSCNHCosCache();
}

// ===============================================================
// Group 2 Range and Range Rate
// ===============================================================

//----------------------------------------------------------------
// Procedure: evalROC()
//   Purpose: Determine rate-of-closure for a given heading,speed

double CPXEngineThin::evalROC(double osh, double osv) const
{
  //* double os_to_cn_spd = m_os_cn_relbng_cos_cache[(unsigned int)(osh)] * osv; 
  double os_to_cn_spd = os_cn_relbng_cos_osh((unsigned int)(osh)) * osv; 

  double roc = os_to_cn_spd + m_stat_cn_to_os_spd;

  return(roc);
}

// ===============================================================
// Group 3 Bearing Rate
// ===============================================================
//----------------------------------------------------------------
// Procedure: bearingRate() 

double CPXEngineThin::bearingRate(double osh, double osv) const
{
  // Part 1: Calculate speed of ownship in direction of tangent angle

  //* double os_spd_at_tangent = m_os_tn_cos_cache[(unsigned int)(osh)] * -osv;
  double os_spd_at_tangent =os_tn_cos((unsigned int)(osh)) * -osv;

  // Part 2: Speed of contact in direction of tangent angle has been
  // pre-calculated and cached. Sum to two speeds to get the combined
  // speed in the direction of the tangent angle
  
  double spd_at_tangent_angle = os_spd_at_tangent + m_stat_spd_cn_at_tangent;

  // Part 3: Calculate the bearing rate from the speed at tangent
  // This just the cummulative speed * (360 / (2 * range * PI)), where
  // range is the current range between vehicles. This number has also
  // been cached to removed three multiplications: m_stat_tn_constant.

  double bng_rate = spd_at_tangent_angle * m_stat_tn_constant;

  return(bng_rate);
}

// ===============================================================
// Group A - CPA
// ===============================================================
//----------------------------------------------------------------
// Procedure: evalCPA
//   Purpose: Evaluates the given <Course, Speed, Time-on-leg> tuple 
//            Determines Closest-Point-of-Approach (CPA)

double CPXEngineThin::evalCPA(double osh, double osv, double ostol) const
{
  double xdist = -1;
  double xtime = -1;

  evalCPA(osh, osv, ostol, xdist, xtime);
  return(xdist);
}

//----------------------------------------------------------------
// Procedure: evalTimeCPA()
//   Purpose: Evaluates the given <Course, Speed, Time-on-leg> tuple 
//            Determines Time of Closest-Point-of-Approach (CPA)

double CPXEngineThin::evalTimeCPA(double osh, double osv,
				  double ostol) const
{
  double xdist = -1;
  double xtime = -1;

  evalCPA(osh, osv, ostol, xdist, xtime);
  return(xtime);
}


//----------------------------------------------------------------
// Procedure: evalCPA()
//   Purpose: Evaluates the given <Course, Speed, Time-on-leg> tuple 
//            Determines Closest-Point-of-Approach (CPA)

void CPXEngineThin::evalCPA(double osh, double osv, double ostol,
			    double& xdist, double& xtime) const
{
  xdist = m_stat_range;
  xtime = 0;

  if((osh >= 360) || (osh < 0))
    osh = angle360(osh);

  if(m_stat_cn_to_os_closing) {
    if(osv > m_stat_cn_to_os_spd) {
      //*      if(osv >= m_os_vthresh_cache_360[(unsigned int)(osh)])
      if(osv >= os_vthresh_osh((unsigned int)(osh)))
	return;
    }
  }
  else {
    //* if(osv <= m_os_vthresh_cache_360[(unsigned int)(osh)]) 
    if(osv <= os_vthresh_osh((unsigned int)(osh))) 
      return;
  }

  //=========================================================
  // Handle K2
  //=========================================================
  double k2 = m_stat_k2;

  double cos_osh_sin_osh_sq = 1;

  //* double k2_val = m_k2_cache[(unsigned int)(osh)];
  double k2_val = k2_osh((unsigned int)(osh));
  k2_val += (cos_osh_sin_osh_sq) * osv;
  k2 += k2_val * osv;
  if(k2 < 0)
    return;

  //=========================================================
  // Handle K1
  //=========================================================
  double k1 = m_stat_k1;
  //* double k1_val = m_k1_cache[(unsigned int)(osh)] * osv;
  double k1_val = k1_osh((unsigned int)(osh)) * osv;

  k1 += k1_val;
  
  double minT = 0;
  if(k2 != 0)
    minT = k1 / (-2.0 * k2);

  if(minT <= 0) 
    return;

  if(minT >= ostol)
    minT = ostol;

  //=========================================================
  // Handle K0 and final calculatoin
  //=========================================================
  double k0 = m_stat_k0;
  //double dist_squared = (k2 * minT * minT) + (k1 * minT) + k0;
  double dist_squared = minT * ((k2 * minT) + k1) + k0;
  if(dist_squared < 0)
    return;

  xtime = minT;
  xdist = sqrt(dist_squared); 
}



// ===============================================================
// Group B - Checks for Passing Port and/or Starboard
// ===============================================================

//----------------------------------------------------------------
// Procedure: passesPort()
//   Purpose: checks to see if ownship, on the given heading and speed,
//            will pass the contact on the contact's port side. 
//            A "pass" means it will cross the contact's beam, the line
//            perpendicular to the contact's bow-stern line.

bool CPXEngineThin::passesPort(double osh, double osv) const
{
  //============================================================
  // Handle Special Cases
  //============================================================
  // Special Case 1: ownship and contact are on the same point
  if(m_stat_os_on_contact)
    return(false);

  // Special Case 2: ownship does not pass the contact at all
  if(!passesPortOrStar(osh, osv))
    return(false);

  // Special Case 3: ownship is on contact's beam
  if(m_stat_os_aft_of_cn && m_stat_os_fore_of_cn)
    return(m_stat_os_port_of_cn);

  //============================================================
  // Handle General Cases
  //============================================================
  // Case #1: ownship is aft of contact                       //
  //                           |                              //       
  //                           |                              //       
  //                           |                              //       
  //                          / \                             //       
  //                          | |                             //
  //          ----------------|C|-----------------            //
  //          Case:           | |                             //
  //                          ---                             //
  //          Aft of contact   |                              //
  //                           |                              //
  //                           |                              //
  //            (Ownship)      |                              //
  //
  if(m_stat_os_aft_of_cn)
    return(bearingRate(osh, osv) > 0);

  // Case #4: ownship is fore and starboard of contact        //
  //                           |                              //
  //                 (ownship) |     Case:                    //
  //                           |     Fore of contact          //
  //                           |                              //
  //                          / \                             //
  //                          | |                             //
  //          ----------------|C|-----------------            //
  //                          | |                             //
  //                          ---                             //
  //                           |                              //
  //                           |                              //
  //                           |                              //
  //                           |                              //
  //
  
  return(bearingRate(osh, osv) < 0);
}

//----------------------------------------------------------------
// Procedure: passesPortDist()              (Convenience function)
 
double CPXEngineThin::passesPortDist(double osh, double osv) const
{
  double xdist = -1;
  double xtime = -1;
  passesPortDist(osh, osv, xdist, xtime);
  return(xdist);
}

//----------------------------------------------------------------
// Procedure: passesPortTime()              (Convenience function)
 
double CPXEngineThin::passesPortTime(double osh, double osv) const
{
  double xdist = -1;
  double xtime = -1;
  passesPortDist(osh, osv, xdist, xtime);
  return(xtime);
}

//----------------------------------------------------------------
// Procedure: passesPortDist()

bool CPXEngineThin::passesPortDist(double osh, double osv,
				   double& xdist, double& xtime) const
{
  // Default return values reflect ownship does not pass contact port
  xdist = -1;
  xtime = -1;
  
  //------------------------------------------------------
  // Sanity checks
  //------------------------------------------------------
  // Sanity check 1: If on the contact, return false
  if(m_stat_os_on_contact) 
    return(false);
  // Sanity check 2: If on the starboard beam, return false
  if(m_stat_os_on_beam && m_stat_os_star_of_cn) 
    return(false);

  // Sanity check 3: If on the port beam, return the current range
  if(m_stat_os_on_beam && m_stat_os_port_of_cn) {
    xdist = m_stat_range;
    return(true);
  }

  // Sanity check 4: Make sure osh is in [0,360) since we're using the
  // whole number to index an array of size 360.
  if((osh < 0) || (osh >= 360))
    osh = angle360(osh);

  //------------------------------------------------------
  // Find out first IF ownship will pass contact (regardless port/star)
  //------------------------------------------------------
  // Step 1: Get ownship speed in contact beam direction, epsilon speed
  //* double v_os_cnh = m_os_cnh_cos_cache[(unsigned int)(osh)] * osv;
  double v_os_cnh = os_cnh_cos((unsigned int)(osh)) * osv;

  double v_os_eps = m_cnv - v_os_cnh;
  if(m_stat_os_aft_of_cn)
    v_os_eps = v_os_cnh - m_cnv;
  
  // Step 2: Determine IF ownship it will pass contact
  if(v_os_eps <= 0)
    return(false);
  
  //------------------------------------------------------
  // Determine how far os will travel in gamma direction (perp to the 
  // cn bow-stern line) in window of time up to moment of passing.
  //------------------------------------------------------
  // Step 3A: Get time it takes for ownship to reach the contact's beam.
  double time_os_eps = m_stat_range_eps / v_os_eps;
  // Step 3B: Get os spd in gamma direction, perp to cn bow-stern line. 
  //* double os_gam_cos = m_os_gam_cos_cache[(unsigned int)(osh)];
  double v_os_gam_cos = os_gam_cos((unsigned int)(osh));
  double spd_os_gam = v_os_gam_cos * osv;
  // Step 3C: Get dist os will travel toward cn. (may be negative)
  double gam_dist = time_os_eps * spd_os_gam;

  // Step 3D: Determine if os passes on port, and if so, the dist.
  // Case 1 of 2 (os presently port of cn) does not cross port to star
  if(m_stat_os_port_of_cn && (gam_dist <= m_stat_range_gam))
    xdist = (m_stat_range_gam - gam_dist);
  
  // Case 2 of 2 (os presently starboard of cn) crosses star to port
  if(m_stat_os_star_of_cn && (gam_dist > m_stat_range_gam))
    xdist = (gam_dist - m_stat_range_gam);
  
  if(xdist < 0) {
    xdist = -1;
    return(false);
  }

  xtime = time_os_eps;
  return(true);
}

//----------------------------------------------------------------
// Procedure: passesStar()
//   Purpose: checks to see if ownship, on the given heading and speed,
//            will pass the contact on the contact's starboard side. 
//            A "pass" means it will cross the line perpendicular to the 
//            bow-stern line.

bool CPXEngineThin::passesStar(double osh, double osv) const
{
  //============================================================
  // Handle Special Cases
  //============================================================
  // Special Case 1: ownship and contact are on the same point
  if(m_stat_os_on_contact)
    return(false);
  
  // Special Case 2: ownship does not pass the contact at all
  if(!passesPortOrStar(osh, osv))
    return(false);
  
  if(!passesPort(osh, osv))
    return(true);
  
  return(false);
}


//----------------------------------------------------------------
// Procedure: passesStarDist()              (Convenience function)
 
double CPXEngineThin::passesStarDist(double osh, double osv) const
{
  double xdist = -1;
  double xtime = -1;
  passesStarDist(osh, osv, xdist, xtime);
  return(xdist);
}

//----------------------------------------------------------------
// Procedure: passesStarTime()              (Convenience function)
 
double CPXEngineThin::passesStarTime(double osh, double osv) const
{
  double xdist = -1;
  double xtime = -1;
  passesStarDist(osh, osv, xdist, xtime);
  return(xtime);
}

//----------------------------------------------------------------
// Procedure: passesStarDist()

bool CPXEngineThin::passesStarDist(double osh, double osv,
				   double& xdist, double& xtime) const
{
  // Default return vals reflect ownship does not pass cn port starboard
  xdist = -1;
  xtime = -1;
  
  //------------------------------------------------------
  // Sanity checks
  //------------------------------------------------------
  // Sanity check 1: If on the contact, return false
  if(m_stat_os_on_contact) 
    return(false);
  // Sanity check 2: If on the port beam, return false
  if(m_stat_os_on_beam && m_stat_os_port_of_cn) 
    return(false);

  // Sanity check 3: If on the starboard beam, return the current range
  if(m_stat_os_on_beam && m_stat_os_star_of_cn) {
    xdist = m_stat_range;
    return(true);
  }
  
  // Sanity check 4: Make sure osh is in [0,360) since we're using the
  // whole number to index an array of size 360.
  if((osh < 0) || (osh >= 360))
    osh = angle360(osh);

  //------------------------------------------------------
  // Find out first IF ownship will pass contact (regardless port/star)
  //------------------------------------------------------
  // Step 1: Get ownship speed in contact beam direction, epsilon speed
  //* double v_os_cnh = m_os_cnh_cos_cache[(unsigned int)(osh)] * osv;
  double v_os_cnh = os_cnh_cos((unsigned int)(osh)) * osv;

  double v_os_eps = m_cnv - v_os_cnh;
  if(m_stat_os_aft_of_cn)
    v_os_eps = v_os_cnh - m_cnv;

  // Step 2: Determine IF ownship it will pass contact
  if(v_os_eps <= 0)
    return(false);
  
  //------------------------------------------------------
  // Determine how far os will travel in gamma direction (perp to the 
  // cn bow-stern line) in window of time up to moment of passing.
  //------------------------------------------------------
  // Step 3A: Get time it takes for ownship to reach the contact's beam.
  double time_os_eps = m_stat_range_eps / v_os_eps;
  // Step 3B: Get os spd in gamma direction, perp to cn bow-stern line. 
  //* double os_gam_cos = m_os_gam_cos_cache[(unsigned int)(osh)];
  double v_os_gam_cos = os_gam_cos((unsigned int)(osh));
  double spd_os_gam = v_os_gam_cos * osv;
  // Step 3C: Get dist os will travel toward cn. (may be negative)
  double gam_dist = time_os_eps * spd_os_gam;

  // Step 3D: Determine if os passes on port, and if so, the dist.
  // Case 1 of 2 (os presently star of cn) does not cross star to port
  if(m_stat_os_star_of_cn && (gam_dist <= m_stat_range_gam))
    xdist = (m_stat_range_gam - gam_dist);

  // Case 2 of 2 (os presently port of cn) crosses port to star
  if(m_stat_os_port_of_cn && (gam_dist > m_stat_range_gam))
    xdist = (gam_dist - m_stat_range_gam);

  if(xdist < 0) {
    xdist = -1;
    return(false);
  }

  xtime = time_os_eps;
  return(true);
}

//----------------------------------------------------------------
// Procedure: passesPortOrStar()
//   Purpose: checks to see if ownship, on the given heading and speed,
//            will pass the contact. A "pass" means it will cross the 
//            line perpendicular to the bow-stern line. (crosses the beam)

bool CPXEngineThin::passesPortOrStar(double osh, double osv) const
{
  if(m_stat_os_on_beam)
    return(true);
  
  //* double v_os_cnh = m_os_cnh_cos_cache[(unsigned int)(osh)] * osv;
  double v_os_cnh = os_cnh_cos((unsigned int)(osh)) * osv;

  double v_os_eps = m_cnv - v_os_cnh;
  if(m_stat_os_aft_of_cn)
    v_os_eps = v_os_cnh - m_cnv;
  
  return(v_os_eps > 0);
}


// ===============================================================
// Group C -  Checks for Crossing Stern and/or Bow 
// ===============================================================

//----------------------------------------------------------------
// Procedure: crossesStern()
//   Purpose: Determine if for the given ownship course and speed
//            whether it is on a path to cross the path of the 
//            contact on its stern.

bool CPXEngineThin::crossesStern(double osh, double osv) const
{
  if(m_stat_os_on_sternline)
    return(true);

  if(osv <= 0)
    return(false);

  // If the speed in the direction of the contact's bow-stern line is
  // not positive, it won't cross. We don't need to know the actual
  // speed just the sign of cos(os_gam).
  //* double gam = m_os_gam_cos_cache[(unsigned int)(osh)];
  double gam = os_gam_cos((unsigned int)(osh));
  if(gam <= 0)
    return(false);

  if(m_stat_os_port_of_cn) 
    return(bearingRate(osh, osv) <= 0);
  else
    return(bearingRate(osh, osv) >= 0);
}

//----------------------------------------------------------------
// Procedure: crossesSternDist()              Convenience function
//
//   Purpose: Determine if for the given ownship course and speed
//            whether it is on a path to cross the path of the 
//            contact on its stern. And if so, at what distance when
//            it crosses?

double CPXEngineThin::crossesSternDist(double osh, double osv) const
{
  double xdist, xtime;
  crossesSternDist(osh, osv, xdist, xtime);
  return(xdist);
}

//----------------------------------------------------------------
// Procedure: crossesSternTime()              Convenience function
//
//   Purpose: Determine if for the given ownship course and speed
//            whether it is on a path to cross the path of the 
//            contact on its stern. And if so, how long from now.

double CPXEngineThin::crossesSternTime(double osh, double osv) const
{
  double xdist, xtime;
  crossesSternDist(osh, osv, xdist, xtime);
  return(xtime);
}

//----------------------------------------------------------------
// Procedure: crossesSternDist()
//   Purpose: Determine if for the given ownship course and speed
//            whether it is on a path to cross contact's bow-stern 
//            line on the stern side. And if so, determine the 
//            distance to the contact when it crosses, and the
//            elapsed time from now when it crosses.

bool CPXEngineThin::crossesSternDist(double osh, double osv,
				     double& xdist, double& xtime) const
{
  // Default return values reflect os does not cross cn stern.
  xdist = -1;
  xtime = -1;

  // Sanity check 1: If on the sternline, return the current range
  if(m_stat_os_on_sternline) {
    xdist = m_stat_range;
    return(true);
  }
  // Sanity check 2: If on the bowline or contact, return false
  if(m_stat_os_on_bowline || m_stat_os_on_contact) 
    return(false);

  // Sanity check 3: Make sure osh is in [0,360) since we're using the
  // whole number to index an array of size 360.
  if((osh < 0) || (osh >= 360))
    osh = angle360(osh);

  // Step 1: Get ownship speed in gamma direction, heading perpendicular 
  // to contact's bow-stern line. If not positive, ownship will not be
  // crossing the contact's bow-stern line, so return now.
  //* double os_gam_cos = m_os_gam_cos_cache[(unsigned int)(osh)];
  double v_os_gam_cos = os_gam_cos((unsigned int)(osh));
  double speed_os_gam = v_os_gam_cos * osv;
  if(speed_os_gam <= 0) 
    return(false);


  // Step 2: Get time it takes to the contact's bow-stern line.
  double time_os_gam = m_stat_range_gam / speed_os_gam;
  
  // Step 3: Get the distance that contact will travel in this time.
  double range_xcn_eps = time_os_gam * m_cnv;

  // Step 4: Get ownship speed in contact direction, heading equal to
  // the contact's current heading.
  //* double v_os_cnh_cos = m_os_cnh_cos_cache[(unsigned int)(osh)] * osv;
  double v_os_cnh_cos = os_cnh_cos((unsigned int)(osh)) * osv;

  //* double speed_os_cnh = os_cnh_cos * osv;

  // Step 5: Get the distance, in the contact heading direction, that
  // ownship will travel during this time.
  double range_xos_eps = time_os_gam * v_os_cnh_cos;

  // Step 6: Get the crossing stern distance
  xdist = range_xcn_eps;
  if(m_stat_os_fore_of_cn)
    xdist -= m_stat_range_eps;
  else
    xdist += m_stat_range_eps;

  xdist -= range_xos_eps;
  
  if(xdist < 0) {
    xdist = -1;
    return(false);
  }

  xtime = time_os_gam;
  return(true);
}

//----------------------------------------------------------------
// Procedure: crossesBow()

bool CPXEngineThin::crossesBow(double osh, double osv) const
{
  if(m_stat_os_on_sternline)
    return(true);

  if(osv <= 0)
    return(false);

  // If the speed in the direction of the contact's bow-stern line is
  // not positive, it won't cross. We don't need to know the actual
  // speed just the sign of cos(os_gam).
  //double gam = m_os_gam_cos_cache[(unsigned int)(osh)];
  double gam = os_gam_cos((unsigned int)(osh));
  if(gam <= 0)
    return(false);

  double bng_rate = bearingRate(osh, osv);

  if(m_stat_os_port_of_cn) 
    return(bng_rate >= 0);
  else
    return(bng_rate <= 0);
}

//----------------------------------------------------------------
// Procedure: crossesBowDist()  (Convenience function)
 
double CPXEngineThin::crossesBowDist(double osh, double osv) const
{
  double xdist, xtime;
  crossesBowDist(osh, osv, xdist, xtime);
  return(xdist);
}

//----------------------------------------------------------------
// Procedure: crossesBowTime()  (Convenience function)
 
double CPXEngineThin::crossesBowTime(double osh, double osv) const
{
  double xdist, xtime;
  crossesBowDist(osh, osv, xdist, xtime);
  return(xtime);
}

//----------------------------------------------------------------
// Procedure: crossesBowDist()
//   Purpose: Determine if for the given ownship course and speed
//            whether it is on a path to cross contact's bow-stern
//            line on the bow side. And if so, determine the distance
//            to the contact when it crosses, and the elapsed time
//            from now when it crosses.

bool CPXEngineThin::crossesBowDist(double osh, double osv,
				   double& xdist, double& xtime) const
{
  // Default return values reflect os does not cross contact bow.
  xdist = -1;
  xtime = -1;

  // Sanity check 1: If on the bowline, return the current range
  if(m_stat_os_on_bowline) {
    xdist = m_stat_range;
    return(true);
  }
  // Sanity check 2: If on the sternline or contact, return false
  if(m_stat_os_on_sternline || m_stat_os_on_contact) 
    return(false);

  // Sanity check 3: Make sure osh is in [0,360) since we're using the
  // whole number to index an array of size 360.
  if((osh < 0) || (osh >= 360))
    osh = angle360(osh);

  // Step 1: Get ownship speed in gamma direction, heading perpendicular 
  // to contact's bow-stern line. If not positive, ownship will not be
  // crossing the contact's bow-stern line, so return now.
  //* double os_gam_cos = m_os_gam_cos_cache[(unsigned int)(osh)];
  double v_os_gam_cos = os_gam_cos((unsigned int)(osh));
  double speed_os_gam = v_os_gam_cos * osv;
  if(speed_os_gam <= 0) 
    return(false);


  // Step 2: Get time it takes to the contact's bow-stern line.
  double time_os_gam = m_stat_range_gam / speed_os_gam;
  
  // Step 3: Get the distance that contact will travel in this time.
  double range_xcn_eps = time_os_gam * m_cnv;

  // Step 4: Get ownship speed in contact direction, heading equal to
  // the contact's current heading.
  //* double v_os_cnh_cos = m_os_cnh_cos_cache[(unsigned int)(osh)] * osv;
  double v_os_cnh_cos = os_cnh_cos((unsigned int)(osh)) * osv;

  // Step 5: Get the distance, in the contact heading direction, that
  // ownship will travel during this time.
  double range_xos_eps = time_os_gam * v_os_cnh_cos;

  // Step 6: Get the crossing bow distance
  xdist = range_xos_eps - range_xcn_eps;
  if(m_stat_os_fore_of_cn)
    xdist += m_stat_range_eps;
  else
    xdist -= m_stat_range_eps;

  if(xdist < 0) {
    xdist = -1;
    return(false);
  }

  xtime = time_os_gam;
  return(true);
}


//----------------------------------------------------------------
// Procedure: crossesBowOrStern()
//   Purpose: Determine if for the given ownship course and speed
//            whether it is on a path to cross the path of the 
//            contact on its stern or bow. 

bool CPXEngineThin::crossesBowOrStern(double osh, double osv) const
{
  if(m_stat_os_on_bowsternline)
    return(true);

  if(osv <= 0)
    return(false);

  //* double gam = m_os_gam_cos_cache[(unsigned int)(osh)];
  double gam = os_gam_cos((unsigned int)(osh));
  return(gam > 0.00001);
}


// ===============================================================
// Group W -  Specific only to this CPAEngine holonomic
// ===============================================================

//----------------------------------------------------------------
// Procedure: osSpdInCNHdgX(double, double)
//   Purpose: Similar to the superclass version. Here the function
//            takes any given osh and osv, rather than the current
//            osh and osv as in the superclass. Also this function
//            uses the cache of cosine values indexed on osh.

double CPXEngineThin::osSpdInCNHdgX(double osh, double osv) const
{
  if((osh < 0) || (osh >= 360))
    osh = angle360(osh);
  //* double v_os_cnh = m_os_cnh_cos_cache[(unsigned int)(osh)] * osv;
  double v_os_cnh = os_cnh_cos((unsigned int)(osh)) * osv;

  return(v_os_cnh);
}

//----------------------------------------------------------------
// Procedure: osToCnRelBngX(double)

double CPXEngineThin::osToCNRelBngX(double osh) const
{
  if((osh < 0) || (osh >= 360))
    osh = angle360(osh);
  //* return(m_os_cn_relbng_cache[(unsigned int)(osh)]);
  return(os_cn_relbng_osh((unsigned int)(osh)));
}


//----------------------------------------------------------------
// Procedure: minMaxROC()
//   Purpose: Determine max Rate-Of-Closure for a given number of  
//            headings and max_speed

double CPXEngineThin::minMaxROC(double speed, double heading_clicks, 
			    double &min_roc, double& max_roc) const
{
  double heading_delta = 360.0 / heading_clicks;
  
  double max_heading = 0;
  
  double try_roc = 0;
  double try_heading = 0;
  while(try_heading < 360) {
    try_roc = evalROC(try_heading, speed);
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

// ===============================================================
// Group X - Intermediate Values
// ===============================================================

//----------------------------------------------------------------
// Procedure: getOSSpeedGamma()
//      Note: The ownship speed in direction perpendicular to contact

double CPXEngineThin::getOSSpeedGamma(double osh, double osv) const
{
  if((osh < 0) || (osh >= 360))
    osh = angle360(osh);
  //* double v_os_gam = m_os_gam_cos_cache[(unsigned int)(osh)] * osv;
  double v_os_gam = os_gam_cos((unsigned int)(osh)) * osv;
  return(v_os_gam);
}


//----------------------------------------------------------------
// Procedure: getOSSpeedEpsilon
//      Note: The ownship speed in direction of the contact beam

double CPXEngineThin::getOSSpeedEpsilon(double osh, double osv) const
{
  if((osh < 0) || (osh >= 360))
    osh = angle360(osh);
  //double v_os_cnh = m_os_cnh_cos_cache[(unsigned int)(osh)] * osv;
  double v_os_cnh = os_cnh_cos((unsigned int)(osh)) * osv;
  
  if(m_stat_os_fore_of_cn) 
    return(m_cnv - v_os_cnh);
  else
    return(v_os_cnh - m_cnv);
}


//----------------------------------------------------------------
// Procedure: getOSTimeGamma()

double CPXEngineThin::getOSTimeGamma(double osh, double osv) const
{
  // Sanity check 1: If on the contact, return 0
  if(m_stat_os_on_contact || m_stat_os_on_bowsternline)
    return(0);

  // Sanity check 2: Make sure osh is in [0,360) since we're using the
  // whole number to index an array of size 360.
  if((osh < 0) || (osh >= 360))
    osh = angle360(osh);

  // Get ownship speed in the direction of contact heading
  //* double v_os_gam = m_os_gam_cos_cache[(unsigned int)(osh)] * osv;
  double v_os_gam = os_gam_cos((unsigned int)(osh)) * osv;
  
  // Avoid division by zero
  if(v_os_gam == 0)
    return(0);
  
  double time_os_gam = m_stat_range_gam / v_os_gam;
  return(time_os_gam);
}


//----------------------------------------------------------------
// Procedure: getOSTimeEpsilon()

double CPXEngineThin::getOSTimeEpsilon(double osh, double osv) const
{
  // Sanity check 1: If on the contact, return 0
  if(m_stat_os_on_contact || m_stat_os_on_beam)
    return(0);

  double v_os_eps = getOSSpeedEpsilon(osh, osv);
  if(v_os_eps == 0)
    return(0);
  return(m_stat_range_eps / v_os_eps);
}

// ===============================================================
// Group Z - Init Static and Cache values
// ===============================================================

//----------------------------------------------------------------
// Procedure: setStatic()
//   Purpose: Determine all terms not dependent on osh, osv or
//            tol. These can be calculated once to save time.
//    Recall: The distance between OS and CN is calculated using
//            pythagorean theorem and Lat/Lon distances. C^2 is the
//            distance between ships and a^2 is LAT distance and 
//            b^2 is LON distance:
//                    a^2 = (new_osy - new_cny)^2
//                    b^2 = (new_osx - new_cnx)^2
//
//            new_m_osy ... newm_cnx indicates the new positions
//            after osTOL units of time, and the given starting
//            positions and trajectories:
//                    new_osy = cos(osh) * osv * tol + osy
//                    new_cny = cos(cnh) * cnv * tol + cny
//                    new_osx = sin(osh) * osv * tol + osx
//                    new_cnx = sin(cnh) * cnv * tol + cnx
//            Multiplying the terms out, we get some terms with
//            tol^2, tol, and constants. We lump the 
//            coeffecients as follows:
//                    K2: coefficients of tol^2.
//                    K1: coefficients of tol.
//                    K0: constants.
//            More than half of these are not dependent on osv
//            or osh, so we calculate them once, here.

void CPXEngineThin::setStatic()
{
  setStaticRoot();
  
  m_stat_theta_tn = angle360(m_stat_abs_bng_os_cn + 90);

  // Calculate the speed of the contact at the tangent angle, for future
  // calculations of bearing rate.
  double cn_delta_heading = angle180(m_cnh - m_stat_theta_tn);
  if(cn_delta_heading < 0)
    cn_delta_heading = -cn_delta_heading;

  // Special case, cos(90) should be exactly zero. Converting 90 to PI/2
  // introduces rounding error with cos(90). Special case handle here.
  m_stat_spd_cn_at_tangent = 0;
  if(cn_delta_heading != 90) {
    double radians = degToRadians(cn_delta_heading);
    m_stat_spd_cn_at_tangent = cos(radians) * m_cnv;
  }

  double cnh_radians = degToRadians(m_cnh); // Angle in radians.
  double m_cos_cnh = cos(cnh_radians);      // Cosine of Angle (m_cnh).
  double m_sin_cnh = sin(cnh_radians);      // Sine   of Angle (m_cnh).

  //========================================
  m_stat_cosCNH_x_cnSPD = -2 * m_cos_cnh * m_cnv;
  m_stat_sinCNH_x_cnSPD = -2 * m_sin_cnh * m_cnv;
  //========================================

  m_stat_k2 = m_cnv * m_cnv;    
  
  m_stat_k1  = (-2.0) * m_osy * m_cos_cnh * m_cnv;    
  m_stat_k1 += (-2.0) * m_osx * m_sin_cnh * m_cnv;    
  m_stat_k1 += ( 2.0) * m_cny * m_cos_cnh * m_cnv;    
  m_stat_k1 += ( 2.0) * m_cnx * m_sin_cnh * m_cnv;    
  
  m_stat_k0  =          m_osy * m_osy;  
  m_stat_k0 +=          m_osx * m_osx;  
  m_stat_k0 += (-2.0) * m_osy * m_cny;  
  m_stat_k0 += (-2.0) * m_osx * m_cnx;  
  m_stat_k0 +=          m_cny * m_cny;  
  m_stat_k0 +=          m_cnx * m_cnx;  // range is sqrt of stat_k0
  
  m_stat_tn_constant = (360 / (2 * m_stat_range * MPI));

  if(m_stat_os_port_of_cn)
    m_stat_theta_os_gam = angle360(m_cnh+90);
  else
    m_stat_theta_os_gam = angle360(m_cnh-90);

  if(m_stat_os_fore_of_cn)
    m_stat_theta_os_eps = angle360(m_cnh + 180);
  else
    m_stat_theta_os_eps = m_cnh;

#if 0
  //---------------------------------------------------------------------
  // Part 2: Initialize the gamma range
  //---------------------------------------------------------------------
  // The gamma ranges (r_gamma and r_gamma_prime) are used for quickly 
  // calculating the range that ownship crosses the contact bow or stern.
  // Note: 
  //    r_gamma   is m_stat_range_gam, and
  //    r_epsilon is m_stat_range_eps
  
  double rel_bng_cn_to_os_180 = m_stat_rel_bng_cn_os;
  if(rel_bng_cn_to_os_180 > 180)
    rel_bng_cn_to_os_180 -= 360;
  else if(rel_bng_cn_to_os_180 < -180)
    rel_bng_cn_to_os_180 += 360;
  if(rel_bng_cn_to_os_180 < 0)
    rel_bng_cn_to_os_180 = -rel_bng_cn_to_os_180;
  // Now rel_bng_cn_to_os_180 is in the range [0 180]
  
  // Special case: os is directly on contact's bow-stern line
  if((rel_bng_cn_to_os_180 == 0) || (rel_bng_cn_to_os_180 == 180)) {
    m_stat_range_gam = 0;
    m_stat_range_eps = m_stat_range;
    return;
  }
    
  double theta_b = 90 - rel_bng_cn_to_os_180;
  double theta_b_rad = degToRadians(theta_b);
  double cos_theta_b = cos(theta_b_rad); // Note theta_b cannot be 90,-90.

  m_stat_range_gam = m_stat_range * cos_theta_b;

  //---------------------------------------------------------------------
  // Part 3: Initialize the epsilon range
  //---------------------------------------------------------------------
  // The epsilon range, r_epsilon, is used for quickly calculating the
  // range that ownship crosses the contact beam

  //theta_e = 
  
  double range_2d     = m_stat_range * m_stat_range;
  double range_gam_2d = m_stat_range_gam * m_stat_range_gam;
  m_stat_range_eps = sqrt(range_2d - range_gam_2d);
  //if(theta_b > 0)
  //  m_stat_range_eps = -m_stat_range_eps;
 #endif
  
}

//----------------------------------------------------------------
// Procedure: initRateCache()

void CPXEngineThin::initRateCache()
{
  // Note: m_stat_cn_to_os_closing set in setStaticRoot (in superclass)

  vector<double> virgin_cache(360,999);
  m_os_vthresh_cache_360 = virgin_cache;  
  // Case 1: contact is closing ownship position, i.e., contact speed
  //         vector is in the direction of ownship rather than away.
  //         The cache will be a cache of speeds ownship must be
  //         greather than or equal to, if the current point in time
  //         is to be considered the CPA.

  if(m_stat_cn_to_os_closing) {
    
    double relang_os_to_cn = relAng(m_cnx, m_cny, m_osx, m_osy);
    for(unsigned int i=0; i<360; i++) {
      double delta = (double)(i) - relang_os_to_cn;
      
      double cos_delta = cos(degToRadians(delta));
      if(cos_delta > 0.0001) {
	double thresh = (m_stat_cn_to_os_spd / cos_delta);
	m_os_vthresh_cache_360[i] = thresh;
      }
    }
    return;
  }   
  // Case 2: contact is opening ownship position, i.e., contact speed
  //         vector is in direction away from ownship rather than toward.
  //         The cache will be a cache of speeds ownship must be less
  //         than or equal to, if the current point in time is to be 
  //         considered the CPA.
  
  double relang_os_to_cn = relAng(m_osx, m_osy, m_cnx, m_cny);
  for(unsigned int i=0; i<360; i++) {
    double delta = (double)(i) - relang_os_to_cn;    
    double cos_delta = cos(degToRadians(delta));
    //double cos_delta = m_cos_cache_1800[(unsigned int)(delta)];
    if(cos_delta > 0.0001) {
      double thresh = (-m_stat_cn_to_os_spd / cos_delta);
      m_os_vthresh_cache_360[i] = thresh;
    }
  }
}


//----------------------------------------------------------------
// Procedure: initK1Cache()

void CPXEngineThin::initK1Cache()
{
  // Part 2: Create the course resolution cache for use with ownship
  vector<double> virgin_cache(360,0);
  m_k1_cache = virgin_cache;

  for(unsigned int i=0; i<360; i++) {
    double cos_osh = m_cos_cache[(unsigned int)(i)];
    double sin_osh = m_sin_cache[(unsigned int)(i)];

    double k1_val = ( 2.0) * cos_osh * m_osy;  // (1,2)(2,1)(a)
    k1_val += ( 2.0) * sin_osh * m_osx;  // (1,2)(2,1)(b)
    k1_val += (-2.0) * cos_osh * m_cny;  // (1,4)(4,1)(a)
    k1_val += (-2.0) * sin_osh * m_cnx;  // (1,4)(4,1)(b)
    
    m_k1_cache[i] = k1_val;
  }
}

//----------------------------------------------------------------
// Procedure: initK2Cache()

void CPXEngineThin::initK2Cache()
{
  // Part 2: Create the course resolution cache for use with ownship
  vector<double> virgin_cache(360,0);
  m_k2_cache = virgin_cache;

  for(unsigned int i=0; i<360; i++) {
    double cos_osh = m_cos_cache[(unsigned int)(i)];
    double sin_osh = m_sin_cache[(unsigned int)(i)];
    
    double k2_val = cos_osh * m_stat_cosCNH_x_cnSPD;  
    k2_val += sin_osh * m_stat_sinCNH_x_cnSPD;  

    m_k2_cache[i] = k2_val;
  }
}

//----------------------------------------------------------------
// Procedure: initOSCNRelBngCache()

void CPXEngineThin::initOSCNRelBngCache()
{
  vector<double> virgin_cache(360,0);
  m_os_cn_relbng_cache = virgin_cache;
  m_os_cn_relbng_cos_cache = virgin_cache;

  double angle_os_to_cn = relAng(m_osx, m_osy, m_cnx, m_cny);

  for(unsigned int i=0; i<360; i++) {
    double rel_bng = angle_os_to_cn - ((double)(i));
    if(rel_bng < 0)
      rel_bng += 360;
    else if(rel_bng >= 360)
      rel_bng -= 360;
    m_os_cn_relbng_cache[i] = rel_bng;

    // Special case, cos(90/270) should be exactly zero. Converting 90 to 
    // PI/2 introduces rounding error with cos(90). Special case handle here.
    m_os_cn_relbng_cos_cache[i] = 0;
    if((rel_bng != 90) && (rel_bng != 270)) {
      double radians = degToRadians(rel_bng); 
      m_os_cn_relbng_cos_cache[i] = cos(radians);
    }
  }

  //m_os_cn_relbng_cos_cache[0]   = 1;
  //m_os_cn_relbng_cos_cache[90]  = 0;
  //m_os_cn_relbng_cos_cache[180] = 0;
}

//----------------------------------------------------------------
// Procedure: initOSCNTangentCache()

void CPXEngineThin::initOSCNTangentCache()
{
  // Part 2: Create the ownship cosine cache in direction of the
  //         tangent angle.
  vector<double> virgin_cache(360,0);
  m_os_tn_cos_cache = virgin_cache;

  for(unsigned int i=0; i<360; i++) {
    double delta = ((double)(i)) - m_stat_theta_tn;
    if(delta > 180)
      delta -= 360;
    else if(delta < -180)
      delta += 360;
    if(delta < 0)
      delta = -delta;

    // Special case, cos(90) should be exactly zero. Converting 90 to PI/2
    // introduces rounding error with cos(90). Special case handle here.
    m_os_tn_cos_cache[i] = 0;
    if(delta != 90) { 
      double radians = degToRadians(delta); 
      m_os_tn_cos_cache[i] = cos(radians); 
    }
  }
}

//----------------------------------------------------------------
// Procedure: initOSGammaCache()

void CPXEngineThin::initOSGammaCache()
{
  //---------------------------------------------------------------------
  // Part 1: Create the theta_os_gam cosine cache.
  //---------------------------------------------------------------------
  // Create ownship gamma cosine cache in direction of line perpendicular
  // to contact bow-stern line, ie, perpendicular to the contact heading.  
  // This value is useful, for example, when calculating the speed at which
  // ownship is nearing the contact bow-stern line.
  vector<double> virgin_cache(360,0);
  m_os_gam_cos_cache = virgin_cache;

  for(unsigned int i=0; i<360; i++) {
    double delta = ((double)(i)) - m_stat_theta_os_gam;
    if(delta > 180)
      delta -= 360;
    else if(delta < -180)
      delta += 360;
    if(delta < 0)
      delta = -delta;

    // Set the cosine value. Handle, as a special case, when delta is
    // 90 degrees. We know that cos(90) is zero, but when converting
    // 90 degrees to PI/2, there is imprecision. This imprecision results
    // in cos(90) having a non-zero value out to the 10th decimal or so.
    m_os_gam_cos_cache[i] = 0; 
    if(delta != 90) {
      double gam = degToRadians(delta); 
      m_os_gam_cos_cache[i] = cos(gam); 
    }
  }
}

//----------------------------------------------------------------
// Procedure: initOSCNHCosCache()

void CPXEngineThin::initOSCNHCosCache()
{
  // Part 2: Create a cosine cache for all possible ownship headings
  //         in the direction of the contact heading. 
  
  vector<double> virgin_cache(360,0);
  m_os_cnh_cos_cache = virgin_cache;

  for(unsigned int i=0; i<360; i++) {
    double delta = ((double)(i)) - m_cnh;
    if(delta > 180)
      delta -= 360;
    else if(delta < -180)
      delta += 360;
    if(delta < 0)
      delta = -delta;

    // Set the cosine value. Handle, as a special case, when delta is
    // 90 degrees. We know that cos(90) is zero, but when converting
    // 90 degrees to PI/2, there is imprecision. This imprecision results
    // in cos(90) having a non-zero value out to the 10th decimal or so.
    m_os_cnh_cos_cache[i] = 0; 
    if(delta != 90) {
      double gam = degToRadians(delta); 
      m_os_cnh_cos_cache[i] = cos(gam); 
    }
  }
}

//================================================================

//----------------------------------------------------------------
// Procedure: os_cnh_cos()
//   Purpose: see initOSCNHCosCache()

double CPXEngineThin::os_cnh_cos(double osh) const
{
  double delta = osh - m_cnh;
  if(delta > 180)
    delta -= 360;
  else if(delta < -180)
    delta += 360;
  if(delta < 0)
    delta = -delta;

  if(delta == 90)
    return(0);

  double gam = degToRadians(delta); 
  double os_cnh_cos = cos(gam); 

  return(os_cnh_cos);
}

//----------------------------------------------------------------
// Procedure: k1_osh()
//   Purpose: See initK1Cache()

double CPXEngineThin::k1_osh(double osh) const
{
  double rad = degToRadians(osh);
  double cos_osh = cos(rad);
  double sin_osh = sin(rad);

  double k1_val = (2.0) * cos_osh * m_osy;  // (1,2)(2,1)(a)
  k1_val += ( 2.0) * sin_osh * m_osx;  // (1,2)(2,1)(b)
  k1_val += (-2.0) * cos_osh * m_cny;  // (1,4)(4,1)(a)
  k1_val += (-2.0) * sin_osh * m_cnx;  // (1,4)(4,1)(b)

  return(k1_val);
}

//----------------------------------------------------------------
// Procedure: k2_osh()

double CPXEngineThin::k2_osh(double osh) const
{
  double rad = degToRadians(osh);
  double cos_osh = cos(rad);
  double sin_osh = sin(rad);

  double k2_val = cos_osh * m_stat_cosCNH_x_cnSPD;  
  k2_val += sin_osh * m_stat_sinCNH_x_cnSPD;  

  return(k2_val);
}


//----------------------------------------------------------------
// Procedure: os_vthresh_osh()  (from initRateCache)

double CPXEngineThin::os_vthresh_osh(double osh) const
{
  // Note: m_stat_cn_to_os_closing set in setStaticRoot (in superclass)

  double thresh = 999;

  // Case 1: contact is closing ownship position, i.e., contact speed
  //         vector is in the direction of ownship rather than away.
  //         The cache will be a cache of speeds ownship must be
  //         greather than or equal to, if the current point in time
  //         is to be considered the CPA.

  if(m_stat_cn_to_os_closing) {
    double relang_os_to_cn = relAng(m_cnx, m_cny, m_osx, m_osy);
    double delta = osh - relang_os_to_cn;
    double cos_delta = cos(degToRadians(delta));
    if(cos_delta > 0.0001) 
      thresh = m_stat_cn_to_os_spd / cos_delta;
    return(thresh);
  }   

  // Case 2: contact is opening ownship position, i.e., contact speed
  //         vector is in direction away from ownship rather than toward.
  //         The cache will be a cache of speeds ownship must be less
  //         than or equal to, if the current point in time is to be 
  //         considered the CPA.
  
  double relang_os_to_cn = relAng(m_osx, m_osy, m_cnx, m_cny);
  double delta = osh - relang_os_to_cn;    
  double cos_delta = cos(degToRadians(delta));
  if(cos_delta > 0.0001)
    thresh = -m_stat_cn_to_os_spd / cos_delta;
  return(thresh);
}


//----------------------------------------------------------------
// Procedure: os_cn_relbng_osh()

double CPXEngineThin::os_cn_relbng_osh(double osh) const
{
  double angle_os_to_cn = relAng(m_osx, m_osy, m_cnx, m_cny);
  double rel_bng = angle_os_to_cn - osh;

  if(rel_bng < 0)
    rel_bng += 360;
  else if(rel_bng >= 360)
    rel_bng -= 360;

  return(rel_bng);
}

//----------------------------------------------------------------
// Procedure: os_cn_relbng_cos_osh()

double CPXEngineThin::os_cn_relbng_cos_osh(double osh) const
{
  double angle_os_to_cn = relAng(m_osx, m_osy, m_cnx, m_cny);
  double rel_bng = angle_os_to_cn - osh;

  if(rel_bng < 0)
    rel_bng += 360;
  else if(rel_bng >= 360)
    rel_bng -= 360;

  // Special case, cos(90/270) should be exactly zero. Converting 90
  // to PI/2 introduces rounding error with cos(90). Special case
  // handle here.

  if((rel_bng == 90) || (rel_bng == 270))
    return(0);
  
  double radians = degToRadians(rel_bng); 
  double os_cn_relbng_cos = cos(radians);

  return(os_cn_relbng_cos);
}

//----------------------------------------------------------------
// Procedure: os_tn_cos()

double CPXEngineThin::os_tn_cos(double osh) const
{
  double delta = osh - m_stat_theta_tn;
  if(delta > 180)
    delta -= 360;
  else if(delta < -180)
    delta += 360;
  if(delta < 0)
    delta = -delta;
  
  // Special case, cos(90) should be exactly zero. Converting 90 to
  // PI/2 introduces rounding error with cos(90). Special case handle
  // here.
  if(delta == 90)
    return(0);

  double radians = degToRadians(delta); 
  double os_tn_cos = cos(radians);
  return(os_tn_cos);
}

//----------------------------------------------------------------
// Procedure: os_gam_cos()

double CPXEngineThin::os_gam_cos(double osh) const
{
  double delta = osh - m_stat_theta_os_gam;
  if(delta > 180)
    delta -= 360;
  else if(delta < -180)
    delta += 360;
  if(delta < 0)
    delta = -delta;
  
  // Set the cosine value. Handle, as a special case, when delta is
  // 90 degrees. We know that cos(90) is zero, but when converting
  // 90 degrees to PI/2, there is imprecision. This imprecision results
  // in cos(90) having a non-zero value out to the 10th decimal or so.
  if(delta == 90)
    return(0);

  double gam = degToRadians(delta); 
  double os_gam_cos = cos(gam); 

  return(os_gam_cos);
}

