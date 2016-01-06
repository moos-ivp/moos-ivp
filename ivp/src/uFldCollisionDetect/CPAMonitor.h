/************************************************************/
/*    NAME: Mike Benjamin, based on an earlier version by   */
/*          Kyle Woerner                                    */
/*    ORGN: MIT CSAIL, Dept of Mechanical Engineering       */
/*    FILE: CPAMonitor.h                                    */
/*    DATE: Dec 20th 2015                                   */
/************************************************************/

#ifndef CPA_MONITOR_HEADER
#define CPA_MONITOR_HEADER

#include <string>
#include <map>
#include <list>
#include "NodeRecord.h"
#include "CPAEvent.h"

class CPAMonitor
{
 public:
  CPAMonitor();
  ~CPAMonitor() {};

  bool handleNodeReport(std::string);
  bool examineAndReport();
  void clear(); 
  void setIgnoreRange(double);
  void setReportRange(double);
  void setSwingRange(double);

  void setIteration(unsigned int v) {m_iteration = v;}
  
  unsigned int getEventCount() const {return(m_events.size());}
  CPAEvent     getEvent(unsigned int) const;

 protected: // Local utility functions
  std::string pairTag(std::string, std::string);
  bool        examineAndReport(std::string);
  bool        examineAndReport(std::string, std::string);
  bool        updatePairRangeAndRate(std::string, std::string); 
  
 protected: // Configuration parameters
  double  m_ignore_range;
  double  m_report_range;
  double  m_swing_range;
  bool    m_verbose;
  
 protected: // map keyed on vname
  std::map<std::string, std::list<NodeRecord> > m_map_vrecords;
  std::map<std::string, bool>                   m_map_updated;
  
 protected: // map keyed on coupled vname#contact, abe#bob
  std::map<std::string, double>     m_map_pair_dist;
  std::map<std::string, double>     m_map_pair_min_dist_running;
  std::map<std::string, double>     m_map_pair_max_dist_running;
  std::map<std::string, double>     m_map_pair_midx;
  std::map<std::string, double>     m_map_pair_midy;
  std::map<std::string, bool>       m_map_pair_closing;
  std::map<std::string, bool>       m_map_pair_valid;
  std::map<std::string, bool>       m_map_pair_examined;
  
 protected: // Indexed on event (cpa occurrence)
  std::vector<CPAEvent>    m_events;

  unsigned int m_iteration;
};

#endif 
