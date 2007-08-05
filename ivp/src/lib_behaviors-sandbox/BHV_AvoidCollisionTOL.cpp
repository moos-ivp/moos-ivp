/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_AvoidCollisionTOL.cpp                            */
/*    DATE: Jul 5th 2005                                         */
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
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "AOF_AvoidCollision.h"
#include "BHV_AvoidCollisionTOL.h"
#include "OF_Reflector.h"
#include "CPAEngine.h"
#include "BuildUtils.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_AvoidCollisionTOL::BHV_AvoidCollisionTOL(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)avoid_collision");
  this->setParam("unifbox", "course=3, speed=3, tol=3");
  this->setParam("gridbox", "course=9, speed=6, tol=6");
  
  domain = subDomain(domain, "course,speed,tol");

  active_distance    = 1000;
  collision_distance = 10; 
  all_clear_distance = 75; 

  info_vars.push_back("NAV_X");
  info_vars.push_back("NAV_Y");
  info_vars.push_back("NAV_SPEED");
  info_vars.push_back("NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_AvoidCollisionTOL::setParam(string g_param, string g_val) 
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
  else if(g_param == "active_distance") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock)
      active_distance = dval;
    return(true);
  }  
  else if(g_param == "collision_distance") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock)
      collision_distance = dval;
    return(true);
  }  
  else if(g_param == "all_clear_distance") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock)
      all_clear_distance = dval;
    return(true);
  }  
  return(false);
}


//-----------------------------------------------------------
// Procedure: produceOF

IvPFunction *BHV_AvoidCollisionTOL::produceOF() 
{
  if(!unif_box || !grid_box) {
    postEMessage("Null UnifBox or GridBox.");
    return(0);
  }
  
  if(them_name == "") {
    postEMessage("contact name not set.");
    return(0);
  }

  bool ok = getBufferInfo();
  if(!ok) 
    return(0);

  double relevance = getRelevance();
  if(relevance <= 0)
    return(0);


  AOF_AvoidCollision aof(domain);
  ok = true;
  ok = ok && aof.setParam("os_lat", os_y);
  ok = ok && aof.setParam("os_lon", os_x);
  ok = ok && aof.setParam("cn_lat", cn_y);
  ok = ok && aof.setParam("cn_lon", cn_x);
  ok = ok && aof.setParam("cn_crs", cn_heading);
  ok = ok && aof.setParam("cn_spd", cn_speed);
  ok = ok && aof.setParam("collision_distance", collision_distance);
  ok = ok && aof.setParam("all_clear_distance", all_clear_distance);
  ok = ok && aof.initialize();

  if(!ok) {
    postEMessage("Unable to init AFO_AvoidCollision.");
    return(false);
  }

  OF_Reflector reflector(&aof, 1);
  reflector.createUniform(unif_box, grid_box);
  IvPFunction *of = reflector.extractOF();

  of->getPDMap()->normalize(0.0, 100.0);

  of->setPWT(relevance * priority_wt);

  return(of);
}

//-----------------------------------------------------------
// Procedure: getBufferInfo

bool BHV_AvoidCollisionTOL::getBufferInfo()
{
  bool ok1, ok2;

  cn_heading = info_buffer->dQuery(them_name+"_NAV_HEADING", ok1);
  cn_speed   = info_buffer->dQuery(them_name+"_NAV_SPEED",   ok2);
  if(!ok1 || !ok2) {
    postWMessage("contact course/speed info not found.");
    return(false);
  }

  os_heading = info_buffer->dQuery("NAV_HEADING", ok1);
  os_speed   = info_buffer->dQuery("NAV_SPEED", ok2);
  if(!ok1 || !ok2) {
    postEMessage("ownship course/speed info not found.");
    return(false);
  }
  
  os_heading = angle360(os_heading);
  cn_heading = angle360(cn_heading);

  cn_x = info_buffer->dQuery(them_name+"_NAV_X", ok2);
  cn_y = info_buffer->dQuery(them_name+"_NAV_Y", ok1);
  if(!ok1 || !ok2) {
    postWMessage("contact x/y info not found.");
    return(false);
  }

  os_x = info_buffer->dQuery("NAV_X", ok2);
  os_y = info_buffer->dQuery("NAV_Y", ok1);
  if(!ok1 || !ok2) {
    postEMessage("ownship x/y info not found.");
    return(false);
  }

  return(true);
}

//-----------------------------------------------------------
// Procedure: getRelevance
//            Calculate the relevance first. If zero-relevance, 
//            we won't bother to create the objective function.

double BHV_AvoidCollisionTOL::getRelevance()
{
  CPAEngine cengine(cn_x, cn_y, cn_heading, cn_speed, 
		    os_x, os_y);
  
  double relevance = 0;
  double cpa_dist = cengine.evalCPA(os_heading, os_speed, 10);
  if(cpa_dist < active_distance)
    relevance = 1.0 - (cpa_dist / active_distance);
  return(relevance);

#if 0
  // A more crude method - no need to use a CPAEngine, but works
  // only on distance to contact. CPAEngine will discern an impending
  // collision vs. a vehicle safely passing behind another. Both
  // situations may have similar proximity prior.
  double distance = hypot((os_x-cn_x),(os_y-cn_y));
  if(distance >= active_distance)
    return(0.0);
  double pct = 1.0 - (distance / active_distance);
  return(pct);
#endif
}









