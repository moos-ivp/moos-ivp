/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmReport.h                                         */
/*    DATE: Sep 26th, 2006                                       */
/*                                                               */
/* (IvPHelm) The IvP autonomous control Helm is a set of         */
/* classes and algorithms for a behavior-based autonomous        */
/* control architecture with IvP action selection.               */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
/*****************************************************************/

#ifndef HELM_REPORT_HEADER
#define HELM_REPORT_HEADER

#include <map>
#include <vector>
#include <string>
#include "IvPDomain.h"

class HelmReport {
  friend class HelmEngine;
public:
  HelmReport();

  void addMsg(const std::string& str) 
    {m_messages.push_back(str);};
  
  void setHaltMsg(const std::string& str) 
    {m_halt_message = str;};
  
  void addActiveBHV(const std::string& descriptor, double time,
		    double pwt, double pcs, double cpu,
		    const std::string& update_summary, 
		    unsigned int ipfs, bool orig=true);
  void addRunningBHV(const std::string& descriptor, double time,
		     const std::string& update_summary);
  void addIdleBHV(const std::string& descriptor, double time,
		  const std::string& updated_summary);
  void addCompletedBHV(const std::string& descriptor, double time,
		       const std::string& update_summary);
  void setModeSummary(const std::string& s) {m_modes=s;};

  void setIvPDomain(const IvPDomain &dom) {m_domain = dom;};

  void setTimeUTC(double v)   {m_time_utc = v;};
  void setWarningCount(int v) {m_warning_count = v;};
  
  std::vector<std::string> getMsgs() const  
    {return(m_messages);};
  
  std::string getHaltMsg() const {return(m_halt_message);};
  
  std::string getRunningBehaviors() const;
  std::string getActiveBehaviors() const;
  std::string getIdleBehaviors() const;
  std::string getCompletedBehaviors() const;

  std::string getDecisionSummary() const {return(m_decision_summary);};
  std::string getModeSummary() const {return(m_modes);};

  void   addDecision(const std::string &var, double val);
  void   setDecision(const std::string &var, double val);
  double getDecision(const std::string &var);
  bool   hasDecision(const std::string &var);
  void   delDecision(const std::string &var);

  unsigned int getIteration() const {return(m_iteration);};
  unsigned int getOFNUM() const     {return(m_ofnum);};
  unsigned int getWarnings() const  {return(m_warning_count);};

  double getCreateTime() const     {return(m_create_time);};
  double getSolveTime() const      {return(m_solve_time);};
  double getLoopTime() const       {return(m_loop_time);};
  bool   getHalted() const         {return(m_halted);};

  std::string getReportAsString();
  std::string getReportAsString(const HelmReport&);

protected:
  std::vector<std::string>  m_messages;
  std::string               m_halt_message;
  std::string               m_running_bhvs;
  std::string               m_active_bhvs;
  std::string               m_all_bhvs;
  std::string               m_completed_bhvs;
  std::string               m_idle_bhvs;
  std::string               m_modes;

  std::string               m_decision_summary;

  unsigned int              m_warning_count;
  double                    m_time_utc;
  unsigned int              m_iteration;
  unsigned int              m_ofnum;
  double                    m_create_time;
  double                    m_solve_time;
  double                    m_loop_time;
  bool                      m_halted;

  std::map<std::string, double> m_decisions;

  IvPDomain                 m_domain;
};

#endif

