/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Waypoint1D.cpp                                   */
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
#include "IO_Utilities.h"
#include "BHV_Waypoint1D.h"
#include "AOF_WPT1D.h"
#include "OF_Reflector.h"
#include "IvPDomain.h"
#include "MBUtils.h"
#include "AngleUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Waypoint1D::BHV_Waypoint1D(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setDescriptor("(d)NULL3D");
  this->setParam("unifbox", "course=10");
  this->setParam("gridbox", "course=10");

  current_waypt  = 0;
  arrival_radius = 20; // Meters
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition,tag" where the tag is an optional
//            descriptor of the waypoint.
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_Waypoint1D::setParam(string param, string val) 
{
  if(IvPBehavior::setParamCommon(param, val))
    return(true);

  if(param == "waypoint") {
    vector<string> svector = parseString(val, ',');
    if(svector.size() >= 2) {
      x_waypt.push_back(atof(svector[0].c_str()));
      y_waypt.push_back(atof(svector[1].c_str()));
    }
    if(svector.size() == 3) {
      tag_waypt.push_back(svector[2]);
    }
    return(true);
  }
  
  if(param == "radius") {
    arrival_radius = atof(val.c_str());
    return(arrival_radius >= 0);
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: produceOF

OF *BHV_Waypoint1D::produceOF() 
{
  // clear each time produceOF() is called
  messages.clear();

  if(!unif_box || !grid_box) {
    postEMessage("Null UnifBox or GridBox.");
    return(0);
  }

  // If no more waypoints to hit, do not generate function.
  if(current_waypt >= x_waypt.size())
    return(0);

  bool ok;
  double osX   = info_buffer->dQuery("NAV_X",     &ok);
  double osY   = info_buffer->dQuery("NAV_Y",     &ok);
  double osSPD = info_buffer->dQuery("NAV_SPEED", &ok);

  cout << "+++++BHV_Waypoint1D:produceOF() " << endl;
  cout << "  osX:" << osX << " osY:" << osY << endl;

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
    else
      return(0);
  }

  cout << "  ptX:" << ptX << " ptY:" << ptY << endl;
  double ang1 = relAng(osX, osY, ptX, ptY);
  double ang2 = ang1;
  if(ang2 > 180) 
    ang2 -= 360;

  AOF_WPT1D *aof_wpt = new AOF_WPT1D(domain, osY, osX, ptY, ptX);
  OF_Reflector *ofr_wpt = new OF_Reflector(aof_wpt, 1);

  ofr_wpt->createUniform(unif_box, grid_box);

  OF *of = ofr_wpt->extractOF();
  delete(ofr_wpt);

  of->setDomainName(0, "course");
  of->setPWT(priority_wt);

#if 1
  Box *mpt = of->getPDMap()->getGrid()->getMaxPt();
  cout << "BHV_Waypoint1D::produceOF():" << endl;
  cout << "maxpt:" << endl;
  mpt->print();
  delete(mpt);
#endif

  double dist_meters = hypot((osX-ptX), (osY-ptY));
  double eta_seconds = dist_meters / osSPD;

  string stat = "vname=" + us_name + ",";
  stat += "index=" + intToString(current_waypt)   + ",";
  stat += "dist="  + doubleToString(dist_meters)  + ",";
  stat += "eta="   + doubleToString(eta_seconds);

  VarDataPair msg("VEHICLE_WPT_STAT_US", stat);
  messages.push_back(msg);
  
  msg.set("VEHICLE_WPT_INDEX_US", current_waypt);
  messages.push_back(msg);

  return(of);
}






