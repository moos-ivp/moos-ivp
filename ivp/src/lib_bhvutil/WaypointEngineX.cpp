/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: WaypointEngineX.cpp                                  */
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

#include <iostream>
#include <cmath>
#include "WaypointEngineX.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//-----------------------------------------------------------
// Constructor()

WaypointEngineX::WaypointEngineX()
{
  // Config Vars
  m_reverse        = false;
  m_capture_line   = false;
  m_capture_radius = 3;
  m_slip_radius    = 15;
  m_max_repeats    = 0;  // -1 means unlimited cycles
  m_lead_distance  = 0;
  m_lead_damper    = 0;
  
  // State Vars
  m_curr_ix       = 0;
  m_state         = "transit";
  m_state_prev    = "transit";
  m_current_cpa   = -1;

  m_capt_hits     = 0;
  m_slip_hits     = 0;
  m_line_hits     = 0;
  m_repeats_sofar = 0;
  m_trackpt_set   = false;
}

//-----------------------------------------------------------
// Procedure: resetState()

void WaypointEngineX::resetState()
{
  m_seglist       = XYSegList();
  m_seglist_raw   = XYSegList();
  m_prevpt        = XYPoint();
  
  m_curr_ix       = 0;
  m_state         = "transit";
  m_state_prev    = "transit";
  m_current_cpa   = -1;

  m_capt_hits     = 0;
  m_slip_hits     = 0;
  m_line_hits     = 0;
  m_repeats_sofar = 0;
  m_trackpt_set   = false;
}

//-----------------------------------------------------------
// Procedure: setParam()

bool WaypointEngineX::setParam(string param, string value)
{
  param = tolower(param);
  string lvalue = tolower(value);

  bool handled = true;
  if(param == "order") {
    if((lvalue == "reverse") || (value == "reversed"))
      setReverse(true);
    else if(lvalue == "normal")
      setReverse(false);
    else if(lvalue == "toggle")
      setReverse(!m_reverse);
    else
      handled = false;
  }
  else if(param == "repeat") {
    if(lvalue == "forever")
      m_max_repeats = -1;
    else if(isNumber(value)) {
      int ival = atoi(value.c_str());
      if(ival < 0)
	handled = false;
      else
	m_max_repeats = ival;
    }
  }
  else if((param == "radius") || (param == "capture_radius")) {
    double dval = atof(value.c_str());
    if(dval < 0)
      handled = false;
    else
      m_capture_radius = dval;
  }
  else if((param == "nm_radius") || (param == "slip_radius")) {
    double dval = atof(value.c_str());
    if(dval < 0)
      handled = false;
    else
      m_slip_radius = dval;
  }
  else if(param == "capture_line") {
    if(lvalue == "true")
      m_capture_line = true;
    else if(lvalue == "false")
      m_capture_line = false;
    else if(lvalue == "absolute") {
      m_capture_line = true;
      m_capture_radius = 0;
      m_slip_radius = 0;
    }
    else
      handled = false;
  }
  else if(param == "lead")
    return(setNonNegDoubleOnString(m_lead_distance, value));
  else if(param == "lead_damper")
    return(setNonNegDoubleOnString(m_lead_damper, value));  
  else
    return(false);

  return(handled);
}

//-----------------------------------------------------------
// Procedure: setSegList()
//      Note: We store the value of m_reverse so the desired
//            effect is achieved regardless of whether the points
//            are set first, or the reverse-flag is set first.

void WaypointEngineX::setSegList(const XYSegList& g_seglist,
				 bool retain)
{
  m_state      = "transit";
  m_state_prev = "transit";

  // Reset the "current" cpa distance, s.t. a non-mono hit is 
  // impossible on the first iteration with a new seglist.
  m_current_cpa = -1;

  // The cycle count should also be set to zero since counting
  // cycles pertains to a given pattern, and once the pattern 
  // changes, the previous cycle count is irrelevant.
  m_repeats_sofar = 0;

  m_seglist_raw = g_seglist;
  m_seglist     = g_seglist;

  if(m_reverse)
    m_seglist.reverse();

  // This setting should reset the curr_ix to zero should this
  // function call come in-progress, unless explicitly retained.
  if(retain) {
    unsigned int vsize = m_seglist.size();
    if(m_curr_ix == 0)
      m_prevpt = m_seglist.get_point(vsize-1);
    else 
      m_prevpt = m_seglist.get_point(m_curr_ix-1);    
  }
  else 
    m_curr_ix = 0;
}

//-----------------------------------------------------------
// Procedure: setSegList()
//      Note: Convenience function

void WaypointEngineX::setSegList(const vector<XYPoint>& pts,
				 bool retain)
{
  XYSegList segl;
  for(unsigned int i=0; i<pts.size(); i++)
    segl.add_vertex(pts[i]);

  setSegList(segl, retain);
}

//-----------------------------------------------------------
// Procedure: setReverse()
//      Note: We store the value of m_reverse so the desired
//            effect is achieved regardless of whether the points
//            are set first, or the reverse-flag is set first.

void WaypointEngineX::setReverse(bool bval)
{
  // If this is not a change in state, return immediately.
  if(m_reverse == bval)
    return;

  m_reverse = bval;
  if(m_reverse) {
    m_seglist = m_seglist_raw;
    m_seglist.reverse();
  }
  else
    m_seglist = m_seglist_raw;
  
  // This setting should reset the curr_ix to zero should this
  // function call come in-progress.
  m_curr_ix    = 0;
  m_state      = "transit";
  m_state_prev = "transit";

  m_prevpt.invalidate();
}


//-----------------------------------------------------------
// Procedure: setReverseToggle()

void WaypointEngineX::setReverseToggle()
{
  setReverse(!m_reverse);
}

//-----------------------------------------------------------
// Procedure: setCaptureRadius()

void WaypointEngineX::setCaptureRadius(double g_capture_radius)
{
  if(g_capture_radius >= 0)
    m_capture_radius = g_capture_radius;
}

//-----------------------------------------------------------
// Procedure: setSlipRadius()

void WaypointEngineX::setSlipRadius(double radius)
{
  if(radius >= 0)
    m_slip_radius = radius;
}

//-----------------------------------------------------------
// Procedure: setCurrIndex()
//   Purpose: Typically the current index starts at zero and
//            is incremented as it hits waypoints. But we also
//            may want to let a user set this directly.

void WaypointEngineX::setCurrIndex(unsigned int index)
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

void WaypointEngineX::setCenter(double g_x, double g_y)
{
  m_seglist.new_center(g_x, g_y);
  m_seglist_raw.new_center(g_x, g_y);
}

//-----------------------------------------------------------
// Procedure: getPointX()

double WaypointEngineX::getPointX(unsigned int i) const
{
  if(i < m_seglist.size())
    return(m_seglist.get_vx(i));
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: getPointY()

double WaypointEngineX::getPointY(unsigned int i) const
{
  if(i < m_seglist.size())
    return(m_seglist.get_vy(i));
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: getTotalHits()

unsigned int WaypointEngineX::getTotalHits() const
{
  return(m_capt_hits + m_slip_hits + m_line_hits);
}

//-----------------------------------------------------------
// Procedure: resetForNewTraversal()
//      Note: Typically invoked on a behavior with perpetual
//            set to true. The behavior completes by may be
//            run again at a later time. 

void WaypointEngineX::resetForNewTraversal()
{
  m_curr_ix     = 0;
  m_state       = "transit";
  m_state_prev  = "transit";
  m_current_cpa = -1;

  m_prevpt.invalidate();
  
  m_repeats_sofar = 0;
}

//-----------------------------------------------------------
// Procedure: repeatsRemaining()

unsigned int WaypointEngineX::repeatsRemaining() const
{
  if(m_repeats_sofar > (unsigned int)(m_max_repeats))
    return(0);
  else
    return(m_max_repeats - m_repeats_sofar);
}

//-----------------------------------------------------------
// Procedure: setNextWaypoint()

string WaypointEngineX::setNextWaypoint(double osx, double osy)
{
  // Part 1: Ensure prevpt is something meaningful. If not
  // currently set, set to ownship position.  
  if(!m_prevpt.valid())
    m_prevpt.set_vertex(osx, osy);

  
  // Part 2: Apply ownship position, noting previous state
  m_state_prev = m_state;
  m_state = setNextWptAux(osx, osy);

  // Part 3: Good practice: set trackpt to curr waypoint.
  m_trackpt.set_vx(getPointX());
  m_trackpt.set_vy(getPointY());

  // Part 4: If transit, advanced or cycled, apply trackline
  if((m_state != "completed") && (m_state != "empty"))
    m_trackpt_set = calcTrackPoint(osx, osy);
  
  return(m_state);  
}

//-----------------------------------------------------------
// Procedure: setNextWaypointAux()
//   Returns: "empty", "completed", "cycled", "advanced",
//            or "transit"

string WaypointEngineX::setNextWptAux(double osx, double osy)
{
  // Phase 1: Initial checks and setting of present waypoint
  // --------------------------------------------------------------
  unsigned int vsize = m_seglist.size();
  if(vsize == 0)
    return("empty");
  if(hasCompleted())
    return("completed");
  
  double cx = getPointX();
  double cy = getPointY();

  // Phase 2A: Check for arrival based on capture radii.
  // --------------------------------------------------------------
  double dist = hypot((osx - cx), (osy - cy));

  // (m_current_cpa == -1) indicates first time this function called
  // or the engine has had its CPA reset by the caller.
  if((m_current_cpa == -1) || (dist < m_current_cpa))
    m_current_cpa = dist;
  
  // Determine if waypoint we had been seeking has been reached
  bool point_advance = false;
  if(dist < m_capture_radius) {
    point_advance = true;
    m_capt_hits++;
  }
  else {
    if((m_slip_radius > m_capture_radius) &&
       (dist > m_current_cpa) &&
       (m_current_cpa <= m_slip_radius)) {
      point_advance = true;
      m_slip_hits++;
    }
  }

  // Phase 2B: Check for arrival based on the capture line criteria
  // --------------------------------------------------------------
  if(!point_advance && m_capture_line && (m_prevpt.valid())) {
    double px = m_prevpt.x();
    double py = m_prevpt.y();
    
    double angle = angleFromThreePoints(cx, cy, px, py, osx, osy);
    if(angle >= 90) {
      point_advance = true;
      m_line_hits++;
    }
  }

  // Phase 3: Handle waypoint advancement
  // --------------------------------------------------------------
  // If waypoint has advanced, either
  // (1) just increment the waypoint, or
  // (2) start repeating the waypoints if perpetual or repeats !=0, or
  // (3) declare completion of the behavior.
  if(point_advance) {
    m_curr_ix++;

    if(m_curr_ix == 0)
      m_prevpt = m_seglist.get_point(vsize-1);
    else 
      m_prevpt = m_seglist.get_point(m_curr_ix-1);    

    if(m_curr_ix >= (int)(vsize)) {
      m_curr_ix = 0;
      m_current_cpa = -1;
      m_repeats_sofar++;
      
      if(m_max_repeats >= 0) {
	if((int)(m_repeats_sofar) > m_max_repeats) {
	  m_prevpt.invalidate();
	  return("completed");
	}
      }

      return("cycled");
    }

    double cx = getPointX();
    double cy = getPointY();
    m_current_cpa = hypot((osx - cx), (osy - cy));    
    return("advanced");
  }

  return("transit");
}


//-----------------------------------------------------------
// Procedure: distToEnd()

double WaypointEngineX::distToEnd(double osx, double osy) const
{
  double dist = distToNextWpt(osx, osy);

  dist += m_seglist.length(m_curr_ix);

  return(dist);
}


//-----------------------------------------------------------
// Procedure: distFromBeg()

double WaypointEngineX::distFromBeg(double osx, double osy) const
{
  double dist_to_end = distToEnd(osx, osy);

  double dist_from_beg = m_seglist.length() - dist_to_end;

  // Sanity check
  if(dist_from_beg < 0)
    return(0);

  return(dist_from_beg);
}


//-----------------------------------------------------------
// Procedure: distToPrevWpt()

double WaypointEngineX::distToPrevWpt(double osx, double osy) const
{
  if(!m_prevpt.valid())
    return(-1);

  return(distPointToPoint(osx, osy, m_prevpt.x(), m_prevpt.y()));
}


//-----------------------------------------------------------
// Procedure: distToNextWpt()

double WaypointEngineX::distToNextWpt(double osx, double osy) const
{
  if((m_curr_ix < 0) || ((unsigned int)(m_curr_ix) >= m_seglist.size()))
    return(-1);
  
  double curr_ptx = m_seglist.get_vx(m_curr_ix);
  double curr_pty = m_seglist.get_vy(m_curr_ix);
  double dist = distPointToPoint(osx, osy, curr_ptx, curr_pty);

  return(dist);
}

//-----------------------------------------------------------
// Procedure: pctToNextWpt()

double WaypointEngineX::pctToNextWpt(double osx, double osy) const
{
  if(!m_prevpt.valid())
    return(-1);

  double cx = getPointX();
  double cy = getPointY();
  double px = m_prevpt.x();
  double py = m_prevpt.y();  

  double dist_to_next = distPointToPoint(osx, osy, cx, cy);
  double dist_to_prev = distPointToPoint(osx, osy, px, py);
  double dist_total = dist_to_next + dist_to_prev; 

  if(dist_total <= 0)
    return(-1);

  return(dist_to_prev / dist_total);
}


//-----------------------------------------------------------
// Procedure: calcTrackPoint()
//      Note: Returns true if the trackpt is different from
//            the next waypoint.
//      Note: m_trackpt_distinct will be set to this ret value

bool WaypointEngineX::calcTrackPoint(double osx, double osy)
{
  // Sanity Check 1: Trackline following not enabled
  if((m_lead_distance <= 0) || (m_lead_damper <= 0))
    return(false);
  // Sanity Check 2: No prevpt properly set
  if(!m_prevpt.valid())
    return(false);

  // Part 1: Calculate perpendicular intersect pt on trackline
  double cx = getPointX();
  double cy = getPointY();
  double px = m_prevpt.x();
  double py = m_prevpt.y();  
  double nx = 0;
  double ny = 0;
  perpSegIntPt(px, py, cx, cy, osx, osy, nx, ny);
  XYPoint perp_pt(nx, ny);

  // Part 2: Calculate stretch distance base on lead_damper
  double damper_factor = 1.0;
  if(m_lead_damper > 0) {
    double dist_to_trackline = hypot((nx - osx), (ny - osy));
    if(dist_to_trackline < m_lead_damper) {
      double augment = 1 - (dist_to_trackline / m_lead_damper);
      damper_factor += (2 * augment);
    }
  }
  
  // Part 3: If stretch distance is beyond next pt, we're done
  double dist = distPointToPoint(nx, ny, cx, cy);
  double track_dist = m_lead_distance * damper_factor;
  // If trackpt is beyond nextpt, then trackpt IS nextpt
  if(dist <= track_dist) 
    return(false);

  // Calculate the trackpt
  double  angle = relAng(px, py, cx, cy);
  m_trackpt.projectPt(perp_pt, angle, track_dist);
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: getNextPoint()

XYPoint WaypointEngineX::getNextPoint() const
{
  XYPoint nullpt;
  if((m_curr_ix < 0) || (m_curr_ix >= (int)(m_seglist.size())))
    return(nullpt);

  return(m_seglist.get_point((unsigned int)(m_curr_ix)));
}

//-----------------------------------------------------------
// Procedure: hasCompleted()

bool WaypointEngineX::hasCompleted() const
{
  return(m_state == "completed");
}

//-----------------------------------------------------------
// Procedure: hasAdvanced()

bool WaypointEngineX::hasAdvanced() const
{
  if((m_state == "completed") ||
     (m_state == "cycled") ||
     (m_state == "advanced"))
    return(true);

  return(false);
}

//-----------------------------------------------------------
// Procedure: hasCycled()

bool WaypointEngineX::hasCycled() const
{
  if((m_state == "completed") ||
     (m_state == "cycled"))
    return(true);

  return(false);
}

//-----------------------------------------------------------
// Procedure: isUnderWay()

bool WaypointEngineX::isUnderWay() const
{
  if((m_state == "cycled") ||
     (m_state == "advanced") ||
     (m_state == "transiting"))
    return(true);
  
  return(false);
}

//-----------------------------------------------------------
// Procedure: hasStateChange()

bool WaypointEngineX::hasStateChange() const
{
  return(m_state != m_state_prev);
}

