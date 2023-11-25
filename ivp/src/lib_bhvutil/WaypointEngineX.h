/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: WaypointEngineX.h                                    */
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
 
#ifndef WAYPOINT_ENGINEX_HEADER
#define WAYPOINT_ENGINEX_HEADER

#include "XYSegList.h"
#include "XYPoint.h"

class WaypointEngineX {
 public:
  WaypointEngineX();
  ~WaypointEngineX() {}

  void   setSegList(const XYSegList& seglist, bool retain=false);
  void   setSegList(const std::vector<XYPoint>& pts, bool retain=false);

  bool   setParam(std::string, std::string);
  
  void   setReverse(bool);
  void   setReverseToggle();
  void   setCaptureRadius(double);
  void   setSlipRadius(double);
  void   setCurrIndex(unsigned int);
  void   setCenter(double, double);
  void   resetSegList(double, double y);
  void   resetForNewTraversal();
  void   resetState();
  void   setRepeat(int v)         {m_max_repeats=v;}
  void   setCaptureLine(bool v)   {m_capture_line = v;}

  double getPointX(unsigned int) const;
  double getPointY(unsigned int) const;  
  double getPointX() const         {return(m_seglist.get_vx(m_curr_ix));}
  double getPointY() const         {return(m_seglist.get_vy(m_curr_ix));}
  double getCaptureRadius() const  {return(m_capture_radius);}
  double getSlipRadius() const     {return(m_slip_radius);}
  bool   getReverse() const        {return(m_reverse);}
  bool   usingCaptureLine() const  {return(m_capture_line);}
  bool   trackPointSet() const     {return(m_trackpt_set);}
  int    getCurrIndex() const      {return(m_curr_ix);}
  void   resetCPA()                {m_current_cpa = -1;}

  unsigned int size() const           {return(m_seglist_raw.size());}
  unsigned int getCycleCount() const  {return(m_repeats_sofar);}
  unsigned int getCaptureHits() const {return(m_capt_hits);}
  unsigned int getSlipHits() const    {return(m_slip_hits);}
  unsigned int getLineHits() const    {return(m_line_hits);}
  unsigned int getTotalHits() const; 
  unsigned int repeatsRemaining() const;

  int getMaxRepeats() const {return(m_max_repeats);}
  
  XYSegList getSegList()    {return(m_seglist);}
  XYPoint   getTrackPoint() {return(m_trackpt);}
  XYPoint   getNextPoint() const;
  
  std::string setNextWaypoint(double osx, double osy);
  
  double  distToEnd(double osx, double osy) const;
  double  distFromBeg(double osx, double osy) const;
  double  distToPrevWpt(double osx, double osy) const;
  double  distToNextWpt(double osx, double osy) const;
  double  pctToNextWpt(double osx, double osy) const;
  
  bool   hasCompleted() const;
  bool   hasAdvanced() const;
  bool   hasCycled() const;
  bool   isUnderWay() const;
  bool   hasStateChange() const;

protected:
  bool        calcTrackPoint(double osx, double osy);
  std::string setNextWptAux(double osx, double osy);
  
  
protected: // Config Vars
  bool      m_reverse;
  bool      m_capture_line;
  double    m_capture_radius;
  double    m_slip_radius;
  int       m_max_repeats;
  double    m_lead_distance;
  double    m_lead_damper;
  
protected: // State Vars
  std::string  m_state;
  std::string  m_state_prev;
  unsigned int m_capt_hits;
  unsigned int m_slip_hits;
  unsigned int m_line_hits;
  unsigned int m_repeats_sofar;

  XYSegList m_seglist;      // in-use (maybe temp reversed)
  XYSegList m_seglist_raw;  // original version
  XYPoint   m_prevpt;
  XYPoint   m_nextpt;
  XYPoint   m_trackpt;
  int       m_curr_ix;
  double    m_current_cpa;
  bool      m_trackpt_set;
  
};

#endif

