/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: RealmRepo.cpp                                        */
/*    DATE: Dec 4th 2020                                         */
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
#include <iterator>
#include "MBUtils.h"
#include "RealmRepo.h"

using namespace std;

//---------------------------------------------------------
// Constructor

RealmRepo::RealmRepo()
{
  m_refresh_mode = "events";
}

//---------------------------------------------------------
// Procedure: report()
//   Purpose: Return a human-readable summary of the repo

string RealmRepo::report() const
{
  return(m_realm_tree.report());
}


//---------------------------------------------------------
// Procedure: addNodeSummary()

bool RealmRepo::addRealmSummary(RealmSummary summary)
{
  return(m_realm_tree.addRealmSummary(summary));
}

//---------------------------------------------------------
// Procedure: addRealmCast()
//   Returns: true if first time hearing from this node

bool RealmRepo::addRealmCast(const string& relcast_str)
{
  RealmCast relcast = string2RealmCast(relcast_str);
  return(addRealmCast(relcast));
}

//---------------------------------------------------------
// Procedure: addRealmCast()
//   Returns: true if first time hearing from this node

bool RealmRepo::addRealmCast(const RealmCast& relcast)
{
  return(m_realm_tree.addRealmCast(relcast));
}


//---------------------------------------------------------
// Procedure: removeNode()

bool RealmRepo::removeNode(string node_str)
{
  return(m_realm_tree.removeNode(node_str));
}


//---------------------------------------------------------
// Procedure: setCurrentNode()
//   Returns: true if node is valid 
//      Note: Keep track if this represents a change in the
//            node. Users of the repo may want to act faster
//            on a refresh action if there has been a change

bool RealmRepo::setCurrentNode(string node_str)
{
  if(m_realm_tree.getCurrentNode() != node_str)
    m_node_proc_changed = true;
  
  return(m_realm_tree.setCurrentNode(node_str));
}

//---------------------------------------------------------
// Procedure: setRefreshMode()
//   Returns: true if the given mode is a valid mode name

bool RealmRepo::setRefreshMode(string mode_str)
{
  string mode = tolower(mode_str);
  if((mode == "streaming") || (mode == "events") || (mode == "paused")) {
    m_refresh_mode = mode;
    return(true);
  }
  return(false);
}

//---------------------------------------------------------
// Procedure: setCurrentProc()
//   Returns: true if given str names an proc known to this node, or if it
//            names an ID mapped to a proc known to this node, 
//      Note: The given key may be an Proc name, or an ID mapped to an Proc

bool RealmRepo::setCurrentProc(string proc_str)
{
  if(m_realm_tree.getCurrentProc() != proc_str)
    m_node_proc_changed = true;

  return(m_realm_tree.setCurrentProc(proc_str));
}

//---------------------------------------------------------
// Procedure: getCurrentNodes()

vector<string> RealmRepo::getCurrentNodes() const
{
  vector<string> rvector;
  rvector = m_realm_tree.getNodes();
  return(rvector);
}

//---------------------------------------------------------
// Procedure: getCurrentProcs()

vector<string> RealmRepo::getCurrentProcs() const
{
  return(m_realm_tree.getProcs());
}

//---------------------------------------------------------
// Procedure: getNodeCount()

unsigned int RealmRepo::getNodeCount() const
{
  return(m_realm_tree.getTreeNodeCount());
}

//---------------------------------------------------------
// Procedure: getProcCount()

unsigned int RealmRepo::getProcCount() const
{
  return(m_realm_tree.getTreeProcCount());
}

//---------------------------------------------------------
// Procedure: getProcCount()

unsigned int RealmRepo::getProcCount(string node) const
{
  return(m_realm_tree.getNodeTotalProcCount(node));
}

//---------------------------------------------------------
// Procedure: getRealmCastCount()

unsigned int RealmRepo::getRealmCastCount(string node, string proc) const
{
  return(m_realm_tree.getProcRealmCastCount(node, proc));
}

//---------------------------------------------------------
// Procedure: getRealmCastCount()

unsigned int RealmRepo::getRealmCastCount(string node) const
{
  return(m_realm_tree.getNodeRealmCastCount(node));
}

//---------------------------------------------------------
// Procedure: getNodeProcChanged()

bool RealmRepo::getNodeProcChanged()
{
  bool changed = m_node_proc_changed;
  m_node_proc_changed = false;
  return(changed);
}
