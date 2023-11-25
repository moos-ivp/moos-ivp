/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: HelmReport.h                                         */
/*    DATE: Sep 26th, 2006                                       */
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

#ifndef HELM_REPORT_HEADER
#define HELM_REPORT_HEADER

#include <map>
#include <vector>
#include <list>
#include <deque>
#include <string>
#include "IvPDomain.h"

class HelmReport {
public:
  HelmReport();
  ~HelmReport() {}

  void  initialize();
  void  clear(bool clear_completed=false);
  void  addMsg(const std::string& str)     {m_messages.push_back(str);}
  void  setHaltMsg(const std::string& str) {m_halt_message = str;}

  void  setTemplatingSummary(const std::vector<std::string>&);
  void  clearActiveBHVs();
  void  addActiveBHV(const std::string& descriptor, double time,
		     double pwt, int pcs, double cpu,
		     const std::string& update_summary, 
		     unsigned int ipfs);

  void  clearRunningBHVs();
  void  addRunningBHV(const std::string& descriptor, double time,
		      const std::string& update_summary);

  void  clearIdleBHVs();
  void  addIdleBHV(const std::string& descriptor, double time,
		   const std::string& updated_summary);

  void  clearCompletedBHVs();
  void  addCompletedBHV(const std::string& descriptor, double time,
			const std::string& update_summary);

  void  setModeSummary(const std::string& s) {m_modes=s;}
  void  setIvPDomain(const IvPDomain &dom)   {m_domain = dom;}
  void  setTimeUTC(double v)                 {m_time_utc = v;}
  void  setWarningCount(unsigned int v)      {m_warning_count = v;}
  void  setHalted(bool v)                    {m_halted=v;}
  void  setActiveGoal(bool v)                {m_active_goal=v;}
  void  setIteration(unsigned int iter)      {m_iteration=iter;}
  void  setOFNUM(unsigned int ofnum)         {m_ofnum=ofnum;}
  void  setTotalPcsFormed(unsigned int v)    {m_total_pcs_formed=v;}
  void  setTotalPcsCached(unsigned int v)    {m_total_pcs_cached=v;}
  void  setCreateTime(double t)              {m_create_time=t;}
  void  setSolveTime(double t)               {m_solve_time=t;}
  void  setMaxLoopTime(double t)             {m_max_loop_time=t;}
  void  setMaxCreateTime(double t)           {m_max_create_time=t;}
  void  setMaxSolveTime(double t)            {m_max_solve_time=t;}

  void  clearDecisions();
  void  addDecision(const std::string &var, double val);

  // Getters
  unsigned int getWarnings()   const  {return(m_warning_count);}
  unsigned int getIteration()  const  {return(m_iteration);}
  unsigned int getOFNUM()      const  {return(m_ofnum);}
  unsigned int getTotalPcsFormed() const {return(m_total_pcs_formed);}
  unsigned int getTotalPcsCached() const {return(m_total_pcs_cached);}
  double       getTimeUTC()    const  {return(m_time_utc);}
  double       getCreateTime() const  {return(m_create_time);}
  double       getSolveTime()  const  {return(m_solve_time);}
  double       getLoopTime()   const  {return(m_solve_time+m_create_time);}
  bool         getHalted()     const  {return(m_halted);}
  bool         getActiveGoal() const  {return(m_active_goal);}
  double       getMaxLoopTime() const {return(m_max_loop_time);}
  double       getMaxSolveTime()  const {return(m_max_solve_time);}
  double       getMaxCreateTime() const {return(m_max_create_time);}

  double       getDecision(const std::string&) const;
  bool         hasDecision(const std::string&) const;

  bool         changedBehaviors(const HelmReport&) const;

  std::string  getModeSummary()      const {return(m_modes);}
  std::string  getHaltMsg()          const {return(m_halt_message);}
  std::vector<std::string> getMsgs() const {return(m_messages);}
  std::vector<std::string> getTemplatingSummary() const;

  std::vector<std::string> getUpdateResults() const {return(m_update_results);}

  void setUpdateResults(std::vector<std::string> v) {m_update_results=v;}
  
  // Serialization Helper Methods
  std::string  getDecisionSummary()    const;
  std::string  getActiveBehaviors(bool full=true)    const;
  std::string  getRunningBehaviors(bool full=true)   const;
  std::string  getIdleBehaviors(bool full=true)      const;
  std::string  getCompletedBehaviors(bool full=true) const;
  std::string  getDomainString()       const;
  std::string  timeInState(double, double) const;

  // Serialization Methods
  std::string  getReportAsString() const;
  std::string  getReportAsString(const HelmReport&, bool full=false) const;

  // Formatted Summary
  std::list<std::string> formattedSummary(double, bool=false) const;

  // Debugging
  void print() const;

protected:

  std::vector<std::string>  m_bhvs_running_desc;   // Running Behaviors
  std::vector<double>       m_bhvs_running_time;
  std::vector<std::string>  m_bhvs_running_upds;
  
  std::vector<std::string>  m_bhvs_idle_desc;      // Idle Behaviors
  std::vector<double>       m_bhvs_idle_time;
  std::vector<std::string>  m_bhvs_idle_upds;
  
  std::deque<std::string>   m_bhvs_completed_desc; // Completed Behaviors
  std::deque<double>        m_bhvs_completed_time;
  std::deque<std::string>   m_bhvs_completed_upds;
  
  std::vector<std::string>  m_bhvs_active_desc;    // Active Behaviors
  std::vector<double>       m_bhvs_active_time;
  std::vector<std::string>  m_bhvs_active_upds;
  std::vector<double>       m_bhvs_active_pwt;
  std::vector<double>       m_bhvs_active_cpu;
  std::vector<int>          m_bhvs_active_pcs;
  std::vector<unsigned int> m_bhvs_active_ipfs;

  std::vector<std::string>  m_update_results;
  
  std::vector<std::string>      m_templating_summary;
  std::vector<std::string>      m_messages;
  std::map<std::string, double> m_decisions;  // +

  std::string   m_halt_message;    // +
  std::string   m_modes;           // +
  
  unsigned int  m_warning_count;   // +
  double        m_time_utc;        // +
  unsigned int  m_iteration;       // +
  unsigned int  m_ofnum;           // +
  double        m_create_time;     // + 
  double        m_solve_time;      // + 
  bool          m_halted;          // +
  bool          m_active_goal;     // + nov1419

  unsigned int  m_total_pcs_formed;
  unsigned int  m_total_pcs_cached;
  
  double        m_max_create_time;
  double        m_max_solve_time;
  double        m_max_loop_time;

  IvPDomain     m_domain;          // referenced for varbalk info
};

#endif

