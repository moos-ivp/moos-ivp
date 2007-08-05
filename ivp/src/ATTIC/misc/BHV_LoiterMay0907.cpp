/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Loiter.cpp                                       */
/*    DATE: July 26th 2005 In Elba w/ M.Grund, P.Newman          */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <math.h>
#include "AngleUtils.h"
#include "BHV_Loiter.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Loiter::BHV_Loiter(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_loiter");
  this->setParam("unifbox", "course=3, speed=2");
  this->setParam("gridbox", "course=9, speed=6");

  domain = subDomain(domain, "course,speed");

  current_waypt  = 0;
  arrival_radius = 10;     // meters
  desired_speed  = 0;      // meters/sec
  acquire_mode   = true;
  clockwise      = true;
  nm_radius      = 0;
  current_cpa    = 10000;
  hits           = 0;
  nm_hits        = 0;
  acquire_dist   = 10;
  iterations     = 0;
  poly_changed   = false;

  info_vars.push_back("NAV_X");
  info_vars.push_back("NAV_Y");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_Loiter::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParamCommon(g_param, g_val))
    return(true);

  if(g_param == "polygon") {
    int old_poly_size = poly.size();
    XYPolygon new_poly;
    bool ok = new_poly.initialize(g_val);
    if(!ok)  // Should be convex - false otherwise
      return(false);
    int new_poly_size = new_poly.size();
    if(!param_lock) {
      poly = new_poly;
      if(poly.is_clockwise() != clockwise)
	poly.reverse();
      poly.apply_snap(0.1); // snap to tenth of meter
      loiter_engine.setPoly(poly);
      acquire_mode  = true;
      
      if(new_poly_size != old_poly_size)
	current_waypt = 0;
    }
    poly_changed = true;
    return(true);
  }  
  else if(g_param == "clockwise") {
    g_val = tolower(g_val);
    if((g_val!="true")&&(g_val!="false"))
      return(false);
    if(!param_lock) {
      clockwise = true;
      if(g_val == "false")
	clockwise = false;
      if(poly.is_clockwise() != clockwise)
	poly.reverse();
      loiter_engine.setClockwise(clockwise);
    }
    return(true);
  }  
  else if(g_param == "speed") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock)
      desired_speed = dval;
    return(true);
  }
  else if(g_param == "radius") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock)
      arrival_radius = dval;
    return(true);
  }
  else if(g_param == "acquire_dist") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock)
      acquire_dist = dval;
    return(true);
  }
  else if(g_param == "nm_radius")  {
    double dval = atof(g_val.c_str());
    // val=0 is ok, interpreted as inactive
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock)
      nm_radius = dval;
    return(true);
  }

  return(false);
}

//-----------------------------------------------------------
// Procedure: produceOF

IvPFunction *BHV_Loiter::produceOF() 
{
  iterations++;
  // If poly is empty, not necessarily an era - could be on standby
  // for a dynamic specification.
  if(poly.size() == 0) {
    postWMessage("Empty/NULL Loiter Specification.");
    return(0);
  }

  bool ok1, ok2, ok3;
  // ownship position in meters from some 0,0 reference point.
  double osX   = info_buffer->dQuery("NAV_X", ok1);
  double osY   = info_buffer->dQuery("NAV_Y", ok2);
  double osHDG = info_buffer->dQuery("NAV_HEADING", ok3);

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2 || !ok3) {
    postEMessage("No ownship info in info_buffer.");
    return(0);
  }
  
  double dist_to_poly = poly.dist_to_poly(osX, osY);
  postMessage("DIST_TO_REGION", dist_to_poly);
  
  if(acquire_dist > 0) {
    if(dist_to_poly > acquire_dist)
      acquire_mode = true;
    else
      acquire_mode = false;
  }
  
  if(acquire_mode)
    current_waypt = loiter_engine.acquireVertex(osHDG, osX, osY); 
  
  if((current_waypt < 0) || 
     (current_waypt >= poly.size())) {
    postEMessage("Invalid waypoint determination " + 
    		 intToString(current_waypt));
    return(0);
  }

  double ptX  = poly.get_vx(current_waypt);
  double ptY  = poly.get_vy(current_waypt);
  double dist = hypot((osX-ptX), (osY-ptY));
  if(dist < current_cpa)
    current_cpa = dist;
  
  // Check to see if we have hit the current/prior waypoint. 
  bool next_point = false;
  
  if(dist <= arrival_radius) {
    next_point = true;
    hits++;
  }
  else {
    if((nm_radius > arrival_radius) &&
       (dist > current_cpa) &&
       (current_cpa <= nm_radius)) {
      next_point = true;
      nm_hits++;
    }
  }

  postMessage("LOITER_INDEX", current_waypt);
  if(acquire_mode)
    postMessage("LOITER_ACQUIRE", 1);
  else
    postMessage("LOITER_ACQUIRE", 0);
    

  // If so, update the waypoint, and declare that we are no
  // longer in "acquiring" mode if we were.
  if(next_point) {
    acquire_mode = false; 
    current_waypt++;
    if(current_waypt >= poly.size())
      current_waypt = 0;
    ptX = poly.get_vx(current_waypt);
    ptY = poly.get_vy(current_waypt);
    current_cpa = hypot((osX-ptX),(osY-ptY));
  }


  // SectionStart - build objective function
  IvPFunction *ipf = 0;

  ZAIC_PEAK spd_zaic(domain, "speed");
  spd_zaic.setSummit(desired_speed);
  spd_zaic.setBaseWidth(0.3);
  spd_zaic.setPeakWidth(0.0);
  spd_zaic.setSummitDelta(0.0);
  IvPFunction *spd_of = spd_zaic.extractOF();
  double rel_ang_to_wpt = relAng(osX,osY,ptX,ptY);
  ZAIC_PEAK crs_zaic(domain, "course");
  crs_zaic.setSummit(rel_ang_to_wpt);
  crs_zaic.setBaseWidth(180.0);
  crs_zaic.setValueWrap(true);
  IvPFunction *crs_of = crs_zaic.extractOF();
  OF_Coupler coupler;
  ipf = coupler.couple(crs_of, spd_of);
  // SectionEnd - build objective function

  if(ipf) {
    ipf->getPDMap()->normalize(0,100);
    ipf->setPWT(priority_wt);
  }

  string loiter_report = "Pt:" + intToString(current_waypt);
  loiter_report += " Dist:" + doubleToString(dist,2);
  loiter_report += " Hits:" + intToString(hits);
  loiter_report += " NM_Hits:" + intToString(nm_hits);
  loiter_report += " AQ_MODE:" + boolToString(acquire_mode);
  postMessage("LOITER_REPORT", loiter_report);

  if(iterations < 10)
    postMessage("VIEW_POLYGON", poly.get_spec());

  return(ipf);
}
