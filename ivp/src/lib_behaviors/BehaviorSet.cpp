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
  report_ipf = true;
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
// Procedure: produceOF

IvPFunction* BehaviorSet::produceOF(int ix, int iteration, 
				    string& activity_state)
{
  IvPFunction *ipf = 0;
  double pwt = 0;

  if((ix >= 0) && (ix < behaviors.size())) {
    
    behaviors[ix]->checkForUpdates();

    if(behaviors[ix]->isCompleted())
      activity_state = "completed";
    else {
      if(!behaviors[ix]->isRunnable())
	activity_state = "idle";
      else
	activity_state = "running";
    }

    if(activity_state == "idle") {
      behaviors[ix]->postIdleFlags();
      behaviors[ix]->onIdleState();
    }

    if(activity_state == "running") {
      behaviors[ix]->postRunFlags();
      ipf = behaviors[ix]->produceOF();
      if(ipf && !ipf->freeOfNan()) {
	behaviors[ix]->postEMessage("NaN detected in IvP Function");
	delete(ipf);
	ipf = 0;
      }
      if(ipf) {
	pwt = ipf->getPWT();
	if(pwt <= 0) {
	  delete(ipf);
	  ipf = 0;
	}
      }
      if(ipf && report_ipf) {
	string desc_str = behaviors[ix]->getDescriptor();
	string iter_str = intToString(iteration);
	string ctxt_str = iter_str + ":" + desc_str;
	ipf->setContextStr(ctxt_str);
	string ipf_str = IvPFunctionToString(ipf);
	behaviors[ix]->postMessage("BHV_IPF", ipf_str);
      }
      if(ipf)
	activity_state = "active";
    }

    string bhv_tag = toupper(behaviors[ix]->getDescriptor());
    bhv_tag = findReplace(bhv_tag, "BHV_", "");
    bhv_tag = findReplace(bhv_tag, "(d)", "");

    behaviors[ix]->postMessage("PWT_BHV_"+bhv_tag, pwt);

    if(activity_state == "idle")
      behaviors[ix]->postMessage("STATE_BHV_"+bhv_tag, 0);
    else if(activity_state == "running")
      behaviors[ix]->postMessage("STATE_BHV_"+bhv_tag, 1);
    else if(activity_state == "active") 
      behaviors[ix]->postMessage("STATE_BHV_"+bhv_tag, 2);
    else if(activity_state == "completed") 
      behaviors[ix]->postMessage("STATE_BHV_"+bhv_tag, 3);
  }

  return(ipf);
}

//------------------------------------------------------------
// Procedure: stateOK

bool BehaviorSet::stateOK(int ix)
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
  for(int i=0; i<behaviors.size(); i++) 
    behaviors[i]->resetStateOK();
}

//------------------------------------------------------------
// Procedure: getBehavior

IvPBehavior* BehaviorSet::getBehavior(int ix)
{
  if((ix >= 0) && (ix < behaviors.size()))
    return(behaviors[ix]);
  return(0);
}

//------------------------------------------------------------
// Procedure: getDescriptor

string BehaviorSet::getDescriptor(int ix)
{
  if((ix >= 0) && (ix < behaviors.size()))
    return(behaviors[ix]->getDescriptor());
  return("");
}

//------------------------------------------------------------
// Procedure: getMessages

vector<VarDataPair> BehaviorSet::getMessages(int ix)
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
  for(int i=0; i<behaviors.size(); i++) {
    vector<string> bvector = behaviors[i]->getInfoVars();
    for(int j=0; j<bvector.size(); j++)
      rvector.push_back(bvector[j]);
  }

  vector<string>::iterator p1;

  set<string> temp_set;
  for(int j=0; j<rvector.size(); j++)
    temp_set.insert(rvector[j]);
  
  rvector.clear();

  set<string>::iterator p;
  for(p=temp_set.begin(); p!=temp_set.end(); p++)
    rvector.push_back(*p);

  return(rvector);
}

//------------------------------------------------------------
// Procedure: print

void BehaviorSet::print()
{
  cout << "BehaviorSet::print() " << endl;
  for(int i=0; i<behaviors.size(); i++) {
    cout << "Behavior[" << i << "]: " << endl;
    

    cout << "Behavior descriptor: " << behaviors[i]->m_descriptor << endl;
    cout << " priority weight: " << behaviors[i]->m_priority_wt << endl;
    cout << " BuildInfo: " << endl;
    behaviors[i]->m_build_info;
    cout << "-------" << endl;
  }
}










