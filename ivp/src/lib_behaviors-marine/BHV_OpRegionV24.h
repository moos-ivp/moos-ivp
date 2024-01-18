/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_OpRegionV24.h                                    */
/*    DATE: Nov 24th, 2023                                       */
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
 
#ifndef BHV_OPREGION_V24_HEADER
#define BHV_OPREGION_V24_HEADER

#include "XYPolygon.h"
#include "IvPBehavior.h"
#include "VarDataPair.h"
#include "HintHolder.h"

class BHV_OpRegionV24 : public IvPBehavior {
 public:
  BHV_OpRegionV24(IvPDomain);
  ~BHV_OpRegionV24() {}
  
  bool   setParam(std::string, std::string);
  void   onSetParamComplete();
  IvPFunction* onRunState();
  void   onRunToIdleState() {postErasablePolys();}
  void   onIdleToRunState() {postViewablePolys();}

 protected:
  bool   handleConfigReset(std::string);

  bool   depthVerify();
  bool   altitudeVerify();
  bool   timeoutVerify();
  bool   haltPolyVerify();

  bool   polygonVerify();
  void   postViewablePolys();
  void   postErasablePolys();
  bool   updateInfoIn();

  bool   updateRegionPolys();
  void   updateRangeCache();

  std::string  expandMacros(std::string);
  std::string  determineInitialTurn();
  IvPFunction* buildOF();
  
 protected: // Config Vars (region)
  double    m_max_depth;
  double    m_min_alt;
  double    m_max_time;

  XYPolygon m_core_poly;
  XYPolygon m_save_poly;
  XYPolygon m_halt_poly;

  double    m_save_dist;
  double    m_halt_dist;

  bool      m_draw_save_status;
  bool      m_draw_halt_status;
  
  double    m_trigger_entry_time;
  double    m_trigger_exit_time;
  bool      m_trigger_on_poly_entry;

  // Allow for flags to be posted when/if a breach is made
  std::vector<VarDataPair>  m_breached_poly_flags;
  std::vector<VarDataPair>  m_breached_time_flags;
  std::vector<VarDataPair>  m_breached_altitude_flags;
  std::vector<VarDataPair>  m_breached_depth_flags;
  std::vector<VarDataPair>  m_save_flags;
  std::vector<VarDataPair>  m_savex_flags;

  bool m_save_flags_posted;
  
 protected: // State Variables
  bool   m_halt_poly_entry_made;
  double m_secs_in_halt_poly;
  double m_secs_out_halt_poly;
  double m_time_entered_halt_poly;
  double m_time_exited_halt_poly;

  bool   m_save_poly_entry_made;
  double m_secs_out_save_poly;
  double m_time_exited_save_poly;
  
  double m_start_time; // For max_time determinations

  HintHolder m_hints;  // Visual hints 
  
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

