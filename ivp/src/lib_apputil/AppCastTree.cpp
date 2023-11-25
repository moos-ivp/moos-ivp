/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AppCastTree.cpp                                      */
/*    DATE: Aug 14th 2012                                        */
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

#include <iterator>
#include "MBUtils.h"
#include "AppCastTree.h"

using namespace std;

//---------------------------------------------------------
// Constructor

AppCastTree::AppCastTree()
{
  m_total_appcast_count = 0;
  m_node_id_count = 0;
}

//---------------------------------------------------------
// Procedure: addAppCast
//   Returns: true if first time hearing from this node

bool AppCastTree::addAppCast(const string& str)
{
  AppCast appcast = string2AppCast(str);
  return(addAppCast(appcast));
}

//---------------------------------------------------------
// Procedure: addAppCast
//   Returns: true. May add some error checking in the future

bool AppCastTree::addAppCast(const AppCast& appcast)
{
  m_total_appcast_count++;

  string  appcast_node_name = appcast.getNodeName();
  if(appcast_node_name == "")
    appcast_node_name = "unknown_node";

  // First determine if the node_name has been previously encountered.
  bool new_node = false;
  if(m_map_appcast_sets.count(appcast_node_name) == 0)
    new_node = true;

  // Now update the node (or create new node channelset if need be)
  m_map_appcast_sets[appcast_node_name].addAppCast(appcast);

  // If we've heard from this node before, our job is done.
  if(!new_node)
    return(true);
  else
    m_nodes.push_back(appcast_node_name);

  // If this is a newly found node_name then:
  // (1) always declare update_pending, and (2) generate new id
  if(appcast_node_name == "shoreside")
    m_map_id_node["0"] = appcast_node_name;
  else {
    char c = 97 + (int)(m_node_id_count);  // 'a' + cnt
    m_node_id_count++;
    if((c=='e') || (c=='h') || (c=='p') || (c=='r') || (c=='n')) {
      c++;
      m_node_id_count++;
    }
    
    string id(1,c);
    m_map_id_node[id] = appcast_node_name;
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: removeNode 
//   Returns: true if node is already known to the repo.

bool AppCastTree::removeNode(const string& node)
{
  if(!hasNode(node))
    return(false);
  
  // Step 1
  m_map_appcast_sets.erase(node);
  
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
  
  return(true);
}

//---------------------------------------------------------
// Procedure: hasNode(node)
//   Returns: true if node is already known to the repo.

bool AppCastTree::hasNode(string node) const
{
  return(m_map_appcast_sets.count(node) > 0);
}


//---------------------------------------------------------
// Procedure: getTreeProcCount
//   Returns: Number of known procs for all nodes

unsigned int AppCastTree::getTreeProcCount() const
{
  unsigned int total = 0;
  map<string, AppCastSet>::const_iterator p;
  for(p=m_map_appcast_sets.begin(); p!=m_map_appcast_sets.end(); p++) {
    total += p->second.getTotalProcCount();
  }

  return(total);
}

//---------------------------------------------------------
// Procedure: getNodeIds
//   Returns: list of known nodes

vector<string> AppCastTree::getNodeIDs() const
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
// Procedure: getProcs(node)
//   Returns: list of known procs for the given node

vector<string> AppCastTree::getProcs(string node) const
{
  map<string, AppCastSet>::const_iterator p = m_map_appcast_sets.find(node);
  if(p == m_map_appcast_sets.end()) {
    vector<string> empty_vector;
    return(empty_vector);
  }
  else
    return(p->second.getProcs());
}

//---------------------------------------------------------
// Procedure: getProcIDs(node)
//   Returns: list of known proc IDs for the given node

vector<string> AppCastTree::getProcIDs(string node) const
{
  map<string, AppCastSet>::const_iterator p = m_map_appcast_sets.find(node);
  if(p == m_map_appcast_sets.end()) {
    vector<string> empty_vector;
    return(empty_vector);
  }
  else
    return(p->second.getIDs());
}

 
//---------------------------------------------------------
// Procedure: getNodeNameFromID(id)
//   Returns: Name of the node given the id, or empty string if id unknown

string AppCastTree::getNodeNameFromID(string id) const
{
  map<string, string>::const_iterator p = m_map_id_node.find(id);
  if(p == m_map_id_node.end())
    return("");
  else
    return(p->second);  
}

//---------------------------------------------------------
// Procedure: getNodeAppCastCount(node)
//   Returns: Number of appcasts received for all procs for this node

unsigned int AppCastTree::getNodeAppCastCount(string node) const
{
  map<string, AppCastSet>::const_iterator p = m_map_appcast_sets.find(node);
  if(p == m_map_appcast_sets.end())
    return(0);
  else
    return(p->second.getTotalAppCastCount());  
}

//---------------------------------------------------------
// Procedure: getNodeCfgWarningCount(node)
//   Returns: Number of config warnings for all channels for this node

unsigned int AppCastTree::getNodeCfgWarningCount(string node) const
{
  map<string, AppCastSet>::const_iterator p = m_map_appcast_sets.find(node);
  if(p == m_map_appcast_sets.end())
    return(0);
  else
    return(p->second.getTotalCfgWarningCount());  
}

//---------------------------------------------------------
// Procedure: getNodeRunWarningCount(node)
//   Returns: Number of run warnings for all channels for this node

unsigned int AppCastTree::getNodeRunWarningCount(string node) const
{
  map<string, AppCastSet>::const_iterator p = m_map_appcast_sets.find(node);
  if(p == m_map_appcast_sets.end())
    return(0);
  else
    return(p->second.getTotalRunWarningCount());  
}

//---------------------------------------------------------
// Procedure: getNodeTotalProcCount(node)
//   Returns: Number of run warnings for all channels for this node

unsigned int AppCastTree::getNodeTotalProcCount(string node) const
{
  map<string, AppCastSet>::const_iterator p = m_map_appcast_sets.find(node);
  if(p == m_map_appcast_sets.end())
    return(0);
  else
    return(p->second.getTotalProcCount());  
}

//---------------------------------------------------------
// Procedure: hasNodeProc(node, proc)
//   Returns: true if node with proc is already known to the repo.

bool AppCastTree::hasNodeProc(string node, string proc) const
{
  map<string, AppCastSet>::const_iterator p = m_map_appcast_sets.find(node);
  if(p == m_map_appcast_sets.end())
    return(false);
  else
    return(p->second.hasProc(proc));  
}

//---------------------------------------------------------
// Procedure: getProcNameFromID
//   Returns: Name of the proc, given the node name and process id

string AppCastTree::getProcNameFromID(string node, string id) const
{
  map<string, AppCastSet>::const_iterator p = m_map_appcast_sets.find(node);
  if(p == m_map_appcast_sets.end())
    return("");
  else
    return(p->second.getProcNameFromID(id));
}

//---------------------------------------------------------
// Procedure: getProcAppCastCount
//   Returns: Number of appcasts received by the given proc and node.

unsigned int AppCastTree::getProcAppCastCount(string node, string proc) const
{
  map<string, AppCastSet>::const_iterator p = m_map_appcast_sets.find(node);
  if(p == m_map_appcast_sets.end())
    return(0);
  else
    return(p->second.getAppCastCount(proc));
}


//---------------------------------------------------------
// Procedure: getProcCfgWarningCount
//   Returns: Number of config warnings for the latest appcast received 
//            for the given proc and node.

unsigned int AppCastTree::getProcCfgWarningCount(string node, string proc) const
{
  map<string, AppCastSet>::const_iterator p = m_map_appcast_sets.find(node);
  if(p == m_map_appcast_sets.end())
    return(0);
  else
    return(p->second.getCfgWarningCount(proc));
}

//---------------------------------------------------------
// Procedure: getProcRunWarningCount
//   Returns: Number of run warnings for the latest appcast received 
//            for the given proc and node.

unsigned int AppCastTree::getProcRunWarningCount(string node, string proc) const
{
  map<string, AppCastSet>::const_iterator p = m_map_appcast_sets.find(node);
  if(p == m_map_appcast_sets.end())
    return(0);
  else
    return(p->second.getRunWarningCount(proc));
}

//---------------------------------------------------------
// Procedure: getAppCast

AppCast AppCastTree::getAppCast(string node, string proc) const
{
  map<string, AppCastSet>::const_iterator p = m_map_appcast_sets.find(node);
  if(p == m_map_appcast_sets.end()) {
    AppCast null_appcast;
    return(null_appcast);
  }
  else
    return(p->second.getAppCast(proc));
}










