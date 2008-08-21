/*****************************************************************/
/*    NAME: Michael Benjamin and Henrik Schmidt                  */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_AbortToPoint.cpp                                 */
/*    DATE: August 6th 2008                                      */
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
#include <stdlib.h>
#include "BHV_AbortToPoint.h"
#include "OF_Reflector.h"
#include "AOF_Waypoint.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BuildUtils.h"
#include "XYBuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_AbortToPoint::BHV_AbortToPoint(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_waypoint");
  m_domain = subDomain(m_domain, "course,speed");

  m_cruise_speed  = 0;  // Meters/second
  m_ipf_type      = "zaic";

  // The completed param is initialized in superclass
  // but we initialize here just to be safe and clear.
  m_completed = false; 

  m_osx     = -1;
  m_osy     = -1;
  m_osv     = -1;
  m_abort_x = -1;
  m_abort_y = -1;

  m_capture_radius = 25;
  m_nm_radius      = 200;

  m_state              = "inactive";
  m_sent_msg_inactive  = false;
  m_sent_msg_completed = false;
  m_sent_msg_abandoned = false;

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED");
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition".
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_AbortToPoint::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  if(param == "point") {
    XYPoint apt = stringToPoint(val);
    if(!apt.valid())
      return(false);
    m_abort_x = apt.get_vx();
    m_abort_y = apt.get_vy();
    return(true);
  }
  else if(param == "speed") {
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    m_cruise_speed = dval;
    return(true);
  }
  else if(param == "ipf-type") {
    val = tolower(val);
    if((val=="zaic") || (val=="roc") || (val=="rate_of_closure"))
      m_ipf_type = val;
    return(true);
  }
  else if(param == "radius") {
    double dval = atof(val.c_str());
    if(dval <= 0)
      return(false);
    m_capture_radius = dval;
    return(true);
  }
  else if(param == "nm_radius")  {
    double dval = atof(val.c_str());
    if(dval <= 0) 
      return(false);
    m_nm_radius = dval;
    return(true);
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_AbortToPoint::onIdleState() 
{
  updateInfoOut(false);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_AbortToPoint::onRunState() 
{
  // Set m_osx, m_osy, m_osv
  if(!updateInfoIn()) {
    updateInfoOut(false);
    return(0);
  }
  
  IvPFunction *ipf = buildOF(m_ipf_type);
  if(ipf)
    ipf->setPWT(m_priority_wt);

  updateInfoOut(true);

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

bool BHV_AbortToPoint::updateInfoIn()
{
  bool ok1, ok2, ok3;
  m_osx = getBufferDoubleVal("NAV_X",     ok1);
  m_osy = getBufferDoubleVal("NAV_Y",     ok2);
  m_osv = getBufferDoubleVal("NAV_SPEED", ok3);

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2) {
    postEMessage("No ownship X/Y info in info_buffer.");
    return(false);
  }

  // If NAV_SPEED info is not found in the info_buffer, its
  // not a show-stopper. A warning will be posted.
  if(!ok3) {
    postWMessage("No ownship NAV_SPEED in info_buffer.");
    return(false);
  }

  return(true);
}

//-----------------------------------------------------------
// Procedure: buildOF

IvPFunction *BHV_AbortToPoint::buildOF(string method) 
{
  IvPFunction *ipf = 0;

  if((method == "roc") || (method == "rate_of_closure")) {
    bool ok = true;
    AOF_Waypoint aof_wpt(m_domain);
    ok = ok && aof_wpt.setParam("desired_speed", m_cruise_speed);
    ok = ok && aof_wpt.setParam("osx", m_osx);
    ok = ok && aof_wpt.setParam("osy", m_osy);
    ok = ok && aof_wpt.setParam("ptx", m_abort_x);
    ok = ok && aof_wpt.setParam("pty", m_abort_y);
    ok = ok && aof_wpt.initialize();
    
    if(ok) {
      OF_Reflector reflector(&aof_wpt);
      reflector.create(600, 500);
      string info = reflector.getUniformPieceStr();
      ipf = reflector.extractOF();
    }
  }    
  else { // if (method == "zaic")
    ZAIC_PEAK spd_zaic(m_domain, "speed");
    spd_zaic.setParams(m_cruise_speed, 0, 2.6, 0, 0, 100);
    IvPFunction *spd_of = spd_zaic.extractOF();
    
    double rel_ang_to_wpt = relAng(m_osx, m_osy, m_abort_x, m_abort_y);
    ZAIC_PEAK crs_zaic(m_domain, "course");
    crs_zaic.setValueWrap(true);
    crs_zaic.setParams(rel_ang_to_wpt, 0, 180, 50, 0, 100);
    IvPFunction *crs_of = crs_zaic.extractOF();

    OF_Coupler coupler;
    ipf = coupler.couple(crs_of, spd_of);
  }    
  return(ipf);
}

//-----------------------------------------------------------
// Procedure: updateInfoOut()

void BHV_AbortToPoint::updateInfoOut(bool post)
{
  if((m_state == "inactive") && (!m_sent_msg_inactive)) {
    m_sent_msg_inactive = true;
    postMessage(m_status_var, "INACTIVE");
  }
  else if(m_state == "in_progress")
    postMessage(m_status_var, "IN-PROGRESS");
  else if((m_state == "completed") && (!m_sent_msg_completed)) {
    m_sent_msg_completed = true;
    postMessage(m_status_var, "COMPLETED");
  }
  else if((m_state == "abandoned") && (!m_sent_msg_abandoned)) {
    m_sent_msg_abandoned = true;
    postMessage(m_status_var, "ABANDONED");
  }
}


