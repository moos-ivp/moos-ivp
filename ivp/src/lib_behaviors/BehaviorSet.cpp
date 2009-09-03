/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BehaviorSet.cpp                                      */
/*    DATE: Oct 27th 2004 Sox up 3-0 in the Series               */
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

#include <iostream>
#include <set>
#include "BehaviorSet.h"
#include "MBUtils.h"
#include "IvPFunction.h"
#include "FunctionEncoder.h"

using namespace std;

//------------------------------------------------------------
// Constructor

BehaviorSet::BehaviorSet()
{
  m_report_ipf = true;
  m_curr_time  = -1;
}

//------------------------------------------------------------
// Destructor

BehaviorSet::~BehaviorSet()
{
  int bsize = m_behaviors.size();
  for(int i=0; i<bsize; i++) {
    delete(m_behaviors[i]);
  }
}

//------------------------------------------------------------
// Procedure: addBehavior

void BehaviorSet::addBehavior(IvPBehavior *bhv)
{
  m_behaviors.push_back(bhv);
  m_behavior_states.push_back("");
  m_behavior_state_time_entered.push_back(-1);
  m_behavior_state_time_elapsed.push_back(-1);
}


//------------------------------------------------------------
// Procedure: produceOF

IvPFunction* BehaviorSet::produceOF(unsigned int ix, 
				    unsigned int iteration, 
				    string& new_activity_state)
{
  IvPFunction *ipf = 0;
  double pwt = 0;
  int    pcs = 0;

  if(ix < m_behaviors.size()) {
    // Look for possible dynamic updates to the behavior parameters
    bool update_made = m_behaviors[ix]->checkUpdates();
    if(update_made)
      m_behaviors[ix]->onSetParamComplete();

    // Check if the behavior duration is to be reset
    m_behaviors[ix]->checkForDurationReset();

    if(m_behaviors[ix]->isCompleted())
      new_activity_state = "completed";
    else {
      if(!m_behaviors[ix]->isRunnable())
	new_activity_state = "idle";
      else
	new_activity_state = "running";
    }

    // Now that the new_activity_state is set, act appropriately for
    // each behavior.

    if(new_activity_state == "idle") {
      m_behaviors[ix]->postFlags("idleflags");
      m_behaviors[ix]->postFlags("inactiveflags");
      if((m_behavior_states[ix] == "running") ||
	 (m_behavior_states[ix] == "active"))
	m_behaviors[ix]->onRunToIdleState();
      m_behaviors[ix]->onIdleState();
      m_behaviors[ix]->updateStateDurations("idle");
    }
    
    if(new_activity_state == "running") {
      m_behaviors[ix]->postDurationStatus();
      m_behaviors[ix]->postFlags("runflags");
      if(m_behavior_states[ix] == "idle") 
	m_behaviors[ix]->onIdleToRunState();
      ipf = m_behaviors[ix]->onRunState();
      if(ipf && !ipf->freeOfNan()) {
	m_behaviors[ix]->postEMessage("NaN detected in IvP Function");
	delete(ipf);
	ipf = 0;
      }
      if(ipf) {
	pwt = ipf->getPWT();
	pcs = ipf->getPDMap()->size();
	if(pwt <= 0) {
	  delete(ipf);
	  ipf = 0;
	  pcs = 0;
	}
      }
      if(ipf && m_report_ipf) {
	string desc_str = m_behaviors[ix]->getDescriptor();
	string iter_str = intToString(iteration);
	string ctxt_str = iter_str + ":" + desc_str;
	ipf->setContextStr(ctxt_str);
	string ipf_str = IvPFunctionToString(ipf);
	m_behaviors[ix]->postMessage("BHV_IPF", ipf_str);
      }
      if(ipf) {
	new_activity_state = "active";
	m_behaviors[ix]->postFlags("activeflags");
      }
      else
	m_behaviors[ix]->postFlags("inactiveflags");
      m_behaviors[ix]->updateStateDurations("running");
    }

    string bhv_tag = toupper(m_behaviors[ix]->getDescriptor());
    bhv_tag = findReplace(bhv_tag, "BHV_", "");
    bhv_tag = findReplace(bhv_tag, "(D)", "");

#if 1
    m_behaviors[ix]->postMessage("PWT_BHV_"+bhv_tag, pwt);
    if(new_activity_state == "idle")
      m_behaviors[ix]->postMessage("STATE_BHV_"+bhv_tag, 0);
    else if(new_activity_state == "running")
      m_behaviors[ix]->postMessage("STATE_BHV_"+bhv_tag, 1);
    else if(new_activity_state == "active") 
      m_behaviors[ix]->postMessage("STATE_BHV_"+bhv_tag, 2);
    else if(new_activity_state == "completed") 
      m_behaviors[ix]->postMessage("STATE_BHV_"+bhv_tag, 3);
#endif

    // If this represents a change in states from the previous
    // iteration, note the time at which the state changed.
    if(m_behavior_states[ix] != new_activity_state)
      m_behavior_state_time_entered[ix] = m_curr_time;
    
    m_behavior_states[ix] = new_activity_state;
    double elapsed = m_curr_time - m_behavior_state_time_entered[ix];
    m_behavior_state_time_elapsed[ix] = elapsed;
  }
  return(ipf);
}

//------------------------------------------------------------
// Procedure: stateOK

bool BehaviorSet::stateOK(unsigned int ix)
{
  if(ix < m_behaviors.size())
    return(m_behaviors[ix]->stateOK());
  else
    return(false);
}

//------------------------------------------------------------
// Procedure: resetStateOK

void BehaviorSet::resetStateOK()
{
  for(unsigned int i=0; i<m_behaviors.size(); i++) 
    m_behaviors[i]->resetStateOK();
}

//------------------------------------------------------------
// Procedure: getBehavior

IvPBehavior* BehaviorSet::getBehavior(unsigned int ix)
{
  if(ix < m_behaviors.size())
    return(m_behaviors[ix]);
  return(0);
}

//------------------------------------------------------------
// Procedure: getDescriptor

string BehaviorSet::getDescriptor(unsigned int ix)
{
  if(ix < m_behaviors.size())
    return(m_behaviors[ix]->getDescriptor());
  return("");
}

//------------------------------------------------------------
// Procedure: getStateElapsed

double BehaviorSet::getStateElapsed(unsigned int ix)
{
  if(ix < m_behaviors.size())
    return(m_behavior_state_time_elapsed[ix]);
  else
    return(-1);
}

//------------------------------------------------------------
// Procedure: getUpdateSummary()

string BehaviorSet::getUpdateSummary(unsigned int ix)
{
  if(ix < m_behaviors.size())
    return(m_behaviors[ix]->getUpdateSummary());
  else
    return("err");
}

//------------------------------------------------------------
// Procedure: getFilterLevel

int BehaviorSet::getFilterLevel(unsigned int ix)
{
  if(ix < m_behaviors.size())
    return(m_behaviors[ix]->getFilterLevel());
  else
    return(0);
}

//------------------------------------------------------------
// Procedure: filterBehaviorsPresent

bool BehaviorSet::filterBehaviorsPresent()
{
  unsigned int ix, vsize = m_behaviors.size();
  for(ix=0; ix<vsize; ix++) 
    if(m_behaviors[ix]->getFilterLevel() != 0)
      return(true);
  return(false);
}

//------------------------------------------------------------
// Procedure: getMessages

vector<VarDataPair> BehaviorSet::getMessages(unsigned int ix)
{
  vector<VarDataPair> mvector;
  if(ix < m_behaviors.size()) {
    mvector = m_behaviors[ix]->getMessages();
    m_behaviors[ix]->clearMessages();
  }
  return(mvector);
}

//------------------------------------------------------------
// Procedure: getInfoVars

vector<string> BehaviorSet::getInfoVars()
{
  vector<string> rvector;
  for(unsigned int i=0; i<m_behaviors.size(); i++) {
    vector<string> bvector = m_behaviors[i]->getInfoVars();
    for(unsigned int j=0; j<bvector.size(); j++)
      rvector.push_back(bvector[j]);
  }

  vector<string> mvector = m_mode_set.getConditionVars();

  rvector = mergeVectors(rvector, mvector);

  rvector = removeDuplicates(rvector);
  
  return(rvector);
}

//------------------------------------------------------------
// Procedure: getNewInfoVars

vector<string> BehaviorSet::getNewInfoVars()
{
  vector<string> cvector = getInfoVars();
  int csize = cvector.size();

  vector<string> rvector;
  for(int i=0; i<csize; i++) {
    if(!vectorContains(m_prev_info_vars, cvector[i]))
      rvector.push_back(cvector[i]);
  }

  m_prev_info_vars = cvector;

  return(rvector);
}

//------------------------------------------------------------
// Procedure: updateStateSpaceVars()
//      Note: Since duplicates are checked for here, and this is
//            the only place where the state_space_vars vector
//            is updated, we know there are no duplicates here.
//      Note: Returns true only if a new variable has been detected
//            in any of the instantiated behaviors.

bool BehaviorSet::updateStateSpaceVars()
{
  bool new_var = false;
  
  int vsize = m_behaviors.size();
  for(int i=0; i<vsize; i++) {
    if(m_behaviors[i] != 0) {
      vector<string> ivars = m_behaviors[i]->getStateSpaceVars();
      int isize = ivars.size();
      for(int k=0; k<isize; k++) {
	if(!vectorContains(m_state_space_vars, ivars[k])) {
	  m_state_space_vars.push_back(ivars[k]);
	  new_var = true;
	}
      }
    }
  }
  return(new_var);
}

//------------------------------------------------------------
// Procedure: getStateSpaceVars()

string BehaviorSet::getStateSpaceVars()
{
  string rstr;
  int vsize = m_state_space_vars.size();
  for(int i=0; i<vsize; i++) {
    if(rstr != "")
      rstr += ",";
    rstr += m_state_space_vars[i];
  }
  return(rstr);
}

//------------------------------------------------------------
// Procedure: print

void BehaviorSet::print()
{
  unsigned int i;

  cout << "BehaviorSet::print() " << endl;
  for(i=0; i<m_behaviors.size(); i++) {
    cout << "Behavior[" << i << "]: " << endl;
    

    cout << "Behavior descriptor: " << m_behaviors[i]->m_descriptor << endl;
    cout << " priority weight: " << m_behaviors[i]->m_priority_wt << endl;
    cout << " BuildInfo: " << endl;
    m_behaviors[i]->m_build_info;
    cout << "-------" << endl;
  }
}




