/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_Trail.cpp                                        */
/*    DATE: Jul 3rd 2005 Sunday morning at Brueggers             */
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
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <cmath>
#include <cstdlib>
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "AOF_CutRangeCPA.h"
#include "BHV_Trail.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "MBUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Trail::BHV_Trail(IvPDomain gdomain) : 
  IvPContactBehavior(gdomain)
{
  this->setParam("descriptor", "(d)trail");
  this->setParam("build_info", "uniform_piece=discrete@course:3,speed:2");
  this->setParam("build_info", "uniform_grid =discrete@course:9,speed:6");
  
  // These parameters really should be set in the behavior file, but are
  // left here for now to smoothen the transition (Aug 10, 2008, mikerb)
  //this->setParam("activeflag",   "PURSUIT=1");
  //this->setParam("inactiveflag", "PURSUIT=0");
  
  m_domain = subDomain(m_domain, "course,speed");
  
  m_trail_range    = 50;
  m_trail_angle    = 180;
  m_radius         = 5;
  m_nm_radius      = 20;
  m_max_range      = 0;
  m_angle_relative = true; // as opposed to angle being absolute
  m_time_on_leg    = 60;
  m_post_trail_distance_on_idle = true;
  m_trail_pt_x     = 0;
  m_trail_pt_y     = 0;

  m_no_alert_request  = false;
  
  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam
//  
//        "them": the name of the vehicle to trail.
// "trail_range": desired range to the vehicle trailed.
// "trail_angle": desired angle to the vehicle trailed.
//      "radius": distance to the desired trailing point within
//                which the behavior is "shadowing".
//   "nm_radius": If within this and heading ahead of target slow down
//   "max_range": contact range outside which priority is zero.

bool BHV_Trail::setParam(string param, string param_val) 
{
  if(IvPContactBehavior::setParam(param, param_val))
    return(true);
  
  double dval = atof(param_val.c_str());
  bool non_neg_number = (isNumber(param_val) && (dval >= 0));

  if(param == "nm_radius") {
    if(non_neg_number) {
      m_nm_radius = dval;
      return(true);
    }  
  }
  else if(param == "no_alert_request") {
    return(setBooleanOnString(m_no_alert_request, param_val));
  }  
  else if(param == "post_trail_dist_on_idle") {
    return(setBooleanOnString(m_post_trail_distance_on_idle, param_val));
  }

  else if(param == "post_trail_distance_on_idle") {
    return(setBooleanOnString(m_post_trail_distance_on_idle, param_val));
  }
  else if((param == "pwt_outer_dist") ||   // preferred
	  (param == "max_range")) {        // deprecated
    if(non_neg_number) {
      m_max_range = dval;
      return(true);
    }  
  }
  else if(param == "radius") {
    if(non_neg_number) {
      m_radius = dval;
      return(true);
    }  
  }
  else if(param == "trail_angle") {
    if(isNumber(param_val)) {
      m_trail_angle = angle180(dval);
      return(true);
    }  
  }
  else if(param == "trail_angle_type") {
    param_val = tolower(param_val);
    if(param_val == "absolute")
      m_angle_relative = false;
    else if(param_val == "relative")
      m_angle_relative = true;
    else
      return(false);
    return(true);
  }

  else if(param == "trail_range") {
    if(non_neg_number) {
      m_trail_range = dval;
      return(true);
    }  
  }

  return(false);
}


//-----------------------------------------------------------
// Procedure: onSetParamComplete

void BHV_Trail::onSetParamComplete() 
{
  m_trail_point.set_label(m_us_name + "_trailpoint");
  m_trail_point.set_active("false");
  string bhv_tag = tolower(getDescriptor());
}


//-----------------------------------------------------------
// Procedure: onHelmStart()

void BHV_Trail::onHelmStart() 
{
#if 0
  if(m_no_alert_request || (m_update_var == ""))
    return;
  
  string s_alert_range = doubleToStringX(m_pwt_outer_dist,1);
  string s_cpa_range   = doubleToStringX(m_completed_dist,1);
  string s_alert_templ = "name=avd_$[VNAME] # contact=$[VNAME]";

  string alert_request = "id=avd, var=" + m_update_var;
  alert_request += ", val=" + s_alert_templ;
  alert_request += ", alert_range=" + s_alert_range;
  alert_request += ", cpa_range=" + s_cpa_range;

  postMessage("BCM_ALERT_REQUEST", alert_request);
#endif
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_Trail::onRunState() 
{
  if(!updatePlatformInfo())
    return(0);
    
  // Added Aug11,2008 on GLINT to handle sync AUV multistatic - mikerb
  if(m_extrapolate && m_extrapolator.isDecayMaxed())
    return(0);

  calculateTrailPoint();
  postViewableTrailPoint();

  // double adjusted_angle = angle180(m_cnh + m_trail_angle);
  // projectPoint(adjusted_angle, m_trail_range, m_cnx, m_cny, m_trail_pt_x, m_trail_pt_y);

  // Calculate the relevance first. If zero-relevance, we won't
  // bother to create the objective function.
  double relevance = getRelevance();

  postRepeatableMessage("TRAIL_RELEVANCE", relevance);		       
  
  m_cnh =angle360(m_cnh);  

  postRepeatableMessage("TRAIL_CTR", 1);

  if(relevance <= 0) {
    postMessage("PURSUIT", 0);
    return(0);
  }
  
  postMessage("PURSUIT", 1);
  
  IvPFunction *ipf = 0;
  double head_x = cos(headingToRadians(m_cnh));
  double head_y = sin(headingToRadians(m_cnh));
  
  double distance = updateTrailDistance();
  bool   outside = (distance > m_radius);   
 
  if(outside) {
    if(distance > m_nm_radius) {  // Outside nm_radius
      postMessage("REGION", "Outside nm_radius");
      
      AOF_CutRangeCPA aof(m_domain);
      aof.setParam("cnlat", m_trail_pt_y);
      aof.setParam("cnlon", m_trail_pt_x);
      aof.setParam("cncrs", m_cnh);
      aof.setParam("cnspd", m_cnv);
      aof.setParam("oslat", m_osy);
      aof.setParam("oslon", m_osx);
      aof.setParam("tol",   m_time_on_leg);
      bool ok = aof.initialize();
      
      if(!ok) {
	postWMessage("Error in initializing AOF_CutRangeCPA.");
	return(0);
      }
      
      OF_Reflector reflector(&aof);
      reflector.create(m_build_info);
      if(!reflector.stateOK())
	postWMessage(reflector.getWarnings());
      else
	ipf = reflector.extractIvPFunction();
    }
    else { // inside nm_radius
      postMessage("REGION", "Inside nm_radius");
      
      double ahead_by = head_x*(m_osx-m_trail_pt_x)+head_y*(m_osy-m_trail_pt_y) ;
      //bool ahead = (ahead_by > 0);
      
      // head toward point nm_radius ahead of trail point
      double ppx = head_x*m_nm_radius+m_trail_pt_x;
      double ppy = head_y*m_nm_radius+m_trail_pt_y;
      double distp=hypot((ppx-m_osx), (ppy-m_osy));
      double bear_x = (head_x*m_nm_radius+m_trail_pt_x-m_osx)/distp;
      double bear_y = (head_y*m_nm_radius+m_trail_pt_y-m_osy)/distp;
      double modh = radToHeading(atan2(bear_y,bear_x));
      
      postIntMessage("TRAIL_HEADING", modh);
      
      ZAIC_PEAK hdg_zaic(m_domain, "course");
      
      // summit, pwidth, bwidth, delta, minutil, maxutil
      hdg_zaic.setParams(modh, 30, 150, 50, 0, 100);
      hdg_zaic.setValueWrap(true);
      
      IvPFunction *hdg_ipf = hdg_zaic.extractIvPFunction();
      
      // If ahead, reduce speed proportionally
      // if behind, increaase speed proportionally
      
      double modv = m_cnv * (1 - 0.5*ahead_by/m_nm_radius);
      
      if(modv < 0 || !m_extrapolate)
	modv = 0;
      
      // snap to one decimal precision to reduce excess postings.
      double snapped_modv = snapToStep(modv, 0.1);
      postMessage("TRAIL_SPEED", snapped_modv);
      
      ZAIC_PEAK spd_zaic(m_domain, "speed");
      
      spd_zaic.setSummit(modv);
      spd_zaic.setPeakWidth(0.1);
      spd_zaic.setBaseWidth(2.0);
      spd_zaic.setSummitDelta(50.0); 
      
      // the following creates 0 desired speed. HS 032708
      //      spd_zaic.addSummit(modv, 0, 2.0, 10, 0, 25);
      //	  spd_zaic.setValueWrap(true);
      
      IvPFunction *spd_ipf = spd_zaic.extractIvPFunction();
      
      OF_Coupler coupler;
      ipf = coupler.couple(hdg_ipf, spd_ipf);
    }
  }
  else {
    postMessage("REGION", "Inside radius");
    ZAIC_PEAK hdg_zaic(m_domain, "course");
    
    // summit, pwidth, bwidth, delta, minutil, maxutil
    hdg_zaic.setParams(m_cnh, 30, 150, 50, 0, 100);
    hdg_zaic.setValueWrap(true);
    
    IvPFunction *hdg_ipf = hdg_zaic.extractIvPFunction();
    
    ZAIC_PEAK spd_zaic(m_domain, "speed");
    
    // If inside radius and ahead, reduce speed a little
    double modv=m_cnv;
    //      if (ahead)
    //	modv = m_cnv - 0.1;
    
    if(modv < 0 || !m_extrapolate)
      modv = 0;
    
    postMessage("TRAIL_SPEED", modv);
    
    // summit, pwidth, bwidth, delta, minutil, maxutil
    spd_zaic.setParams(modv, 0.1, 2.0, 50, 0, 100);
    
    // the following creates 0 desired speed. HS 032708
    //      spd_zaic.addSummit(modv, 0, 2.0, 10, 0, 25);
    //      spd_zaic.setValueWrap(true);
    
    IvPFunction *spd_ipf = spd_zaic.extractIvPFunction();
    
    OF_Coupler coupler;
    ipf = coupler.couple(hdg_ipf, spd_ipf);
  }
  
  if(ipf) {
    ipf->getPDMap()->normalize(0.0, 100.0);
    ipf->setPWT(relevance * m_priority_wt);
  }
  
  return(ipf);
}

//-----------------------------------------------------------
// Procedure: onRunToIdleState

void BHV_Trail::onRunToIdleState()
{
  postMessage("PURSUIT", 0);
  postErasableTrailPoint();
}

//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_Trail::onIdleState()
{
  updatePlatformInfo();
  calculateTrailPoint();
  if(m_post_trail_distance_on_idle)
    updateTrailDistance();
}

//-----------------------------------------------------------
// Procedure: getRelevance

double BHV_Trail::getRelevance()
{
  // For now just return 1.0 if within max_range. But we could 
  // imagine that we would reduce its relevance (linearly perhaps) 
  // if the vehicle were already in a good position.
  
  if(m_max_range == 0)
    return(1.0);
  
  postIntMessage("TRAIL_RANGE", m_contact_range );
  postIntMessage("MAX_RANGE", m_max_range );
  
  if(m_contact_range < m_max_range)
    return(1.0);
  else
    return(0.0);
}

//-----------------------------------------------------------
// Procedure: postViewableTrailPoint

void BHV_Trail::postViewableTrailPoint()
{
  m_trail_point.set_active(true);
  string spec = m_trail_point.get_spec();
  postMessage("VIEW_POINT", spec);
}


//-----------------------------------------------------------
// Procedure: postErasableTrailPoint

void BHV_Trail::postErasableTrailPoint()
{
  m_trail_point.set_active(false);
  string spec = m_trail_point.get_spec();
  postMessage("VIEW_POINT", spec);
}

double  BHV_Trail::updateTrailDistance()
{
  double distance = distPointToPoint(m_osx, m_osy, m_trail_pt_x, m_trail_pt_y); 
  postIntMessage("TRAIL_DISTANCE", distance);
  return distance;
}

//-----------------------------------------------------------
// Procedure: calculateTrailPoint

void BHV_Trail::calculateTrailPoint()
{
  // Calculate the trail point based on trail_angle, trail_range.
  //  double m_trail_pt_x, m_trail_pt_y; 
  
  if(m_angle_relative) {
    double abs_angle = headingToRadians(angle360(m_cnh+m_trail_angle));
    m_trail_pt_x = m_cnx + m_trail_range*cos(abs_angle);
    m_trail_pt_y = m_cny + m_trail_range*sin(abs_angle);
  }
  else 
    projectPoint(m_trail_angle, m_trail_range, m_cnx, m_cny, m_trail_pt_x, m_trail_pt_y);

  m_trail_point.set_vertex(m_trail_pt_x, m_trail_pt_y);
}







