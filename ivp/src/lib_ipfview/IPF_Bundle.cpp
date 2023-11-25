/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: IPF_Bundle.cpp                                       */
/*    DATE: Apr 19th 2011                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "IPF_Bundle.h"
#include "FunctionEncoder.h"
#include "MBUtils.h"
#include "BuildUtils.h"

using namespace std;

//--------------------------------------------------------------
// Procedure: addIPF

void IPF_Bundle::addIPF(const std::string& str)
{
  string context_string = StringToIvPContext(str);

  IvPDomain domain = IPFStringToIvPDomain(str);

  string iter_str = biteString(context_string, ':');
  string behavior = context_string;

  unsigned int iteration = atoi(iter_str.c_str());

  if(m_entries.size() == 0) {
    m_entries.push_back(str);
    m_sources.push_back(behavior);
    m_iteration = iteration;
    m_ivp_domain = domain;
    return;
  }

  if(m_iteration != iteration)
    return;
  if(vectorContains(m_sources, behavior))
    return;

  m_entries.push_back(str);
  m_sources.push_back(behavior);
  m_ivp_domain = unionDomain(m_ivp_domain, domain);
}

//--------------------------------------------------------------
// Procedure: getCollectiveQuadSet()

QuadSet IPF_Bundle::getCollectiveQuadSet(string ctype)
{
  QuadSet empty_qset;

  unsigned int k, ksize = m_entries.size();
  if(ksize == 0)
    return(empty_qset);
  
  QuadSet return_qset;
  if(ctype == "collective-depth") {
    for(k=0; k<ksize; k++) {
      QuadSet quad_set_k = m_entries[k].getQuadSet(m_ivp_domain);
      IvPDomain domain = quad_set_k.getDomain();
      if(domain.hasOnlyDomain("depth"))
	return_qset.addQuadSet(quad_set_k);
    }
  }
  if(ctype == "collective-hdgspd") {
    for(k=0; k<ksize; k++) {
      QuadSet quad_set_k = m_entries[k].getQuadSet(m_ivp_domain);
      IvPDomain domain = quad_set_k.getDomain();
      if(!domain.hasOnlyDomain("depth"))
	return_qset.addQuadSet(quad_set_k);
    }
  }

  return(return_qset);
}

//--------------------------------------------------------------
// Procedure: getQuadSet(index)

QuadSet IPF_Bundle::getQuadSet(unsigned int index, bool dense)
{
  if(index < m_entries.size())
    return(m_entries[index].getQuadSet(m_ivp_domain, dense));

  QuadSet empty_qset;
  return(empty_qset);
}

//--------------------------------------------------------------
// Procedure: getQuadSet(source)

QuadSet IPF_Bundle::getQuadSet(string source, bool dense)
{
  unsigned int i, vsize = m_sources.size();
  for(i=0; i<vsize; i++) 
    if(m_sources[i] == source)
      return(m_entries[i].getQuadSet(m_ivp_domain, dense));
  
  QuadSet empty_qset;
  return(empty_qset);
}

//--------------------------------------------------------------
// Procedure: getPieces(source)

unsigned int IPF_Bundle::getPieces(string source) const
{
  unsigned int i, vsize = m_sources.size();
  for(i=0; i<vsize; i++) {
    if(m_sources[i] == source)
      return(m_entries[i].getPieces());
  }

  return(0);
}

//--------------------------------------------------------------
// Procedure: getPriority(source)

double IPF_Bundle::getPriority(string source) const
{
  unsigned int i, vsize = m_sources.size();
  for(i=0; i<vsize; i++) 
    if(m_sources[i] == source)
      return(m_entries[i].getPriority());

  return(0);
}

//--------------------------------------------------------------
// Procedure: getDomain(source)

IvPDomain IPF_Bundle::getDomain(string source) const
{
  unsigned int i, vsize = m_sources.size();
  for(i=0; i<vsize; i++) 
    if(m_sources[i] == source)
      return(m_entries[i].getDomain());

  IvPDomain empty_domain;
  return(empty_domain);
}

//--------------------------------------------------------------
// Procedure: getSource()

string IPF_Bundle::getSource(unsigned int index)
{
  if(index >= m_entries.size())
    return("");
  else
    return(m_sources[index]);
      
}

//--------------------------------------------------------------
// Procedure: getIPFString()

string IPF_Bundle::getIPFString(unsigned int index)
{
  if(index >= m_entries.size())
    return("");
  else
    return(m_entries[index].getIPFString());
}


//--------------------------------------------------------------
// Procedure: getIPFStrings()

vector<string> IPF_Bundle::getIPFStrings()
{
  vector<string> ipf_strings;
  
  unsigned int i, vsize = m_entries.size();
  for(i=0; i<vsize; i++) {
    string ipf_string = m_entries[i].getIPFString();
    if(ipf_string != "")
      ipf_strings.push_back(ipf_string);
  }

  return(ipf_strings);
}










