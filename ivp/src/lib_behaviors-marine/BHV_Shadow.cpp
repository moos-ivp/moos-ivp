/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Shadow.cpp                                       */
/*    DATE: May 10th 2005                                        */
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
#include "BHV_Shadow.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"
#include "BuildUtils.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Shadow::BHV_Shadow(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_shadow");

  m_domain = subDomain(m_domain, "course,speed");

  m_max_range     = 0;
  m_hdg_peakwidth = 20;
  m_hdg_basewidth = 160;
  m_spd_peakwidth = 0.1;
  m_spd_basewidth = 2.0;

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_Shadow::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParam(g_param, g_val))
    return(true);

  if((g_param == "them") || (g_param == "contact")) {
    m_them_name = toupper(g_val);
    addInfoVars(m_them_name+"_NAV_X");
    addInfoVars(m_them_name+"_NAV_Y");
    addInfoVars(m_them_name+"_NAV_SPEED");
    addInfoVars(m_them_name+"_NAV_HEADING");
    return(true);
  }  
  else if(g_param == "max_range") {
    m_max_range = atof(g_val.c_str());
    return(true);
  }  
  if((g_param == "hdg_peakwidth") || (g_param == "heading_peakwidth")) {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_hdg_peakwidth = dval;
    return(true);
  }
  if((g_param == "hdg_basewidth") || (g_param == "heading_basewidth")) {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_hdg_basewidth = dval;
    return(true);
  }

  if((g_param == "spd_peakwidth") || (g_param == "speed_peakwidth")) {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_spd_peakwidth = dval;
    return(true);
  }
  if((g_param == "spd_basewidth") || (g_param == "speed_basewidth")) {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_spd_basewidth = dval;
    return(true);
  }

  return(false);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_Shadow::onRunState() 
{
  if(m_them_name == "") {
    postWMessage("contact ID not set.");
    return(0);
  }

  //  if(!m_domain.hasDomain("course")) {
  //   postWMessage("No 'heading/course' variable in the helm domain");
  //  return(0);
  //}
  //if(!m_domain.hasDomain("speed")) {
  //  postWMessage("No 'speed' variable in the helm domain");
  //  return(0);
  //}

  // Set m_osx, m_osy, m_cnx, m_cny, m_cnv, m_cnh
  if(!updateInfoIn())
    return(0);
  
  // Calculate the relevance first. If zero-relevance, we won't
  // bother to create the objective function.
  double relevance = getRelevance();

  postIntMessage("SHADOW_CONTACT_X", m_cnx);
  postIntMessage("SHADOW_CONTACT_Y", m_cny);
  postIntMessage("SHADOW_CONTACT_SPEED", m_cnv);
  postIntMessage("SHADOW_CONTACT_HEADING", m_cnh);
  postIntMessage("SHADOW_RELEVANCE", relevance);
  

  if(relevance <= 0)
    return(0);

      ZAIC_PEAK hdg_zaic(m_domain, "course");
      hdg_zaic.setSummit(m_cnh);
      hdg_zaic.setValueWrap(true);
      hdg_zaic.setPeakWidth(m_hdg_peakwidth);
      hdg_zaic.setBaseWidth(m_hdg_basewidth);
      hdg_zaic.setSummitDelta(50.0);
      hdg_zaic.setMinMaxUtil(0,100);
      //  hdg_zaic.addSummit(m_cnh, m_hdg_peakwidth, m_hdg_basewidth, 50, 0, 100);
      // hdg_zaic.setValueWrap(true);
      IvPFunction *hdg_ipf = hdg_zaic.extractOF();
  
      ZAIC_PEAK spd_zaic(m_domain, "speed");
      spd_zaic.setSummit(m_cnv);
      spd_zaic.setPeakWidth(m_spd_peakwidth);
      spd_zaic.setBaseWidth(m_spd_basewidth);
      spd_zaic.setSummitDelta(10.0); 
      hdg_zaic.setMinMaxUtil(0,25);
      
      //  spd_zaic.addSummit(m_cnv, m_spd_peakwidth, m_spd_basewidth, 10, 0, 25);
      // spd_zaic.setValueWrap(true);
      IvPFunction *spd_ipf = spd_zaic.extractOF();
  
      OF_Coupler coupler;
      IvPFunction *ipf = coupler.couple(hdg_ipf, spd_ipf);
      //      if(ipf)
	//	ipf->setPWT(relevance * m_priority_wt);
      
	//     ipf->getPDMap()->normalize(0.0, 100.0);
      
  if(ipf) 
    {
      ipf->getPDMap()->normalize(0.0, 100.0);
      ipf->setPWT(relevance * m_priority_wt);
    }
  
#if 0
    postIntMessage("SHADOW_MIN_WT", ipf->getPDMap()->getMinWT());
    postIntMessage("SHADOW_MAX_WT", ipf->getPDMap()->getMaxWT());
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

bool BHV_Shadow::updateInfoIn()
{
  bool ok1, ok2;
  
  m_cnh = getBufferDoubleVal(m_them_name+"_NAV_HEADING", ok1);
  m_cnv = getBufferDoubleVal(m_them_name+"_NAV_SPEED", ok2);
  if(!ok1 || !ok2)
    {
    postEMessage("contact speed and heading info not found.");
    return(false);
    }
  m_cnh = angle360(m_cnh);

  m_cnx = getBufferDoubleVal(m_them_name+"_NAV_X", ok1);
  m_cny = getBufferDoubleVal(m_them_name+"_NAV_Y", ok2);
  if(!ok1 || !ok2)
    {
      postEMessage("contact x/y info not found.");
      return(false);
    }

  m_osx = getBufferDoubleVal("NAV_X", ok1);
  m_osy = getBufferDoubleVal("NAV_Y", ok2);
  if(!ok1 || !ok2) {
    postEMessage("ownship x/y info not found.");
    return(false);
  }
  return(true);
}


//-----------------------------------------------------------
// Procedure: getRelevance

double BHV_Shadow::getRelevance()
{
  if(m_max_range == 0)
    return(1.0);
  
  double contact_range = hypot((m_osx-m_cnx), (m_osy-m_cny));
  if(contact_range < m_max_range)
    return(1.0);
  else
    return(0.0);
}









