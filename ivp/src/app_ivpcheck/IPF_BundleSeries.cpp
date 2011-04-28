/*****************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J.Leonard                     */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: IPF_BundleSeries.cpp                                 */
/*    DATE: Apr 26th 2011                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
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
}


//--------------------------------------------------------------
// Procedure: getCollectiveQuadSet

QuadSet IPF_BundleSeries::getCollectiveQuadSet(unsigned int iter)
{
  return(m_bundles[iter].getCollectiveQuadSet());
}


//--------------------------------------------------------------
// Procedure: getQuadSet

QuadSet IPF_BundleSeries::getQuadSet(unsigned int iter, string src)
{
  return(m_bundles[iter].getQuadSet(src));
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
