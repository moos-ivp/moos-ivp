/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmReporter.h                                       */
/*    DATE: March 10th, 2010                                     */
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

#ifndef ALOG_HELM_REPORTER_HEADER
#define ALOG_HELM_REPORTER_HEADER

#include <vector>
#include <string>
#include <set>
#include "LifeEventHistory.h"
#include "HelmReport.h"

class HelmReporter
{
 public:
  HelmReporter();
  ~HelmReporter() {}

  bool handle(const std::string&);
  void printReport();
  
  void reportLifeEvents(bool v=true)      {m_report_life_events=v;}
  void reportBehaviorChanges(bool v=true) {m_report_bhv_changes=v;}
  void reportModeChanges(bool v=true)     {m_report_mode_changes=v;}
  void setWatchBehavior(std::string s)    {m_watch_behavior=s;}
  void setUseColor(bool v=true)           {m_use_color=v;}
  void setColorActive(bool v)             {m_life_events.setColorActive(v);}
  void setVarTrunc(bool v)                {m_var_trunc=v;}

  void addWatchVar(std::string);
  
 protected: // Utility Functions
  void handleNewHelmSummary(std::string, std::string);

 protected: // State Variables
  LifeEventHistory m_life_events;

  bool             m_no_prev_report;
  HelmReport       m_prev_report;

  std::string      m_prev_mode_value;
  std::string      m_mode_var;


 protected: // Configuration Variables
  bool             m_report_life_events;
  bool             m_report_mode_changes;
  bool             m_report_bhv_changes;
  bool             m_use_color;
  bool             m_var_trunc;

  std::string      m_watch_behavior;

  std::vector<std::string> m_watch_vars;

};

#endif









