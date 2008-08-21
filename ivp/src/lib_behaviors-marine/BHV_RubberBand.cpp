/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_RubberBand.cpp                                  */
/*    DATE: Aug 25 2006                                          */
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
#include "BHV_RubberBand.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"


using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_RubberBand::BHV_RubberBand(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_waypoint");

  m_domain = subDomain(m_domain, "course,speed");

  // Default values for Configuration Parameters
  m_station_x    = 0;
  m_station_y    = 0;
  m_outer_radius = 15;
  m_inner_radius = 4;
  m_outer_speed  = 1.2;
  m_extra_speed  = 0;
  m_center_activate = false;

  // Default values for State  Variables
  m_center_pending  = false;
  m_center_assign   = "";
  m_station_set     = false;

  stiffness = 0.1;
  relevance = 1.0;

  // Declare information needed by this behavior
  addInfoVars("NAV_X, NAV_Y");
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition".
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_RubberBand::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  val = stripBlankEnds(val);

  if(param == "station_pt") {
    m_center_assign  = val;
    m_center_pending = true;
    return(updateCenter());
    return(true);
  }

  else if(param == "center_activate") {
    val = tolower(val);
    if((val!="true")&&(val!="false"))
      return(false);
    m_center_activate = (val == "true");
    return(true);
  }  

  else if(param == "outer_radius") {
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    m_outer_radius = dval;
    return(true);
  }

  else if(param == "inner_radius") {
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    m_inner_radius = dval;
    return(true);
  }

  else if(param == "outer_speed") {
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    m_outer_speed = dval;
    //    if(m_extra_speed < m_outer_speed)
    //  m_extra_speed = m_outer_speed;
    return(true);
  }

  else if(param == "extra_speed") {
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    m_extra_speed = dval;
    return(true);
  }

  else if(param == "stiffness") {
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    stiffness = dval;
    return(true);
  }

  return(false);
}


//-----------------------------------------------------------
// Procedure: onIdleState
//      Note: If m_center_pending is true, each time the behavior
//            goes inactive (and thus this function is called), 
//            a pending new center is declared, and set to the 
//            special value of present_position, which will be 
//            determined when the activation occurs.

void BHV_RubberBand::onIdleState()
{
  postStationMessage(false);
  if(!m_center_activate)
    return;
  m_center_pending = true;
  m_center_assign  = "present_position";
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_RubberBand::onRunState() 
{
  // Set m_osx, m_osy
  if(!updateInfoIn())
    return(0);

  updateCenter();

  if(!m_station_set) {
    postWMessage("STATION_NOT_SET");
    postStationMessage(false);
    return(0);
  }

  IvPFunction *ipf = 0;

  bool ok1, ok2;
  double nav_x = getBufferDoubleVal("NAV_X", ok1);
  double nav_y = getBufferDoubleVal("NAV_Y", ok2);

  // If no ownship position from info_buffer, return null
  if(!ok1 || !ok2) {
    postWMessage("No ownship X/Y info in info_buffer.");
    postStationMessage(false);
    return(0);
  }

  double dist_to_station  = distPointToPoint(nav_x, nav_y, 
					     m_station_x, m_station_y);

  // Post distance at integer precision unless close to zero
  if(dist_to_station <= 1)
    postMessage("DIST_TO_STATION", dist_to_station);
  else
    postIntMessage("DIST_TO_STATION", dist_to_station);

  postStationMessage(true);
  if(dist_to_station <= m_inner_radius)
    return(0);

  double angle_to_station = relAng(nav_x, nav_y, 
				   m_station_x, m_station_y);

  relevance = 0;

  double desired_speed = 0;
  if((dist_to_station > m_inner_radius) && (dist_to_station < m_outer_radius)) 
    {
      double range  = m_outer_radius - m_inner_radius;
      double pct    = (dist_to_station - m_inner_radius) / range;
      desired_speed = pct * m_outer_speed;
      relevance = 1.0;
    }

  if(dist_to_station >= m_outer_radius)
    {
      if (m_extra_speed > 0)
	desired_speed = m_extra_speed;
      else
	desired_speed = m_outer_speed;

      relevance += stiffness*(dist_to_station - m_outer_radius)/m_outer_radius;
    }

  ZAIC_PEAK spd_zaic(m_domain, "speed");
  spd_zaic.setSummit(desired_speed);
  spd_zaic.setBaseWidth(2.0);
  spd_zaic.setPeakWidth(0.5);
  spd_zaic.setSummitDelta(50.0);
  spd_zaic.setMinMaxUtil(0, 25);
  IvPFunction *spd_ipf = spd_zaic.extractOF();
  
  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setSummit(angle_to_station);
  crs_zaic.setPeakWidth(120);
  crs_zaic.setBaseWidth(60.0);
  crs_zaic.setSummitDelta(10.0);
  crs_zaic.setMinMaxUtil(0, 100);
  crs_zaic.setValueWrap(true);
  IvPFunction *crs_ipf = crs_zaic.extractOF();
  
  OF_Coupler coupler;
  ipf = coupler.couple(crs_ipf, spd_ipf);

  if(ipf)
    ipf->setPWT(relevance * m_priority_wt);

  return(ipf);
}


//-----------------------------------------------------------
// Procedure: updateInfoIn()
//   Purpose: Update info need by the behavior from the info_buffer.
//            Error or warning messages can be posted.
//   Returns: true if no vital info is missing from the info_buffer.
//            false otherwise.
//      Note: By posting an EMessage, this sets the state_ok member
//            variable to false which will communicate the gravity
//            of the situation to the helm.

bool BHV_RubberBand::updateInfoIn()
{
  bool ok1, ok2;
  // ownship position in meters from some 0,0 reference point.
  m_osx = getBufferDoubleVal("NAV_X", ok1);
  m_osy = getBufferDoubleVal("NAV_Y", ok2);

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2) {
    postEMessage("No ownship X/Y info in info_buffer.");
    return(false);
  }
  
  return(true);
}



//-----------------------------------------------------------
// Procedure: updateCenter()

bool BHV_RubberBand::updateCenter()
{
  if(!m_center_pending)
    return(true);
  
  bool ok_update = false;

  m_center_assign = tolower(m_center_assign);

  if(m_center_assign == "present_position") {
    m_station_x   = m_osx;
    m_station_y   = m_osy;
    m_station_set = true;
    ok_update = true;
  }
  else {
    vector<string> svector = parseString(m_center_assign, ',');
    if(svector.size() == 2) {
      svector[0] = stripBlankEnds(svector[0]);
      svector[1] = stripBlankEnds(svector[1]);
      if(isNumber(svector[0]) && isNumber(svector[1])) {
	double xval = atof(svector[0].c_str());
	double yval = atof(svector[1].c_str());
	m_station_x   = xval;
	m_station_y   = yval;
	m_station_set = true;
	ok_update     = true;
      }
    }
  }
  
  m_center_assign = "";
  m_center_pending = false;
  return(ok_update);
}


//-----------------------------------------------------------
// Procedure: postStationMessage()

void BHV_RubberBand::postStationMessage(bool post)
{
  string str_x = doubleToString(m_station_x,0);
  string str_y = doubleToString(m_station_y,0);
  string station = str_x + "," + str_y + ",";
  
  if(post)
    station += doubleToString(m_outer_radius,0) + ",";
  else
    station += "0,";

  station += m_us_name;
  postMessage("STATION_CIRCLE", station);

}


