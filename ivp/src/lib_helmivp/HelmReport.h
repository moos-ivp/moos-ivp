/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HelmReport.h                                         */
/*    DATE: Sep 26th, 2006                                       */
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

#ifndef HELM_REPORT_HEADER
#define HELM_REPORT_HEADER

#include <vector>
#include <string>
#include "IvPDomain.h"

class HelmReport {
  friend class HelmEngine;
public:
  HelmReport();

  void addMsg(const std::string& str) 
    {m_messages.push_back(str);};
  
  void addActiveBHV(const std::string& descriptor, 
		    double time, double pwt, int pcs, double cpu,
		    const std::string& update_summary);
  void addRunningBHV(const std::string& descriptor, double time,
		     const std::string& update_summary);
  void addIdleBHV(const std::string& descriptor, double time,
		  const std::string& updated_summary);
  void addCompletedBHV(const std::string& descriptor, double time,
		       const std::string& update_summary);
  
  void setIvPDomain(const IvPDomain &dom) {m_domain = dom;};

  void setTimeUTC(double v)   {m_time_utc = v;};
  void setWarningCount(int v) {m_warning_count = v;};
  
  std::vector<std::string> getMsgs() const  
    {return(m_messages);};
  
  std::string getRunningBehaviors();
  std::string getActiveBehaviors();

  void   addDecision(const std::string &var, double val);
  double getDecision(const std::string &var);
  bool   hasDecision(const std::string &var);

  int    getIteration() const      {return(m_iteration);};
  int    getOFNUM() const          {return(m_ofnum);};
  double getCreateTime() const     {return(m_create_time);};
  double getSolveTime() const      {return(m_solve_time);};
  double getLoopTime() const       {return(m_loop_time);};
  bool   getHalted() const         {return(m_halted);};

  std::string getReportAsString();

protected:
  std::vector<std::string>  m_messages;
  std::string               m_running_bhvs;
  std::string               m_active_bhvs;
  std::string               m_all_bhvs;
  std::string               m_completed_bhvs;
  std::string               m_idle_bhvs;

  int                       m_warning_count;
  double                    m_time_utc;
  int                       m_iteration;
  int                       m_ofnum;
  double                    m_create_time;
  double                    m_solve_time;
  double                    m_loop_time;
  bool                      m_halted;
  std::vector<std::string>  m_decision_var;
  std::vector<double>       m_decision_val;

  IvPDomain                 m_domain;
};

#endif
