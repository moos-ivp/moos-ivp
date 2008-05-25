/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
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
#include "IvPBehavior.h"
#include "VarDataPair.h"

class IvPFunction;
class BehaviorSet
{
public:
  BehaviorSet();
  virtual ~BehaviorSet();
  
  void addBehavior(IvPBehavior *b);
  void addInitialVar(VarDataPair msg) {initial_vars.push_back(msg);};
  void addDefaultVar(VarDataPair msg) {default_vars.push_back(msg);};
  int  getCount()                     {return(behaviors.size());};
  void setCurrTime(double v)          {m_curr_time = v;};
  
  void         setReportIPF(bool v)   {m_report_ipf=v;};
  bool         stateOK(unsigned int);
  void         resetStateOK();
  IvPFunction* produceOF(unsigned int ix, unsigned int iter, 
			 std::string& activity_state);

  IvPBehavior*   getBehavior(unsigned int);
  std::string    getDescriptor(unsigned int);
  double         getStateElapsed(unsigned int);
  std::string    getUpdateSummary(unsigned int);

  std::vector<VarDataPair> getMessages(unsigned int);
  std::vector<VarDataPair> getInitialVariables() {return(initial_vars);};
  std::vector<VarDataPair> getDefaultVariables() {return(default_vars);};
  std::vector<std::string> getInfoVars();
  std::vector<std::string> getNewInfoVars();

  bool        updateStateSpaceVars();
  std::string getStateSpaceVars();

  void print();

protected:
  std::vector<IvPBehavior*>   behaviors;
  std::vector<std::string>    behavior_states;
  std::vector<double>         behavior_state_time_entered;
  std::vector<double>         behavior_state_time_elapsed;

  std::vector<VarDataPair>    initial_vars;
  std::vector<VarDataPair>    default_vars;

  std::vector<std::string>    prev_info_vars;
  std::vector<std::string>    state_space_vars;

  bool   m_report_ipf;
  double m_curr_time;
};

#endif 
