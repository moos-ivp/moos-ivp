/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IvPChecker.cpp                                       */
/*    DATE: April 18th, 2011                                     */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include "IvPProblem.h"
#include "MBUtils.h"
#include "IvPChecker.h"
#include "FunctionEncoder.h"
#include "DomainExpander.h"

using namespace std;

//--------------------------------------------------------
// Procedure: checkIvP
//     Notes: 

bool IvPChecker::checkIvP(const vector<string>& ipf_strings)
{
  buildIvPFunctions(ipf_strings);
  buildIvPDomain();

  IvPProblem ivp_problem;
  ivp_problem.setDomain(m_ivp_domain);

  unsigned int i, ipfs = m_ivp_functions.size();
  for(i=0; i<ipfs; i++) 
    ivp_problem.addOF(m_ivp_functions[i]);

  ivp_problem.alignOFs();
  ivp_problem.solve();

  cout << "IvP Search Results: " << endl;
  cout << "  Best Value: " << ivp_problem.getMaxWT() << endl;
  unsigned int dsize = m_ivp_domain.size();
  for(i=0; i<dsize; i++) {
    string dom_name = m_ivp_domain.getVarName(i);
    double decision = ivp_problem.getResult(dom_name);
    cout << "Max for variable " << dom_name << " is: " << decision << endl;
  }    
  cout << endl;

  m_ivp_functions.clear();
  return(true);
}

//--------------------------------------------------------
// Procedure: checkBrute
//     Notes: 

void IvPChecker::checkBrute(const vector<string>& ivp_strings)
{
  buildIvPFunctions(ivp_strings);
  buildIvPDomain();

  DomainExpander dex;
  dex.setIvPDomain(m_ivp_domain);

  bool   no_point_evaluated = true;
  double max_so_far;
  double min_so_far;
  IvPBox max_pt;

  while(dex.isFullyExpanded() == false) {
    IvPBox ptbox = dex.getCurrDomainPointBox();    

    unsigned int i, ipfs = m_ivp_functions.size();
    double ptval = 0;
    for(i=0; i<ipfs; i++) {
      double pwt = m_ivp_functions[i]->getPWT();
      PDMap *pdmap = m_ivp_functions[i]->getPDMap();
      ptval += (pdmap->evalPoint(&ptbox) * pwt);
    }

    if(no_point_evaluated || (ptval > max_so_far)) {
      max_so_far = ptval;
      max_pt = ptbox;
    }

    if(no_point_evaluated || (ptval < min_so_far))
      min_so_far = ptval;

    no_point_evaluated = false;
  }

  cout << "Brute Force Search Results:" << endl;
  cout << "  Max value: " << max_so_far << endl;
  cout << "  Min value: " << min_so_far << endl;
  cout << "  Max Point: " << endl;
  unsigned int d, dsize = max_pt.getDim();
  for(d=0; d<dsize; d++) {
    unsigned int sol_index = max_pt.pt(d);
    cout << m_ivp_domain.getVarName(d) << ": ";
    cout << m_ivp_domain.getVal(d, sol_index) << endl;
  }
  cout << endl;
  clearIvPFunctions();
}

//--------------------------------------------------------
// Procedure: checkCollectiveQuadSet()
//     Notes: 

void IvPChecker::checkCollectiveQuadSet(const QuadSet& collective_quadset)
{
  collective_quadset.print();
}

//--------------------------------------------------------
// Procedure: buildIvPFunctions(vector<string>)
//     Notes: 

void IvPChecker::buildIvPFunctions(const vector<string>& ipf_strings)
{
  unsigned int k, ksize = ipf_strings.size();
  
  for(k=0; k<ksize; k++) {
    IvPFunction *ipf = StringToIvPFunction(ipf_strings[k]);
    if(ipf)
      m_ivp_functions.push_back(ipf);
  }
}


//--------------------------------------------------------
// Procedure: clearIvPFunctions()
//     Notes: 

void IvPChecker::clearIvPFunctions()
{
  unsigned int k, ipfs = m_ivp_functions.size();
  for(k=0; k<ipfs; k++) {
    if(m_ivp_functions[k]) {
      delete(m_ivp_functions[k]);
      m_ivp_functions[k] = 0;
    }
  }
  m_ivp_functions.clear();
}


//--------------------------------------------------------
// Procedure: buildIvPDomain
//     Notes: 

void IvPChecker::buildIvPDomain()
{
  unsigned int j, jsize = m_ivp_functions.size();
  for(j=0; j<jsize; j++) {
    if(m_ivp_functions[j]) {
      IvPDomain domain = m_ivp_functions[j]->getPDMap()->getDomain();
      unsigned int k, ksize = domain.size();
      for(k=0; k<ksize; k++) {
	if(!m_ivp_domain.hasDomain(domain.getVarName(k)))
	  m_ivp_domain.addDomain(domain, domain.getVarName(k));
      }
    }
  }

  cout << "IvPChecker::buildIvPDomain() IvP Domain:" << endl;
  m_ivp_domain.print();


  for(j=0; j<jsize; j++)
    m_ivp_functions[j]->transDomain(m_ivp_domain);
}

