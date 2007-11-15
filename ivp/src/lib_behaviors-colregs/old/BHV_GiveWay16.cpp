/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_GiveWay16.cpp                                    */
/*    DATE:                                                      */
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
#include "BHV_GiveWay16.h"
#include "AOF_R16.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_GiveWay16::BHV_GiveWay16(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_giveway16");
  this->setParam("unifbox", "course=2, speed=2, tol=2");
  this->setParam("gridbox", "course=8, speed=6, tol=6");

  domain = subDomain(domain, "course,speed,tol");

  range_active = -1;
  range_fullwt = -1;

  info_vars.push_back("NAV_X");
  info_vars.push_back("NAV_Y");
  info_vars.push_back("NAV_SPEED");
  info_vars.push_back("NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_GiveWay16::setParam(string g_param, string g_val) 
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
  else if(g_param == "range_active") {
    range_active = atof(g_val.c_str());
    return(true);
  }  
  else if(g_param == "range_fullwt") {
    range_fullwt = atof(g_val.c_str());
    return(true);
  }  
  return(false);
}


//-----------------------------------------------------------
// Procedure: produceOF

IvPFunction *BHV_GiveWay16::produceOF() 
{
  if(!unif_box || !grid_box) {
    postEMessage("Null UnifBox or GridBox.");
    return(0);
  }
  
  // Refresh ownship and contact positions and heading vector
  bool ok = getCurrentPositions();
  if(!ok)
    return(0);

  // Determine whether or not this behavior/rule is applicable
  double relevance = getRelevance();
  if(relevance == 0)
    return(0);
  
  // Build the "underlying" function to be approximated
  AOF_R16 aof(domain);
  aof.setParam("os_lat", osLAT);
  aof.setParam("os_lon", osLON);
  aof.setParam("cn_lat", cnLAT);
  aof.setParam("cn_lon", cnLON);
  aof.setParam("cn_crs", cnCRS);
  aof.setParam("cn_spd", cnSPD);
  aof.initialize();
  
  // Setup piecewise function in relation to underlying function
  OF_Reflector reflector(&aof, 1);

  reflector.setStrictRange(true);
  // Create the piecewise function
  reflector.createUniform(unif_box, grid_box);

  // Turn it into an ObjectiveFunction object for returning
  IvPFunction *of = reflector.extractOF();

  of->setPWT(relevance * priority_wt);

  return(of);
}

//-----------------------------------------------------------
// Procedure: getCurrentPositions()

bool BHV_GiveWay16::getCurrentPositions() 
{
  if(them_name == "") {
    postEMessage("contact ID not set.");
    return(0);
  }

  bool ok1, ok2;

  cnCRS = info_buffer->dQuery(them_name+"_NAV_HEADING", ok1);
  cnSPD = info_buffer->dQuery(them_name+"_NAV_SPEED", ok2);
  if(!ok1 || !ok2) {
    postWMessage("contact course/speed info not found.");
    return(false);
  }

  osCRS = info_buffer->dQuery("NAV_HEADING", ok1);
  osSPD = info_buffer->dQuery("NAV_SPEED", ok2);
  if(!ok1 || !ok2) {
    postEMessage("ownship course/speed info not found.");
    return(false);
  }

  osCRS = angle360(osCRS);
  cnCRS = angle360(cnCRS);

  cnLAT = info_buffer->dQuery(them_name+"_NAV_Y", ok1);
  cnLON = info_buffer->dQuery(them_name+"_NAV_X", ok2);
  if(!ok1 || !ok2) {
    postWMessage("contact x/y info not found.");
    return(false);
  }

  osLAT = info_buffer->dQuery("NAV_Y", ok1);
  osLON = info_buffer->dQuery("NAV_X", ok2);
  if(!ok1 || !ok2) {
    postEMessage("ownship x/y info not found.");
    return(false);
  }
  return(true);
}

//-----------------------------------------------------------
// Procedure: getRelevance

double BHV_GiveWay16::getRelevance()
{
  
  // If for some reason the two vehicles are in the same x,y 
  // position - return zero relevance.
  if((osLON==cnLON)&&(osLAT==cnLAT))
    return(0);

  // If range was specified, and we're out of range, return
  // zero relevance. Range of -1 means range wasn't specified.
  double dist = hypot((osLON-cnLON), (osLAT-cnLAT));
  if((range_active > -1) && (dist > range_active))
    return(0);
  
  double ang_to_os = relAng(cnLON, cnLAT, osLON, osLAT);

  //cout << "ang_to_os:" << ang_to_os << endl;

  // Determine the "bearing delta" - the difference between the 
  // course of a vehicle and the bearing of that vehicle to the 
  // other vehicle.
  //
  //                 os     Bearing to ownship is 35 degrees
  //                /
  //               /
  //              /        "Bearing delta" is 55 degrees
  //             /
  //            /
  //          cn------->   contact course is 90 degrees
  //

  double cn_bearing_delta = angle360(cnCRS - ang_to_os);

  //cout << "cn_bearing_delta:" << cn_bearing_delta << endl;

  if((cn_bearing_delta <= 15.0) || (cn_bearing_delta > 90.0))
    return(0.0);
  
  if(dist < range_fullwt)
    dist = range_fullwt;
  
  double range_range = (range_active - range_fullwt);
  double pct = 1.0 -  ((dist - range_fullwt) / range_range);
  return(pct);
}





