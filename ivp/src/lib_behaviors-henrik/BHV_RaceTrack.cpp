/*****************************************************************/
/*    NAME: Henrik Schmidt                                       */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: BHV_RaceTrack.cpp                                 */
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
#include "BHV_RaceTrack.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "AngleUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_RaceTrack::BHV_RaceTrack(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_racetrack");

  // Behavior Parameter Default Values:
  track_length               = 500; // meters
  track_width            = 100;   // meters
  heading              = 0;     // degrees
  heading_out          = 0;     // degrees
  heading_in           = 0;     // degrees
  distance_traveled    = 0;   // meters
  approach_angle = 80;
  dist_tolerance = 10;
  // Behavior State Variable Initial Values:
  first_iteration      = true;
  deploy_x = 0;
  deploy_y = 0;
  m_clockwise = false;
  m_deploy_pending = false;
  m_deploy_location = "";

  m_domain = subDomain(m_domain, "speed,course");

  // Declare the variables we will need from the info_buffer
  addInfoVars("NAV_X");
  addInfoVars("NAV_Y");
}

//-----------------------------------------------------------
// Procedure: onIdleState
//      Note: This function overrides the onIdleState() virtual
//            function defined for the IvPBehavior superclass
//            This function will be executed by the helm each
//            time the behavior FAILS to meet its run conditions.

void BHV_RaceTrack::onIdleState()
{
  bool ok,ok1;
  // Do your thing here
  first_iteration      = true;
  distance_traveled    = 0;
  deploy_x = getBufferDoubleVal("NAV_X", ok);
  deploy_y = getBufferDoubleVal("NAV_Y", ok1);
  if (!m_deploy_pending)
    {
      m_deploy_pending = true;
      m_deploy_location  = "present_position";
    }
}


//-----------------------------------------------------------
// Procedure: setParam

bool BHV_RaceTrack::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParamCommon(g_param, g_val))
    return(true);

  g_val = stripBlankEnds(g_val);

  if(g_param == "track_length") 
    {
      double dval = atof(g_val.c_str());
      if((dval <= 0) || (!isNumber(g_val)))
	return(false);
      track_length = dval;
	
      return(true);
    } 
  else if(g_param == "track_width") 
    {
      double dval = atof(g_val.c_str());
      if((dval < 0) || (!isNumber(g_val)))
	return(false);
      track_width = dval;
      return(true);
    }
  if(g_param == "distance_tolerance") 
    {
      double dval = atof(g_val.c_str());
      if((dval <= 0) || (!isNumber(g_val)))
	return(false);
      dist_tolerance = dval;
	
      return(true);
    } 
  if(g_param == "approach_angle") 
    {
      double dval = atof(g_val.c_str());
      if((dval <= 0) || (!isNumber(g_val)))
	return(false);
      approach_angle = dval;
	
      return(true);
    } 
  if(g_param == "speed") 
    {
      double dval = atof(g_val.c_str());
      if((dval <= 0) || (!isNumber(g_val)))
	return(false);
      speed = dval;
	
      return(true);
    } 
   else if(g_param == "heading") 
     {
       double dval = atof(g_val.c_str());
       if((dval < 0) || (!isNumber(g_val)))
	 return(false);
       heading = angle360(dval);
       return(true);
     }
  else if(g_param == "points") {
    m_deploy_location  = g_val;
    m_deploy_pending = true;
    return(true);
  }  
  else if(g_param == "clockwise") {
    g_val = tolower(g_val);
    if((g_val!="true")&&(g_val!="false"))
      return(false);
    m_clockwise = (g_val == "true");
    return(true);
  }  

  return(false);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_RaceTrack::onRunState() 
{
  bool   ok,ok1,ok2;
  double curr_x = getBufferDoubleVal("NAV_X", ok);
  double curr_y = getBufferDoubleVal("NAV_Y", ok1);
  double dir;
  heading_out = heading;
  heading_in = angle360(heading_out + 180.0);

  //  deploy_x_out = deploy_x;
  //  deploy_y_out = deploy_y;
  updateStart();

  head_angle = headingToRadians(heading_out); 

  head_x = cos(head_angle);
  head_y = sin(head_angle);
  
  // postMessage("RTRK_LEG_HEADING",heading_out);
  // postMessage("RTRK_HEAD_ANGLE",head_angle);
  // postMessage("RTRK_HEAD_X",head_x);
  // postMessage("RTRK_HEAD_Y",head_y);
  
  if (m_clockwise)
    dir = -1;
  else
    dir =1;

  end_x_out = deploy_x_out + dir * track_length * head_x; 
  end_y_out = deploy_y_out + dir * track_length * head_y;
  deploy_x_in = end_x_out - dir * track_width * head_y;
  deploy_y_in = end_y_out + dir * track_width * head_x;
  end_x_in = deploy_x_out - dir * track_width * head_y;
  end_y_in = deploy_y_out + dir * track_width * head_x;
  
  if(first_iteration) 
    {
      // define track waypoints starting at current position
      first_iteration = false;
      deploy_x = curr_x;
      deploy_y = curr_y;
      distance_traveled = 0;
      leg = 0;
     }  
  else if (leg > 0)
    {
      // Projected distance in desired heading direction
      distance_traveled = (curr_x - deploy_x_out)*head_x + (curr_y - deploy_y_out)*head_y;
      // postMessage("RTRK_DEPLOY_X",deploy_x_out);
      // postMessage("RTRK_DEPLOY_Y",deploy_y_out);
      // postMessage("RTRK_END_X",end_x_out);
      // postMessage("RTRK_END_Y",end_y_out);
    }
  else 
    {
      // Projected distance in desired heading direction
      distance_traveled = (deploy_x_in -  curr_x)*head_x + (deploy_y_in - curr_y)*head_y;
      // postMessage("RTRK_DEPLOY_X",deploy_x_in);
      // postMessage("RTRK_DEPLOY_Y",deploy_y_in);
      // postMessage("RTRK_END_X",end_x_in);
      // postMessage("RTRK_END_Y",end_y_in);
    }

  if (distance_traveled > track_length)
     leg = -leg;

  if (distance_traveled < 0.0)
    postMessage("IN_TRACK","FALSE");
  else
    postMessage("IN_TRACK","TRUE");


  // Determine heading for trackline to other end
  double diff, corr;
  double fac = 5;

  if (leg == 0)
    {
      double dist_out = sqrt(pow(curr_x-deploy_x_out,2) + pow(curr_y-deploy_y_out,2));
      double dist_in = sqrt(pow(curr_x-deploy_x_in,2) + pow(curr_y-deploy_y_in,2));
      if (dist_out <= dist_in)
	{
	  leg = 1;
	  new_heading = radToHeading(atan2(end_y_out - curr_y,end_x_out - curr_x));
	}      
      else
	{
	  leg = -1;
	  new_heading = radToHeading(atan2(end_y_in - curr_y,end_x_in - curr_x));
	}
    }
  else if (leg > 0)
    {
      bearing = radToHeading(atan2(end_y_out - curr_y,end_x_out - curr_x));
      diff = angle180(angle360(bearing - heading_out));
      double dist = fabs(tan(M_PI/180.0 * diff)) * (track_length-distance_traveled);
      if ( fabs(diff)*exp((fac-1)*fabs(dist/dist_tolerance)) < approach_angle)
	  corr = diff * exp( (fac-1) * fabs(dist/dist_tolerance) );
      else if ( diff < 0 )
	corr = - approach_angle;
      else
	corr = approach_angle;

      new_heading = angle360(heading_out + corr);
    }
  else
    {
      bearing = radToHeading(atan2(end_y_in - curr_y,end_x_in - curr_x));
      diff = angle180(angle360(bearing - heading_in));
      double dist = fabs(tan(M_PI/180.0 * diff)) * (track_length-distance_traveled);
      if ( fabs(diff)*exp((fac-1)*fabs(dist/dist_tolerance)) < approach_angle)
	  corr = diff * exp((fac-1)*fabs(dist/dist_tolerance));
      else if ( diff < 0 )
	corr = - approach_angle;
      else
	corr = approach_angle;

      new_heading = angle360(heading_in + corr);
    }

  //      postMessage("DIST_TRAVELED",distance_traveled);
  //    postMessage("RTRK_LEG",double(leg));
  //    postMessage("RTRK_BEARING",bearing);
  //    postMessage("RTRK_DIFF",diff);
  //    postMessage("RTRK_NEW_HEADING",new_heading);
  
  ZAIC_PEAK zaic_heading(m_domain,"course");
  zaic_heading.setSummit(new_heading);
  zaic_heading.setValueWrap(true);
  zaic_heading.setPeakWidth(30.0);
  zaic_heading.setBaseWidth(60.0);
  zaic_heading.setSummitDelta(60.0);

  IvPFunction *ipf_heading = zaic_heading.extractOF();

  ZAIC_PEAK zaic_speed(m_domain, "speed");
  zaic_speed.setSummit(speed);
  zaic_speed.setSummitDelta(100.0);
  zaic_speed.setPeakWidth(0.5);
  zaic_speed.setMinMaxUtil(0, 100);
 
  IvPFunction *ipf_speed = zaic_speed.extractOF();

  OF_Coupler coupler;
  IvPFunction *new_ipf = coupler.couple(ipf_speed, ipf_heading);

  new_ipf->setPWT(m_priority_wt);

  return(new_ipf);

}

void BHV_RaceTrack::updateStart()
{
  if(!m_deploy_pending)
    return;
  
  m_deploy_location = tolower(m_deploy_location);

  if(m_deploy_location == "present_position") {
    deploy_x_out = deploy_x;
    deploy_y_out = deploy_y;
  }
  else {
    vector<string> svector = parseString(m_deploy_location, ',');
    if(svector.size() == 2) {
      if(isNumber(svector[0]) && isNumber(svector[1]))
	svector[0] = stripBlankEnds(svector[0]);
	svector[1] = stripBlankEnds(svector[1]);
	double xval = atof(svector[0].c_str());
	double yval = atof(svector[1].c_str());
	deploy_x_out = xval;
	deploy_y_out = yval;
    }
  }

  m_deploy_location = "";
  m_deploy_pending = false;
}

