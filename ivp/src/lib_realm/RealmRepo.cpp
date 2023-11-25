/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RealmRepo.cpp                                        */
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

#include <algorithm>
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

  m_node_proc_changed = false;
  m_force_refresh_watchcast = false;
  
  m_watchcast_count = 0;
  m_realmcast_count = 0;
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

bool RealmRepo::addRealmSummary(RealmSummary summary, bool& changed)
{
  unsigned int prev_node_count = getNodeCount();
  unsigned int prev_proc_count = getProcCount();
  string prev_curr_node = getCurrentNode();
  string prev_curr_proc = getCurrentProc();

  bool ok = m_realm_tree.addRealmSummary(summary, m_onstart_node,
					 m_onstart_proc);

  unsigned new_node_count = getNodeCount();
  unsigned new_proc_count = getProcCount();
  
  string new_curr_node = getCurrentNode();
  string new_curr_proc = getCurrentProc();


  if((new_curr_node != prev_curr_node) ||
     (new_curr_proc != prev_curr_proc))
    changed = true;
  else if((new_node_count != prev_node_count) ||
	  (new_proc_count != prev_proc_count))
    changed = true;
  else
    changed = false;

  return(ok);
}

//---------------------------------------------------------
// Procedure: addWatchCluster()

bool RealmRepo::addWatchCluster(string str)
{
  string cluster_key = tokStringParse(str, "key", ',', '=');

  bool cluster_key_exists = false;
  unsigned int cluster_ix = 0;
  for(unsigned int i=0; i<m_watch_clusters.size(); i++) {
    if(cluster_key == m_watch_clusters[i].getKey()) {
      cluster_key_exists = true;
      cluster_ix = i;
    }
  }

  if(!cluster_key_exists) {
    WatchCluster new_wcluster;
    new_wcluster.setKey(cluster_key);
    m_watch_clusters.push_back(new_wcluster);
    cluster_ix = m_watch_clusters.size() - 1;
  }
  
  string vars = tokStringParse(str, "vars", ',', '=');  
  vector<string> jvector = parseString(vars, ':');
  for(unsigned int j=0; j<jvector.size(); j++) {
    bool ok = m_watch_clusters[cluster_ix].addVar(jvector[j]);
    if(!ok)
      return(false);
  }
  
  return(true);
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
  m_realmcast_count++;
  return(m_realm_tree.addRealmCast(relcast));
}


//---------------------------------------------------------
// Procedure: addWatchCast()

bool RealmRepo::addWatchCast(const string& str)
{
  WatchCast new_wcast = string2WatchCast(str);
  if(!new_wcast.valid())
    return(false);

  m_watchcast_count++;
  bool accepted_by_atleast_one = false;
  for(unsigned int i=0; i<m_watch_clusters.size(); i++) {
    bool accepted = m_watch_clusters[i].addWatchCast(new_wcast);
    if(accepted)
      accepted_by_atleast_one = true;
  }

  return(accepted_by_atleast_one);
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
  clearOnStartPreferences();
  if(m_realm_tree.getCurrentNode() != node_str)
    m_node_proc_changed = true;

  vector<string> cluster_keys = getClusterKeys();
  if(vectorContains(cluster_keys, node_str)) {
    m_curr_cluster_key = node_str;
    m_curr_cluster_var = "";
    return(true);
  }
  else
    m_curr_cluster_key = "";

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
// Procedure: setOnStartPreferences()
//   Purpose: All the user to configure a starting channel
//  Examples: "abe"       - default proc for vehicle abe
//            "abe:iSay"  - The iSay app on vehicle abe
//            "cluster1"  - The collective table for cluster 1
//            "cluster1:DEPLOY" - The var DEPLOY in cluster 1

bool RealmRepo::setOnStartPreferences(string str)
{
  string primary   = biteStringX(str, ':');
  string secondary = str;

  m_onstart_node = primary;
  m_onstart_proc = secondary;

  return(true);
}

//---------------------------------------------------------
// Procedure: checkStartCluster()
//   Purpose: Once watch_clusters and cluster_vars have been built
//            during a startup phase, then check if there was also an
//            onstart_node or _proc preference that matches one of the
//            watch_clusters. If so, then go ahead and set the
//            curr_cluster_key and _var to these onstart preferences.
//            This function is called at the end of OnStartUp methods
//            once all the config params have been met, since we don't
//            know the order of the config params.


bool RealmRepo::checkStartCluster()
{
  // First check to see if primary names a cluster
  for(unsigned int i=0; i<m_watch_clusters.size(); i++) {
    if(m_watch_clusters[i].getKey() == m_onstart_node) {
      m_curr_cluster_key = m_onstart_node;
      vector<string> vars = m_watch_clusters[i].getVars();
      if(vectorContains(vars, m_onstart_proc))
	m_curr_cluster_var = m_onstart_proc;
      else
	m_curr_cluster_var = "";

      return(true);
    }
  }
  return(false);
}

//---------------------------------------------------------
// Procedure: clearOnStartPreferences()

void RealmRepo::clearOnStartPreferences()
{
  m_onstart_node = "";
  m_onstart_proc = "";
}

//---------------------------------------------------------
// Procedure: setCurrentProc()
//   Returns: true if given str names an proc known to this node, or if it
//            names an ID mapped to a proc known to this node, 
//      Note: The given key may be an Proc name, or an ID mapped to an Proc

bool RealmRepo::setCurrentProc(string proc_str)
{
  clearOnStartPreferences();
  if(m_curr_cluster_key != "") {
    m_curr_cluster_var = proc_str;
    return(true);
  }
  
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
  if(rvector.size() == 0)
    return(rvector);
  
  std::sort(rvector.begin(), rvector.end());

  if(rvector.back() == "shoreside") {
    vector<string> rvector2;
    rvector2.push_back("shoreside");
    for(unsigned int i=0; i<rvector.size()-1; i++)
      rvector2.push_back(rvector[i]);
    rvector = rvector2;
  }

  return(rvector);
}

//---------------------------------------------------------
// Procedure: getCurrentProcs()

vector<string> RealmRepo::getCurrentProcs() const
{
  if(m_curr_cluster_key != "") {
    for(unsigned int i=0; i<m_watch_clusters.size(); i++) {
      if(m_watch_clusters[i].getKey() == m_curr_cluster_key) {
	vector<string> cluster_vars = m_watch_clusters[i].getVars();
	return(cluster_vars);
      }
    }
  }
  
  return(m_realm_tree.getProcs());
}

//---------------------------------------------------------
// Procedure: getClusterKeys()

vector<string> RealmRepo::getClusterKeys() const
{
  vector<string> watch_cluster_keys;

  for(unsigned i=0; i<m_watch_clusters.size(); i++) {
    string key = m_watch_clusters[i].getKey();
    watch_cluster_keys.push_back(key);
  }

  return(watch_cluster_keys);
}

//---------------------------------------------------------
// Procedure: getWatchCastCount()

unsigned int RealmRepo::getWatchCastCount(string key) const
{
  for(unsigned i=0; i<m_watch_clusters.size(); i++) {
    if(m_watch_clusters[i].getKey() == key) {
      return(m_watch_clusters[i].getWCastCount());
    }
  }
  return(0);
}

//---------------------------------------------------------
// Procedure: getClusterVars()

vector<string> RealmRepo::getClusterVars(string key) const
{
  for(unsigned i=0; i<m_watch_clusters.size(); i++) {
    if(m_watch_clusters[i].getKey() == key) {
      return(m_watch_clusters[i].getVars());
    }
  }

  vector<string> no_vars;  
  return(no_vars);
}

//---------------------------------------------------------
// Procedure: getClusterReport()

vector<string> RealmRepo::getClusterReport(InfoCastSettings ics)
{
  vector<string> report;
  
  for(unsigned i=0; i<m_watch_clusters.size(); i++) {
    if(m_watch_clusters[i].getKey() == m_curr_cluster_key) {
      return(m_watch_clusters[i].getReport(m_curr_cluster_var, ics));
    }
  }
  
  return(report);
}

//---------------------------------------------------------
// Procedure: getWatchCount()

unsigned int RealmRepo::getWatchCount(string key, string var) const
{
  for(unsigned i=0; i<m_watch_clusters.size(); i++) {
    if(m_watch_clusters[i].getKey() == key) {
      return(m_watch_clusters[i].getVarCount(var));
    }
  }

  return(0);
}

//---------------------------------------------------------
// Procedure: getCurrClusterVars()

string RealmRepo::getCurrClusterVars() const
{
  for(unsigned i=0; i<m_watch_clusters.size(); i++) {
    if(m_watch_clusters[i].getKey() == m_curr_cluster_key) {
      return(m_watch_clusters[i].getVarsAsString());
    }
  }
  
  return("");
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

