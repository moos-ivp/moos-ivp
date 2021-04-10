/****************************************************************/
/*   NAME: Mike Benjamin                                        */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: LCheckSet.h                                          */
/*   DATE: Oct 5th, 2020                                        */
/****************************************************************/

#ifndef LCHECK_SET_HEADER
#define LCHECK_SET_HEADER

#include <vector>
#include "VarDataPair.h"
#include "LogicBuffer.h"

class LCheckSet 
{
 public:
  LCheckSet();
  ~LCheckSet() {};

  bool addLeadCondition(std::string);
  bool addPassCondition(std::string);
  bool addFailCondition(std::string);

  void update(double curr_time=0);

  std::string checkConfig();
  std::string getStatus() const {return(m_lcheck_status);}

  bool enabled() const;
  bool isEvaluated() const;
  bool isSatisfied() const;

  void handleMail(std::string, std::string);
  void handleMail(std::string, double);

  std::set<std::string> getVars() const {return(m_logic_vars);}

  std::set<std::string> getInfoBufferReport() const;
  
  std::list<std::string> getReport() const;

private: 
  LogicBuffer m_lead_cbuff;
  LogicBuffer m_pass_cbuff;
  LogicBuffer m_fail_cbuff;
  std::set<std::string> m_logic_vars;
  
  bool m_lead_conditions_met;
  bool m_eval_conditions_evaluated;
  bool m_eval_conditions_met;

  std::string m_lcheck_status;
};

#endif 
 
