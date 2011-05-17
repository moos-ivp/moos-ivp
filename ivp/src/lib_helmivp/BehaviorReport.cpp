/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: BehaviorReport.cpp                                   */
/*    DATE: May 10th 2011                                        */
/*****************************************************************/

#include "BehaviorReport.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

BehaviorReport::BehaviorReport()
{
  m_keys_unique = true;
}

//-----------------------------------------------------------
// Procedure: addIPF
//      Note: Returns true if the given key is unique

bool BehaviorReport::addIPF(IvPFunction *ipf, string key)
{
  if(vectorContains(m_keys, key))
    m_keys_unique = false;

  m_keys.push_back(key);
  m_ipfs.push_back(ipf);
  
  return(m_keys_unique);
}

//-----------------------------------------------------------
// Procedure: getIPF(unsigned int index)

IvPFunction *BehaviorReport::getIPF(unsigned int index) const
{
  if(index >= m_ipfs.size())
    return(0);  
  return(m_ipfs[index]);
}

//-----------------------------------------------------------
// Procedure: getIPF(string key)

IvPFunction *BehaviorReport::getIPF(string key) const
{
  unsigned int i, vsize = m_ipfs.size();
  for(i=0; i<vsize; i++) 
    if(m_keys[i] == key)
      return(m_ipfs[i]);
  return(0);
}

// Note - in the Behavior Registry - clear out any stored IPF's not
// explicitly vouched for on any given iteration. This way we don't
// have to worry that a key augmented here will result in a runaway
// growing registry.
