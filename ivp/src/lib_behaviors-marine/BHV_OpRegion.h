/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_OpRegion.h                                       */
/*    DATE: May 1st, 2005 Sunday at Joe's in Maine               */
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
 
#ifndef BHV_OPREGION_HEADER
#define BHV_OPREGION_HEADER

#include "XYPolygon.h"
#include "IvPBehavior.h"

class BHV_OpRegion : public IvPBehavior {
 public:
  BHV_OpRegion(IvPDomain);
  ~BHV_OpRegion() {}
  
  bool         setParam(std::string, std::string);
  void         onSetParamComplete();

  IvPFunction* onRunState();
  void         onIdleState();
  void         onCompleteState() {postErasablePolygon();}

 protected:
  void      polygonVerify();
  void      postPolyStatus();
  void      depthVerify();
  void      altitudeVerify();
  void      timeoutVerify();
  void      setTimeStamps();
  void      checkForReset();
  void      checkForSoftPolyReset();
  void      handleVisualHint(std::string);
  void      postViewablePolygon();
  void      postErasablePolygon();
  void      postBreachFlags(std::string);
  void      postTimeRemaining();

 protected: // Configuration Variables
  std::vector<XYPolygon> m_polygons;
  XYPolygon m_polygon;
  double    m_max_depth;
  double    m_min_altitude;
  double    m_max_time;
  double    m_trigger_entry_time;
  double    m_trigger_exit_time;
  bool      m_trigger_on_poly_entry;

  // Allow for possible reset once poly has been breached
  std::string m_reset_var;

  // Optional soft polygon breach
  bool m_soft_poly_breach;
  
  // Allow for possible posting of time_remaining
  std::string m_time_remaining_var;

  // Allow for possible posting of the opregion polygon
  std::string m_opregion_poly_var;

  // Allow for flags to be posted when/if a breach is made
  std::vector<VarDataPair>  m_breached_poly_flags;
  std::vector<VarDataPair>  m_breached_time_flags;
  std::vector<VarDataPair>  m_breached_altitude_flags;
  std::vector<VarDataPair>  m_breached_depth_flags;

  bool m_breached_poly_flags_posted;
  bool m_breached_time_flags_posted;
  bool m_breached_altitude_flags_posted;
  bool m_breached_depth_flags_posted;


  // Visual hints affecting properties of polygons/points
  std::string m_hint_vertex_color;
  std::string m_hint_edge_color;
  double      m_hint_vertex_size;
  double      m_hint_edge_size;


 protected: // State Variables
  bool      m_poly_entry_made;
  double    m_previous_time;  // Seconds
  double    m_current_time;
  double    m_delta_time;
  double    m_start_time;
  double    m_elapsed_time;

  double    m_secs_in_poly;
  double    m_secs_out_poly;

  bool      m_first_time;
  bool      m_previously_in_poly;

  // Keep track of when soft poly breach posted so later the 
  // behavior warning may be retracted.
  bool      m_soft_poly_breach_posted;
};
#endif









