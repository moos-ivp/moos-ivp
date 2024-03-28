/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_Waypoint.h                                       */
/*    DATE: Nov 2004 (original version - many changes since)     */
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
 
#ifndef BHV_WAYPOINT_HEADER
#define BHV_WAYPOINT_HEADER

#include <string>
#include "IvPBehavior.h"
#include "WaypointEngine.h"
#include "XYPoint.h"
#include "HintHolder.h"

class BHV_Waypoint : public IvPBehavior {
public:
  BHV_Waypoint(IvPDomain);
  ~BHV_Waypoint() {}
  
  bool           setParam(std::string, std::string);
  IvPFunction*   onRunState();
  void           onIdleState();
  void           onIdleToRunState();
  BehaviorReport onRunState(std::string);
  void           onRunToIdleState();
  void           onSetParamComplete();
  void           onCompleteState() {postErasables();}
  void           postConfigStatus();
  std::string    expandMacros(std::string);

protected:
  bool         updateInfoIn();
  bool         setNextWaypoint();
  IvPFunction* buildOF(std::string);
  
  void         checkLeadConditions();
  void         postStatusReport();
  void         postViewableSegList();
  void         postErasableSegList();
  void         postErasables();
  void         updateOdoDistance();
  void         markOdoLeg();
  std::string  getReverseStr() const;
  
protected: 
  WaypointEngine m_waypoint_engine;

protected: // configuration parameters
  double      m_cruise_speed;
  double      m_cruise_speed_alt;
  bool        m_lead_to_start;
  bool        m_use_alt_speed;
  double      m_lead_distance;
  double      m_lead_damper;
  bool        m_lead_allowed;
  bool        m_wpt_flag_on_start;
  bool        m_eager_prev_index_flag;
  std::string m_efficiency_measure;
  std::string m_ipf_type;

  // Configurable names of MOOS variables for reports
  std::string m_var_report;
  std::string m_var_index;
  std::string m_var_cyindex;
  std::string m_var_suffix;
  std::string m_var_dist_to_prev;
  std::string m_var_dist_to_next;

  // Var-Data flags for posting when behavior finishes cycle
  std::vector<VarDataPair> m_cycle_flags;
  std::vector<VarDataPair> m_wpt_flags;

  // Logic condition tied to use of the lead parameter
  vector<LogicCondition> m_lead_conditions;
  
  // Visual hints affecting properties of polygons/points
  HintHolder  m_hints;

protected: // intermediate or object global variables.
  
  // Time/Distance efficiency state information
  bool      m_odo_set_flag;
  bool      m_odo_leg_disq;
  double    m_odo_settime;
  double    m_odo_setx;
  double    m_odo_sety;
  bool      m_odo_virgin;

  bool      m_wpt_flag_published;
  
  double    m_dist_leg_odo;

  double    m_dist_total_odo;
  double    m_dist_total_linear;
  double    m_time_total_odo;
  double    m_time_total_linear;

  double    m_course_pct;
  double    m_speed_pct;

  double    m_osx_prev;
  double    m_osy_prev;

  XYPoint   m_nextpt;
  XYPoint   m_trackpt;
  XYPoint   m_markpt;

  bool      m_waypt_hit;
  
  XYPoint   m_prevpt;

  bool      m_greedy_tour_pending;
  bool      m_draw_path_loop;
  
  int       m_prev_cycle_index;
  int       m_prev_waypt_index;
  

};

#endif





