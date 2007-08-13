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
  this->setParam("unifbox", "course=3, speed=2");
  this->setParam("gridbox", "course=9, speed=6");

  domain = subDomain(domain, "course,speed");

  m_trail_range  = 0;
  m_trail_angle  = 180;
  m_radius       = 5;
  m_max_range    = 0;

  m_min_util_cpa_dist = 100;
  m_max_util_cpa_dist = 0; 

  info_vars.push_back("NAV_X");
  info_vars.push_back("NAV_Y");
  info_vars.push_back("NAV_SPEED");
  info_vars.push_back("NAV_HEADING");
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

bool BHV_Trail::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParamCommon(g_param, g_val))
    return(true);

  if((g_param == "them") || (g_param == "contact")) {
    if(!param_lock) {
      m_contact = toupper(g_val);
      info_vars.push_back(m_contact+"_NAV_X");
      info_vars.push_back(m_contact+"_NAV_Y");
      info_vars.push_back(m_contact+"_NAV_SPEED");
      info_vars.push_back(m_contact+"_NAV_HEADING");
    }
    return(true);
  }  
  else if(g_param == "trail_range") {
    if(!param_lock)
      m_trail_range = atof(g_val.c_str());
    return(true);
  }  
  else if(g_param == "trail_angle") {
    if(!param_lock)
      m_trail_angle = angle180(atof(g_val.c_str()));
    return(true);
  }  
  else if(g_param == "radius") {
    if(!param_lock)
      m_radius = atof(g_val.c_str());
    return(true);
  }  
  else if(g_param == "max_range") {
    if(!param_lock)
      m_max_range = atof(g_val.c_str());
    return(true);
  }  
  return(false);
}


//-----------------------------------------------------------
// Procedure: produceOF

IvPFunction *BHV_Trail::produceOF() 
{
  if(!unif_box || !grid_box) {
    postEMessage("Null UnifBox or GridBox.");
    return(0);
  }
  
  if(m_contact == "") {
    postWMessage("contact ID not set.");
    return(0);
  }

  // Set m_osx, m_osy, m_osh, m_osv, m_cnx, m_cny, m_cnh, m_cnv
  if(!updateInfoIn())
    return(0);

  // Calculate the trail point based on trail_angle, trail_range.
  double posX, posY; 
  double adjusted_angle = m_cnh + m_trail_angle;
  projectPoint(adjusted_angle, m_trail_range, m_cnx, m_cny, posX, posY);

  // Calculate the relevance first. If zero-relevance, we won't
  // bother to create the objective function.
  double relevance = getRelevance();
  if(relevance <= 0)
    return(0);

  postMessage("PURSUIT", "true");

  IvPFunction *ipf = 0;

  if(distPointToPoint(m_osx, m_osy, posX, posY) > m_radius) {
    AOF_CutRangeCPA aof(domain);
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
    reflector.createUniform(unif_box, grid_box);
    ipf = reflector.extractOF();
  }
  else {
    ZAIC_PEAK hdg_zaic(domain, "course");
    hdg_zaic.addSummit(m_cnh, 0, 180, 80, 0, 100);
    hdg_zaic.setValueWrap(true);
    IvPFunction *hdg_ipf = hdg_zaic.extractOF();
    
    ZAIC_PEAK spd_zaic(domain, "speed");
    double modv = m_cnv - 0.1;
    if(modv < 0)
      modv = 0;
    spd_zaic.addSummit(modv, 0, 2.0, 10, 0, 25);
    spd_zaic.setValueWrap(true);
    IvPFunction *spd_ipf = spd_zaic.extractOF();
    
    OF_Coupler coupler;
    ipf = coupler.couple(hdg_ipf, spd_ipf);
  }
  
  if(ipf) {
    ipf->getPDMap()->normalize(0.0, 100.0);
    ipf->setPWT(relevance * priority_wt);
  }

  if(!silent) {
    postMessage("TRAIL_MIN_WT", ipf->getPDMap()->getMinWT());
    postMessage("TRAIL_MAX_WT", ipf->getPDMap()->getMaxWT());
  }
  

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

bool BHV_Trail::updateInfoIn()
{
  bool ok1, ok2, ok3, ok4, ok5, ok6, ok7, ok8;
 
  m_osx = info_buffer->dQuery("NAV_X", ok1);
  m_osy = info_buffer->dQuery("NAV_Y", ok2);
  m_osh = info_buffer->dQuery("NAV_HEADING", ok3);
  m_osv = info_buffer->dQuery("NAV_SPEED", ok4);

  m_cnx = info_buffer->dQuery(m_contact+"_NAV_X", ok5);
  m_cny = info_buffer->dQuery(m_contact+"_NAV_Y", ok6);
  m_cnh = info_buffer->dQuery(m_contact+"_NAV_HEADING", ok7);
  m_cnv = info_buffer->dQuery(m_contact+"_NAV_SPEED", ok8);

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

  if(!ok1 || !ok2 || !ok3 || !ok4 || !ok5 || !ok6 || !ok7 || !ok8)
    return(false);
  
  return(true);
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
  if(contact_range < m_max_range)
    return(1.0);
  else
    return(0.0);
}








