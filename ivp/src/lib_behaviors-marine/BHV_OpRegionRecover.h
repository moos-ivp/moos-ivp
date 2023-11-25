/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_OpRegion.h                                       */
/*    DATE: Aug 10th, 2016                                       */
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
 
#ifndef BHV_OPREGION_RECOVER_HEADER
#define BHV_OPREGION_RECOVER_HEADER

#include "XYPolygon.h"
#include "IvPBehavior.h"

class BHV_OpRegionRecover : public IvPBehavior {
 public:
  BHV_OpRegionRecover(IvPDomain);
  ~BHV_OpRegionRecover() {}
  
  bool         setParam(std::string, std::string);
  void         onSetParamComplete();

  IvPFunction* onRunState();
  void         onIdleState();
  void         onCompleteState() {postErasablePolygon();}

 protected:
  bool        polygonVerify();
  void        postPolyStatus();
  void        checkForReset();
  void        handleVisualHint(std::string);
  void        postViewablePolygon();
  void        postErasablePolygon();
  bool        updateInfoIn();

  void        updateRangeCache();
  std::string determineInitialTurn();

 protected: // Configuration Variables
  XYPolygon m_polygon_core;
  XYPolygon m_polygon_buff;
  double    m_trigger_entry_time;
  double    m_trigger_exit_time;
  bool      m_trigger_on_poly_entry;

  double    m_buffer_dist;
  
  // Allow for possible reset once poly has been breached
  std::string m_reset_var;

  // Allow for possible posting of the opregion polygon
  std::string m_opregion_poly_var;

  std::string m_dynamic_region_var;
  
  // Visual hints affecting properties of polygons/points
  std::string m_hint_vertex_color;
  std::string m_hint_edge_color;
  double      m_hint_vertex_size;
  double      m_hint_edge_size;
  std::string m_hint_label_color;


 protected: // State Variables
  bool    m_poly_entry_made;

  double  m_time_entered_poly;
  double  m_time_exited_poly;

  double  m_secs_in_poly;
  double  m_secs_out_poly;

  bool    m_previously_in_poly;

  double  m_osx; 
  double  m_osy; 
  double  m_osv; 
  double  m_osh;   

  std::string m_committed_turn;
  bool        m_heading_to_poly;

  std::vector<double> m_heading_cache;
  std::vector<double> m_range_cache;
  std::vector<double> m_range_cache_norm;
  std::vector<double> m_range_cache_norm_no_lft;
  std::vector<double> m_range_cache_norm_no_rgt;
};

#endif


