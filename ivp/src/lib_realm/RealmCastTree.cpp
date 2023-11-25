/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RealmCastTree.cpp                                    */
/*    DATE: Dec 4th 2020                                         */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include <iterator>
#include "MBUtils.h"
#include "RealmCastTree.h"

using namespace std;

//---------------------------------------------------------
// Constructor

RealmCastTree::RealmCastTree()
{
  m_total_relcast_count = 0;
  m_node_id_count = 0;
}

//---------------------------------------------------------
// Procedure: report()

string RealmCastTree::report() const
{
  string str;
  map<string, RealmCastSet>::const_iterator p;
  for(p=m_map_relcast_sets.begin(); p!=m_map_relcast_sets.end(); p++) {
    string node = p->first;
    RealmCastSet rset = p->second;
    if(str != "")
      str += "##";
    str += "node=" + node + "," + rset.report();
  }
  return(str);
}


//---------------------------------------------------------
// Procedure: addRealmSummary()
//   Example: node=alpha # channels=pHelmIvP,pNodeReporter,...

bool RealmCastTree::addRealmSummary(RealmSummary summary,
				    string onstart_node,
				    string onstart_proc)
{
  if(!summary.valid())
    return(false);

  string node = summary.getNode();

  // Handle if node has not yet been encountered
  if(m_map_relcast_sets.count(node) == 0) {
    if(!vectorContains(m_nodes, node))
      m_nodes.push_back(node);
    setNewNodeID(node);
  }

  bool ok = m_map_relcast_sets[node].addRealmSummary(summary, onstart_proc);

  // Normally we dont switch the current_node upon an incoming summary,
  // except for these two cases:

  string old = m_current_node;
  
  if((onstart_node != "") && (node == onstart_node))
    m_current_node = node;
  if(m_current_node == "") 
    m_current_node = node;

  return(ok);
}

//---------------------------------------------------------
// Procedure: addRealmCast()
//   Returns: true if first time hearing from this node

bool RealmCastTree::addRealmCast(const string& str)
{
  RealmCast relcast = string2RealmCast(str);
  return(addRealmCast(relcast));
}

//---------------------------------------------------------
// Procedure: addRealmCast()
//   Returns: true. May add some error checking in the future

bool RealmCastTree::addRealmCast(const RealmCast& relcast)
{
  m_total_relcast_count++;

  string  relcast_node_name = relcast.getNodeName();
  if(relcast_node_name == "")
    relcast_node_name = "unknown_node";

  // First determine if the node_name has been previously encountered.
  bool new_node = false;
  if(m_map_relcast_sets.count(relcast_node_name) == 0)
    new_node = true;


  // Now update the node (or create new node channelset if need be)
  m_map_relcast_sets[relcast_node_name].addRealmCast(relcast);

  // If we've heard from this node before, our job is done.
  if(!new_node)
    return(true);
  else
    m_nodes.push_back(relcast_node_name);

  setNewNodeID(relcast_node_name);

  return(true);
}

//---------------------------------------------------------
// Procedure: setNewNodeID()
//      Note: Terminal based apps will need to use single chars
//            to select a node. Likely not used in GUI apps.

void RealmCastTree::setNewNodeID(string node_str)
{
  if(node_str == "shoreside")
    m_map_id_node["0"] = node_str;
  else {
    char c = 97 + (int)(m_node_id_count);  // 'a' + cnt
    m_node_id_count++;
    if((c=='e') || (c=='h') || (c=='p') || (c=='r') || (c=='n')) {
      c++;
      m_node_id_count++;
    }
    
    string id(1,c);
    m_map_id_node[id] = node_str;
  }
}

//---------------------------------------------------------
// Procedure: removeNode()
//   Returns: true if node is already known to the repo.

bool RealmCastTree::removeNode(const string& node)
{
  if(!hasNode(node))
    return(false);
  
  // Step 1
  m_map_relcast_sets.erase(node);
  
  // Step 2
  string id;
  map<string, string>::iterator p;
  for(p=m_map_id_node.begin(); p!=m_map_id_node.end(); p++) {
    string this_id   = p->first;
    string this_node = p->second;
    if(this_node == node) 
      id = this_id;
  }
  if(id != "")
    m_map_id_node.erase(id);

  // Step 3
  vector<string> new_nodes;
  for(unsigned int i=0; i<m_nodes.size(); i++) {
    if(m_nodes[i] != node)
      new_nodes.push_back(m_nodes[i]);
  }
  m_nodes = new_nodes;

  // Step 4
  m_current_node = "";
  if(m_nodes.size() != 0)
    m_current_node = m_nodes[0];
  
  return(true);
}

//---------------------------------------------------------
// Procedure: hasNode(node)
//   Returns: true if node is already known to the repo.

bool RealmCastTree::hasNode(string node) const
{
  return(m_map_relcast_sets.count(node) > 0);
}


//---------------------------------------------------------
// Procedure: getTreeProcCount()
//   Returns: Number of known procs for all nodes

unsigned int RealmCastTree::getTreeProcCount() const
{
  unsigned int total = 0;
  map<string, RealmCastSet>::const_iterator p;
  for(p=m_map_relcast_sets.begin(); p!=m_map_relcast_sets.end(); p++) {
    total += p->second.getTotalProcCount();
  }

  return(total);
}

//---------------------------------------------------------
// Procedure: getNodeIds()
//   Returns: list of known nodes

vector<string> RealmCastTree::getNodeIDs() const
{
  vector<string> rvector;

  map<string, string>::const_iterator p;
  for(p=m_map_id_node.begin(); p!=m_map_id_node.end(); p++) {
    string id = p->first;
    rvector.push_back(id);
  }

  return(rvector);
}

//---------------------------------------------------------
// Procedure: getProcs()

vector<string> RealmCastTree::getProcs() const
{
  return(getProcs(m_current_node));
}


//---------------------------------------------------------
// Procedure: getProcs(node)
//   Returns: list of known procs for the given node

vector<string> RealmCastTree::getProcs(string node) const
{
  map<string, RealmCastSet>::const_iterator p = m_map_relcast_sets.find(node);
  if(p == m_map_relcast_sets.end()) {
    vector<string> empty_vector;
    return(empty_vector);
  }
  else
    return(p->second.getProcs());
}


//---------------------------------------------------------
// Procedure: getProcIDs(node)
//   Returns: list of known proc IDs for the given node

vector<string> RealmCastTree::getProcIDs(string node) const
{
  map<string, RealmCastSet>::const_iterator p = m_map_relcast_sets.find(node);
  if(p == m_map_relcast_sets.end()) {
    vector<string> empty_vector;
    return(empty_vector);
  }
  else
    return(p->second.getIDs());
}

 
//---------------------------------------------------------
// Procedure: getNodeNameFromID(id)
//   Returns: Name of the node given the id, or empty string if id unknown

string RealmCastTree::getNodeNameFromID(string id) const
{
  map<string, string>::const_iterator p = m_map_id_node.find(id);
  if(p == m_map_id_node.end())
    return("");
  else
    return(p->second);  
}

//---------------------------------------------------------
// Procedure: getNodeRealmCastCount(node)
//   Returns: Number of relcasts received for all procs for this node

unsigned int RealmCastTree::getNodeRealmCastCount(string node) const
{
  map<string, RealmCastSet>::const_iterator p = m_map_relcast_sets.find(node);
  if(p == m_map_relcast_sets.end())
    return(0);
  else
    return(p->second.getTotalRealmCastCount());  
}

//---------------------------------------------------------
// Procedure: getNodeTotalProcCount(node)
//   Returns: Number of run warnings for all channels for this node

unsigned int RealmCastTree::getNodeTotalProcCount(string node) const
{
  map<string, RealmCastSet>::const_iterator p = m_map_relcast_sets.find(node);
  if(p == m_map_relcast_sets.end())
    return(0);
  else
    return(p->second.getTotalProcCount());  
}

//---------------------------------------------------------
// Procedure: hasNodeProc(node, proc)
//   Returns: true if node with proc is already known to the repo.

bool RealmCastTree::hasNodeProc(string node, string proc) const
{
  map<string, RealmCastSet>::const_iterator p = m_map_relcast_sets.find(node);
  if(p == m_map_relcast_sets.end())
    return(false);
  else
    return(p->second.hasProc(proc));  
}

//---------------------------------------------------------
// Procedure: getProcNameFromID()
//   Returns: Name of the proc, given the node name and process id

string RealmCastTree::getProcNameFromID(string node, string id) const
{
  map<string, RealmCastSet>::const_iterator p = m_map_relcast_sets.find(node);
  if(p == m_map_relcast_sets.end())
    return("");
  else
    return(p->second.getProcNameFromID(id));
}

//---------------------------------------------------------
// Procedure: getProcRealmCastCount()
//   Returns: Number of relcasts received by the given proc and node.

unsigned int RealmCastTree::getProcRealmCastCount(string node, string proc) const
{

  map<string, RealmCastSet>::const_iterator p = m_map_relcast_sets.find(node);
  if(p == m_map_relcast_sets.end())
    return(0);
  else
    return(p->second.getRealmCastCount(proc));
}

//---------------------------------------------------------
// Procedure: getRealmCast()

RealmCast RealmCastTree::getRealmCast(string node, string proc) const
{
  map<string, RealmCastSet>::const_iterator p = m_map_relcast_sets.find(node);
  if(p == m_map_relcast_sets.end()) {
    RealmCast null_relcast;
    return(null_relcast);
  }
  else
    return(p->second.getRealmCast(proc));
}

//---------------------------------------------------------
// Procedure: setCurrentNode()

bool RealmCastTree::setCurrentNode(string node)
{
  if(!vectorContains(m_nodes, node))
    return(false);

  m_current_node = node;
  return(true);
}

//---------------------------------------------------------
// Procedure: getCurrentProc()

string RealmCastTree::getCurrentProc() const
{
  if(m_current_node == "")
    return("");
  
  map<string, RealmCastSet>::const_iterator p;
  p = m_map_relcast_sets.find(m_current_node);
  if(p == m_map_relcast_sets.end()) 
    return("");
  else
    return(p->second.getCurrentProc());
}


//---------------------------------------------------------
// Procedure: setCurrentProc()

bool RealmCastTree::setCurrentProc(string proc)
{
  if(m_current_node == "")
    return(false);

  if(m_map_relcast_sets.count(m_current_node) == 0)
    return(false);

  return(m_map_relcast_sets[m_current_node].setCurrentProc(proc));
}






