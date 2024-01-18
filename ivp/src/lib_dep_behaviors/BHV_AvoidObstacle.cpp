/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
#include "AOF_AvoidObstacle.h"
#include "OF_Reflector.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BuildUtils.h"
#include "XYFormatUtilsPoly.h"
#include "XYPolyExpander.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_AvoidObstacle::BHV_AvoidObstacle(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "avoid_obstacles");

  m_domain = subDomain(m_domain, "course,speed");

  m_osx               = 0;
  m_osy               = 0;
  m_osh               = 0;
  m_buffer_dist       = 0;
  m_allowable_ttc     = 20;

  m_completed_dist    = 75;
  m_pwt_outer_dist    = 30;
  m_pwt_inner_dist    = 20;

  m_completed_dist_set = false;
  m_pwt_outer_dist_set = false;
  m_pwt_inner_dist_set = false;

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

  m_hide_deprecation_warning = false;
  
  m_no_alert_request  = false;
  m_resolved_pending  = false;
  
  addInfoVars("NAV_X, NAV_Y, NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam()
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
    XYPolygon new_polygon = string2Poly(val);
    if(!new_polygon.is_convex())
      return(false);
    m_obstacle_orig = new_polygon;
    m_obstacle_relevance = 1;
  }
  else if(param == "allowable_ttc")
    return(setNonNegDoubleOnString(m_allowable_ttc, val));
  else if(param == "buffer_dist")
    return(setNonNegDoubleOnString(m_buffer_dist, val));
  else if(param == "obstacle_key")
    return(setNonWhiteVarOnString(m_obstacle_key, val));
  
  else if((param == "pwt_outer_dist") && non_neg_number) {
    m_pwt_outer_dist = dval;
    m_pwt_outer_dist_set = true;
    
    if(m_pwt_inner_dist > m_pwt_outer_dist) {
      m_pwt_inner_dist = m_pwt_outer_dist;
      if(m_pwt_inner_dist_set) {
	postBadConfig("pwt_outer_dist must be > pwt_inner_dist");
	return(false);
      }
    }
    if(m_completed_dist <= m_pwt_outer_dist) {
      m_completed_dist = m_pwt_outer_dist + 1;
      if(m_completed_dist_set)
	return(false);
    }
  }  

  else if((param == "pwt_inner_dist") && non_neg_number) {
    m_pwt_inner_dist = dval;
    m_pwt_inner_dist_set = true;

    if(m_pwt_outer_dist < m_pwt_inner_dist) {
      m_pwt_outer_dist = m_pwt_inner_dist;
      if(m_pwt_outer_dist_set)
	return(false);
    }
  }  

  else if((param == "completed_dist") && non_neg_number) {
    m_completed_dist = dval;
    m_completed_dist_set = true;

    if(m_completed_dist <= m_pwt_outer_dist) {
      m_completed_dist = m_pwt_outer_dist + 1;
      if(m_pwt_outer_dist_set) {
	postBadConfig("completed_dist must be > pwt_outer_dist");
	return(false);
      }
    }
  }
  else if(param == "no_alert_request")
    return(setBooleanOnString(m_no_alert_request, val));
  else if(param == "resolved")
    return(setBooleanOnString(m_resolved_pending, val));
  else if(param == "i_understand_this_behavior_is_deprecated")
    return(setBooleanOnString(m_hide_deprecation_warning, val));
  else if(param == "visual_hints")
    return(handleVisualHints(val));
  else
    return(false);
  
  return(true);
}


//-----------------------------------------------------------
// Procedure: isDeprecated()
//   Purpose: Users of this behavior will get a configuration
//            warning unless they use the "i_understand" param
//            essentially saying use at your own risk.

string BHV_AvoidObstacle::isDeprecated()
{
  if(m_hide_deprecation_warning)
    return("");
  
  string msg;
  msg += "BHV_AvoidObstacle is no longer supported.";
  msg += "# Use BHV_FixedTurn instead.";
    msg += "# Set i_understand_this_behavior_is_deprecated=true";
  msg += "# to suppress this warning and use at your own risk.";  
  return(msg);
}

//-----------------------------------------------------------
// Procedure: onSetParamComplete()
//   Example: OBSTACLE_UPDATE_REQUEST = "obstacle_key=abe,
//                                       update_var=OBSTACLE_UPDATE_ABE"

void BHV_AvoidObstacle::onSetParamComplete()
{
  postConfigStatus();
}

//-----------------------------------------------------------
// Procedure: onHelmStart()

void BHV_AvoidObstacle::onHelmStart()
{
  if(m_no_alert_request || (m_update_var == ""))
    return;

  string alert_request = "name=" + m_descriptor;
  alert_request += ",update_var=" + m_update_var;
  alert_request += ",alert_range=" + doubleToStringX(m_pwt_outer_dist,1);

  postMessage("OBM_ALERT_REQUEST", alert_request);
}

//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_AvoidObstacle::onIdleState()
{
  postErasablePolygons();
}

//-----------------------------------------------------------
// Procedure: onCompleteState()

void BHV_AvoidObstacle::onCompleteState() 
{
  postErasablePolygons();
  postRepeatableMessage("OBSTACLE_RESOLVED", m_obstacle_key);
}

//-----------------------------------------------------------
// Procedure: onInactiveState

void BHV_AvoidObstacle::onInactiveState()
{
  postErasablePolygons();
}

//-----------------------------------------------------------
// Procedure: onIdleToRunState

void BHV_AvoidObstacle::onIdleToRunState()
{
  postConfigStatus();
}

//-----------------------------------------------------------
// Procedure: onSpawn()

void BHV_AvoidObstacle::onSpawn()
{
  postMessage("AVD_OB_SPAWN", m_descriptor);
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_AvoidObstacle::onRunState() 
{
  // Part 1: Handle case where behavior may have been resolved because
  // we no longer have sensor informaion on the obstacle, even though
  // we may still be in range of its last known location.
  if(m_resolved_pending) {
    setComplete();
    return(0);
  }
    
  // Part 2: Sanity checks
  if(!updatePlatformInfo())
    return(0);  
  if(!m_obstacle_orig.is_convex())
    return(0);

  // Part 3: Build/update the buffer retion around the obstacle
  bool buffer_applied = applyBuffer();
  if(!buffer_applied)
    return(0);

  // Part 4: Handle case where behavior is completed due to range 
  double os_dist_to_poly = m_obstacle_orig.dist_to_poly(m_osx, m_osy);
  postMessage("OS_DIST_TO_POLY", os_dist_to_poly);
  if(os_dist_to_poly > m_completed_dist) {
    setComplete();
    return(0);
  }

  // Part 5: Determine the relevance
  m_obstacle_relevance = getRelevance();
  if(m_obstacle_relevance <= 0)
    return(0);
    
  // Part 6: Build/update underlying objective function and init
  AOF_AvoidObstacle  aof_avoid(m_domain);
  aof_avoid.setObstacleOrig(m_obstacle_orig);
  aof_avoid.setObstacleBuff(m_obstacle_buff);
  aof_avoid.setParam("os_x", m_osx);
  aof_avoid.setParam("os_y", m_osy);
  aof_avoid.setParam("os_h", m_osh);
  aof_avoid.setParam("buffer_dist", m_buffer_dist);
  aof_avoid.setParam("allowable_ttc", m_allowable_ttc);

  bool ok_init = aof_avoid.initialize();
  if(!ok_init) {
    string aof_msg = aof_avoid.getCatMsgsAOF();
    postWMessage("Unable to init AOF_AvoidObstacle:"+aof_msg);
    return(0);
  }
  
  // Part 7: Build the actual objective function with reflector
  OF_Reflector reflector(&aof_avoid, 1);
  if(m_build_info != "")
    reflector.create(m_build_info);
  else {
    reflector.setParam("uniform_piece", "discrete@course:3,speed:3");
    reflector.setParam("uniform_grid",  "discrete@course:3,speed:3");
    reflector.create();
  }
  if(!reflector.stateOK()) {
    postWMessage(reflector.getWarnings());
    return(0);
  }
  
  // Part 8: Extract objective function, apply priority, post visuals
  IvPFunction *ipf = reflector.extractIvPFunction(true); // true means normalize
  if(ipf) {
    ipf->setPWT(m_obstacle_relevance * m_priority_wt);
    postViewablePolygons();
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
  if(m_pwt_outer_dist < m_pwt_inner_dist)
    return(0);

  // Part 2: Zero relevance if the obstacle is more than 20 degs
  // abaft our beam.
  if(polyAft(m_osx, m_osy, m_osh, m_obstacle_orig, 20))
    return(0);
  
  // Part 3: Get the obstacle range and reject if zero
  double obstacle_range = m_obstacle_orig.dist_to_poly(m_osx, m_osy);
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
  // Part 1 - Render the original obstacle polygon
  if(m_obstacle_orig.is_convex()) {
    m_obstacle_orig.set_active(true);
    m_obstacle_orig.set_color("edge", m_hint_obst_edge_color);
    m_obstacle_orig.set_color("vertex", m_hint_obst_vertex_color);
    m_obstacle_orig.set_color("vertex", "pink");
    
    // If the obstacle is relevant, perhaps draw filled in
    if(m_obstacle_relevance > 0) {
      m_obstacle_orig.set_color("fill", m_hint_obst_fill_color);
      m_obstacle_orig.set_transparency(m_hint_obst_fill_transparency);
    }
    string spec = m_obstacle_orig.get_spec(3);
    postMessage("VIEW_POLYGON", spec, "orig");
  }
    
  // Part 2 - Render the buffer obstacle polygon
  if(m_obstacle_buff.is_convex() && (m_buffer_dist > 0)) {
    m_obstacle_buff.set_active(true);
    m_obstacle_buff.set_color("edge", m_hint_buff_edge_color);
    m_obstacle_buff.set_color("vertex", m_hint_buff_vertex_color);

    // If the obstacle is relevant, perhaps draw filled in
    if(m_obstacle_relevance > 0) {
      m_obstacle_buff.set_color("fill", m_hint_buff_fill_color);
      m_obstacle_buff.set_transparency(m_hint_buff_fill_transparency);
    }
    string spec = m_obstacle_buff.get_spec(3);
    postMessage("VIEW_POLYGON", spec, "buff");
  }
}


//-----------------------------------------------------------
// Procedure: postErasablePolygons

void BHV_AvoidObstacle::postErasablePolygons()
{
  if(m_obstacle_orig.is_convex()) {
    string spec = m_obstacle_orig.get_spec_inactive();
    postMessage("VIEW_POLYGON", spec, "orig");
  }
   
  if(m_obstacle_buff.is_convex() && (m_buffer_dist > 0)) {
    string spec = m_obstacle_buff.get_spec_inactive();
    postMessage("VIEW_POLYGON", spec, "buff");
  }
}

//-----------------------------------------------------------
// Procedure: updatePlatformInfo()

bool BHV_AvoidObstacle::updatePlatformInfo()
{
  bool ok1, ok2, ok3;
  m_osx = getBufferDoubleVal("NAV_X", ok1);
  m_osy = getBufferDoubleVal("NAV_Y", ok2);
  m_osh = getBufferDoubleVal("NAV_HEADING", ok3);

  if(!ok1 || !ok2) {
    postWMessage("No Ownship NAV_X and/or NAV_Y in info_buffer");
    return(false);
  }
  if(!ok3) {
    postWMessage("No Ownship NAV_HEADING in info_buffer");
    return(false);
  }
  return(true);
}

//----------------------------------------------------------------
// Procedure: applyBuffer

bool BHV_AvoidObstacle::applyBuffer()
{
  m_obstacle_buff = m_obstacle_orig;

  // Sanity check 1 - if buffer dist is zero, buffer poly is orig
  if(m_buffer_dist <= 0)
    return(false);

  // Sanity check 2 - if ownship in orig poly, buffer poly is orig
  if(m_obstacle_orig.contains(m_osx, m_osy))
    return(false);

  double curr_buffer_dist = m_buffer_dist;
  double os_rng_to_obstacle = m_obstacle_orig.dist_to_poly(m_osx, m_osy);
  if(os_rng_to_obstacle < (m_buffer_dist + 1)) {
    curr_buffer_dist = os_rng_to_obstacle - 2;
    if(curr_buffer_dist < 0)
      curr_buffer_dist = 0;
  }

  // Part 1: Build the buffer poly to spec size.
  XYPolyExpander expander;
  expander.setPoly(m_obstacle_orig);
  expander.setDegreeDelta(10);
  expander.setVertexProximityThresh(0.5);
  m_obstacle_buff = expander.getBufferPoly(curr_buffer_dist);

  if(!m_obstacle_buff.is_convex()) {
    postWMessage("Buffer obstacle not convex");
    m_obstacle_buff = m_obstacle_orig;
    return(false);
  }
  if(m_obstacle_buff.contains(m_osx, m_osy)) {
    postWMessage("Buffer obstacle contains OS position");
    m_obstacle_buff = m_obstacle_orig;
    return(false);
  }

  m_obstacle_buff.set_label(m_obstacle_orig.get_label() + "_buff");

  return(true);
}

//-----------------------------------------------------------
// Procedure: postConfigStatus

void BHV_AvoidObstacle::postConfigStatus()
{
  string str = "type=BHV_AvoidObstacles,name=" + m_descriptor;
  
  str += ",allowable_ttc="  + doubleToString(m_allowable_ttc,2);
  str += ",buffer_dist="   + doubleToString(m_buffer_dist,2);

  str += ",pwt_outer_dist=" + doubleToString(m_pwt_outer_dist,2);
  str += ",pwt_inner_dist=" + doubleToString(m_pwt_inner_dist,2);
  str += ",completed_dist=" + doubleToString(m_completed_dist,2);

  str += ",obstacle_key=" + m_obstacle_key;

  postRepeatableMessage("BHV_SETTINGS", str);
}

