/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_AvoidObstacles.cpp                               */
/*    DATE: Aug 2nd 2006                                         */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
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
  m_pheading_influence = 90;

  m_hint_obst_edge_color   = "white";
  m_hint_obst_vertex_color = "dodger_blue";
  m_hint_obst_fill_color   = "gray60";
  m_hint_obst_fill_transparency = 0.7;

  m_hint_buff_edge_color   = "gray60";
  m_hint_buff_vertex_color = "dodger_blue";
  m_hint_buff_fill_color   = "gray70";
  m_hint_buff_fill_transparency = 0.1;

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
  else if(param == "visual_hints")
    return(handleVisualHints(val));
  
  return(false);
}


//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_AvoidObstacles::onIdleState()
{
  postErasablePolygons();
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_AvoidObstacles::onRunState() 
{
  // Part 1: Sanity checks
  bool ok1, ok2, ok3;
  double os_x   = getBufferDoubleVal("NAV_X", ok1);
  double os_y   = getBufferDoubleVal("NAV_Y", ok2);
  double os_hdg = getBufferDoubleVal("NAV_HEADING", ok3);

  if(!ok1 || !ok2) {
    postWMessage("No Ownship NAV_X and/or NAV_Y in info_buffer");
    return(0);
  }
  if(!ok3) {
    postWMessage("No Ownship NAV_HEADING in info_buffer");
    return(0);
  }
  if(!m_aof_avoid) {
    postWMessage("AOF Not properly set in BHV_AvoidObstacles");
    return(0);
  }

  // Part 2: Build the underlying objective function and initialize
  m_aof_avoid->setParam("os_x", os_x);
  m_aof_avoid->setParam("os_y", os_y);
  m_aof_avoid->setParam("os_h", os_hdg);
  m_aof_avoid->setParam("buffer_dist", m_buffer_dist);
  m_aof_avoid->setParam("activation_dist", m_activation_dist);
  m_aof_avoid->setParam("allowable_ttc", m_allowable_ttc);

  // Check if ownship violates either an obstacle or obstacle+buffer. 
  // Do this before init, because init includes buffer shrinking.
  if(m_aof_avoid->ownshipInObstacle(false))
    postWMessage("Ownship position within stated space of obstacle");
  if(m_aof_avoid->ownshipInObstacle(true))
    postWMessage("Ownship position within stated BUFFER space of obstacle");

  bool ok_init = m_aof_avoid->initialize();
  if(!ok_init) {
    postWMessage("BHV_AvoidObstacles: AOF-Init Error");
    return(0);
  }

  // Part 3: Post the Visuals
  postViewablePolygons();
  
  // Par 4: Build the actual objective function
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
// Procedure: handleVisualHints()

bool BHV_AvoidObstacles::handleVisualHints(string hints)
{
  vector<string> svector = parseStringQ(hints, ',');

  for(unsigned int i=0; i<svector.size(); i++) {

    string hint = svector[i];
    string param = tolower(biteStringX(hint, '='));
    string value = hint;
    
    if((param == "obstacle_edge_color") && isColor(value))
      m_hint_obst_edge_color = value;
    else if((param == "obstacle_vertex_color") && isColor(value))
      m_hint_obst_vertex_color = value;
    else if((param == "obstacle_fill_color") && isColor(value))
      m_hint_obst_fill_color = value;
    else if((param == "obstacle_fill_transparency") && isNumber(value)) {
      double transparency = atof(value.c_str());
      transparency = vclip(transparency, 0, 1);
      m_hint_obst_fill_transparency = transparency;
    }
    
    else if((param == "buffer_edge_color") && isColor(value))
      m_hint_buff_edge_color = value;
    else if((param == "buffer_vertex_color") && isColor(value))
      m_hint_buff_vertex_color = value;
    else if((param == "buffer_fill_color") && isColor(value))
      m_hint_buff_fill_color = value;
    else if((param == "buffer_fill_transparency") && isNumber(value)) {
      double transparency = atof(value.c_str());
      transparency = vclip(transparency, 0, 1);
      m_hint_buff_fill_transparency = transparency;
    }
    else
      return(false);
  }
  return(true);
}


//-----------------------------------------------------------
// Procedure: postViewablePolygons

void BHV_AvoidObstacles::postViewablePolygons()
{
  if(!m_aof_avoid)
    return;
  
  // Part 1: Handle the original obstacles (minus the buffer)
  for(unsigned int i=0; i<m_aof_avoid->size(); i++) {

    XYPolygon orig_poly = m_aof_avoid->getObstacleOrig(i);
    orig_poly.set_color("edge", m_hint_obst_edge_color);
    orig_poly.set_color("vertex", m_hint_obst_vertex_color);

    // If the obstacle is pertinent, perhaps draw filled in
    if(m_aof_avoid->isObstaclePert(i)) {
      orig_poly.set_color("fill", m_hint_obst_fill_color);
      orig_poly.set_transparency(m_hint_obst_fill_transparency);
    }
    string spec_orig = orig_poly.get_spec();
    postMessage("VIEW_POLYGON", spec_orig, "orig");
  }

  // (If no buffer, don't render buffer obstacles)
  if(m_buffer_dist <= 0)
    return;

  // Part 2: Handle the obstacle buffer obstacles
  for(unsigned int i=0; i<m_aof_avoid->size(); i++) {

    XYPolygon buff_poly = m_aof_avoid->getObstacleBuff(i);
    buff_poly.set_color("edge", m_hint_buff_edge_color);
    buff_poly.set_color("vertex", m_hint_buff_vertex_color);

    // If the obstacle is pertinent, perhaps draw filled in
    if(m_aof_avoid->isObstaclePert(i)) {
      buff_poly.set_color("fill", m_hint_buff_fill_color);
      buff_poly.set_transparency(m_hint_buff_fill_transparency);
    }

    string spec_buff = buff_poly.get_spec();
    postMessage("VIEW_POLYGON", spec_buff, "buff");
  }
}


//-----------------------------------------------------------
// Procedure: postErasablePolygons

void BHV_AvoidObstacles::postErasablePolygons()
{
  if(!m_aof_avoid)
    return;
  
  // Part 1: Handle the original obstacles (minus the buffer)
  unsigned int i, vsize = m_aof_avoid->size();
  for(i=0; i<vsize; i++) {
    XYPolygon orig_poly = m_aof_avoid->getObstacleOrig(i);
    orig_poly.set_active(false);
    string spec_orig = orig_poly.get_spec();
    postMessage("VIEW_POLYGON", spec_orig, "orig");
  }

  // (If no buffer, don't render buffer obstacles)
  if(m_buffer_dist <= 0)
    return;

  // Part 2: Handle the obstacle buffer obstacles
  for(i=0; i<vsize; i++) {
    XYPolygon buff_poly = m_aof_avoid->getObstacleBuff(i);
    buff_poly.set_active(false);
    string spec_buff = buff_poly.get_spec();
    postMessage("VIEW_POLYGON", spec_buff, "buff");
  }
}



