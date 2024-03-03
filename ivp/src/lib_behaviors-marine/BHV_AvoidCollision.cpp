/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_AvoidCollision.cpp                               */
/*    DATE: Nov 18th 2006                                        */
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
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "AOF_AvoidCollision.h"
#include "AOF_AvoidCollisionDepth.h"
#include "BHV_AvoidCollision.h"
#include "OF_Reflector.h"
#include "RefineryCPA.h"
#include "BuildUtils.h"
#include "MBUtils.h"
#include "CPA_Utils.h"
#include "XYSegList.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_AvoidCollision::BHV_AvoidCollision(IvPDomain gdomain) : 
  IvPContactBehavior(gdomain)
{
  this->setParam("descriptor", "avoid_collision");
  this->setParam("build_info", "uniform_piece = discrete @ course:3,speed:3");
  this->setParam("build_info", "uniform_grid  = discrete @ course:9,speed:6");
  
  if(m_domain.hasDomain("depth"))
    m_domain = subDomain(m_domain, "course,speed,depth");
  else
    m_domain = subDomain(m_domain, "course,speed");
  
  m_collision_depth   = 0;

  m_completed_dist    = 500;
  m_pwt_outer_dist    = 200;
  m_pwt_inner_dist    = 50;
  m_min_util_cpa_dist = 10; 
  m_max_util_cpa_dist = 75; 
  m_pwt_grade         = "quasi";
  m_roc_max_dampen    = -2.0; 
  m_roc_max_heighten  = 2.0; 
  m_bearing_line_show = false;
  m_time_on_leg       = 120;  // Overriding the superclass default=60

  m_no_alert_request  = false;

  // Initialize state variables
  m_curr_closing_spd  = 0;
  m_total_evals       = 0;
  m_avoiding          = false;

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING, AVOIDING");

  // Release 19.8 additions
  m_use_refinery   = false;
  m_check_plateaus = false;
  m_check_validity = false;
  m_pcheck_thresh  = 0.001;
  m_verbose        = false;
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_AvoidCollision::setParam(string param, string param_val) 
{
  if(IvPContactBehavior::setParam(param, param_val))
    return(true);

  double dval = atof(param_val.c_str());
  bool non_neg_number = (isNumber(param_val) && (dval >= 0));

  if(param == "pwt_inner_dist") {
    return(setMinPartOfPairOnString(m_pwt_inner_dist,
				    m_pwt_outer_dist, param_val));
  }  
  else if(param == "pwt_outer_dist") { 
    return(setMaxPartOfPairOnString(m_pwt_inner_dist,
				    m_pwt_outer_dist, param_val));
  }

  else if(param == "min_util_cpa_dist") {
    return(setMinPartOfPairOnString(m_min_util_cpa_dist,
				    m_max_util_cpa_dist, param_val));
  }    
  else if(param == "max_util_cpa_dist") {
    return(setMaxPartOfPairOnString(m_min_util_cpa_dist,
				    m_max_util_cpa_dist, param_val));
  }
  
  else if(param == "completed_dist")
    return(setNonNegDoubleOnString(m_completed_dist, param_val));
  else if((param == "contact_type_required") && (param_val != "")) 
    return(IvPContactBehavior::setParam("match_type", param_val));

  else if(param == "collision_depth") {
    if(dval <= 0)
      return(false);
    if(!m_domain.hasDomain("depth"))
      return(false);
    if(dval >= m_domain.getVarHigh("depth"))
      return(true);  // Should not be considered a config error
    m_collision_depth = dval;
    return(true);
  }  
  
  else if(param == "pwt_grade") {
    param_val = tolower(param_val);
    if((param_val!="linear") && (param_val!="quadratic") && 
       (param_val!="quasi"))
      return(false);
    m_pwt_grade = param_val;
    return(true);
  }  
  else if(param == "roc_max_heighten") {
    if(!non_neg_number)
      return(false);
    m_roc_max_heighten = dval;
    if(m_roc_max_dampen > m_roc_max_heighten)
      m_roc_max_dampen = m_roc_max_heighten;
    return(true);
  }  
  else if(param == "roc_max_dampen") {
    if(!non_neg_number)
      return(false);
    m_roc_max_dampen = dval;
    if(m_roc_max_heighten < m_roc_max_heighten)
      m_roc_max_heighten = m_roc_max_dampen;
    return(true);
  }  
  else if(param == "no_alert_request")
    return(setBooleanOnString(m_no_alert_request, param_val));
  else if(param == "verbose")
    return(setBooleanOnString(m_verbose, param_val));
  else if(param == "use_refinery")
    return(setBooleanOnString(m_use_refinery, param_val));
  else if(param == "check_plateaus")
    return(setBooleanOnString(m_check_plateaus, param_val));
  else if(param == "check_validity") 
    return(setBooleanOnString(m_check_validity, param_val));
  else if(param == "pcheck_thresh")
    return(setNonNegDoubleOnString(m_pcheck_thresh, param_val));

  // Safety check, in case user did not explicitly set completed dist
  if(m_completed_dist < m_pwt_outer_dist)
    m_completed_dist = m_pwt_outer_dist;
  
  return(false);
}


//-----------------------------------------------------------
// Procedure: onHelmStart()
//      Note: This function is called when the helm starts, even if,
//            especially if, the behavior is just a template at start
//            time to be spawned later. 
//      Note: An alert request will be sent to the contact manager if
//            the behavior is configured with templating enabled, and
//            an updates variable has been provided.  In the rare case
//            that the above is true but the user still does not want
//            an alert request generated, this can be done by setting
//            m_no_alert_request to true.

void BHV_AvoidCollision::onHelmStart()
{
  if(m_no_alert_request || (m_update_var == "") || !m_dynamically_spawnable)
    return;

  string alert_templ = m_update_var + "=name=$[VNAME] # contact=$[VNAME]";
  string request = "id=" + getDescriptor();
  request += ", on_flag=" + alert_templ;
  request += ",alert_range=" + doubleToStringX(m_pwt_outer_dist,1);
  request += ", cpa_range=" + doubleToStringX(m_completed_dist,1);
  request = augmentSpec(request, getFilterSummary());
  
  postMessage("BCM_ALERT_REQUEST", request);
}

//-----------------------------------------------------------
// Procedure: onIdleState()

void BHV_AvoidCollision::onIdleState() 
{
  if(!updatePlatformInfo())
    return;

  postRange();

  // See the Mod July 30th, 2022 comment below.
  if(!filterCheckHolds() || (m_contact_range >= (m_completed_dist * 1.1)))
    setComplete();  
}

//-----------------------------------------------------------
// Procedure: onInactiveState()

void BHV_AvoidCollision::onInactiveState() 
{
  if(m_bearing_line_show)
    postViewableBearingLine(false);
}

//-----------------------------------------------------------
// Procedure: getInfo()

string BHV_AvoidCollision::getInfo(string str) 
{
  if(str == "debug1")
    return(uintToString(m_total_evals));

  return("");
}

//-----------------------------------------------------------
// Procedure: onRunState()

IvPFunction *BHV_AvoidCollision::onRunState() 
{
  m_total_evals = 0;
  if(!platformUpdateOK()) {
    postRange();
    return(0);
  }
  
  postRange();

  // Mod July 30th, 2022: Changed the actual comleted distance to be
  // 10pct greater than set completed distance. This is to ensure that
  // the behavior isn't spawned and killed immediately. If the
  // spawning and completed distances are exactly the same, then it's
  // possible that, if the entity doing the spawning and the helm
  // disagree slightly on the range between vessels due to timing, the
  // behavior will meet the complete threshold immediately, and the
  // entity doing the spawning may not notice and re-alert the helm
  // since from its perspective the range never crossedb back over the
  // threshhold for sending a new alert.
  
  if(!filterCheckHolds() || (m_contact_range >= (m_completed_dist * 1.1))) {
    setComplete();
    return(0);
  }
  
  m_relevance = getRelevance();
  if(m_relevance <= 0)
    return(0);

  IvPFunction *ipf = 0;
  if(m_collision_depth > 0)
    ipf = getAvoidDepthIPF();
  else
    ipf = getAvoidIPF();

  
  if(ipf) {
    ipf->getPDMap()->normalize(0, 100);
    ipf->setPWT(m_relevance * m_priority_wt);
  }

  postViewableBearingLine();

  return(ipf);
}



//-----------------------------------------------------------
// Procedure: getAvoidIPF()

IvPFunction *BHV_AvoidCollision::getAvoidIPF()
{
  // ===========================================================
  // Prepare the AOF to be passed to the Reflector
  // ===========================================================
  double min_util_cpa_dist = m_min_util_cpa_dist;
  if(m_contact_range <= m_min_util_cpa_dist)
    min_util_cpa_dist = (m_contact_range / 2);
  
  AOF_AvoidCollision aof(m_domain);
  aof.setCPAEngine(m_cpa_engine);
  aof.setOwnshipParams(m_osx, m_osy);
  aof.setContactParams(m_cnx, m_cny, m_cnh, m_cnv);
  aof.setParam("tol", m_time_on_leg);
  aof.setParam("collision_distance", min_util_cpa_dist);
  aof.setParam("all_clear_distance", m_max_util_cpa_dist);
  bool ok = aof.initialize();
  
  if(!ok) {
    postEMessage("Unable to init AOF_AvoidCollision.");
    return(0);
  }    
  
  OF_Reflector reflector(&aof, 1);
  m_domain = subDomain(m_domain, "course,speed");
  
  // ===========================================================
  // Utilize the Refinery to identify plateau regions
  // ===========================================================
  if(m_use_refinery) {
    RefineryCPA refinery;
    refinery.init(m_osx, m_osy, m_cnx, m_cny, m_cnh, m_cnv, m_time_on_leg,
		  m_min_util_cpa_dist, m_max_util_cpa_dist, m_domain,
		  &m_cpa_engine);
    refinery.setVerbose(m_verbose);
    
    vector<IvPBox> regions;
    regions = refinery.getRefineRegions();
    m_total_evals = refinery.getTotalQueriesCPA();

    for(unsigned int i=0; i<regions.size(); i++) {
      reflector.setParam("plateau_region", regions[i]);
      //string warnings = reflector.getWarnings();
      //postMessage("AVD_DEBUG", "["+intToString(i)+"]: "+warnings);
    }

    // Check plateaus code was used to verify correctness of plateaus
    // and remains for future validation.
    if(m_check_plateaus) {
      reflector.setParam("pcheck_thresh", m_pcheck_thresh);
      double worst_fail = reflector.checkPlateaus(true);
      bool ok_plateaus = (worst_fail == 0);

      if(m_verbose) {
	cout.precision(15);
	cout << "Worst fail: " << worst_fail << endl;
	cout << "ok_plateaus: " << boolToString(ok_plateaus) << endl;
	cout << "  BHV_AvoidCollision checkPlateaus   START()" << endl;
	cout << "  BHV_AvoidCollision plat_thresh: " <<
	  m_pcheck_thresh << endl;
	cout << "Plateaus OK: " << boolToString(ok_plateaus) << endl;
	cout << "  BHV_AvoidCollision checkPlateaus     END()" << endl;
	postBoolMessage("PLATEAU_CHECK_OK", ok_plateaus);
	postRepeatableMessage("PLATEAU_LOGIC_CASE",
			      refinery.getLogicCase());
      }

      if(!ok_plateaus)
	postRepeatableMessage("PLATEAU_WORST_FAIL", worst_fail);
    }
  }

  // ===========================================================
  // Build the IvP Function
  // ===========================================================
  reflector.create(m_build_info);
  IvPFunction *ipf = reflector.extractIvPFunction();

  m_total_evals += reflector.getTotalEvals();
  
  string warnings = reflector.getWarnings();
  if(warnings != "")
    postMessage("AVD_DEBUG", warnings);

  if(m_check_validity) {
    bool valid_ipf = true;
    if(ipf) 
      valid_ipf = ipf->valid();
 
    if(m_verbose)
      cout << boolToString(valid_ipf) << endl;
    postBoolMessage("VALID_CHECK_OK", valid_ipf);
  }
    
  return(ipf);
}

//-----------------------------------------------------------
// Procedure: getAvoidDepthIPF()

IvPFunction *BHV_AvoidCollision::getAvoidDepthIPF()
{
  AOF_AvoidCollisionDepth aof(m_domain);
  bool ok = true;
  ok = ok && aof.setParam("osy", m_osy);
  ok = ok && aof.setParam("osx", m_osx);
  ok = ok && aof.setParam("osh", m_osh);
  ok = ok && aof.setParam("osv", m_osv);
  ok = ok && aof.setParam("cny", m_cny);
  ok = ok && aof.setParam("cnx", m_cnx);
  ok = ok && aof.setParam("cnh", m_cnh);
  ok = ok && aof.setParam("cnv", m_cnv);
  ok = ok && aof.setParam("tol", m_time_on_leg);
  ok = ok && aof.setParam("collision_distance", m_min_util_cpa_dist);
  ok = ok && aof.setParam("all_clear_distance", m_max_util_cpa_dist);
  ok = ok && aof.setParam("collision_depth", m_collision_depth);
  ok = ok && aof.initialize();
 
  //double roc = aof.getROC();
  //postMessage("ROC", roc);

  if(!ok) {
    postEMessage("Unable to init AOF_AvoidCollision.");
    return(0);
  }    
  OF_Reflector reflector(&aof, 1);
  
  unsigned int index = (unsigned int)(m_domain.getIndex("depth"));
  unsigned int disc_val = m_domain.getDiscreteVal(index, m_collision_depth, 1);
  
  IvPBox region_bot = domainToBox(m_domain);
  region_bot.pt(index,0) = disc_val;
  
  IvPBox region_top = domainToBox(m_domain);
  region_top.pt(index,1) = disc_val-1;
  
  reflector.setParam("uniform_amount", "1");
  reflector.setParam("refine_region", region_top);
  reflector.setParam("refine_piece", "discrete @ course:10,speed:5,depth:100");
  
  reflector.setParam("refine_region", region_bot);
  reflector.setParam("refine_piece", region_bot);
  reflector.create();
  IvPFunction *ipf = reflector.extractIvPFunction();
  
  m_total_evals = reflector.getTotalEvals();
  
  string warnings = reflector.getWarnings();
  postMessage("AVD_STATUS", warnings);

  return(ipf);
}


//-----------------------------------------------------------
// Procedure: getRelevance
//            Calculate the relevance first. If zero-relevance, 
//            we won't bother to create the objective function.

double BHV_AvoidCollision::getRelevance()
{
  // First declare the range of relevance values to be calc'ed
  double min_dist_relevance = 0.0;
  double max_dist_relevance = 1.0;
  double rng_dist_relevance = max_dist_relevance - min_dist_relevance;
  
  m_contact_range = hypot((m_osx - m_cnx),(m_osy - m_cny));
  m_curr_closing_spd = closingSpeed(m_osx, m_osy, m_osv, m_osh,
				    m_cnx, m_cny, m_cnv, m_cnh);

  postMessage("AVD_DEBUG", "In getRelevance: " + doubleToString(m_contact_range));
  if(m_contact_range >= m_pwt_outer_dist) {
    return(0);
  }

  double dpct, drange;
  if(m_contact_range <= m_pwt_inner_dist)
    dpct = max_dist_relevance;
  
  // Note: drange should never be zero since either of the above
  // conditionals would be true and the function would have returned.
  drange = (m_pwt_outer_dist - m_pwt_inner_dist);
  dpct = (m_pwt_outer_dist - m_contact_range) / drange;
  
  // Apply the grade scale to the raw distance
  double mod_dpct = dpct; // linear case
  if(m_pwt_grade == "quadratic")
    mod_dpct = dpct * dpct;
  else if(m_pwt_grade == "quasi")
    mod_dpct = pow(dpct, 1.5);

  double d_relevance = (mod_dpct * rng_dist_relevance) + min_dist_relevance;

  return(d_relevance);  
}

//-----------------------------------------------------------
// Procedure: postRange()

void BHV_AvoidCollision::postRange()
{
  // Sanity check: Postings made to variables that contain contact
  // name may be disabled. Set post_per_contact_info=true to enable.
  // By default this is false.
  if(!postingPerContactInfo())
    return;
  
  // Round the speed a bit first so to reduce the number of posts 
  // to the db which are based on change detection.
  double cls_speed = snapToStep(m_curr_closing_spd, 0.1);
  postMessage(("CLSG_SPD_AVD_" + m_contact), cls_speed);
    
  // Post to integer precision unless very close to contact
  if(m_contact_range <= 10)
    postMessage(("RANGE_AVD_" + m_contact), m_contact_range);
  else
    postIntMessage(("RANGE_AVD_" + m_contact), m_contact_range);
}

