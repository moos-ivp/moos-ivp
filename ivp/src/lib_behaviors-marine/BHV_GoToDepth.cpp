/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_GoToDepth.cpp                                    */
/*    DATE: Aug 1 2006                                           */
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
#include "BHV_GoToDepth.h"
#include "ZAIC_PEAK.h"
#include "BuildUtils.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_GoToDepth::BHV_GoToDepth(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  m_domain = subDomain(m_domain, "depth");

  // configuration parameters
  m_repeat        = 0;
  m_basewidth     = 5;
  m_peakwidth     = 0;
  m_arrival_delta = 1;  // meters

  // local state variables
  m_curr_index          = 0;
  m_prior_vehicle_depth = 0;
  m_plateau_mode        = false;
  m_plateau_start_time  = 0;
  m_arrivals            = 0;
  m_first_iteration     = true;
  
  addInfoVars("NAV_DEPTH");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_GoToDepth::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  //  50,10;10,10;50,10;50
  if(param == "depth") {
    vector<string> out_vector = parseString(val, ':');
    int outsize = out_vector.size();
    for(int i=0; i<outsize; i++) {
      vector<string> in_vector = parseString(out_vector[i], ',');
      int in_size = in_vector.size();
      double idepth = 0;
      double itime  = 0;
      if((in_size<1) || (in_size > 2))
	return(false);

      // Determine the chosen depth
      if(isNumber(in_vector[0]) == false)
	return(false);
      idepth = atof(in_vector[0].c_str());
      
      // Determine the chosen time - if not spec, ok, then zero
      if(in_size == 2) {
	if(isNumber(in_vector[1]) == false)
	  return(false);
	itime = atof(in_vector[1].c_str());	
      }
      else
	itime = 0;
      
      // Check for semantically unallowed values
      if((idepth < 0) || (itime < 0))
	return(false);
      
      m_level_depths.push_back(idepth);
      m_level_times.push_back(itime);
    }
    return(true);
  }
  else if(param == "repeat") {
    int ival = atoi(val.c_str());
    if((ival < 0) || (!isNumber(val)))
      return(false);
    m_repeat = ival;
    return(true);
  }
  else if((param == "arrival_delta") || 
	  (param == "capture_delta")) {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    m_arrival_delta = dval;
    return(true);
  }
  else if((param == "arrival_flag") ||
	  (param == "capture_flag")) {
    m_arrival_flag = "GTD_" + val;
    return(true);
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_GoToDepth::onRunState() 
{
  if(!m_domain.hasDomain("depth")) {
    postEMessage("No 'depth' variable in the helm domain");
    return(0);
  }

  bool valid_depth_level = setNextLevelDepth();
 
  // Once the target depth level has been calculated for the first
  // time, declare first_iteration to be false.
  m_first_iteration = false;


  // If no next-depth-level return NULL - it may or may not be an
  // error condition. That is determined in setNextDepthLevel();
  if(!valid_depth_level)
    return(0);
  
  ZAIC_PEAK zaic(m_domain, "depth");
  zaic.setSummit(m_level_depths[m_curr_index]);
  zaic.setBaseWidth(m_basewidth);
  zaic.setPeakWidth(m_peakwidth);

  IvPFunction *ipf = zaic.extractOF();
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: setNextDepthLevel
//      Note: Objective is to incmrement m_curr_index if warranted.
//   Returns: false if at the end or an error.
//            If an error, will also post an EMessage.

bool BHV_GoToDepth::setNextLevelDepth()
{
  bool ok;
  double vehicle_depth = getBufferDoubleVal("NAV_DEPTH", ok);
  
  // Must get ownship depth from InfoBuffer
  if(!ok) {
    postEMessage("No ownship Depth info in Helm info_buffer.");
    return(false);
  }

  double curr_time = getBufferCurrTime();

  bool new_level = false;

  if(m_plateau_mode) {
    double elapsed_time = curr_time - m_plateau_start_time;
    if(elapsed_time >= m_level_times[m_curr_index]) {
      m_plateau_mode = false;
      bool incremented = incrementLevelDepth();
      if(!incremented)
	return(false);
    }
    else
      return(true);
  }

  double delta = vehicle_depth - m_level_depths[m_curr_index];
  if(delta < 0)
    delta *= -1;
  
  // If within arrival_delta meters of target depth, declare arrival
  if(delta <= m_arrival_delta)
    new_level = true;

  // If we crossed the target depth going down, we declare arrival
  if((vehicle_depth >= m_level_depths[m_curr_index]) &&
     (m_level_depths[m_curr_index] >= m_prior_vehicle_depth) &&
     (!m_first_iteration))
    new_level = true;

  // If we crossed the target depth going up, we declare arrival
  if((vehicle_depth <= m_level_depths[m_curr_index]) &&
     (m_level_depths[m_curr_index] <= m_prior_vehicle_depth) &&
     (!m_first_iteration))
    new_level = true;
  
  if(new_level) {
    m_arrivals++;
    if(m_arrival_flag != "")
      postMessage(m_arrival_flag, m_arrivals);
    m_plateau_mode = true;
    m_plateau_start_time = curr_time;
  }
  
  // Set the prior depth to curr depth for access on next iteration
  m_prior_vehicle_depth = vehicle_depth;

  return(true);
}

//-----------------------------------------------------------
// Procedure: incrementLevelDepth

bool BHV_GoToDepth::incrementLevelDepth()
{
  m_curr_index++;
  if((m_curr_index >= m_level_depths.size()) && (m_repeat==0)) {
    setComplete();
    if(m_perpetual) {
      m_curr_index = 0;
      m_first_iteration = true;
      m_arrivals = 0;
    }
    return(false);
  }
  
  // If last depth level but repeats are indicated
  if(m_curr_index >= m_level_depths.size()) {
    m_repeat--;
    m_curr_index = 0;
  }

  postMessage("LEVEL_DEPTH", m_level_depths[m_curr_index]);
  return(true);
}
  


