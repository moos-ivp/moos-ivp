/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_HeadingHystereis.cpp                             */
/*    DATE: July 30th 2009                                       */
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
#include "BHV_HeadingHysteresis.h"
#include "ZAIC_PEAK.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BuildUtils.h"

#ifndef M_PI
#define M_PI 3.1415926
#endif

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_HeadingHysteresis::BHV_HeadingHysteresis(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "heading_hysteresis");

  m_domain = subDomain(m_domain, "course");

  m_memory_time   = -1;
  m_filter_level  = 1;
  m_variable_name = "DESIRED_HEADING_UNFILTERED";

  addInfoVars(m_variable_name);
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_HeadingHysteresis::setParam(string param, string val) 
{
  if(param == "memory_time") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    m_memory_time = dval;
    return(true);
  }
  else if(param == "filter_var") {
    val = stripBlankEnds(val);
    if(strContainsWhite(val))
      return(false);
    m_variable_name = toupper(val);
    addInfoVars(m_variable_name);
    return(true);
  }

  return(false);
}


//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_HeadingHysteresis::onIdleState() 
{
  string check_result = updateHeadingHistory();
  if(check_result != "ok") 
    postWMessage(check_result);
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_HeadingHysteresis::onRunState() 
{
  string check_result = updateHeadingHistory();
  if(check_result != "ok") {
    postWMessage(check_result);
    return(0);
  }
  
  double hdg_average  = getHeadingAverage();
  double hdg_variance = getHeadingVariance(hdg_average);
  
  // round to integer to reduce distinct posts to the db
  postMessage("HIST_HEADING_AVERAGE", hdg_average);
  postMessage("HIST_HEADING_VARIANCE", hdg_variance);

  IvPFunction *ipf = 0;

#if 0
  double peak_width;
  double max_variance = 30;
  if(m_min_heading_window == m_max_heading_window)
    peak_width = m_min_heading_window;
  else {
    double diff = max_variance - heading_variance;
    if(diff < 0) 
      diff = 0;
    double pct = (diff / max_variance);
    peak_width = m_min_heading_window;
    peak_width += (pct * (m_max_heading_window - m_min_heading_window));
    peak_width *= 0.5;
  }

  double window_utility = m_max_window_utility;

  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setSummit(m_current_heading);
  crs_zaic.setValueWrap(true);
  crs_zaic.setPeakWidth(peak_width);  
  crs_zaic.setBaseWidth(180-peak_width);  
  crs_zaic.setSummitDelta(100-window_utility);

  ipf = crs_zaic.extractOF();
#endif

  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}


//-----------------------------------------------------------
// Procedure: updateHeadingHistory

string BHV_HeadingHysteresis::updateHeadingHistory()
{
  if(m_memory_time < 0)
    return("Variable memory_time not specified");

  bool ok;
  m_current_heading = getBufferDoubleVal(m_variable_name, ok);
  
  if(!ok) 
    return("HeadingHysteresis filter variable not found in info_buffer");
  
  double currtime = getBufferCurrTime();
  addHeadingEntry(m_current_heading, currtime);
  return("ok");
}

//-----------------------------------------------------------
// Procedure: addHeadingEntry(double, double)

void BHV_HeadingHysteresis::addHeadingEntry(double value, double currtime)
{
  m_heading_val.push_back(value);
  m_heading_time.push_back(currtime);
  
  int counter = 0;

  // Remove all stale elements from memory
  list<double>::iterator p;
  for(p = m_heading_time.begin(); p!=m_heading_time.end(); p++) {
    double itime = *p;
    if((currtime - itime) > m_memory_time) {
      counter++;
    }
  }

  for(int i=0; i<counter; i++) {
    m_heading_val.pop_front();
    m_heading_time.pop_front();
  }
}

//-----------------------------------------------------------
// Procedure: getHeadingAverage

double BHV_HeadingHysteresis::getHeadingAverage()
{
  double ssum = 0.0;
  double csum = 0.0;
  list<double>::iterator p;
  for(p = m_heading_val.begin(); p!=m_heading_val.end(); p++) {
    double iheading = *p;      
    double s = sin(iheading*M_PI/180.0);
    double c = cos(iheading*M_PI/180.0);
    ssum += s;
    csum += c;    
  }
  double avg = atan2(ssum,csum)*180.0/M_PI;
  avg = angle360(avg);
  return(avg);
}

//-----------------------------------------------------------
// Procedure: getHeadingVariance

double BHV_HeadingHysteresis::getHeadingVariance(double current_average)
{
  double total = 0;
  unsigned int entry_count = m_heading_val.size();
  
  list<double>::iterator p;
  for(p = m_heading_val.begin(); p!=m_heading_val.end(); p++) {
    double value = *p;
    double delta = angle180(value - current_average);
    total += (delta * delta);
  }

  return(sqrt(total)/(double)(entry_count));
}












