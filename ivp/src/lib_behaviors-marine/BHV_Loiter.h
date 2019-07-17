/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_Loiter.h                                         */
/*    DATE: July 26th 2005 In Elba w/ M.Grunde, P.Newman         */
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
 
#ifndef BHV_LOITER_HEADER
#define BHV_LOITER_HEADER

#include <string>
#include "IvPBehavior.h"
#include "XYPolygon.h"
#include "LoiterEngine.h"
#include "WaypointEngine.h"

class IvPDomain;
class BHV_Loiter : public IvPBehavior {
public:
  BHV_Loiter(IvPDomain);
  ~BHV_Loiter() {}
  
  bool         setParam(std::string, std::string);
  IvPFunction* onRunState();
  void         onIdleState();
  void         onIdleToRunState();
  void         onCompleteState();
  void         postConfigStatus();

protected:
  bool         updateInfoIn();
  void         updateCenter();
  void         updateLoiterMode();
  IvPFunction* buildIPF(std::string);
  void         postStatusReports();
  void         postViewablePolygon();
  void         postErasablePolygon();
  void         postViewablePoint();
  void         postErasablePoint();
  void         handleVisualHint(std::string);
  
private:  // Configuration Parameters
  bool         m_clockwise;
  bool         m_dynamic_clockwise;
  double       m_desired_speed;
  double       m_desired_speed_alt;
  double       m_acquire_dist;
  bool         m_center_activate;
  bool         m_use_alt_speed;

  std::string  m_ipf_type;
  std::string  m_var_suffix;

  // Visual Hint Defaults
  double       m_hint_vertex_size;
  double       m_hint_edge_size;
  std::string  m_hint_vertex_color;
  std::string  m_hint_edge_color;
  std::string  m_hint_nextpt_color;
  std::string  m_hint_nextpt_lcolor;
  double       m_hint_nextpt_vertex_size;
  std::string  m_hint_poly_label;

  std::string  m_loiter_mode;

  double       m_slingshot;
  
private:  // State Structures
  LoiterEngine   m_loiter_engine;
  WaypointEngine m_waypoint_engine;
  
private:  // State Variables
  bool         m_acquire_mode;     // true:acquiring, false:loitering
  double       m_dist_to_poly;     // Curr ownship dist to poly (mtrs)
  double       m_eta_to_poly;      // Curr ownship eta to poly (mtrs)
  double       m_ptx;              // Curr waypt   x/lon (mtrs)
  double       m_pty;              // Curr waypt   y/lat (mtrs)
  double       m_osx;              // Curr owhship x/lon (mtrs)
  double       m_osy;              // Curr ownship y/lat (mtrs)
  double       m_osh;              // Curr ownship Heading (degs)
  double       m_osv;              // Curr ownship Speed (m/s)

  double       m_patience;         // [1,99]
  
  bool         m_center_pending; 
  std::string  m_center_assign;

  double       m_bng_total;
  double       m_bng_last;
};
#endif



















