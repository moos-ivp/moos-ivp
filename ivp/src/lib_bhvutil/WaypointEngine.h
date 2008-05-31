/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: WaypointEngine.h                                     */
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
 
#ifndef WAYPOINT_ENGINE_HEADER
#define WAYPOINT_ENGINE_HEADER

#include "XYSegList.h"

class WaypointEngine {
 public:
  WaypointEngine();
  ~WaypointEngine() {};

  void   setSegList(const XYSegList& seglist);
  void   setReverse(bool);
  void   setRepeat(unsigned int);
  void   setCaptureRadius(double);
  void   setNonmonotonicRadius(double);
  void   setCurrIndex(unsigned int);
  void   setPerpetual(bool v) {m_perpetual = v;};
  void   setCenter(double, double);
  void   reset() {m_complete=false; m_curr_ix=0;};
  
  double getPointX(unsigned int);
  double getPointY(unsigned int);  
  double getPointX()      {return(m_seglist.get_vx(m_curr_ix));};
  double getPointY()      {return(m_seglist.get_vy(m_curr_ix));};
  bool   isComplete()     {return(m_complete);};
  int    getCurrIndex()   {return(m_curr_ix);};
  int    getCaptureHits() {return(m_capture_hits);};
  int    getNonmonoHits() {return(m_nonmono_hits);};
  int    getTotalHits()   {return(m_capture_hits + m_nonmono_hits);};
  bool   currPtChanged();

  XYSegList getSegList()  {return(m_seglist);};

  std::string setNextWaypoint(double os_x, double os_y);

 protected:
  XYSegList m_seglist;
  XYSegList m_seglist_raw;

  bool      m_reverse;
  bool      m_perpetual;
  double    m_capture_radius;
  double    m_nm_radius;

  double    m_current_cpa;
  bool      m_complete;
  int       m_prev_ix;
  int       m_curr_ix;

  unsigned int  m_repeat;
  unsigned int  m_capture_hits;
  unsigned int  m_nonmono_hits;
};

#endif








