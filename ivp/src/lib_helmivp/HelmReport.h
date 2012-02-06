/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmReport.h                                         */
/*    DATE: Sep 26th, 2006                                       */
/*****************************************************************/

#ifndef HELM_REPORT_HEADER
#define HELM_REPORT_HEADER

#include <map>
#include <vector>
#include <string>
#include "IvPDomain.h"

class HelmReport {
public:
  HelmReport();

  void  addMsg(const std::string& str)     {m_messages.push_back(str);};
  void  setHaltMsg(const std::string& str) {m_halt_message = str;};
  
  void  addActiveBHV(const std::string& descriptor, double time,
		     double pwt, double pcs, double cpu,
		     const std::string& update_summary, 
		     unsigned int ipfs, bool orig=true);
  void  addRunningBHV(const std::string& descriptor, double time,
		      const std::string& update_summary);
  void  addIdleBHV(const std::string& descriptor, double time,
		   const std::string& updated_summary);
  void  addCompletedBHV(const std::string& descriptor, double time,
			const std::string& update_summary);

  void  setActiveBHVs(const std::string& s)    {m_active_bhvs=s;};
  void  setRunningBHVs(const std::string& s)   {m_running_bhvs=s;};
  void  setIdleBHVs(const std::string& s)      {m_idle_bhvs=s;};
  void  setCompletedBHVs(const std::string& s) {m_completed_bhvs=s;};


  void  setModeSummary(const std::string& s) {m_modes=s;};
  void  setIvPDomain(const IvPDomain &dom)   {m_domain = dom;};
  void  setTimeUTC(double v)                 {m_time_utc = v;};
  void  setWarningCount(unsigned int v)      {m_warning_count = v;};
  void  setHalted(bool v)                    {m_halted=v;};
  void  setIteration(unsigned int iter)      {m_iteration=iter;};
  void  setOFNUM(unsigned int ofnum)         {m_ofnum=ofnum;};
  void  setCreateTime(double t)              {m_create_time=t;};
  void  setSolveTime(double t)               {m_solve_time=t;};

  void  clearDecisions();
  void  addDecision(const std::string &var, double val);

  // Getters
  unsigned int getWarnings() const   {return(m_warning_count);};
  unsigned int getIteration() const  {return(m_iteration);};
  unsigned int getOFNUM() const      {return(m_ofnum);};
  double       getTimeUTC() const    {return(m_time_utc);};
  double       getCreateTime() const {return(m_create_time);};
  double       getSolveTime() const  {return(m_solve_time);};
  double       getLoopTime() const   {return(m_solve_time+m_create_time);};
  bool         getHalted() const     {return(m_halted);};
  double       getDecision(const std::string &var);
  bool         hasDecision(const std::string &var);

  std::string  getReportAsString();
  std::string  getReportAsString(const HelmReport&);

  std::string  getModeSummary() const        {return(m_modes);};
  std::string  getHaltMsg() const            {return(m_halt_message);};
  std::string  getActiveBehaviors() const    {return(m_active_bhvs);};;
  std::string  getRunningBehaviors() const   {return(m_running_bhvs);};
  std::string  getIdleBehaviors() const      {return(m_idle_bhvs);};
  std::string  getCompletedBehaviors() const {return(m_completed_bhvs);};
  std::string  getDecisionSummary() const    {return(m_decision_summary);};

  std::vector<std::string> getMsgs() const   {return(m_messages);};

  void print() const;

protected:
  std::vector<std::string>      m_messages;
  std::map<std::string, double> m_decisions;  // +

  std::string   m_halt_message;    // +
  std::string   m_active_bhvs;     // +
  std::string   m_running_bhvs;    // +
  std::string   m_completed_bhvs;  // +
  std::string   m_idle_bhvs;       // +
  std::string   m_modes;           // +
  
  // A decision summary for easy comparing between successive reports
  std::string   m_decision_summary; 

  unsigned int  m_warning_count;   // +
  double        m_time_utc;        // +
  unsigned int  m_iteration;       // +
  unsigned int  m_ofnum;           // +
  double        m_create_time;     // + 
  double        m_solve_time;      // + 
  bool          m_halted;          // +

  IvPDomain     m_domain;          // referenced for varbalk info
};

#endif

