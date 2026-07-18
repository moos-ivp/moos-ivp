/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CPXEngine.cpp                                        */
/*    DATE: June 12th 2017                                       */
/*    DATE: June 24th 2025  Mods to support platmodel version    */
/*    DATE: Dec 28th 2025  More Mods to support platmodel        */
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
#include "CPXEngine.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

#define MPI 3.14159265359

using namespace std;

//----------------------------------------------------------
// Constructor()

CPXEngine::CPXEngine(double cny, double cnx, double cnh, double cnv,
		     double osy, double osx, double osh, double osv)
{
  CPXEngine::reset(cny, cnx, cnh, cnv, osy, osx, osh, osv);

  // Init of trig cache only in constructor. Not repeated on reset.
  initTrigCache();
}

//----------------------------------------------------------
// Procedure: reset()

void CPXEngine::reset(double cny, double cnx, double cnh, double cnv,
		      double osy, double osx, double osh, double osv)
{
  m_cny=cny;  m_cnx=cnx;
  m_cnv=cnv;  m_cnh=angle360(cnh);
  m_osx=osx;  m_osy=osy;
  m_osv=osv;  m_osh=angle360(osh);  

  if(m_cnv < 0)  m_cnv = 0;
  if(m_osv < 0)  m_osv = 0;

  m_stat_range = 0;
  
  m_stat_abs_bng_cn_os = 0;
  m_stat_rel_bng_cn_os = 0;
  m_stat_abs_bng_os_cn = 0;
  m_stat_rel_bng_os_cn = 0;

  m_stat_os_fore_of_cn = false;
  m_stat_os_aft_of_cn  = false;
  m_stat_os_port_of_cn = false;
  m_stat_os_star_of_cn = false;

  m_stat_cn_to_os_spd     = 0;
  m_stat_cn_to_os_closing = false;    

  m_stat_os_on_contact      = false;
  m_stat_os_on_bowline      = false;
  m_stat_os_on_sternline    = false; 
  m_stat_os_on_bowsternline = false;
  m_stat_os_on_beam         = false;

  m_stat_range_gam = 0;
  m_stat_range_eps = 0;

  setStaticRoot();
}

//----------------------------------------------------------
// Procedure: getSpec()

string CPXEngine::getSpec() const
{
  string s;
  s += "osx:" + doubleToStringX(m_osx,2);
  s += ",osy:" + doubleToStringX(m_osy,2);
  s += ",osv:" + doubleToStringX(m_osv,2);
  s += ",osh:" + doubleToStringX(m_osh,2);

  s += ",cnx:" + doubleToStringX(m_cnx,2);
  s += ",cny:" + doubleToStringX(m_cny,2);
  s += ",cnv:" + doubleToStringX(m_cnv,2);
  s += ",cnh:" + doubleToStringX(m_cnh,2);
  return(s);
}

// ================================================================
// Group 3 
// ================================================================

double CPXEngine::bearingRate(double osh, double osv) const
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
  // tangent angle
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

// ================================================================
// Group 3 
// ================================================================

double CPXEngine::osSpdInCNPos() const
{
  double bearing = relAng(m_osx,m_osy, m_cnx,m_cny);
  double delta = m_osh - bearing;
  if(delta > 180)
    delta -= 360;
  else if(delta < 180)
    delta += 360;
  if(delta < 0)
    delta = -delta;

  double rad = degToRadians(delta);
  double os_spd_in_os_pos = cos(rad) * m_osv;

  return(os_spd_in_os_pos);
}

double CPXEngine::osSpdInCNHdg() const
{
  double delta = m_osh - m_cnh;
  if(delta > 180)
    delta -= 360;
  else if(delta < 180)
    delta += 360;
  if(delta < 0)
    delta = -delta;

  double rad = degToRadians(delta);
  double os_spd_in_os_hdg = cos(rad) * m_osv;

  return(os_spd_in_os_hdg);
}

double CPXEngine::cnSpdInOSHdg() const
{
  double delta = m_cnh - m_osh;
  if(delta > 180)
    delta -= 360;
  else if(delta < 180)
    delta += 360;
  if(delta < 0)
    delta = -delta;

  double rad = degToRadians(delta);
  double cn_spd_in_os_hdg = cos(rad) * m_cnv;

  return(cn_spd_in_os_hdg);
}


//----------------------------------------------------------
// Procedure: setStaticRoot()

void CPXEngine::setStaticRoot()
{
  // Part 1: Set relative bearing and contact angle
  m_stat_abs_bng_cn_os = relAng(m_cnx, m_cny, m_osx, m_osy);
  m_stat_rel_bng_cn_os = relBearing(m_cnx, m_cny, m_cnh, m_osx, m_osy);
  m_stat_abs_bng_os_cn = relAng(m_osx, m_osy, m_cnx, m_cny);
  m_stat_rel_bng_os_cn = relBearing(m_osx, m_osy, m_osh, m_cnx, m_cny);

  // Part 2: Check of ownship is on present contact position
  m_stat_os_on_contact = false;
  if((m_osx == m_cnx) && (m_osy == m_cny))
    m_stat_os_on_contact = true;

  // Part 3: Check if ownship is on bowline or sternline of contact. 
  m_stat_os_on_bowline = false;
  m_stat_os_on_sternline = false;

  double angle_cn_to_os = relAng(m_cnx, m_cny, m_osx, m_osy);
  if(angle_cn_to_os == angle360(m_cnh))
    m_stat_os_on_bowline = true;
  else if(angle_cn_to_os == angle360(m_cnh-180))
    m_stat_os_on_sternline = true;

  // Part 4: Check if ownship on constct bowsternline given above
  m_stat_os_on_bowsternline = false;
  if(m_stat_os_on_contact || m_stat_os_on_sternline || m_stat_os_on_bowline)
    m_stat_os_on_bowsternline = true;

  // Part 5: Determine For/Aft and Port/Starboard rel positions
  m_stat_os_fore_of_cn = ptForeOfOwnship(m_cnx,m_cny,m_cnh, m_osx,m_osy); 
  m_stat_os_aft_of_cn  = ptAftOfOwnship(m_cnx,m_cny,m_cnh, m_osx,m_osy); 
  m_stat_os_star_of_cn = ptStarOfOwnship(m_cnx,m_cny,m_cnh, m_osx,m_osy); 
  m_stat_os_port_of_cn = ptPortOfOwnship(m_cnx,m_cny,m_cnh, m_osx,m_osy); 

  // Part 6: Determind ownship is on contact beam
  if(m_stat_os_on_contact || (m_stat_os_fore_of_cn && m_stat_os_aft_of_cn))
    m_stat_os_on_beam = true;
  else
    m_stat_os_on_beam = false;

  // Part 7: Determine cn_to_os_spd and if contact is closing
  //
  // Note Special case, cos(90) should be exactly zero. Converting 90
  // to PI/2 introduces rounding error with cos(90). Special case
  // handle here.
  m_stat_cn_to_os_spd = 0;
  if(m_stat_rel_bng_cn_os != 90) 
    m_stat_cn_to_os_spd = m_cnv * cos(degToRadians(m_stat_rel_bng_cn_os));

  m_stat_cn_to_os_closing = (m_stat_cn_to_os_spd > 0);

  m_stat_range = hypot(m_osx-m_cnx, m_osy-m_cny);


  // Part 8: Init gamma range (dist to contact bowstern line)

  // First calc rel_bng_cn_to_os_180 to be in the range [0 180]
  double rel_bng_cn_to_os_180 = m_stat_rel_bng_cn_os;
  if(rel_bng_cn_to_os_180 > 180)
    rel_bng_cn_to_os_180 -= 360;
  else if(rel_bng_cn_to_os_180 < -180)
    rel_bng_cn_to_os_180 += 360;
  if(rel_bng_cn_to_os_180 < 0)
    rel_bng_cn_to_os_180 = -rel_bng_cn_to_os_180;

  if(m_stat_os_on_bowsternline) 
    m_stat_range_gam = 0;
  else {    
    double theta_b = 90 - rel_bng_cn_to_os_180;
    double theta_b_rad = degToRadians(theta_b);
    double cos_theta_b = cos(theta_b_rad); // Note theta_b cannot be 90,-90.
    m_stat_range_gam = m_stat_range * cos_theta_b;
  }
  
  // Part 9: Init epsilon range (dist to contact's beam)
  if(m_stat_os_on_beam) 
    m_stat_range_eps = 0;
  else {
    double range_2d     = m_stat_range * m_stat_range;
    double range_gam_2d = m_stat_range_gam * m_stat_range_gam;
    m_stat_range_eps = sqrt(range_2d - range_gam_2d);
  }
}

//----------------------------------------------------------------
// Procedure: initTrigCache()

void CPXEngine::initTrigCache()
{
  vector<double> virgin_cache(360,0);
  m_cos_cache = virgin_cache;
  m_sin_cache = virgin_cache;

  for(unsigned int i=0; i<360; i++) {
    double rad = degToRadians(i);
    m_cos_cache[i] = cos((double)(rad));
    m_sin_cache[i] = sin((double)(rad));
  }
}

