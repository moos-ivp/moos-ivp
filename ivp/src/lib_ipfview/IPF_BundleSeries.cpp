/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IPF_BundleSeries.cpp                                 */
/*    DATE: Apr 26th 2011                                        */
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
#include "IPF_BundleSeries.h"
#include "FunctionEncoder.h"
#include "MBUtils.h"
#include "BuildUtils.h"

using namespace std;

//--------------------------------------------------------------
// Constructor

IPF_BundleSeries::IPF_BundleSeries()
{
  m_min_iteration = 0;
  m_max_iteration = 0;
}

//--------------------------------------------------------------
// Procedure: clear()

void IPF_BundleSeries::clear() 
{
  m_bundles.clear();
  m_min_iteration = 0;
  m_max_iteration = 0;
}

//--------------------------------------------------------------
// Procedure: addIPF

void IPF_BundleSeries::addIPF(const std::string& str)
{
  string  context_string = StringToIvPContext(str);
  string       iter_str  = biteString(context_string, ':');
  unsigned int iteration = atoi(iter_str.c_str());

  if((iteration < m_min_iteration) || (m_bundles.size() == 0))
    m_min_iteration = iteration;
  if((iteration > m_max_iteration) || (m_bundles.size() == 0))
    m_max_iteration = iteration;

  map<unsigned int, IPF_Bundle>::iterator p;
  p = m_bundles.find(iteration);
  if(p == m_bundles.end())
    m_bundles[iteration] = IPF_Bundle();

  m_bundles[iteration].addIPF(str);


  // Maintain the local list of all known behavior sources
  vector<string> sources = m_bundles[iteration].getSources();
  m_all_bhv_sources = mergeVectors(m_all_bhv_sources, sources);
  m_all_bhv_sources = removeDuplicates(m_all_bhv_sources);
}


//--------------------------------------------------------------
// Procedure: getCollectiveQuadSet
//      Note: Typical values for ctype: "collective-depth", 
//            "collective-hdgspd"

QuadSet IPF_BundleSeries::getCollectiveQuadSet(unsigned int iter,
					       string ctype)
{
  return(m_bundles[iter].getCollectiveQuadSet(ctype));
}


//--------------------------------------------------------------
// Procedure: getQuadSet

QuadSet IPF_BundleSeries::getQuadSet(unsigned int iter, string src,
				     bool dense)
{
  return(m_bundles[iter].getQuadSet(src, dense));
}

//--------------------------------------------------------------
// Procedure: getDomain

IvPDomain IPF_BundleSeries::getDomain(unsigned int iter)
{
  return(m_bundles[iter].getDomain());
}


//--------------------------------------------------------------
// Procedure: getDomain

IvPDomain IPF_BundleSeries::getDomain(unsigned int iter, string src)
{
  return(m_bundles[iter].getDomain(src));
}


//--------------------------------------------------------------
// Procedure: getPriority

double IPF_BundleSeries::getPriority(unsigned int iter, string src)
{
  return(m_bundles[iter].getPriority(src));
}


//--------------------------------------------------------------
// Procedure: getPieces

unsigned int IPF_BundleSeries::getPieces(unsigned int iter, string src)
{
  return(m_bundles[iter].getPieces(src));
}

//--------------------------------------------------------------
// Procedure: getTotalFunctions

unsigned int IPF_BundleSeries::getTotalFunctions(unsigned int iter) 
{
  return(m_bundles[iter].size());
}

//--------------------------------------------------------------
// Procedure: getIPFStrings()

vector<string> IPF_BundleSeries::getIPFStrings(unsigned int iter)
{
  return(m_bundles[iter].getIPFStrings());
}

//--------------------------------------------------------------
// Procedure: getFirstSource
//      Note: A convenience functions. Often users of this class
//            just want a name of *any* source if there is one.

string IPF_BundleSeries::getFirstSource() const
{
  if(m_all_bhv_sources.size() > 0)
    return(m_all_bhv_sources[0]);
  else
    return("");
}



//--------------------------------------------------------------
// Procedure: popFront()

void IPF_BundleSeries::popFront(unsigned int amt) 
{
  if((amt == 0) || (m_bundles.size() == 0))
    return;

  if(amt >= m_bundles.size()) {
    clear();
    return;
  }
    
  unsigned int popped = 0;
  while(popped < amt) {
    if(m_bundles.count(m_min_iteration)) {
      m_bundles.erase(m_min_iteration);
      popped++;
    }   
    m_min_iteration++;
  }  
  
  // Catch error conditions
  if(m_min_iteration > m_max_iteration)
    cout << "WARNING: Stack popFront underflow!!!!" << endl;
}

//--------------------------------------------------------------
// Procedure: popBack

void IPF_BundleSeries::popBack(unsigned int amt) 
{
  if((amt == 0) || (m_bundles.size() == 0))
    return;

  if(amt >= m_bundles.size()) {
    clear();
    return;
  }
    
  unsigned int popped = 0;
  while(popped < amt) {
    if(m_bundles.count(m_max_iteration)) {
      m_bundles.erase(m_max_iteration);
      popped++;
    }   
    m_max_iteration--;
  }  
  
  // Catch error conditions
  if(m_min_iteration > m_max_iteration)
    cout << "WARNING: Stack popBack underflow!!!!" << endl;
}

//--------------------------------------------------------------
// Procedure: print()

void IPF_BundleSeries::print() const
{
  cout << "Bundle Series:   " << endl;
  cout << "  Size:          " << m_bundles.size() << endl;;
  cout << "  Min iteration: " << m_min_iteration << endl;
  cout << "  Max iteration: " << m_max_iteration << endl;
}








