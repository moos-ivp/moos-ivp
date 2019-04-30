/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CPAMonitor.h                                         */
/*    DATE: Dec 20th 2015                                        */
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

  double getClosestRange() const {return(m_closest_range);}
  
 protected: // Local utility functions
  std::string pairTag(std::string, std::string);
  bool        examineAndReport(std::string);
  bool        examineAndReport(std::string, std::string);
  bool        updatePairRangeAndRate(std::string, std::string); 

  double      relBng(std::string vname1, std::string vname2);
  
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

 protected: // Overall state

  double m_closest_range;
  unsigned int m_iteration;
};

#endif 




