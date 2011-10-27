/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IvPBehavior.h                                        */
/*    DATE: Oct 20, 2003 4 days after Grady's Gaffe              */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#ifndef IVP_BEHAVIOR_HEADER
#define IVP_BEHAVIOR_HEADER

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "IvPFunction.h"
#include "InfoBuffer.h"
#include "VarDataPair.h"
#include "LogicCondition.h"
#include "BehaviorReport.h"

class IvPBehavior {
friend class BehaviorSet;
public:
  IvPBehavior(IvPDomain);
  virtual ~IvPBehavior() {};

  virtual IvPFunction* onRunState() {return(0);};
  virtual BehaviorReport onRunState(std::string);
  virtual bool setParam(std::string, std::string);
  virtual void onSetParamComplete() {};
  virtual void onIdleState() {};
  virtual void onCompleteState() {};
  virtual void onIdleToRunState() {};
  virtual void onRunToIdleState() {};

  bool   setParamCommon(std::string, std::string);
  void   setInfoBuffer(const InfoBuffer*);
  bool   checkUpdates();
  std::string isRunnable();

  void   statusInfoAdd(std::string param, std::string value);
  void   statusInfoPost();

  std::vector<std::string> getInfoVars();
  std::string getDescriptor()            {return(m_descriptor);};
  std::string getBehaviorType()          {return(m_behavior_type);};
  std::string getUpdateSummary()         {return(m_update_summary);};
  std::vector<VarDataPair> getMessages() {return(m_messages);};
  int    getFilterLevel() const          {return(m_filter_level);};
  bool   stateOK() const                 {return(m_bhv_state_ok);};
  void   clearMessages()                 {m_messages.clear();};
  void   resetStateOK()                  {m_bhv_state_ok=true;};

protected:
  bool    setBehaviorName(std::string str);
  bool    augBehaviorName(std::string str);
  void    setBehaviorType(std::string str) {m_behavior_type = str;};

  void    addInfoVars(std::string, std::string="");
  void    setComplete();
  void    postMessage(std::string, double, std::string key="");
  void    postMessage(std::string, std::string, std::string key="");
  void    postBoolMessage(std::string, bool, std::string key="");
  void    postIntMessage(std::string, double, std::string key="");
  void    postRepeatableMessage(std::string, double);
  void    postRepeatableMessage(std::string, std::string);
  void    postEMessage(std::string);
  void    postWMessage(std::string);
  void    postFlags(const std::string&);

  void    postDurationStatus();
  bool    durationExceeded();
  void    durationReset();
  void    updateStateDurations(std::string);
  bool    checkConditions();
  bool    checkForDurationReset();
  bool    checkNoStarve();

  double                   getBufferCurrTime();
  double                   getBufferTimeVal(std::string);
  double                   getBufferDoubleVal(std::string, bool&);
  std::string              getBufferStringVal(std::string, bool&);
  std::vector<double>      getBufferDoubleVector(std::string, bool&);
  std::vector<std::string> getBufferStringVector(std::string, bool&);
  std::vector<std::string> getStateSpaceVars();

protected:
  const InfoBuffer* m_info_buffer;

  std::string m_us_name;       
  std::string m_descriptor;    
  std::string m_contact; // Name for contact in InfoBuffer
  std::string m_behavior_type;
  std::string m_duration_status;
  std::string m_build_info;
  std::string m_status_info;

  std::vector<std::string>       m_info_vars;
  std::vector<std::string>       m_info_vars_no_warning;

  std::vector<VarDataPair>       m_messages;
  std::vector<LogicCondition>    m_logic_conditions;
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

  // The state_ok flag shouldn't be set to true once it has been 
  // set to false. So prevent subclasses from setting this directly.
  // This variable should only be accessible via (1) postEMessage()
  // and resetStateOK().
private:
  bool        m_bhv_state_ok;
};

#endif

