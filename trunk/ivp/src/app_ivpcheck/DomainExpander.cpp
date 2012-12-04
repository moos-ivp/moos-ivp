/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: DomainExpander.cpp                                   */
/*    DATE: April 18th, 2011                                     */
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



