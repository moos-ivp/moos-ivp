/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BehaviorSet.h                                        */
/*    DATE: Oct 27th 2004                                        */
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
  
  bool       addBehaviorDir(std::string);
  void       addBehaviorSpec(BehaviorSpec spec);  
  void       setDomain(IvPDomain domain);
  void       connectInfoBuffer(InfoBuffer*);
  bool       buildBehaviorsFromSpecs();
  SpecBuild  buildBehaviorFromSpec(BehaviorSpec spec, std::string s="",
				   bool on_startup=false);
  bool       handlePossibleSpawnings();
  bool       refreshMapUpdateVars();
  
  void   addBehavior(IvPBehavior *b);
  void   clearBehaviors();
  void   setOwnship(std::string s)      {m_ownship=s;}
  void   addInitialVar(VarDataPair msg) {m_initial_vars.push_back(msg);}
  void   addDefaultVar(VarDataPair msg) {m_default_vars.push_back(msg);}
  void   setCompletedPending(bool v)    {m_completed_pending = v;}
  void   setCurrTime(double v)          {m_curr_time = v;}
  double getCurrTime()                  {return(m_curr_time);}
  void   setModeSet(ModeSet v)          {m_mode_set = v;}

  unsigned int getTCount()              {return(m_total_behaviors_ever);}
  
  unsigned int size()                   {return(m_bhv_entry.size());}

  void         setReportIPF(bool v)     {m_report_ipf=v;}
  bool         stateOK(unsigned int);
  void         resetStateOK();
  IvPFunction* produceOF(unsigned int ix, unsigned int iter, 
			 std::string& activity_state, bool& ipf_reuse);

  BehaviorReport produceOFX(unsigned int ix, unsigned int iter, 
			    std::string& activity_state);
  
  unsigned int   removeCompletedBehaviors();
  IvPBehavior*   getBehavior(unsigned int);
  std::string    getDescriptor(unsigned int);
  std::string    getUpdateSummary(unsigned int);
  std::string    getUpdateVarSummary();
  double         getStateElapsed(unsigned int);
  double         getStateTimeEntered(unsigned int);
  int            getFilterLevel(unsigned int);
  bool           filterBehaviorsPresent();
  bool           getCompletedPending() const {return(m_completed_pending);}

  std::vector<VarDataPair> getMessages(unsigned int bhv_ix, bool clear=true);
  std::vector<VarDataPair> getInitialVariables()  {return(m_initial_vars);}
  std::vector<VarDataPair> getDefaultVariables()  {return(m_default_vars);}
  std::vector<VarDataPair> getHelmStartMessages() {return(m_helm_start_msgs);}
  std::vector<std::string> getInfoVars();
  std::vector<std::string> getNewInfoVars();
  std::vector<std::string> getSpecUpdateVars();
  std::vector<std::string> getUpdateResults() const {return(m_update_results);}

  void                     clearUpdateResults() {m_update_results.clear();}
  void                     addWarning(const std::string&);
  std::vector<std::string> getWarnings()     {return(m_warnings);}
  void                     clearWarnings()   {m_warnings.clear();}

  std::vector<LifeEvent>   getLifeEvents()   {return(m_life_events);}
  void                     clearLifeEvents() {m_life_events.clear();}

  bool        uniqueNameX(const std::string&, 
			  const std::set<std::string>&);
  bool        updateStateSpaceVars();
  std::string getStateSpaceVars();

  void printModeSet()   {m_mode_set.print();}
  void consultModeSet() {m_mode_set.evaluate();}

  std::vector<VarDataPair> getModeVarDataPairs()    
    {return(m_mode_set.getVarDataPairs());}
  std::string getModeSummary()     
    {return(m_mode_set.getModeSummary());}
  std::string getModeSetDefinition()  
    {return(m_mode_set.getStringDescription());}

  std::vector<std::string> getTemplatingSummary() const;

  // Added by tes to support Graphviz 
  std::map<std::string, std::vector<LogicCondition> > getModeLogicConditions()
    {return(m_mode_set.getNonModeLogicConditions());}

  void print();

  unsigned long int size() const;
  
protected:
  std::vector<BehaviorSetEntry> m_bhv_entry;
  std::set<std::string>         m_bhv_names;

  std::vector<VarDataPair>      m_initial_vars;
  std::vector<VarDataPair>      m_default_vars;
  std::vector<VarDataPair>      m_helm_start_msgs;

  std::set<std::string>         m_prev_info_vars;   
  std::set<std::string>         m_state_space_vars; 

  std::vector<std::string>      m_warnings;

  std::vector<BehaviorSpec>     m_behavior_specs;
  
  BFactoryStatic                m_bfactory_static;
  BFactoryDynamic               m_bfactory_dynamic;

  std::vector<LifeEvent>        m_life_events;

  std::vector<std::string>      m_update_results;
  
  std::map<std::string, std::string> m_map_update_vars;
  
  std::string m_ownship;

  bool    m_report_ipf;
  double  m_curr_time;
  bool    m_completed_pending;

  ModeSet m_mode_set;

  unsigned int m_total_behaviors_ever;
};

#endif 




