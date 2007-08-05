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
  this->setParam("unifbox", "course=3, speed=2, tol=2");
  this->setParam("gridbox", "course=9, speed=6, tol=6");

  domain = subDomain(domain, "course,speed,tol");

  range_max = 0;
  range_min = 50;

  info_vars.push_back("NAV_X");
  info_vars.push_back("NAV_Y");
  info_vars.push_back("NAV_SPEED");
  info_vars.push_back("NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_CutRange::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParamCommon(g_param, g_val))
    return(true);

  if((g_param == "them") || (g_param == "contact")) {
    if(!param_lock) {
      them_name = toupper(g_val);
      info_vars.push_back(them_name+"_NAV_X");
      info_vars.push_back(them_name+"_NAV_Y");
      info_vars.push_back(them_name+"_NAV_SPEED");
      info_vars.push_back(them_name+"_NAV_HEADING");
    }
    return(true);
  }  
  else if(g_param == "max_range") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock)
      range_max = dval;
    return(true);
  }  
  else if(g_param == "min_range") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock)
      range_min = dval;
    return(true);
  }  
  return(false);
}


//-----------------------------------------------------------
// Procedure: produceOF

IvPFunction *BHV_CutRange::produceOF() 
{
  if(!unif_box || !grid_box) {
    postEMessage("Null UnifBox or GridBox.");
    return(0);
  }
  
  if(them_name == "") {
    postEMessage("contact ID not set.");
    return(0);
  }

  bool ok1, ok2;

  double cnCRS = info_buffer->dQuery(them_name+"_NAV_HEADING", ok1);
  double cnSPD = info_buffer->dQuery(them_name+"_NAV_SPEED", ok2);
  if(!ok1 || !ok2) {
    postWMessage("contact course/speed info not found.");
    return(0);
  }

  double osCRS = info_buffer->dQuery("NAV_HEADING", ok1);
  double osSPD = info_buffer->dQuery("NAV_SPEED", ok2);
  if(!ok1 || !ok2) {
    postEMessage("ownship course/speed info not found.");
    return(0);
  }

  if(cnCRS < 0) cnCRS += 360.0;

  double cnX = info_buffer->dQuery(them_name+"_NAV_X", ok2);
  double cnY = info_buffer->dQuery(them_name+"_NAV_Y", ok1);
  if(!ok1 || !ok2) {
    postWMessage("contact x/y info not found.");
    return(0);
  }

  double osX = info_buffer->dQuery("NAV_X", ok2);
  double osY = info_buffer->dQuery("NAV_Y", ok1);
  if(!ok1 || !ok2) {
    postEMessage("ownship x/y info not found.");
    return(0);
  }


  // Calculate the relevance first. If zero-relevance, we won't
  // bother to create the objective function.
  double relevance = getRelevance(osX, osY, cnX, cnY);

  if(relevance <= 0)
    return(0);

  AOF_CutRangeCPA aof(domain);
  aof.setParam("cnlat", cnY);
  aof.setParam("cnlon", cnX);
  aof.setParam("cncrs", cnCRS);
  aof.setParam("cnspd", cnSPD);
  aof.setParam("oslat", osY);
  aof.setParam("oslon", osX);
  aof.initialize();

  OF_Reflector reflector(&aof, 1);

  reflector.createUniform(unif_box, grid_box);

  IvPFunction *of = reflector.extractOF();

  if(!silent) {
    cout << "CutRange Pre-Normalize MIN-WT: " << of->getPDMap()->getMinWT() << endl;
    cout << "CutRange Pre-Normalize MAX-WT: " << of->getPDMap()->getMaxWT() << endl;
  }

  of->getPDMap()->normalize(0.0, 100.0);

  if(!silent) {
    cout << "CutRange MIN-WT: " << of->getPDMap()->getMinWT() << endl;
    cout << "CutRange MAX-WT: " << of->getPDMap()->getMaxWT() << endl;
  }

  of->setPWT(relevance * priority_wt);

  return(of);
}

//-----------------------------------------------------------
// Procedure: getRelevance

double BHV_CutRange::getRelevance(double osX, double osY,
				  double cnX, double cnY)
{
  double total_range = range_max - range_min;
  if(total_range <= 0)  // This would likely be an error
    return(0);


  double dist = hypot((osX - cnX), (osY - cnY));
  if(!silent)
    cout << "BHV_CutRange: Current Distance ------" << dist << endl;

  if(dist > range_max)
    return(0.0);
  if(dist < range_min)
    return(0.0);

  double pct = (dist - range_min) / total_range;
  return(pct);
}







