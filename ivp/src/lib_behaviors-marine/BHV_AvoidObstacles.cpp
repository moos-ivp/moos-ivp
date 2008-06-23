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
  this->setParam("build_info", "uniform_piece=discrete@course:1,speed:1");
  this->setParam("build_info", "uniform_grid =discrete@course:9,speed:6");

  m_domain = subDomain(m_domain, "course,speed");

  m_buffer_dist     = 0;
  m_activation_dist = -1;
  m_allowable_ttc   = 20;

  m_aof_avoid = new AOF_AvoidObstacles(m_domain);

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

  if((param=="polygon") || (param=="points") || (param=="poly")) {
    XYPolygon new_polygon = stringToPoly(val);
    if(!new_polygon.is_convex())
      return(false);
    addNewObstacle(new_polygon);
    m_aof_avoid->setParam("polygon", val);
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
    postEMessage("No Ownship NAV_X and/or NAV_Y in info_buffer");
    return(false);
  }

  if(!m_aof_avoid) {
    postEMessage("AOF Not properly set in BHV_AvoidObstacles");
    return(false);
  }

  unsigned int i;
  for(i=0; i<m_obstacles_orig.size(); i++)
    postMessage("VIEW_POLYGON", m_obstacles_orig[i].get_spec());
  
  AOF_AvoidObstacles aof_avoid(m_domain);
  
  for(i=0; i<m_obstacles_orig.size(); i++)
    if(m_obstacles_orig[i].contains(os_x, os_y))
      postEMessage("Ownship position within stated space of obstacle");
  
  for(i=0; i<m_obstacles_buff.size(); i++)
    if(m_obstacles_buff[i].contains(os_x, os_y))
      postWMessage("Ownship position within stated BUFFER space of obstacle");
  

  for(i=0; i<m_obstacles_buff.size(); i++)
    aof_avoid.addObstacle(m_obstacles_buff[i]);

  aof_avoid.setParam("os_x", os_x);
  aof_avoid.setParam("os_y", os_y);
  aof_avoid.setParam("buffer_dist", m_buffer_dist);
  aof_avoid.setParam("activation_dist", m_activation_dist);
  aof_avoid.setParam("allowable_ttc", m_allowable_ttc);

  if(aof_avoid.obstaclesInRange() == 0)
    return(0);

  bool ok_init = aof_avoid.initialize();
  if(!ok_init) {
    postEMessage("BHV_AvoidObstacles: AOF-Init Error");
    return(0);
  }

  IvPFunction *ipf = 0;

  OF_Reflector reflector(&aof_avoid, 1);
  reflector.create(m_build_info);
  if(reflector.hasErrors())
    postWMessage(reflector.getErrors());
  else {
    ipf = reflector.extractOF(true); // true means normalize [0,100]
    ipf->setPWT(m_priority_wt);
  }
  
  return(ipf);
}


//-----------------------------------------------------------
// Procedure: addNewPolygon
//      Note: The label of the new polygon is checked against the 
//            labels of existing polygons. If the label matches then
//            new polygon *replaces* the previously added polygon 
//            of the same label.
//      Note: If the label of the new polygon is the empty string, 
//            then it is assumed to be unique and is added without
//            any checking.

void BHV_AvoidObstacles::addNewObstacle(const XYPolygon& newpoly_orig)
{
  XYPolygon newpoly_buff = newpoly_orig;
  if(m_buffer_dist > 0)
    newpoly_buff.grow_by_amt(m_buffer_dist);

  
  string newlabel = newpoly_orig.get_label();
  bool unique_label = true;
  
  if(newlabel != "") {
    unsigned int i;
    unsigned int vsize = m_obstacles_orig.size();
    for(i=0; i<vsize; i++) {
      if(m_obstacles_orig[i].get_label() == newlabel) {
	unique_label = false;
	m_obstacles_orig[i] = newpoly_orig;
	m_obstacles_buff[i] = newpoly_buff;
      }
    }
  }
  
  if(unique_label) {
    m_obstacles_orig.push_back(newpoly_orig);
    m_obstacles_buff.push_back(newpoly_buff);
  }
}
    


  
