/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_OpRegionV26.h                                    */
/*    DATE: Nov 24th, 2023                                       */
/*    DATE: Aug 4th, 2026 V26                                    */
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
 
#ifndef BHV_OPREGION_V26_HEADER
#define BHV_OPREGION_V26_HEADER

#include "XYGenPolygon.h"
#include "XYPolygon.h"
#include "IvPBehavior.h"
#include "VarDataPair.h"
#include "HintHolder.h"

class BHV_OpRegionV26 : public IvPBehavior {
 public:
  BHV_OpRegionV26(IvPDomain);
  ~BHV_OpRegionV26() {}
  
  bool   setParam(std::string, std::string);
  void   onSetParamComplete();
  void   onRunToIdleState() {postErasablePolys();}
  void   onIdleToRunState() {postViewablePolys();}

  IvPFunction* onRunState();

 protected:
  void   reInitStateVars();
  bool   handleConfigReset(std::string);

  void   updateState();
  void   handleStateLapsed();
  void   handleStateBreached();
  void   handleStateContained();
  void   handleStateEntering();
  
  bool   polygonVerify();
  void   postViewablePolys();
  void   postErasablePolys();
  void   postViewableRegion();
  void   postErasableRegion();
  bool   updateInfoIn();

  bool   updateRegionPolys();
  void   updateRangeCache();

  std::string  expandMacros(std::string);
  std::string  determineInitialTurn();
  IvPFunction* buildOF();

  bool   setCorePolyBoundary(std::string);
  
 protected: // Config Vars (region)
  XYGenPolygon m_core_poly;
  double       m_lapse_dist;
  double       m_breach_dist;

  double      m_trigger_entry_time;
  double      m_trigger_breach_time;
  bool        m_trigger_on_poly_entry;

  std::string m_dynamic_region_var;  

  double m_recover_spd;
  
  // Allow for flags to be depending on state, prev_state
  std::vector<VarDataPair>  m_breached_flags;
  std::vector<VarDataPair>  m_lapsed_flags;
  std::vector<VarDataPair>  m_lapsedx_flags;
  std::vector<VarDataPair>  m_contained_flags;
  std::vector<VarDataPair>  m_containedx_flags;
  std::vector<VarDataPair>  m_entering_flags;
  std::vector<VarDataPair>  m_enteringx_flags;
  
  HintHolder m_hints;  // Visual hints 
  
 protected: // State Variables
  std::string m_state;
  std::string m_prev_state;
  bool        m_contained_ever;

  double      m_time_contained_start;
  double      m_time_contained_total;
  double      m_time_lapsed_start;
  double      m_time_lapsed_total;
  double      m_time_breached_start;
  double      m_time_breached_total;

  double      m_visuals_posted_utc;
  
 protected: // State Vars for recovering
  
  bool        m_heading_to_poly;
  std::string m_committed_turn;
  std::vector<double> m_heading_cache;
  std::vector<double> m_range_cache;
  std::vector<double> m_range_cache_norm;
  std::vector<double> m_range_cache_norm_no_lft;
  std::vector<double> m_range_cache_norm_no_rgt;
};

#endif

