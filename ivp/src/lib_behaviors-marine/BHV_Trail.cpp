/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Trail.cpp                                        */
/*    DATE: Jul 3rd 2005 Sunday morning at Brueggers             */
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
#include <stdlib.h>
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

BHV_Trail::BHV_Trail(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)trail");
  this->setParam("build_info", "uniform_piece=course:3,speed:2");
  this->setParam("build_info", "uniform_grid=course:9,speed:6");
  
  m_domain = subDomain(m_domain, "course,speed");
  
  m_trail_range  = 0;
  m_trail_angle  = 180;
  m_radius       = 5;
  m_nm_radius    = 20;
  m_max_range    = 0;
  m_extrapolate  = true;
  
  m_decay_start = 5;
  m_decay_end   = 10;
  
  m_extrapolator.setDecay(m_decay_start, m_decay_end);
  
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

bool BHV_Trail::setParam(string g_param, string g_val) 
{
  g_val = stripBlankEnds(g_val);
  if(IvPBehavior::setParam(g_param, g_val))
    return(true);
  
  if((g_param == "them") || (g_param == "contact")) {
    m_contact = toupper(g_val);
    addInfoVars(m_contact+"_NAV_UTC");
    addInfoVars(m_contact+"_NAV_X");
    addInfoVars(m_contact+"_NAV_Y");
    addInfoVars(m_contact+"_NAV_SPEED");
    addInfoVars(m_contact+"_NAV_HEADING");
    return(true);
  }  
  else if (g_param == "extrapolate") 
    {
      string extrap = toupper(g_val);
      m_extrapolate = (extrap == "TRUE");
      return(true);
    }  
  else if(g_param == "trail_range") {
    if(isNumber(g_val)) {
      m_trail_range = atof(g_val.c_str());
      return(true);
    }  
  }
  else if(g_param == "trail_angle") {
    if(isNumber(g_val)) {
      m_trail_angle = angle180(atof(g_val.c_str()));
      return(true);
    }  
  }
  else if(g_param == "radius") {
    if(isNumber(g_val)) {
      m_radius = atof(g_val.c_str());
      return(true);
    }  
  }
  else if(g_param == "nm_radius") {
    if(isNumber(g_val)) {
      m_nm_radius = atof(g_val.c_str());
      return(true);
    }  
  }
  else if(g_param == "max_range") {
    if(isNumber(g_val)) {
      m_max_range = atof(g_val.c_str());
      return(true);
    }  
  }
  else if(g_param == "decay") {
    vector<string> svector = parseString(g_val, ',');
    if(svector.size() == 2) {
      svector[0] = stripBlankEnds(svector[0]);
      svector[1] = stripBlankEnds(svector[1]);
      if(isNumber(svector[0]) && isNumber(svector[1])) {
	double start = atof(svector[0].c_str());
	double end   = atof(svector[1].c_str());
	if((start >= 0) && (start <= end)) {
	  m_decay_start = start;
	  m_decay_end   = end;
	  m_extrapolator.setDecay(start,end);
	  return(true);
	}
      }
    }
  }  
  else if(g_param == "decay_end") {
    if(isNumber(g_val)) {
      m_decay_end = atof(g_val.c_str());
      
      return(true);
    }
  }  
  return(false);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_Trail::onRunState() 
{
  if(m_contact == "") {
    postWMessage("contact ID not set.");
    return(0);
  }
  
  // Set m_osx, m_osy, m_osh, m_osv, m_cnx, m_cny, m_cnh, m_cnv
  if(!updateInfoIn())
    return(0);
  
  // Calculate the trail point based on trail_angle, trail_range.
  double posX, posY; 
  // double adjusted_angle = angle180(m_cnh + m_trail_angle);
  // projectPoint(adjusted_angle, m_trail_range, m_cnx, m_cny, posX, posY);
  double abs_angle = headingToRadians(angle360(m_cnh+m_trail_angle));
  posX = m_cnx + m_trail_range*cos(abs_angle);
  posY = m_cny + m_trail_range*sin(abs_angle);
  
  postMessage("TRAIL_POINT_X", posX);
  postMessage("TRAIL_POINT_Y", posY);
  
  
  // Calculate the relevance first. If zero-relevance, we won't
  // bother to create the objective function.
  double relevance = getRelevance();
  
  m_cnh =angle360(m_cnh);  
  postMessage("TRAIL_CONTACT_X", m_cnx);
  postMessage("TRAIL_CONTACT_Y", m_cny);
  postMessage("TRAIL_CONTACT_SPEED", m_cnv);
  postMessage("TRAIL_CONTACT_HEADING", m_cnh);
  postMessage("TRAIL_RELEVANCE", relevance);
  
  
  if(relevance <= 0) {
    postMessage("PURSUIT", 0);
    return(0);
  }
  
  postMessage("PURSUIT", 1);
  
  IvPFunction *ipf = 0;
  
  double head_x = cos(headingToRadians(m_cnh));
  double head_y = sin(headingToRadians(m_cnh));
  double distance = distPointToPoint(m_osx, m_osy, posX, posY); 
  bool outside = (distance > m_radius);   
  
  postMessage("TRAIL_DISTANCE", distance);

  if(outside) {
    if(distance > m_nm_radius) {  // Outside nm_radius
      postMessage("REGION", "Outside nm_radius");
      
      AOF_CutRangeCPA aof(m_domain);
      aof.setParam("cnlat", posY);
      aof.setParam("cnlon", posX);
      aof.setParam("cncrs", m_cnh);
      aof.setParam("cnspd", m_cnv);
      aof.setParam("oslat", m_osy);
      aof.setParam("oslon", m_osx);
      aof.setParam("tol",   60);
      bool ok = aof.initialize();
      
      if(!ok) {
	postWMessage("Error in initializing AOF_CutRangeCPA.");
	return(0);
      }
      
      OF_Reflector reflector(&aof, 1);
      reflector.create(m_build_info);
      ipf = reflector.extractOF();
    }
    else { // inside nm_radius
      postMessage("REGION", "Inside nm_radius");
      
      double ahead_by = head_x*(m_osx-posX)+head_y*(m_osy-posY) ;
      //bool ahead = (ahead_by > 0);
      
      // head toward point nm_radius ahead of trail point
      double ppx = head_x*m_nm_radius+posX;
      double ppy = head_y*m_nm_radius+posY;
      double distp=hypot((ppx-m_osx), (ppy-m_osy));
      double bear_x = (head_x*m_nm_radius+posX-m_osx)/distp;
      double bear_y = (head_y*m_nm_radius+posY-m_osy)/distp;
      double modh = radToHeading(atan2(bear_y,bear_x));
      
      postMessage("TRAIL_HEADING", modh);
      
      ZAIC_PEAK hdg_zaic(m_domain, "course");
      
      // summit, pwidth, bwidth, delta, minutil, maxutil
      hdg_zaic.setParams(modh, 30, 150, 50, 0, 100);
      hdg_zaic.setValueWrap(true);
      
      IvPFunction *hdg_ipf = hdg_zaic.extractOF();
      
      // If ahead, reduce speed proportionally
      // if behind, increaase speed proportionally
      
      double modv = m_cnv * (1 - 0.5*ahead_by/m_nm_radius);
      
      if(modv < 0 || !m_extrapolate)
	modv = 0;
      
      postMessage("TRAIL_SPEED", modv);
      
      ZAIC_PEAK spd_zaic(m_domain, "speed");
      
      spd_zaic.setSummit(modv);
      spd_zaic.setPeakWidth(0.1);
      spd_zaic.setBaseWidth(2.0);
      spd_zaic.setSummitDelta(50.0); 
      
      // the following creates 0 desired speed. HS 032708
      //      spd_zaic.addSummit(modv, 0, 2.0, 10, 0, 25);
      //	  spd_zaic.setValueWrap(true);
      
      IvPFunction *spd_ipf = spd_zaic.extractOF();
      
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
    
    IvPFunction *hdg_ipf = hdg_zaic.extractOF();
    
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
    
    IvPFunction *spd_ipf = spd_zaic.extractOF();
    
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
// Procedure: onIdleState
//      Note: This function overrides the onIdleState() virtual
//            function defined for the IvPBehavior superclass
//            This function will be executed by the helm each
//            time the behavior FAILS to meet its run conditions.

void BHV_Trail::onIdleState()
{
  // Do your thing here
  postMessage("PURSUIT", 0);
}

//-----------------------------------------------------------
// Procedure: updateInfoIn()
//   Purpose: Update info need by the behavior from the info_buffer.
//            Error or warning messages can be posted.
//   Returns: true if no vital info is missing from the info_buffer.
//            false otherwise.
//      Note: By posting an EMessage, this sets the state_ok member
//            variable to false which will communicate the gravity
//            of the situation to the helm.

bool BHV_Trail::updateInfoIn()
{
  bool ok1, ok2, ok3, ok4, ok5, ok6, ok7, ok8, ok9;
  
  m_osx = getBufferDoubleVal("NAV_X", ok1);
  m_osy = getBufferDoubleVal("NAV_Y", ok2);
  m_osh = getBufferDoubleVal("NAV_HEADING", ok3);
  m_osv = getBufferDoubleVal("NAV_SPEED", ok4);

  // double old_cnx = m_cnx;
  // double old_cny = m_cny;
  // double old_cnutc =m_cnutc;

  m_cnx = getBufferDoubleVal(m_contact+"_NAV_X", ok5);
  m_cny = getBufferDoubleVal(m_contact+"_NAV_Y", ok6);

  m_cnh = getBufferDoubleVal(m_contact+"_NAV_HEADING", ok7);
  m_cnv = getBufferDoubleVal(m_contact+"_NAV_SPEED", ok8);
  m_cnutc = getBufferDoubleVal(m_contact+"_NAV_UTC", ok9);

  // hack hs
  // m_cnv = (sqrt(pow(m_cnx-old_cnx,2))+ pow(m_cny-old_cny,2))/(m_cnutc-old_cnutc);
  // m_cnh = radToHeading(atan2(m_cny-old_cny,m_cnx-old_cny));

  if(!ok1 || !ok2 || !ok3 || !ok4 || !ok5 || 
     !ok6 || !ok7 || !ok8 || !ok9)
    return(false);
  
  double curr_time = getBufferCurrTime();
  // double mark_time = getBufferTimeVal(m_contact+"_NAV_X");
  
  postMessage("TRAIL_CONTACT_TIME", m_cnutc);
  postMessage("TRAIL_CURRENT_TIME", curr_time);
  postMessage("TRAIL_DELTA_TIME", curr_time-m_cnutc);
  
  if(!m_extrapolate)
    return(true);
  
  // if(mark_time == 0)
  m_extrapolator.setPosition(m_cnx, m_cny, m_cnv, m_cnh, m_cnutc);
  
  // Even if mark_time is zero and thus "fresh", still derive the 
  // the contact position from the extrapolator since the UTC time
  // of the position in this report may still be substantially 
  // behind the current own-platform UTC time.
  
  double new_cnx, new_cny;
  bool ok = m_extrapolator.getPosition(new_cnx, new_cny, curr_time);
  
  if(ok) {
    m_cnx = new_cnx;
    m_cny = new_cny;
    return(true);
  }
  else {
    postWMessage("Incomplete Linear Extrapolation");
    return(false);
  }
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
  
  double contact_range = hypot((m_osx-m_cnx), (m_osy-m_cny));
  postMessage("TRAIL_RANGE",contact_range );
  
  if(contact_range < m_max_range)
    return(1.0);
  else
    return(0.0);
}









