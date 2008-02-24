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
  this->setParam("descriptor", "(d)bhv_avoid_obstacles");
  this->setParam("build_info", "uniform_piece=course:1,speed:2");
  this->setParam("build_info", "uniform_grid=course:9,speed:6");

  m_domain = subDomain(m_domain, "course,speed");

  aof_avoid = new AOF_AvoidObstacles(m_domain);

  addInfoVars("NAV_X, NAV_Y");
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition".
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_AvoidObstacles::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  if((param == "polygon") || (param == "points")) {
    XYPolygon new_polygon;
    bool ok = new_polygon.initialize(val);
    if(!ok)
      return(false);
    obstacles.push_back(new_polygon);
    aof_avoid->setParam("polygon", val);
    return(true);
  }
  else if(param == "allowable_ttc") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    aof_avoid->setParam("allowable_ttc", dval);
    return(true);
  }
  else if(param == "activation_dist") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    aof_avoid->setParam("activation_dist", dval);
    return(true);
  }
  else if(param == "buffer_dist") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    aof_avoid->setParam("buffer_dist", dval);
    return(true);
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_AvoidObstacles::onRunState() 
{
  bool ok1, ok2;
  double os_x = getBufferDoubleVal("NAV_X", ok1);
  double os_y = getBufferDoubleVal("NAV_Y", ok2);

  if(!ok1 || !ok2) {
    postEMessage("No Ownship NAV_X and/or NAV_Y in info_buffer");
    return(false);
  }

  if(!aof_avoid) {
    postEMessage("AOF Not properly set in BHV_AvoidObstacles");
    return(false);
  }

  for(int i=0; i<obstacles.size(); i++)
    postMessage("VIEW_POLYGON", obstacles[i].get_spec());

  aof_avoid->setParam("os_x", os_x);
  aof_avoid->setParam("os_y", os_y);

  if(aof_avoid->obstaclesInRange() == 0)
    return(0);

  bool ok_init = aof_avoid->initialize();
  if(!ok_init) {
    postEMessage("BHV_AvoidObstacles: AOF-Init Error");
    return(0);
  }
  IvPFunction *ipf = 0;

  OF_Reflector reflector(aof_avoid, 1);
  reflector.create(m_build_info);
  ipf = reflector.extractOF(false);
  
  ipf->setPWT(m_priority_wt);

#if 0
  IvPBox mpt = ipf->getPDMap()->getGrid()->getMaxPt();
  cout << "BHV_AvoidObstacles::produceOF():" << endl;
  cout << "maxpt:" << endl;
  mpt.print();
#endif

  return(ipf);
}
