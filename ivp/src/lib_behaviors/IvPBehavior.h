/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
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

#include <map>
#include <vector>
#include <string>
#include "IvPFunction.h"
#include "InfoBuffer.h"
#include "VarDataPair.h"
#include "LogicCondition.h"

class IvPBehavior {
friend class BehaviorSet;
public:
  IvPBehavior(IvPDomain);
  virtual ~IvPBehavior();

  virtual IvPFunction* produceOF() {return(0);};
  virtual bool setParam(std::string, std::string);
  virtual bool preCheck();
  virtual void onIdleState() {};

  bool   setParamCommon(std::string, std::string);
  void   setInfoBuffer(const InfoBuffer*);
  void   checkForUpdates();
  double getPriority()                   {return(priority_wt);};
  void   postRunFlags()                  {postFlags(run_flags);};
  void   postEndFlags()                  {postFlags(end_flags);};
  void   postIdleFlags()                 {postFlags(idle_flags);};

  std::vector<std::string> getInfoVars();
  std::vector<std::string> getConditionVars();
  std::string getDescriptor()            {return(descriptor);};
  std::vector<VarDataPair> getMessages() {return(messages);};
  void   clearMessages()                 {messages.clear();};
  bool   stateOK()                       {return(m_state_ok);};
  void   resetStateOK()                  {m_state_ok=true;};

protected:
  void    setComplete();
  void    postMessage(std::string, double);
  void    postMessage(std::string, std::string);
  void    postEMessage(std::string);
  void    postWMessage(std::string);
  void    postPCMessage(std::string);
  void    postFlags(const std::vector<VarDataPair>&);

  bool    durationExceeded();
  bool    checkConditions();
  bool    checkNoStarve();

protected:
  const InfoBuffer* info_buffer;

  std::string descriptor;    // ALL behaviors utilize
  std::string us_name;       // MOST behaviors utilize
  std::string duration_status;

  std::vector<std::string>           info_vars;
  std::vector<VarDataPair>           messages;
  std::vector<LogicCondition>        logic_conditions;
  std::vector<VarDataPair>           run_flags;
  std::vector<VarDataPair>           end_flags;
  std::vector<VarDataPair>           idle_flags;
  std::map<std::string, double>      starve_vars;
  
  IvPDomain   domain;        
  double      priority_wt;   // ALL behaviors utilize
  IvPBox*     unif_box;      // MOST behaviors utilize
  IvPBox*     grid_box;      // MOST behaviors utilize

  // Variables for providing behaviors w/ "udate" capability
  std::string m_update_var;
  std::string m_curr_update_str;
  int         m_good_updates;
  int         m_bad_updates;
  double      m_last_update_age;

  // variables for providing behaviors w/ "duration" capability
  double      m_duration;
  double      m_start_time;
  bool        m_started;
  bool        m_completed;
  bool        m_perpetual; 

  // The state_ok flag shouldn't be set to true once it has been 
  // set to false. So prevent subclasses from setting this directly.
private:
  bool        m_state_ok;
};

#endif










