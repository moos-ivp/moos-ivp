/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BehaviorReport.cpp                                   */
/*    DATE: May 10th 2011                                        */
/*                                                               */
/* (IvPHelm) The IvP autonomous control Helm is a set of         */
/* classes and algorithms for a behavior-based autonomous        */
/* control architecture with IvP action selection.               */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
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

