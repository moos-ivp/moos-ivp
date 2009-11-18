/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_PeriodicSpeed.cpp                                */
/*    DATE: Jun 9th 2006                                         */
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
#include <stdlib.h>
#include "BHV_PeriodicSpeed.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_PeriodicSpeed::BHV_PeriodicSpeed(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  // First set variables at the superclass level
  m_descriptor = "periodic_speed";  
  m_domain     = subDomain(m_domain, "speed");

  // These parameters really should be set in the behavior file, but are
  // left here for now to smoothen the transition (Aug 10, 2008, mikerb)
  IvPBehavior::setParam("activeflag",   "PERIODIC_SPEED=1");
  IvPBehavior::setParam("inactiveflag", "PERIODIC_SPEED=0");

  // Initialize Configuration Parameters
  m_period_gap        = 0;
  m_period_length     = 0;
  m_period_speed      = 0;
  m_zaic_basewidth    = 0;
  m_zaic_peakwidth    = 0;
  m_zaic_summit_delta = 25;
  
  m_var_pending_active   = "PS_PENDING_ACTIVE";
  m_var_pending_inactive = "PS_PENDING_INACTIVE";
  m_var_period_start     = "PS_PERIOD_INDEX";
  
  // Initialize State Variables
  m_state_active  = false;
  m_mark_needed   = true;
  m_mark_time     = 0;
  m_period_index  = 0;
  m_reset_on_idle = true;
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_PeriodicSpeed::setParam(string param, string value) 
{
  double dval = atof(value.c_str());

  if(param == "period_gap") {
    if(!isNumber(value) || (dval <= 0))
      return(false);
    m_period_gap = dval;
    return(true);
  }  
  else if(param == "period_length") {
    if(!isNumber(value) || (dval <= 0))
      return(false);
    m_period_length = dval;
    return(true);
  }  
  else if(param == "period_speed") {
    if(!isNumber(value) || (dval < 0))
      return(false);
    m_period_speed = dval;
    return(true);
  }  
  else if((param == "period_basewidth") || (param == "zaic_basewidth")) {
    if(!isNumber(value) || (dval < 0))
      return(false);
    m_zaic_basewidth = dval;
    return(true);
  }  
  else if((param == "period_peakwidth") || (param == "zaic_peakwidth")) {
    if(!isNumber(value) || (dval < 0))
      return(false);
    m_zaic_peakwidth = dval;
    return(true);
  } 
  else if(param == "zaic_summit_delta") {
    if(!isNumber(value) || (dval < 0))
      return(false);
    m_zaic_summit_delta = dval;
    return(true);
  } 
  else if(param == "reset_on_idle")
    return(setBooleanOnString(m_reset_on_idle, value));
  else if(param == "initially_active") {
    if(tolower(value) == "true")
      m_state_active  = true;
    else if(tolower(value) == "false")
      m_state_active  = false;
    else
      return(false);
    m_mark_needed   = true;
    return(true);
  }

  return(false);
}

//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_PeriodicSpeed::onIdleState() 
{
  if(m_reset_on_idle) {
    m_time_to_active   = m_period_gap;
    m_time_to_inactive = 0;
  }
  else
    updateInfoIn();

  postStatusReport();
}

//-----------------------------------------------------------
// Procedure: onIdleToRunState

void BHV_PeriodicSpeed::onIdleToRunState() 
{
  if(m_reset_on_idle) {
    m_mark_time = getBufferCurrTime();
    m_state_active = false;
  }
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_PeriodicSpeed::onRunState() 
{
  // Update m_state_active, m_time_to_active, m_time_to_inactive, 
  // m_mark_time, m_period_index
  updateInfoIn();

  postStatusReport();

  if(!m_state_active)
    return(0);

  ZAIC_PEAK zaic(m_domain, "speed");
  zaic.setSummit(m_period_speed);
  zaic.setBaseWidth(m_zaic_basewidth);
  zaic.setPeakWidth(m_zaic_peakwidth);
  zaic.setSummitDelta(m_zaic_summit_delta);

  IvPFunction *new_of = zaic.extractIvPFunction();
  new_of->getPDMap()->normalize(0,100);
  new_of->setPWT(m_priority_wt);

  return(new_of);
}

//-----------------------------------------------------------
// Procedure: updateInfoIn()

void BHV_PeriodicSpeed::updateInfoIn() 
{
  double curr_time = getBufferCurrTime();
  
  if(m_mark_needed) {
    m_mark_needed = false;
    m_mark_time = curr_time;
  }

  double time_since_mark = curr_time - m_mark_time;

  if(m_state_active) {
    m_time_to_inactive = m_period_length - time_since_mark;
    if(m_time_to_inactive <= 0) {
      m_state_active  = false;
      m_mark_time     = curr_time;
      time_since_mark = 0;
      m_time_to_inactive = 0;
    }
  }
  else {
    m_time_to_active = m_period_gap - time_since_mark;
    if(m_time_to_active <= 0) {
      m_state_active   = true;
      m_mark_time      = curr_time;
      time_since_mark  = 0;
      m_time_to_active = 0;
      postMessage(m_var_period_start, m_period_index);
      m_period_index++;
    }
  }

  if(m_state_active == true)
    postMessage("PDSPD_ACTIVE", 1);
  else
    postMessage("PDSPD_ACTIVE", 0);
}



//-----------------------------------------------------------
// Procedure: postStatusReport()

void BHV_PeriodicSpeed::postStatusReport() 
{
  // For double-value postings of time, we post at integer precision 
  // when not close to zero to reduce the number of postings to the DB. 
  if(m_time_to_inactive <= 1)
    postMessage(m_var_pending_inactive,  m_time_to_inactive);
  else
    postIntMessage(m_var_pending_inactive,  m_time_to_inactive);

  if(m_time_to_active <= 1)
    postMessage(m_var_pending_active, m_time_to_active);
  else
    postIntMessage(m_var_pending_active, m_time_to_active);
}


