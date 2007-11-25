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
  ivp_domain   = g_ivp_domain;
  iteration    = 0;
}

//------------------------------------------------------------------
// Procedure: determineNextDecision()
//      Note: An instance of IvPProblem is created on each iteration 
//            of this loop. This object is allowed to persist as a
//            member variable so that calls to getIMG can access the 
//            objective functions of the problem for viewing if 
//            desired. The instance is deleted just prior to the new
//            instance creation each time.

HelmReport HelmEngine::determineNextDecision(BehaviorSet *bhv_set, 
					     double curr_time)
{
  iteration++;

  HelmReport helm_report;
  helm_report.m_iteration = iteration;

  if(!bhv_set) {
    helm_report.m_halted = true;
    helm_report.addMsg("HELM HALTING CONDITION: NULL Behavior Set");
    return(helm_report);
  }
  else
    bhv_set->resetStateOK();


  int i, bhv_ix;
  int bhv_cnt = bhv_set->getCount();

  vector<IvPFunction*> ofs;

  // get all the objective functions and add time info to helm report
  MBTimer create_timer;
  create_timer.start();
  for(bhv_ix=0; bhv_ix<bhv_cnt; bhv_ix++) {
    MBTimer of_timer;
    of_timer.start();
    bool idle;
    IvPFunction *newof = bhv_set->produceOF(bhv_ix, iteration, idle);
    of_timer.stop();
    
    if(!bhv_set->stateOK(bhv_ix)) {
      helm_report.m_halted = true;
      helm_report.addMsg("HELM HALTING: Safety Emergency!!!");
      return(helm_report);
    }

    string descriptor =  bhv_set->getDescriptor(bhv_ix);
    string report_line = descriptor;
    if(newof) {
      double of_time  = of_timer.get_float_cpu_time();
      int    pieces   = newof->getPDMap()->size();
      string timestr  = doubleToString(of_time,2);
      report_line += " produces obj-function - time:" + timestr;
      report_line += " pcs: " + doubleToString(pieces);
      report_line += " pwt: " + doubleToString(newof->getPWT());
    }
    else
      report_line += " did NOT produce an obj-function";
    
    if(newof) {
      double pwt = newof->getPWT();
      helm_report.addActiveBHV(descriptor, pwt);
    }
    if(!idle)
      helm_report.addNonIdleBHV(descriptor);
	
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

  ivp_problem->setDomain(sub_domain);
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
  
  int dsize = sub_domain.size();
  for(i=0; i<dsize; i++) {
    string dom_name = sub_domain.getVarName(i);
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
//              ivp_domain. Indicate result=false if not.
//            Build a modified ivp_domain that contains only 
//              those domain names found in one or more OF's.
//            If the modified ivp_domain has *no* domains, 
//              indicate result=false.

bool HelmEngine::checkOFDomains(vector<IvPFunction*> ofs)
{
  int i,j,k,vsize;

  // First build a vector of unique domain names strings from
  // all the objective functions.
  vector<string> of_domains;
  vsize = ofs.size();
  for(i=0; i<vsize; i++) {
    if(ofs[i]) {
      int dim = ofs[i]->getDim();
      for(j=0; j<dim; j++) {
	string dname = ofs[i]->getVarName(j);
	bool already_present = false;
	for(k=0; k<of_domains.size(); k++)
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

  vsize = of_domains.size();
  for(i=0; i<vsize; i++) {
    bool ok_domain = false;
    int count = ivp_domain.size();
    for(j=0; (!ok_domain && (j<count)); j++) {
      string dname = ivp_domain.getVarName(j);
      if(of_domains[i] == dname)
	ok_domain = true;
    }
    if(!ok_domain) {
      cout << "DomainVar " << of_domains[i] << " is not recognized ";
      cout << " by the IvPDomain configured to pHelmIvP" << endl;
      sub_domain = IvPDomain();
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

  if(vsize == ivp_domain.size()) {
    sub_domain = ivp_domain;
    return(true);
  }
  else {
    sub_domain = IvPDomain();
    for(int i=0; i<vsize; i++) {
      const char* dname = of_domains[i].c_str();
      int    index = ivp_domain.getIndex(dname);
      double dlow  = ivp_domain.getVarLow(index);
      double dhigh = ivp_domain.getVarHigh(index);
      int    dpoints = ivp_domain.getVarPoints(index);
      sub_domain.addDomain(dname, dlow, dhigh, dpoints);
    }
    return(true);
  }
}







