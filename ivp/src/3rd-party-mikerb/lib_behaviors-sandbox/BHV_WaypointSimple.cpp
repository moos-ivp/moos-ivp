/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_WaypointSimple.cpp                               */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <iostream>
#include "MBTimer.h"
#include <math.h> 
#include "BHV_WaypointSimple.h"
#include "AOF_WaypointRateClosure.h"
#include "OF_Reflector.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_WaypointSimple::BHV_WaypointSimple(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("name", "(d)bhv_waypoint_simple");
  this->setParam("build_info", "uniform_box=course:3,speed:3");
  this->setParam("build_info", "uniform_grid=course:9,speed:6");
  m_domain = subDomain(m_domain, "course,speed");

  // All distances are in meters, all speed in meters per second

  // Default values for configuration parameters 
  m_desired_speed  = 0; 
  m_arrival_radius = 10;

  // Default values for behavior state variables
  m_osx  = 0;
  m_osy  = 0;
  m_ptx  = 0;
  m_pty  = 0;
  m_curr_waypt = 0;

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED");
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition".
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_WaypointSimple::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);
  
  if(param == "points") {
    bool ok = m_seglist.initialize(val);
    return(ok);
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
// Procedure: produceOF
//      Note: Posting an EMessage sets the state_ok variable to 
//            false and communicates the problem to the helm.

IvPFunction *BHV_WaypointSimple::produceOF() 
{
  // Determine the current waypoint based on updated information.
  if(!setNextWaypoint())
    return(0);
  else
    postMessage("WAYPOINT_INDEX", m_curr_waypt);

  
  IvPFunction *ipf = buildOF("aof");
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}



//-----------------------------------------------------------
// Procedure: setNextWaypoint

bool BHV_WaypointSimple::setNextWaypoint()
{
  // Handle the case where no waypoints are provided
  int vsize = m_seglist.size();
  if(vsize == 0)
    return(false);

  // Check for and get essential infor from the info_buffer
  bool ok1, ok2;
  m_osx = m_info_buffer->dQuery("NAV_X", ok1);
  m_osy = m_info_buffer->dQuery("NAV_Y", ok2);

  if(!ok1 || !ok2) {
    postEMessage("No ownship X/Y info in info_buffer.");
    return(false);
  }

  // Check whether we've hit the current target waypoint
  bool next_point = false;
  m_ptx = m_seglist.get_vx(m_curr_waypt);
  m_pty = m_seglist.get_vy(m_curr_waypt);
  double dist = hypot((m_osx - m_ptx),(m_osy - m_pty));
  if(dist < m_arrival_radius)
    next_point = true;

  // If appropriate, advance to the next waypoint
  if(next_point) {
    if(m_curr_waypt < ((vsize-1))) {
      m_curr_waypt++;
      m_ptx = m_seglist.get_vx(m_curr_waypt);
      m_pty = m_seglist.get_vy(m_curr_waypt);
    }
    else {
      setComplete();
      return(false);
    }
  }
  
  return(true);
}


//-----------------------------------------------------------
// Procedure: buildOF

IvPFunction *BHV_WaypointSimple::buildOF(string method) 
{
  IvPFunction *ipf = 0;

  MBTimer bravo_timer;
  bravo_timer.start();

  if(method == "zaic") {
    ZAIC_PEAK spd_zaic(m_domain, "speed");
    spd_zaic.setSummit(m_desired_speed);
    spd_zaic.setBaseWidth(2.6);
    spd_zaic.setPeakWidth(0.0);
    spd_zaic.setSummitDelta(0.0);
    IvPFunction *spd_of = spd_zaic.extractOF();

    double rel_ang_to_wpt = relAng(m_osx, m_osy, m_ptx, m_pty);
    ZAIC_PEAK crs_zaic(m_domain, "course");
    crs_zaic.setSummit(rel_ang_to_wpt);
    crs_zaic.setBaseWidth(180.0);
    crs_zaic.setValueWrap(true);
    IvPFunction *crs_of = crs_zaic.extractOF();

    OF_Coupler coupler;
    ipf = coupler.couple(crs_of, spd_of);
  }    
  else {
    MBTimer echo_timer;
    echo_timer.start();

    bool ok = true;
    AOF_WaypointRateClosure aof_wpt(m_domain);
    ok = ok && aof_wpt.setParam("desired_speed", m_desired_speed);
    ok = ok && aof_wpt.setParam("oslat", m_osy);
    ok = ok && aof_wpt.setParam("oslon", m_osx);
    ok = ok && aof_wpt.setParam("ptlat", m_pty);
    ok = ok && aof_wpt.setParam("ptlon", m_ptx);
    ok = ok && aof_wpt.initialize();
    double wtime_echo = echo_timer.get_float_cpu_time();
    double ctime_echo = echo_timer.get_float_wall_time();
    cout << "wtime_echo: " << wtime_echo << endl;
    cout << "ctime_echo: " << ctime_echo << endl;
    
    if(ok) {
      MBTimer charlie_timer;
      charlie_timer.start();
      OF_Reflector reflector(&aof_wpt);
      reflector.create(m_build_info);
      double wtime_charlie = charlie_timer.get_float_wall_time();
      double ctime_charlie = charlie_timer.get_float_cpu_time();
      cout << "wtime_charlie: " << wtime_charlie << endl;
      cout << "ctime_charlie: " << ctime_charlie << endl;

      MBTimer alpha_timer;
      alpha_timer.start();
      ipf = reflector.extractOF();
      double wtime_alpha = alpha_timer.get_float_wall_time();
      double ctime_alpha = alpha_timer.get_float_cpu_time();
      cout << "wtime_alpha: " << wtime_alpha << endl;
      cout << "ctime_alpha: " << ctime_alpha << endl;

    }
  }

  double wtime_bravo = bravo_timer.get_float_wall_time();
  double ctime_bravo = bravo_timer.get_float_cpu_time();
  cout << "wtime_bravo: " << wtime_bravo << endl;
  cout << "ctime_bravo: " << wtime_bravo << endl;

  return(ipf);
}

