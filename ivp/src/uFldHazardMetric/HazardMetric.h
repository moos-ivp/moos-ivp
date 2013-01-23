/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardMetric.h                                       */
/*    DATE: March 12th, 2012                                     */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifndef HAZARD_METRIC_HEADER
#define HAZARD_METRIC_HEADER

#include <string>
#include <vector>
#include <map>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "XYHazard.h"
#include "XYHazardSet.h"
#include "XYHazardRepEval.h"

class HazardMetric : public AppCastingMOOSApp
{
 public:
  HazardMetric();
  ~HazardMetric() {};
  
 public: // Standard MOOSApp functions to overload
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload
  bool buildReport();

 protected: 
  void    registerVariables();
  bool    processHazardFile(std::string filename);
  bool    addHazardReport(std::string report);
  void    evaluateReport(const XYHazardSet&);

 private: // Configuration variables for grading reports.
  double  m_penalty_false_alarm;
  double  m_penalty_missed_hazard;
  double  m_max_time;
  double  m_penalty_max_time_over;
  double  m_penalty_max_time_rate;

 private: // State variables
  
  // Ground Truth
  std::string  m_hazard_file;
  XYHazardSet  m_hazards;

  double       m_search_start_time;
  double       m_elapsed_time;
  unsigned int m_total_reports_received;

  double       m_worst_possible_score;

  // Collection of Incoming reports/stats. Each key is vehicle name
  std::map<std::string, XYHazardSet>     m_map_reports;
  std::map<std::string, XYHazardRepEval> m_map_evals;
  std::map<std::string, unsigned int>    m_map_report_amt;

  // Most recent source of reported information
  std::string m_most_recent_source;
};

#endif 

