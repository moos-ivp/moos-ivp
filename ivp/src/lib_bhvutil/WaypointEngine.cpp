/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: WaypointEngine.cpp                                   */
/*    DATE: May 6th, 2007                                        */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <iostream>
#include <cmath>
#include "WaypointEngine.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

WaypointEngine::WaypointEngine()
{
  m_curr_ix         = 0;
  m_prev_ix         = -1;
  m_reverse         = false;
  m_perpetual       = false;
  m_capture_line    = false;
  m_capture_radius  = 3;
  m_slip_radius     = 15;
  m_complete        = true;
  m_current_cpa     = -1;
  m_capture_hits    = 0;
  m_nonmono_hits    = 0;
  m_cycle_count     = 0;

  m_repeats_allowed = 0;
  m_repeats_sofar   = 0;
  m_repeats_endless = false;
}

//-----------------------------------------------------------
// Procedure: setSegList
//      Note: We store the value of m_reverse so the desired
//            effect is achieved regardless of whether the points
//            are set first, or the reverse-flag is set first.

void WaypointEngine::setSegList(const XYSegList& g_seglist)
{
  // This setting should reset the curr_ix to zero should this
  // function call come in-progress.
  m_curr_ix  = 0;
  m_complete = false;

  // Reset the "current" cpa distance, s.t. a non-mono hit is 
  // impossible on the first iteration with a new seglist.
  m_current_cpa = -1;

  // The cycle count should also be set to zero since counting
  // cycles pertains to a given pattern, and once the pattern 
  // changes, the previous cycle count is irrelevant.
  m_cycle_count = 0;

  // Also ensure that a call to currPtChanged() will be true.
  m_prev_ix  = -1;

  m_seglist_raw = g_seglist;
  m_seglist     = g_seglist;

  if(m_reverse)
    m_seglist.reverse();
}

//-----------------------------------------------------------
// Procedure: setReverse
//      Note: We store the value of m_reverse so the desired
//            effect is achieved regardless of whether the points
//            are set first, or the reverse-flag is set first.

void WaypointEngine::setReverse(bool g_val)
{
  // If this is not a change in state, return immediately.
  if(m_reverse == g_val)
    return;
  m_reverse = g_val;

  if(g_val == true) {
    m_seglist = m_seglist_raw;
    m_seglist.reverse();
  }
  else
    m_seglist = m_seglist_raw;
  
  // This setting should reset the curr_ix to zero should this
  // function call come in-progress.
  m_curr_ix  = 0;
  m_prev_ix  = -1;
  m_complete = false;
}


//-----------------------------------------------------------
// Procedure: setReverseToggle

void WaypointEngine::setReverseToggle()
{
  setReverse(!m_reverse);
}

//-----------------------------------------------------------
// Procedure: setCaptureRadius

void WaypointEngine::setCaptureRadius(double g_capture_radius)
{
  if(g_capture_radius >= 0)
    m_capture_radius = g_capture_radius;
}

//-----------------------------------------------------------
// Procedure: setNonmonotonicRadius  (To Be Depricated)

void WaypointEngine::setNonmonotonicRadius(double radius)
{
  if(radius >= 0)
    m_slip_radius = radius;
}

//-----------------------------------------------------------
// Procedure: setSlipRadius

void WaypointEngine::setSlipRadius(double radius)
{
  if(radius >= 0)
    m_slip_radius = radius;
}

//-----------------------------------------------------------
// Procedure: setCurrIndex
//   Purpose: Typically the current index starts at zero and
//            is incremented as it hits waypoints. But we also
//            may want to let a user set this directly, (for
//            example the loiter behavior that is working in
//            an acquire mode).

void WaypointEngine::setCurrIndex(unsigned int index)
{
  if(index >= m_seglist.size())
    return;
  
  // Need to set the current_cpa to -1 so the very next distance
  // calculated will be treated as the minimum.
  m_curr_ix     = index;
  m_current_cpa = -1;
}

//-----------------------------------------------------------
// Procedure: setCenter

void WaypointEngine::setCenter(double g_x, double g_y)
{
  m_seglist.new_center(g_x, g_y);
  m_seglist_raw.new_center(g_x, g_y);
}

//-----------------------------------------------------------
// Procedure: getPointX

double WaypointEngine::getPointX(unsigned int i) const
{
  if(i < m_seglist.size())
    return(m_seglist.get_vx(i));
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: currPtChanged
//      Note: Determine if the current index has changed between
//            successive calls to this function.

bool WaypointEngine::currPtChanged()
{
  bool changed = (m_curr_ix != m_prev_ix);
  
  m_prev_ix = m_curr_ix;
  return(changed);
}

//-----------------------------------------------------------
// Procedure: getPointY

double WaypointEngine::getPointY(unsigned int i) const
{
  if(i < m_seglist.size())
    return(m_seglist.get_vy(i));
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: resetForNewTraversal

void WaypointEngine::resetForNewTraversal()
{
  m_repeats_sofar = 0;
  m_current_cpa   = -1;
  m_curr_ix       = 0;
  m_prev_ix       = -1;
  m_complete      = false;
}

//-----------------------------------------------------------
// Procedure: resetsRemaining()
//   Returns: 

unsigned int WaypointEngine::resetsRemaining() const
{
  if(m_repeats_sofar > m_repeats_allowed)
    return(0);
  else
    return(m_repeats_allowed - m_repeats_sofar);
}

//-----------------------------------------------------------
// Procedure: setNextWaypoint
//   Returns: "empty_seglist"
//            "completed"
//            "cycled"
//            "advanced"
//            "in-transit"
//      Note: Error can be checked for by checking if curr_ix == -1      


string WaypointEngine::setNextWaypoint(double os_x, double os_y)
{
  // Phase 1: Initial checks and setting of present waypoint
  // --------------------------------------------------------------
  unsigned int vsize = m_seglist.size();
  if(vsize == 0)
    return("empty_seglist");
  if(m_complete)
    return("completed");
  
  double pt_x  = m_seglist.get_vx(m_curr_ix);
  double pt_y  = m_seglist.get_vy(m_curr_ix);


  // Phase 2A: Check for arrival based on capture radii.
  // --------------------------------------------------------------
  double dist  = hypot((os_x - pt_x),(os_y - pt_y));

  // (m_current_cpa == -1) indicates first time this function called
  // or the engine has had its CPA reset by the caller.
  if((m_current_cpa == -1) || (dist < m_current_cpa))
    m_current_cpa = dist;
  
  // Determine if waypoint we had been seeking has been reached
  bool point_advance = false;
  if(dist < m_capture_radius) {
    point_advance = true;
    m_capture_hits++;
  }
  else {
    if((m_slip_radius > m_capture_radius) &&
       (dist > m_current_cpa) &&
       (m_current_cpa <= m_slip_radius)) {
      point_advance = true;
      m_nonmono_hits++;
    }
  }

  // Phase 2B: Check for arrival based on the capture line criteria
  // --------------------------------------------------------------
  if(m_capture_line && (m_prevpt.valid())) {
    double prevpt_x = m_prevpt.get_vx();
    double prevpt_y = m_prevpt.get_vy();
    
    double angle = angleFromThreePoints(pt_x, pt_y, prevpt_x, prevpt_y, os_x, os_y);
    if(angle >= 90)
      point_advance = true;
  }

  // Phase 3: Handle waypoint advancement
  // --------------------------------------------------------------
  // If waypoint has advanced, either
  // (1) just increment the waypoint, or
  // (2) start repeating the waypoints if perpetual or repeats !=0, or
  // (3) declare completion of the behavior.
  if(point_advance) {
    m_curr_ix++;
    if(m_curr_ix >= (int)(vsize)) {
      m_cycle_count++;
      m_curr_ix = 0;
      m_current_cpa = -1;
      if(m_perpetual == true) {
	m_repeats_sofar++;
	if(!m_repeats_endless && (m_repeats_sofar > m_repeats_allowed))
	  m_complete = true;
      }
      else
	m_complete = true;

      if(m_complete)
	return("completed");
      else
	return("cycled");
    }
    
    if(m_curr_ix == 0)
      m_prev_ix = vsize - 1;
    else
      m_prev_ix = m_curr_ix - 1;

    pt_x = m_seglist.get_vx(m_curr_ix);
    pt_y = m_seglist.get_vy(m_curr_ix);
    m_current_cpa = hypot((os_x - pt_x), (os_y - pt_y));    
    return("advanced");
  }

  return("in-transit");
}


//-----------------------------------------------------------
// Procedure: distToPrevWpt()

double WaypointEngine::distToPrevWpt(double osx, double osy) const
{
  if((m_prev_ix < 0) || ((unsigned int)(m_prev_ix) >= m_seglist.size()))
    return(-1);
  
  double prev_ptx = m_seglist.get_vx(m_prev_ix);
  double prev_pty = m_seglist.get_vy(m_prev_ix);

  double dist = distPointToPoint(osx, osy, prev_ptx, prev_pty);

  return(dist);
}


//-----------------------------------------------------------
// Procedure: distToNextWpt()

double WaypointEngine::distToNextWpt(double osx, double osy) const
{
  if((m_curr_ix < 0) || ((unsigned int)(m_curr_ix) >= m_seglist.size()))
    return(-1);
  
  double curr_ptx = m_seglist.get_vx(m_curr_ix);
  double curr_pty = m_seglist.get_vy(m_curr_ix);

  double dist = distPointToPoint(osx, osy, curr_ptx, curr_pty);

  return(dist);
}





