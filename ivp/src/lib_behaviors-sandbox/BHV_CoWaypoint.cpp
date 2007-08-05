/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_CoWaypoint.cpp                                   */
/*    DATE: Nov 2004                                             */
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
#include "BHV_CoWaypoint.h"
#include "MBUtils.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_CoWaypoint::BHV_CoWaypoint(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_cowaypoint");

  domain = subDomain(domain, "speed");

  politeness = 50.0;   
  wait_speed = 0.0;

  domain = subDomain(domain, "speed");

  info_vars.push_back("VEHICLE_WPT_STAT");
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition,tag" where the tag is an optional
//            descriptor of the waypoint.
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_CoWaypoint::setParam(string g_param, string g_val) 
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
  else if(g_param == "politeness") {
    if(!isNumber(g_val))
      return(false);
    if(!param_lock)
      politeness = atof(g_val.c_str());
    return(true);
  }
  else if(g_param == "wait_speed") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock)
      wait_speed = dval;
    return(true);
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: produceOF
//
//          ^     should-speed    
//      100-|           .
//          |          / \
//          | piece-1 /   \ piece-2
//          |        /     \
// Utility  |       /       \
//          |      /         \
//          |     /           \
//          |    /             \
//          |___/_______________\__________________
//                    Speed
//
//

IvPFunction *BHV_CoWaypoint::produceOF() 
{
  // Need to know name of us and them to query own waypt progress
  if(them_name == "") {
    postEMessage("contact ID not set");
    return(0);
  }

  bool ok1, ok2;
  string us_waypt_stat   = info_buffer->sQuery("VEHICLE_WPT_STAT", ok1);
  string them_waypt_stat = info_buffer->sQuery(them_name+"_VEHICLE_WPT_STAT", ok2);

  if(!silent) {
    cout << "+++++BHV_CoWaypoint::produceOF() " << endl;
    cout << "  us_waypt_stat:" << us_waypt_stat << endl;
    cout << "  them_waypt_stat:" << them_waypt_stat << endl;
  }

  // Report an error condition if we don't find at least an empty string
  if(!ok1) {    
    postWMessage("No ownship waypoint progress in InfoBuffer");
    return(0);
  }
  if(!ok2) {
    postWMessage("No contact waypoint progress in InfoBuffer");
    return(0);
  }

  // If waypt_stat strings are present, but one or both is empty, that
  // indicates that one or the other vehicle has reached its final 
  // waypoint, and thus no coordination is necessary
  
  if((us_waypt_stat == "complete") || (them_waypt_stat == "complete"))
    return(0);

  int    us_index, them_index;
  double us_dist,  them_dist;
  double us_eta,   them_eta;

  ok1 = parseStatString(us_waypt_stat, us_index, us_dist, us_eta);
  ok2 = parseStatString(them_waypt_stat, them_index, them_dist, them_eta);
  if(!ok1 || !ok2) 
    return(0);

  double peak_utility = 100;

  int    spdIndex  = domain.getIndex("speed");
  double spdBase   = domain.getVarLow(spdIndex);
  double spdHigh   = domain.getVarHigh(spdIndex);
  double spdDelta  = domain.getVarDelta(spdIndex);
  int    spdPoints = domain.getVarPoints(spdIndex);


  // Setting the "should" speed - the speed at which we
  // would arrive at the next waypoint simultaneously.
  double raw_should_speed = us_dist / them_eta;
  double should_speed = raw_should_speed;
  if((us_index < them_index) || (should_speed > spdHigh))
    should_speed = spdHigh;
  if((us_index > them_index) || (should_speed < 0)) {
    if((wait_speed >= 0) && (wait_speed <= spdHigh))
      should_speed = wait_speed;
    else
      should_speed = 0;
  }

  // Convert the "should" speed into an index in the IvPDomain
  double double_index = (should_speed - spdBase) / spdDelta;
  int    domain_index = (int)(floor(double_index + 0.5))+1;


  // Build the one or two pieces that capture what we want
  double slope = peak_utility / domain_index;

  IvPBox universe = IvPBox(1,1);
  IvPBox *piece0 = new IvPBox(1,1);
  IvPBox *piece1 = 0;

  universe.setPTS(0, 0, spdPoints-1);
  piece0->setPTS(0, 0, domain_index);
  piece0->wt(0) = slope;
  piece0->wt(1) = 0.0;

  if(domain_index < spdPoints) {
    piece1 = new IvPBox(1,1);
    piece1->setPTS(0, domain_index+1, spdPoints);
    piece1->wt(0) = -slope;
    piece1->wt(1) = 2.0 * peak_utility;
  }

  // Build a PDMap out of the pieces
  PDMap *pdmap;
  if(piece1) {
    pdmap = new PDMap(2, domain, 1);
    pdmap->bx(1) = piece1;
  }
  else
    pdmap = new PDMap(1, domain, 1);
  pdmap->bx(0) = piece0;

  // Build an objective function from the PDMap
  IvPFunction *of = new IvPFunction(pdmap);

  // Calc and Set the Priority of the objective function
  double weight = calcPriority(us_eta, them_eta);
  of->setPWT(weight);

  return(of);
}

//-----------------------------------------------------------
// Procedure: calcPriority
//     Notes: Depending on the situation, (a) how much the discrepency
//            is, and (b) the scale of the discrepency, we want to 
//            adjust the priority weight.
//     Notes: By default the priority weight can fluctuation between
//            its set value and 1.5 times that value for "dire" out
//            of sync situations. The value of pwt_max can change this 
//            upper bound.


double BHV_CoWaypoint::calcPriority(double us_eta, double them_eta)
{
  double delta_eta = fabs(us_eta - them_eta);
  
  // (1) First handle the situation where we're behind
  if(us_eta > them_eta) { 
    double eta_ratio = delta_eta / us_eta;
    return(eta_ratio * priority_wt);
  }

  // (2) Second handle the situation where we're behind
    
  double eta_ratio = delta_eta / them_eta;
  double weight = eta_ratio * priority_wt;
  weight = weight * (politeness / 100.00);
  return(weight);
}


//-----------------------------------------------------------
// Procedure: parseStatString
//      Note: "index=4, dist=34.5, eta=12.89"

bool BHV_CoWaypoint::parseStatString(string str, int& index, 
				     double& dist, double& eta)
{
  string str_index;
  string str_dist;
  string str_eta;

  bool ok1 = tokParse(str, "index", ',', '=', str_index);
  bool ok2 = tokParse(str, "dist",  ',', '=', str_dist);
  bool ok3 = tokParse(str, "eta",   ',', '=', str_eta);

  if(ok1 && ok2 && ok3) {
    index = atoi(str_index.c_str());
    dist  = atof(str_dist.c_str());
    eta   = atof(str_eta.c_str());
    return(true);
  }
  else {
    index = -1;
    dist = eta = -1;
    postEMessage("Invalid waypoint string");
    return(false);
  }
}
  







