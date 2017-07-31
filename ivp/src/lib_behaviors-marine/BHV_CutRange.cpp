/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
#include "BuildUtils.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_CutRange::BHV_CutRange(IvPDomain gdomain) : 
  IvPContactBehavior(gdomain)
{
  this->setParam("descriptor", "bhv_cutrange");
  this->setParam("build_info", "uniform_piece=discrete@course:2,speed:3");
  this->setParam("build_info", "uniform_grid =discrete@course:8,speed:6");
  
  m_domain = subDomain(m_domain, "course,speed");

  m_pwt_outer_dist = 0;
  m_pwt_inner_dist = 0;

  m_patience     = 0;
  m_giveup_range = 0;   // meters - zero means never give up
  m_time_on_leg  = 15;  // seconds
  
  addInfoVars("NAV_X, NAV_Y");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_CutRange::setParam(string param, string param_val) 
{
  if(IvPContactBehavior::setParam(param, param_val))
    return(true);

  double dval = atof(param_val.c_str());
  bool non_neg_number = (isNumber(param_val) && (dval >= 0));

  if(param == "pwt_outer_dist") { 
    if(!non_neg_number)
      return(false);
    m_pwt_outer_dist = dval;
    if(m_pwt_inner_dist > m_pwt_outer_dist)
      m_pwt_inner_dist = m_pwt_outer_dist;
    return(true);
  }  
  else if(param == "pwt_inner_dist") {
    if(!non_neg_number)
      return(false);
    m_pwt_inner_dist = dval;
    if(m_pwt_outer_dist < m_pwt_inner_dist)
      m_pwt_outer_dist = m_pwt_inner_dist;
    return(true);
  }  
  if(param == "dist_priority_interval") {     // Deprecated 4/2010
    param_val = stripBlankEnds(param_val);
    string left  = stripBlankEnds(biteString(param_val, ','));
    string right = stripBlankEnds(param_val);
    if(!isNumber(left) || !isNumber(right))
      return(false);
    double dval1 = atof(left.c_str());
    double dval2 = atof(right.c_str());
    if((dval1 < 0) || (dval2 < 0) || (dval1 > dval2))
      return(false);
    m_pwt_inner_dist = dval1;
    m_pwt_outer_dist = dval2;
    return(true);
  }  
  else if((param == "giveup_dist") ||      // preferred
	  (param == "giveup_range")) {     // supported alternative
    if(!non_neg_number)
      return(false);
    m_giveup_range = dval;
    return(true);
  }  
  else if(param == "patience") {
    if((dval < 0) || (dval > 100) || (!isNumber(param_val)))
      return(false);
    m_patience = dval;
    return(true);
  }  

  return(false);
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_CutRange::onRunState() 
{
  if(!updatePlatformInfo())
    return(0);

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

#if 0
  cout << "CutRange Pre-Normalize MIN-WT: " << ipf->getPDMap()->getMinWT() << endl;
  cout << "CutRange Pre-Normalize MAX-WT: " << ipf->getPDMap()->getMaxWT() << endl;
  cout << "CutRange MIN-WT: " << ipf->getPDMap()->getMinWT() << endl;
  cout << "CutRange MAX-WT: " << ipf->getPDMap()->getMaxWT() << endl;
#endif
  
  return(ipf);
}

//-----------------------------------------------------------
// Procedure: getRelevance

double BHV_CutRange::getRelevance()
{
  // Should be caught when setting the parameters, but check again
  if((m_pwt_outer_dist < 0) || (m_pwt_inner_dist < 0) || 
     (m_pwt_inner_dist > m_pwt_outer_dist)) {
    postWMessage("Priority Range Error");
    return(0);
  }
  
  if((m_giveup_range > 0) && (m_contact_range > m_giveup_range))
    return(0);

  double total_range = m_pwt_outer_dist - m_pwt_inner_dist;
  
  // if total_range==0 one of the two cases will result in a return
  if(m_contact_range >= m_pwt_outer_dist)
    return(100.0);
  if(m_contact_range < m_pwt_inner_dist)
    return(0.0);

  double pct = (m_contact_range - m_pwt_inner_dist) / total_range;
  return(pct);
}


















