/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HelmEngine.cpp                                       */
/*    DATE: Mar 24th, 2005                                       */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include <iostream>
#include <string>
#include "HelmEngine.h"
#include "MBUtils.h"
#include "MBTimer.h"
#include "IO_Utilities.h"
#include "IvPProblem.h"
#include "BehaviorSet.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

HelmEngine::HelmEngine(IvPDomain g_ivp_domain)
{
  m_ivp_domain = g_ivp_domain;
  m_iteration  = 0;
}

//------------------------------------------------------------------
// Procedure: determineNextDecision()

HelmReport HelmEngine::determineNextDecision(BehaviorSet *bhv_set, 
					     double curr_time)
{
  m_iteration++;

  HelmReport helm_report;

  helm_report.m_domain    = m_ivp_domain;
  helm_report.m_iteration = m_iteration;
  helm_report.m_time_utc  = curr_time;

  if(!bhv_set) {
    helm_report.m_halted = true;
    helm_report.addMsg("HELM HALTING CONDITION: NULL Behavior Set");
    return(helm_report);
  }
  else
    bhv_set->resetStateOK();

  bhv_set->setCurrTime(curr_time);

  int i, bhv_ix;
  int bhv_cnt = bhv_set->getCount();

  vector<IvPFunction*> ofs;

  // get all the objective functions and add time info to helm report
  MBTimer create_timer;
  create_timer.start();
  for(bhv_ix=0; bhv_ix<bhv_cnt; bhv_ix++) {
    MBTimer of_timer;
    of_timer.start();
    string astate;
    IvPFunction *newof = bhv_set->produceOF(bhv_ix, m_iteration, astate);
    of_timer.stop();
    
    // Determine the amt of time the bhv has been in this state
    double state_elapsed = bhv_set->getStateElapsed(bhv_ix);

    if(!bhv_set->stateOK(bhv_ix)) {
      helm_report.m_halted = true;
      helm_report.addMsg("HELM HALTING: Safety Emergency!!!");
      return(helm_report);
    }
    
    string upd_summary = bhv_set->getUpdateSummary(bhv_ix);
    string descriptor  =  bhv_set->getDescriptor(bhv_ix);
    string report_line = descriptor;
    if(newof) {
      double of_time  = of_timer.get_float_cpu_time();
      int    pieces   = newof->size();
      string timestr  = doubleToString(of_time,2);
      report_line += " produces obj-function - time:" + timestr;
      report_line += " pcs: " + doubleToString(pieces);
      report_line += " pwt: " + doubleToString(newof->getPWT());
    }
    else
      report_line += " did NOT produce an obj-function";
    
    if(newof) {
      double of_time  = of_timer.get_float_cpu_time();
      double pwt = newof->getPWT();
      int pcs = newof->size();
      helm_report.addActiveBHV(descriptor, state_elapsed, pwt, pcs, 
			       of_time, upd_summary);
    }
    if(astate=="running")
      helm_report.addRunningBHV(descriptor, state_elapsed, upd_summary);
    if(astate=="idle")
      helm_report.addIdleBHV(descriptor, state_elapsed, upd_summary);
    if(astate=="completed")
      helm_report.addCompletedBHV(descriptor, state_elapsed, upd_summary);
	
    helm_report.addMsg(report_line);
    ofs.push_back(newof);
  }

  bool result = checkOFDomains(ofs);
  if(!result) {
    helm_report.m_halted = true;
    helm_report.addMsg("HELM HALTING: Unrecognized domain var");
    return(helm_report);
  }

  // Create an IvPProblem w/ newly created OFs
  IvPProblem *ivp_problem = new IvPProblem;
  for(i=0; i<bhv_cnt; i++) {
    if(ofs[i])
      ivp_problem->addOF(ofs[i]);
  }

  int ofnum = ivp_problem->getOFNUM();
  helm_report.addMsg("Number of Objective Functions: " + intToString(ofnum)); 
  helm_report.m_ofnum = ofnum;

  if(ofnum == 0) {
    helm_report.addMsg("No Decision due to zero (behavior) OF's");
    if(ivp_problem)
      delete(ivp_problem);
    return(helm_report);
  }

  ivp_problem->setDomain(m_sub_domain);
  ivp_problem->alignOFs();
  create_timer.stop();

  MBTimer solve_timer;
  solve_timer.start();
  ivp_problem->solve();
  solve_timer.stop();

  double create_time = create_timer.get_float_cpu_time();
  double solve_time  = solve_timer.get_float_cpu_time();
  helm_report.m_create_time = create_time;
  helm_report.m_solve_time  = solve_time;
  helm_report.m_loop_time   = create_time + solve_time;
  
  int dsize = m_sub_domain.size();
  for(i=0; i<dsize; i++) {
    string dom_name = m_sub_domain.getVarName(i);
    bool   dec_made;
    double decision = ivp_problem->getResult(dom_name, &dec_made);
    
    // Add the decision to the report and a message for output
    helm_report.addDecision(dom_name, decision);
    helm_report.addMsg("DESIRED_" + toupper(dom_name) + ":  " +
		       doubleToString(decision, 2));
    
    // If the IvPSolver didnt produce a decision in the subdomain
    // this is a problem worthy of halting the helm (never shd hapn)
    if(!dec_made) {
      helm_report.m_halted = true;
      helm_report.addMsg("HELM HALTING CONDITION: No decision for " +
			 dom_name);
    }
  }
  delete(ivp_problem);

  return(helm_report);
}

//-----------------------------------------------------------
// Procedure: checkOFDomains()
//      Note: 
//            Ensure all OF domain names are contained in the 
//              m_ivp_domain. Indicate result=false if not.
//            Build a modified ivp_domain that contains only 
//              those domain names found in one or more OF's.
//            If the modified ivp_domain has *no* domains, 
//              indicate result=false.

bool HelmEngine::checkOFDomains(vector<IvPFunction*> ofs)
{
  // First build a vector of unique domain names strings from
  // all the objective functions.
  vector<string> of_domains;
  int vsize = ofs.size();
  for(int i=0; i<vsize; i++) {
    if(ofs[i]) {
      int dim = ofs[i]->getDim();
      for(int j=0; j<dim; j++) {
	string dname = ofs[i]->getVarName(j);
	bool already_present = false;
	for(unsigned int k=0; k<of_domains.size(); k++)
	  if(of_domains[k] == dname)
	    already_present = true;
	if(!already_present)
	  of_domains.push_back(dname);
      }
    }
  }

  // Now make sure there are no unknown domain names. All domain
  // names produced by objective functions should be present in
  // the ivp_domain structure.

  unsigned int domsize = of_domains.size();
  for(unsigned int i=0; i<domsize; i++) {
    bool ok_domain = false;
    int count = m_ivp_domain.size();
    for(int j=0; (!ok_domain && (j<count)); j++) {
      string dname = m_ivp_domain.getVarName(j);
      if(of_domains[i] == dname)
	ok_domain = true;
    }
    if(!ok_domain) {
      cout << "DomainVar " << of_domains[i] << " is not recognized ";
      cout << " by the IvPDomain configured to pHelmIvP" << endl;
      m_sub_domain = IvPDomain();
      return(false);
    }
  }

  // Now determine if we need to modify the ivp_domain. Since we
  // know from the the above block that there are no "unknown" 
  // domains present in any of the OF's, we can compare the size
  // of the of_domains vector with the number of elements in 
  // ivp_domain. If they are the same, modifying ivp_domain is not
  // necessary. If of_domains is smaller, we want to build a smaller
  // modified ivp_domain, and return it.

  if(domsize == m_ivp_domain.size()) {
    m_sub_domain = m_ivp_domain;
    return(true);
  }
  else {
    m_sub_domain = IvPDomain();
    for(unsigned int i=0; i<domsize; i++) {
      const char* dname = of_domains[i].c_str();
      int    index = m_ivp_domain.getIndex(dname);
      double dlow  = m_ivp_domain.getVarLow(index);
      double dhigh = m_ivp_domain.getVarHigh(index);
      int    dpoints = m_ivp_domain.getVarPoints(index);
      m_sub_domain.addDomain(dname, dlow, dhigh, dpoints);
    }
    return(true);
  }
}







