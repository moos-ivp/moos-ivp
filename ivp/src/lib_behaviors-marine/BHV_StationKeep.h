/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_StationKeep.h                                    */
/*    DATE: Aug 25 2006                                          */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
 
#ifndef BHV_STATION_KEEP_HEADER
#define BHV_STATION_KEEP_HEADER

#include <string>
#include <vector>
#include <list>
#include "IvPBehavior.h"
#include "XYSegList.h"

class BHV_StationKeep : public IvPBehavior {
public:
  BHV_StationKeep(IvPDomain);
  ~BHV_StationKeep() {}
  
  bool         setParam(std::string, std::string);
  IvPFunction* onRunState();
  void         onIdleToRunState();
  void         onRunToIdleState();
  void         onCompleteState() {postStationMessage(false);}
  void         postConfigStatus();

protected:
  bool         updateInfoIn();  
  bool         updateCenter();
  void         postStationMessage(bool);
  void         updateHibernationState();
  bool         historyShowsProgressStart();
  bool         historyShowsProgressEnd();
  void         handleVisualHint(std::string);
  void         historyClear();

private:  // Configuration Parameters
  double      m_station_x;
  double      m_station_y;
  bool        m_station_set;
  double      m_static_station_x;
  double      m_static_station_y;
  bool        m_static_station_defined;

  double      m_outer_radius;
  double      m_inner_radius;
  bool        m_center_activate;

  // The outer speed is the speed at the outer radius. The 
  // desired_speed will decrease linearly until the inner
  // ring, and will be zero inside the inner ring.
  double      m_outer_speed;
  
  // The extra_speed is the desired speed should the
  // vehicle drift outside the outer radius.
  double      m_extra_speed;

  // A UUV station-keeping may need to hibernate until it drifts
  // beyond a radius. The hibernation allows it to float to zero
  // depth. It may need to dive to depth when it is not hibernating.
  double      m_pskeep_radius;
  std::string m_pskeep_variable;
  std::string m_station_ctr_var;

private:  // State Variables
  bool         m_center_pending;
  std::string  m_center_assign;
  double       m_osx;
  double       m_osy;
  double       m_currtime;
  double       m_dist_to_station;
  std::string  m_pskeep_state;     // pending_progress_start, noted_prog...
  std::string  m_transit_state;    // seeking_station, hibernating

  // Time at which the behavior transitioned from idle to running.
  double       m_mark_time;
  // Amount of time the station circle is allowed to drift with the
  // vehicle after first entering the station mode. Only relevant 
  // when activate under "center-active" style.
  double       m_swing_time;

  std::list<double> m_distance_history;    // distance
  std::list<double> m_distance_thistory;   // time recorded

  std::string  m_hint_vertex_color;
  std::string  m_hint_edge_color;
  std::string  m_hint_label_color;
  double       m_hint_vertex_size;
  double       m_hint_edge_size;
};

#endif









