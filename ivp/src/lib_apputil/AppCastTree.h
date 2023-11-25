/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AppCastTree.h                                        */
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

#ifndef APPCAST_TREE_HEADER
#define APPCAST_TREE_HEADER

#include <string>
#include <map>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCast.h"
#include "AppCastSet.h"

class AppCastTree
{
 public:
  AppCastTree();
  ~AppCastTree() {}
  
  // Return true if first time heard from this node
  bool addAppCast(const std::string&);
  bool addAppCast(const AppCast&);

  bool removeNode(const std::string&);

  // Global getters (Queries requiring no key)
  unsigned int getTreeAppCastCount() const   {return(m_total_appcast_count);}
  unsigned int getTreeNodeCount() const      {return(m_map_appcast_sets.size());}
  unsigned int getTreeProcCount() const;
  std::vector<std::string> getNodes() const  {return(m_nodes);}
  std::vector<std::string> getNodeIDs() const;

  // Node Level getters (Queries requiring a node or node_id argument)
  bool         hasNode(std::string node)                const;
  std::string  getNodeNameFromID(std::string id)        const;
  unsigned int getNodeAppCastCount(std::string node)    const;
  unsigned int getNodeCfgWarningCount(std::string node) const;
  unsigned int getNodeRunWarningCount(std::string node) const;
  unsigned int getNodeTotalProcCount(std::string node)  const;

  std::vector<std::string> getProcIDs(std::string node) const;
  std::vector<std::string> getProcs(std::string node)   const;

  // App Level getters (Queries requiring a node + app or app_id argument)
  bool         hasNodeProc(std::string node, std::string proc)         const;
  std::string  getProcNameFromID(std::string node, std::string id)     const; 
  unsigned int getProcAppCastCount(std::string node, std::string proc) const;
  unsigned int getProcCfgWarningCount(std::string node, std::string proc) const;
  unsigned int getProcRunWarningCount(std::string node, std::string proc) const;
  
  AppCast      getAppCast(std::string node, std::string proc) const;

 private: 
  // Mapping from ID to NodeName
  std::map<std::string, std::string>  m_map_id_node;

  // Mappings from NodeName to various data
  std::map<std::string, AppCastSet>   m_map_appcast_sets;

  // Keeping running total of nodes for help with auto-generating IDs
  unsigned int m_node_id_count;
  unsigned int m_total_appcast_count;

  // Keep a separate vector of node names so that when node name vector is
  // retrieved by caller, the earlier items stay in the same order. The 
  // alternative, iterating through the map, means order may shift as map grows.
  std::vector<std::string> m_nodes;
};

#endif 









