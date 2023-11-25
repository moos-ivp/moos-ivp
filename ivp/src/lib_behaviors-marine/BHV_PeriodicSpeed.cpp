/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_PeriodicSpeed.cpp                                */
/*    DATE: Jun 9th 2006                                         */
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
#include <cstdlib>
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
  m_period_busy       = 0;
  m_period_lazy       = 0;
  m_period_speed      = 0;
  m_zaic_basewidth    = 0;
  m_zaic_peakwidth    = 0;
  m_zaic_summit_delta = 25;
  
  m_var_pending_busy  = "PS_PENDING_ACTIVE";
  m_var_pending_lazy  = "PS_PENDING_INACTIVE";
  m_var_busy_count    = "PS_BUSY_COUNT";
  
  // Initialize State Variables
  m_mode_busy     = false;
  m_mark_needed   = true;
  m_mark_time     = 0;
  m_busy_count    = 0;

  m_reset_upon_running = true;
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_PeriodicSpeed::setParam(string param, string value) 
{
  double dval = atof(value.c_str());

  if(param == "period_busy") {
    if(!isNumber(value) || (dval <= 0))
      return(false);
    m_period_busy = dval;
    return(true);
  }  
  else if(param == "period_lazy") {
    if(!isNumber(value) || (dval <= 0))
      return(false);
    m_period_lazy = dval;
    return(true);
  }  
  else if(param == "period_speed") {
    if(!isNumber(value) || (dval < 0))
      return(false);
    m_period_speed = dval;
    return(true);
  }  
  else if((param == "basewidth")         ||     // preferred 
	  (param == "zaic_basewidth")    ||     // deprecated
	  (param == "period_basewidth")) {      // deprecated
    if(!isNumber(value) || (dval < 0))
      return(false);
    m_zaic_basewidth = dval;
    return(true);
  }  
  else if((param == "peakwidth")         ||     // preferred
	  (param == "period_peakwidth")  ||     // deprecated
	  (param == "zaic_peakwidth")) {        // deprecated
    if(!isNumber(value) || (dval < 0))
      return(false);
    m_zaic_peakwidth = dval;
    return(true);
  } 
  else if((param == "summit_delta")      ||     // preferred
	  (param == "zaic_summit_delta")) {     // deprecated
    if(!isNumber(value) || (dval < 0))
      return(false);
    m_zaic_summit_delta = dval;
    return(true);
  } 
  else if(param == "reset_upon_running")
    return(setBooleanOnString(m_reset_upon_running, value));
  else if(param == "initially_busy") {
    if(tolower(value) == "true")
      m_mode_busy  = true;
    else if(tolower(value) == "false")
      m_mode_busy  = false;
    else
      return(false);
    m_mark_needed   = true;
    return(true);
  }

  return(false);
}

//-----------------------------------------------------------
// Procedure: onSetParamComplete

void BHV_PeriodicSpeed::onSetParamComplete()
{
  postMessage(m_var_busy_count, 0);
}

//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_PeriodicSpeed::onIdleState() 
{
  if(m_reset_upon_running) {
    m_time_to_busy = m_period_lazy;
    m_time_to_lazy = 0;
  }
  else
    updateInfoIn();

  postStatusReport();
}

//-----------------------------------------------------------
// Procedure: onIdleToRunState

void BHV_PeriodicSpeed::onIdleToRunState() 
{
  if(m_reset_upon_running) {
    m_mark_time = getBufferCurrTime();
    m_mode_busy = false;
  }
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_PeriodicSpeed::onRunState() 
{
  // Update m_mode_busy, m_time_to_busy, m_time_to_lazy, 
  // m_mark_time, m_busy_count
  updateInfoIn();
  
  postStatusReport();
  
  if(!m_mode_busy)
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

  if(m_mode_busy) {
    m_time_to_lazy = m_period_busy - time_since_mark;
    if(m_time_to_lazy <= 0) {
      m_mode_busy     = false;
      m_mark_time     = curr_time;
      time_since_mark = 0;
      m_time_to_lazy  = 0;
    }
  }
  else {
    m_time_to_busy = m_period_lazy - time_since_mark;
    if(m_time_to_busy <= 0) {
      m_mode_busy      = true;
      m_mark_time      = curr_time;
      time_since_mark  = 0;
      m_time_to_busy   = 0;
      m_busy_count++;
      postMessage(m_var_busy_count, m_busy_count);
    }
  }
}

//-----------------------------------------------------------
// Procedure: postStatusReport()

void BHV_PeriodicSpeed::postStatusReport() 
{
  // For double-value postings of time, we post at integer precision 
  // when not close to zero to reduce the number of postings to the DB. 
  if(m_time_to_lazy <= 1)
    postMessage(m_var_pending_lazy,  m_time_to_lazy);
  else
    postIntMessage(m_var_pending_lazy,  m_time_to_lazy);

  if(m_time_to_busy <= 1)
    postMessage(m_var_pending_busy, m_time_to_busy);
  else
    postIntMessage(m_var_pending_busy, m_time_to_busy);
}












