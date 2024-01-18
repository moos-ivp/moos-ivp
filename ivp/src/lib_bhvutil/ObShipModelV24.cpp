/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / MIT Cambridge MA            */
/*    FILE: ObShipModelV24.cpp                                   */
/*    DATE: Sep 6th, 2019                                        */
/*    DATE: Jul 31st, 2023 ObShipModelX with PlatModel           */
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
#include <cstdlib>
#include <cmath>
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "ObShipModelV24.h"
#include "XYFormatUtilsPoly.h"
#include "XYPolyExpander.h"
#include "FileBuffer.h"
 
using namespace std;

// ----------------------------------------------------------
// Constructor()

ObShipModelV24::ObShipModelV24(double osx, double osy,
			       double osh, double osv)
{
  // Config vars (affecting PlatModel and dynamic cache)
  m_min_util = 0;
  m_max_util = 100;
  m_min_util_cpa = 8;
  m_max_util_cpa = 16;  
  m_min_util_cpa_flex = 8;
  m_max_util_cpa_flex = 16;  

  m_pwt_inner_dist = 10;
  m_pwt_outer_dist = 50;
  m_allowable_ttc  = 20;

  m_completed_dist = 50;

  // Set the precision for rounding/expanding the obstacle_buff
  // polygon. Affects the work involved for CPA calculations
  m_obuff_rdegs = 30;
  
  // State variables
  m_cx = 0;
  m_cy = 0;

  m_dynamic_updates = 0;
  
  m_stale_cache = true;

  m_obcent_bng = -1;
  m_obcent_relbng = -1;

  m_range = -1;
  m_range_in_osh = -1;
  m_cpa_in_osh = -2;

  m_gut_bng_min_to_poly = 0;
  m_gut_bng_max_to_poly = 0;
  m_gut_bng_hit_count   = 0;
  m_gut_bng_unhit_count = 0;

  m_rim_bng_min_to_poly = 0;
  m_rim_bng_max_to_poly = 0;
  m_rim_bng_hit_count   = 0;
  m_rim_bng_unhit_count = 0;

  m_gut_bng_min_dist_to_poly = 0;
  m_gut_bng_max_dist_to_poly = 0;
  m_rim_bng_min_dist_to_poly = 0;
  m_rim_bng_max_dist_to_poly = 0;

  m_turn_cache_enabled = true;
  m_pause_update_dynamic = false;
}


// ----------------------------------------------------------
// Procedure: setPose()

bool ObShipModelV24::setPose(double osx, double osy,
			   double osh, double osv)
{
  setPoseOSX(osx);
  setPoseOSY(osy);
  setPoseOSH(osh);
  setPoseOSV(osv);
  m_stale_cache = true;
  return(true);
}


// ----------------------------------------------------------
// Procedure: setPoseOSX()

bool ObShipModelV24::setPoseOSX(double osx)
{
  m_plat_model.setOSX(osx);
  m_stale_cache = true;
  return(true);
}

// ----------------------------------------------------------
// Procedure: setPoseOSY()

bool ObShipModelV24::setPoseOSY(double osy)
{
  m_plat_model.setOSY(osy);
  m_stale_cache = true;
  return(true);
}

// ----------------------------------------------------------
// Procedure: setPoseOSH()

bool ObShipModelV24::setPoseOSH(double osh)
{
  m_plat_model.setOSH(osh);
  m_stale_cache = true;
  return(true);
}

// ----------------------------------------------------------
// Procedure: setPoseOSV()

bool ObShipModelV24::setPoseOSV(double osv)
{
  m_plat_model.setOSV(osv);
  m_stale_cache = true;
  return(true);
}

// ----------------------------------------------------------
// Procedure: setPlatModel()

void ObShipModelV24::setPlatModel(PlatModel plat_model)
{
  m_plat_model = plat_model;
  m_stale_cache = true;
}

// ----------------------------------------------------------
// Procedure: setMinUtil()

void ObShipModelV24::setMinUtil(double min_util)
{
  m_min_util = min_util;

  if(m_min_util < 0)
    m_min_util = 0;
  if(m_max_util < m_min_util)
    m_max_util = m_min_util;

  m_stale_cache = true;
}

// ----------------------------------------------------------
// Procedure: setMaxUtil()

void ObShipModelV24::setMaxUtil(double max_util)
{
  m_max_util = max_util;

  if(m_max_util < 0)
    m_max_util = 0;
  if(m_min_util > m_max_util)
    m_min_util = m_max_util;

  m_stale_cache = true;
}

// ----------------------------------------------------------
// Procedure: setMinMaxUtil()

void ObShipModelV24::setMinMaxUtil(double min_util, double max_util)
{
  if((min_util < 0) || (max_util < 0) || (min_util > max_util))
    return;

  m_min_util = min_util;
  m_max_util = max_util;

  m_stale_cache = true;
}

// ----------------------------------------------------------
// Procedure: setOBuffRDegs()
//   Purpose: When a polygon is expanded with a buffer region,
//            corner edges expand to rounded edges to achieve
//            a uniform buffer distance from the original poly.
//            These rounded corners, circular in theory, are
//            approximated with piecewise linear segments every
//            N degrees (m_obuff_rdegs degrees). Lower values
//            of N result in more rounded corners. Higher values
//            result in more jagged corners. Rounded corners
//            look nicer when viewed (not a high priority).
//            Jagged corners mean less calculations when
//            calculating CPA values. Somewhere in between is
//            nice, but we want the option to scale.

void ObShipModelV24::setOBuffRDegs(double dval)
{
  if(dval < 1)
    dval = 1;
  else if(dval > 45)
    dval = 45;
  m_obuff_rdegs = dval;

  m_stale_cache = true;
  //updateDynamic();
}

// ----------------------------------------------------------
// Procedure: setGutPoly()

string ObShipModelV24::setGutPoly(string polystr)
{
  XYPolygon new_poly = string2Poly(polystr);
  return(setGutPoly(new_poly));
}

// ----------------------------------------------------------
// Procedure: setGutPoly()

string ObShipModelV24::setGutPoly(XYPolygon new_gut_poly)
{
  if(!new_gut_poly.is_convex())
    return("obstacle is not convex");

  m_gut_poly = new_gut_poly;
  
  m_set_params.insert("gut_poly");

  // If this setting is just an adjustment, then updateDynamic()
  // is needed. It might also be an initial setting. So we ignore
  // the result of the update.
  //updateDynamic();
  
  m_stale_cache = true;
  return("");
}

// ----------------------------------------------------------
// Procedure: setPwtInnerDist()

string ObShipModelV24::setPwtInnerDist(double val)
{
  if(val < 0)
    return("pwt_inner_dist must be a positive number");
  
  m_pwt_inner_dist = val;
  m_set_params.insert("pwt_inner_dist");
  
  if(m_pwt_outer_dist < m_pwt_inner_dist) {
    m_pwt_outer_dist = m_pwt_inner_dist;
    if(m_set_params.count("pwt_outer_dist")) {
      return("pwt_inner_dist must be <= pwt_outer_dist");
    }
  }
  if(m_completed_dist < m_pwt_inner_dist) {
    m_completed_dist = m_pwt_inner_dist;
    if(m_set_params.count("completed_dist")) {
      return("pwt_inner_dist must be <= completed_dist");
    }
  }
  m_stale_cache = true;
  return("");    
}

// ----------------------------------------------------------
// Procedure: setPwtOuterDist()

string ObShipModelV24::setPwtOuterDist(double val)
{
  if(val < 0)
    return("pwt_outer_dist must be a positive number");

  m_stale_cache = true;

  m_pwt_outer_dist = val;
  m_set_params.insert("pwt_outer_dist");

  if(m_pwt_inner_dist > m_pwt_outer_dist) {
    m_pwt_inner_dist = m_pwt_outer_dist;
    if(m_set_params.count("pwt_inner_dist")) {
      return("pwt_outer_dist must be >= pwt_inner_dist");
    }
  }
  if(m_completed_dist < m_pwt_outer_dist) {
    m_completed_dist = m_pwt_outer_dist;
    if(m_set_params.count("completed_dist")) {
      return("pwt_outer_dist must be <= completed_dist");
    }
  }
  return("");  
}

// ----------------------------------------------------------
// Procedure: setCompletedDist()

string ObShipModelV24::setCompletedDist(double val)
{
  if(val < 0)
    return("completed_dist must be a positive number");
  
  m_stale_cache = true;
  m_completed_dist = val;
  m_set_params.insert("completed_dist");

  if(m_pwt_inner_dist > m_completed_dist) {
    m_pwt_inner_dist = m_completed_dist;
    if(m_set_params.count("pwt_inner_dist")) {
      return("completed_dist must be >= pwt_inner_dist");
    }
  }
  if(m_pwt_outer_dist > m_completed_dist) {
    m_pwt_outer_dist = m_completed_dist;
    if(m_set_params.count("pwt_outer_dist")) {
      return("completed_dist must be >= pwt_outer_dist");
    }
  }
  return("");  
}

// ----------------------------------------------------------
// Procedure: setMinUtilCPA()

string ObShipModelV24::setMinUtilCPA(double val)
{
  if(val < 0)
    return("min_util_cpa cannot be a negative number");
  
  m_stale_cache = true;
  m_min_util_cpa = val;
  m_set_params.insert("min_util_cpa");
  
  if(m_max_util_cpa < m_min_util_cpa) {
    m_max_util_cpa = m_min_util_cpa;
    if(m_set_params.count("max_util_cpa")) {
      return("min_util_cpa must be <= max_util_cpa");
    }
  }

  // If this setting is just an adjustment, then updateDynamic()
  // is needed. It might also be an initial setting. So we ignore
  // the result of the update.
  // updateDynamic();

  return("");
}

// ----------------------------------------------------------
// Procedure: setMaxUtilCPA()

string ObShipModelV24::setMaxUtilCPA(double val)
{
  if(val < 0)
    return("max_util_cpa cannot be a negative number");
  
  m_stale_cache = true;
  m_max_util_cpa = val;
  m_set_params.insert("max_util_cpa");

  if(m_min_util_cpa > m_max_util_cpa) {
    m_min_util_cpa = m_max_util_cpa;
    if(m_set_params.count("min_util_cpa")) {
      return("max_util_cpa must be >= min_util_cpa");
    }
  }

  // If this setting is just an adjustment, then updateDynamic()
  // is needed. It might also be an initial setting. So we ignore
  // the result of the update.
  // updateDynamic();

  return("");
}

// ----------------------------------------------------------
// Procedure: setAllowableTTC()

string ObShipModelV24::setAllowableTTC(double val)
{
  if(val < 0)
    return("allowable_ttc cannot be a negative number");

  m_stale_cache = true;
  m_allowable_ttc = val;
  m_set_params.insert("allowable_ttc");

  // If this setting is just an adjustment, then updateDynamic()
  // is needed. It might also be an initial setting. So we ignore
  // the result of the update.
  // updateDynamic();

  return("");
}

// ----------------------------------------------------------
// Procedure: paramIsSet()

bool ObShipModelV24::paramIsSet(string param) const
{
  if((param == "osx") || (param == "osy") ||
     (param == "osh") || (param == "osv"))
    return(m_plat_model.valid());
  
  if(m_set_params.count(param) == 0)
    return(false);
  return(true);
}

// ----------------------------------------------------------
// Procedure: ownshipInGutPoly()

bool ObShipModelV24::ownshipInGutPoly() const
{
  
  return(m_gut_poly.contains(getOSX(), getOSY()));
}

// ----------------------------------------------------------
// Procedure: ownshipInMidPoly()

bool ObShipModelV24::ownshipInMidPoly() const
{
  return(m_mid_poly.contains(getOSX(), getOSY()));
}

// ----------------------------------------------------------
// Procedure: ownshipInRimPoly()

bool ObShipModelV24::ownshipInRimPoly() const
{
  return(m_rim_poly.contains(getOSX(), getOSY()));
}


// ----------------------------------------------------------
// Procedure: getRangeRelevance()
//   Returns: A number in the range of [0,1]

double ObShipModelV24::getRangeRelevance() 
{
  // Part 1: Sanity checks
  if(m_pwt_outer_dist < m_pwt_inner_dist)
    return(0);

  //cout << "m_range: " << m_range << endl;
  
  // Part 2: Now the easy range cases: when the obstacle is outside 
  //         the min or max priority weight ranges
  if(m_range >= m_pwt_outer_dist)
    return(0);

  double pct = 0;
  if(m_range <= m_pwt_inner_dist)
    pct = 1;
  else { // Part 3: Handle the in-between case
    double drange = (m_pwt_outer_dist - m_pwt_inner_dist);
    if(drange <= 0)
      return(0);
    pct = (m_pwt_outer_dist - m_range) / drange;
  }

  //cout << "initial pct:" << pct << endl;
  
  // Part 4: Discount based on bearing to obstacle. Or full
  // weight if gut_poly is dead ahead.

  
  double osx = getOSX();
  double osy = getOSY();
  double osh = getOSH();

  
  // Part 4A: Edge cases: if for some reason the calc of gut
  // bng min/max not completed, or if all headings will hit.
  double bmin, bmax;
  bearingMinMaxToPoly(osx, osy, m_gut_poly, bmin, bmax);
  //cout << "bng_min: " << doubleToStringX(bmin) << endl;
  //cout << "bng_max: " << doubleToStringX(bmax) << endl;

  // Part 4C: If obstacle is dead ahead (angle wrap)
  if(bmin > bmax)
    return(pct);

  // Part 4D: General case. Discount based on cos(theta) where
  // theta is the smaller angle between ownship current heading
  // and one of the two bng_min/max vals.
  double angle_diff1 = angleDiff(osh, bmin);
  double angle_diff2 = angleDiff(osh, bmax);
  double theta = angle_diff1;
  if(angle_diff2 < angle_diff1)
    theta = angle_diff2;
  double theta_rad = degToRadians(theta);
  double cos_theta = cos(theta_rad);
  if(cos_theta < 0)
    cos_theta = 0;

  //cout << "osh: " << osh << endl;
  //cout << "angle_diff1:" << angle_diff1 << endl;
  //cout << "angle_diff2:" << angle_diff2 << endl;
  //cout << "theta:" << theta << endl;
  //cout << "theta_rad:" << theta_rad << endl;
  //cout << "cos_theta:" << cos_theta << endl;
  
  
  
  //cout << "pct: " << pct << endl;
  double pct2 = cos_theta * pct;
  //cout << "new_pct: " << pct2 << endl;
  if((pct > 0) && (pct2 == 0))
    pct2 = 0.01;
  //cout << "new_new_pct: " << pct2 << endl;

  return(pct2);
}

// ----------------------------------------------------------
// Procedure: isObstacleAft()
//      Note: The xbng parameter generalizes this function. Normally a
//            point is "aft" of ownship its relative bearing is in the
//            range (90,270). With the xbng parameter, "aft" can be
//            generalized, e.g., xbng=10 means the polygon must be at
//            least 10 degrees abaft of beam.


bool ObShipModelV24::isObstacleAft(double xbng) const
{
  double osx = getOSX();
  double osy = getOSY();
  double osh = getOSH();
  return(polyAft(osx, osy, osh, m_gut_poly, xbng));
}

// ----------------------------------------------------------
// Procedure: isValid()

bool ObShipModelV24::isValid() const
{
  string issue;
  if(m_pwt_outer_dist < m_pwt_inner_dist)
    issue = "pwt_outer < pwt_inner";
  if(m_completed_dist < m_pwt_outer_dist)
    issue += ",completed_dist < pwt_outer";
  if(m_max_util_cpa < m_min_util_cpa)
    issue += ",max_util_cpa < min_util_cpa";
  if(!m_gut_poly.is_convex())
    issue += ",gut_poly is not convex";
  if(!m_mid_poly.is_convex())
    issue += ",mid_poly is not convex";
  if(!m_rim_poly.is_convex())
    issue += ",rim_poly is not convex";
  if(!m_plat_model.valid())
    issue += ",plat_model not valid";
  if(m_stale_cache)
    issue += ",stale cache";

  if(issue != "") {
    cout << issue << endl;
    return(false);
  }

  return(true);
}


// ----------------------------------------------------------
// Procedure: getFailedExpandPolyStr()

string ObShipModelV24::getFailedExpandPolyStr(bool clear_val) 
{  
  string rval = m_failed_expand_poly_str;
  if(clear_val)
    m_failed_expand_poly_str = "";
  return(rval);
}

// ----------------------------------------------------------
// Procedure: print()

void ObShipModelV24::print(string key) const
{
  cout << "-------------------------------------" << endl;
  cout << "ObShipModelV24: (" << key << ")        " << endl;
  cout << "-------------------------------------" << endl;
  cout << "osx: " << getOSX() << endl;
  cout << "osy: " << getOSY() << endl;
  cout << "osh: " << getOSH() << endl;
  cout << "range: " << m_range << endl;
  cout << "min_util_cpa: " << m_min_util_cpa << endl;
  cout << "max_util_cpa: " << m_max_util_cpa << endl;
  cout << "min_util_cpa_flex: " << m_min_util_cpa_flex << endl;
  cout << "max_util_cpa_flex: " << m_max_util_cpa_flex << endl;
  cout << "pwt_inner_dist: " << m_pwt_inner_dist << endl;
  cout << "pwt_outer_dist: " << m_pwt_outer_dist << endl;
  cout << "m_allowable_ttc: " << m_allowable_ttc << endl;
  cout << "m_completed_dist: " << m_completed_dist << endl;
  cout << "gut_poly: " << m_gut_poly.get_spec() << endl; 
  cout << "mid_poly: " << m_mid_poly.get_spec() << endl; 
  cout << "rim_poly: " << m_rim_poly.get_spec() << endl; 
}

// ----------------------------------------------------------
// Procedure: printBnds()

void ObShipModelV24::printBnds() const
{
  cout << "-------------------------------------" << endl;
  cout << "ObShipModelV24 Bnds: " << m_dynamic_updates << endl;
  cout << "-------------------------------------" << endl;
  cout << "gut_bng_min_to_poly: " << m_gut_bng_min_to_poly << endl; 
  cout << "gut_bng_max_to_poly: " << m_gut_bng_max_to_poly << endl; 
  cout << "rim_bng_min_to_poly: " << m_rim_bng_min_to_poly << endl; 
  cout << "rim_bng_max_to_poly: " << m_rim_bng_max_to_poly << endl; 
}

// ----------------------------------------------------------
// Procedure: rayCPA()

double ObShipModelV24::rayCPA(double hdg,
			    double& rx, double &ry) const
{
  double osx = getOSX();
  double osy = getOSY();
  double cpa = polyRayCPA(osx, osy, hdg, m_gut_poly, rx, ry);

  return(cpa);
}
 
  
// ----------------------------------------------------------
// Procedure: seglrCPA()

double ObShipModelV24::seglrCPA(double hdg, double& rx, double &ry,
			      double& stemdist, bool verbose) const
{
  XYSeglr seglr = m_plat_model.getTurnSeglr(hdg);
  double cpa_dist = distSeglrToPoly(seglr, m_gut_poly, rx, ry,
				    stemdist, verbose);
  return(cpa_dist);
}

 
// ----------------------------------------------------------
// Procedure: evalHdgSpd()

double ObShipModelV24::evalHdgSpd(double hdg, double spd,
				bool verbose) const
{

#if 1
  double vpct = 1;
#endif
  
#if 0
  double fast_spd = 1;
  double safe_spd = getOSV();

  double vpct = (spd - safe_spd) / (fast_spd - safe_spd);
  if(vpct < 0.5)
    vpct = 0.5;
  if(vpct > 1)
    vpct = 1;
#endif
  
  double min_util_cpa = vpct * m_min_util_cpa;
  double max_util_cpa = vpct * m_max_util_cpa;

    
  double ix,iy; // ToDo reason about TTC with ix,iy 
  double stemdist;
  double cpa = seglrCPA(hdg, ix,iy, stemdist, verbose);

  if(cpa >= m_max_util_cpa)
    return(m_max_util);
  if(cpa <= m_min_util_cpa)
    return(m_min_util); 

  double util_range = max_util_cpa - min_util_cpa;
  // Sanity check - the util_range.
  if(util_range <= 0)
    return(m_min_util);
  
  double delta = cpa - min_util_cpa;
  double pct = delta / util_range;

  // Sanity check - pct should be in [0,1], due to above logic
  // but sanity check anyway
  if(pct < 0)
    pct = 0;
  else if(pct > 1)
    pct = 1;
  
  return(pct * (m_max_util - m_min_util));
}


// ----------------------------------------------------------
// Procedure: setCachedVals()

void ObShipModelV24::setCachedVals(bool force)
{
  
  if(!force && !m_stale_cache)
    return;
  
  // Will almost always enable turn_cache, but we leave it
  // as an optional setting so we can benchmark performance
  // when it is disable.
  //if(m_turn_cache_enabled)
  //  fillTurnCache();
  
  updateDynamic();
  
  m_stale_cache = false;
}

// ---------------------------------- PROTECTED -------------
// Procedure: fillTurnCache()                      
//   Purpose: Calculate intermediate values (segments on seglrs)
//            and store them in the PlatModel class.

void ObShipModelV24::fillTurnCache(const XYPolygon& poly)
{
  if(!m_turn_cache_enabled)
    return; 

  // =======================================================
  // Handle Port side (PlatModel not assumed symetric)
  // =======================================================
  double osx = getOSX();
  double osy = getOSY();

  XYPoint port_cpa_pt(osx, osy);
  double port_cpa = -1;
  double port_stem_cpa = -1;
  double pvx = osx;
  double pvy = osy;
  double ix  = 0;
  double iy  = 0;
  double prior_port_stem = 0;
  vector<XYPoint> ppts = m_plat_model.getPoints("port_spoke");
  for(unsigned int i=0; i<ppts.size(); i++) {
    double x2 = ppts[i].x();
    double y2 = ppts[i].y();
    double cpa = distSegToPoly(pvx,pvy, x2,y2, poly, ix,iy);
    if((port_cpa < 0) || (cpa < port_cpa)) {
      port_cpa_pt.set_vertex(ix,iy);
      port_cpa = cpa;
      port_stem_cpa = prior_port_stem + hypot(ix-pvx, iy-pvy); 
    }
    prior_port_stem += hypot(pvx-x2, pvy-y2);
    pvx=x2;
    pvy=y2;
    m_plat_model.setCacheDistCPA(true, i, port_cpa);
    m_plat_model.setCachePtCPA(true, i, port_cpa_pt);
    m_plat_model.setCacheStemCPA(true, i, port_stem_cpa);
  }

  // =======================================================
  // Handle Starboard side (PlatModel not assumed symetric)
  // =======================================================
  XYPoint star_cpa_pt(osx, osy);
  double star_cpa = -1;
  double star_stem_cpa = -1;
  double svx = osx;
  double svy = osy;
  double prior_star_stem = 0;
  vector<XYPoint> spts = m_plat_model.getPoints("star_spoke");
  for(unsigned int i=0; i<spts.size(); i++) {
    double x2 = spts[i].x();
    double y2 = spts[i].y();
    double cpa = distSegToPoly(svx,svy, x2,y2, poly, ix,iy);
    if((star_cpa < 0) || (cpa < star_cpa)) {
      star_cpa_pt.set_vertex(ix,iy);
      star_cpa = cpa;
      star_stem_cpa = prior_star_stem + hypot(ix-svx, iy-svy); 
    }
    prior_star_stem += hypot(svx-x2, svy-y2);
    svx=x2;
    svy=y2;
    m_plat_model.setCacheDistCPA(false, i, star_cpa);
    m_plat_model.setCachePtCPA(false, i, star_cpa_pt);
    m_plat_model.setCacheStemCPA(false, i, star_stem_cpa);    
  }
}

// ---------------------------------- PROTECTED -------------
// Procedure: updateDynamic()
//   Purpose: Update vars that can be cached/set once (a) ownship
//            position is set, (b) obstacle location is set, (c)
//            config params re: utility and priority are set.
//
//      Note: Needs to be recalculated when/if following change:
//            1. ownship position or heading (or PlatModel)
//            2. obstacle location or dimensions
//            3. min_util/max_util_cpa (affects mid/rim polys)
//
//  Vars (range)
//  1. m_range:
//     Range from ownship to closest pt on obstacle
//  2. m_range_in_os:
//     Range from ownship to obstacle along current os heading
//
//  Vars (flex cpa range)
//  3. m_min_util_cpa_flex
//     Same as min_util_cpa, but if m_range is already less
//     than min_util_cpa, set min_util_cpa_flex to m_rang
//  4. m_max_util_cpa_flex
//     Same as max_util_cpa, but if m_range is already less
//     than max_util_cpa, set max_util_cpa_flex to m_range
//
//  Vars (buffer polys)
//  5. m_mid_poly
//     The gut poly expanded by a distance of min_util_cpa.
//  6. m_rim_poly
//     The gut_poly expanded by a distance of max_util_cpa.
//     A cpa within this poly will have less than full utility,
//     degrading linearly down to zero utility for a CPA with
//     the smaller m_mid_poly.
//
//  Vars (other)
//  7. m_cx - obstacle center x
//  8. m_cy - obstacle center y
//  9. m_obcent_bng - angle from ownship to obst center
//  10. m_obcent_relbng - rel bearing from ownship center
//  11. m_cpa_in_osh - cpa of owship to obs in curr osh
//  12. m_passing_side - port/star rel obs ctr in curr osh

bool ObShipModelV24::updateDynamic() 
{
  if(!m_gut_poly.is_convex())
    return(false);

  m_dynamic_updates++;
  
  // =======================================================
  // Part 1: Update the range of ownship to the polygon
  // =======================================================
  double osx = getOSX();
  double osy = getOSY();
  double osh = getOSH();

  m_range = 0;
  m_range_in_osh = 0; 
  if(!m_gut_poly.contains(osx, osy)) {
    m_range = m_gut_poly.dist_to_poly(osx, osy);
    m_range_in_osh = m_gut_poly.dist_to_poly(osx, osy, osh);
  }

  // =======================================================
  // Part 2: Calculate the Flex Ranges
  // =======================================================
  m_min_util_cpa_flex = m_min_util_cpa;
  m_max_util_cpa_flex = m_max_util_cpa;

  if(m_range < (m_min_util_cpa_flex + 1)) {
    m_min_util_cpa_flex = m_range - 1;
    if(m_min_util_cpa_flex < 0)
      m_min_util_cpa_flex = 0;
  }
  if(m_range < (m_max_util_cpa_flex + 1)) {
    m_max_util_cpa_flex = m_range - 1;
    if(m_max_util_cpa_flex < 0)
      m_max_util_cpa_flex = 0;
  }

  // =======================================================
  // Part 3: Expand the buffer polygons
  // =======================================================
  XYPolyExpander expander;
  expander.setPoly(m_gut_poly);
  expander.setDegreeDelta(m_obuff_rdegs);
  expander.setVertexProximityThresh(1);
  
  XYPolygon new_mid_poly = expander.getBufferPoly(m_min_util_cpa_flex);
  XYPolygon new_rim_poly = expander.getBufferPoly(m_max_util_cpa_flex);

  // Error checking and noting possible failures
  if(!new_mid_poly.is_convex() || !new_rim_poly.is_convex()) {
    m_failed_expand_poly_str = "Bad Expand: " + m_gut_poly.get_spec();
    return(false);
  }
  if(!new_mid_poly.contains(m_gut_poly) ||
     !new_rim_poly.contains(m_gut_poly)) {
    m_failed_expand_poly_str = "Bad expand: " + m_gut_poly.get_spec();
  }

  string obs_label = m_gut_poly.get_label();
  m_mid_poly = new_mid_poly;
  m_mid_poly.set_label(obs_label + "_mid_poly");
  m_rim_poly = new_rim_poly;
  m_rim_poly.set_label(obs_label + "_rim_poly");

  //====================================================
  // Part 5: Update the other core dynamic variables:
  //         m_cx - obstacle center x position
  //         m_cy - obstacle center y position
  //         m_obcent_bng - angle from ownship to obst center
  //         m_obcent_relbng - rel bearing from ownship center
  //         m_cpa_in_osh - cpa of owship to obs in curr osh
  //         m_passing_side - port or star re
  //====================================================
  m_cx = m_gut_poly.get_center_x();
  m_cy = m_gut_poly.get_center_y();

  m_obcent_bng = relAng(osx, osy, m_cx, m_cy);
  m_obcent_relbng = relBearing(osx, osy, osh, m_cx, m_cy);

  m_passing_side = "n/a";
  if((m_obcent_relbng > 0) && (m_obcent_relbng < 180))
    m_passing_side = "star";
  if(m_obcent_relbng > 180)
    m_passing_side = "port";
  
  double ix, iy;
  m_cpa_in_osh = polyRayCPA(osx, osy, osh, m_gut_poly, ix, iy);
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: updateBngExtremes()
//   Purpose: Update bng extremes to gut_poly and rim_poly. 
//            A required calculation to support a refinery.
//      Note: For gut_poly, calc the min/max bngs inclusive
//            of which have a zero CPA to the gut_poly.
//            This will inform a refinery basin.
//            For rim_poly, calc the min/max bngs inclusive
//            of which have a NON-ZERO CPA to the rim_poly.
//            This will inform a refinery plateau.
//
//            The angles to the gut (actual/physical) poly:
//              m_gut_bng_min_to_poly
//              m_gut_bng_max_to_poly
//              m_gut_bng_min_dist_to_poly 
//              m_gut_bng_max_dist_to_poly
//           The angles to the rim (outermost) polygon:
//              m_rim_bng_min_to_poly
//              m_rim_bng_max_to_poly
//              m_rim_bng_min_dist_to_poly 
//              m_rim_bng_max_dist_to_poly
//====================================================

void ObShipModelV24::updateBngExtremes() 
{
  fillTurnCache(m_rim_poly);
  bool ok1 = setBngMinMaxToRimPoly();
  fillTurnCache(m_gut_poly);
  bool ok2 = setBngMinMaxToGutPoly();

  if(!ok1)
    cout << "bngMinMaxToRimPoly(): " << boolToString(ok1) << endl;
  if(!ok2)
    cout << "bngMinMaxToGutPoly(): " << boolToString(ok2) << endl;
  
  //cout << "jjjgut_bng_min_to_poly: " << m_gut_bng_min_to_poly << endl; 
  //cout << "jjjgut_bng_max_to_poly: " << m_gut_bng_max_to_poly << endl; 
  //cout << "jjjrim_bng_min_to_poly: " << m_rim_bng_min_to_poly << endl; 
  //cout << "jjjrim_bng_max_to_poly: " << m_rim_bng_max_to_poly << endl; 
}


// ----------------------------------------------------------
// Procedure: setBngMinMaxToGutPoly()
//   Purpose: Find the min/max bearings within which all bearings
//            intersect with the poly.
//      Note: A bearing interects if the ownship turn with the
//            desired heading of the bearing, results in a CPA
//            to the poly of zero. Since the platform model is
//            consulted, ownship current position, heading and
//            turn radius are all factored in to a Seglr which
//            is then used for calculating the CPA to the poly.

bool ObShipModelV24::setBngMinMaxToGutPoly()
{
  //===========================================================
  // Pt1: Determine cpa_dist and stem_dist from ownship on all
  //      possible desired hdg choices. cpa_dist is the closest
  //      point of approach of the seglr to gut poly. stem_cpa is
  //      the dist from ownship to the CPA point along the seglr.
  //      The latter enables gauging the time to CPA.
  //===========================================================

  // Tally the number of headings that collide or don't collide
  // with the poly. If either is zero, it's a special case.
  m_gut_bng_hit_count   = 0;
  m_gut_bng_unhit_count = 0;
  m_gut_bng_min_to_poly = -1;
  m_gut_bng_max_to_poly = -1;

  vector<double> gut_cpa_dist;
  vector<double> gut_stem_dist;

  double ix, iy, stem_dist;
  for(unsigned int ang=0; ang<360; ang++) {
    XYSeglr seglr = m_plat_model.getTurnSeglr((double)(ang));
    double cpa_dist = distSeglrToPoly(seglr, m_gut_poly,
				      ix, iy, stem_dist);

    gut_cpa_dist.push_back(cpa_dist);
    gut_stem_dist.push_back(stem_dist);
    if(cpa_dist <= 0)
      m_gut_bng_hit_count++;
    else
      m_gut_bng_unhit_count++;
  }

  //===========================================================
  // Step 3: Sanity check results and check for special cases
  //===========================================================
  // Sanity check intermediate results
  if((gut_cpa_dist.size() != 360) && (gut_stem_dist.size() != 360)) 
    return(false);
    
  if(m_gut_bng_hit_count == 0) // ownship cannot hit poly no matter hdg
    return(true);
  if(m_gut_bng_unhit_count == 0) // ownship does hit poly no matter hdg
    return(true);

  //===========================================================
  // Step 4: Find the min/max bearings that hit the poly
  //===========================================================
  double wrap = false;
  if((gut_cpa_dist[0] <= 0) && (gut_cpa_dist[359] <= 0))
    wrap = true;

  unsigned int bmin_ix = 0;
  unsigned int bmax_ix = 0;
  if(wrap) { // recall cpa_dist at bmin_ix and bmax_ix both should be > 0    
    for(unsigned int i=1; i<359; i++) {
      double ix = 359 - i;
      double cpa_dist = gut_cpa_dist[ix];
      if(cpa_dist > 0) {
	bmin_ix = ix;
	break;
      }
    }
    for(unsigned int i=1; i<359; i++) {
      double ix = i;
      double cpa_dist = gut_cpa_dist[ix];
      if(cpa_dist > 0) {
	bmax_ix = ix;
	break;
      }
    }
  }
  else { // recall cpa_dist at bmin_ix and bmax_ix both should be > 0    
    bool found_start = false;
    for(unsigned int i=0; i<359; i++) {
      double cpa_dist = gut_cpa_dist[i];
      if(cpa_dist <= 0) {
	if(!found_start) {
	  if(i == 0)
	    bmin_ix = 359;
	  else
	    bmin_ix = i-1;
	  found_start = true;
	}
      }
      if(found_start && (cpa_dist > 0)) {
	bmax_ix = i;
	break;
      }
    }
  }

  // Sanity checks
  if(bmin_ix >= gut_stem_dist.size())
    return(false);
  if(bmax_ix >= gut_stem_dist.size())
    return(false);
  
  //===========================================================
  // Step 5: Calc stem_dist vals along the min/max bng seglrs
  //===========================================================
  m_gut_bng_min_to_poly = (double)(bmin_ix);
  m_gut_bng_max_to_poly = (double)(bmax_ix);

  m_gut_bng_min_dist_to_poly = gut_stem_dist[bmin_ix];
  m_gut_bng_max_dist_to_poly = gut_stem_dist[bmax_ix];
  return(true);
}

// ----------------------------------------------------------
// Procedure: setBngMinMaxToRimPoly()
//   Purpose: Find the min/max bearings within which all bearings
//            DO NOT intersect with the poly.
//      Note: A bearing interects if the ownship turn with the
//            desired heading of the bearing, results in a CPA
//            to the poly of zero. Since the platform model is
//            consulted, ownship current position, heading and
//            turn radius are all factored in to a Seglr which
//            is then used for calculating the CPA to the poly.

bool ObShipModelV24::setBngMinMaxToRimPoly()
{
  //===========================================================
  // Pt1: Determine cpa_dist and stem_dist from ownship on all
  //      possible desired hdg choices. cpa_dist is the closest
  //      point of approach of the seglr to rim poly. stem_cpa is
  //      the dist from ownship to the CPA point along the seglr.
  //      The latter enables gauging the time to CPA.
  //===========================================================

  // Tally the number of headings that collide or don't collide
  // with the poly. If either is zero, it's a special case.
  m_rim_bng_hit_count   = 0;
  m_rim_bng_unhit_count = 0;
  m_rim_bng_min_to_poly = -1;
  m_rim_bng_max_to_poly = -1;
  
  vector<double> rim_cpa_dist;
  vector<double> rim_stem_dist;
  
  double ix, iy, stem_dist;
  for(unsigned int ang=0; ang<360; ang++) {
    XYSeglr seglr = m_plat_model.getTurnSeglr((double)(ang));
    double cpa_dist = distSeglrToPoly(seglr, m_rim_poly,
				      ix, iy, stem_dist);

    rim_cpa_dist.push_back(cpa_dist);
    rim_stem_dist.push_back(stem_dist);

    if(cpa_dist <= 0)
      m_rim_bng_hit_count++;
    else
      m_rim_bng_unhit_count++;
  }

  //===========================================================
  // Step 3: Sanity check results and check for special cases
  //===========================================================
  // Sanity check intermediate results
  if((rim_cpa_dist.size() != 360) && (rim_stem_dist.size() != 360)) 
    return(false);
  
  if(m_rim_bng_hit_count == 0) // ownship cannot hit poly no matter hdg
    return(true);
  if(m_rim_bng_unhit_count == 0) // ownship does hit poly no matter hdg
    return(true);

  //===========================================================
  // Step 4: Find the min/max bearings that do no hit the poly
  //===========================================================
  double wrap = false;
  if((rim_cpa_dist[0] > 0) && (rim_cpa_dist[359] > 0))
    wrap = true;

  unsigned int bmin_ix = 0;
  unsigned int bmax_ix = 0;
  if(wrap) { // recall cpa_dist at bmin_ix and bmax_ix both should be > 0    
    for(unsigned int i=1; i<359; i++) {
      double ix = 359 - i;
      double cpa_dist = rim_cpa_dist[ix];
      if(cpa_dist <= 0) {
	bmin_ix = ix + 1;
	break;
      }
    }
    for(unsigned int i=1; i<359; i++) {
      double ix = i;
      double cpa_dist = rim_cpa_dist[ix];
      if(cpa_dist <= 0) {
	bmax_ix = ix-1;
	break;
      }
    }
  }
  else { // recall cpa_dist at bmin_ix and bmax_ix both should be > 0    
    bool found_start = false;
    for(unsigned int i=0; i<359; i++) {
      double cpa_dist = rim_cpa_dist[i];
      if(cpa_dist > 0) {
	if(!found_start) {
	  bmin_ix = i;
	  found_start = true;
	}
      }
      if(found_start && (cpa_dist <= 0)) {
	bmax_ix = i-1;
	break;
      }
    }
  }

  // Sanity checks
  if(bmin_ix >= rim_cpa_dist.size())
    return(false);
  if(bmax_ix >= rim_cpa_dist.size())
    return(false);
  
  //===========================================================
  // Step 5: Calc stem_dist vals along the min/max bng seglrs
  //===========================================================
  m_rim_bng_min_to_poly = (double)(bmin_ix);
  m_rim_bng_max_to_poly = (double)(bmax_ix);

  m_rim_bng_min_dist_to_poly = rim_stem_dist[bmin_ix];
  m_rim_bng_max_dist_to_poly = rim_stem_dist[bmax_ix];
  return(true);
}

