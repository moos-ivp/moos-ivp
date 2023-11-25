/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_CutRange.cpp                                     */
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
#include <iostream>
#include <cmath>
#include <cstdlib>
#include "AOF_CutRangeCPA.h"
#include "BHV_CutRange.h"
#include "OF_Reflector.h"
#include "RefineryCPA.h"
#include "BuildUtils.h"
#include "VarDataPairUtils.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_CutRange::BHV_CutRange(IvPDomain gdomain) : IvPContactBehavior(gdomain)
{
  this->setParam("descriptor", "bhv_cutrange");
  this->setParam("build_info", "uniform_piece=discrete@course:2,speed:3");
  this->setParam("build_info", "uniform_grid =discrete@course:8,speed:6");
  
  m_domain = subDomain(m_domain, "course,speed");
  
  m_pwt_outer_dist = 0;
  m_pwt_inner_dist = 0;

  m_patience      = 0;
  m_giveup_range  = 0;   // meters - zero means never give up
  m_giveup_thresh = 1;   // meters - anti thrashing buffer
  m_time_on_leg   = 15;  // seconds

  m_in_pursuit = false;
  
  m_no_alert_request  = false;
  
  addInfoVars("NAV_X, NAV_Y");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_CutRange::setParam(string param, string param_val) 
{
  if(IvPContactBehavior::setParam(param, param_val))
    return(true);

  double dval = atof(param_val.c_str());

  if(param == "pwt_inner_dist") {
    return(setMinPartOfPairOnString(m_pwt_inner_dist,
				    m_pwt_outer_dist, param_val));
  }  
  else if(param == "pwt_outer_dist") { 
    return(setMaxPartOfPairOnString(m_pwt_inner_dist,
				    m_pwt_outer_dist, param_val));
  }
  else if((param == "giveup_dist") || (param == "giveup_range")) 
    return(setNonNegDoubleOnString(m_giveup_range, param_val));
  else if(param == "pursueflag")
    return(addVarDataPairOnString(m_pursue_flags, param_val));
  else if(param == "giveupflag") 
    return(addVarDataPairOnString(m_giveup_flags, param_val));
  else if((param == "patience") && isNumber(param_val)) {
    if((dval < 0) || (dval > 100))
      return(false);
    m_patience = dval;
    return(true);
  }  

  return(false);
}

//-----------------------------------------------------------
// Procedure: onHelmStart()
//      Note: This function is called when the helm starts, even if,
//            especially if, the behavior is just a template at start
//            time to be spawned later. 
//      Note: An alert request will be sent to the contact manager if
//            the behavior is configured with templating enabled, and
//            an updates variable has been provided.  In the rare case
//            that the above is true but the user still does not want
//            an alert request generated, this can be done by setting
//            m_no_alert_request to true.

void BHV_CutRange::onHelmStart()
{
  if(m_no_alert_request || (m_update_var == "") || !m_dynamically_spawnable)
    return;

  string alert_templ = m_update_var + "=name=$[VNAME] # contact=$[VNAME]";
  string request = "id=" + getDescriptor();
  request += ", on_flag=" + alert_templ;
  request += ",alert_range=" + doubleToStringX(m_pwt_outer_dist,1);
  request += ", cpa_range=" + doubleToStringX(m_pwt_outer_dist+1,1);
  request = augmentSpec(request, getFilterSummary());
  
  postMessage("BCM_ALERT_REQUEST", request);
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_CutRange::onRunState() 
{
  if(!updatePlatformInfo())
    return(0);

  checkPursuit();
  // Calculate the relevance first. If zero-relevance, we won't
  // bother to create the objective function.
  double relevance = getRelevance();

  if(relevance <= 0)
    return(0);

  AOF_CutRangeCPA aof(m_domain);
  aof.setParam("cnlat", m_cny);
  aof.setParam("cnlon", m_cnx);
  aof.setParam("cncrs", m_cnh);
  aof.setParam("cnspd", m_cnv);
  aof.setParam("oslat", m_osy);
  aof.setParam("oslon", m_osx);
  aof.setParam("tol", m_time_on_leg);
  aof.setParam("patience", m_patience);
  bool ok = aof.initialize();
  if(!ok) {
    postWMessage("Error in initializing AOF_CutRangeCPA.");
    return(0);
  }
  
  IvPFunction *ipf = 0;
  OF_Reflector reflector(&aof);

  reflector.create(m_build_info);
  if(!reflector.stateOK())
    postWMessage(reflector.getWarnings());
  else {
    ipf = reflector.extractIvPFunction(true); // true means normalize [0,100]
    ipf->setPWT(relevance * m_priority_wt);
  }

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: checkPursuit()

void BHV_CutRange::checkPursuit()
{
  // Sanity check
  if(m_giveup_range <= 0)
    return;
  
  bool in_pursuit = true;
  if(m_contact_range > (m_giveup_range + m_giveup_thresh))
    in_pursuit = false;
  
  if(in_pursuit && !m_in_pursuit)
    postFlags(m_pursue_flags);
  if(!in_pursuit && m_in_pursuit)
    postFlags(m_giveup_flags);

  m_in_pursuit = in_pursuit;
}

//-----------------------------------------------------------
// Procedure: getRelevance

double BHV_CutRange::getRelevance()
{
  // Sanity check
  if((m_pwt_outer_dist < 0) || (m_pwt_inner_dist < 0) || 
     (m_pwt_inner_dist > m_pwt_outer_dist)) {
    postWMessage("Priority Range Error");
    return(0);
  }

  // Sanity check
  double total_range = m_pwt_outer_dist - m_pwt_inner_dist;  
  if(total_range == 0)
    return(0);

  // Sanity check
  if(!m_in_pursuit)
    return(0);


  // if total_range==0 one of the two cases will result in a return
  if(m_contact_range >= m_pwt_outer_dist)
    return(1);
  if(m_contact_range < m_pwt_inner_dist)
    return(0);


  double pct = (m_contact_range - m_pwt_inner_dist) / total_range;
  return(pct);
}

