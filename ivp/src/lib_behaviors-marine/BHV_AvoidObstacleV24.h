/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_AvoidObstacleV24.h                               */
/*    DATE: Aug 2nd 2006                                         */
/*    DATE: Sep 9th 2019  Rewrite with different AOF & refinery  */
/*    DATE: Feb 27th 2021 Further mods related to completion.    */
/*    DATE: Feb 27th 2021 Created AvoidObstacleV21 version       */
/*    DATE: Aug 5th 2023 Created AvoidObstacleV24 version        */
/*    DATE: Aug 5th 2023 Use TurnModel, Oct23 renamed PlatModel  */
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
 
#ifndef BHV_AVOID_OBSTACLE_V24_HEADER
#define BHV_AVOID_OBSTACLE_V24_HEADER

#include "IvPBehavior.h"
#include "ObShipModelV24.h"
#include "XYPolygon.h"
#include "HintHolder.h"

class BHV_AvoidObstacleV24 : public IvPBehavior {
public:
  BHV_AvoidObstacleV24(IvPDomain);
  ~BHV_AvoidObstacleV24() {}
  
  bool         setParam(std::string, std::string);
  void         onHelmStart();
  IvPFunction* onRunState();
  void         onIdleState();
  void         onCompleteState() {postErasablePolygons();}
  void         onSetParamComplete();
  void         onIdleToRunState();
  void         onInactiveState()  {postErasablePolygons();}
  void         onEveryState(std::string);
  void         postConfigStatus();
  double       getDoubleInfo(std::string);
  bool         isConstraint() {return(true);}
  std::string  expandMacros(std::string);
  
 protected: 
  bool   handleParamRangeFlag(std::string);

  double getRelevance();
  bool   updatePlatformInfo();
  void   postViewablePolygons();
  void   postErasablePolygons();
  void   initVisualHints();
  
  bool   applyBuffer();
  IvPFunction* buildOF();
  
 protected:
  ObShipModelV24 m_obship_model;

 protected: // Config Params
  bool        m_use_refinery;
  std::string m_pwt_grade;

  std::string m_resolved_obstacle_var;
  std::string m_obstacle_id;

  std::vector<double>      m_rng_thresh;
  std::vector<VarDataPair> m_rng_flags;
  std::vector<VarDataPair> m_cpa_flags;

  bool m_draw_buff_min_poly;
  bool m_draw_buff_max_poly;
  
 protected: // State variables

  double  m_obstacle_relevance;
  bool    m_resolved_pending;

  bool    m_valid_cn_obs_info;
  
  bool    m_closing;
  double  m_cpa_rng_sofar;
  double  m_fpa_rng_sofar;
  double  m_cpa_rng_ever;
  double  m_cpa_reported;

  std::string m_side_lock;

  
protected:
  HintHolder m_hints;
 
};
#endif
