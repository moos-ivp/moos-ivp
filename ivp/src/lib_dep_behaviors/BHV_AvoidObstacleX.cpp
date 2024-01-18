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
#include "BHV_AvoidObstacleX.h"
#include "AOF_AvoidObstacleX.h"
#include "OF_Reflector.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BuildUtils.h"
#include "RefineryObAvoid.h"
#include "XYFormatUtilsPoly.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor()
//      Note: Most of the behavior state is contained in the
//            member variable m_obship_model

BHV_AvoidObstacleX::BHV_AvoidObstacleX(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "avoid_obstacle");
  m_domain = subDomain(m_domain, "course,speed");

  // Initialize config vars
  m_pwt_grade = "linear";

  m_hint_obst_edge_color   = "white";
  m_hint_obst_vertex_color = "white";
  m_hint_obst_vertex_size  = 1;
  m_hint_obst_fill_color   = "gray60";
  m_hint_obst_fill_transparency = 0.7;

  m_hint_buff_min_edge_color   = "gray60";
  m_hint_buff_min_vertex_size  = 1;
  m_hint_buff_min_vertex_color = "dodger_blue";
  m_hint_buff_min_fill_color   = "gray70";
  m_hint_buff_min_fill_transparency = 0.25;

  m_hint_buff_max_edge_color   = "gray60";
  m_hint_buff_max_vertex_size  = 1;
  m_hint_buff_max_vertex_color = "dodger_blue";
  m_hint_buff_max_fill_color   = "gray70";
  m_hint_buff_max_fill_transparency = 0.1;

  m_use_refinery     = false;
  m_resolved_pending = false;

  m_resolved_obstacle_var = "OBM_RESOLVED";
  
  m_hide_deprecation_warning = false;
  
  // Initialize state vars
  m_obstacle_relevance = 0;

  addInfoVars("NAV_X, NAV_Y, NAV_HEADING");
  addInfoVars(m_resolved_obstacle_var);
}

//-----------------------------------------------------------
// Procedure: setParam()
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition".
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_AvoidObstacleX::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  double dval = atof(val.c_str());
  bool   non_neg_number = (isNumber(val) && (dval >= 0));

  string config_result;
  if((param=="polygon") || (param=="poly"))
    config_result = m_obship_model.setObstacle(val);
  else if(param == "allowable_ttc")
    config_result = m_obship_model.setAllowableTTC(dval);
  else if((param == "min_util_cpa_dist") && non_neg_number)
    config_result = m_obship_model.setMinUtilCPA(dval);
  else if((param == "max_util_cpa_dist") && non_neg_number)
    config_result = m_obship_model.setMaxUtilCPA(dval);
  else if((param == "pwt_inner_dist") && non_neg_number)
    config_result = m_obship_model.setPwtInnerDist(dval);
  else if((param == "pwt_outer_dist") && non_neg_number)
    config_result = m_obship_model.setPwtOuterDist(dval);
  else if((param == "completed_dist") && non_neg_number) 
    config_result = m_obship_model.setCompletedDist(dval);
  else if(param == "resolved")
    return(setBooleanOnString(m_resolved_pending, val));
  else if(param == "visual_hints")
    return(handleParamVisualHints(val));
  else if(param == "use_refinery")
    return(setBooleanOnString(m_use_refinery, val));
  else if(param == "id")
    return(setNonWhiteVarOnString(m_obstacle_id, val));
  else if(param == "i_understand_this_behavior_is_deprecated")
    return(setBooleanOnString(m_hide_deprecation_warning, val));
  else
    return(false);

  if(config_result != "") {
    postBadConfig(config_result);
    return(false);
  }

  return(true);
}

//-----------------------------------------------------------
// Procedure: isDeprecated()
//   Purpose: Users of this behavior will get a configuration
//            warning unless they use the "i_understand" param
//            essentially saying use at your own risk.

string BHV_AvoidObstacleX::isDeprecated()
{
  if(m_hide_deprecation_warning)
    return("");
  
  string msg;
  msg += "BHV_AvoidObstacleX is no longer supported.";
  msg += "# Use BHV_FixedTurn instead.";
  msg += "# Set i_understand_this_behavior_is_deprecated=true";
  msg += "# to suppress this warning and use at your own risk.";  
  return(msg);
}

//-----------------------------------------------------------
// Procedure: onSetParamComplete()

void BHV_AvoidObstacleX::onSetParamComplete()
{
  postConfigStatus();
}

//-----------------------------------------------------------
// Procedure: onHelmStart()

void BHV_AvoidObstacleX::onHelmStart()
{
  if(isDynamicallySpawnable() && (m_update_var != "")) {
    double pwt_outer_dist = m_obship_model.getPwtOuterDist();
    string alert_request = "name=" + m_descriptor;
    alert_request += ",update_var=" + m_update_var;
    alert_request += ",alert_range=" + doubleToStringX(pwt_outer_dist,1);
    postMessage("OBM_ALERT_REQUEST", alert_request);
  }
}
  
//-----------------------------------------------------------
// Procedure: onEveryState()

void BHV_AvoidObstacleX::onEveryState(string str)
{
  // Get list of all obstacles declared to be resolved by the obstacle mgr
  bool ok = true;
  vector<string> obstacles_resolved;
  obstacles_resolved = getBufferStringVector(m_resolved_obstacle_var, ok);
  if(!ok)
    return;

  // Check ids of all resolved obstacles against our own id. If match then
  // declare the resolution to be pending.
  for(unsigned int i=0; i<obstacles_resolved.size(); i++) {
    string obstacle_id = obstacles_resolved[i];
    postMessage("NOTED_RESOLVED", obstacle_id);

    if(m_obstacle_id == obstacle_id)
      m_resolved_pending = true;
  }
}

//-----------------------------------------------------------
// Procedure: onIdleState()

void BHV_AvoidObstacleX::onIdleState()
{
  postErasablePolygons();

  if(m_resolved_pending)
    setComplete();
}

//-----------------------------------------------------------
// Procedure: onCompleteState()

void BHV_AvoidObstacleX::onCompleteState() 
{
  postErasablePolygons();
}

//-----------------------------------------------------------
// Procedure: onInactiveState()

void BHV_AvoidObstacleX::onInactiveState()
{
  postErasablePolygons();
}

//-----------------------------------------------------------
// Procedure: onIdleToRunState()

void BHV_AvoidObstacleX::onIdleToRunState()
{
  postConfigStatus();
}

//-----------------------------------------------------------
// Procedure: onSpawn()

void BHV_AvoidObstacleX::onSpawn()
{
  postMessage("AVD_OB_SPAWN", m_descriptor);
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_AvoidObstacleX::onRunState() 
{
  string debug_info = m_descriptor + ":" + uintToString(m_helm_iter);

  if(m_resolved_pending) {
    setComplete();
    return(0);
  }
  // Part 1: Sanity checks
  if(m_obship_model.getFailedExpandPolyStr(false) != "") {
    string msg = m_obship_model.getFailedExpandPolyStr();
    postWMessage(msg);
  }
  if(!updatePlatformInfo())
    return(0);  
  if(!m_obship_model.isValid())
    return(0);

  // Part 2: Handle case where behavior is completed 
  double os_dist_to_poly = m_obship_model.getRange();
  postMessage("OS_DIST_TO_POLY", os_dist_to_poly);

  if(os_dist_to_poly > m_obship_model.getCompletedDist()) {
    setComplete();
    return(0);
  }

  if(m_obship_model.isObstacleAft(20))
    return(0);
  
  // Part 3: Determine the relevance
  m_obstacle_relevance = getRelevance();
  if(m_obstacle_relevance <= 0)
    return(0);
  
  // Part 4: Set and init the AOF
  AOF_AvoidObstacleX  aof_avoid(m_domain);
  aof_avoid.setObShipModel(m_obship_model);
  bool ok_init = aof_avoid.initialize();
  if(!ok_init) {
    string aof_msg = aof_avoid.getCatMsgsAOF();
    postWMessage("Unable to init AOF_AvoidObstacleX:"+aof_msg);
    return(0);
  }
  
  // Part 6: Build the actual objective function with reflector
  OF_Reflector reflector(&aof_avoid, 1);

  // ===========================================================
  // Utilize the Refinery to identify plateau, and basin regions
  // ===========================================================
  if(m_use_refinery) {
    RefineryObAvoid refinery(m_domain);
    refinery.setVerbose(true);
    refinery.setRefineRegions(m_obship_model);

    vector<IvPBox> plateau_regions = refinery.getPlateaus();
    vector<IvPBox> basin_regions   = refinery.getBasins();

    for(unsigned int i=0; i<plateau_regions.size(); i++) {
      reflector.setParam("plateau_region", plateau_regions[i]);
      plateau_regions[i].print();
    }
    for(unsigned int i=0; i<basin_regions.size(); i++) {
      reflector.setParam("basin_region", basin_regions[i]);
      basin_regions[i].print();
    }
  }

  if(m_build_info != "")
    reflector.create(m_build_info);
  else {
    reflector.setParam("uniform_piece", "discrete@course:3,speed:2");
    reflector.setParam("uniform_grid",  "discrete@course:6,speed:4");
    reflector.create();
  }
  if(!reflector.stateOK()) {
    postWMessage(reflector.getWarnings());
    return(0);
  }

  // Part 7: Extract objective function, apply priority, post visuals
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

double BHV_AvoidObstacleX::getRelevance()
{
  // Let the ObShipModel tell us the raw range relevance
  double range_relevance = m_obship_model.getRangeRelevance();
  cout << "Range Relevance: " << range_relevance << endl;
  if(range_relevance <= 0)
    return(0);
  
  // Part 2: Possibly apply the grade scale to the raw distance
  double relevance = range_relevance;
  if(m_pwt_grade == "quadratic")
    relevance = range_relevance * range_relevance;
  else if(m_pwt_grade == "quasi")
    relevance = pow(range_relevance, 1.5);

  return(relevance);
}

//-----------------------------------------------------------
// Procedure: handleParamVisualHints()

bool BHV_AvoidObstacleX::handleParamVisualHints(string hints)
{
  vector<string> svector = parseStringQ(hints, ',');

  for(unsigned int i=0; i<svector.size(); i++) {

    string hint  = svector[i];
    string param = tolower(biteStringX(hint, '='));
    string value = hint;
    
    if(param == "obstacle_edge_color")
      return(setColorOnString(m_hint_obst_edge_color, value));
    else if(param == "obstacle_vertex_color")
      return(setColorOnString(m_hint_obst_vertex_color, value));
    else if(param == "obstacle_vertex_size")
      return(setNonNegDoubleOnString(m_hint_obst_vertex_size, value));
    else if(param == "obstacle_fill_color")
      return(setColorOnString(m_hint_obst_fill_color, value));
    else if((param == "obstacle_fill_transparency") && isNumber(value)) {
      double transparency = atof(value.c_str());
      transparency = vclip(transparency, 0, 1);
      m_hint_obst_fill_transparency = transparency;
    }
    
    else if(param == "buffer_min_edge_color")
      return(setColorOnString(m_hint_buff_min_edge_color, value));
    else if(param == "buffer_min_vertex_color")
      return(setColorOnString(m_hint_buff_min_vertex_color, value));
    else if(param == "buffer_min_vertex_size")
      return(setNonNegDoubleOnString(m_hint_buff_min_vertex_size, value));
    else if(param == "buffer_min_fill_color")
      return(setColorOnString(m_hint_buff_min_fill_color, value));
    else if((param == "buffer_min_fill_transparency") && isNumber(value)) {
      double transparency = atof(value.c_str());
      transparency = vclip(transparency, 0, 1);
      m_hint_buff_max_fill_transparency = transparency;
    }

    else if(param == "buffer_max_edge_color")
      return(setColorOnString(m_hint_buff_max_edge_color, value));
    else if(param == "buffer_max_vertex_color")
      return(setColorOnString(m_hint_buff_max_vertex_color, value));
    else if(param == "buffer_max_vertex_size")
      return(setNonNegDoubleOnString(m_hint_buff_max_vertex_size, value));
    else if(param == "buffer_max_fill_color")
      return(setColorOnString(m_hint_buff_max_fill_color, value));
    else if((param == "buffer_max_fill_transparency") && isNumber(value)) {
      double transparency = atof(value.c_str());
      transparency = vclip(transparency, 0, 1);
      m_hint_buff_max_fill_transparency = transparency;
    }
    else
      return(false);
  }
  return(true);
}


//-----------------------------------------------------------
// Procedure: postViewablePolygons

void BHV_AvoidObstacleX::postViewablePolygons()
{
  // =================================================
  // Part 1 - Render the original obstacle polygon
  // =================================================
  XYPolygon obstacle = m_obship_model.getObstacle();
  obstacle.set_active(true);
  obstacle.set_color("edge", m_hint_obst_edge_color);
  obstacle.set_color("vertex", m_hint_obst_vertex_color);
  obstacle.set_vertex_size(m_hint_obst_vertex_size);
  
  // If the obstacle is relevant, perhaps draw filled in
  if(m_obstacle_relevance > 0) {
    obstacle.set_color("fill", m_hint_obst_fill_color);
    obstacle.set_transparency(m_hint_obst_fill_transparency);
  }
  postMessage("VIEW_POLYGON", obstacle.get_spec(3), "one");
    
  // =================================================
  // Part 2 - Render the buffer min_cpa obstacle polygon
  // =================================================
  XYPolygon obstacle_buff_min = m_obship_model.getObstacleBuffMin();
  obstacle_buff_min.set_active(true);
  obstacle_buff_min.set_color("edge", m_hint_buff_min_edge_color);
  obstacle_buff_min.set_color("vertex", m_hint_buff_min_vertex_color);
  obstacle_buff_min.set_vertex_size(m_hint_buff_min_vertex_size);
  
  // If the obstacle is relevant, perhaps draw filled in
  if(m_obstacle_relevance > 0) {
    obstacle_buff_min.set_color("fill", m_hint_buff_min_fill_color);
    obstacle_buff_min.set_transparency(m_hint_buff_min_fill_transparency);
  }
  postMessage("VIEW_POLYGON", obstacle_buff_min.get_spec(3), "two");

  // =================================================
  // Part 3 - Render the buffer max_cpa obstacle polygon
  // =================================================
  XYPolygon obstacle_buff_max = m_obship_model.getObstacleBuffMax();
  obstacle_buff_max.set_active(true);
  obstacle_buff_max.set_color("edge", m_hint_buff_max_edge_color);
  obstacle_buff_max.set_color("vertex", m_hint_buff_max_vertex_color);
  obstacle_buff_max.set_vertex_size(m_hint_buff_max_vertex_size);
  
  // If the obstacle is relevant, perhaps draw filled in
  if(m_obstacle_relevance > 0) {
    obstacle_buff_max.set_color("fill", m_hint_buff_max_fill_color);
    obstacle_buff_max.set_transparency(m_hint_buff_max_fill_transparency);
  }
  postMessage("VIEW_POLYGON", obstacle_buff_max.get_spec(3), "three");
}


//-----------------------------------------------------------
// Procedure: postErasablePolygons

void BHV_AvoidObstacleX::postErasablePolygons()
{
  XYPolygon obstacle = m_obship_model.getObstacle();
  postMessage("VIEW_POLYGON", obstacle.get_spec_inactive(), "one");

  XYPolygon obstacle_buff_min = m_obship_model.getObstacleBuffMin();
  postMessage("VIEW_POLYGON", obstacle_buff_min.get_spec_inactive(), "two");

  XYPolygon obstacle_buff_max = m_obship_model.getObstacleBuffMax();
  obstacle_buff_max.set_color("fill", "invisible");
  postMessage("VIEW_POLYGON", obstacle_buff_max.get_spec_inactive(), "three");

  
}

//-----------------------------------------------------------
// Procedure: updatePlatformInfo()

bool BHV_AvoidObstacleX::updatePlatformInfo()
{
  bool ok1, ok2, ok3;
  double osx = getBufferDoubleVal("NAV_X", ok1);
  double osy = getBufferDoubleVal("NAV_Y", ok2);
  double osh = getBufferDoubleVal("NAV_HEADING", ok3);
  
  bool ok_obship_model_update = m_obship_model.setPose(osx, osy, osh);
  if(!ok_obship_model_update) {
    postWMessage("Problem updating obship_model pose");
    return(false);
  }

  if(!m_obship_model.getObstacle().is_convex()) {
    postWMessage("Non-convex Obstacle");
    return(false);
  }
  if(!m_obship_model.getObstacleBuffMin().is_convex()) {
    postWMessage("Non-convex ObstacleBuffMin");
    return(false);
  }
  if(!m_obship_model.getObstacleBuffMax().is_convex()) {
    postWMessage("Non-convex ObstacleBuffMax");
    return(false);
  }
  
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

//-----------------------------------------------------------
// Procedure: postConfigStatus

void BHV_AvoidObstacleX::postConfigStatus()
{
  string str = "type=BHV_AvoidObstacleXs,name=" + m_descriptor;

  double pwt_inner_dist = m_obship_model.getPwtInnerDist();
  double pwt_outer_dist = m_obship_model.getPwtOuterDist();
  double completed_dist = m_obship_model.getCompletedDist();
  double min_util_cpa   = m_obship_model.getMinUtilCPA();
  double max_util_cpa   = m_obship_model.getMaxUtilCPA();
  double allowable_ttc  = m_obship_model.getAllowableTTC();
  
  str += ",allowable_ttc="  + doubleToString(allowable_ttc,2);
  str += ",min_util_cpa="   + doubleToString(min_util_cpa,2);
  str += ",max_util_cpa="   + doubleToString(max_util_cpa,2);
  str += ",pwt_outer_dist=" + doubleToString(pwt_outer_dist,2);
  str += ",pwt_inner_dist=" + doubleToString(pwt_inner_dist,2);
  str += ",completed_dist=" + doubleToString(completed_dist,2);

  postRepeatableMessage("BHV_SETTINGS", str);
}

//-----------------------------------------------------------
// Procedure: getDoubleInfo()

double BHV_AvoidObstacleX::getDoubleInfo(string str)
{
  if(str == "osx")
    return(m_obship_model.getOSX());
  else if(str == "osy")
    return(m_obship_model.getOSY());
  else if(str == "osh")
    return(m_obship_model.getOSH());
  else if(str == "allowable_ttc")
    return(m_obship_model.getAllowableTTC());
  else if(str == "pwt_outer_dist")
    return(m_obship_model.getPwtOuterDist());
  else if(str == "pwt_inner_dist")
    return(m_obship_model.getPwtInnerDist());
  else if(str == "completed_dist")
    return(m_obship_model.getCompletedDist());
  else if(str == "min_util_cpa")
    return(m_obship_model.getMinUtilCPA());
  else if(str == "max_util_cpa")
    return(m_obship_model.getMaxUtilCPA());

  return(0);
}

