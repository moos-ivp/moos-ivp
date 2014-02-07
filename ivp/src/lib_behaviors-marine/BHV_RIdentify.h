/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_RIdentify.h                                      */
/*    DATE: October 3rd 2009                                     */
/*          (From BHV_Identify April 2009)                       */
/*****************************************************************/
 
#ifndef BHV_RIDENTIFY_HEADER
#define BHV_RIDENTIFY_HEADER

#include <string>
#include <list>
#include "IvPBehavior.h"
#include "WaypointEngine.h"
#include "XYPoint.h"
#include "PatternGenerator.h"

class BHV_RIdentify : public IvPBehavior {
public:
  BHV_RIdentify(IvPDomain);
  ~BHV_RIdentify();
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onIdleState();
  void         onSetParamComplete();

protected:
  bool         updateInfoIn();
  bool         setNextWaypoint();
  IvPFunction* buildOF(std::string);

  void         postWaypointStatusReport();
  void         postIDStatusReport();
  void         postViewableSegList();
  void         postErasableSegList();

  void         postViewableIDPoints(std::string s="all");
  void         postErasableIDPoints();
  void         postAwaitingPointsFlag();

  bool         handleNewIDPoints(std::string);
  bool         setNextRIPoint(std::string);
  void         adjustRIPattern();
  void         handleVisualHint(std::string);

 protected:
  std::string  setPatternParameters();

protected: 
  WaypointEngine    m_waypoint_engine;
  PatternGenerator *m_pattern_generator;

protected: // configuration parameters
  
  bool        m_greedy_traverse;
  bool        m_clustering;

  double      m_cruise_speed;
  double      m_lead_distance;
  double      m_lead_damper;
  std::string m_ipf_type;

  std::string m_var_suffix;
  std::string m_var_idstream;

  std::string m_hint_idpt_color;
  std::string m_hint_idpt_lcolor;
  std::string m_hint_segl_vertex_color;
  std::string m_hint_segl_edge_color;

  std::map<std::string, std::string> m_pattern_params;

protected: // intermediate or object global variables.
  std::list<XYPoint> m_done_pts;
  std::list<XYPoint> m_active_pts;
  std::list<XYPoint> m_todo_pts;
  XYPoint   m_curr_ript;
    
  double    m_osh;  // Ownship heading
  double    m_osv;  // Ownship velocity
  double    m_osx;  // Ownship x position
  double    m_osy;  // Ownship y position
  int       m_last_new_id;
  double    m_last_busy_time;  

  XYPoint   m_trackpt;
  XYPoint   m_nextpt;
  
  // The RI pattern may rotate on approach, but once started may no
  // longer rotate (m_patt_rotatable=false). Rotating at all may be
  // shut off (m_patt_rotatable_ever=false).
  bool      m_patt_rotatable;
  bool      m_patt_rotatable_ever;
  bool      m_transiting;
};

extern "C" {
  IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_RIdentify(domain);}
}

#endif

