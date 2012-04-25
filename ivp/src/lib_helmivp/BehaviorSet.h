/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BehaviorSet.h                                        */
/*    DATE: Oct 27th 2004                                        */
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

#ifndef BEHAVIOR_SET_HEADER
#define BEHAVIOR_SET_HEADER

#include <string>
#include <vector>
#include <set>
#include "IvPBehavior.h"
#include "IvPDomain.h"
#include "VarDataPair.h"
#include "ModeSet.h"
#include "BehaviorSpec.h"
#include "BehaviorReport.h"
#include "SpecBuild.h"
#include "BFactoryStatic.h"
#include "BFactoryDynamic.h"
#include "BehaviorSetEntry.h"
#include "LifeEvent.h"

class IvPFunction;
class BehaviorSet
{
public:
  BehaviorSet();
  virtual ~BehaviorSet();
  
  void       addBehaviorDir(std::string);
  void       addBehaviorSpec(BehaviorSpec spec);  
  void       setDomain(IvPDomain domain);
  void       connectInfoBuffer(InfoBuffer*);
  bool       buildBehaviorsFromSpecs();
  SpecBuild  buildBehaviorFromSpec(BehaviorSpec spec, std::string s="");
  bool       handlePossibleSpawnings();

  void   addBehavior(IvPBehavior *b);
  void   clearBehaviors();
  void   addInitialVar(VarDataPair msg) {m_initial_vars.push_back(msg);};
  void   addDefaultVar(VarDataPair msg) {m_default_vars.push_back(msg);};
  void   setCompletedPending(bool v)    {m_completed_pending = v;};
  void   setCurrTime(double v)          {m_curr_time = v;};
  double getCurrTime()                  {return(m_curr_time);};
  void   setModeSet(ModeSet v)          {m_mode_set = v;};

  unsigned int getTCount()              {return(m_total_behaviors_ever);};

  unsigned int size()                   {return(m_bhv_entry.size());};

  void         setReportIPF(bool v)     {m_report_ipf=v;};
  bool         stateOK(unsigned int);
  void         resetStateOK();
  IvPFunction* produceOF(unsigned int ix, unsigned int iter, 
			 std::string& activity_state);

  BehaviorReport produceOFX(unsigned int ix, unsigned int iter, 
			    std::string& activity_state);
  
  unsigned int   removeCompletedBehaviors();
  IvPBehavior*   getBehavior(unsigned int);
  std::string    getDescriptor(unsigned int);
  std::string    getUpdateSummary(unsigned int);
  double         getStateElapsed(unsigned int);
  double         getStateTimeEntered(unsigned int);
  int            getFilterLevel(unsigned int);
  bool           filterBehaviorsPresent();

  std::vector<VarDataPair> getMessages(unsigned int);
  std::vector<VarDataPair> getInitialVariables() {return(m_initial_vars);};
  std::vector<VarDataPair> getDefaultVariables() {return(m_default_vars);};
  std::vector<std::string> getInfoVars();
  std::vector<std::string> getNewInfoVars();
  std::vector<std::string> getSpecUpdateVars();

  std::vector<LifeEvent>   getLifeEvents()   {return(m_life_events);};
  void                     clearLifeEvents() {m_life_events.clear();};

  bool        uniqueNameX(const std::string&, 
			  const std::set<std::string>&);
  bool        updateStateSpaceVars();
  std::string getStateSpaceVars();

  void printModeSet()   {m_mode_set.print();};
  void consultModeSet() {m_mode_set.evaluate();};
  std::vector<VarDataPair> getModeVarDataPairs()    
    {return(m_mode_set.getVarDataPairs());};
  std::string getModeSummary()     
    {return(m_mode_set.getModeSummary());};
  std::string getModeSetDefinition()  
    {return(m_mode_set.getStringDescription());};

  void print();

protected:
  std::vector<BehaviorSetEntry> m_bhv_entry;
  std::set<std::string>         m_bhv_names;

  std::vector<VarDataPair>      m_initial_vars;
  std::vector<VarDataPair>      m_default_vars;

  std::set<std::string>         m_prev_info_vars;   
  std::set<std::string>         m_state_space_vars; 

  std::vector<BehaviorSpec>     m_behavior_specs;
  BFactoryStatic                m_bfactory_static;
  BFactoryDynamic               m_bfactory_dynamic;

  std::vector<LifeEvent>        m_life_events;

  bool    m_report_ipf;
  double  m_curr_time;
  bool    m_completed_pending;

  ModeSet m_mode_set;

  unsigned int m_total_behaviors_ever;
};

#endif 

