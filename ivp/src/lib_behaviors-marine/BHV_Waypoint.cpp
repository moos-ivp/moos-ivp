/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Waypoint.cpp                                     */
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
#include "BHV_Waypoint.h"
#include "OF_Reflector.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BuildUtils.h"
#include "FunctionEncoder.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Waypoint::BHV_Waypoint(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_waypoint");
  domain = subDomain(domain, "course,speed");

  cruise_speed    = 0;  // Meters/second
  lead_distance   = -1;

  // The completed and perpetual vars are initialized in superclass
  // but we initialize here just to be safe and clear.
  completed       = false; 
  perpetual       = false;

  osX   = -1;
  osY   = -1;
  osSPD = -1;
  ptX   = -1;
  ptY   = -1;
  iptX  = -1;
  iptY  = -1;

  info_vars.push_back("NAV_X");
  info_vars.push_back("NAV_Y");
  info_vars.push_back("NAV_SPEED");
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition".
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_Waypoint::setParam(string param, string val) 
{
  if(IvPBehavior::setParamCommon(param, val))
    return(true);

  if((param == "polygon") || (param == "points")) {
    XYSegList new_seglist;
    bool ok = new_seglist.initialize(val);
    if(!ok)
      return(false);
    if(!param_lock)
      waypoint_engine.setSegList(new_seglist);
    return(true);
  }
  else if(param == "speed") {
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    if(!param_lock) {
      cruise_speed = dval;
    }
    return(true);
  }
  else if(param == "lead") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    if(!param_lock)
      lead_distance = dval;
    return(true);
  }
  else if(param == "order") {
    if((val!="reverse") && (val!="reversed") && (val!="normal"))
      return(false);
    if(!param_lock) {
      bool reverse = ((val == "reverse") || (val == "reversed"));
      waypoint_engine.setReverse(reverse);
    }
    return(true);
  }
  else if(param == "repeat") {
    int ival = atoi(val.c_str());
    if((ival < 0) || (!isNumber(val)))
      return(false);
    if(!param_lock)
      waypoint_engine.setRepeat(ival);
    return(true);
  }
  else if(param == "radius") {
    double dval = atof(val.c_str());
    if(dval <= 0)
      return(false);
    if(!param_lock)
      waypoint_engine.setCaptureRadius(dval);
    return(true);
  }
  else if(param == "nm_radius")  {
    double dval = atof(val.c_str());
    if(dval <= 0) 
      return(false);
    if(!param_lock)
      waypoint_engine.setNonmonotonicRadius(dval);
    return(true);
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: produceOF

IvPFunction *BHV_Waypoint::produceOF() 
{
  // Set osX, osY, osSPD
  if(!updateInfoIn())
    return(0);

  // Set ptX, ptY, iptX, iptY;
  if(!setNextWaypoint())
    return(0);
  
  IvPFunction *ipf = buildOF("zaic");
  if(ipf)
    ipf->setPWT(priority_wt);

  updateInfoOut();

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: updateInfoIn()
//   Purpose: Update info need by the behavior from the info_buffer.
//            Error or warning messages can be posted.
//   Returns: true if no vital info is missing from the info_buffer.
//            false otherwise.
//      Note: By posting an EMessage, this sets the state_ok member
//            variable to false which will communicate the gravity
//            of the situation to the helm.

bool BHV_Waypoint::updateInfoIn()
{
  bool ok1, ok2, ok3;
  osX   = info_buffer->dQuery("NAV_X",     ok1);
  osY   = info_buffer->dQuery("NAV_Y",     ok2);
  osSPD = info_buffer->dQuery("NAV_SPEED", ok3);

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2) {
    postEMessage("No ownship X/Y info in info_buffer.");
    return(false);
  }
  if(!ok3)
    postWMessage("No ownship speed info in info_buffer.");

  return(true);
}


//-----------------------------------------------------------
// Procedure: setNextWaypoint

bool BHV_Waypoint::setNextWaypoint()
{
  waypoint_engine.setNextWaypoint(osX, osY);

  if(waypoint_engine.isComplete()) {
    postMessage("VEHICLE_WPT_STAT", "complete");
    setComplete();
    return(false);
  }
  
  ptX   = waypoint_engine.getPointX();
  ptY   = waypoint_engine.getPointY();
  
  // By default, the steering point is the next waypoint.
  iptX = ptX;
  iptY = ptY;

  // If lead_distance is non-neg, and we've already hit the
  // first waypoint, then steer to an intermediate point that
  // is lead_distance away from the perpendicular intersection
  // point between the current position and the trackline.
  
  int current_waypt = waypoint_engine.getCurrIndex();
  if(lead_distance >= 0) {
    if(current_waypt > 0) {
      double x1 = waypoint_engine.getPointX(current_waypt-1);
      double y1 = waypoint_engine.getPointY(current_waypt-1);

      double nx, ny;
      perpSegIntPt(x1,y1,ptX,ptY,osX,osY,nx,ny);

      double angle = relAng(x1,y1,ptX,ptY);
      double dist  = distPointToPoint(nx, ny, ptX, ptY);
      if(dist > lead_distance) 
	dist = lead_distance; 
      projectPoint(angle, dist, nx, ny, iptX, iptY);
      string str = "radial:" + doubleToString(iptX,1);
      str += "," + doubleToString(iptY,1) + ",2,6";
      postMessage("INFO_POLYGON", str);
    }
  }
  return(true);
}


//-----------------------------------------------------------
// Procedure: buildOF

IvPFunction *BHV_Waypoint::buildOF(string method) 
{
  IvPFunction *ipf = 0;

  if(method == "zaic") {
    ZAIC_PEAK spd_zaic(domain, "speed");
    spd_zaic.setSummit(cruise_speed);
    spd_zaic.setBaseWidth(2.6);
    spd_zaic.setPeakWidth(0.0);
    spd_zaic.setSummitDelta(0.0);
    IvPFunction *spd_of = spd_zaic.extractOF();

    double rel_ang_to_wpt = relAng(osX, osY, iptX, iptY);
    ZAIC_PEAK crs_zaic(domain, "course");
    crs_zaic.setSummit(rel_ang_to_wpt);
    crs_zaic.setBaseWidth(180.0);
    crs_zaic.setValueWrap(true);
    IvPFunction *crs_of = crs_zaic.extractOF();

    OF_Coupler coupler;
    ipf = coupler.couple(crs_of, spd_of);
  }    

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: updateInfoOut()

void BHV_Waypoint::updateInfoOut()
{
  if(osSPD > 0) {
    int    current_waypt = waypoint_engine.getCurrIndex();
    double dist_meters   = hypot((osX-ptX), (osY-ptY));
    double eta_seconds   = dist_meters / osSPD;

    string stat = "vname=" + us_name + ",";
    stat += "index=" + intToString(current_waypt)   + ",";
    stat += "dist="  + doubleToString(dist_meters)  + ",";
    stat += "eta="   + doubleToString(eta_seconds);
    
    postMessage("VEHICLE_WPT_STAT_LOCAL", stat);
    postMessage("VEHICLE_WPT_INDEX", current_waypt);
  }

  XYSegList seglist = waypoint_engine.getSegList();
  postMessage("VIEW_POLYGON", seglist.get_spec());

  if(waypoint_engine.currPtChanged()) {
    string ptmsg;
    ptmsg += doubleToString(ptX,2) + ",";
    ptmsg += doubleToString(ptY,2) + ",5," + getDescriptor();
    postMessage("VIEW_POINT", ptmsg);
  }
}

