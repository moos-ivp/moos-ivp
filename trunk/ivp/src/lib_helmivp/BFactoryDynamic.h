/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BFactoryDynamic.h                                    */
/*    DATE: Feb 11th, 2010                                       */
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

#ifndef BFACTORY_DYNAMIC_HEADER
#define BFACTORY_DYNAMIC_HEADER

#include <string>
#include <map>
#include <set>
#include <vector>

#include "IvPDomain.h"
#include "IvPBehavior.h"

class BFactoryDynamic {
 public:
  BFactoryDynamic() {}
  virtual ~BFactoryDynamic();
  
  // Configuring the domain and loading directories for search.
  bool   loadDirectory(std::string dirname);
  void   loadEnvVarDirectories(std::string envVar);
  void   setDomain(IvPDomain domain) {m_domain = domain;}

  // Building Behaviors
  bool         isKnownBehavior(std::string bhv_name) const;
  IvPBehavior* newBehavior(std::string name);
  
 private:
  // These describe the signature of the function that creates an IPF
  // object. Each shared library that implements a behavior must have one 
  // of each of these functions, and it must be named "createBehavior".
  typedef IvPBehavior* (*TFuncPtrCreateBehavior) (std::string, IvPDomain);

  std::map<std::string, TFuncPtrCreateBehavior> m_creation_funcs_map;
  
  std::vector<void*> m_open_library_handles;

  // A map from bhv_name to directory. Two behaviors of the same name
  // should not be loaded dynamically. Dangerously ambiguous.
  std::map<std::string, std::string> m_map_bhv_dir;

  std::set<std::string> m_loaded_dirs;

  IvPDomain m_domain;
};

#endif










