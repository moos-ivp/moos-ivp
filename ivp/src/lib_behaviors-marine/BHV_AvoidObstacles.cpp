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
#include <stdlib.h>
#include "BHV_AvoidObstacles.h"
#include "OF_Reflector.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BuildUtils.h"
#include "XYBuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_AvoidObstacles::BHV_AvoidObstacles(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_avoid_obstacles");
  //this->setParam("build_info", "uniform_piece=discrete@course:2,speed:3");
  //this->setParam("build_info", "uniform_grid =discrete@course:9,speed:6");

  m_domain = subDomain(m_domain, "course,speed");

  m_buffer_dist     = 0;
  m_activation_dist = -1;
  m_allowable_ttc   = 20;

  m_aof_avoid = new AOF_AvoidObstacles(m_domain);

  addInfoVars("NAV_X, NAV_Y");
}

//-----------------------------------------------------------
// Procedure: Destructor

BHV_AvoidObstacles::~BHV_AvoidObstacles()
{
  if(m_aof_avoid)
    delete(m_aof_avoid);
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
  
  if((param=="polygon") || (param=="points") || (param=="poly")) {
    XYPolygon new_polygon = stringToPoly(val);
    if(!new_polygon.is_convex())
      return(false);
    m_aof_avoid->addObstacle(new_polygon);
    return(true);
  }
  else if(param == "allowable_ttc") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    m_allowable_ttc = dval;
    return(true);
  }
  else if(param == "activation_dist") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    m_activation_dist = dval;
    return(true);
  }
  else if(param == "buffer_dist") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    m_buffer_dist = dval;
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
    postWMessage("No Ownship NAV_X and/or NAV_Y in info_buffer");
    return(false);
  }

  if(!m_aof_avoid) {
    postWMessage("AOF Not properly set in BHV_AvoidObstacles");
    return(false);
  }
  
  for(int i=0; i<m_aof_avoid->size(); i++) {
    string spec_orig = m_aof_avoid->getObstacleSpec(i,false);
    string spec_buff = m_aof_avoid->getObstacleSpec(i,true);
    postMessage("VIEW_POLYGON", spec_orig, "orig");
    postMessage("VIEW_POLYGON", spec_buff, "buff");
  }
  
  if(m_aof_avoid->objectInObstacle(os_x, os_y, false))
    postWMessage("Ownship position within stated space of obstacle");
  
  if(m_aof_avoid->objectInObstacle(os_x, os_y, true))
    postWMessage("Ownship position within stated BUFFER space of obstacle");
  
  m_aof_avoid->setParam("os_x", os_x);
  m_aof_avoid->setParam("os_y", os_y);
  m_aof_avoid->setParam("buffer_dist", m_buffer_dist);
  m_aof_avoid->setParam("activation_dist", m_activation_dist);
  m_aof_avoid->setParam("allowable_ttc", m_allowable_ttc);

  if(m_aof_avoid->obstaclesInRange() == 0)
    return(0);

  bool ok_init = m_aof_avoid->initialize();
  if(!ok_init) {
    postWMessage("BHV_AvoidObstacles: AOF-Init Error");
    return(0);
  }

  IvPFunction *ipf = 0;

  OF_Reflector reflector(m_aof_avoid, 1);

  if(m_build_info != "")
    reflector.create(m_build_info);
  else {
    reflector.setParam("uniform_piece", "discrete@course:3,speed:3");
    reflector.setParam("uniform_grid",  "discrete@course:6,speed:6");
    reflector.create();
  }

  if(reflector.hasErrors())
    postWMessage(reflector.getErrors());
  else {
    ipf = reflector.extractOF(true); // true means normalize [0,100]
    if(ipf)
      ipf->setPWT(m_priority_wt);
  }
  
  return(ipf);
}

