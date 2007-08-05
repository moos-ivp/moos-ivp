/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_PeriodicSurface.cpp                              */
/*    DATE: Feb 21st 2007                                        */
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
  this->setParam("descriptor", "(d)bhv_periodic_surface");

  // Behavior Parameter Default Values:
  period              = 300; // Seconds
  mark_variable       = "GPS_UPDATE_RECEIVED";
  status_variable     = "PENDING_SURFACE";
  max_time_at_surface = 0;
  speed_to_surface    = 0;
  zero_speed_depth    = 0;

  // Behavior State Variable Initial Values:
  first_iteration      = true;
  first_marking_string = true;
  first_marking_double = true;
  mark_time            = 0;
  curr_mark_string_val = "";
  curr_mark_double_val = 0;

  domain = subDomain(domain, "speed,depth");

  // Declare the variables we will need from the info_buffer
  info_vars.push_back(mark_variable);
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_PeriodicSurface::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParamCommon(g_param, g_val))
    return(true);

  g_val = stripBlankEnds(g_val);

  if(g_param == "period") {
    double dval = atof(g_val.c_str());
    if((dval <= 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock)
      period = dval;
    return(true);
  } 
  else if(g_param == "mark_variable") {
    if(g_val == "")
      return(false);
    if(!param_lock) {
      mark_variable = g_val;
      info_vars.push_back(mark_variable);
    }
    return(true);
  }
  else if(g_param == "status_variable") {
    if(g_val == "")
      return(false);
    if(!param_lock)
      status_variable = g_val;
    return(true);
  }
  else if(g_param == "speed_to_surface") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock)
      speed_to_surface = dval;
    return(true);
  }
  else if(g_param == "zero_speed_depth") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock)
      zero_speed_depth = dval;
    return(true);
  }
  else if(g_param == "max_time_at_surface") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    if(!param_lock)
      max_time_at_surface = dval;
    return(true);
  }
  
  return(false);
}

//-----------------------------------------------------------
// Procedure: produceOF

IvPFunction *BHV_PeriodicSurface::produceOF() 
{
  checkForMarking();
  double curr_time = info_buffer->getCurrTime();

  double time_since_mark = curr_time - mark_time;
  double time_to_surface = period - time_since_mark;
  if(time_to_surface < 0)
    time_to_surface = 0;

  postMessage(status_variable, time_to_surface);


  bool   ok;
  double curr_depth = info_buffer->dQuery("NAV_DEPTH", ok);

  double desired_speed = speed_to_surface;
  if(curr_depth <= zero_speed_depth)
    desired_speed = 0;
  
  if(time_to_surface > 0)
    return(0);
  
  ZAIC_PEAK zaic_depth(domain, "depth");
  zaic_depth.setSummit(0);
  zaic_depth.setBaseWidth(0);
  zaic_depth.setPeakWidth(0);
  zaic_depth.setMinMaxUtil(0, 100);

  IvPFunction *ipf_depth = zaic_depth.extractOF();

  ZAIC_PEAK zaic_speed(domain, "speed");
  zaic_speed.setSummit(desired_speed);
  if(desired_speed == 0)
    zaic_speed.setBaseWidth(0);
  else
    zaic_speed.setBaseWidth(0.5);
  zaic_speed.setPeakWidth(0);
  zaic_speed.setMinMaxUtil(0, 100);

  IvPFunction *ipf_speed = zaic_speed.extractOF();

  OF_Coupler coupler;
  IvPFunction *new_ipf = coupler.couple(ipf_speed, ipf_depth);

  new_ipf->setPWT(priority_wt);

  return(new_ipf);
}

//-----------------------------------------------------------
// Procedure: checkForMarking

void BHV_PeriodicSurface::checkForMarking()
{
  bool   mark_noted = false;
  double curr_time = info_buffer->getCurrTime();

  bool   ok1, ok2;
  double double_mark_val = info_buffer->dQuery(mark_variable, ok1);
  string string_mark_val = info_buffer->sQuery(mark_variable, ok2);

  if(first_iteration) {
    first_iteration = false;
    mark_noted = true;
  }  

  if(ok1) {
    if(first_marking_double || (double_mark_val != curr_mark_double_val)) {
      mark_noted = true;
      curr_mark_double_val = double_mark_val;
      first_marking_double = false;
    }
  }
  
  if(ok2) {
    if(first_marking_string || (string_mark_val != curr_mark_string_val)) {
      mark_noted = true;
      curr_mark_string_val = string_mark_val;
      first_marking_string = false;
    }
  }

  if(mark_noted)
    mark_time = curr_time;
}

