/**************************************************************/
/*    NAME: Michael Benjamin                                  */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA  */
/*    FILE: EvalLoiter.h                                      */
/*    DATE: January 6th, 2016                                 */
/**************************************************************/

#ifndef EVAL_LOITER_HEADER
#define EVAL_LOITER_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "XYPolygon.h"
#include "NodeRecord.h"
#include "LogicCondition.h"
#include "InfoBuffer.h"

class EvalLoiter : public AppCastingMOOSApp
{
 public:
  EvalLoiter();
  ~EvalLoiter() {};
  
 protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
 protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();
  
 protected:
  void   registerVariables();
  bool   handleMailUpdatePoly(std::string);
  bool   handleMailEvalRequest(std::string);
  bool   handleMailNodeReport(std::string);
  bool   handleMailLoiterSettings(std::string);
  
  void   checkLoiterMode();
  void   performPointEval();
  void   processEvalRequests();
  
  void   addPointEval(double);
  void   clearEvals();
  double getDistToPolyAtTime(double&) const;
  
  double getEvalSummary() const;
  
 protected: // Functions in support of logic conditions
  bool   updateInfoBuffer(CMOOSMsg&);
  bool   checkConditions();
  
 private: // Configuration variables
  double      m_eval_window;
  double      m_nav_stale_thresh;
  double      m_mode_thresh;
  double      m_max_nav_age;
  double      m_range_max_ineff;

  double      m_notable_cpa_thresh;
  double      m_notable_eff_thresh;
  std::string m_notable_cpa_var;
  std::string m_notable_eff_var;
  
 private: // State variables
  XYPolygon   m_loiter_poly;
  std::string m_loiter_mode;
  double      m_transit_speed;
  double      m_curr_dist_to_poly;
  bool        m_conditions_ok;
  
  unsigned int m_cnt_bhv_settings;
  unsigned int m_cnt_eval_requests;
  unsigned int m_cnt_evals_posted;
  unsigned int m_cnt_node_reports;
  unsigned int m_cnt_mode_changes;
  
  // map of pending evaluation requests id:timestamp
  std::map<std::string, double> m_map_eval_requests;
  
  // List for storing node record history
  std::list<NodeRecord> m_records;
  
  // Lists for storing evaluation history
  std::list<double> m_point_evals;
  std::list<double> m_point_times;

  // Set of logic conditions determining when evaluation is active
  std::vector<LogicCondition> m_logic_conditions;
  InfoBuffer *m_info_buffer;
};

#endif 
