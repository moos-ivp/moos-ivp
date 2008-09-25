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
  this->setParam("descriptor", "periodic_speed");

  // These parameters really should be set in the behavior file, but are
  // left here for now to smoothen the transition (Aug 10, 2008, mikerb)
  this->setParam("activeflag",   "PERIODIC_SPEED=1");
  this->setParam("inactiveflag", "PERIODIC_SPEED=0");

  m_domain = subDomain(m_domain, "speed");

  // Initialize Configuration Parameters
  m_period_gap       = 0;
  m_period_length    = 0;
  m_period_speed     = 0;
  m_period_basewidth = 0;
  m_period_peakwidth = 0;

  // Initialize State Variables
  m_state_active    = false;
  m_first_iteration = true;
  m_mark_time       = 0;
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_PeriodicSpeed::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParam(g_param, g_val))
    return(true);

  if(g_param == "period_gap") {
    double dval = atof(g_val.c_str());
    if((dval <= 0) || (!isNumber(g_val)))
      return(false);
    m_period_gap = dval;
    return(true);
  }  
  else if(g_param == "period_length") {
    double dval = atof(g_val.c_str());
    if((dval <= 0) || (!isNumber(g_val)))
      return(false);
    m_period_length = dval;
    return(true);
  }  
  else if(g_param == "period_speed") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_period_speed = dval;
    return(true);
  }  
  else if(g_param == "period_basewidth") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_period_basewidth = dval;
    return(true);
  }  
  else if(g_param == "period_peakwidth") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_period_peakwidth = dval;
    return(true);
  }  
  else if(g_param == "stat_pending_active") {
    m_var_pending_active = g_val;
    return(true);
  }  
  else if(g_param == "stat_pending_inactive") {
    m_var_pending_inactive = g_val;
    return(true);
  }  
  else if(g_param == "period_flag") {
    g_val = findReplace(g_val, ',', '=');
    vector<string> svector = parseString(g_val, '=');
    if(svector.size() != 2)
      return(false);
    else {
      string var = stripBlankEnds(svector[0]);
      string val = stripBlankEnds(svector[1]);
      if((var=="") || (val==""))
	return(false);
      m_period_start_flag  = var;
      m_period_start_value = val;
    }
    return(true);
  }

  return(false);
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_PeriodicSpeed::onRunState() 
{
  double curr_time = getBufferCurrTime();

  if(m_first_iteration) {
    m_first_iteration = false;
    m_mark_time = curr_time;
    return(0);
  }

  double time_since_mark = curr_time - m_mark_time;

  double time_to_active   = 0;
  double time_to_inactive = 0;

  if(m_state_active) {
    if(time_since_mark > m_period_length) {
      m_state_active = false;
      m_mark_time = curr_time;
      time_since_mark = 0;
    }
    time_to_inactive = m_period_length - time_since_mark;
  }
  else {
    if(time_since_mark > m_period_gap) {
      m_state_active = true;
      m_mark_time = curr_time;
      time_since_mark = 0;
    }
    else
      time_to_active = m_period_gap - time_since_mark;
  }

  // For double-value postings of time, we post at integer precision 
  // when not close to zero to reduce the number of postings to the DB. 

  if(time_to_inactive <= 1)
    postMessage(m_var_pending_inactive,  time_to_inactive);
  else
    postIntMessage(m_var_pending_inactive,  time_to_inactive);

  if(time_to_active <= 1)
    postMessage(m_var_pending_active, time_to_active);
  else
    postIntMessage(m_var_pending_active, time_to_active);


  if(!m_state_active)
    return(0);

  ZAIC_PEAK zaic(m_domain, "speed");
  zaic.setSummit(m_period_speed);
  zaic.setBaseWidth(m_period_basewidth);
  zaic.setPeakWidth(m_period_peakwidth);

  IvPFunction *new_of = zaic.extractOF();
  new_of->getPDMap()->normalize(0,100);
  new_of->setPWT(m_priority_wt);

  return(new_of);
}








