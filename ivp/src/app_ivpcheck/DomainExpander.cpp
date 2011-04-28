/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: DomainExpander.cpp                                   */
/*    DATE: April 18th, 2011                                     */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "DomainExpander.h"
#include "BuildUtils.h"

using namespace std;

//--------------------------------------------------------
// Constructor

DomainExpander::DomainExpander()
{
  m_fully_expanded = false;
}

//--------------------------------------------------------
// Procedure: setIvPDomain
//     Notes: 

void DomainExpander::setIvPDomain(IvPDomain domain)
{
  m_ivp_domain = domain;
  
  m_current_point.clear();
  unsigned int k, ksize = domain.size();
  for(k=0; k<ksize; k++)
    m_current_point.push_back(0);
}


//--------------------------------------------------------
// Procedure: setIvPDomain
//     Notes: 

void DomainExpander::setIvPDomain(string domain_str)
{
  IvPDomain new_domain = stringToDomain(domain_str);
  
  if(new_domain.size() == 0)
    return;
  else
    setIvPDomain(new_domain);
}


//--------------------------------------------------------
// Procedure: getCurrDomainPointBox
//     Notes: 

IvPBox DomainExpander::getCurrDomainPointBox(bool increment)
{
  unsigned int k, ksize = m_current_point.size();
  IvPBox retbox(ksize, 1);
  for(k=0; k<ksize; k++)
    retbox.setPTS(k, m_current_point[k], m_current_point[k]);
  
  if(increment)
    incNextDomainPoint();
  
  return(retbox);
}


//--------------------------------------------------------
// Procedure: incNextDomainPoint
//     Notes: 

void DomainExpander::incNextDomainPoint(bool verbose)
{
  unsigned int k, ksize = m_current_point.size(); 
  
  bool incremented = false;
  for(k=0; (k<ksize) && !incremented; k++) {
    unsigned int kd_size = m_ivp_domain.getVarPoints(k);
    
    if(m_current_point[k] < kd_size) {
      m_current_point[k]++;
      incremented = true;
    }
    else
      m_current_point[k] = 0;
  }

  if(!incremented)
    m_fully_expanded = true;

  if(!verbose)
    return;

  cout << "Val: ";
  for(k=0; k<ksize; k++) {
    if(k>0)
      cout << ",";
    cout << m_current_point[k];
  }
  cout << endl;
}

