/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_Shadow.cpp                                       */
/*    DATE: May 10th 2005                                        */
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
#include "BHV_Shadow.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"
#include "BuildUtils.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Shadow::BHV_Shadow(IvPDomain gdomain) : 
  IvPContactBehavior(gdomain)
{
  this->setParam("descriptor", "bhv_shadow");

  m_domain = subDomain(m_domain, "course,speed");

  m_pwt_outer_dist = 0;
  m_hdg_peakwidth  = 20;
  m_hdg_basewidth  = 160;
  m_spd_peakwidth  = 0.1;
  m_spd_basewidth  = 2.0;

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_Shadow::setParam(string param, string param_val) 
{
  if(IvPContactBehavior::setParam(param, param_val))
    return(true);

  double dval = atof(param_val.c_str());
  bool non_neg_number = (isNumber(param_val) && (dval >= 0));

  if((param == "pwt_outer_dist") ||         // preferred
	  (param == "max_range")) {         // deprecated 4/2010
    if(!non_neg_number)
      return(false);    
    m_pwt_outer_dist = dval;
    return(true);
  }  
  else if((param == "heading_peakwidth") ||  // preferred
	  (param == "hdg_peakwidth")) {      // supported alternative
    if(!non_neg_number)
      return(false);
    m_hdg_peakwidth = dval;
    return(true);
  }
  else if((param == "heading_basewidth") ||  // preferred
	  (param == "hdg_basewidth")) {      // supported alternative
    if(!non_neg_number)
      return(false);
    m_hdg_basewidth = dval;
    return(true);
  }
  else if((param == "speed_peakwidth") ||    // preferred
	  (param == "spd_peakwidth")) {      // supported alternative
    if(!non_neg_number)
      return(false);
    m_spd_peakwidth = dval;
    return(true);
  }
  else if((param == "speed_basewidth") ||    // preferred
	  (param == "spd_basewidth")) {      // supported alternative
    if(!non_neg_number)
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
  if(!updatePlatformInfo())
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
  IvPFunction *hdg_ipf = hdg_zaic.extractIvPFunction();
  if(!hdg_ipf) { 
    postEMessage("Unable to generate hdg component of IvP function");
    postWMessage(hdg_zaic.getWarnings());
    return(0);
  }
  
  ZAIC_PEAK spd_zaic(m_domain, "speed");
  spd_zaic.setSummit(m_cnv);
  spd_zaic.setPeakWidth(m_spd_peakwidth);
  spd_zaic.setBaseWidth(m_spd_basewidth);
  spd_zaic.setSummitDelta(10.0); 
  hdg_zaic.setMinMaxUtil(0,25);  
  IvPFunction *spd_ipf = spd_zaic.extractIvPFunction();
  if(!spd_ipf) { 
    postEMessage("Unable to generate spd component of IvP function");
    postWMessage(spd_zaic.getWarnings());
    return(0);
  }
  
  OF_Coupler coupler;
  IvPFunction *ipf = coupler.couple(hdg_ipf, spd_ipf);
      
  if(ipf) {
    ipf->getPDMap()->normalize(0.0, 100.0);
    ipf->setPWT(relevance * m_priority_wt);
  }
  else
    postEMessage("Unable to generate coupled IvP function");

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: getRelevance

double BHV_Shadow::getRelevance()
{
  if(m_pwt_outer_dist == 0)
    return(1.0);
  
  if(m_contact_range < m_pwt_outer_dist)
    return(1.0);
  else
    return(0.0);
}


















