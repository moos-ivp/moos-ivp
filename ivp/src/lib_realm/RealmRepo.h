/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AppCastRepo.h                                        */
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

#ifndef REALM_REPO_HEADER
#define REALM_REPO_HEADER

#include <string>
#include <map>
#include "RealmCastTree.h"
#include "RealmSummary.h"

class RealmRepo
{
 public:
  RealmRepo();
  ~RealmRepo() {}

  bool addRealmSummary(RealmSummary);

  // Return true if first time heard from this node
  bool addRealmCast(const std::string&);
  bool addRealmCast(const RealmCast&);
  bool removeNode(std::string node);

  bool setCurrentNode(std::string node);
  bool setCurrentProc(std::string proc);
  bool setRefreshMode(std::string mode);

  std::string  getCurrentNode() const  {return(m_realm_tree.getCurrentNode());}
  std::string  getCurrentProc() const  {return(m_realm_tree.getCurrentProc());}
  std::string  getRefreshMode() const  {return(m_refresh_mode);}

  std::vector<std::string> getCurrentNodes() const;
  std::vector<std::string> getCurrentProcs() const;
  
  const RealmCastTree& rctree() const {return(m_realm_tree);}

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
  
};

#endif 
