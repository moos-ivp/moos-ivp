/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RealmCastTree.h                                      */
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

#ifndef REALMCAST_TREE_HEADER
#define REALMCAST_TREE_HEADER

#include <string>
#include <map>
#include "RealmCast.h"
#include "RealmCastSet.h"
#include "RealmSummary.h"

class RealmCastTree
{
 public:
  RealmCastTree();
  ~RealmCastTree() {}

  bool addRealmSummary(RealmSummary, std::string, std::string);
  void setDefaultNode(std::string s)   {m_default_node=s;}
  void clearDefaultNode(std::string s) {m_default_node="";}
  
  // Return true if first time heard from this node
  bool addRealmCast(const std::string&);
  bool addRealmCast(const RealmCast&);

  bool removeNode(const std::string&);

  // Global getters (Queries requiring no key)
  unsigned int getTreeRealmCastCount() const   {return(m_total_relcast_count);}
  unsigned int getTreeNodeCount() const      {return(m_map_relcast_sets.size());}
  unsigned int getTreeProcCount() const;
  std::vector<std::string> getNodes() const  {return(m_nodes);}
  std::vector<std::string> getNodeIDs() const;

  // Node Level getters (Queries requiring a node or node_id argument)
  bool         hasNode(std::string node)                const;
  std::string  getNodeNameFromID(std::string id)        const;
  unsigned int getNodeRealmCastCount(std::string node)  const;
  unsigned int getNodeTotalProcCount(std::string node)  const;

  std::vector<std::string> getProcIDs(std::string node) const;
  std::vector<std::string> getProcs(std::string node)   const;
  std::vector<std::string> getProcs()   const;

  // Realm Level getters (Queries requiring a node + realm or realm_id argument)
  bool         hasNodeProc(std::string node, std::string proc)         const;
  std::string  getProcNameFromID(std::string node, std::string id)     const; 
  unsigned int getProcRealmCastCount(std::string node, std::string proc) const;
  
  RealmCast    getRealmCast(std::string node, std::string proc) const;

  std::string  report() const;

  bool        setCurrentNode(std::string);
  bool        setCurrentProc(std::string);
  std::string getCurrentNode() const {return(m_current_node);}
  std::string getCurrentProc() const;

  
 protected:
  void setNewNodeID(std::string node);
  
private: 
  std::string m_current_node;
  
  // Default node used only if user wants the client app to view a node upon
  // upon client app startup
  std::string m_default_node;
  
  // Mapping from ID to NodeName
  std::map<std::string, std::string>  m_map_id_node;

  // Mappings from NodeName to various data
  std::map<std::string, RealmCastSet>   m_map_relcast_sets;

  // Keeping running total of nodes for help with auto-generating IDs
  unsigned int m_node_id_count;
  unsigned int m_total_relcast_count;

  // Keep a separate vector of node names so that when node name vector is
  // retrieved by caller, the earlier items stay in the same order. The 
  // alternative, iterating through the map, means order may shift as map grows.
  std::vector<std::string> m_nodes;
};

#endif 

