/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BehaviorReport.cpp                                   */
/*    DATE: May 10th 2011                                        */
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

#include "BehaviorReport.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "FunctionEncoder.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

BehaviorReport::BehaviorReport()
{
  m_iteration = 0;
  m_priority  = 0;
}

BehaviorReport::BehaviorReport(string bhv_name, unsigned int iter)
{
  m_bhv_name  = bhv_name;
  m_iteration = iter;
  m_priority  = 0;
}

//-----------------------------------------------------------
// Procedure: addIPF
//      Note: Returns true if the given key is unique

void BehaviorReport::addIPF(IvPFunction *ipf, string key)
{
  if(ipf && (ipf->getPWT() > 0)) {
    m_key.push_back(key);
    m_ipf.push_back(ipf);
    m_ipf_string.push_back("");
    m_key_unique.push_back(false);
  }
}

//-----------------------------------------------------------
// Procedure: setPriority

void BehaviorReport::setPriority(double pwt)
{
  if(pwt >= 0)
    m_priority = pwt;
  else
    m_priority = 0;
}

//-----------------------------------------------------------
// Procedure: makeKeysUnique

void BehaviorReport::makeKeysUnique()
{
  unsigned int i, j, vsize = m_key.size();
  // Pass 0 - In the special case where there is only one IPF, its
  // key is by definition unique, even if the key is the empty string.
  if(vsize == 1) {
    m_key_unique[0] = true;
    return;
  }

  // Pass 1 - If the key is already unique, mark it so. Even when
  // we make non-unique keys unique in later passes, we do not mark
  // them as unique since they weren't unique coming from the 
  // behavior.
  for(i=0; i<vsize; i++) {
    if(m_key[i] != "") {
      bool match = false;
      for(j=0; j<vsize; j++) {
	if((i!=j) && (m_key[i] == m_key[j]))
	  match = true;
      }
      if(!match)
	m_key_unique[i] = true;
    }
  }
  
  // Pass 2 - If the key is empty, give it a key based on domain
  // For example "depth", "course:speed"
  for(i=0; i<vsize; i++) {
    if(m_key[i] == "") {
      IvPDomain domain = m_ipf[i]->getPDMap()->getDomain();
      m_key[i] = domainToString(domain, false);
    }
  }

  // Pass 3 - For all keys that are still not unique, make them unique
  // by adding suffixes that numerically increment
  // Example:
  // [0] apples    -> apples_0
  // [1] pears     -> pears_0
  // [2] pears     -> pears_1
  // [3] apples    -> apples_1
  // [4] pears     -> pears_2

  // Even though all keys should be unique by the end of the pass, 
  // only keys that were originally non-empty and unique are marked
  // as such in the m_key_unique vector.

  vector<bool> key_unique = m_key_unique;
  for(i=0; i<vsize; i++) {
    if(!key_unique[i]) {
      bool match = false;
      unsigned int suffix = 0;
      for(j=i+1; j<vsize; j++) {
	if(m_key[i] == m_key[j]) {
	  suffix++;
	  m_key[j] += "_" + uintToString(suffix);
	  key_unique[j] = true;
	  match = true;
	}
      }
      key_unique[i] = true;
      if(match)
	m_key[i] += "_0";
    }
  }
}

//-----------------------------------------------------------
// Procedure: checkForNans

bool BehaviorReport::checkForNans()
{
  bool nan_detected = false;
  unsigned int i, vsize = m_ipf.size();
  for(i=0; i<vsize; i++) {
    if(m_ipf[i] && !m_ipf[i]->freeOfNan())
      nan_detected = true;
  }
  return(nan_detected);
}

//-----------------------------------------------------------
// Procedure: setIPFStrings

void BehaviorReport::setIPFStrings()
{
  unsigned int i, vsize = m_ipf.size();
  for(i=0; i<vsize; i++) {
    if(m_ipf[i]) {
      string context_string = uintToString(m_iteration) + ":";
      context_string += m_bhv_name;
      if(m_key[i] != "")
	context_string += ":" + m_key[i];
      m_ipf[i]->setContextStr(context_string);
      m_ipf_string[i] = IvPFunctionToString(m_ipf[i]);
    }
  }
}


//-----------------------------------------------------------
// Procedure: getIPF(string key)

IvPFunction *BehaviorReport::getIPF(string key) const
{
  unsigned int i, vsize = m_ipf.size();
  for(i=0; i<vsize; i++) 
    if(m_key[i] == key)
      return(m_ipf[i]);
  return(0);
}

//-----------------------------------------------------------
// Procedure: getIPF(unsigned int index)

IvPFunction *BehaviorReport::getIPF(unsigned int index) const
{
  if(index >= m_ipf.size())
    return(0);  
  return(m_ipf[index]);
}

//-----------------------------------------------------------
// Procedure: getKey(unsigned int index)

string BehaviorReport::getKey(unsigned int index) const
{
  if(index >= m_key.size())
    return("");  
  return(m_key[index]);
}

//-----------------------------------------------------------
// Procedure: getIPFString(unsigned int index)

string BehaviorReport::getIPFString(unsigned int index) const
{
  if(index >= m_ipf_string.size())
    return("");  
  return(m_ipf_string[index]);
}

//-----------------------------------------------------------
// Procedure: hasIPFString(unsigned int index)

bool BehaviorReport::hasIPFString(unsigned int index) const
{
  if(index >= m_ipf_string.size())
    return(false);  
  return(m_ipf_string[index].length() > 0);
}

//-----------------------------------------------------------
// Procedure: hasUniqueKey

bool BehaviorReport::hasUniqueKey(unsigned int index) const
{
  if(index >= m_key_unique.size())
    return(false);  
  return(m_key_unique[index]);
}

//-----------------------------------------------------------
// Procedure: getAvgPieces

double BehaviorReport::getAvgPieces() const
{
  unsigned int i, vsize = m_ipf.size();
  if(vsize == 0)
    return(0);

  double count = 0;
  for(i=0; i<vsize; i++) 
    if(m_ipf[i])
      count += m_ipf[i]->getPDMap()->size();

  return(count / (double)(vsize));
}


// Note - in the Behavior Registry - clear out any stored IPF's not
// explicitly vouched for on any given iteration. This way we don't
// have to worry that a key augmented here will result in a runaway
// growing registry.










