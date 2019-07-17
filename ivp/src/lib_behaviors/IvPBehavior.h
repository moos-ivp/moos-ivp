/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IvPBehavior.h                                        */
/*    DATE: Oct 20, 2003 4 days after Grady's Gaffe              */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef IVP_BEHAVIOR_HEADER
#define IVP_BEHAVIOR_HEADER

#include <map>
#include <vector>
#include <string>
#include "IvPFunction.h"
#include "InfoBuffer.h"
#include "CPAEngine.h"
#include "VarDataPair.h"
#include "LogicCondition.h"
#include "BehaviorReport.h"

using namespace std;

class IvPBehavior {
friend class BehaviorSet;
public:
  IvPBehavior(IvPDomain);
  virtual ~IvPBehavior() {}

  virtual bool onRunStatePrior() {return(true);}
  virtual IvPFunction* onRunState() {return(0);}

  virtual BehaviorReport onRunState(std::string);
  virtual void setCPAEngine(const CPAEngine&) {};
  virtual bool setParam(std::string, std::string);
  virtual void onSetParamComplete() {postConfigStatus();}
  virtual void onHelmStart() {}
  virtual void onSpawn() {}
  virtual void onIdleState() {}
  virtual void onCompleteState() {}
  virtual void onInactiveState() {}
  virtual void onIdleToRunState() {}
  virtual void onRunToIdleState() {}
  virtual void postConfigStatus() {}
  virtual std::string getInfo(std::string)  {return("");}
  virtual double getDoubleInfo(std::string) {return(0);}
  virtual double getMemSize() {return(0);}
  
  bool   setParamCommon(std::string, std::string);
  void   setInfoBuffer(const InfoBuffer*);
  bool   checkUpdates();
  std::string isRunnable();

  void   statusInfoAdd(std::string param, std::string value);
  void   statusInfoPost();

  std::vector<std::string> getInfoVars();
  std::string getDescriptor()            {return(m_descriptor);}
  std::string getUpdateVar() const       {return(m_update_var);}
  std::string getBehaviorType()          {return(m_behavior_type);}
  std::string getUpdateSummary()         {return(m_update_summary);}
  std::vector<VarDataPair> getMessages() {return(m_messages);}
  int    getFilterLevel() const          {return(m_filter_level);}
  bool   stateOK() const                 {return(m_bhv_state_ok);}
  void   clearMessages()                 {m_messages.clear();}
  void   resetStateOK()                  {m_bhv_state_ok=true;}

  void    postMessage(std::string, std::string, std::string key="");
  void    noteLastRunCheck(bool, double);

 protected:
  bool    setBehaviorName(std::string str);
  bool    augBehaviorName(std::string str);
  void    setBehaviorType(std::string str) {m_behavior_type = str;}
  void    setPriorityWt(double);

  void    addInfoVars(std::string, std::string="");
  void    setComplete();
  void    postBadConfig(std::string);
  void    postMessage(std::string, double, std::string key="");

  void    postBoolMessage(std::string, bool, std::string key="");
  void    postIntMessage(std::string, double, std::string key="");
  void    postRepeatableMessage(std::string, double);
  void    postRepeatableMessage(std::string, std::string);
  void    postEMessage(std::string);
  void    postWMessage(std::string);
  void    postRetractWMessage(std::string);
  void    postFlags(const std::string&, bool repeat=false);

  void    postDurationStatus();
  bool    durationExceeded();
  void    durationReset();
  void    updateStateDurations(std::string);
  bool    checkConditions();
  bool    checkForDurationReset();
  bool    checkNoStarve();

  void    setHelmIteration(unsigned int iter) {m_helm_iter=iter;}
  
  double                   getPriorityWt() {return(m_priority_wt);}
  double                   getBufferCurrTime();
  double                   getBufferLocalTime();
  double                   getBufferMsgTimeVal(std::string);
  double                   getBufferTimeVal(std::string);
  double                   getBufferDoubleVal(std::string, bool&);
  std::string              getBufferStringVal(std::string);
  std::string              getBufferStringVal(std::string, bool&);
  std::vector<double>      getBufferDoubleVector(std::string, bool&);
  std::vector<std::string> getBufferStringVector(std::string, bool&);
  std::vector<std::string> getStateSpaceVars();

  std::vector<std::string> getUpdateResults() const {return(m_update_results);}
  
protected:
  const InfoBuffer* m_info_buffer;

  std::string m_us_name;       
  std::string m_descriptor;    
  std::string m_contact; // Name for contact in InfoBuffer
  std::string m_behavior_type;
  std::string m_duration_status;
  bool        m_duration_reset_pending;
  std::string m_build_info;
  std::string m_status_info;

  unsigned int m_helm_iter;
  
  std::vector<std::string>       m_info_vars;
  std::vector<std::string>       m_info_vars_no_warning;

  std::vector<VarDataPair>       m_messages;
  std::vector<LogicCondition>    m_logic_conditions;
  std::vector<VarDataPair>       m_spawn_flags;
  std::vector<VarDataPair>       m_run_flags;
  std::vector<VarDataPair>       m_active_flags;
  std::vector<VarDataPair>       m_inactive_flags;
  std::vector<VarDataPair>       m_end_flags;
  std::vector<VarDataPair>       m_idle_flags;
  std::map<std::string, double>  m_starve_vars;
  std::map<std::string, std::string> m_remap_vars;
 
  IvPDomain  m_domain;        
  double     m_priority_wt; 

  // Variables for providing behaviors w/ "update" capability
  std::string  m_update_var;
  std::string  m_prev_update_str;
  unsigned int m_good_updates;
  unsigned int m_bad_updates;
  std::string  m_update_summary;
  std::vector<std::string> m_update_results;

  // variables for providing behaviors w/ "duration" capability
  double      m_duration;
  double      m_duration_start_time;
  bool        m_duration_started;
  std::string m_duration_reset_var;
  std::string m_duration_reset_val;
  std::string m_duration_prev_state;
  double      m_duration_reset_timestamp;
  bool        m_duration_reset_on_transition;
  bool        m_duration_idle_decay;

  double      m_duration_running_time;
  double      m_duration_idle_time;
  double      m_duration_prev_timestamp;

  bool        m_completed;
  bool        m_perpetual; 
  int         m_filter_level;

  bool        m_last_runcheck_post;
  double      m_last_runcheck_time;
  
  // The state_ok flag shouldn't be set to true once it has been 
  // set to false. So prevent subclasses from setting this directly.
  // This variable should only be accessible via (1) postEMessage()
  // and resetStateOK().
private:
  bool        m_bhv_state_ok;
};

#endif




