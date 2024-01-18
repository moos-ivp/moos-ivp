/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_AvoidObstacleX.h                                 */
/*    DATE: Aug 2nd 2006                                         */
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
 
#ifndef BHV_AVOID_OBSTACLE_V21_HEADER
#define BHV_AVOID_OBSTACLE_V21_HEADER

#include "IvPBehavior.h"
#include "ObShipModel.h"
#include "XYPolygon.h"

class BHV_AvoidObstacleV21 : public IvPBehavior {
public:
  BHV_AvoidObstacleV21(IvPDomain);
  ~BHV_AvoidObstacleV21() {}
  
  bool         setParam(std::string, std::string);
  void         onHelmStart();
  IvPFunction* onRunState();
  void         onIdleState();
  void         onCompleteState();
  void         onSetParamComplete();
  void         onIdleToRunState();
  void         onInactiveState();
  void         onEveryState(std::string);
  void         postConfigStatus();
  double       getDoubleInfo(std::string);
  bool         isConstraint() {return(true);}
  std::string  expandMacros(std::string);
  std::string  isDeprecated();

 protected: 
  bool   handleParamVisualHints(std::string);
  bool   handleParamRangeFlag(std::string);

  double getRelevance();
  bool   updatePlatformInfo();
  void   postViewablePolygons();
  void   postErasablePolygons();

  bool   applyBuffer();

  
 protected:
  ObShipModel m_obship_model;

 protected: // Config Params
  bool        m_use_refinery;
  std::string m_pwt_grade;

  std::string m_resolved_obstacle_var;
  std::string m_obstacle_id;

  std::vector<double>      m_rng_thresh;
  std::vector<VarDataPair> m_rng_flags;
  std::vector<VarDataPair> m_cpa_flags;

  bool        m_hide_deprecation_warning;
    
 protected: // State variables

  double  m_obstacle_relevance;
  bool    m_resolved_pending;

  bool    m_valid_cn_obs_info;
  
  bool    m_closing;
  double  m_cpa_rng_sofar;
  double  m_fpa_rng_sofar;
  double  m_cpa_rng_ever;
  double  m_cpa_reported;
  
  
 protected: // Config Visual hints
  std::string m_hint_obst_edge_color;
  std::string m_hint_obst_vertex_color;
  double      m_hint_obst_vertex_size;
  std::string m_hint_obst_fill_color;
  double      m_hint_obst_fill_transparency;

  std::string m_hint_buff_min_edge_color;
  std::string m_hint_buff_min_vertex_color;
  double      m_hint_buff_min_vertex_size;
  std::string m_hint_buff_min_fill_color;
  double      m_hint_buff_min_fill_transparency;

  std::string m_hint_buff_max_edge_color;
  std::string m_hint_buff_max_vertex_color;
  double      m_hint_buff_max_vertex_size;
  std::string m_hint_buff_max_fill_color;
  double      m_hint_buff_max_fill_transparency;

};

#endif

