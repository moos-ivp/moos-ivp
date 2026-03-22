/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CPXEnginePM.cpp                                      */
/*    DATE: Dec 26th 2025                                        */
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
#include "CPXEnginePM.h"
#include "CPXEngineThin.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

#define MPI 3.14159265359

//----------------------------------------------------------
// Procedure: Constructor()

CPXEnginePM::CPXEnginePM(double cny, double cnx,
			 double cnh, double cnv,
			 double osy, double osx,
			 double osh, double osv)
  : CPXEngine(cny, cnx, cnh, cnv, osy, osx, osh, osv)
{
  CPXEnginePM::reset(cny, cnx, cnh, cnv, osy, osx, osh, osv);
}

//----------------------------------------------------------
// Procedure: reset()

void CPXEnginePM::reset(double cny, double cnx,
			double cnh, double cnv,
			double osy, double osx,
			double osh, double osv)
{
  CPXEngine::reset(cny, cnx, cnh, cnv, osy, osx, osh, osv);
}

// ===============================================================
// Group 2 Range and Range Rate
// ===============================================================

//----------------------------------------------------------------
// Procedure: evalROC()
//   Purpose: Determine rate-of-closure for a given heading,speed
//      Note: This version of the function does not use a cache,
//            unlike the version working with a holonomic model.
//            It is presumed that the only time this function will
//            be called is for the current osh and osv, not for
//            candidate osh/osv maneuevers. Therefore the ROI does
//            not warrant the building/using of cache.

double CPXEnginePM::evalROC(double osh, double osv) const
{
  double bearing = relAng(m_osx,m_osy, m_cnx,m_cny);
  double delta = osh - bearing;
  if(delta > 180)
    delta -= 360;
  else if(delta < 180)
    delta += 360;
  if(delta < 0)
    delta = -delta;

  double rad = degToRadians(delta);
  double os_spd_in_cn_pos = cos(rad) * osv;  
  double roc = os_spd_in_cn_pos + m_stat_cn_to_os_spd;

  return(roc);
}

// ===============================================================
// Group 3 Bearing Rate
// ===============================================================
//----------------------------------------------------------------
// Procedure: bearingRate() 
//      Note: This version of the function does not use a cache,
//            unlike the version working with a holonomic model.
//            It is presumed that the only time this function will
//            be called is for the current osh and osv, not for
//            candidate osh/osv maneuevers. Therefore the ROI 
//            warrent the building/using of cache.

double CPXEnginePM::bearingRate(double osh, double osv) const
{
  // Part 1: Calculate the tangent angle
  double relang_os_to_cn = relAng(m_osx, m_osy, m_cnx, m_cny);
  double tangent_angle   = angle360(relang_os_to_cn - 90);

  // Part 2: Calculate the speed of ownship in the direction of the
  // tangent angle
  double os_delta_heading = osh - tangent_angle;
  if(os_delta_heading > 180)
    os_delta_heading -= 360;
  else if(os_delta_heading < -180)
    os_delta_heading += 360;
  if(os_delta_heading < 0)
    os_delta_heading = -os_delta_heading;

  // Special case, cos(90) should be exactly zero. Converting 90 to PI/2
  // introduces rounding error with cos(90). Special case handle here.
  double os_spd_at_tangent = 0;
  if(os_delta_heading != 90)
    os_spd_at_tangent = cos(degToRadians(os_delta_heading)) * -osv;

  // Part 3: Calculate the speed of contact in the direction of the
  // tangent angle double cn_delta_heading = m_cnh - tangent_angle;
  double cn_delta_heading = m_cnh - tangent_angle;
  if(cn_delta_heading > 180)
    cn_delta_heading -= 360;
  else if(cn_delta_heading < -180)
    cn_delta_heading += 360;
  if(cn_delta_heading < 0)
    cn_delta_heading = -cn_delta_heading;

  // Special case, cos(90) should be exactly zero. Converting 90 to PI/2
  // introduces rounding error with cos(90). Special case handle here. 
  double cn_spd_at_tangent = 0;
  if(cn_delta_heading != 90)
    cn_spd_at_tangent = cos(degToRadians(cn_delta_heading)) * m_cnv;

  double spd_at_tangent_angle = os_spd_at_tangent + cn_spd_at_tangent;

  // Part 4: Calculate the bearing rate from the speed at tangent
  double bng_rate = spd_at_tangent_angle * (-360 / (2*m_stat_range*MPI));

  return(bng_rate);
}


// ===============================================================
// Group A - CPA
// ===============================================================
//----------------------------------------------------------------
// Procedure: evalCPA
//   Purpose: Evaluates the given <Course, Speed, Time-on-leg> tuple 
//            Determines Closest-Point-of-Approach (CPA)

double CPXEnginePM::evalCPA(double osh, double osv, double ostol) const
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

double CPXEnginePM::evalTimeCPA(double osh, double osv, double ostol) const
{
  double xdist = -1;
  double xtime = -1;

  evalCPA(osh, osv, ostol, xdist, xtime);
  return(xtime);
}

//----------------------------------------------------------------
// Procedure: evalCPA()
//   Purpose: Evaluates the given <Course, Speed, Time-on-leg> tuple 
//            Determines Time of Closest-Point-of-Approach (CPA)

void CPXEnginePM::evalCPA(double osh, double osv, double ostol,
			    double& xdist, double& xtime) const
{
  XYSeglr seglr = m_plat_model.getTurnSeglr(osh);

  for(unsigned int i=0; i<seglr.size(); i++) {
    double osx, osy, osh, cnx, cny, segt = 0;
    bool ok = seglrIndex(seglr, i, osx, osy, osh, osv, cnx, cny, segt);
    if(!ok)
      break;

    CPXEngineThin cpa_engine(cny,cnx,m_cnh,m_cnv, osy,osx,osh,osv);
    cpa_engine.evalCPA(osh,osv,ostol, xdist,xtime);

    if(xtime >= 0) {
      if((segt < 0) || (xtime <= segt)) {
	double pdist = seglr.getVertPDist(i);
	double ptime = pdist / osv;
	xtime += ptime;
	return;
      } 
    }
  }

  xdist = -1;
  xtime = -1;
}


// ===============================================================
// Group B - Checks for Passing Port and/or Starboard
// ===============================================================

//----------------------------------------------------------------
// Procedure: passesPort()                  (Convenience function)

//   Purpose: checks to see if ownship, on the given hdg and spd,
//            will pass the contact on the contact's port side. 
//            A "pass" means it will cross the contact's beam, 
//            the line perpendicular to contact's bow-stern line.

bool CPXEnginePM::passesPort(double osh, double osv) const
{
  double xdist = -1;
  double xtime = -1;

  bool passes_port = passesPortDist(osh, osv, xdist, xtime);
  return(passes_port);
}

//----------------------------------------------------------------
// Procedure: passesPortDist()              (Convenience function)
 
double CPXEnginePM::passesPortDist(double osh, double osv) const
{
  double xdist = -1;
  double xtime = -1;

  passesPortDist(osh, osv, xdist, xtime);
  return(xdist);
}

//----------------------------------------------------------------
// Procedure: passesPortTime()              (Convenience function)
 
double CPXEnginePM::passesPortTime(double osh, double osv) const
{
  double xdist = -1;
  double xtime = -1;

  passesPortDist(osh, osv, xdist, xtime);
  return(xtime);
}

//----------------------------------------------------------------
// Procedure: passesPortDist()

bool CPXEnginePM::passesPortDist(double osh, double osv,
				 double& xdist, double& xtime) const
{
  XYSeglr seglr = m_plat_model.getTurnSeglr(osh);

  for(unsigned int i=0; i<seglr.size(); i++) {
    double osx, osy, osh, cnx, cny, segt = 0;
    bool ok = seglrIndex(seglr, i, osx, osy, osh, osv, cnx, cny, segt);

    if(!ok)
      break;

    CPXEngineThin cpa_engine(cny,cnx,m_cnh,m_cnv, osy,osx,osh,osv);
    cpa_engine.passesPortDist(osh,osv,xdist,xtime);

    if(xtime >= 0) {
      if((segt < 0) || (xtime <= segt)) {
	double pdist = seglr.getVertPDist(i);
	double ptime = pdist / osv;
	xtime += ptime;
	return(true);
      } 
    }
  }

  xdist = -1;
  xtime = -1;
  return(false);
}

//----------------------------------------------------------------
// Procedure: passesStar()
//   Purpose: checks to see if ownship, on the given heading and speed,
//            will pass the contact on the contact's starboard side. 
//            A "pass" means it will cross the line perpendicular to the 
//            bow-stern line.

bool CPXEnginePM::passesStar(double osh, double osv) const
{
  double xdist = -1;
  double xtime = -1;

  bool passes_star = passesStarDist(osh, osv, xdist, xtime);
  return(passes_star);
}


//----------------------------------------------------------------
// Procedure: passesStarDist()              (Convenience function)
 
double CPXEnginePM::passesStarDist(double osh, double osv) const
{
  double xdist = -1;
  double xtime = -1;

  passesStarDist(osh, osv, xdist, xtime);
  return(xdist);
}

//----------------------------------------------------------------
// Procedure: passesStarTime()              (Convenience function)
 
double CPXEnginePM::passesStarTime(double osh, double osv) const
{
  double xdist = -1;
  double xtime = -1;

  passesStarDist(osh, osv, xdist, xtime);
  return(xtime);
}

//----------------------------------------------------------------
// Procedure: passesStarDist()

bool CPXEnginePM::passesStarDist(double osh, double osv,
				 double& xdist, double& xtime) const
{
  XYSeglr seglr = m_plat_model.getTurnSeglr(osh);

  for(unsigned int i=0; i<seglr.size(); i++) {
    double osx = 0;
    double osy = 0;
    double osh = 0;
    double cnx = 0;
    double cny = 0;
    double segt = 0; // Time window on current leg, or -1 if ray

    bool ok = seglrIndex(seglr, i, osx, osy, osh, osv, cnx, cny, segt);
    if(!ok)
      break;

    CPXEngineThin cpa_engine(cny,cnx,m_cnh,m_cnv, osy,osx,osh,osv);
    cpa_engine.passesStarDist(osh,osv,xdist,xtime);

    if(xtime >= 0) {
      if((segt < 0) || (xtime <= segt)) {
	double pdist = seglr.getVertPDist(i);
	double ptime = pdist / osv;
	xtime += ptime;
	return(true);
      } 
    }
  }

  xdist = -1;
  xtime = -1;
  return(false);
}

//----------------------------------------------------------------
// Procedure: passesPortOrStar()
//   Purpose: checks to see if ownship, on the given heading and speed,
//            will pass the contact. A "pass" means it will cross the 
//            line perpendicular to the bow-stern line. (crosses the beam)

bool CPXEnginePM::passesPortOrStar(double osh, double osv) const
{
  if(passesPort(osh, osv))
    return(true);
  if(passesStar(osh, osv)) 
    return(true);
  
  return(false);
}


// ===============================================================
// Group C -  Checks for Crossing Stern and/or Bow 
// ===============================================================
//----------------------------------------------------------------
// Procedure: crossesStern()
//   Purpose: Determine if for the given ownship course and speed
//            whether it is on a path to cross the path of the 
//            contact on its stern.

bool CPXEnginePM::crossesStern(double osh, double osv) const
{
  double xdist = -1;
  double xtime = -1;

  bool crosses_stern = crossesSternDist(osh, osv, xdist, xtime);
  return(crosses_stern);
}

//----------------------------------------------------------------
// Procedure: crossesSternDist()  convenience function
//
//   Purpose: Determine if for the given ownship course and speed
//            whether it is on a path to cross the path of the 
//            contact on its stern. And if so, at what distance when
//            it crosses?

double CPXEnginePM::crossesSternDist(double osh, double osv) const
{
  double xdist = -1;
  double xtime = -1;

  crossesSternDist(osh, osv, xdist, xtime);
  return(xdist);
}

//----------------------------------------------------------------
// Procedure: crossesSternTime()  convenience function
//
//   Purpose: Determine if for the given ownship course and speed
//            whether it is on a path to cross the path of the 
//            contact on its stern. And if so, how long from now.

double CPXEnginePM::crossesSternTime(double osh, double osv) const
{
  double xdist = -1;
  double xtime = -1;

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

bool CPXEnginePM::crossesSternDist(double osh, double osv,
				   double& xdist, double& xtime) const
{
  XYSeglr seglr = m_plat_model.getTurnSeglr(osh);

  for(unsigned int i=0; i<seglr.size(); i++) {
    double osx, osy, osh, cnx, cny, segt = 0;
    bool ok = seglrIndex(seglr, i, osx, osy, osh, osv, cnx, cny, segt);
    if(!ok)
      break;

    CPXEngineThin cpa_engine(cny,cnx,m_cnh,m_cnv, osy,osx,osh,osv);
    cpa_engine.crossesSternDist(osh,osv,xdist,xtime);

    if(xtime >= 0) {
      if((segt < 0) || (xtime <= segt)) {
	double pdist = seglr.getVertPDist(i);
	double ptime = pdist / osv;
	xtime += ptime;
	return(true);
      } 
    }
  }

  xdist = -1;
  xtime = -1;
  return(false);
}

//----------------------------------------------------------------
// Procedure: crossesBow()

bool CPXEnginePM::crossesBow(double osh, double osv) const
{
  double xdist = -1;
  double xtime = -1;

  bool crosses_bow = crossesBowDist(osh, osv, xdist, xtime);
  return(crosses_bow);
}

//----------------------------------------------------------------
// Procedure: crossesBowDist()  (Convenience function)
 
double CPXEnginePM::crossesBowDist(double osh, double osv) const
{
  double xdist = -1;
  double xtime = -1;

  crossesBowDist(osh, osv, xdist, xtime);
  return(xdist);
}

//----------------------------------------------------------------
// Procedure: crossesBowTime()  (Convenience function)
 
double CPXEnginePM::crossesBowTime(double osh, double osv) const
{
  double xdist = -1;
  double xtime = -1;

  crossesBowDist(osh, osv, xdist, xtime);
  return(xtime);
}

//----------------------------------------------------------------
// Procedure: crossesBowDist()

bool CPXEnginePM::crossesBowDist(double osh, double osv,
				 double& xdist, double& xtime) const
{
  XYSeglr seglr = m_plat_model.getTurnSeglr(osh);

  for(unsigned int i=0; i<seglr.size(); i++) {
    double osx, osy, osh, cnx, cny, segt = 0;
    bool ok = seglrIndex(seglr, i, osx, osy, osh, osv, cnx, cny, segt);
    if(!ok)
      break;

    CPXEngineThin cpa_engine(cny,cnx,m_cnh,m_cnv, osy,osx,osh,osv);
    cpa_engine.crossesBowDist(osh,osv,xdist,xtime);

    if(xtime >= 0) {
      if((segt < 0) || (xtime <= segt)) {
	double pdist = seglr.getVertPDist(i);
	double ptime = pdist / osv;
	xtime += ptime;
	return(true);
      } 
    }
  }

  xdist = -1;
  xtime = -1;
  return(false);
}

//----------------------------------------------------------------
// Procedure: crossesBowOrStern()
//   Purpose: Determine if for the given ownship course and speed
//            whether it is on a path to cross the path of the 
//            contact on its stern or bow. 

bool CPXEnginePM::crossesBowOrStern(double osh, double osv) const
{
  if(crossesBow(osh, osv))
    return(true);
  if(crossesStern(osh, osv))
    return(true);
  
  return(false);
}


//----------------------------------------------------------------
// Procedure: seglrIndex()
//   Purpose: This is a core utility function for all CPAEngine
//            core function groups.
//            The input is
//              (a) Contact position, hdg, spd: mcnx,mcny,mcnh,mcnv
//              (b) Ownship speed: mosv
//              (c) A Seglr which presumably represents an ownship
//                  turn, originating from base vertex (index 0).
//              (d) an index into that Seglr.
//            The goal, based on the index into the Seglr, is to
//              (1) Determine the ownship position based on the
//                  index into Seglr. Ownship position will always
//                  be exactly at one of the vertices in the Seglr
//              (2) Determine ownship heading based on the vertex
//                  and the angle to the next vertex in the Seglr.
//                  If the vertex is the last one, it is simply
//                  the ray angle of the Seglr.
//              (3) Given ownship position on one of the vertices,
//                  determine how much *time* it took ownship 
//                  to get to this position based on the distance
//                  of previous segments, and ownship speed.
//              (4) Advance the contact to a new cnx,cny position,
//                  based on (a) the time in ownship's past, (b)
//                  the contact start position (c) the contact
//                  heading and (d) contact speed.
//
//      Note: ownship position will be at the vertex in the seglr
//            given by index (ix).
//      Note: ownship hdg is determined by the direction from
//            vertex ix to ix+1, or the ray angle if ix is the
//            last vertex.
//      Note: contact position will be forwarded a distance based
//            on (a) the Time Prior to index (ix) and cn spd.
//      Note: Time Prior is based on the sum of distances for
//            all segments prior to index (ix), and osv.
//  
//  m_cnx,m_cny                            ---+
//   o-----------o--------------->            |          
//               cnx,cny                      |
//                                            |
//                                            |
//                                            |
//     [1]           [2]                      |
//       o------------o                       |
//      /              \                      |
//     /                \                     |
//    /                  V                    | 
//   o [0]                                 ---+
//
// Inputs: m_cnx, m_cny, m_cnh, m_cnv
//         m_osv

bool CPXEnginePM::seglrIndex(const XYSeglr& seglr, unsigned int ix,
			     double& osx, double& osy, double& osh, double osv,
			     double& cnx, double& cny, double& segt) const
{
  // Init answers to default answers
  //osx = 0;
  //osy = 0;
  //osh = 0;
  //cnx = mcnx;
  //cny = mcny;

  // Sanity Check 1: Must have valid seglr (at least size 1)
  if(!seglr.valid())
    return(false);
  
  // Sanity check 2: index (ix) in bounds. 
  if(ix >= seglr.size())
    return(false);

  // Sanity check 3: If osv is zero, then the os position and
  // hdg based on vertex 0 in the seglr, and cn is not advanced
  if(osv <= 0) {
    osx = seglr.getVX(0);
    osy = seglr.getVY(0);
    osh = seglr.getVertAngle(0);
    cnx = m_cnx;
    cny = m_cny;
    segt = -1;
    return(true);
  }

  // Part 1: Determine Time/Dist Prior to current index (ix)
  //   Note: We do not make assumption of equal distance
  //         of line segments.
  //   Note: If index (ix) is 0, prior dist and time are zero.
  
  double dist_prior = 0;
  for(unsigned int i=0; i<ix; i++) {
    double x1 = seglr.getVX(i);
    double y1 = seglr.getVY(i);
    double x2 = seglr.getVX(i+1);
    double y2 = seglr.getVY(i+1);
    dist_prior += hypot(x1-x2, y1-y2);
  }
  double time_prior = dist_prior / osv;

  // Part 2: Advance the contact position
  if(m_cnv > 0) {
    double cn_dist = time_prior * m_cnv;
    projectPoint(m_cnh, cn_dist, m_cnx, m_cny, cnx, cny);
  }
    
  // Part 3: Set ownship position
  osx = seglr.getVX(ix);
  osy = seglr.getVY(ix);
  osh = seglr.getVertAngle(ix);

  // vdist is dist from vertex ix, to the next vertex, or -1
  // if ix is the last vertex
  double vdist = seglr.getVertDist(ix);
  if(vdist < 0)
    segt = -1;
  else
    segt = vdist / osv;

  return(true);
}

