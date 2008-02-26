/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    MOD.: Henrik Schmidt                                       */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_ClassifyYoyo.cpp                                 */
/*    DATE: Mar. 3, 2007                                         */
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
#include <math.h> 
#include "BHV_ClassifyYoyo.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_ClassifyYoyo::BHV_ClassifyYoyo(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_classify_yoyo");

  // Behavior Parameter Default Values:
  period      = 300; // meters
  period_segments      = 200;
  mark_increment = period/period_segments;
  max_depth            = 0;   // meters
  min_depth            = 0;   // meters
  min_altitude         = 0;   // meters
  max_depth_local      = 0;   // meters
  distance_traveled    = 0;   // meters
  depth_function = "cosine";

  // Behavior State Variable Initial Values:
  first_iteration      = true;
  mark_distance        = 0;
  start_time = 0;
  iter_time = 0;

  m_domain = subDomain(m_domain, "depth");

  // Declare the variables we will need from the info_buffer
  addInfoVars("NAV_DEPTH");
  addInfoVars("NAV_SPEED");
  addInfoVars("NAV_ALTITUDE");
  addInfoVars("DESIRED_SPEED");
  addInfoVars("DESIRED_DEPTH");

}

//-----------------------------------------------------------
// Procedure: onIdleState
//      Note: This function overrides the onIdleState() virtual
//            function defined for the IvPBehavior superclass
//            This function will be executed by the helm each
//            time the behavior FAILS to meet its run conditions.

void BHV_ClassifyYoyo::onIdleState()
{
  // Do your thing here
  first_iteration      = true;
  mark_distance        = 0;
  start_time = 0;
  iter_time = 0;
  distance_traveled    = 0;   
   }


//-----------------------------------------------------------
// Procedure: setParam

bool BHV_ClassifyYoyo::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParamCommon(g_param, g_val))
    return(true);

  g_val = stripBlankEnds(g_val);

  if(g_param == "period") 
    {
      double dval = atof(g_val.c_str());
      if((dval <= 0) || (!isNumber(g_val)))
	return(false);
      period = dval;
      mark_increment = period/period_segments;
      return(true);
    } 
  if(g_param == "period_segments") 
    {
      double dval = atof(g_val.c_str());
      if((dval <= 0) || (!isNumber(g_val)))
	return(false);
      period_segments = dval;
      mark_increment = period/period_segments;
      return(true);
    } 
   else if(g_param == "min_depth") 
     {
       double dval = atof(g_val.c_str());
       if((dval < 0) || (!isNumber(g_val)))
	 return(false);
       min_depth = dval;
       return(true);
     }
  else if(g_param == "max_depth") 
    {
      double dval = atof(g_val.c_str());
      if((dval < 0) || (!isNumber(g_val)))
	return(false);
      max_depth = dval;
      return(true);
    }
  else if(g_param == "min_altitude") 
    {
      double dval = atof(g_val.c_str());
      if((dval < 0) || (!isNumber(g_val)))
	return(false);
      min_altitude = dval;
      return(true);
    }
  else if(g_param == "depth_function") 
    {
      if(g_val == "")
	return(false);
      depth_function = g_val;
      return(true);
    } 

  return(false);
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_ClassifyYoyo::onRunState() 
{
  double curr_ang;
  double curr_time = getBufferCurrTime();
 
  bool   ok,ok1,ok2;
  double curr_depth = getBufferDoubleVal("NAV_DEPTH", ok);
  double curr_altitude = getBufferDoubleVal("NAV_ALTITUDE", ok1);
  double curr_speed = getBufferDoubleVal("NAV_SPEED", ok2);
  // if min_altitude > 0, check here for bottom altitude and change max depth if necessary
  double curr_water_depth = curr_depth + curr_altitude;
  if (curr_altitude > 0 && curr_water_depth-min_altitude < max_depth && min_altitude > 0)
    max_depth_local = curr_water_depth-min_altitude;
  else
    max_depth_local = max_depth;

  // where are we inperiod
  double mean_depth = 0.5*(max_depth_local+min_depth);
  if(first_iteration) 
    {
      first_iteration = false;
      if (depth_function == "linear")
	{
	  if (curr_depth <= min_depth)
	    distance_traveled = 0.5*period;
	  else if (curr_depth >= max_depth_local)
	    distance_traveled = 0;
	  else
	    distance_traveled = 0.5*period*(max_depth_local - curr_depth)/(max_depth_local-min_depth);
	}
      else
	{
	  if (curr_depth < min_depth)
	    curr_ang=M_PI;
	  else if (curr_depth > max_depth_local)
	    curr_ang = 0;
	  else 
	    curr_ang = acos((curr_depth - mean_depth)/(max_depth_local-mean_depth));
	  distance_traveled = period*curr_ang/(2*M_PI);
	}
      new_depth = curr_depth; 
    }  
  else
    {
      double time_inc = curr_time - iter_time;
      distance_traveled = distance_traveled + time_inc*curr_speed;
    }

  iter_time = curr_time; 

  if ((distance_traveled-mark_distance) >= mark_increment)
    {
      if (depth_function == "linear")
	{
	  int half_period_num = (int) (distance_traveled*2.0/period);
	  double into_period = distance_traveled - half_period_num * 0.5 * period;
	  double depth_ratio = 2*into_period/period; 
	  if ( half_period_num%2 == 1 )
	    new_depth = min_depth + depth_ratio * (max_depth_local - min_depth);
	  else
	    new_depth = max_depth_local - depth_ratio * (max_depth_local - min_depth);
	}
      else
      new_depth = mean_depth + (max_depth_local-mean_depth)*cos((distance_traveled/period)*2*M_PI);
      mark_distance = distance_traveled;  
    }
  
  ZAIC_PEAK zaic_depth(m_domain, "depth");
  zaic_depth.setSummit(new_depth);
  zaic_depth.setBaseWidth(5.0);
  zaic_depth.setPeakWidth(1.0);
  zaic_depth.setSummitDelta(60.0);
  zaic_depth.setMinMaxUtil(0, 100);

  IvPFunction *ipf_depth = zaic_depth.extractOF();

  ipf_depth->setPWT(m_priority_wt);

  return(ipf_depth);
}

