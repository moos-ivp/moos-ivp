/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_AvoidObstacles.cpp                               */
/*    DATE: Aug 2nd 2006                                         */
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
#include "BHV_AvoidObstacles.h"
#include "OF_Reflector.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_AvoidObstacles::BHV_AvoidObstacles(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_waypoint");
  this->setParam("unifbox", "course=3, speed=2");
  this->setParam("gridbox", "course=9, speed=6");

  domain = subDomain(domain, "course,speed");

  info_vars.push_back("NAV_X");
  info_vars.push_back("NAV_Y");
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition".
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_AvoidObstacles::setParam(string param, string val) 
{
  if(IvPBehavior::setParamCommon(param, val))
    return(true);

  if((param == "polygon") || (param == "points")) {
    XYPolygon new_polygon;
    bool ok = new_polygon.initialize(val);
    if(!ok)
      return(false);
    obstacles.push_back(new_polygon);
    inner_dist.push_back(0);
    outer_dist.push_back(0);
    return(true);
  }
  else if(param == "inner_dist") {
    vector<string> svector = parseString(val, ',');
    if(svector.size() != 2)
      return(false);    
    string poly_label = svector[0];
    string poly_dist  = svector[1];
    double dval = atof(poly_dist.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    return(setInnerDist(poly_label, dval));
    return(true);
  }
  else if(param == "outer_dist") {
    vector<string> svector = parseString(val, ',');
    if(svector.size() != 2)
      return(false);    
    string poly_label = svector[0];
    string poly_dist  = svector[1];
    double dval = atof(poly_dist.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    return(setOuterDist(poly_label, dval));
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: produceOF

IvPFunction *BHV_AvoidObstacles::produceOF() 
{
#if 0
  if(!unif_box || !grid_box) {
    postEMessage("Null UnifBox or GridBox.");
    return(0);
  }

  // Set osX, osY, ptX, ptY, iptX, iptY;
  bool valid_point = setNextWaypoint();

  // If no next-waypoint return NULL - it may or may not be an
  // error condition. That is determined in the setNextWaypoint()
  // function.
  if(!valid_point)
    return(0);

  IvPFunction *ipf = 0;

  AOF_WPT2D aof(domain);
  aof.setParam("oslat", osY);
  aof.setParam("oslon", osX);
  aof.setParam("ptlat", iptY);
  aof.setParam("ptlon", iptX);
  aof.setParam("desired_speed", ptSPD);
  aof.initialize();
  
  OF_Reflector reflector(&aof, 1);
  reflector.createUniform(unif_box, grid_box);
  ipf = reflector.extractOF();
  
  ipf->setPWT(priority_wt);

  if(!silent) {
    IvPBox mpt = ipf->getPDMap()->getGrid()->getMaxPt();
    cout << "BHV_AvoidObstacles::produceOF():" << endl;
    cout << "maxpt:" << endl;
    mpt.print();
  }

  if(osSPD > 0) {
    double dist_meters = hypot((osX-ptX), (osY-ptY));
    double eta_seconds = dist_meters / osSPD;

    string stat = "vname=" + us_name + ",";
    stat += "index=" + intToString(current_waypt)   + ",";
    stat += "dist="  + doubleToString(dist_meters)  + ",";
    stat += "eta="   + doubleToString(eta_seconds);
    
    postMessage("VEHICLE_WPT_STAT_US", stat);
    postMessage("VEHICLE_WPT_STAT", stat);
    postMessage("VEHICLE_WPT_INDEX", current_waypt);
  }

  postMessage("VIEW_POLYGON", seglist.get_spec());

  return(ipf);
#endif
  return(0);
}

//-----------------------------------------------------------
// Procedure: setInnerDist

bool BHV_AvoidObstacles::setInnerDist(string label, double val)
{
  int ix = -1;
  for(int i=0; i<obstacles.size(); i++)
    if(obstacles[i].get_label() == label)
      ix = i;

  if(ix==-1)
    return(false);

  inner_dist[ix] = val;
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: setOuterDist

bool BHV_AvoidObstacles::setOuterDist(string label, double val)
{
  int ix = -1;
  for(int i=0; i<obstacles.size(); i++)
    if(obstacles[i].get_label() == label)
      ix = i;

  if(ix==-1)
    return(false);

  outer_dist[ix] = val;
  
  return(true);
}
