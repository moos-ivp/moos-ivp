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
  this->setParam("descriptor", "(d)bhv_periodic_speed");

  m_domain = subDomain(m_domain, "speed");

  // Initialize Configuration Parameters
  m_period_gap       = 0;
  m_period_length    = 0;
  m_period_speed     = 0;
  m_period_basewidth = 0;
  m_period_peakwidth = 0;

  // Initialize State Variables
  m_state_inperiod  = false;
  m_first_iteration = true;
  m_mark_time     = 0;
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
  double curr_time = m_info_buffer->getCurrTime();

  if(m_first_iteration) {
    m_first_iteration = false;
    m_mark_time = curr_time;
    return(0);
  }

  double time_since_mark = curr_time - m_mark_time;

  double time_to_slowdown = 0;
  double time_to_speedup  = 0;

  if(m_state_inperiod) {
    if(time_since_mark > m_period_length) {
      m_state_inperiod = false;
      m_mark_time = curr_time;
      time_since_mark = 0;
    }
    time_to_speedup = m_period_length - time_since_mark;
  }
  else {
    if(time_since_mark > m_period_gap) {
      m_state_inperiod = true;
      m_mark_time = curr_time;
      time_since_mark = 0;
    }
    else
      time_to_slowdown = m_period_gap - time_since_mark;
  }

  postMessage("TIME_TO_SPEEDUP",  time_to_speedup);
  postMessage("TIME_TO_SLOWDOWN", time_to_slowdown);

  if(m_state_inperiod)
    postMessage("PERIODIC_SPEED", 1);
  else
    postMessage("PERIODIC_SPEED", 0);


  if(!m_state_inperiod)
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








