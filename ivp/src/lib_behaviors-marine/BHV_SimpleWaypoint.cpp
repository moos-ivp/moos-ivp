/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_SimpleWaypoint.cpp                               */
/*    DATE: July 1st 2007  (For purposes of simp illustration)   */
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

#include "BHV_SimpleWaypoint.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_SimpleWaypoint::BHV_SimpleWaypoint(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("name", "simple_waypoint");
  m_domain = subDomain(m_domain, "course,speed");

  // All distances are in meters, all speed in meters per second
  // Default values for configuration parameters 
  m_desired_speed  = 0; 
  m_arrival_radius = 10;
  m_ptx = 0;
  m_pty = 0;

  // Default values for behavior state variables
  m_osx  = 0;
  m_osy  = 0;
  m_waypoint_set = false;

  addInfoVars("NAV_X, NAV_Y");
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition".
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_SimpleWaypoint::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);
  
  if(param == "ptx") {
    m_ptx = atof(val.c_str());
    return(true);
  }
  if(param == "pty") {
    m_pty = atof(val.c_str());
    return(true);
  }
  else if(param == "speed") {
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    m_desired_speed = dval;
    return(true);
  }
  else if(param == "radius") {
    double dval = atof(val.c_str());
    if(dval <= 0)
      return(false);
    m_arrival_radius = dval;
    return(true);
  }
  return(false);
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_SimpleWaypoint::onRunState() 
{
  ZAIC_PEAK spd_zaic(m_domain, "speed");
  spd_zaic.setSummit(m_desired_speed);
  spd_zaic.setBaseWidth(2.6);
  spd_zaic.setPeakWidth(0.0);
  spd_zaic.setSummitDelta(0.0);
  IvPFunction *spd_ipf = spd_zaic.extractOF();
  
  double rel_ang_to_wpt = relAng(m_osx, m_osy, m_ptx, m_pty);
  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setSummit(rel_ang_to_wpt);
  crs_zaic.setBaseWidth(180.0);
  crs_zaic.setValueWrap(true);
  IvPFunction *crs_ipf = crs_zaic.extractOF();
  
  OF_Coupler coupler;
  IvPFunction *ipf = coupler.couple(crs_ipf, spd_ipf);

  return(ipf);
}

