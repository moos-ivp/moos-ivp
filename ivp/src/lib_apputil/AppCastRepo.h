/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AppCastRepo.h                                        */
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

#ifndef APPCAST_REPO_HEADER
#define APPCAST_REPO_HEADER

#include <string>
#include <map>
#include "AppCastTree.h"

class AppCastRepo
{
 public:
  AppCastRepo();
  ~AppCastRepo() {}
  
  // Return true if first time heard from this node
  bool addAppCast(const std::string&);
  bool addAppCast(const AppCast&);
  bool removeNode(const std::string& node);

  bool setCurrentNode(const std::string& node);
  bool setCurrentProc(const std::string& proc);
  bool setRefreshMode(const std::string& mode);

  std::string  getCurrentNode() const  {return(m_current_node);}
  std::string  getCurrentProc() const  {return(m_current_proc);}
  std::string  getRefreshMode() const  {return(m_refresh_mode);}

  std::vector<std::string> getCurrentNodes() const;
  std::vector<std::string> getCurrentProcs() const;
  
  const AppCastTree& actree() const    {return(m_appcast_tree);}
  
  // Methods for detecting potential need for refresh by users
  unsigned int getNodeCount() const;
  unsigned int getProcCount() const;
  unsigned int getProcCount(std::string node) const;
  unsigned int getAppCastCount(std::string node, std::string proc) const;
  unsigned int getAppCastCount(std::string node) const;

 private: 
  AppCastTree  m_appcast_tree;
  std::string  m_current_node;
  std::string  m_current_proc;

  std::string  m_refresh_mode; // paused,events,streaming

  std::map<std::string, std::string> m_map_node_proc;
};

#endif 








