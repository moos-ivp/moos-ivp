/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
#include <cmath> 
#include <cstdlib>
#include "BHV_AvoidObstacles.h"
#include "OF_Reflector.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BuildUtils.h"
#include "XYFormatUtilsPoly.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_AvoidObstacles::BHV_AvoidObstacles(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "avoid_obstacles");
  //this->setParam("build_info", "uniform_piece=discrete@course:2,speed:3");
  //this->setParam("build_info", "uniform_grid =discrete@course:9,speed:6");

  m_domain = subDomain(m_domain, "course,speed");

  m_buffer_dist        = 0;
  m_activation_dist    = -1;
  m_allowable_ttc      = 20;
  m_pheading_influence = 10;

  m_hint_buff_edge_color = "yellow";
  m_hint_obst_edge_color = "red";

  m_aof_avoid = new AOF_AvoidObstacles(m_domain);

  addInfoVars("NAV_X, NAV_Y, NAV_HEADING");
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
    XYPolygon new_polygon = string2Poly(val);
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
  else if(param == "pheading_influence") {
    double dval = atof(val.c_str());
    if((dval < 0) || (dval > 100) || (!isNumber(val)))
      return(false);
    m_pheading_influence = dval;
    return(true);
  }
  else if(param == "buffer_dist") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    m_buffer_dist = dval;
    return(true);
  }
  else if(param == "visual_hints")  {
    vector<string> svector = parseStringQ(val, ',');
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++) 
      handleVisualHint(svector[i]);
    return(true);
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_AvoidObstacles::onRunState() 
{
  bool ok1, ok2, ok3;
  double os_x   = getBufferDoubleVal("NAV_X", ok1);
  double os_y   = getBufferDoubleVal("NAV_Y", ok2);
  double os_hdg = getBufferDoubleVal("NAV_HEADING", ok3);

  if(!ok1 || !ok2) {
    postWMessage("No Ownship NAV_X and/or NAV_Y in info_buffer");
    return(false);
  }

  if(!ok3) {
    postWMessage("No Ownship NAV_HEADING in info_buffer");
    return(false);
  }

  if(!m_aof_avoid) {
    postWMessage("AOF Not properly set in BHV_AvoidObstacles");
    return(false);
  }
  
  if(m_aof_avoid->objectInObstacle(os_x, os_y, false))
    postWMessage("Ownship position within stated space of obstacle");
  
  if(m_aof_avoid->objectInObstacle(os_x, os_y, true))
    postWMessage("Ownship position within stated BUFFER space of obstacle");
  
  m_aof_avoid->setParam("os_x", os_x);
  m_aof_avoid->setParam("os_y", os_y);
  m_aof_avoid->setParam("present_heading", os_hdg);
  m_aof_avoid->setParam("present_heading_influence", m_pheading_influence);
  m_aof_avoid->setParam("buffer_dist", m_buffer_dist);
  m_aof_avoid->setParam("activation_dist", m_activation_dist);
  m_aof_avoid->setParam("allowable_ttc", m_allowable_ttc);

  bool ok_init = m_aof_avoid->initialize();
  if(!ok_init) {
    postWMessage("BHV_AvoidObstacles: AOF-Init Error");
    return(0);
  }

  postViewablePolygons();
  
  int pertinent_obstacles = m_aof_avoid->pertObstacleCount();
  if(pertinent_obstacles == 0) {
    postWMessage("BHV_AvoidObstacles: No pertinent obstacles");
    return(0);
  }

  if(m_aof_avoid->obstaclesInRange() == 0)
    return(0);
  
  IvPFunction *ipf = 0;
  OF_Reflector reflector(m_aof_avoid, 1);

  if(m_build_info != "")
    reflector.create(m_build_info);
  else {
    reflector.setParam("uniform_piece", "discrete@course:3,speed:3");
    reflector.setParam("uniform_grid",  "discrete@course:6,speed:6");
    reflector.create();
  }

  if(!reflector.stateOK())
    postWMessage(reflector.getWarnings());
  else {
    ipf = reflector.extractIvPFunction(true); // true means normalize [0,100]
    if(ipf)
      ipf->setPWT(m_priority_wt);
  }
  
  return(ipf);
}

//-----------------------------------------------------------
// Procedure: handleVisualHint()

void BHV_AvoidObstacles::handleVisualHint(string hint)
{
  string param = tolower(stripBlankEnds(biteString(hint, '=')));
  string value = stripBlankEnds(hint);

  if((param == "obstacle_edge_color") && isColor(value))
    m_hint_obst_edge_color = value;
  else if((param == "buffer_edge_color") && isColor(value))
    m_hint_obst_edge_color = value;
}

//-----------------------------------------------------------
// Procedure: postViewablePolygons

void BHV_AvoidObstacles::postViewablePolygons()
{
  if(!m_aof_avoid)
    return;
  
  unsigned int i;
  for(i=0; i<m_aof_avoid->size(); i++) {
    string spec_orig = m_aof_avoid->getObstacleSpec(i,false,true);
    string spec_buff = m_aof_avoid->getObstacleSpec(i,true, true);

    double range = m_aof_avoid->rangeToObstacle(i);
    if(range > m_activation_dist) {
      spec_orig += ":edge_color,white:edge_color,white";
      postMessage("VIEW_POLYGON", spec_orig, "orig");
    }
    else {
      spec_orig += ":edge_color," + m_hint_obst_edge_color;
      spec_orig += ":vertex_color," + m_hint_obst_edge_color;
      spec_buff += ":edge_color," + m_hint_buff_edge_color;
      spec_buff += ":vertex_color," + m_hint_buff_edge_color;
      postMessage("VIEW_POLYGON", spec_orig, "orig");
      postMessage("VIEW_POLYGON", spec_buff, "buff");
    }
  }
}


//-----------------------------------------------------------
// Procedure: postErasablePolygons

void BHV_AvoidObstacles::postErasablePolygons()
{
  if(!m_aof_avoid)
    return;
  
  unsigned int i, vsize = m_aof_avoid->size();
  for(i=0; i<vsize; i++) {
    string spec_orig = m_aof_avoid->getObstacleSpec(i, false, false);
    string spec_buff = m_aof_avoid->getObstacleSpec(i, true, false);

    postMessage("VIEW_POLYGON", spec_orig, "orig");
    postMessage("VIEW_POLYGON", spec_buff, "buff");
  }
}



