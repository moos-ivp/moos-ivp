/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_MemoryTurnLimit.cpp                              */
/*    DATE: Aug 7th 2006                                         */
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
#include "BHV_MemoryTurnLimit.h"
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

BHV_MemoryTurnLimit::BHV_MemoryTurnLimit(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_memturnlimit");

  domain = subDomain(domain, "course");

  m_memory_time = -1;
  m_turn_range  = -1;

  info_vars.push_back("NAV_HEADING");
  info_vars.push_back("NAV_SPEED"); //dpe
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition".
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_MemoryTurnLimit::setParam(string param, string val) 
{
  if(IvPBehavior::setParamCommon(param, val))
    return(true);

  if(param == "memory_time") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    if(!param_lock) {
      m_memory_time = dval;
    }
    return(true);
  }
  else if(param == "turn_range") {
    double dval = atof(val.c_str());
    if((dval < 0) || (dval > 180) || (!isNumber(val)))
      return(false);
    if(!param_lock) {
      m_turn_range = dval;
    }
    return(true);
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: produceOF

IvPFunction *BHV_MemoryTurnLimit::produceOF() 
{
  if(m_memory_time < 0) {
    postWMessage("Variable memory_time not specified");
    return(0);
  }
  if(m_turn_range < 0) {
    postWMessage("Variable turn_range not specified");
    return(0);
  }
    

  bool ok,ok2;
  double heading = info_buffer->dQuery("NAV_HEADING", ok);
  double speed   = info_buffer->dQuery("NAV_SPEED", ok2); //dpe

  if(!ok) {
    postEMessage("No Ownship NAV_HEADING in info_buffer");
    return(0);
  }

  if(!ok2) {                                                 //dpe
    postEMessage("No Ownship NAV_SPEED in info_buffer");     //dpe
    return(0);                                               //dpe 
  }
 
  double currtime = info_buffer->getCurrTime();
  
  addHeading(heading, currtime);
  double heading_avg;
  ok = getHeadingAvg2(heading_avg);

  if(!ok)
    return(0);
  
  postMessage("MEM_HDG_AVG", heading_avg);

  ZAIC_PEAK crs_zaic(domain, "course");
  crs_zaic.setSummit(heading_avg);
  crs_zaic.setValueWrap(true);
  crs_zaic.setPeakWidth(m_turn_range*speed/1.5);  //dpe-make 1.5 a param in future

  IvPFunction *ipf = crs_zaic.extractOF();

  if(ipf)
    ipf->setPWT(priority_wt);

  return(ipf);
}


//-----------------------------------------------------------
// Procedure: addHeading(double)

void BHV_MemoryTurnLimit::addHeading(double heading, 
				     double currtime)
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
// Procedure: getHeadingAvg

bool BHV_MemoryTurnLimit::getHeadingAvg(double& heading_avg)
{
  double anchor_heading; 
  double heading_delta_total = 0;

  list<double>::iterator p;
  int counter = 0;
  for(p = m_heading_val.begin(); p!=m_heading_val.end(); p++) {
    double iheading = *p;
    if(counter == 0)
      anchor_heading = iheading;
    else {
      double h_delta = angle180(iheading - anchor_heading);
      heading_delta_total += h_delta;
    }
    counter++;
  }

  if(counter == 0) {
    heading_avg = 0;
    return(false);
  }
  else if(counter==1) {
    heading_avg = anchor_heading;
    return(true);
  }

  double avg_delta = heading_delta_total / counter;
  heading_avg = angle360(anchor_heading + avg_delta);
  

  // Pass 2: Let the anchor_heading be the derived heading 
  // from the first pass
  anchor_heading = heading_avg;
  heading_delta_total = 0;
  counter = 0;
  for(p = m_heading_val.begin(); p!=m_heading_val.end(); p++) {
    double iheading = *p;
    if(counter > 0) {
      double h_delta = angle180(iheading - anchor_heading);
      heading_delta_total += h_delta;
    }
    counter++;
  }

  avg_delta = heading_delta_total / counter;
  heading_avg = angle360(anchor_heading + avg_delta);
  // End Pass 2

  return(true);
}


//-----------------------------------------------------------
// Procedure: getHeadingAvg2

bool BHV_MemoryTurnLimit::getHeadingAvg2(double& heading_avg)
{
  double s,c,ssum,csum,avg;

  ssum = 0.0;
  csum = 0.0;

  list<double>::iterator p;
  for(p = m_heading_val.begin(); p!=m_heading_val.end(); p++) {
    double iheading = *p;
  
    s = sin(iheading*M_PI/180.0);
    c = cos(iheading*M_PI/180.0);

    ssum += s;
    csum += c;    
  }

  avg = atan2(ssum,csum)*180.0/M_PI;

  if (avg < 0.0)
    avg += 360.0;

  heading_avg = avg;

  return(true);
}

