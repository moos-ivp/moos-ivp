/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: BehaviorRegistry.cpp                                 */
/*    DATE: May 10th 2011                                        */
/*****************************************************************/

#include "BehaviorRegistry.h"
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

bool BehaviorRegistry::addIPF(IvPFunction *ipf, 
			      unsigned int iter, 
			      string key)
{
  if(vectorContains(m_keys, key))
    m_keys_unique = false;

  m_keys.push_back(key);
  m_ipfs.push_back(ipf);
  
  return(m_keys_unique);
}


#if 0

Need to have a RegistrySummary class. This 
