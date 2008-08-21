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
  int bsize = behaviors.size();
  for(int i=0; i<bsize; i++) {
    delete(behaviors[i]);
  }
}

//------------------------------------------------------------
// Procedure: addBehavior

void BehaviorSet::addBehavior(IvPBehavior *bhv)
{
cout << "BehaviorSet::addBehavior( " << ((void*) bhv) << ")" << endl;

  behaviors.push_back(bhv);
  behavior_states.push_back("");
  behavior_state_time_entered.push_back(-1);
  behavior_state_time_elapsed.push_back(-1);
}


//------------------------------------------------------------
// Procedure: produceOF

IvPFunction* BehaviorSet::produceOF(unsigned int ix, unsigned int iteration, 
				    string& new_activity_state)
{
  IvPFunction *ipf = 0;
  double pwt = 0;
  int    pcs = 0;

  if((ix >= 0) && (ix < behaviors.size())) {
    
    // Look for possible dynamic updates to the behavior parameters
    behaviors[ix]->checkUpdates();

    if(behaviors[ix]->isCompleted())
      new_activity_state = "completed";
    else {
      if(!behaviors[ix]->isRunnable())
	new_activity_state = "idle";
      else
	new_activity_state = "running";
    }

    if(new_activity_state == "idle") {
      behaviors[ix]->postFlags("idleflags");
      behaviors[ix]->postFlags("inactiveflags");
      behaviors[ix]->onIdleState();
    }
    
    if(new_activity_state == "running") {
      behaviors[ix]->postFlags("runflags");
      ipf = behaviors[ix]->onRunState();
      if(ipf && !ipf->freeOfNan()) {
	behaviors[ix]->postEMessage("NaN detected in IvP Function");
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
	string desc_str = behaviors[ix]->getDescriptor();
	string iter_str = intToString(iteration);
	string ctxt_str = iter_str + ":" + desc_str;
	ipf->setContextStr(ctxt_str);
	string ipf_str = IvPFunctionToString(ipf);
	behaviors[ix]->postMessage("BHV_IPF", ipf_str);
      }
      if(ipf) {
	new_activity_state = "active";
	behaviors[ix]->postFlags("activeflags");
      }
      else
	behaviors[ix]->postFlags("inactiveflags");
    }

    string bhv_tag = toupper(behaviors[ix]->getDescriptor());
    bhv_tag = findReplace(bhv_tag, "BHV_", "");
    bhv_tag = findReplace(bhv_tag, "(d)", "");

#if 1
    behaviors[ix]->postMessage("PWT_BHV_"+bhv_tag, pwt);

    if(new_activity_state == "idle")
      behaviors[ix]->postMessage("STATE_BHV_"+bhv_tag, 0);
    else if(new_activity_state == "running")
      behaviors[ix]->postMessage("STATE_BHV_"+bhv_tag, 1);
    else if(new_activity_state == "active") 
      behaviors[ix]->postMessage("STATE_BHV_"+bhv_tag, 2);
    else if(new_activity_state == "completed") 
      behaviors[ix]->postMessage("STATE_BHV_"+bhv_tag, 3);
#endif

    if(behavior_states[ix] != new_activity_state)
      behavior_state_time_entered[ix] = m_curr_time;
    
    behavior_states[ix] = new_activity_state;
    double elapsed = m_curr_time - behavior_state_time_entered[ix];
    behavior_state_time_elapsed[ix] = elapsed;
  }
  return(ipf);
}

//------------------------------------------------------------
// Procedure: stateOK

bool BehaviorSet::stateOK(unsigned int ix)
{
  if((ix >= 0) && (ix < behaviors.size()))
    return(behaviors[ix]->stateOK());
  else
    return(false);
}

//------------------------------------------------------------
// Procedure: resetStateOK

void BehaviorSet::resetStateOK()
{
  for(unsigned int i=0; i<behaviors.size(); i++) 
    behaviors[i]->resetStateOK();
}

//------------------------------------------------------------
// Procedure: getBehavior

IvPBehavior* BehaviorSet::getBehavior(unsigned int ix)
{
  if((ix >= 0) && (ix < behaviors.size()))
    return(behaviors[ix]);
  return(0);
}

//------------------------------------------------------------
// Procedure: getDescriptor

string BehaviorSet::getDescriptor(unsigned int ix)
{
  if((ix >= 0) && (ix < behaviors.size()))
    return(behaviors[ix]->getDescriptor());
  return("");
}

//------------------------------------------------------------
// Procedure: getStateElapsed

double BehaviorSet::getStateElapsed(unsigned int ix)
{
  if((ix >= 0) && (ix < behaviors.size()))
    return(behavior_state_time_elapsed[ix]);
  else
    return(-1);
}

//------------------------------------------------------------
// Procedure: getUpdateSummary()

string BehaviorSet::getUpdateSummary(unsigned int ix)
{
  if((ix >= 0) && (ix < behaviors.size()))
    return(behaviors[ix]->getUpdateSummary());
  else
    return("err");
}

//------------------------------------------------------------
// Procedure: getMessages

vector<VarDataPair> BehaviorSet::getMessages(unsigned int ix)
{
  vector<VarDataPair> mvector;
  if((ix >= 0) && (ix < behaviors.size())) {
    mvector = behaviors[ix]->getMessages();
    behaviors[ix]->clearMessages();
  }
  return(mvector);
}

//------------------------------------------------------------
// Procedure: getInfoVars

vector<string> BehaviorSet::getInfoVars()
{
  vector<string> rvector;
  for(unsigned int i=0; i<behaviors.size(); i++) {
    vector<string> bvector = behaviors[i]->getInfoVars();
    for(unsigned int j=0; j<bvector.size(); j++)
      rvector.push_back(bvector[j]);
  }

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
    if(!vectorContains(prev_info_vars, cvector[i]))
      rvector.push_back(cvector[i]);
  }

  prev_info_vars = cvector;

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
  
  int vsize = behaviors.size();
  for(int i=0; i<vsize; i++) {
    if(behaviors[i] != 0) {
      vector<string> ivars = behaviors[i]->getStateSpaceVars();
      int isize = ivars.size();
      for(int k=0; k<isize; k++) {
	if(!vectorContains(state_space_vars, ivars[k])) {
	  state_space_vars.push_back(ivars[k]);
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
  int vsize = state_space_vars.size();
  for(int i=0; i<vsize; i++) {
    if(rstr != "")
      rstr += ",";
    rstr += state_space_vars[i];
  }
  return(rstr);
}

//------------------------------------------------------------
// Procedure: print

void BehaviorSet::print()
{
  unsigned int i;

  cout << "BehaviorSet::print() " << endl;
  for(i=0; i<behaviors.size(); i++) {
    cout << "Behavior[" << i << "]: " << endl;
    

    cout << "Behavior descriptor: " << behaviors[i]->m_descriptor << endl;
    cout << " priority weight: " << behaviors[i]->m_priority_wt << endl;
    cout << " BuildInfo: " << endl;
    behaviors[i]->m_build_info;
    cout << "-------" << endl;
  }
}










