/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: WaypointEngine.cpp                                   */
/*    DATE: May 6th, 2007                                        */
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
#include "WaypointEngine.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

WaypointEngine::WaypointEngine()
{
  m_curr_ix         = 0;
  m_prev_ix         = -1;
  m_reverse         = false;
  m_perpetual       = false;
  m_repeat          = 0;
  m_capture_radius  = 0;
  m_nm_radius       = 0;
  m_complete        = true;
  m_current_cpa     = -1;
  m_capture_hits    = 0;
  m_nonmono_hits    = 0;
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
  m_complete = false;
}

//-----------------------------------------------------------
// Procedure: setRepeat

void WaypointEngine::setRepeat(unsigned int g_repeat)
{
  if(g_repeat >= 0)
    m_repeat = g_repeat;
}

//-----------------------------------------------------------
// Procedure: setCaptureRadius

void WaypointEngine::setCaptureRadius(double g_capture_radius)
{
  if(g_capture_radius >= 0)
    m_capture_radius = g_capture_radius;
}

//-----------------------------------------------------------
// Procedure: setNonmonotonicRadius

void WaypointEngine::setNonmonotonicRadius(double g_nm_radius)
{
  if(g_nm_radius >= 0)
    m_nm_radius = g_nm_radius;
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
  if((index < 0) || (index >= m_seglist.size()))
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

double WaypointEngine::getPointX(unsigned int i)
{
  if((i >= 0) && (i < m_seglist.size()))
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

double WaypointEngine::getPointY(unsigned int i)
{
  if((i >= 0) && (i < m_seglist.size()))
    return(m_seglist.get_vy(i));
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: setNextWaypoint
//   Returns: true if waypoint index is advanced on this iteration
//            false if not advanced (or not advanced through error)
//      Note: Error can be checked for by checking if curr_ix == -1      


string WaypointEngine::setNextWaypoint(double os_x, double os_y)
{
  unsigned int vsize = m_seglist.size();
  if(vsize == 0)
    return("empty seglist");

  if(m_complete)
    return("completed");
  
  double pt_x  = m_seglist.get_vx(m_curr_ix);
  double pt_y  = m_seglist.get_vy(m_curr_ix);
  double dist  = hypot((os_x - pt_x),(os_y - pt_y));

  // (m_current_cpa == -1) indicates first time this function called
  if((m_current_cpa == -1) || (dist < m_current_cpa))
    m_current_cpa = dist;
  
  // Determine if waypoint we had been seeking has been reached
  bool point_advance = false;
  if(dist < m_capture_radius) {
    point_advance = true;
    m_capture_hits++;
  }
  else {
    if((m_nm_radius > m_capture_radius) &&
       (dist > m_current_cpa) &&
       (m_current_cpa <= m_nm_radius)) {
      point_advance = true;
      m_nonmono_hits++;
    }
  }

  // If waypoint has advanced, either
  // (1) just increment the waypoint, or
  // (2) start repeating the waypoints if perpetual or repeats !=0, or
  // (3) declare completion of the behavior.
  if(point_advance) {
    m_curr_ix++;
    if(m_curr_ix >= (int)(vsize)) {
      m_curr_ix = 0;
      if(!m_perpetual) {
	if(m_repeat > 0)
	  m_repeat--;
	else {
	  m_complete = true;
	  return("advanced and completed");
	}
      }
    }
    pt_x = m_seglist.get_vx(m_curr_ix);
    pt_y = m_seglist.get_vy(m_curr_ix);
    m_current_cpa = hypot((os_x - pt_x), (os_y - pt_y));    
  }

  return("advanced");
}


