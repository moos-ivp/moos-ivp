/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_RStationKeep.cpp                                 */
/*    DATE: Oct 08 2007                                          */
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
#include "BHV_RStationKeep.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"


using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_RStationKeep::BHV_RStationKeep(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_waypoint");

  m_domain = subDomain(m_domain, "course,speed");

  // Default values for Configuration Parameters
  m_station_range = 0;
  m_station_angle = 0;
  m_outer_radius  = 25;
  m_inner_radius  = 5;
  m_outer_speed   = 1.4;
  m_extra_speed   = 2.0;
  m_center_activate = false;

  // Default values for State  Variables
  m_center_pending  = false;
  m_center_assign   = "";
  m_station_set     = false;

  // Declare information needed by this behavior
  addInfoVars("NAV_X, NAV_Y");
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition".
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_RStationKeep::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  val = stripBlankEnds(val);

  if(param == "rstation_position") {
    m_center_assign  = val;
    m_center_pending = true;
    return(updateRCenter());
    return(true);
  }
  
  if((param == "them") || (param == "contact")) {
    m_contact = toupper(val);
    addInfoVars(m_contact+"_NAV_X");
    addInfoVars(m_contact+"_NAV_Y");
    addInfoVars(m_contact+"_NAV_SPEED");
    addInfoVars(m_contact+"_NAV_HEADING");
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
    if(m_extra_speed < m_outer_speed)
      m_extra_speed = m_outer_speed;
    return(true);
  }

  else if(param == "extra_speed") {
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    m_extra_speed = dval;
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

void BHV_RStationKeep::onIdleState()
{
  postStationMessage(false);
  if(!m_center_activate)
    return;
  m_center_pending = true;
  m_center_assign  = "present_position";
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_RStationKeep::onRunState() 
{
  // Set m_osx, m_osy
  if(!updateInfoIn())
    return(0);

  updateRCenter();

  if(!m_station_set) {
    postWMessage("RSTATION_NOT_SET");
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

  if(dist_to_station <= 1)
    postMessage("DIST_TO_RSTATION", dist_to_station);
  else
    postIntMessage("DIST_TO_RSTATION", dist_to_station);

  postStationMessage(true);
  if(dist_to_station <= m_inner_radius)
    return(0);

  double angle_to_station = relAng(nav_x, nav_y, 
				   m_station_x, m_station_y);

  double desired_speed = 0;
  if((dist_to_station > m_inner_radius) && (dist_to_station < m_outer_radius)) {
    double range  = m_outer_radius - m_inner_radius;
    double pct    = (dist_to_station - m_inner_radius) / range;
    desired_speed = pct * m_outer_speed;
  }

  if(dist_to_station >= m_outer_radius)
    desired_speed = m_extra_speed;

  ZAIC_PEAK spd_zaic(m_domain, "speed");
  spd_zaic.setSummit(desired_speed);
  spd_zaic.setBaseWidth(0.4);
  spd_zaic.setPeakWidth(0.0);
  spd_zaic.setSummitDelta(0.0);
  spd_zaic.setMinMaxUtil(0, 25);
  IvPFunction *spd_ipf = spd_zaic.extractOF();
  
  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setSummit(angle_to_station);
  crs_zaic.setBaseWidth(180.0);
  crs_zaic.setValueWrap(true);
  IvPFunction *crs_ipf = crs_zaic.extractOF();
  
  OF_Coupler coupler;
  ipf = coupler.couple(crs_ipf, spd_ipf);

  if(ipf)
    ipf->setPWT(m_priority_wt);

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

bool BHV_RStationKeep::updateInfoIn()
{
  bool ok1, ok2, ok3, ok4, ok5, ok6;
  // ownship position in meters from some 0,0 reference point.
  m_osx = getBufferDoubleVal("NAV_X", ok1);
  m_osy = getBufferDoubleVal("NAV_Y", ok2);

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2) {
    postEMessage("No ownship X/Y info in info_buffer.");
    return(false);
  }

  m_cnx = getBufferDoubleVal(m_contact+"_NAV_X", ok3);
  m_cny = getBufferDoubleVal(m_contact+"_NAV_Y", ok4);
  m_cnh = getBufferDoubleVal(m_contact+"_NAV_HEADING", ok5);
  m_cnv = getBufferDoubleVal(m_contact+"_NAV_SPEED", ok6);

  if(!ok3)
    postWMessage("No contact NAV_X in info_buffer.");
  if(!ok4)
    postWMessage("No contact NAV_Y in info_buffer.");
  if(!ok5)
    postWMessage("No contact NAV_HEADING in info_buffer.");
  if(!ok6)
    postWMessage("No contact NAV_SPEED in info_buffer.");

  if(!ok1 || !ok2 || !ok3 || !ok4 || !ok5 || !ok6)
    return(false);
  
  double adjusted_angle = angle360(m_station_angle + m_cnh);
  projectPoint(adjusted_angle, m_station_range, m_cnx, m_cny, 
	       m_station_x, m_station_y);    

  return(true);
}



//-----------------------------------------------------------
// Procedure: updateRCenter()

bool BHV_RStationKeep::updateRCenter()
{
  if(!m_center_pending)
    return(true);
  
  bool ok_update = false;

  m_center_assign = tolower(m_center_assign);

  if(m_center_assign == "present_position") {
    
    double curr_range = hypot((m_osx-m_cnx),(m_osy-m_cny));
    double curr_adj_angle = relAng(m_cnx, m_cny, m_osx, m_osy) - m_cnh;
    
    m_station_range = curr_range;
    m_station_angle = angle360(curr_adj_angle);
    
    m_station_x = m_osx;
    m_station_y = m_osy;
    
    m_station_set = true;
    ok_update = true;
  }
  else {
    vector<string> svector = parseString(m_center_assign, ',');
    if(svector.size() == 2) {
      svector[0] = stripBlankEnds(svector[0]);
      svector[1] = stripBlankEnds(svector[1]);
      if(isNumber(svector[0]) && isNumber(svector[1])) {
	double range = atof(svector[0].c_str());
	double angle = atof(svector[1].c_str());
	
	m_station_range = range;
	m_station_angle = angle;
	
	double adjusted_angle = angle360(m_station_angle + m_cnh);
	projectPoint(adjusted_angle, m_station_range, m_cnx, m_cny, 
		     m_station_x, m_station_y);    

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

void BHV_RStationKeep::postStationMessage(bool post)
{
  string str_x = doubleToString(m_station_x,1);
  string str_y = doubleToString(m_station_y,1);
  string station = str_x + "," + str_y + ",";
  
  if(post)
    station += doubleToString(m_outer_radius,1) + ",";
  else
    station += "0,";

  station += m_us_name + "_rstation";
  postMessage("RSTATION_CIRCLE", station);

  string ptmsg;
  if(post) {
    ptmsg += "x=" + dstringCompact(doubleToString(m_station_x,0));
    ptmsg += ",y=" + dstringCompact(doubleToString(m_station_y,0));
    ptmsg += ",label=" + m_us_name + "_rstation";
    ptmsg += ",type=rstation";
  }
  else {
    ptmsg += "x=0,y=0"; 
    ptmsg += ",label=" + m_us_name + "_rstation";
    ptmsg += ",type=rstation, active=false";
  }    

  postMessage("RVIEW_POINT", ptmsg);
}


