/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_CutRange.cpp                                     */
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
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include "AOF_CutRangeCPA.h"
#include "BHV_CutRange.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_CutRange::BHV_CutRange(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_cutrange");
  this->setParam("build_info", "uniform_piece=discrete@course:2,speed:3");
  this->setParam("build_info", "uniform_grid =discrete@course:8,speed:6");
  
  m_domain = subDomain(m_domain, "course,speed");

  // If equal: 100% priority above, 0% priority below
  // If not equal: Linear scale between 0-100 in between

  m_min_priority_range = 0;
  m_max_priority_range = 0;

  m_patience     = 0;
  m_giveup_range = 0;   // meters - zero means never give up
  m_time_on_leg  = 15;  // seconds

  addInfoVars("NAV_X, NAV_Y");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_CutRange::setParam(string g_param, string g_val) 
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
  else if(g_param == "dist_priority_interval") {
    g_val = stripBlankEnds(g_val);
    vector<string> svector = parseString(g_val, ',');
    if(svector.size() != 2)
      return(false);
    if(!isNumber(svector[0]) || !isNumber(svector[1]))
      return(false);
    
    double dval1 = atof(svector[0].c_str());
    double dval2 = atof(svector[1].c_str());
    if((dval1 < 0) || (dval2 < 0) || (dval1 > dval2))
      return(false);
    m_min_priority_range = dval1;
    m_max_priority_range = dval2;
    return(true);
  }  
  else if(g_param == "time_on_leg") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_time_on_leg = dval;
    return(true);
  }  
  else if(g_param == "giveup_range") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_giveup_range = dval;
    return(true);
  }  
  else if(g_param == "patience") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (dval > 100) || (!isNumber(g_val)))
      return(false);
    m_patience = dval;
    return(true);
  }  
  return(false);
}

//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_CutRange::onIdleState()
{
  postMessage("CUT_RANGE_ACTIVE", 0);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_CutRange::onRunState() 
{
  postMessage("CUT_RANGE_ACTIVE", 1);

  if(m_them_name == "") {
    postEMessage("contact ID not set.");
    return(0);
  }

  bool ok1, ok2;

  double cnCRS = getBufferDoubleVal(m_them_name+"_NAV_HEADING", ok1);
  double cnSPD = getBufferDoubleVal(m_them_name+"_NAV_SPEED", ok2);
  if(!ok1 || !ok2)
    return(0);

  if(cnCRS < 0) cnCRS += 360.0;

  double cnX = getBufferDoubleVal(m_them_name+"_NAV_X", ok1);
  double cnY = getBufferDoubleVal(m_them_name+"_NAV_Y", ok2);
  if(!ok1 || !ok2)
    return(0);

  double osX = getBufferDoubleVal("NAV_X", ok1);
  double osY = getBufferDoubleVal("NAV_Y", ok2);
  if(!ok1 || !ok2) {
    postEMessage("ownship x/y info not found.");
    return(0);
  }


  // Calculate the relevance first. If zero-relevance, we won't
  // bother to create the objective function.
  double relevance = getRelevance(osX, osY, cnX, cnY);

  if(relevance <= 0)
    return(0);

  AOF_CutRangeCPA aof(m_domain);
  aof.setParam("cnlat", cnY);
  aof.setParam("cnlon", cnX);
  aof.setParam("cncrs", cnCRS);
  aof.setParam("cnspd", cnSPD);
  aof.setParam("oslat", osY);
  aof.setParam("oslon", osX);
  aof.setParam("tol", m_time_on_leg);
  aof.setParam("patience", m_patience);
  ok1 = aof.initialize();
  if(!ok1) {
    postWMessage("Error in initializing AOF_CutRangeCPA.");
    return(0);
  }
  
  IvPFunction *ipf = 0;
  OF_Reflector reflector(&aof);
  reflector.create(m_build_info);
  if(reflector.hasErrors())
    postWMessage(reflector.getErrors());
  else {
    ipf = reflector.extractOF(true); // true means normalize [0,100]
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

double BHV_CutRange::getRelevance(double osX, double osY,
				  double cnX, double cnY)
{
  double range_max = m_max_priority_range;
  double range_min = m_min_priority_range;

  // Should be caught when setting the parameters, but check again
  if((range_max < 0) || (range_min < 0) || (range_min > range_max)) {
    postWMessage("Priority Range Error");
    return(0);
  }
  
  double dist = hypot((osX - cnX), (osY - cnY));

  if((m_giveup_range > 0) && (dist > m_giveup_range))
    return(0);

  
  double total_range = range_max - range_min;
  
  // if total_range==0 one of the two cases will result in a return
  if(dist >= range_max)
    return(100.0);
  if(dist < range_min)
    return(0.0);

  double pct = (dist - range_min) / total_range;
  return(pct);
}









