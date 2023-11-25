/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AppCastRepo.h                                        */
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

#ifndef REALM_REPO_HEADER
#define REALM_REPO_HEADER

#include <string>
#include <map>
#include "RealmCastTree.h"
#include "RealmSummary.h"
#include "WatchCluster.h"
#include "InfoCastSettings.h"

class RealmRepo
{
 public:
  RealmRepo();
  ~RealmRepo() {}

  bool addRealmSummary(RealmSummary, bool& changed);
  bool addWatchCluster(std::string);
  
  // Return true if first time heard from this node
  bool addRealmCast(const std::string&);
  bool addRealmCast(const RealmCast&);
  bool addWatchCast(const std::string&);

  bool removeNode(std::string node);

  bool setCurrentNode(std::string node);
  bool setCurrentProc(std::string proc);
  bool setRefreshMode(std::string mode);

  bool setOnStartPreferences(std::string);
  void clearOnStartPreferences();
  bool checkStartCluster();
  
  std::string  getCurrentNode() const  {return(m_realm_tree.getCurrentNode());}
  std::string  getCurrentProc() const  {return(m_realm_tree.getCurrentProc());}
  std::string  getRefreshMode() const  {return(m_refresh_mode);}

  std::vector<std::string> getCurrentNodes() const;
  std::vector<std::string> getCurrentProcs() const;

  unsigned int getWatchCastCount(std::string cluster_key) const;
  
  std::vector<std::string> getClusterKeys() const;
  std::vector<std::string> getClusterVars(std::string key) const;
  std::vector<std::string> getClusterReport(InfoCastSettings ics);
  std::string getCurrClusterKey() const {return(m_curr_cluster_key);}
  std::string getCurrClusterVar() const {return(m_curr_cluster_var);}
  std::string getCurrClusterVars() const;

  unsigned int getWatchCount(std::string key, std::string var) const;

  unsigned int getWatchCastCount() const {return(m_watchcast_count);}
  unsigned int getRealmCastCount() const {return(m_realmcast_count);}
  
  const RealmCastTree& rctree() const {return(m_realm_tree);}

  void setForceRefreshWC(bool v=true) {m_force_refresh_watchcast=v;}
  bool getForceRefreshWC() const {return(m_force_refresh_watchcast);}

  bool getNodeProcChanged();
  
  std::string report() const;
  
  // Methods for detecting potential need for refresh by users
  unsigned int getNodeCount() const;
  unsigned int getProcCount() const;
  unsigned int getProcCount(std::string node) const;
  unsigned int getRealmCastCount(std::string node, std::string proc) const;
  unsigned int getRealmCastCount(std::string node) const;

 private: 
  RealmCastTree m_realm_tree;

  std::string m_refresh_mode; // paused,events,streaming

  bool m_node_proc_changed;

  bool m_force_refresh_watchcast;
  
  std::vector<WatchCluster> m_watch_clusters;  

  std::string m_onstart_node;
  std::string m_onstart_proc;
  
  std::string m_curr_cluster_key;
  std::string m_curr_cluster_var;

  unsigned int m_watchcast_count;
  unsigned int m_realmcast_count;
};

#endif 

