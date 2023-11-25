/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BehaviorRegistry.cpp                                 */
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

#include "BehaviorRegistry.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

BehaviorReport::BehaviorReport()
{
  m_keys_unique      = true;
  m_latest_iteration = 0;
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


/*




[waypoint:course]    [45] [ipf*] [true]
[waypoint:speed]     [45] [ipf*] [true]
[const_depth:depth]  [45] [ipf*] [false]
[const_depth:depth]  [45] [ipf*] [false]
[coll_avd_berta]     [45] [ipf*] [true]

The whole idea of the registry is to convey information back to 
the behaviors: "by the way I know something about an IPF you sent
on the previous iteration with the tag xyz, you know, just in case
you want me to just re-use that one this time."


Assumptions enforced by the helm:
(1) All behaviors have a unique name or key (colons not allowed)
(2) Behaviors can enforce all IPFs have a unique name by using
    the behavior name + ":" + unique_key


Another reason we want IPFs to have unique tags, (besides for the 
registry), we want alogview and uFunctionVis to discern them.



*/










