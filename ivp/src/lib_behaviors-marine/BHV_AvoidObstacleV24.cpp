/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / MIT Cambridge MA            */
/*    FILE: BHV_AvoidObstacleV24.cpp                             */
/*    DATE: Aug 2nd 2006                                         */
/*    DATE: Sep 9th 2019 Rewrite with different AOF and refinery */
/*    DATE: Feb 27th 2021 Further mods related to completion.    */
/*    DATE: Feb 27th 2021 Created AvoidObstacleV21 version       */
/*    DATE: Aug 5th 2023 Created AvoidObstacleV24 version        */
/*    DATE: Aug 5th 2023 Using TurnModel                         */
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
#include "BHV_AvoidObstacleV24.h"
#include "AOF_AvoidObstacleV24.h"
#include "OF_Reflector.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BuildUtils.h"
#include "MacroUtils.h"
#include "RefineryObAvoidV24.h"
#include "XYFormatUtilsPoly.h"
#include "VarDataPairUtils.h"

using namespace std;

//-----------------------------------------------------------
// Constructor()
// Note: Most of the behavior state is contained in the
//       member variable m_obship_model

BHV_AvoidObstacleV24::BHV_AvoidObstacleV24(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "avdobs");
  m_domain = subDomain(m_domain, "course,speed");

  // Initialize config vars
  m_pwt_grade = "linear";

  m_use_refinery     = false;
  m_resolved_pending = false;

  m_resolved_obstacle_var = "OBM_RESOLVED";
  m_draw_buff_min_poly = true;
  m_draw_buff_max_poly = true;
  
  // Initialize state vars
  m_obstacle_relevance = 0;

  m_valid_cn_obs_info = false;

  m_cpa_rng_sofar = -1;
  m_fpa_rng_sofar = -1;
  m_cpa_reported = -1;
  m_cpa_rng_ever = -1;
  m_closing = false;

  initVisualHints();
  addInfoVars("NAV_X, NAV_Y, NAV_HEADING");
  addInfoVars(m_resolved_obstacle_var);
}

//-----------------------------------------------------------
// Procedure: initVisualHints()

void BHV_AvoidObstacleV24::initVisualHints() 
{
  m_hints.setMeasure("vertex_size", 0);
  m_hints.setMeasure("edge_size", 1);
  m_hints.setColor("vertex_color", "gray50");
  m_hints.setColor("edge_color", "gray50");
  m_hints.setColor("fill_color", "off");
  m_hints.setColor("label_color", "white");

  m_hints.setColor("obst_edge_color", "white");
  m_hints.setColor("obst_vertex_color", "white");
  m_hints.setColor("obst_fill_color", "gray60");
  m_hints.setMeasure("obst_vertex_size", 1);
  m_hints.setMeasure("obst_fill_transparency", 0.7);
  
  m_hints.setColor("buff_min_edge_color", "gray60");
  m_hints.setColor("buff_min_vertex_color", "dodger_blue");
  m_hints.setColor("buff_min_fill_color", "gray70");
  m_hints.setColor("buff_min_label_color", "off");
  m_hints.setMeasure("buff_min_vertex_size", 1);
  m_hints.setMeasure("buff_min_fill_transparency", 0.25);
  
  m_hints.setColor("buff_max_edge_color", "gray60");
  m_hints.setColor("buff_max_vertex_color", "dodger_blue");
  m_hints.setColor("buff_max_fill_color", "gray70");
  m_hints.setColor("buff_max_label_color", "off");
  m_hints.setMeasure("buff_max_vertex_size", 1);
  m_hints.setMeasure("buff_max_fill_transparency", 0.1);
}

//-----------------------------------------------------------
// Procedure: setParam()

bool BHV_AvoidObstacleV24::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  double dval = atof(val.c_str());
  bool   non_neg_number = (isNumber(val) && (dval >= 0));

  string config_result;
  if((param=="polygon") || (param=="poly")) {
    config_result = m_obship_model.setGutPoly(val);
  }
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

  else if(param == "draw_buff_min_poly") 
    return(setBooleanOnString(m_draw_buff_min_poly, val));
  else if(param == "draw_buff_max_poly") 
    return(setBooleanOnString(m_draw_buff_max_poly, val));

  //else if((param == "turn_radius") && non_neg_number)
  //  return(m_obship_model.setTurnModelRadius(dval));
  //else if((param == "turn_model_degs") && non_neg_number)
  //  return(m_obship_model.setTurnModelDegs(dval));

  else if(param == "rng_flag")
    return(handleParamRangeFlag(val));
  else if(param == "cpa_flag")
    return(addVarDataPairOnString(m_cpa_flags, val));
  else if(param == "visual_hints")
    return(m_hints.setHints(val));
  else if(param == "use_refinery")
    return(setBooleanOnString(m_use_refinery, val));
  else if(param == "id") {
    // Once the obstacle id is set, it cannot be overwritten
    if((m_obstacle_id != "") && (m_obstacle_id != val))
      return(false);
    return(setNonWhiteVarOnString(m_obstacle_id, val));
  }
  //else if(param == "radius")
  //  return(m_tm_generator.setParam("radius", dval));
  //else if(param == "spoke_degs")
  //  return(m_tm_generator.setParam("spoke_degs", dval));

  else
    return(false);

  if(config_result != "") {
    postBadConfig(config_result);
    return(false);
  }

  return(true);
}

//-----------------------------------------------------------
// Procedure: handleParamRangeFlag()
//   Example: rng_flag = <100 RNG_INFO = $[RNG]
//            rng_flag = RNG_INFO = range=$[RNG],speed=$[SPD]
//      Note: Whenever a range threshold is satisfied, flag is posted.
//            So it will be posted continuously when in range

bool BHV_AvoidObstacleV24::handleParamRangeFlag(string str)
{
  double thresh = -1;

  if(str.length() == 0)
    return(false);

  if(str[0] == '<') {
    biteString(str, '<');
    
    string rng_str = biteStringX(str, ' ');
    if(!isNumber(rng_str))
      return(false);
    thresh = atof(rng_str.c_str());
  }
  
  // Sanity checks on the value part
  bool ok = addVarDataPairOnString(m_rng_flags, str);
  if(ok)
    m_rng_thresh.push_back(thresh);

  return(true);
}

//-----------------------------------------------------------
// Procedure: onSetParamComplete()

void BHV_AvoidObstacleV24::onSetParamComplete()
{
  m_obship_model.setPlatModel(m_plat_model);
  m_obship_model.setCachedVals(true);
  postConfigStatus();
}

//-----------------------------------------------------------
// Procedure: onHelmStart()

void BHV_AvoidObstacleV24::onHelmStart()
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

void BHV_AvoidObstacleV24::onEveryState(string str)
{
  // =================================================================
  // Part 1: Check for completion based on obstacle manager 
  // =================================================================
  // Get list of all obstacles declared to be resolved by obstacle mgr
  bool ok = true;
  vector<string> obstacles_resolved;
  obstacles_resolved = getBufferStringVector(m_resolved_obstacle_var, ok);
  
  // Check ids of all resolved obstacles against our own id. If match
  // then declare the resolution to be pending.
  for(unsigned int i=0; i<obstacles_resolved.size(); i++) {
    string obstacle_id = obstacles_resolved[i];
    cout << "Resolved Obstacle: " << obstacle_id << endl;
    postMessage("NOTED_RESOLVED", obstacle_id);

    if(m_obstacle_id == obstacle_id)
      m_resolved_pending = true;
  }


  // =================================================================
  // Part 2: Update the platform info and obship model
  // =================================================================
  if(!updatePlatformInfo())
    postWMessage("Invalid update of ownship position");

  // =================================================================
  // Part 2A: For now, until TurnModels come from the helm...
  // Use the TurnModelGenerator to generate a turn_model based on 
  // =================================================================

  m_obship_model.setPlatModel(m_plat_model);
  m_obship_model.setCachedVals();

  m_valid_cn_obs_info = true;
  if(!m_obship_model.isValid()) {
    cout << "invalid cn_ob_info2" << endl;
    m_valid_cn_obs_info = false;
  }
  if(!m_valid_cn_obs_info)
    postWMessage("Invalid update of ownship/obship model");
  
  if(m_obship_model.getFailedExpandPolyStr(false) != "") {
    string msg = m_obship_model.getFailedExpandPolyStr();
    postWMessage(msg);
  }
  if(!m_valid_cn_obs_info)
    return;

  double os_range_to_poly = m_obship_model.getRange();
  if((m_cpa_rng_ever < 0) || (os_range_to_poly < m_cpa_rng_ever))
    m_cpa_rng_ever = os_range_to_poly;
  m_cpa_reported = m_cpa_rng_ever;

  // =================================================================
  // Part 3: Handle Range Flags if any
  // =================================================================
  if(m_rng_thresh.size() != m_rng_flags.size())
    postWMessage("Range flag mismatch");
  else {
    vector<VarDataPair> rng_flags;
    for(unsigned int i=0; i<m_rng_thresh.size(); i++) {
      double thresh = m_rng_thresh[i];
      if((thresh <= 0) || (os_range_to_poly < thresh))
	rng_flags.push_back(m_rng_flags[i]);
    }
    postFlags(rng_flags);
  }

  // =================================================================
  // Part 4: Handle CPA Flags if any, if a CPA event is observed
  // =================================================================
  // Part 4A: Check for CPA Event
  bool cpa_event = false;
  if((m_cpa_rng_sofar < 0) || (m_fpa_rng_sofar < 0)) {
    m_cpa_rng_sofar = os_range_to_poly;
    m_fpa_rng_sofar = os_range_to_poly;
  }
  
  if(m_closing) {
    if(os_range_to_poly < m_cpa_rng_sofar)
      m_cpa_rng_sofar = os_range_to_poly;
    if(os_range_to_poly > (m_cpa_rng_sofar + 1)) {
      m_closing = false;
      cpa_event = true;
      m_fpa_rng_sofar = os_range_to_poly;
    }
  }
  else {
    if(os_range_to_poly > m_fpa_rng_sofar)
      m_fpa_rng_sofar = os_range_to_poly;
    if(os_range_to_poly < (m_fpa_rng_sofar - 1)) {
      m_closing = true;
      m_cpa_rng_sofar = os_range_to_poly;
    }
  }

  // Part 4B: If CPA event observed, post CPA flags if any
  // NOTE: When posting CPA events, the $[CPA] macro will expand to the CPA
  //       value for this encounter. Otherwise $[CPA] is min CPA ever.
  if((cpa_event) && (os_range_to_poly < m_obship_model.getPwtOuterDist())) {
    m_cpa_reported = m_cpa_rng_sofar;
    postFlags(m_cpa_flags);
    m_cpa_reported = m_cpa_rng_ever;
  }
  
  // =================================================================
  // Part 5: Check for completion based on range
  // =================================================================
  if(os_range_to_poly > m_obship_model.getCompletedDist()) {
    cout << "os_range_to_poly:" << os_range_to_poly << endl;
    cout << "complet_dist: " << m_obship_model.getCompletedDist() << endl;
    m_resolved_pending = true;
  }
}

//-----------------------------------------------------------
// Procedure: onIdleState()

void BHV_AvoidObstacleV24::onIdleState()
{
  postErasablePolygons();

  if(m_resolved_pending)
    setComplete();
}

//-----------------------------------------------------------
// Procedure: onIdleToRunState()

void BHV_AvoidObstacleV24::onIdleToRunState()
{
  postConfigStatus();
}

//-----------------------------------------------------------
// Procedure: onRunState()

IvPFunction *BHV_AvoidObstacleV24::onRunState() 
{
  // Part 1: Handle if obstacle has been resolved
  if(m_resolved_pending) {
    setComplete();
    //cout << "reason1" << endl;
    return(0);
  }
  if(!m_valid_cn_obs_info)
    return(0);

  m_obship_model.setCachedVals();

  // Part 2: No IvP function if obstacle is aft
  if(m_obship_model.isObstacleAft(20)) {
    //cout << "reason3" << endl;
    return(0);
  }
  
  // Part 3: Determine the relevance
  m_obstacle_relevance = getRelevance();
  if(m_obstacle_relevance <= 0)
    return(0);

  IvPFunction *ipf = buildOF();
  return(ipf);
}

//-----------------------------------------------------------
// Procedure: buildOF()

IvPFunction *BHV_AvoidObstacleV24::buildOF()
{
  // Part 1: Set and init the AOF
  AOF_AvoidObstacleV24  aof_avoid(m_domain);
  aof_avoid.setObShipModel(m_obship_model);
  bool ok_init = aof_avoid.initialize();
  if(!ok_init) {
    string aof_msg = aof_avoid.getCatMsgsAOF();
    postWMessage("Unable to init AOF_AvoidObstacleV24:"+aof_msg);
    //cout << "reason5" << endl;
    return(0);
  }
  
  // Part 2: Build the actual objective function with reflector
  OF_Reflector reflector(&aof_avoid, 1);

  // ===========================================================
  // Utilize the Refinery to identify plateau, and basin regions
  // ===========================================================
  if(m_use_refinery) {
    RefineryObAvoidV24 refinery(m_domain);
    //refinery.setVerbose(true);
    refinery.setSideLock(m_side_lock);
    refinery.setRefineRegions(m_obship_model);

    vector<IvPBox> plateau_regions = refinery.getPlateaus();
    vector<IvPBox> basin_regions   = refinery.getBasins();

    for(unsigned int i=0; i<plateau_regions.size(); i++) 
      reflector.setParam("plateau_region", plateau_regions[i]);
    for(unsigned int i=0; i<basin_regions.size(); i++)
      reflector.setParam("basin_region", basin_regions[i]);   
  }

  
  
  if(m_build_info != "")
    reflector.create(m_build_info);
  else {
    reflector.setParam("uniform_piece", "discrete@course:3,speed:3");
    reflector.setParam("uniform_grid",  "discrete@course:9,speed:9");
    reflector.create();
  }
  if(!reflector.stateOK()) {
    postWMessage(reflector.getWarnings());
    return(0);
  }

  // Part 3: Extract IPF, apply priority, post visuals
  IvPFunction *ipf = reflector.extractIvPFunction(true); // true normalize
  if(ipf) {
    ipf->setPWT(m_obstacle_relevance * m_priority_wt);
    postViewablePolygons();
  }

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: getRelevance()
//            Calculate the relevance first. If zero-relevance, 
//            we won't bother to create the objective function.

double BHV_AvoidObstacleV24::getRelevance()
{
  // Let the ObShipModel tell us the raw range relevance
  double range_relevance = m_obship_model.getRangeRelevance();
  if(range_relevance <= 0)
    return(0);

  if(range_relevance > 0.6) {
    if(m_side_lock == "") {
      if(m_obship_model.getPassingSide() == "star")
	m_side_lock = "port";
      else if(m_obship_model.getPassingSide() == "port")
	m_side_lock = "star";
      else 
	m_side_lock = "";
    }
  }
  else
    m_side_lock = "";
  
  // Part 2: Possibly apply the grade scale to the raw distance
  double relevance = range_relevance;
  if(m_pwt_grade == "quadratic")
    relevance = range_relevance * range_relevance;
  else if(m_pwt_grade == "quasi")
    relevance = pow(range_relevance, 1.5);

  return(relevance);
}

//-----------------------------------------------------------
// Procedure: postViewablePolygons()

void BHV_AvoidObstacleV24::postViewablePolygons()
{
  // =================================================
  // Part 1 - Render the gut (physical) polygon
  // =================================================
  XYPolygon gut_poly = m_obship_model.getGutPoly();
  gut_poly.set_active(true);
  // If the obstacle is relevant, perhaps draw filled in
  if(m_obstacle_relevance > 0) {
    if(m_side_lock != "")
      m_hints.setColor("gut_fill_color", "pink");
    else
      m_hints.setColor("gut_fill_color", "gray60");
  }
  else
    m_hints.setColor("gut_fill_color", "off");

  applyHints(gut_poly, m_hints, "gut");
  postMessage("VIEW_POLYGON", gut_poly.get_spec(5), "gut");
    
  // =================================================
  // Part 2 - Render the mid polygon
  // =================================================
  XYPolygon mid_poly = m_obship_model.getMidPoly();
  mid_poly.set_active(true);
  // If the obstacle is relevant, perhaps draw filled in
  if(m_obstacle_relevance > 0)
    m_hints.setColor("mid_fill_color", "gray70");
  else
    m_hints.setColor("mid_fill_color", "off");

  applyHints(mid_poly, m_hints, "mid");
  postMessage("VIEW_POLYGON", mid_poly.get_spec(5), "mid");

  // =================================================
  // Part 3 - Render the rim (outermost) polygon
  // =================================================
  XYPolygon rim_poly = m_obship_model.getRimPoly();
  rim_poly.set_active(true);
  
  // If the obstacle is relevant, perhaps draw filled in
  if(m_obstacle_relevance > 0)
    m_hints.setColor("rim_fill_color", "gray70");
  else
    m_hints.setColor("rim_fill_color", "off");

  applyHints(rim_poly, m_hints, "rim");
  postMessage("VIEW_POLYGON", rim_poly.get_spec(5), "rim");
}


//-----------------------------------------------------------
// Procedure: postErasablePolygons()

void BHV_AvoidObstacleV24::postErasablePolygons()
{
  XYPolygon gut_poly = m_obship_model.getGutPoly();
  postMessage("VIEW_POLYGON", gut_poly.get_spec_inactive(), "gut");

  XYPolygon mid_poly = m_obship_model.getMidPoly();
  postMessage("VIEW_POLYGON", mid_poly.get_spec_inactive(), "mid");

  XYPolygon rim_poly = m_obship_model.getRimPoly();
  rim_poly.set_color("fill", "invisible");
  postMessage("VIEW_POLYGON", rim_poly.get_spec_inactive(), "rim");

  
}

//-----------------------------------------------------------
// Procedure: updatePlatformInfo()

bool BHV_AvoidObstacleV24::updatePlatformInfo()
{
  bool ok1, ok2, ok3, ok4;
  m_osx = getBufferDoubleVal("NAV_X", ok1);
  m_osy = getBufferDoubleVal("NAV_Y", ok2);
  m_osh = getBufferDoubleVal("NAV_HEADING", ok3);
  m_osv = getBufferDoubleVal("NAV_SPEED", ok4);

  string warning_msg;
  if(!ok1 || !ok2)
    warning_msg = "No Ownship NAV_X and/or NAV_Y in info_buffer";
  if(!ok3)
    warning_msg = "No Ownship NAV_HEADING in info_buffer";
  if(!ok4)
    warning_msg = "No Ownship NAV_HEADING in info_buffer";

#if 0    
    bool ok_update = m_obship_model.setPose(m_osx, m_osy, m_osh);
  if(!ok_update) 
    warning_msg = "Problem updating obship_model pose";
  if(!m_obship_model.getObstacle().is_convex()) 
    warning_msg = "Non-convex Obstacle";
  if(!m_obship_model.getObstacleBuffMin().is_convex()) 
    warning_msg = "Non-convex ObstacleBuffMin";
  if(!m_obship_model.getObstacleBuffMax().is_convex()) 
    warning_msg = "Non-convex ObstacleBuffMax";
#endif
    
  if(warning_msg != "") {
    cout << "Warning:" << warning_msg << endl;
    postWMessage(warning_msg);
    return(false);
  }
  return(true);

}

//-----------------------------------------------------------
// Procedure: postConfigStatus()

void BHV_AvoidObstacleV24::postConfigStatus()
{
  string str = "type=BHV_AvoidObstacleV24,name=" + m_descriptor;

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

double BHV_AvoidObstacleV24::getDoubleInfo(string str)
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

//-----------------------------------------------------------
// Procedure: expandMacros()

string BHV_AvoidObstacleV24::expandMacros(string sdata)
{
  double os_rng_to_poly = m_obship_model.getRange();
  double os_bng_to_poly = m_obship_model.getObcentBng();
  double os_rbng_to_poly = m_obship_model.getObcentRelBng();
  string side = m_obship_model.getPassingSide();
  //double osv = m_obship_model.getOSV();
  string obs_id = m_obship_model.getObstacleLabel();
  
  sdata = macroExpand(sdata, "CPA", m_cpa_reported);
  sdata = macroExpand(sdata, "RNG", os_rng_to_poly);
  sdata = macroExpand(sdata, "BNG", os_bng_to_poly);
  sdata = macroExpand(sdata, "RBNG", os_rbng_to_poly);
  sdata = macroExpand(sdata, "SIDE", side);
  sdata = macroExpand(sdata, "SLOCK", m_side_lock);
  //sdata = macroExpand(sdata, "OSV", osv);
  //sdata = macroExpand(sdata, "SPD", osv);
  sdata = macroExpand(sdata, "OID", obs_id);
  //sdata = macroExpand(sdata, "PWT", m_);

  return(sdata);
}
