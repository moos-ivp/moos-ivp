/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_OpRegion.h                                       */
/*    DATE: Aug 10th, 2016                                       */
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

extern "C" {
  IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_OpRegionRecover(domain);}
}


#endif

