/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_Convoy.cpp                                       */
/*    DATE: Jul 28th 2018                                        */
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

#include <cmath>
#include <cstdlib>
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BHV_Convoy.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Convoy::BHV_Convoy(IvPDomain gdomain) : 
  IvPContactBehavior(gdomain)
{
  this->setParam("descriptor", "convoy");
  
  m_domain = subDomain(m_domain, "course,speed");

  // Initialize State variables
  m_next_id = 0;
  m_wptx    = 0;
  m_wpty    = 0;
  m_set_speed = 0;

  m_cnv_avg_2sec = 0;
  m_cnv_avg_5sec = 0;

  m_total_length = 0;
  
  // Initialize Config variables
  m_radius    = 5;   // meters
  m_nm_radius = 20;  // meters

  m_cruise_speed = -1;
  m_spd_max    = 2.0;
  m_spd_faster = 1.5;
  m_spd_slower = 0.5;
  
  m_inter_mark_range = 10;   // meters
  m_max_mark_range   = 150;  // meters

  m_marks_up_bound = 200; 

  m_rng_estop   = 15;
  m_rng_tgating = 30;
  m_rng_lagging = 70;
  
  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam
//  
// "trail_range": desired range to the vehicle trailed.
//      "radius": distance to the desired trailing point within
//                which the behavior is "shadowing".
//   "nm_radius": If within this and heading ahead of target slow down

bool BHV_Convoy::setParam(string param, string param_val) 
{
  if(IvPContactBehavior::setParam(param, param_val))
    return(true);
  
  bool handled = false;
  if(param == "nm_radius")
    handled = setNonNegDoubleOnString(m_nm_radius, param_val);
  else if(param == "radius")
    handled = setNonNegDoubleOnString(m_radius, param_val);
  else if(param == "max_mark_range") 
    handled = setPosDoubleOnString(m_max_mark_range, param_val);
  else if(param == "inter_mark_range") 
    handled = setPosDoubleOnString(m_inter_mark_range, param_val);
  else if(param == "cruise_speed") 
    handled = setPosDoubleOnString(m_cruise_speed, param_val);

  else if(param == "spd_max") 
    handled = setPosDoubleOnString(m_spd_max, param_val);
  else if(param == "spd_slower") {
    handled = setPosDoubleOnString(m_spd_slower, param_val);
    if((m_spd_slower < 0) || (m_spd_slower > 1))
      handled = false;
  }
  else if(param == "spd_faster") {
    handled = setPosDoubleOnString(m_spd_faster, param_val);
    if((m_spd_faster < 1) || (m_spd_faster > 3))
      handled = false;
  }
  
  else if(param == "rng_estop") 
    handled = setPosDoubleOnString(m_rng_estop, param_val);
  else if(param == "rng_tgating") 
    handled = setPosDoubleOnString(m_rng_tgating, param_val);
  else if(param == "rng_lagging") 
    handled = setPosDoubleOnString(m_rng_lagging, param_val);

  return(handled);
}


//-----------------------------------------------------------
// Procedure: onSetParamComplete

void BHV_Convoy::onSetParamComplete() 
{
  if(m_inter_mark_range > 0) {
    double bound = (m_max_mark_range / m_inter_mark_range);
    m_marks_up_bound = (unsigned int)(bound + 1);
  }

  //=========================================================
  // Part 1: Examine and adjust range settings
  //=========================================================
  
  bool range_adjustment_made = false;
  // Make sure rng_estop is at least 5 meters
  if(m_rng_estop < 5) {
    m_rng_estop = 5;
    range_adjustment_made = true;
  }
  
  // Make sure rng_lagging >rng_tgating > rng_estop.
  // But don't make rng_tgating smaller.
  // Do our best to fix, but if fixing is done, post a run warning

  if(m_rng_tgating < (m_rng_estop + 10)) {
    m_rng_tgating = m_rng_estop + 10;
    range_adjustment_made = true;
  }
  if(m_rng_lagging < (m_rng_tgating + 10)) {
    m_rng_lagging = m_rng_tgating + 10;  
    range_adjustment_made = true;
  }

  if(range_adjustment_made)
    postWMessage("Safety ranges were auto-adusted bigger.");

  //=========================================================
  // Part 2: Examine and adjust speed settings
  //=========================================================

  if(m_cruise_speed > m_spd_max) {
    m_cruise_speed = m_spd_max;
    postWMessage("Cruise speed auto-capped at max speed.");
  }
}

//-----------------------------------------------------------
// Procedure: onHelmStart()

void BHV_Convoy::onHelmStart() 
{
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_Convoy::onRunState() 
{
  if(!updatePlatformInfo())
    return(0);

  postMessage("TLEN", m_total_length);
  postMessage("QLEN", getQueueLen());

  postMessage("MXRNG", m_max_mark_range);


  
  handleNewContactSpd(m_cnv);
  postMessage("AVG2", m_cnv_avg_2sec);
  postMessage("AVG5", m_cnv_avg_5sec);
  
  bool new_mark  = handleNewContactPos(m_cnx, m_cny);
  bool drop_mark = checkDropOldestMark();
  if(new_mark || drop_mark)
    updateVisuals();

  // Part 1: Get the next waypoint if there is one
  m_wptx = getOldestX();
  m_wpty = getOldestY();

  bool wpt_active = false;
  bool done = false;
  while(!done) {

    double dist = distPointToPoint(m_osx, m_osy, m_wptx, m_wpty);
    if(dist < m_radius) {
      popOldPoint();
      if(getQueueLen() == 0)
	done = true;
      else {
	m_wptx = getOldestX();
	m_wpty = getOldestY();
      }
    }
    else {
      done = true;
      wpt_active = true;
    }
  }

  postMessage("WPTX", m_wptx);
  postMessage("WPTY", m_wpty);
  
  if(!wpt_active)
    return(0);

  updateSetSpeed();
  
  // Part 2: Generate the IvP function 
  IvPFunction *ipf = buildOF();
  
  return(ipf);
}


//-----------------------------------------------------------
// Procedure: handleNewContactSpd()

void BHV_Convoy::handleNewContactSpd(double cnv)
{
  double curr_time = getBufferCurrTime();
  m_cn_spd_value.push_front(cnv);
  m_cn_spd_tstamp.push_front(curr_time);

  // Part 1A: Ensure behavior spd queue is no older than 5 secs.
  bool done = false;
  while(!done && !m_cn_spd_tstamp.empty()) {
    if((curr_time - m_cn_spd_tstamp.back()) > 5) {
      m_cn_spd_value.pop_back();
      m_cn_spd_tstamp.pop_back();
    }
    else
      done = true;
  }

  // Part 1B: Get the 5 second average
  double total_spd = 0;
  unsigned int cnt = 0;
  list<double>::iterator p;
  for(p=m_cn_spd_value.begin(); p!=m_cn_spd_value.end(); p++) {
    double this_spd = *p;
    total_spd += this_spd;
    cnt++;
  }
  if(cnt > 0)
    m_cnv_avg_5sec = total_spd / (double)(cnt);

  // Part 2A: Build a copy of the spd queue only over 2 secs
  list<double> cn_spd_value  = m_cn_spd_value;
  list<double> cn_spd_tstamp = m_cn_spd_tstamp;
  done = false;
  while(!done && !cn_spd_tstamp.empty()) {
    if((curr_time - cn_spd_tstamp.back()) > 2) {
      cn_spd_value.pop_back();
      cn_spd_tstamp.pop_back();
    }
    else
      done = true;
  }

  // Part 2B: Get the 2 second average
  total_spd = 0;
  cnt = 0;
  for(p=cn_spd_value.begin(); p!=cn_spd_value.end(); p++) {
    double this_spd = *p;
    total_spd += this_spd;
    cnt++;
  }
  if(cnt > 0)
    m_cnv_avg_2sec = total_spd / (double)(cnt);
}

//-----------------------------------------------------------
// Procedure: updateCurrTrailRange()

void BHV_Convoy::updateCurrTrailRange() 
{
#if 0
  double delta_low = m_rng_follow - m_rng_tgating;
  double delta_hgh = m_rng_lagging - m_rng_follow;
  if((delta_low <= 0) || (delta_hgh <= 0))
    return(0);

  double mid_range_low = m_rng_follow - (0.15 * delta_low);
  double mid_range_hgh = m_rng_follow + (0.15 * delta_hgh);

#endif
}

//-----------------------------------------------------------
// Procedure: updateSetSpeed()

void BHV_Convoy::updateSetSpeed() 
{
  // If ownship is within estop range to contact this overrides all else
  if(m_contact_range < m_rng_estop) {
    m_set_speed = 0;
    return;
  }
  
  // If cruise_speed is set, then this overrides all else
  if(m_cruise_speed > 0) {
    m_set_speed = m_cruise_speed;
    return;
  }

  if(m_contact_range > m_rng_lagging) {
    m_set_speed = m_cnv_avg_2sec * m_spd_faster;
  }
  else if (m_contact_range > m_rng_tgating) {
    m_set_speed = m_cnv_avg_2sec;
  }
  else 
    m_set_speed = m_cnv_avg_2sec * m_spd_slower;

}

//-----------------------------------------------------------
// Procedure: buildOF

IvPFunction *BHV_Convoy::buildOF() 
{
  IvPFunction *ipf = 0;

  IvPFunction *spd_ipf = 0;  
  ZAIC_PEAK spd_zaic(m_domain, "speed");
  double peak_width = m_set_speed / 2;
  spd_zaic.setParams(m_set_speed, peak_width, 1.6, 20, 0, 100);

  spd_ipf = spd_zaic.extractIvPFunction();
  if(!spd_ipf)
    postWMessage("Failure on the SPD ZAIC via ZAIC_PEAK utility");
  
  double rel_ang_to_wpt = relAng(m_osx, m_osy, m_wptx, m_wpty);

  postMessage("RANG", rel_ang_to_wpt);
  
  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setValueWrap(true);
  crs_zaic.setParams(rel_ang_to_wpt, 0, 180, 50, 0, 100);
  int ix = crs_zaic.addComponent();
  crs_zaic.setParams(m_osh, 30, 180, 5, 0, 20, ix);
  
  IvPFunction *crs_ipf = crs_zaic.extractIvPFunction(false);  
  if(!crs_ipf) 
    postWMessage("Failure on the CRS ZAIC");
  
  OF_Coupler coupler;
  ipf = coupler.couple(crs_ipf, spd_ipf, 0.5, 0.5);
  if(!ipf)
    postWMessage("Failure on the CRS_SPD COUPLER");

  return(ipf);
}


//-----------------------------------------------------------
// Procedure: onRunToIdleState

void BHV_Convoy::onRunToIdleState()
{
  list<unsigned int>::iterator p;
  for(p=m_cn_mark_id.begin(); p!=m_cn_mark_id.end(); p++) {
    unsigned int id = *p;
    erasePoint(id);
  }

  m_cn_mark_x.clear();
  m_cn_mark_y.clear();
  m_cn_mark_time.clear();
  m_cn_mark_id.clear();
  m_total_length = 0;
}

//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_Convoy::onIdleState()
{
}

//-----------------------------------------------------------
// Procedure: getRelevance

double BHV_Convoy::getRelevance()
{
  return(0);
}



//-----------------------------------------------------------
// Procedure: handleNewContactPos()
//   Returns: true if queue length changes

bool BHV_Convoy::handleNewContactPos(double cnx, double cny)
{
  if(getQueueLen() == 0) {
    pushNewPoint(cnx, cny);
    return(true);
  }

  // Get the position of the newest
  double newest_x = getNewestX();
  double newest_y = getNewestY();

  double range = distPointToPoint(cnx, cny, newest_x, newest_y);

  if(range > m_inter_mark_range) {
    pushNewPoint(cnx, cny);
    return(true);
  }
    
  return(false);
}

//-----------------------------------------------------------
// Procedure: checkDropOldestMark()
//   Returns: true if queue length changes

bool BHV_Convoy::checkDropOldestMark()
{
  if(getQueueLen() == 0)
    return(false);

  if(m_total_length > m_max_mark_range) {
    popOldPoint();
    return(true);
  }
    
  return(false);
}

//-----------------------------------------------------------
// Procedure: postPoint()

void BHV_Convoy::postPoint(double cnx, double cny, unsigned int id)
{
  XYPoint point(cnx, cny);
  string label = m_us_name + "_" + m_contact + "_" + intToString(id);
  
  point.set_label(label);
  point.set_color("label", "invisible");
  point.set_color("vertex", "white");
  
  string spec = point.get_spec();
  postMessage("VIEW_POINT", spec);
}

//-----------------------------------------------------------
// Procedure: erasePoint()

void BHV_Convoy::erasePoint(unsigned int id)
{
  XYPoint point;
  string label = m_us_name + "_" + m_contact + "_" + intToString(id);
  point.set_label(label);
  point.set_active(false);

  string spec = point.get_spec();
  postMessage("VIEW_POINT", spec);
}

//-----------------------------------------------------------
// Procedure: pushNewPoint()

bool BHV_Convoy::pushNewPoint(double cnx, double cny, double tstamp)
{
  if(getQueueLen() > 0) {
    double newest_x = getNewestX();
    double newest_y = getNewestY();
    double new_leg_len = distPointToPoint(cnx, cny, newest_x, newest_y);
    m_total_length += new_leg_len;
  }

  m_cn_mark_x.push_front(cnx);
  m_cn_mark_y.push_front(cny);
  m_cn_mark_time.push_front(tstamp);
  m_cn_mark_id.push_front(m_next_id);

  postPoint(cnx, cny, m_next_id);
  
  m_next_id++;
  if(m_next_id > m_marks_up_bound)
    m_next_id = 0;
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: popOldPoint()

bool BHV_Convoy::popOldPoint()
{
  if(getQueueLen() == 0)
    return(false);

  erasePoint(m_cn_mark_id.back());

  // get the x,y coords of the mark we're about to pop
  double pop_x = getOldestX();
  double pop_y = getOldestY();

  m_cn_mark_x.pop_back();
  m_cn_mark_y.pop_back();
  m_cn_mark_time.pop_back();
  m_cn_mark_id.pop_back();

  if(getQueueLen() > 0) {
    // get the x,y coords of the oldest mark
    double old_x = getOldestX();
    double old_y = getOldestY();
    double old_leg_len = distPointToPoint(old_x, old_y, pop_x, pop_y);
    m_total_length -= old_leg_len;
  }
  
  return(true);
}

