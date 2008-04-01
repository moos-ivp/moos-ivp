/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_HTrail.cpp                                        */
/*    DATE: Jul 3rd 2005 Sunday morning at Brueggers             */
/*    MOD to CCL and UTC extrapolation:  Henrik Schmidt                                */
/*    DATE: Mar 16 2008 Monday afternoon at NURC                 */
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
#include "GeomUtils.h"
#include "AOF_CutRangeCPA.h"
#include "BHV_HTrail.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "MBUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_HTrail::BHV_HTrail(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)trail");
  this->setParam("build_info", "uniform_piece=course:3,speed:2");
  this->setParam("build_info", "uniform_grid=course:9,speed:6");

  m_domain = subDomain(m_domain, "course,speed");

  m_trail_range  = 0;
  m_trail_angle  = 180;
  m_radius       = 5;
  m_max_range    = 0;
  m_interpolate  = true;
  m_obsolete = 120;
  m_speed_delta = 0;

  m_min_util_cpa_dist = 100;
  m_max_util_cpa_dist = 0; 

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING, DB_TIME");
}

//-----------------------------------------------------------
// Procedure: setParam
//  
//        "them": the name of the vehicle to trail.
// "trail_range": desired range to the vehicle trailed.
// "trail_angle": desired angle to the vehicle trailed.
//      "radius": distance to the desired trailing point within
//                which the behavior is "shadowing".
//   "max_range": contact range outside which priority is zero.

bool BHV_HTrail::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParam(g_param, g_val))
    return(true);

  if((g_param == "them") || (g_param == "contact")) 
    {
      m_contact = toupper(g_val);
      addInfoVars(m_contact+"_NAV_X");
      addInfoVars(m_contact+"_NAV_Y");
      addInfoVars(m_contact+"_NAV_SPEED");
      addInfoVars(m_contact+"_NAV_HEADING");
      addInfoVars(m_contact+"_NAV_TIME");
      return(true);
    }  
  else if(g_param == "trail_range") 
    {
      m_trail_range = atof(g_val.c_str());
      return(true);
    }  
  else if(g_param == "trail_angle") 
    {
      m_trail_angle = angle180(atof(g_val.c_str()));
      return(true);
    }  
  else if(g_param == "radius") 
    {
      m_radius = atof(g_val.c_str());
      return(true);
    }  
  else if(g_param == "max_range") 
    {
      m_max_range = atof(g_val.c_str());
      return(true);
    }  
  else if (g_param == "obsolete")
    {
      m_obsolete = atof(g_val.c_str());
      return(true);
    }
  else if (g_param == "speed_delta")
    {
      m_speed_delta = atof(g_val.c_str());
      return(true);
    }
 
  return(false);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_HTrail::onRunState() 
{
  if(m_contact == "") {
    postWMessage("contact ID not set.");
    return(0);
  }

  // Set m_osx, m_osy, m_osh, m_osv, m_cnx, m_cny, m_cnh, m_cnv
  if(!updateInfoIn())
    return(0);

  // Calculate the trail point based on trail_angle, trail_range.
  //double posX, posY; 
  //double adjusted_angle = angle360(m_cnh + m_trail_angle);
  //projectPoint(adjusted_angle, m_trail_range, m_cnx, m_cny, posX, posY);

  // Changed by HS. heading/degree problem 

  double adjusted_angle = headingToRadians(angle360(m_cnh + m_trail_angle));
  double posX = m_cnx + m_trail_range*cos(adjusted_angle);
  double posY = m_cny + m_trail_range*sin(adjusted_angle);


  // Calculate the relevance first. If zero-relevance, we won't
  // bother to create the objective function.
   //  double relevance = getRelevance();
  double relevance = 0;
  if (((curr_time - m_cnt) < m_obsolete) || (m_obsolete == 0))
    {
      relevance = getRelevance();
    }
 
  postMessage("TRAIL_CONTACT_TIME", m_cnt);
  postMessage("TRAIL_CURRENT_TIME", curr_time);
  postMessage("TRAIL_DELTA_TIME", curr_time-m_cnt);
  
  postMessage("TRAIL_CONTACT_X", m_cnx);
  postMessage("TRAIL_CONTACT_Y", m_cny);
  //  postMessage("TRAIL_CONTACT_MSG_X", contact_x);
  //  postMessage("TRAIL_CONTACT_MSG_Y", contact_y);
  postMessage("TRAIL_CONTACT_SPEED", m_cnv);
  postMessage("TRAIL_CONTACT_HEADING", m_cnh);
  postMessage("TRAIL_RELEVANCE", relevance);
  
  if(relevance <= 0) {
    postMessage("PURSUIT", 0);
    return(0);
  }

  postMessage("PURSUIT", 1);

  IvPFunction *ipf = 0;

  if(distPointToPoint(m_osx, m_osy, posX, posY) > m_radius) {
    AOF_CutRangeCPA aof(m_domain);
    aof.setParam("cnlat", posY);
    aof.setParam("cnlon", posX);
    aof.setParam("cncrs", m_cnh);
    aof.setParam("cnspd", m_cnv);
    aof.setParam("oslat", m_osy);
    aof.setParam("oslon", m_osx);
    aof.setParam("tol",   60);
    aof.setParam("min_util_cpa_dist", m_min_util_cpa_dist);
    aof.setParam("max_util_cpa_dist", m_max_util_cpa_dist);
    bool ok = aof.initialize();

    if(!ok) {
      postWMessage("Error in initializing AOF_CutRangeCPA.");
      return(0);
    }

    OF_Reflector reflector(&aof, 1);
    reflector.create(m_build_info);
    ipf = reflector.extractOF();
  }
  else {
    ZAIC_PEAK hdg_zaic(m_domain, "course");
    hdg_zaic.setParams(m_cnh, 0, 180, 80, 0, 100);
    hdg_zaic.setValueWrap(true);
    IvPFunction *hdg_ipf = hdg_zaic.extractOF();
    
    ZAIC_PEAK spd_zaic(m_domain, "speed");
    //    double modv = m_cnv - 0.1;
    //   if(modv < 0)
    //    modv = 0;

    double modv =m_cnv;
    // If inside radius and ahead, reduce speed
    double head_x = cos(headingToRadians(contact_heading));
    double head_y = sin(headingToRadians(contact_heading));
    bool ahead = (head_x*(m_osx-posX)+head_y*(m_osy-posY) > 0.0);
    if (ahead && (m_cnv > m_speed_delta))
	modv -= m_speed_delta;

    spd_zaic.setParams(modv, 0, 2.0, 10, 0, 25);
    spd_zaic.setValueWrap(false);
    IvPFunction *spd_ipf = spd_zaic.extractOF();
    
    OF_Coupler coupler;
    ipf = coupler.couple(hdg_ipf, spd_ipf);
  }
  
  if(ipf) {
    ipf->getPDMap()->normalize(0.0, 100.0);
    ipf->setPWT(relevance * m_priority_wt);
  }

#if 0
    postMessage("TRAIL_MIN_WT", ipf->getPDMap()->getMinWT());
    postMessage("TRAIL_MAX_WT", ipf->getPDMap()->getMaxWT());
#endif  

  return(ipf);
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

bool BHV_HTrail::updateInfoIn()
{
  bool ok, ok1, ok2, ok3, ok4, ok5, ok6, ok7, ok8, ok9;
 
  m_osx = getBufferDoubleVal("NAV_X", ok1);
  m_osy = getBufferDoubleVal("NAV_Y", ok2);
  m_osh = getBufferDoubleVal("NAV_HEADING", ok3);
  m_osv = getBufferDoubleVal("NAV_SPEED", ok4);
  curr_time = getBufferDoubleVal("DB_TIME", ok);
  
  m_cnx = getBufferDoubleVal(m_contact+"_NAV_X", ok5);
  m_cny = getBufferDoubleVal(m_contact+"_NAV_Y", ok6);
  m_cnh = getBufferDoubleVal(m_contact+"_NAV_HEADING", ok7);
  m_cnv = getBufferDoubleVal(m_contact+"_NAV_SPEED", ok8);
  m_cnt = getBufferDoubleVal(m_contact+"_NAV_TIME", ok9);

  if(!ok1)
    postWMessage("No ownship NAV_X in info_buffer.");
  if(!ok2)
    postWMessage("No ownship NAV_Y in info_buffer.");
  if(!ok3)
    postWMessage("No ownship NAV_HEADING in info_buffer.");
  if(!ok4)
    postWMessage("No ownship NAV_SPEED in info_buffer.");
  if(!ok5)
    postWMessage("No contact NAV_X in info_buffer.");
  if(!ok6)
    postWMessage("No contact NAV_Y in info_buffer.");
  if(!ok7)
    postWMessage("No contact NAV_HEADING in info_buffer.");
  if(!ok8)
    postWMessage("No contact NAV_SPEED in info_buffer.");
  if(!ok9)
    postWMessage("No contact NAV_TIME in info_buffer.");

  if(!ok1 || !ok2 || !ok3 || !ok4 || !ok5 || !ok6 || !ok7 || !ok8 || !ok9)
    return(false);

  if(!m_interpolate)
    return(true);

  // Henrik's extrapolation
  double dist_extrap = m_cnv * (curr_time - m_cnt);
  double head_x = dist_extrap*cos(headingToRadians(m_cnh));
  double head_y = dist_extrap*sin(headingToRadians(m_cnh));
  m_cnx   += head_x;
  m_cny   += head_y;

  //  double curr_time = getBufferCurrTime();
  // double mark_time = getBufferTimeVal(m_contact+"_NAV_X");
  // double mark_time = cnt;
  //  if(mark_time == curr_time)
  //    m_extrapolator.setPosition(m_cnx, m_cny, m_cnv, m_cnh, curr_time);
    
  //double new_cnx, new_cny;
  //bool ok = m_interpolator.getPosition(new_cnx, new_cny, curr_time);
  //if(ok) {
  //  m_cnx = new_cnx;
  //  m_cny = new_cny;
  //}

  return(true);
}



//-----------------------------------------------------------
// Procedure: getRelevance

double BHV_HTrail::getRelevance()
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









