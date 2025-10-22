/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
// Constructor()

WaypointEngine::WaypointEngine()
{
  // Config Vars
  m_reverse         = false;
  m_perpetual       = false;
  m_capture_line    = false;
  m_capture_radius  = 3;
  m_slip_radius     = 15;
  m_repeats_endless = false;
  m_repeats_allowed = 0;
  
  // State Vars
  m_curr_ix         = 0;
  m_prev_ix         = -1;
  m_complete        = true;
  m_current_cpa     = -1;
  m_capture_hits    = 0;
  m_nonmono_hits    = 0;
  m_cycle_count     = 0;
  m_repeats_sofar   = 0;
}

//-----------------------------------------------------------
// Procedure: resetState()

void WaypointEngine::resetState()
{
  m_seglist       = XYSegList();
  m_seglist_raw   = XYSegList();
  m_prevpt        = XYPoint();
  
  m_curr_ix       = 0;
  m_prev_ix       = -1;
  m_complete      = true;
  m_current_cpa   = -1;

  m_capture_hits  = 0;
  m_nonmono_hits  = 0;
  m_cycle_count   = 0;
  m_repeats_sofar = 0;
}

//-----------------------------------------------------------
// Procedure: setSegList()
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
// Procedure: setSegList()
//      Note: Convenience function

void WaypointEngine::setSegList(const vector<XYPoint>& pts)
{
  XYSegList segl;
  for(unsigned int i=0; i<pts.size(); i++)
    segl.add_vertex(pts[i]);

  setSegList(segl);
}

//-----------------------------------------------------------
// Procedure: addWaypoint()

void WaypointEngine::addWaypoint(const XYPoint& point)
{
  if(!m_reverse) {
    m_seglist_raw.add_vertex(point.x(), point.y());
    m_seglist.add_vertex(point.x(), point.y());
  }
}

//-----------------------------------------------------------
// Procedure: setReverse()
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
// Procedure: setReverseToggle()

void WaypointEngine::setReverseToggle()
{
  setReverse(!m_reverse);
}

//-----------------------------------------------------------
// Procedure: setCaptureRadius()

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
// Procedure: setSlipRadius()

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
// Procedure: setCenter()

void WaypointEngine::setCenter(double g_x, double g_y)
{
  m_seglist.new_center(g_x, g_y);
  m_seglist_raw.new_center(g_x, g_y);
}

//-----------------------------------------------------------
// Procedure: getPointX()

double WaypointEngine::getPointX(unsigned int i) const
{
  if(i < m_seglist.size())
    return(m_seglist.get_vx(i));
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: currPtChanged()
//      Note: Determine if the current index has changed between
//            successive calls to this function.

bool WaypointEngine::currPtChanged()
{
  bool changed = (m_curr_ix != m_prev_ix);
  
  m_prev_ix = m_curr_ix;
  return(changed);
}

//-----------------------------------------------------------
// Procedure: getPointY()

double WaypointEngine::getPointY(unsigned int i) const
{
  if(i < m_seglist.size())
    return(m_seglist.get_vy(i));
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: resetForNewTraversal()

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
// Procedure: setNextWaypoint()
//   Returns: "empty_seglist"
//            "completed"
//            "cycled"
//            "advanced"
//            "in-transit"
//      Note: Error can be checked for by checking if curr_ix == -1      


string WaypointEngine::setNextWaypoint(double os_x, double os_y)
{
  //cout << "curr_ix(a): " << m_curr_ix << endl;
  //cout << "prev_ix(a): " << m_prev_ix << endl;
  
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

    double angle = angleFromThreePoints(pt_x, pt_y, prevpt_x, prevpt_y,
					os_x, os_y);
    if(angle >= 90)
      point_advance = true;

    double rang = relAng(prevpt_x, prevpt_y, pt_x, pt_y);
    double x1,y1,x2,y2;
    projectPoint(rang+90, 20, pt_x, pt_y, x1, y1);
    projectPoint(rang-90, 20, pt_x, pt_y, x2, y2);
    m_capture_segl = XYSegList(x1,y1,x2,y2);
    m_capture_pt = XYPoint(prevpt_x, prevpt_y);
  }
  else {
    m_capture_segl = XYSegList();
    m_capture_pt = XYPoint();
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

    // An "normal" advance means that we no longer regard an
    // externally set prevpt to be valid.
    m_prevpt.invalidate();

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
  if(m_prevpt.valid())
    return(distPointToPoint(osx, osy, m_prevpt.x(), m_prevpt.y()));
  
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

//-----------------------------------------------------------
// Procedure: distToEndCycle()
//   Purpose: Calculate ownships remaining distance in curr cycle

double WaypointEngine::distToEndCycle(double osx, double osy) const
{
  // Sanity Check
  if((m_curr_ix < 0) || ((unsigned int)(m_curr_ix) >= m_seglist.size()))
    return(-1);

  // Be nice and use the proper type to avoid warnings
  unsigned int curr_ix = (unsigned int)(m_curr_ix);
  
  // Part 1: Distance from current position to next waypoint
  double curr_ptx = m_seglist.get_vx(curr_ix);
  double curr_pty = m_seglist.get_vy(curr_ix);
  double dist = distPointToPoint(osx, osy, curr_ptx, curr_pty);

  // Part 2: Distance for remaining legs in cycle
  for(unsigned int ix=curr_ix; ix<m_seglist.size()-1; ix++) {
    double next_ptx = m_seglist.get_vx(ix + 1);
    double next_pty = m_seglist.get_vy(ix + 1);
    double leg_dist = distPointToPoint(curr_ptx, curr_pty,
				       next_ptx, next_pty);

    curr_ptx = next_ptx;
    curr_pty = next_pty;
    
    dist += leg_dist;
  }
  
  return(dist);
}

//-----------------------------------------------------------
// Procedure: distToEndFinal()
//   Purpose: Calculate ownships remaining distance in curr cycle
//            Plus the distance in remaining full cycles

double WaypointEngine::distToEndFinal(double osx, double osy) const
{
  // Sanity Check: If endless repeats, the distance is infinite
  if(m_repeats_endless)
    return(-1);
  // Sanity check
  if((m_curr_ix < 0) || ((unsigned int)(m_curr_ix) >= m_seglist.size()))
    return(-1);

  // Part 1: Calculate dist to end of cycle
  double dist_to_end = distToEndCycle(osx, osy);

  // Part 2: Det remaining cycles. If zero, no need to calc cycle len
  unsigned int remaining_cycles = 0;
  if(m_repeats_allowed > m_repeats_sofar)
    remaining_cycles = m_repeats_allowed - m_repeats_sofar;
  if(remaining_cycles == 0)
    return(dist_to_end);
  
  // Part 3: Calc Cycle dist, including dist from finish to start.
  double cycle_dist = m_seglist.length();
  XYPoint first_pt = m_seglist.get_first_point();
  XYPoint last_pt  = m_seglist.get_last_point();
  double  gap_dist = distPointToPoint(first_pt, last_pt);
  cycle_dist += gap_dist;

  // Part 4: apply remaining cycles to overall dist remaining
  dist_to_end += (cycle_dist * remaining_cycles);

  return(dist_to_end);
}

//-----------------------------------------------------------
// Procedure: getPointsStr()

string WaypointEngine::getPointsStr() const
{
  string spec = m_seglist.get_spec_pts();
  return(spec);
}
  

