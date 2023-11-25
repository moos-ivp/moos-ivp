/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ObShipModel.cpp                                      */
/*    DATE: Sep 6th, 2019                                        */
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
#include "ObShipModel.h"
#include "XYFormatUtilsPoly.h"
#include "XYPolyExpander.h"
#include "FileBuffer.h"
 
using namespace std;

// ----------------------------------------------------------
// Constructor

ObShipModel::ObShipModel()
{
  // Config variables
  m_osx = 0;
  m_osy = 0;
  m_osv = 0; 
  m_osh = 0;

  m_min_util_cpa = 8;
  m_max_util_cpa = 16;  
  m_min_util_cpa_flex = 8;
  m_max_util_cpa_flex = 16;  
  m_pwt_inner_dist = 10;
  m_pwt_outer_dist = 50;
  m_allowable_ttc  = 20;

  m_completed_dist = 50;
  
  // State variables
  m_cx = 0;
  m_cy = 0;

  m_obcent_bng = -1;
  m_obcent_relbng = -1;

  m_range = -1;
  m_range_in_osh = -1;
  m_cpa_in_osh = -2;

  m_osh180  = -1;
  m_theta_w = -1;
  m_theta_b = -1;

  m_bng_min_to_poly = 0;
  m_bng_max_to_poly = 0;
  m_cpa_bng_min_to_poly = 0;
  m_cpa_bng_max_to_poly = 0;
  m_bng_min_dist_to_poly = 0;
  m_bng_max_dist_to_poly = 0;
  m_cpa_bng_min_dist_to_poly = 0;
  m_cpa_bng_max_dist_to_poly = 0;
}


// ----------------------------------------------------------
// Procedure: setPose()

bool ObShipModel::setPose(double osx, double osy,
			  double osh, double osv)
{
  m_osx = osx;
  m_osy = osy;
  m_osh = angle360(osh); 

  if(osv >= 0)
    m_osv = osv;

  m_set_params.insert("osx");
  m_set_params.insert("osy");
  m_set_params.insert("osh");
  m_set_params.insert("osv");

  bool ok = updateDynamic();
  return(ok);
}


// ----------------------------------------------------------
// Procedure: setPoseOSX()

bool ObShipModel::setPoseOSX(double osx)
{
  m_osx = osx;
  m_set_params.insert("osx");

  bool ok = updateDynamic();
  return(ok);
}

// ----------------------------------------------------------
// Procedure: setPoseOSY()

bool ObShipModel::setPoseOSY(double osy)
{
  m_osy = osy;
  m_set_params.insert("osy");

  bool ok = updateDynamic();
  return(ok);
}

// ----------------------------------------------------------
// Procedure: setPoseOSH()

bool ObShipModel::setPoseOSH(double osh)
{
  m_osh = osh;
  m_set_params.insert("osh");

  bool ok = updateDynamic();
  return(ok);
}

// ----------------------------------------------------------
// Procedure: setPoseOSV()

bool ObShipModel::setPoseOSV(double osv)
{
  m_osv = osv;
  m_set_params.insert("osv");

  bool ok = updateDynamic();
  return(ok);
}

// ----------------------------------------------------------
// Procedure: setObstacle()

string ObShipModel::setObstacle(string polystr)
{
  XYPolygon new_obstacle = string2Poly(polystr);
  return(setObstacle(new_obstacle));
}

// ----------------------------------------------------------
// Procedure: setObstacle()

string ObShipModel::setObstacle(XYPolygon new_obstacle)
{
  if(!new_obstacle.is_convex())
    return("obstacle is not convex");

  m_obstacle = new_obstacle;
  
  m_set_params.insert("obstacle");

  // If this setting is just an adjustment, then updateDynamic()
  // is needed. It might also be an initial setting. So we ignore
  // the result of the update.
  updateDynamic();
  
  return("");
}

// ----------------------------------------------------------
// Procedure: setPwtInnerDist()

string ObShipModel::setPwtInnerDist(double val)
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
  return("");    
}

// ----------------------------------------------------------
// Procedure: setPwtOuterDist()

string ObShipModel::setPwtOuterDist(double val)
{
  if(val < 0)
    return("pwt_outer_dist must be a positive number");

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
// Procedure: setCompletedDist

string ObShipModel::setCompletedDist(double val)
{
  if(val < 0)
    return("completed_dist must be a positive number");
  
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

string ObShipModel::setMinUtilCPA(double val)
{
  if(val < 0)
    return("min_util_cpa cannot be a negative number");
  
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
  updateDynamic();

  return("");
}

// ----------------------------------------------------------
// Procedure: setMaxUtilCPA()

string ObShipModel::setMaxUtilCPA(double val)
{
  if(val < 0)
    return("max_util_cpa cannot be a negative number");
  
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
  updateDynamic();

  return("");
}

// ----------------------------------------------------------
// Procedure: setAllowableTTC()

string ObShipModel::setAllowableTTC(double val)
{
  if(val < 0)
    return("allowable_ttc cannot be a negative number");

  m_allowable_ttc = val;
  m_set_params.insert("allowable_ttc");

  // If this setting is just an adjustment, then updateDynamic()
  // is needed. It might also be an initial setting. So we ignore
  // the result of the update.
  updateDynamic();

  return("");
}

// ----------------------------------------------------------
// Procedure: paramIsSet()

bool ObShipModel::paramIsSet(string param) const
{
  if(m_set_params.count(param) == 0)
    return(false);
  return(true);
}

// ----------------------------------------------------------
// Procedure: ownshipInObstacle()

bool ObShipModel::ownshipInObstacle() const
{
  return(m_obstacle.contains(m_osx, m_osy));
}

// ----------------------------------------------------------
// Procedure: ownshipInObstacleBuffMin()

bool ObShipModel::ownshipInObstacleBuffMin() const
{
  return(m_obstacle_buff_min.contains(m_osx, m_osy));
}

// ----------------------------------------------------------
// Procedure: ownshipInObstacle()

bool ObShipModel::ownshipInObstacleBuffMax() const
{
  return(m_obstacle_buff_max.contains(m_osx, m_osy));
}

// ----------------------------------------------------------
// Procedure: osHdgInPlatMajor()
//      Note: Assumes that bearing angles have been set, want
//            to avoid a call to updateAngles() on this call.

bool ObShipModel::osHdgInPlatMajor(double osh) const
{
  osh = angle360(osh);
  
  // Case 1: the bearing range does not wrap-around
  if(m_cpa_bng_min_to_poly <= m_cpa_bng_max_to_poly) {
    if((osh < m_cpa_bng_min_to_poly) ||
       (osh > m_cpa_bng_max_to_poly))
      return(true);
    else
      return(false);
  }
  // Case 2: the bearing range DOES wrap-around
  else {
    if((osh < m_cpa_bng_min_to_poly) &&
       (osh > m_cpa_bng_max_to_poly))
      return(true);
    else
      return(false);
  }
}

// ----------------------------------------------------------
// Procedure: osHdgInBasinMajor()
//      Note: Assumes that bearing angles have been set, want
//            to avoid a call to updateAngles() on this call.

bool ObShipModel::osHdgInBasinMajor(double osh, bool verbose) const
{
  osh = angle360(osh);

  double bmin = 0;
  double bmax = 0;
  if(m_passing_side == "port") {
    bmin = m_theta_b;
    bmax = m_obcent_bng;
    if(verbose) {
      cout << "passing_side A:" << m_passing_side << endl;
      cout << "bmin:" << bmin << endl;
      cout << "bmax:" << bmax << endl;
    }  
  }
  else if(m_passing_side == "star") {
    bmin = m_obcent_bng;
    bmax = m_theta_b;
    if(verbose) {
      cout << "passing_side B:" << m_passing_side << endl;
      cout << "bmin:" << bmin << endl;
      cout << "bmax:" << bmax << endl;
    }  
  }
  else {
    if(verbose) {
      cout << "passing_side C:" << m_passing_side << endl;
      cout << "bmin:" << bmin << endl;
      cout << "bmax:" << bmax << endl;
    }      
    return(false);
  }
  // Case 1: the bearing range does not wrap-around
  if(bmin <= bmax) {
    if(verbose) {
      cout << "passing_side D:" << m_passing_side << endl;
      cout << "bmin:" << bmin << endl;
      cout << "bmax:" << bmax << endl;
    }  
    return((osh >= bmin) && (osh <= bmax));
  }
  // Case 2: the bearing range DOES wrap-around
  else {
    if(verbose) {
      cout << "passing_side E:" << m_passing_side << endl;
      cout << "bmin:" << bmin << endl;
      cout << "bmax:" << bmax << endl;
    }  
    return((osh >= bmin) || (osh <= bmax));
  }
}

// ----------------------------------------------------------
// Procedure: osSpdInPlatMinor()
//      Note: Assumes that bearing angles have been set, want
//            to avoid a call to updateAngles() on this call.

bool ObShipModel::osSpdInPlatMinor(double osv) const
{
  double bdist  = m_obstacle_buff_max.dist_to_point(m_osx, m_osy);
  double minspd = bdist / m_allowable_ttc;
  if(osv >= minspd)
    return(false);

  return(true);
}

// ----------------------------------------------------------
// Procedure: osHdgSpdInBasinMinor()
//      Note: Assumes that bearing angles have been set, want
//            to avoid a call to updateAngles() on this call.

bool ObShipModel::osHdgSpdInBasinMinor(double osh, double osv) const
{
  if(m_allowable_ttc <= 0)
    return(false);
  
  // Part 1: Check if is fast enough to be basin contender
  double dist1 = m_bng_min_dist_to_poly;
  double dist2 = m_bng_max_dist_to_poly;
  double range = dist1;
  if(dist2 > dist1)
    range = dist2;
  
  double smin = range / m_allowable_ttc;
  if(osv <= smin)
    return(false);
  
  // Part 2: Check if osh is in the basin heading range
  double bmin = m_bng_min_to_poly;
  double bmax = m_bng_max_to_poly;
  
  // Case 1: the bearing range does not wrap-around
  if(bmin <= bmax)
    return((osh >= bmin) && (osh <= bmax));
  // Case 2: the bearing range DOES wrap-around
  else
    return((osh >= bmin) || (osh <= bmax));
}

// ----------------------------------------------------------
// Procedure: getRangeRelevance()
//   Returns: A number in the range of [0,1]

double ObShipModel::getRangeRelevance() const
{
  // Part 1: Sanity checks
  if(m_pwt_outer_dist < m_pwt_inner_dist)
    return(0);

  // Part 2: Now the easy range cases: when the obstacle is outside 
  //         the min or max priority weight ranges
  if(m_range >= m_pwt_outer_dist)
    return(0);
  if(m_range <= m_pwt_inner_dist)
    return(1);

  // Part 4: Handle the in-between case
  double drange = (m_pwt_outer_dist - m_pwt_inner_dist);
  if(drange <= 0)
    return(0);
  double pct = (m_pwt_outer_dist - m_range) / drange;
  return(pct);
}

// ----------------------------------------------------------
// Procedure: isObstacleAft()
//      Note: The xbng parameter generalizes this function. Normally a
//            point is "aft" of ownship its relative bearing is in the
//            range (90,270). With the xbng parameter, "aft" can be
//            generalized, e.g., xbng=10 means the polygon must be at
//            least 10 degrees abaft of beam.


bool ObShipModel::isObstacleAft(double xbng) const
{
  return(polyAft(m_osx, m_osy, m_osh, m_obstacle, xbng));
}

// ----------------------------------------------------------
// Procedure: isValid()

bool ObShipModel::isValid() const
{
  if(m_pwt_outer_dist < m_pwt_inner_dist)
    return(false);
  if(m_completed_dist < m_pwt_outer_dist)
    return(false);
  if(m_max_util_cpa < m_min_util_cpa)
    return(false);
  if(!m_obstacle.is_convex())
    return(false);
  if(!m_obstacle_buff_min.is_convex())
    return(false);
  if(!m_obstacle_buff_max.is_convex())
    return(false);
  if(m_set_params.count("osx") == 0)
    return(false);
  if(m_set_params.count("osy") == 0)
    return(false);
  if(m_set_params.count("osh") == 0)
    return(false);

  return(true);
}


// ----------------------------------------------------------
// Procedure: getFailedExpandPolyStr()

string ObShipModel::getFailedExpandPolyStr(bool clear_val) 
{  
  string rval = m_failed_expand_poly_str;
  if(clear_val)
    m_failed_expand_poly_str = "";
  return(rval);
}

// ----------------------------------------------------------
// Procedure: print()

void ObShipModel::print(string key) const
{
  cout << "ObShipModel: (" << key << ")         " << endl;
  cout << "-------------------------------------" << endl;
  cout << "osx: " << m_osx << endl;
  cout << "osy: " << m_osy << endl;
  cout << "osh: " << m_osh << endl;
  cout << "osv: " << m_osv << endl;
  cout << "range: " << m_range << endl;
  cout << "min_util_cpa: " << m_min_util_cpa << endl;
  cout << "max_util_cpa: " << m_max_util_cpa << endl;
  cout << "min_util_cpa_flex: " << m_min_util_cpa_flex << endl;
  cout << "max_util_cpa_flex: " << m_max_util_cpa_flex << endl;
  cout << "pwt_inner_dist: " << m_pwt_inner_dist << endl;
  cout << "pwt_outer_dist: " << m_pwt_outer_dist << endl;
  cout << "m_allowable_ttc: " << m_allowable_ttc << endl;
  cout << "m_completed_dist: " << m_completed_dist << endl;
}

// ----------------------------------------------------------
// Procedure: updateDynamic

bool ObShipModel::updateDynamic() 
{
  if(!m_obstacle.is_convex())
    return(false);
  
  // =======================================================
  // Part 1: Update the range of ownship to the polygon
  // =======================================================
  m_range = 0;
  m_range_in_osh = 0; 
  if(!m_obstacle.contains(m_osx, m_osy)) {
    m_range = m_obstacle.dist_to_poly(m_osx, m_osy);
    m_range_in_osh = m_obstacle.dist_to_poly(m_osx, m_osy, m_osh);
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

  cout << "osx: " << m_osx << endl;
  cout << "osy: " << m_osy << endl;
  cout << "osh: " << m_osh << endl;
  cout << "osv: " << m_osv << endl;
  cout << "range: " << m_range << endl;
  cout << "min_util_cpa_flex: " << m_min_util_cpa_flex << endl;
  cout << "max_util_cpa_flex: " << m_max_util_cpa_flex << endl;
  
  // =======================================================
  // Part 3: Expand the buffer polygons
  // =======================================================
  XYPolyExpander expander;
  expander.setPoly(m_obstacle);
  expander.setDegreeDelta(10);
  expander.setVertexProximityThresh(1);
  
  XYPolygon new_poly_min = expander.getBufferPoly(m_min_util_cpa_flex);
  XYPolygon new_poly_max = expander.getBufferPoly(m_max_util_cpa_flex);

  // Error checking and noting possible failures
  if(!new_poly_min.is_convex() || !new_poly_max.is_convex()) {
    m_failed_expand_poly_str = "Failed Expand: " + m_obstacle.get_spec();
    return(false);
  }
  if(!new_poly_min.contains(m_obstacle) ||
     !new_poly_max.contains(m_obstacle)) {
    m_failed_expand_poly_str = "Failed expand: " + m_obstacle.get_spec();
  }

  string obs_label = m_obstacle.get_label();
  m_obstacle_buff_min = new_poly_min;
  m_obstacle_buff_min.set_label(obs_label + "_buff_min");
  m_obstacle_buff_max = new_poly_max;
  m_obstacle_buff_max.set_label(obs_label + "_buff_max");

  //====================================================
  // Part 4: Update the angles to the obstacle
  //         The angles to the actual obstacle:
  //            m_bng_min_to_poly
  //            m_bng_max_to_poly
  //            m_bng_min_dist_to_poly 
  //            m_bng_max_dist_to_poly
  //         The angles to the outer buffer obstacle:
  //            m_cpa_bng_min_to_poly
  //            m_cpa_bng_max_to_poly
  //            m_cpa_bng_min_dist_to_poly 
  //            m_cp_bng_max_dist_to_poly
  //====================================================
  double bmin = -1;   double bmin_dist = -1;
  double bmax = -1;   double bmax_dist = -1;

  bool ok1 = bearingMinMaxToPolyX(m_osx, m_osy, m_obstacle,
				  bmin, bmax, bmin_dist, bmax_dist);
  if(ok1) {
    m_bng_min_to_poly = bmin;
    m_bng_max_to_poly = bmax;
    m_bng_min_dist_to_poly = bmin_dist;
    m_bng_max_dist_to_poly = bmax_dist;
  }

  bool ok2 = bearingMinMaxToPolyX(m_osx, m_osy, m_obstacle_buff_max,
				  bmin, bmax, bmin_dist, bmax_dist);
  if(ok2) {
    m_cpa_bng_min_to_poly = bmin;
    m_cpa_bng_max_to_poly = bmax;
    m_cpa_bng_min_dist_to_poly = bmin_dist;
    m_cpa_bng_max_dist_to_poly = bmax_dist;
  }

  //====================================================
  // Part 5: Update the other core dynamice variables:
  //         m_cx - obstacle center x position
  //         m_cy - obstacle center y position
  //         m_obcent_bng - angle from ownship to obst center
  //         m_obcent_relbng - rel bearing from ownship center
  //         m_cpa_in_osh 
  //         m_os180 
  //         m_passing_side
  //         m_theta_w
  //         m_theta_b  
  //====================================================
  m_cx = m_obstacle.get_center_x();
  m_cy = m_obstacle.get_center_y();

  m_obcent_bng = relAng(m_osx, m_osy, m_cx, m_cy);
  m_obcent_relbng = relBearing(m_osx, m_osy, m_osh, m_cx, m_cy);

  m_passing_side = "n/a";
  if((m_obcent_relbng > 0) && (m_obcent_relbng < 180))
    m_passing_side = "star";
  if(m_obcent_relbng > 180)
    m_passing_side = "port";
  
  double ix, iy;
  m_cpa_in_osh = polyRayCPA(m_osx, m_osy, m_osh, m_obstacle, ix, iy);

  m_theta_w = -1;
  m_theta_b = -1;
  m_osh180  = angle360(m_osh + 180);

  if(m_passing_side == "port") {
    m_theta_w = angle360(m_obcent_bng - 90);
    m_theta_b = m_theta_w;
    if(!containsAngle(m_obcent_bng, m_theta_w, m_osh180))
      m_theta_b = m_osh180;    
  }
 
  if(m_passing_side == "star") {
    m_theta_w = angle360(m_obcent_bng + 90);
    m_theta_b = m_theta_w;
    if(!containsAngle(m_obcent_bng, m_theta_w, m_osh180))
      m_theta_b = m_osh180;    
  }
  return(true);
}
 

