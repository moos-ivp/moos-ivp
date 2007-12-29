/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_WaypointTOL.cpp                                  */
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
#include "BHV_WaypointTOL.h"
#include "AOF_WPT2D.h"
#include "AOF_WPT3D.h"
#include "OF_Reflector.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BuildUtils.h"
#include "FunctionEncoder.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_WaypointTOL::BHV_WaypointTOL(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_waypoint");
  this->setParam("unifbox", "course=3, speed=2, tol=4");
  this->setParam("gridbox", "course=9, speed=6, tol=8");

  this->setParam("build_info", "uniform_box=course:2,speed:2,tol=4");
  this->setParam("build_info", "uniform_grid=course:6,speed:6,tol=8");


  m_domain = subDomain(m_domain, "course,speed,tol");

  current_waypt   = 0;
  arrival_radius  = 10; // Meters
  cruise_speed    = 0;  // Meters/second
  non_mono_thresh = 10; // Meters
  non_mono_active = false;
  current_cpa     = 100000;
  lead_distance   = -1;
  repeat          = 0;

  m_completed     = false;

  osX   = -1;
  osY   = -1;
  osSPD = -1;
  ptX   = -1;
  ptY   = -1;
  iptX  = -1;
  iptY  = -1;

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED");
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition".
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_WaypointTOL::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  if((param == "polygon") || (param == "points")) {
    seglist.initialize(val);
    if((segspd.size() > 0) && (segspd.size() != seglist.size()))
      return(false);
    return(true);
  }
  else if(param == "speed") {
    cruise_speed = atof(val.c_str());
    return(cruise_speed > 0);
  }
  else if(param == "segspd") {
    vector<string> svector = parseString(val, ':');
    for(int i=0; i<svector.size(); i++)
      segspd.push_back(atof(svector[i].c_str()));
    if((seglist.size() > 0) && (segspd.size() != seglist.size()))
      return(false);
    return(true);
  }
  else if(param == "radius") {
    arrival_radius = atof(val.c_str());
    return(arrival_radius >= 0);
  }
  else if(param == "lead") {
    lead_distance = atof(val.c_str());
    return(lead_distance >= 0);
  }
  else if(param == "repeat") {
    repeat = atoi(val.c_str());
    if(repeat >= 0) 
      return(true);
  }
  else if(param == "non_mono_thresh") {
    non_mono_thresh = atof(val.c_str());
    non_mono_active = true;
    return(true);
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_WaypointTOL::onRunState() 
{
  // Set osX, osY, ptX, ptY, iptX, iptY;
  bool valid_point = setNextWaypoint();

  // If no next-waypoint return NULL - it may or may not be an
  // error condition. That is determined in the setNextWaypoint()
  // function.
  if(!valid_point)
    return(0);

  IvPFunction *of = 0;

  AOF_WPT3D aof(m_domain);
  aof.setParam("oslat", osY);
  aof.setParam("oslon", osX);
  aof.setParam("ptlat", ptY);
  aof.setParam("ptlon", ptX);
  aof.setParam("desired_speed", ptSPD);
  aof.initialize();
  
  OF_Reflector reflector(&aof, 1);
  reflector.create(m_build_info);
  of = reflector.extractOF();

  of->setPWT(m_priority_wt);

#if 0
  IvPBox mpt = of->getPDMap()->getGrid()->getMaxPt();
  cout << "BHV_WaypointTOL::produceOF():" << endl;
  cout << "maxpt:" << endl;
  mpt.print();
#endif

  if(osSPD > 0) {
    double dist_meters = hypot((osX-ptX), (osY-ptY));
    double eta_seconds = dist_meters / osSPD;

    string stat = "vname=" + m_us_name + ",";
    stat += "index=" + intToString(current_waypt)   + ",";
    stat += "dist="  + doubleToString(dist_meters)  + ",";
    stat += "eta="   + doubleToString(eta_seconds);
    
    postMessage("VEHICLE_WPT_STAT_US", stat);
    postMessage("VEHICLE_WPT_STAT", stat);
    postMessage("VEHICLE_WPT_INDEX", current_waypt);
  }

  //string str = IvPFunctionToString(of);
  //postMessage("TEST_OF", str);

  return(of);
}

//-----------------------------------------------------------
// Procedure: setNextWaypoint

bool BHV_WaypointTOL::setNextWaypoint()
{
  bool ok1, ok2, ok3;
  osX   = m_info_buffer->dQuery("NAV_X",     ok1);
  osY   = m_info_buffer->dQuery("NAV_Y",     ok2);
  osSPD = m_info_buffer->dQuery("NAV_SPEED", ok3);

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2) {
    postEMessage("No ownship X/Y info in info_buffer.");
    return(false);
  }

  if(!ok3) {
    postWMessage("No ownship speed info in info_buffer.");
    osSPD = -1;
  }

  int vsize = seglist.size();
  if(vsize == 0)
    return(false);

  // If no more waypoints to hit, post the EndFlags, and
  // do not generate function.
  if(current_waypt >= vsize) {
    if(repeat <= 0) {
      return(false);
    }
    else {
      repeat--;
      current_waypt = 0;
    }
  }

  ptX  = seglist.get_vx(current_waypt);
  ptY  = seglist.get_vy(current_waypt);
  double dist = hypot((osX-ptX),(osY-ptY));

  if(current_waypt < segspd.size())
    ptSPD = segspd[current_waypt];
  else
    ptSPD = cruise_speed;
  
  if(dist < current_cpa)
    current_cpa = dist;

  bool next_point = false;
  if(dist < arrival_radius)
    next_point = true;
  else {
    if(non_mono_active) {
      if(dist >= (current_cpa + non_mono_thresh))
	next_point = true;
    }
  }

  if(next_point) {
    if(current_waypt < (vsize-1)) {
      current_waypt++;
      ptX = seglist.get_vx(current_waypt);
      ptY = seglist.get_vy(current_waypt);
      current_cpa = hypot((osX-ptX),(osY-ptY));
    }
    else {
      postMessage("VEHICLE_WPT_STAT_US", "complete");
      postMessage("VEHICLE_WPT_STAT", "complete");
      postFlags(m_end_flags);
      m_completed = true;
      return(false);
    }
  }

  // By default, the steering point is the next waypoint.
  iptX = ptX;
  iptY = ptY;

  // If lead_distance is non-neg, and we've already hit the
  // first waypoint, then steer to an intermediate point that
  // is lead_distance away from the perpendicular intersection
  // point between the current position and the trackline.

  if(lead_distance >= 0) {
    if(current_waypt > 0) {
      double x1 = seglist.get_vx(current_waypt-1);
      double y1 = seglist.get_vx(current_waypt-1);

      double nx, ny;
      perpSegIntPt(x1,y1,ptX,ptY,osX,osY,nx,ny);

      double angle = relAng(x1,y1,ptX,ptY);
      double dist  = distPointToPoint(nx, ny, ptX, ptY);
      if(dist > lead_distance) 
	dist = lead_distance; 
      projectPoint(angle, dist, nx, ny, iptX, iptY);
    }
  }
    
#if 0
  cout << "   ptX:" <<  ptX << "  ptY:" << ptY  << endl;
  cout << "  iptX:" << iptX << " iptY:" << iptY << endl;
#endif

  return(true);
}








