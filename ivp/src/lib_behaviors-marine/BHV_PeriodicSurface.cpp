/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_PeriodicSurface.cpp                              */
/*    DATE: Feb 21st 2007                                        */
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
#include "BHV_PeriodicSurface.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_PeriodicSurface::BHV_PeriodicSurface(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "periodic_surface");

  m_domain = subDomain(m_domain, "speed,depth");

  // Behavior Parameter Default Values:
  m_period               = 300; // Seconds
  m_mark_variable        = "GPS_UPDATE_RECEIVED";
  m_acomms_mark_variable = "";
  m_pending_status_var   = "PENDING_SURFACE";
  m_atsurface_status_var = "TIME_AT_SURFACE";
  m_max_time_at_surface  = 300;
  m_ascent_speed         = -1; 
  m_zero_speed_depth     = 0;
  m_ascent_grade         = "linear";

  // Behavior State Variable Initial Values:
  m_first_iteration        = true;
  m_first_marking_string   = true;
  m_first_marking_double   = true;
  m_amark_extension_time   = 0;
  m_curr_amark_string_val  = "";

  m_depth_at_ascent_begin  = 0;
  m_speed_at_ascent_begin  = 0;

  m_curr_mark_string_val   = "";
  m_curr_mark_double_val   = 0;

  m_surface_mark_time      = 0;
  m_at_surface             = false;

  m_curr_depth = 0;

  m_state = "waiting";

  m_ascending_flag = "PERIODIC_ASCEND";

  // Declare information needed by this behavior
  addInfoVars("NAV_DEPTH, NAV_SPEED");
  addInfoVars(m_mark_variable, "no_warning");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_PeriodicSurface::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParam(g_param, g_val))
    return(true);

  g_val = stripBlankEnds(g_val);

  if(g_param == "period") {
    double dval = atof(g_val.c_str());
    if((dval <= 0) || (!isNumber(g_val)))
      return(false);
    m_period = dval;
  } 
  else if(g_param == "mark_variable") {
    if(g_val == "")
      return(false);
    m_mark_variable = g_val;
    addInfoVars(m_mark_variable, "no_warning");
  }
  else if(g_param == "acomms_mark_variable") {
    string variable = stripBlankEnds(biteString(g_val, ','));
    string interval = stripBlankEnds(biteString(g_val, ','));
    string max_time = stripBlankEnds(g_val);
    if((variable == "") || strContainsWhite(variable))
      return(false);
    if((interval == "") || !isNumber(interval))
      return(false);
    if((max_time == "") || !isNumber(max_time))
      return(false);
    
    double d_interval = atof(interval.c_str());
    double d_max_time = atof(max_time.c_str());
    if((d_interval <= 0) || (d_max_time < d_interval))
      return(false);

    m_acomms_mark_variable = variable;
    m_acomms_mark_interval = d_interval;
    m_acomms_mark_max_time = d_max_time;
    
    addInfoVars(m_acomms_mark_variable);
    
    // initialize acomms mark remaining extension time
    m_amark_extension_time = 0;
  }
  else if((g_param == "pending_status_variable") ||
	  (g_param == "pending_status_var")) {
    if(g_val == "")
      return(false);
    m_pending_status_var = g_val;
  }
  else if((g_param == "atsurface_status_variable") ||
	  (g_param == "atsurface_status_var")) {
    if(g_val == "")
      return(false);
    m_atsurface_status_var = g_val;
  }
  else if((g_param == "speed_to_surface") || 
	  (g_param == "ascent_speed")) {
    double dval;
    if(isNumber(g_val)) {
      dval = atof(g_val.c_str());
      if(dval < 0)
	return(false);
    }
    else {
      if(g_val == "current")
	dval = -1;
      else
	return(false);
    }
    m_ascent_speed = dval;
  }
  else if(g_param == "zero_speed_depth") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_zero_speed_depth = dval;
  }
  else if(g_param == "max_time_at_surface") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_max_time_at_surface = dval;
  }
  else if(g_param == "ascent_grade") {
    g_val = tolower(g_val);
    if((g_val!="linear") && (g_val!="quadratic") && 
       (g_val!="quasi") && (g_val != "fullspeed"))
      return(false);
    m_ascent_grade = g_val;
  }
  else  
    return(false);

  return(true);
}

//-----------------------------------------------------------
// Procedure: onIdleState
//      Note: This function overrides the onIdleState() virtual
//            function defined for the IvPBehavior superclass
//            This function will be executed by the helm each
//            time the behavior FAILS to meet its run conditions.

void BHV_PeriodicSurface::onIdleState()
{
  updateInfoIn();
  if((m_curr_depth >= m_zero_speed_depth) && m_first_iteration) {
    m_first_iteration = false;
    m_mark_time = m_curr_time;
  }
  
  double time_since_mark       = m_curr_time - m_mark_time;
  double period_wait_remaining = m_period - time_since_mark;

  // Now consider any extension via acomms marking.
  period_wait_remaining += m_amark_extension_time;

  //  if(period_wait_remaining < 0)
  //  period_wait_remaining = 0;
  
  if(m_pending_status_var != "")
    postIntMessage(m_pending_status_var, period_wait_remaining);

}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_PeriodicSurface::onRunState() 
{
  // Set m_curr_depth, m_curr_speed, m_curr_time
  if(!updateInfoIn()) {
    postMessage(m_pending_status_var, "ERROR");
    return(0);
  }

  double time_since_mark       = m_curr_time - m_mark_time;
  double period_wait_remaining = m_period - time_since_mark;

  // Now consider any extension via acomms marking.
  period_wait_remaining += m_amark_extension_time;

  //  if(period_wait_remaining < 0)
  //  period_wait_remaining = 0;
  
  if(m_pending_status_var != "")
    postIntMessage(m_pending_status_var, period_wait_remaining);

  if(checkForMarking())
    return(0);

  checkForAcommsMarking();
  
  if(period_wait_remaining > 0) {
    m_state = "waiting";
    postMessage(m_atsurface_status_var, 0);
    return(0);
  }

  if(m_state == "waiting") {
    m_state = "ascending";
    postMessage(m_ascending_flag, "true");
    m_depth_at_ascent_begin = m_curr_depth;
    if(m_ascent_speed != -1)
      m_speed_at_ascent_begin = m_ascent_speed;
    else
      m_speed_at_ascent_begin = m_curr_speed;
  }
  
  if(m_state == "ascending") {
    if(m_curr_depth <= m_zero_speed_depth) {
      postMessage(m_ascending_flag, "false");
      m_state = "at_surface";
      m_surface_mark_time = m_curr_time;
    }
  }

  if(m_state == "at_surface") {
    double time_at_surface = m_curr_time - m_surface_mark_time;
    postIntMessage(m_atsurface_status_var, time_at_surface);
    if(time_at_surface >= m_max_time_at_surface) {
      m_mark_time = m_curr_time;
      m_state = "waiting";
      // here we should post GPS_UPDATE_RECEIVED
      string buf = "Timestamp="+doubleToString(m_curr_time,2);
      postMessage("PSF_AT_SURFACE_TIMEOUT", buf);  
      return(0);
    }
  }
  else
    postMessage(m_atsurface_status_var, 0);
  
  double desired_speed;
  if(m_state == "ascending")
    desired_speed = setDesiredSpeed();
  else
    desired_speed = 0;



  ZAIC_PEAK zaic_speed(m_domain, "speed");
  zaic_speed.setSummit(desired_speed);
  if(desired_speed == 0)
    zaic_speed.setBaseWidth(0);
  else
    zaic_speed.setBaseWidth(0.5);
  zaic_speed.setPeakWidth(0);
  zaic_speed.setMinMaxUtil(0, 100);

  IvPFunction *ipf_speed = zaic_speed.extractIvPFunction();


  ZAIC_PEAK zaic_depth(m_domain, "depth");
  zaic_depth.setSummit(0);
  zaic_depth.setBaseWidth(0);
  zaic_depth.setPeakWidth(0);
  zaic_depth.setMinMaxUtil(0, 100);

  IvPFunction *ipf_depth = zaic_depth.extractIvPFunction();

  OF_Coupler coupler;
  IvPFunction *new_ipf = coupler.couple(ipf_speed, ipf_depth);

  new_ipf->setPWT(m_priority_wt);

  return(new_ipf);
}

//-----------------------------------------------------------
// Procedure: updatInfoIn
//

bool BHV_PeriodicSurface::updateInfoIn()
{  
  bool ok1, ok2;
  m_curr_depth = getBufferDoubleVal("NAV_DEPTH", ok1);
  m_curr_speed = getBufferDoubleVal("NAV_SPEED", ok2);
  m_curr_time  = getBufferCurrTime();
  
  if(!ok1 || !ok2) {
    postEMessage("No ownship NAV_DEPTH or NAV_SPEED in info_buffer");
    return(false);
  }
  return(true);
}

//-----------------------------------------------------------
// Procedure: checkForMarking
//
//   Check if a "marking" has occurred. The status_variable is 
//   examined and if different than the previous value, a "marking"
//   is declared to have occurred. 

bool BHV_PeriodicSurface::checkForMarking()
{
  bool mark_noted = false;

  if(m_first_iteration) {
    m_first_iteration = false;
    mark_noted = true;
    m_mark_time = m_curr_time;
  }  
  else {
    bool   ok1, ok2;
    double double_mark_val = getBufferDoubleVal(m_mark_variable, ok1);
    string string_mark_val = getBufferStringVal(m_mark_variable, ok2);
    
    if(ok1) {
      if(m_first_marking_double || 
	 (double_mark_val != m_curr_mark_double_val)) {
	mark_noted = true;
	m_curr_mark_double_val = double_mark_val;
	m_first_marking_double = false;
      }
    }
      
    if(ok2) {
      if(m_first_marking_string || 
	 (string_mark_val != m_curr_mark_string_val)) {
	mark_noted = true;
	m_curr_mark_string_val = string_mark_val;
	m_first_marking_string = false;
      }
    }
  }
      
  if(mark_noted) {
    m_mark_time = m_curr_time;
    m_amark_extension_time = 0;
  }

  return(mark_noted);
}

//-----------------------------------------------------------
// Procedure: checkForAcommsMarking

void BHV_PeriodicSurface::checkForAcommsMarking()
{
  // If no acomms mark variable specified, don't bother with this.
  if(m_acomms_mark_variable == "")
    return;
  // If no extension time remaining, don't bother with this.
  if(m_amark_extension_time >= m_acomms_mark_max_time)
    return;

  bool ok, amark_noted = false;

  string string_amark_val;
  string_amark_val = getBufferStringVal(m_acomms_mark_variable, ok);
    
  if(ok) {
    if(string_amark_val != m_curr_amark_string_val) {
      m_curr_amark_string_val = string_amark_val;
      amark_noted = true;
    }
  }
  
  if(amark_noted) {
    m_amark_extension_time += m_acomms_mark_interval;
    if(m_amark_extension_time > m_acomms_mark_max_time)
      m_amark_extension_time = m_acomms_mark_max_time;
  }
}


//-----------------------------------------------------------
// Procedure: setDesiredSpeed
//
//  Info used in this calculation:
//    (1) m_curr_speed - (state variable)
//    (2) m_curr_depth -  (state variable)
//    (3) m_ascent_grade - (configuration variable)
//    (4) m_ascent_speed - (configuration variable)
//    (5) m_zero_speed_depth - (configuration variable)
//    (6) m_speed_at_ascent_begin - (state variable)
//    (7) m_depth_at_ascent_begin - (state variable)


double BHV_PeriodicSurface::setDesiredSpeed()
{
  double desired_speed;

  // Handle the possibility that the ascent begins at depth
  // more shallow than the m_zero_speed_depth.
  if(m_depth_at_ascent_begin <= m_zero_speed_depth)
    return(0);


  // Given that the m_depth_at_ascent_begin is deeper than the
  // m_zero_speed_depth, then THREE cases are possible:


  // Case1: current depth shallower than the m_zero_speed_depth
  if((m_curr_depth <= m_zero_speed_depth) ||
     (m_zero_speed_depth >= m_depth_at_ascent_begin))
    desired_speed = 0;
  // Case2: current depth deeper than m_depth_at_ascent_begin
  else if(m_curr_depth > m_depth_at_ascent_begin)
    desired_speed = m_speed_at_ascent_begin;
  // Case3: current depth somewhere between the zero_speed depth
  //        and the m_depth_at_ascent_begin
  else {
    double range = m_depth_at_ascent_begin - m_zero_speed_depth;
    double sofar = m_curr_depth - m_zero_speed_depth;
    double pct   = sofar / range;
    if(m_ascent_grade == "quadratic")
      desired_speed = m_speed_at_ascent_begin * pct * pct;
    else if(m_ascent_grade == "quasi")
      desired_speed = m_speed_at_ascent_begin * pow(pct, 1.5);
    else if(m_ascent_grade == "fullspeed")
      desired_speed = m_speed_at_ascent_begin;
    else // linear
      desired_speed = m_speed_at_ascent_begin * pct;
  }

  return(desired_speed);
}














