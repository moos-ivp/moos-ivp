/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: WaypointEngine.h                                     */
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
 
#ifndef WAYPOINT_ENGINE_HEADER
#define WAYPOINT_ENGINE_HEADER

#include "XYSegList.h"
#include "XYPoint.h"

class WaypointEngine {
 public:
  WaypointEngine();
  ~WaypointEngine() {}

  void   setSegList(const XYSegList& seglist);
  void   setReverse(bool);
  void   setReverseToggle();
  void   setCaptureRadius(double);
  void   setNonmonotonicRadius(double);
  void   setSlipRadius(double);
  void   setCurrIndex(unsigned int);
  void   setCenter(double, double);
  void   resetSegList(double, double y);
  void   resetForNewTraversal();
  void   setRepeat(unsigned int v)  {m_repeats_allowed = v;}
  void   setCaptureLine(bool v)     {m_capture_line = v;}
  void   setPerpetual(bool v)       {m_perpetual = v;}
  void   setRepeatsEndless(bool v)  {m_repeats_endless=v;}

  double getPointX(unsigned int) const;
  double getPointY(unsigned int) const;  
  double getPointX() const      {return(m_seglist.get_vx(m_curr_ix));}
  double getPointY() const      {return(m_seglist.get_vy(m_curr_ix));}
  bool   isComplete() const     {return(m_complete);}
  int    getCurrIndex() const   {return(m_curr_ix);}
  bool   currPtChanged();
  void   resetCPA()             {m_current_cpa = -1;}

  unsigned int size() const     {return(m_seglist_raw.size());}
  unsigned int getCycleCount()  {return(m_cycle_count);}
  unsigned int getTotalHits()   {return(m_capture_hits + m_nonmono_hits);}
  unsigned int getCaptureHits() {return(m_capture_hits);}
  unsigned int getNonmonoHits() {return(m_nonmono_hits);}
  unsigned int resetsRemaining() const;

  XYSegList getSegList()  {return(m_seglist);}

  void        setPrevPoint(XYPoint pt) {m_prevpt=pt;}
  std::string setNextWaypoint(double osx, double osy);
  
  double  distToPrevWpt(double osx, double osy) const;
  double  distToNextWpt(double osx, double osy) const;


 protected:
  XYSegList m_seglist;
  XYSegList m_seglist_raw;

  bool      m_reverse;
  bool      m_perpetual;
  double    m_capture_radius;
  double    m_slip_radius;
  bool      m_capture_line;

  double    m_current_cpa;
  bool      m_complete;
  int       m_prev_ix;
  int       m_curr_ix;
  unsigned int  m_cycle_count;

  XYPoint   m_prevpt;

  bool          m_repeats_endless;
  unsigned int  m_repeats_allowed;
  unsigned int  m_repeats_sofar;


  unsigned int  m_capture_hits;
  unsigned int  m_nonmono_hits;
};

#endif
