/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HelmEngineBeta.cpp                                   */
/*    DATE: July 29th 2009                                       */
/*    DATE: Mar 24th, 2005 (based on an older implementation)    */
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
#include "HelmEngineBeta.h"
#include "MBUtils.h"
#include "MBTimer.h"
#include "IO_Utilities.h"
#include "IvPProblem.h"
#include "BehaviorSet.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

HelmEngineBeta::HelmEngineBeta(IvPDomain g_ivp_domain, 
			       InfoBuffer *g_info_buffer)
{
  m_ivp_domain  = g_ivp_domain;
  m_info_buffer = g_info_buffer;
  m_iteration   = 0;
  m_bhv_set     = 0;
  m_curr_time   = 0;
  m_ivp_problem = 0;
}

//-----------------------------------------------------------
// Procedure: Destructor

HelmEngineBeta::~HelmEngineBeta()
{
  delete(m_ivp_problem);
}

//------------------------------------------------------------------
// Procedure: determineNextDecision()

HelmReport HelmEngineBeta::determineNextDecision(BehaviorSet *bhv_set, 
						 double curr_time)
{
  // Update the HelmEngine member variables
  m_iteration++;
  m_bhv_set     = bhv_set;
  m_curr_time   = curr_time;
  m_helm_report = HelmReport();
  m_ivp_functions.clear();

  bool filter_behaviors_present = bhv_set->filterBehaviorsPresent();

  bool handled = true;
  handled = handled && part1_PreliminaryBehaviorSetHandling();
  handled = handled && part2_GetFunctionsFromBehaviorSet(0);
  handled = handled && part3_VerifyFunctionDomains();

  if(filter_behaviors_present) {
    handled = handled && part4_BuildAndSolveIvPProblem("prefilter");
    handled = handled && part2_GetFunctionsFromBehaviorSet(1);
    handled = handled && part3_VerifyFunctionDomains();
  }

  handled = handled && part4_BuildAndSolveIvPProblem();
  handled = handled && part6_FinishHelmReport();
  return(m_helm_report);
}

//------------------------------------------------------------------
// Procedure: part1_PreliminaryBehaviorSetHandling

bool HelmEngineBeta::part1_PreliminaryBehaviorSetHandling()
{
  m_helm_report.m_domain    = m_ivp_domain;
  m_helm_report.m_iteration = m_iteration;
  m_helm_report.m_time_utc  = m_curr_time;

  if(!m_bhv_set) {
    m_helm_report.m_halted = true;
    m_helm_report.addMsg("HELM HALTING CONDITION: NULL Behavior Set");
    return(false);
  }
  else
    m_bhv_set->resetStateOK();

  m_bhv_set->setCurrTime(m_curr_time);

  // Update Modes and add mode_summary to the m_helm_report.
  m_bhv_set->consultModeSet();
  string mode_summary = m_bhv_set->getModeSummary();
  m_helm_report.setModeSummary(mode_summary);
  
  return(true);
}


//------------------------------------------------------------------
// Procedure: part2_GetFunctionsFromBehaviorSet()
//     Notes: Gets the IvP functions from behaviors and adds them 
//            to the m_ivp_functions
//
//    Inputs: m_bhv_set, filter_level
//   Outputs: m_ivp_functions, m_helm_report, timers

bool HelmEngineBeta::part2_GetFunctionsFromBehaviorSet(int filter_level)
{
  int bhv_ix, bhv_cnt = m_bhv_set->getCount();

  // get all the objective functions and add time info to helm report
  m_create_timer.start();
  for(bhv_ix=0; bhv_ix<bhv_cnt; bhv_ix++) {
    if(m_bhv_set->getFilterLevel(bhv_ix) == filter_level) {
      string bhv_state;
      m_ipf_timer.start();
      IvPFunction *newof = m_bhv_set->produceOF(bhv_ix, m_iteration, bhv_state);
      m_ipf_timer.stop();
  
      // Determine the amt of time the bhv has been in this state
      double state_elapsed = m_bhv_set->getStateElapsed(bhv_ix);

      if(!m_bhv_set->stateOK(bhv_ix)) {
	m_helm_report.m_halted = true;
	m_helm_report.addMsg("HELM HALTING: Safety Emergency!!!");
	m_create_timer.stop();
	return(false);
      }
      
      string upd_summary = m_bhv_set->getUpdateSummary(bhv_ix);
      string descriptor  = m_bhv_set->getDescriptor(bhv_ix);
      string report_line = descriptor;
      if(newof) {
	double of_time  = m_ipf_timer.get_float_cpu_time();
	int    pieces   = newof->size();
	string timestr  = doubleToString(of_time,2);
	report_line += " produces obj-function - time:" + timestr;
	report_line += " pcs: " + doubleToString(pieces);
	report_line += " pwt: " + doubleToString(newof->getPWT());
      }
      else
	report_line += " did NOT produce an obj-function";
      m_helm_report.addMsg(report_line);
      
      if(newof) {
	double of_time  = m_ipf_timer.get_float_cpu_time();
	double pwt = newof->getPWT();
	int    pcs = newof->size();
	m_helm_report.addActiveBHV(descriptor, state_elapsed, pwt, pcs, 
				   of_time, upd_summary);
	m_ivp_functions.push_back(newof);
      }

      if(bhv_state=="running")
	m_helm_report.addRunningBHV(descriptor, state_elapsed, upd_summary);
      if(bhv_state=="idle")
	m_helm_report.addIdleBHV(descriptor, state_elapsed, upd_summary);
      if(bhv_state=="completed")
	m_helm_report.addCompletedBHV(descriptor, state_elapsed, upd_summary);
    }
  }
  m_create_timer.stop();
  
  return(true);
}


//-----------------------------------------------------------
// Procedure: part3_VerifyFunctionDomains()
//      Note: (1) Ensure all OF domain names are contained in the 
//                m_ivp_domain. Indicate result=false if not.
//            (2) Build a modified ivp_domain that contains only 
//                those domain names found in one or more OF's.
//            (3) If the modified ivp_domain has *no* domains, 
//                indicate result=false.
//  
//    Inputs: m_ivp_functions
//   Outputs: m_sub_domain, m_helm_report



bool HelmEngineBeta::part3_VerifyFunctionDomains()
{
  // First build a vector of unique domain names strings from
  // all the objective functions.
  vector<string> of_domains;
  int vsize = m_ivp_functions.size();
  for(int i=0; i<vsize; i++) {
    if(m_ivp_functions[i]) {
      int dim = m_ivp_functions[i]->getDim();
      for(int j=0; j<dim; j++) {
	string dname = m_ivp_functions[i]->getVarName(j);
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
      m_helm_report.m_halted = true;
      m_helm_report.addMsg("HELM HALTING: Unrecognized domain var");
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

//------------------------------------------------------------------
// Procedure: part4_BuildAndSolveIvPProblem()

bool HelmEngineBeta::part4_BuildAndSolveIvPProblem(string phase)
{
  unsigned int i, ipfs = m_ivp_functions.size(); 
  m_helm_report.addMsg("Number of IvP Functions: " + intToString(ipfs)); 
  m_helm_report.m_ofnum = ipfs;
  if(ipfs == 0) {
    m_helm_report.addMsg("No Decision due to zero IvP functions");
    return(false);
  }

  // Create, Prepare, and Solve the IvP problem
  m_ivp_problem = new IvPProblem;
  m_solve_timer.start();
  for(i=0; i<ipfs; i++)
      m_ivp_problem->addOF(m_ivp_functions[i]);
  m_ivp_problem->setDomain(m_sub_domain);
  m_ivp_problem->alignOFs();
  m_ivp_problem->solve();
  m_solve_timer.stop();

  unsigned int dsize = m_sub_domain.size();
  for(i=0; i<dsize; i++) {
    string dom_name = m_sub_domain.getVarName(i);
    double decision = m_ivp_problem->getResult(dom_name);
    string post_str = "DESIRED_" + toupper(dom_name);

    // Add the decision to the report and a message for output  
    if(phase == "prefilter")
      m_info_buffer->setValue((post_str + "_UNFILTERED"), decision);
    else {
      m_helm_report.addDecision(dom_name, decision);
      m_helm_report.addMsg(post_str+": " + doubleToString(decision,2));
    }
  }    
  
  if(phase == "prefilter")
    m_ivp_problem->setOwnerIPFs(false);

  delete(m_ivp_problem);
  m_ivp_problem = 0;
  
  return(true);
}

//------------------------------------------------------------------
// Procedure: part6_FinishHelmReport()

bool HelmEngineBeta::part6_FinishHelmReport()
{
  double create_time = m_create_timer.get_float_cpu_time();
  double solve_time  = m_solve_timer.get_float_cpu_time();
  m_create_timer.reset();
  m_solve_timer.reset();
  m_helm_report.m_create_time = create_time;
  m_helm_report.m_solve_time  = solve_time;
  m_helm_report.m_loop_time   = create_time + solve_time;
  return(true);
}

