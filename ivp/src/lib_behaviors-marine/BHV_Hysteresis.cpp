/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Hystereis.cpp                                    */
/*    DATE: August 4th 2008 (aboard the Alliance)                */
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
#include <iostream>
#include <math.h> 
#include <stdlib.h>
#include "BHV_Hysteresis.h"
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

BHV_Hysteresis::BHV_Hysteresis(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_memturnlimit");

  m_domain = subDomain(m_domain, "course");

  m_memory_time        = -1;
  m_min_heading_window = 10.0;
  m_max_heading_window = 280;
  m_max_window_utility = 90.0;

  addInfoVars("NAV_HEADING, NAV_SPEED");
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition".
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_Hysteresis::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  if(param == "memory_time") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    m_memory_time = dval;
    return(true);
  }
  else if(param == "min_heading_window") {
    double dval = atof(val.c_str());
    if((dval < 0) || (dval > 360) || (!isNumber(val)))
      return(false);
    m_min_heading_window = dval;
    if(m_min_heading_window > m_max_heading_window)
      m_max_heading_window = m_min_heading_window;
    return(true);
  }
  else if(param == "max_heading_window") {
    double dval = atof(val.c_str());
    if((dval < 0) || (dval > 360) || (!isNumber(val)))
      return(false);
    m_max_heading_window = dval;
    if(m_max_heading_window < m_min_heading_window)
      m_min_heading_window = m_max_heading_window;
    return(true);
  }
  else if(param == "max_window_utility") {
    double dval = atof(val.c_str());
    if((dval < 0) || (dval > 100) || (!isNumber(val)))
      return(false);
    m_max_window_utility = dval;
    return(true);
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_Hysteresis::onIdleState() 
{
  string check_result = updateHeadingAvg();
  if(check_result != "ok") 
    postWMessage(check_result);
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_Hysteresis::onRunState() 
{
  string check_result = updateHeadingAvg();
  if(check_result != "ok") {
    postWMessage(check_result);
    return(0);
  }

  double heading_average  = getHeadingAverage();
  double heading_variance = getHeadingVariance(heading_average);

  // round to integer to reduce distinct posts to the db
  postMessage("HYSTER_HDG_AVG", (int)(heading_average+0.5));
  postMessage("HYSTER_HDG_VAR", (int)(heading_variance+0.5));

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

  IvPFunction *ipf = crs_zaic.extractOF();

  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}


//-----------------------------------------------------------
// Procedure: updateHeadingAvg

string BHV_Hysteresis::updateHeadingAvg()
{
  if(m_memory_time < 0)
    return("Variable memory_time not specified");
  if(m_turn_range < 0)
    return("Variable turn_range not specified");

  bool ok, ok2;
  m_current_heading = getBufferDoubleVal("NAV_HEADING", ok);
  m_current_speed   = getBufferDoubleVal("NAV_SPEED", ok2); 
  
  m_current_heading = angle360(m_current_heading);

  if(!ok) 
    return("No Ownship NAV_HEADING in info_buffer");
  
  if(!ok2) 
    return("No Ownship NAV_SPEED in info_buffer"); 
  
  double currtime = getBufferCurrTime();
  addHeading(m_current_heading, currtime);
  return("ok");
}

//-----------------------------------------------------------
// Procedure: addHeading(double, double)

void BHV_Hysteresis::addHeading(double heading, double currtime)
{
  m_heading_val.push_back(heading);
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

double BHV_Hysteresis::getHeadingAverage()
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

  return(angle360(avg));
}

//-----------------------------------------------------------
// Procedure: getHeadingVariance

double BHV_Hysteresis::getHeadingVariance(double current_average)
{
  double total = 0;
  list<double>::iterator p;
  int counter = 0;
  for(p = m_heading_val.begin(); p!=m_heading_val.end(); p++) {
    double iheading = *p;
    double delta    = angle180(iheading - current_average);
   
    total += (delta * delta);
    counter++;
  }
  return(sqrt(total)/(double)(counter));
}


