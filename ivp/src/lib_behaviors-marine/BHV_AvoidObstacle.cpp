/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_AvoidObstacle.cpp                                */
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

#include <iostream>
#include <cmath> 
#include <cstdlib>
#include "BHV_AvoidObstacle.h"
#include "OF_Reflector.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BuildUtils.h"
#include "XYFormatUtilsPoly.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_AvoidObstacle::BHV_AvoidObstacle(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "avoid_obstacles");
  //this->setParam("build_info", "uniform_piece=discrete@course:2,speed:3");
  //this->setParam("build_info", "uniform_grid =discrete@course:9,speed:6");

  m_domain = subDomain(m_domain, "course,speed");

  m_osx               = 0;
  m_osy               = 0;
  m_osh               = 0;
  m_buffer_dist       = 0;
  m_activation_dist   = -1;
  m_allowable_ttc     = 20;

  m_completed_dist    = 75;
  m_pwt_outer_dist    = 30;
  m_pwt_inner_dist    = 20;

  m_obstacle_relevance = 0;
  
  m_pwt_grade = "linear";

  m_hint_obst_edge_color   = "white";
  m_hint_obst_vertex_color = "dodger_blue";
  m_hint_obst_fill_color   = "gray60";
  m_hint_obst_fill_transparency = 0.7;

  m_hint_buff_edge_color   = "gray60";
  m_hint_buff_vertex_color = "dodger_blue";
  m_hint_buff_fill_color   = "gray70";
  m_hint_buff_fill_transparency = 0.1;

  m_aof_avoid = new AOF_AvoidObstacle(m_domain);

  addInfoVars("NAV_X, NAV_Y, NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition".
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_AvoidObstacle::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  double dval = atof(val.c_str());
  bool   non_neg_number = (isNumber(val) && (dval >= 0));
  
  if((param=="polygon") || (param=="points") || (param=="poly")) {
    if(m_aof_avoid == 0)
      return(false);
    XYPolygon new_polygon = string2Poly(val);
    if(!new_polygon.is_convex())
      return(false);
    m_aof_avoid->setObstacle(new_polygon);
    m_obstacle_relevance = 1;
  }
  else if((param == "allowable_ttc") && non_neg_number)
    m_allowable_ttc = dval;
  else if((param == "activation_dist") && non_neg_number) 
    m_activation_dist = dval;
  else if((param == "buffer_dist") && non_neg_number) 
    m_buffer_dist = dval;
  else if((param == "obstacle_key") && (val != ""))
    m_obstacle_key = val;
  else if((param == "pwt_outer_dist") && non_neg_number) {
    m_pwt_outer_dist = dval;
    if(m_pwt_inner_dist > m_pwt_outer_dist)
      m_pwt_inner_dist = m_pwt_outer_dist;
  }  
  else if((param == "pwt_inner_dist") && non_neg_number) {
    m_pwt_inner_dist = dval;
    if(m_pwt_outer_dist < m_pwt_inner_dist)
      m_pwt_outer_dist = m_pwt_inner_dist;
  }  
  else if((param == "completed_dist") && non_neg_number) 
    m_completed_dist = dval;
  else if(param == "visual_hints")
    return(handleVisualHints(val));
  else
    return(false);
  
  return(true);
}


//-----------------------------------------------------------
// Procedure: onSetParamComplete
//   Example: OBSTACLE_UPDATE_REQUEST = "obstacle_key=abe,
//                                       update_var=OBSTACLE_UPDATE_ABE"

void BHV_AvoidObstacle::onSetParamComplete()
{
  if(m_obstacle_key != "") {
    m_obstacle_update_var = "OBSTACLE_UPDATE_" + toupper(m_obstacle_key);
    string msg = "obstacle_key=" + m_obstacle_key;
    msg += ",update_var=" + m_obstacle_update_var;
    postMessage("OBSTACLE_UPDATE_REQUEST", msg);
    addInfoVars(m_obstacle_update_var, "no_warning");
  }
  postConfigStatus();
}

//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_AvoidObstacle::onIdleState()
{
  checkForObstacleUpdate();
  postErasablePolygons();
}

//-----------------------------------------------------------
// Procedure: onIdleToRunState

void BHV_AvoidObstacle::onIdleToRunState()
{
  postConfigStatus();
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_AvoidObstacle::onRunState() 
{
  // Part 1: Sanity checks
  bool ok1, ok2, ok3;

  m_osx = getBufferDoubleVal("NAV_X", ok1);
  m_osy = getBufferDoubleVal("NAV_Y", ok2);
  m_osh = getBufferDoubleVal("NAV_HEADING", ok3);

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

  checkForObstacleUpdate();

  // Part 2: Build/update the underlying objective function and initialize
  m_aof_avoid->setParam("os_x", m_osx);
  m_aof_avoid->setParam("os_y", m_osy);
  m_aof_avoid->setParam("os_h", m_osh);
  m_aof_avoid->setParam("buffer_dist", m_buffer_dist);
  m_aof_avoid->setParam("activation_dist", m_activation_dist);
  m_aof_avoid->setParam("allowable_ttc", m_allowable_ttc);

  // Initialization must be done immediately before any containment checks
  // since the buffer zones around the obstacles are built during init.
  bool ok_init = m_aof_avoid->initialize();
  if(!ok_init) {
    postWMessage("BHV_AvoidObstacles: AOF-Init Error");
    return(0);
  }

  // Part 3: Check if ownship violates either an obstacle or obstacle+buffer. 
  // Do this before init, because init includes buffer shrinking.
  if(m_aof_avoid->ownshipInObstacle(false)) {
    postWMessage("Ownship position within stated space of obstacle");
    postMessage("OBSTACLE_HIT", 1);
  }
  else
    postMessage("OSTACLE_HIT", 0);

  if(m_aof_avoid->ownshipInObstacle(true)) {
    postWMessage("Ownship position within stated BUFFER space of obstacle");
    postWMessage(m_aof_avoid->getDebugMsg());
  }

  // Part 4: postInitialize will cache values
  bool ok_post_init = m_aof_avoid->postInitialize();
  if(!ok_post_init) {
    postWMessage("BHV_AvoidObstacles: AOF-Post-Init Error");
    return(0);
  }
  
  // Part 5: Determine the relevance
  m_obstacle_relevance = getRelevance();

  // Part 6: Post the Visuals
  postViewablePolygons();

  if(m_obstacle_relevance <= 0)
    return(0);
  
  
  // Par 7: Build the actual objective function
  IvPFunction *ipf = 0;
  OF_Reflector reflector(m_aof_avoid, 1);

#if 1
  if(m_build_info != "")
    reflector.create(m_build_info);
  else {
    reflector.setParam("uniform_piece", "discrete@course:3,speed:3");
    reflector.setParam("uniform_grid",  "discrete@course:6,speed:6");
    reflector.create();
  }
#endif
#if 0
  unsigned int crs_choices = m_domain.getVarPoints("course");
  unsigned int spd_choices = m_domain.getVarPoints("speed");

  double bearing_min, bearing_max;
  m_aof_avoid->bearingMinMaxToBufferPoly(bearing_min, bearing_max);

  relector.create(1);
  relector.setParam();

  reflector.setParam("uniform_piece", "discrete @ x:5,y:5");  
  reflector.setParam("refine_region", "native @ x:10:24,y:-25:20");  
  reflector.setParam("refine_piece",  "discrete @ x:2,y:2");  
#endif

  if(!reflector.stateOK())
    postWMessage(reflector.getWarnings());
  else {
    ipf = reflector.extractIvPFunction(true); // true means normalize [0,100]
    if(ipf)
      ipf->setPWT(m_obstacle_relevance * m_priority_wt);
  }
  
  return(ipf);
}

//-----------------------------------------------------------
// Procedure: getRelevance
//            Calculate the relevance first. If zero-relevance, 
//            we won't bother to create the objective function.

double BHV_AvoidObstacle::getRelevance()
{
  // Part 1: Sanity checks
  if(m_aof_avoid == 0)
    return(0);
  if(m_pwt_outer_dist < m_pwt_inner_dist)
    return(0);

  // Part 2: Check for easy rejection based on relative position
  //         of ownship to polygon
  XYPolygon buff_poly = m_aof_avoid->getObstacleOrig();
  if(polyAft(m_osx, m_osy, m_osh, buff_poly))
    return(0);

  // Part 3: Get the obstacle range and reject if zero
  double obstacle_range = buff_poly.dist_to_poly(m_osx, m_osy);
  if(obstacle_range == 0)
    return(0);

  // Part 4: Now the easy range cases: when the obstacle is outside 
  //         the min or max priority ranges
  // First declare the range of relevance values to be calc'ed
  double min_dist_relevance = 0;
  double max_dist_relevance = 1;

  if(obstacle_range >= m_pwt_outer_dist)
    return(min_dist_relevance);
  if(obstacle_range <= m_pwt_inner_dist)
    return(max_dist_relevance);

  // Part 5: Handle the in-between case
  // Note: drange should never be zero since either of the above
  // conditionals would be true and the function would have returned.
  double drange = (m_pwt_outer_dist - m_pwt_inner_dist);
  if(drange <= 0)
    return(0);
  double dpct = (m_pwt_outer_dist - obstacle_range) / drange;
  
  // Part 6: Possibly apply the grade scale to the raw distance
  if(m_pwt_grade == "quadratic")
    dpct = dpct * dpct;
  else if(m_pwt_grade == "quasi")
    dpct = pow(dpct, 1.5);

  // Part 7: Now calculate and return the relevance
  double rng_dist_relevance = max_dist_relevance - min_dist_relevance;
  double d_relevance = (dpct * rng_dist_relevance) + min_dist_relevance;

  return(d_relevance);  
}

//-----------------------------------------------------------
// Procedure: checkForObstacleUpdate

bool BHV_AvoidObstacle::checkForObstacleUpdate()
{
  if(m_obstacle_update_var == "") 
    return(true);
      
  double time_since_update = getBufferTimeVal(m_obstacle_update_var);
  if(time_since_update > 0)
    return(false);  

  bool ok;
  string str = getBufferStringVal(m_obstacle_update_var, ok);
  if(!ok)
    return(false);

  // Example: pts={120,-80:120,-50:150,-50:150,-80:138,-80},label=a
  
  bool update_ok = setParam("poly", str);
   
  return(update_ok);
}
  


//-----------------------------------------------------------
// Procedure: handleVisualHints()

bool BHV_AvoidObstacle::handleVisualHints(string hints)
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

void BHV_AvoidObstacle::postViewablePolygons()
{
  if(!m_aof_avoid)
    return;
  if(!m_aof_avoid->obstacleSet())
    return;
  
  XYPolygon orig_poly = m_aof_avoid->getObstacleOrig();
  orig_poly.set_color("edge", m_hint_obst_edge_color);
  orig_poly.set_color("vertex", m_hint_obst_vertex_color);
  
  // If the obstacle is relevant, perhaps draw filled in
  if(m_obstacle_relevance > 0) {
    orig_poly.set_color("fill", m_hint_obst_fill_color);
    orig_poly.set_transparency(m_hint_obst_fill_transparency);
  }
  string spec_orig = orig_poly.get_spec();
  postMessage("VIEW_POLYGON", spec_orig, "orig");

  // (If no buffer, don't render buffer obstacles)
  if(m_buffer_dist <= 0)
    return;

  XYPolygon buff_poly = m_aof_avoid->getObstacleBuff();
  buff_poly.set_color("edge", m_hint_buff_edge_color);
  buff_poly.set_color("vertex", m_hint_buff_vertex_color);

  // If the obstacle is relevant, perhaps draw filled in
  if(m_obstacle_relevance > 0) {
    buff_poly.set_color("fill", m_hint_buff_fill_color);
    buff_poly.set_transparency(m_hint_buff_fill_transparency);
  }

  string spec_buff = buff_poly.get_spec();
  postMessage("VIEW_POLYGON", spec_buff, "buff");
}


//-----------------------------------------------------------
// Procedure: postErasablePolygons

void BHV_AvoidObstacle::postErasablePolygons()
{
  if(!m_aof_avoid)
    return;
  if(!m_aof_avoid->obstacleSet())
    return;
  
  // Part 1: Handle the original obstacle (minus the buffer)
  XYPolygon orig_poly = m_aof_avoid->getObstacleOrig();
  orig_poly.set_active(false);
  string spec_orig = orig_poly.get_spec();
  postMessage("VIEW_POLYGON", spec_orig, "orig");
  
  // (If no buffer, don't render buffer obstacles)
  if(m_buffer_dist <= 0)
    return;

  // Part 2: Handle the obstacle buffer obstacles
  XYPolygon buff_poly = m_aof_avoid->getObstacleBuff();
  buff_poly.set_active(false);
  string spec_buff = buff_poly.get_spec();
  postMessage("VIEW_POLYGON", spec_buff, "buff");
}



//-----------------------------------------------------------
// Procedure: postConfigStatus

void BHV_AvoidObstacle::postConfigStatus()
{
  string str = "type=BHV_AvoidObstacles,name=" + m_descriptor;
  
  str += ",allowable_ttc="  + doubleToString(m_allowable_ttc,2);
  str += ",activation_dist=" + doubleToString(m_activation_dist,2);
  str += ",buffer_dist="   + doubleToString(m_buffer_dist,2);

  str += ",pwt_outer_dist=" + doubleToString(m_pwt_outer_dist,2);
  str += ",pwt_inner_dist=" + doubleToString(m_pwt_inner_dist,2);
  str += ",completed_dist=" + doubleToString(m_completed_dist,2);

  str += ",obstacle_key=" + m_obstacle_key;
  str += ",obtacle_update_var=" + m_obstacle_update_var;

  postRepeatableMessage("BHV_SETTINGS", str);
}


