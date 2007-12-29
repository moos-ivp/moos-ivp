/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Waydpoint2D.cpp                                  */
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

#include <string>
#include <iostream>
#include <math.h> 
#include <assert.h> 
#include "IO_Utilities.h"
#include "BHV_Waypoint2D.h"
#include "AOF_WPT2D.h"
#include "OF_Reflector.h"
#include "IvPDomain.h"
#include "MBUtils.h"
#include "AngleUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Waypoint2D::BHV_Waypoint2D(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setDescriptor("(d)waypoint1D");
  this->setParam("build_info", "uniform_box=course:2,speed:1");
  this->setParam("build_info", "uniform_grid=course:10,speed:5");

  current_waypt  = 0;
  arrival_radius = 10; // Meters
  cruise_speed   = 0;  // Meters/second
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition,tag" where the tag is an optional
//            descriptor of the waypoint.
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_Waypoint2D::setParam(string param, string val) 
{
  if(IvPBehavior::setParamCommon(param, val))
    return(true);

  if(param == "waypoint") {
    vector<string> svector = parseString(val, ',');
    if(svector.size() == 2) {
      x_waypt.push_back(atof(svector[0].c_str()));
      y_waypt.push_back(atof(svector[1].c_str()));
      string label = "WP#" + intToString(x_waypt.size());
      tag_waypt.push_back(label);
    }
    if(svector.size() == 3) {
      x_waypt.push_back(atof(svector[0].c_str()));
      y_waypt.push_back(atof(svector[1].c_str()));
      tag_waypt.push_back(svector[2]);
    }
    return(true);
  }
  
  if(param == "speed") {
    cruise_speed = atof(val.c_str());
    return(cruise_speed > 0);
  }
  if(param == "radius") {
    arrival_radius = atof(val.c_str());
    return(arrival_radius >= 0);
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: onRunState

OF *BHV_Waypoint2D::onRunState() 
{
  bool silent = true;

  // If no more waypoints to hit, do not generate function.
  if(current_waypt >= x_waypt.size())
    return(0);
  //    current_waypt = 0;

  bool ok;
  double osX   = getBufferDoubleVal("NAV_X",     &ok);
  double osY   = getBufferDoubleVal("NAV_Y",     &ok);
  double osSPD = getBufferDoubleVal("NAV_SPEED", &ok);

  if(!silent) cout << "+++++BHV_Waypoint2D::produceOF() " << endl;
  if(!silent) cout << "  osX:" << osX << " osY:" << osY << endl;

  // Must get ownship position from InfoBuffer
  if(!ok) {
    postEMessage("No ownship info in info_buffer.");
    return(0);
  }

  double ptX = x_waypt[current_waypt];
  double ptY = y_waypt[current_waypt];

  if(hypot((osX-ptX),(osY-ptY)) < arrival_radius) {
    if(current_waypt < (x_waypt.size()-1)) {
      current_waypt++;
      ptX = x_waypt[current_waypt];
      ptY = y_waypt[current_waypt];
    }
    else {
      VarDataPair msg("VEHICLE_WPT_STAT_US", "complete");
      m_messages.push_back(msg);
      return(0);
    }
  }

  if(!silent) cout << "  ptX:" << ptX << " ptY:" << ptY << endl;

  AOF_WPT2D *aof_wpt = new AOF_WPT2D(domain, cruise_speed, osY, osX, ptY, ptX);
  OF_Reflector *ofr_wpt = new OF_Reflector(aof_wpt, 1);

  ofr_wpt->create(m_build_info);
  OF *of = ofr_wpt->extractOF();

  delete(ofr_wpt);

  of->setDomainName(0, "course");
  of->setDomainName(1, "speed");
  of->setPWT(m_priority_wt);

  if(!silent) {
    Box *mpt = of->getPDMap()->getGrid()->getMaxPt();
    cout << "BHV_Waypoint2D::produceOF():" << endl;
    cout << "maxpt:" << endl;
    mpt->print();
    delete(mpt);
  }

  double dist_meters = hypot((osX-ptX), (osY-ptY));
  double eta_seconds = dist_meters / osSPD;

  string stat = "vname=" + m_us_name + ",";
  stat += "index=" + intToString(current_waypt)   + ",";
  stat += "dist="  + doubleToString(dist_meters)  + ",";
  stat += "eta="   + doubleToString(eta_seconds);

  VarDataPair msg("VEHICLE_WPT_STAT_US", stat);
  m_messages.push_back(msg);
  
  msg.set("VEHICLE_WPT_INDEX_US", current_waypt);
  m_messages.push_back(msg);

  return(of);
}






